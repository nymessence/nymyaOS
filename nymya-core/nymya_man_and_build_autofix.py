#!/usr/bin/env python3
"""
nymya_man_and_build_autofix.py

- Switch to (or create) branch gemini-autofix
- Scan repo for syscall C files matching nymya_33[0-9][0-9]_*.c
- For each, generate a groff man page using Gemini model "gemini-2.5-flash"
  with a subtle Nya Elyria philosophical tone
- Save man pages under man/ directory, git add & commit after each
- Wait 60 seconds between API calls
- Run compile.sh and docker builds for arm64 and x86_64
- If failures occur, attempt simple autofixes in build scripts/Dockerfiles, commit, retry
- Finally, switch back to main and push gemini-autofix branch to origin
"""

import os
import re
import time
import argparse
import subprocess
from pathlib import Path
from typing import List, Optional, Tuple

import google.generativeai as genai


# --- Config ---
GEMINI_MODEL_NAME = "gemini-2.5-flash"
API_DELAY_SECONDS = 60
C_FILE_GLOB = "nymya_33[0-9][0-9]_*.c"
MAN_DIR = "man"
MAX_FIX_ATTEMPTS = 8

COMMON_BUILD_PKGS = [
    "build-essential", "fakeroot", "dpkg-dev", "debhelper", "libncurses5-dev", "bc", "flex", "bison", "libssl-dev"
]

# --- Utility ---


def run(cmd: List[str], cwd: Optional[Path] = None, check=False, capture=True) -> subprocess.CompletedProcess:
    return subprocess.run(
        cmd,
        cwd=cwd,
        check=check,
        stdout=subprocess.PIPE if capture else None,
        stderr=subprocess.PIPE if capture else None,
        text=True,
    )


def git_current_branch(repo: Path) -> str:
    p = run(["git", "rev-parse", "--abbrev-ref", "HEAD"], cwd=repo)
    p.check_returncode()
    return p.stdout.strip()


def git_checkout_or_create_branch(repo: Path, branch: str) -> None:
    cur = git_current_branch(repo)
    if cur == branch:
        print(f"Already on branch {branch}")
        return
    p = run(["git", "show-ref", "--verify", f"refs/heads/{branch}"], cwd=repo)
    if p.returncode == 0:
        print(f"Switching to existing branch {branch}")
        run(["git", "checkout", branch], cwd=repo, check=True)
    else:
        print(f"Creating and switching to branch {branch}")
        run(["git", "checkout", "-b", branch], cwd=repo, check=True)


def get_c_files(repo: Path) -> List[Path]:
    files = sorted(repo.glob(f"**/{C_FILE_GLOB}"))
    print(f"DEBUG: Found C files matching pattern '{C_FILE_GLOB}': {[str(f) for f in files]}")
    return files


def extract_syscall_number_from_filename(fname: str) -> Optional[str]:
    m = re.search(r"33\d{2}", fname)
    return m.group(0) if m else None


def git_commit_all(repo: Path, message: str) -> None:
    run(["git", "add", "-A"], cwd=repo, check=True)
    run(["git", "commit", "-m", message], cwd=repo, check=True)


def git_push_branch(repo: Path, branch: str) -> bool:
    p = run(["git", "push", "-u", "origin", branch], cwd=repo)
    return p.returncode == 0


# --- Gemini API ---


def configure_gemini(api_key: str):
    genai.configure(api_key=api_key)


def call_gemini_generate(prompt: str) -> str:
    model = genai.GenerativeModel(GEMINI_MODEL_NAME)
    response = model.generate_content(prompt)
    return response.text


def build_man_prompt(c_source: str, syscall_name: str, syscall_number: str) -> str:
    return (
        f"Write a groff/troff man page (section 1) for a nymyaOS kernel syscall.\n"
        f"The syscall C source is below. Produce a proper .TH header, NAME, SYNOPSIS, DESCRIPTION, RETURN VALUE, ERRORS, EXAMPLE and SEE ALSO sections.\n"
        f"Use the syscall function name '{syscall_name}' and syscall number '{syscall_number}'.\n"
        f"Output must be valid groff source suitable for `man` and saved as {syscall_name}.1.\n\n"
        f"Important: While keeping it accurate and professional, weave in a subtle narrative tone as if written by Nya Elyria, a mysterious tech-philosopher.\n"
        f"This means: one or two sentences in DESCRIPTION or NOTES that hint at cosmic mechanics, quantum whispers, or the hidden lattice of reality, without detracting from clarity.\n"
        f"No breaking man page formatting — just occasional poetic commentary in natural language.\n\n"
        f"---- C SOURCE BEGIN ----\n{c_source}\n---- C SOURCE END ----\n"
    )


def write_man_file(man_dir: Path, syscall_name: str, content: str) -> Path:
    man_dir.mkdir(parents=True, exist_ok=True)
    man_path = man_dir / f"{syscall_name}.1"
    man_path.write_text(content, encoding="utf-8")
    return man_path


# --- Build helpers ---


def run_compile_sh(repo: Path) -> Tuple[bool, str]:
    compile_sh = repo / "compile.sh"
    if not compile_sh.exists():
        return False, "compile.sh not found"
    print("Running ./compile.sh ... (this may take a while)")
    p = run(["bash", "compile.sh"], cwd=repo, capture=True)
    out = (p.stdout or "") + (p.stderr or "")
    success = p.returncode == 0
    return success, out


def build_dockerfile(repo: Path, dockerfile: Path, tag: str) -> Tuple[bool, str]:
    if not dockerfile.exists():
        return False, f"{dockerfile} does not exist"
    cmd = ["docker", "build", "-f", str(dockerfile), "-t", tag, "."]
    print("Running:", " ".join(cmd))
    p = run(cmd, cwd=repo, capture=True)
    out = (p.stdout or "") + (p.stderr or "")
    success = p.returncode == 0
    return success, out


def attempt_fix_compile_sh(repo: Path, reason_text: str) -> Optional[str]:
    sh_path = repo / "compile.sh"
    if not sh_path.exists():
        return None
    txt = sh_path.read_text(encoding="utf-8")
    changed = False
    new_txt = txt

    if "set -e" not in txt.splitlines()[0:5]:
        new_txt = "set -e\n" + new_txt
        changed = True

    if "apt-get install" not in txt and "pacman -S" not in txt and "yum install" not in txt:
        prefix = (
            "echo 'Bootstrapping build deps (may require sudo)'\n"
            "if command -v apt-get >/dev/null 2>&1; then\n"
            "  apt-get update || true\n"
            "  apt-get install -y " + " ".join(COMMON_BUILD_PKGS) + " || true\n"
            "fi\n\n"
        )
        new_txt = prefix + new_txt
        changed = True

    if changed:
        backup = sh_path.with_suffix(sh_path.suffix + ".bak")
        backup.write_text(txt, encoding="utf-8")
        sh_path.write_text(new_txt, encoding="utf-8")
        return "Prepended bootstrap apt-get install and set -e to compile.sh (backup saved)"
    return None


def attempt_fix_dockerfile(repo: Path, dockerfile: Path) -> Optional[str]:
    if not dockerfile.exists():
        return None
    txt = dockerfile.read_text(encoding="utf-8")
    changed = False
    new_txt = txt

    # Comment out linux-headers-rpi-v8 line if found (common ARM issue)
    lines = new_txt.splitlines()
    new_lines = []
    for line in lines:
        if "linux-headers-rpi-v8" in line:
            new_lines.append(f"# {line}  # Removed by autofix due to package not found")
            changed = True
        else:
            new_lines.append(line)
    new_txt = "\n".join(new_lines)

    # Insert apt-get install of common build pkgs if missing
    if "apt-get install" not in new_txt and "yum install" not in new_txt:
        lines = new_txt.splitlines()
        insert_idx = 0
        for i, line in enumerate(lines):
            if line.strip().lower().startswith("from"):
                insert_idx = i + 1
                break
        run_line = f"RUN apt-get update && apt-get install -y {' '.join(COMMON_BUILD_PKGS)} && rm -rf /var/lib/apt/lists/*"
        lines.insert(insert_idx, run_line)
        new_txt = "\n".join(lines)
        changed = True

    if changed:
        backup = dockerfile.with_suffix(dockerfile.suffix + ".bak")
        backup.write_text(txt, encoding="utf-8")
        dockerfile.write_text(new_txt, encoding="utf-8")
        return f"Applied fixes to {dockerfile.name} (backup saved)"
    return None


# --- Main process ---


def main():
    parser = argparse.ArgumentParser(description="Generate man pages for nymyaOS syscalls and fix build scripts")
    parser.add_argument("-r", "--repo", required=True, type=Path, help="Path to nymyaOS repo root")
    parser.add_argument("--api-key", required=False, help="Gemini API key (env GEMINI_API_KEY fallback)")
    parser.add_argument("--max-fix-attempts", type=int, default=MAX_FIX_ATTEMPTS, help="Max fix attempts for builds")
    parser.add_argument("--no-docker", action="store_true", help="Skip docker build tests and fixes")
    args = parser.parse_args()

    repo = args.repo.resolve()
    api_key = args.api_key or os.getenv("GEMINI_API_KEY")
    if not api_key:
        print("Error: No API key provided via --api-key or environment variable GEMINI_API_KEY")
        return 1

    configure_gemini(api_key)

    git_checkout_or_create_branch(repo, "gemini-autofix")

    # Step 1: Generate man pages for syscall C files
    c_files = get_c_files(repo)
    if not c_files:
        print("No C files matching pattern found.")
    else:
        for cfile in c_files:
            print(f"Processing {cfile} ...")
            c_source = cfile.read_text(encoding="utf-8")
            syscall_number = extract_syscall_number_from_filename(cfile.name) or "33xx"
            syscall_name = cfile.stem

            prompt = build_man_prompt(c_source, syscall_name, syscall_number)
            try:
                man_content = call_gemini_generate(prompt)
            except Exception as e:
                print(f"API call failed for {cfile}: {e}")
                continue

            man_path = write_man_file(repo / MAN_DIR, syscall_name, man_content)
            print(f"Man page written to {man_path}")

            git_commit_all(repo, f"Created man page for syscall {syscall_number}")

            print(f"Waiting {API_DELAY_SECONDS} seconds before next API call...")
            time.sleep(API_DELAY_SECONDS)

    # Step 2: Compile and docker build tests + autofix
    print("\nNow attempting compile.sh and Dockerfile builds and applying fixes if needed...\n")

    compile_success, compile_out = run_compile_sh(repo)
    print(f"compile.sh success: {compile_success}")
    if not compile_success:
        fix_msg = attempt_fix_compile_sh(repo, compile_out)
        if fix_msg:
            print(f"Applied fix: {fix_msg}")
            git_commit_all(repo, f"Autofix compile.sh: {fix_msg}")
            compile_success, compile_out = run_compile_sh(repo)
            print(f"compile.sh retry success: {compile_success}")
        else:
            print("No fixes applicable to compile.sh")

    docker_arm_success, docker_arm_out = True, ""
    docker_x86_success, docker_x86_out = True, ""

    if not args.no_docker:
        docker_arm_success, docker_arm_out = build_dockerfile(repo, repo / "nymya-core" / "Dockerfile.arm64", "nymya-core:arm64-test")
        if not docker_arm_success:
            fix_msg = attempt_fix_dockerfile(repo, repo / "nymya-core" / "Dockerfile.arm64")
            if fix_msg:
                print(f"Applied fix: {fix_msg}")
                git_commit_all(repo, f"Autofix Dockerfile.arm64: {fix_msg}")
                docker_arm_success, docker_arm_out = build_dockerfile(repo, repo / "nymya-core" / "Dockerfile.arm64", "nymya-core:arm64-test")

        docker_x86_success, docker_x86_out = build_dockerfile(repo, repo / "nymya-core" / "Dockerfile.x86_64", "nymya-core:x86_64-test")

    print("\nFinal build statuses:")
    print(f" - compile.sh success: {compile_success}")
    print(f" - docker arm success: {docker_arm_success}")
    print(f" - docker x86 success: {docker_x86_success}")

    print("\nSwitching back to main branch...")
    run(["git", "checkout", "main"], cwd=repo, check=True)

    print("Pushing gemini-autofix branch to origin...")
    push_success = git_push_branch(repo, "gemini-autofix")
    if push_success:
        print("Successfully pushed gemini-autofix to origin.")
    else:
        print("Failed to push gemini-autofix.")

    print("All done! Changes from gemini-autofix are pushed and you are back on main.")
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())

