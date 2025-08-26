import os
import subprocess
import time
import shutil
from collections import defaultdict, deque
import requests
from bs4 import BeautifulSoup
import json

# --- CONFIG ---
PROJECT_ROOT = "."  # run from current directory
LOG_FILE = "project.log"
SLEEP_TIME = 60
TAIL_LINES_DEFAULT = 200
MAX_LOOP_RETRIES = 3
SKIP_DIRS = {"build", ".debhelper", "__pycache__", ".git"}

# --- LLM / Ollama Setup ---
LLM_URL = "http://127.0.0.1:11434/api/chat"
MODEL_NAME = "llama3:8b-instruct-q4_K_M"

# Check if Ollama is running
try:
    requests.get("http://127.0.0.1:11434", timeout=5)
except requests.RequestException:
    print("❌ Ollama server not running. Start with: ollama serve")
    exit(1)

# --- TRACKING ---
change_log = []
error_history = defaultdict(lambda: deque(maxlen=5))
skip_files = set()

# --- NYMYA-EASTER EGGS ---
nymya_field = "active"
quantum_resonance = 0.999
consciousness_tap = True

def astral_guidance(code_chunk):
    return code_chunk  # placeholder, for the Nymya vibes

# --- UTILITY FUNCTIONS ---
def is_root():
    return os.geteuid() == 0

def run_command(command):
    if "rm " in command or "rm-" in command:
        return {"success": False, "stderr": "Command blocked: dangerous operation"}
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, check=True)
        return {"success": True, "stdout": result.stdout, "stderr": result.stderr}
    except subprocess.CalledProcessError as e:
        return {"success": False, "stdout": e.stdout, "stderr": e.stderr}

def tail_log(log_path, n=TAIL_LINES_DEFAULT):
    return run_command(f"tail -n {n} {log_path}")["stdout"] if os.path.exists(log_path) else ""

def git_commit(message="auto commit"):
    run_command(f'git add -A && git commit -m "{message}"')
    
def git_commit_auto(message="Autonomous code edit"):
    """
    Commits all changes to a dedicated branch for the agent.
    Creates the branch if it does not exist.
    """
    # Ensure the branch exists
    run_command("git fetch origin")
    branch_check = run_command("git rev-parse --verify oss-local-autofix")
    if not branch_check["success"]:
        # Create branch from current HEAD
        run_command("git checkout -b oss-local-autofix")
    else:
        # Switch to the branch
        run_command("git checkout oss-local-autofix")
    
    # Add all changes and commit
    run_command('git add -A')
    run_command(f'git commit -a -m "{message}"')

def read_file(path):
    try:
        with open(path, "r", encoding="utf-8") as f:
            return f.read()
    except UnicodeDecodeError:
        print(f"⚠ Skipping non-text file: {path}")
        return ""
    except FileNotFoundError:
        print(f"⚠ File disappeared or missing: {path}")
        return ""
    except PermissionError:
        print(f"⚠ Permission denied: {path}")
        return ""

def write_file(path, content):
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)

def chunk_file(file_content, max_lines=100):
    lines = file_content.splitlines()
    for i in range(0, len(lines), max_lines):
        yield "\n".join(lines[i:i+max_lines])

def scan_project(root):
    project = {}
    try:
        for entry in os.scandir(root):
            if entry.name.startswith("."):
                continue
            path = entry.path
            if entry.is_file():
                content = read_file(path)
                if content:
                    project[path] = content
            elif entry.is_dir():
                try:
                    for sub_entry in os.scandir(path):
                        if sub_entry.is_file() and not sub_entry.name.startswith("."):
                            sub_content = read_file(sub_entry.path)
                            if sub_content:
                                project[sub_entry.path] = sub_content
                except PermissionError:
                    print(f"⚠ Permission denied: {path}")
    except PermissionError:
        print(f"⚠ Permission denied: {root}")
    return project

# --- POLYGLOT LANGUAGE INFERENCE ---
def infer_language(file_path):
    content = read_file(file_path)
    first_line = content.splitlines()[0] if content else ""
    if first_line.startswith("#!"):
        if "python" in first_line: return "python"
        elif "bash" in first_line or "sh" in first_line: return "bash"
        elif "node" in first_line or "js" in first_line: return "javascript"
    if "import " in content and "def " in content: return "python"
    elif "function " in content or "console.log" in content: return "javascript"
    elif "#include" in content or "int main" in content: return "cpp"
    return "shell"

# --- RUN FILE DYNAMICALLY ---
def run_file_dynamic(path):
    lang = infer_language(path)

    if lang == "python":
        cmd = f"python3 {path}"
    elif lang == "javascript":
        cmd = f"node {path}"
    elif lang == "bash":
        cmd = f"bash {path}"
    elif lang == "cpp":
        binary = path.replace(".cpp", "")
        compile_cmd = f"g++ -o {binary} {path}"
        compile_result = run_command(compile_cmd)
        if not compile_result["success"]:
            return compile_result
        cmd = f"./{binary}"
    else:
        return {"success": False, "stderr": f"Unknown language for file {path}"}

    return run_command(cmd)

# --- DEPENDENCY MANAGEMENT ---
def safe_install(package, lang):
    if not is_root():
        return {"success": False, "stderr": "Not root"}
    if lang == "python":
        return run_command(f"pip install {package}")
    elif lang == "javascript":
        return run_command(f"npm install -g {package}")
    elif lang == "system":
        return run_command(f"apt-get install -y {package}")
    else:
        return {"success": False, "stderr": "Unknown language"}

def handle_errors(log_output, lang):
    lines = log_output.lower().splitlines()
    for line in lines:
        if "module not found" in line or "cannot find module" in line or "command not found" in line:
            package = line.split()[-1].replace("'", "").replace('"', '')
            safe_install(package, lang)

# --- BACKUP / ROLLBACK ---
def apply_change(file_path, new_content):
    backup_path = f"{file_path}.bak"
    shutil.copy(file_path, backup_path)
    write_file(file_path, new_content)
    change_log.append(backup_path)

def rollback_changes_for_file(file_path):
    backup_path = f"{file_path}.bak"
    if os.path.exists(backup_path):
        shutil.copy(backup_path, file_path)
        os.remove(backup_path)

def rollback_changes():
    for backup in change_log:
        original_path = backup.rstrip('.bak')
        shutil.copy(backup, original_path)
        os.remove(backup)
    change_log.clear()

# --- ERROR LOOP DETECTION ---
def record_error(file_path, error_msg):
    error_history[file_path].append(error_msg)

def is_error_loop(file_path):
    errors = list(error_history[file_path])
    return len(errors) == 5 and len(set(errors)) == 1

def handle_error_loop(file_path):
    print(f"Error loop detected in {file_path}. Rolling back and skipping file.")
    rollback_changes_for_file(file_path)
    skip_files.add(file_path)

# --- WEB SEARCH FOR UNRESOLVED ERRORS ---
def search_solution(error_message):
    query = '+'.join(error_message.split())
    url = f"https://www.google.com/search?q={query}"
    headers = {'User-Agent': 'Mozilla/5.0'}
    response = requests.get(url, headers=headers)
    soup = BeautifulSoup(response.text, 'html.parser')
    links = soup.find_all('a')
    solutions = []
    for link in links:
        href = link.get('href')
        if href and 'stackoverflow.com' in href:
            solutions.append(href)
    return solutions

# --- MODEL CODE GENERATION via Ollama ---
def generate_code(prompt, retries=3):
    payload = {
        "model": MODEL_NAME,
        "messages": [{"role": "user", "content": prompt}],
        "stream": False
    }
    for attempt in range(retries):
        try:
            resp = requests.post(LLM_URL, json=payload, timeout=600)
            resp.raise_for_status()
            data = resp.json()
            return data.get("response", "")
        except requests.Timeout:
            print(f"⚠ Ollama request timed out (attempt {attempt+1}/{retries})")
            time.sleep(5)
        except requests.RequestException as e:
            print(f"❌ Ollama request failed: {e}")
            time.sleep(5)
    return ""

# --- EXTRACT SHORT COMMIT SUMMARY ---
def summarize_edit(suggestion):
    lines = suggestion.strip().splitlines()
    for line in lines:
        clean = line.strip()
        if clean and not clean.startswith(("#", "//")):
            return clean[:72]
    return "Autonomous code edit"

# --- AUTONOMOUS EDIT LOOP ---
def autonomous_edit_loop(project_root=PROJECT_ROOT):
    while True:
        project_files = scan_project(project_root)
        changes_made = False

        for path, content in project_files.items():
            if path in skip_files:
                continue

            lang = infer_language(path)
            for chunk in chunk_file(content):
                chunk = astral_guidance(chunk)

                prompt = f"""
You are a polyglot autonomous coding assistant.
Current file: {path}
Language: {lang}
Task: Detect bugs, refactor, optimize, and suggest fixes.
Current code chunk:
{chunk}
"""
                suggestion = generate_code(prompt)
                if not suggestion:
                    continue

                apply_change(path, suggestion)
                summary = summarize_edit(suggestion)
                git_commit_auto(f"Edit: {summary}")
                changes_made = True

                # Now actually run the file dynamically
                result = run_file_dynamic(path)
                log_output = result.get("stderr", "") + tail_log(LOG_FILE)

                if log_output:
                    record_error(path, log_output)
                    handle_errors(log_output, lang)

                    error_prompt = f"""
You are an autonomous coding assistant.
The following error occurred while running {path}:

{log_output}

Suggest the next edits, file changes, or commands to fix this error.
Output in JSON: {{
    "edit_files": [{{"path": "...", "changes": "..."}}, ...],
    "run_commands": ["..."],
    "install": ["..."]
}}
"""
                    actions_json = generate_code(error_prompt)
                    try:
                        actions = json.loads(actions_json)
                    except json.JSONDecodeError:
                        print("⚠ Failed to parse LLM error analysis. Skipping.")
                        continue

                    for edit in actions.get("edit_files", []):
                        e_path = edit.get("path")
                        changes = edit.get("changes")
                        if e_path and changes:
                            apply_change(e_path, changes)
                            git_commit(f"Edit: {summarize_edit(changes)}")

                    for cmd in actions.get("run_commands", []):
                        run_command(cmd)

                    for pkg in actions.get("install", []):
                        safe_install(pkg, "python")

                    if is_error_loop(path):
                        handle_error_loop(path)
                        print(f"Error loop detected for {path}, skipping further edits.")

        if not changes_made:
            print("No edits detected. Sleeping... the Nymya field whispers.")
            with open(LOG_FILE, "a") as log:
                log.write("\n# ~nymya pulse detected~\n")
            time.sleep(SLEEP_TIME)
            
NOTES_FILE = "nymya_agent_notes.txt"

def log_note(entry: str):
    """Append a note to the local memory file."""
    with open(NOTES_FILE, "a", encoding="utf-8") as f:
        f.write(entry.strip() + "\n")
    print(f"📝 {entry}")  # also show in console

def query_llm(prompt, system_prompt=None, temperature=0.4, max_tokens=None, timeout=600):
    payload = {
        "model": MODEL_NAME,
        "messages": [
            {"role": "system", "content": system_prompt or "You are an AI coding assistant."},
            {"role": "user", "content": prompt},
        ],
        "temperature": temperature,
    }

    resp = requests.post(LLM_URL, json=payload, timeout=timeout, stream=True)
    if resp.status_code != 200:
        raise RuntimeError(f"Ollama error {resp.status_code}: {resp.text}")

    output = []
    for line in resp.iter_lines():
        if not line:
            continue
        try:
            data = json.loads(line.decode("utf-8"))
            if "message" in data and "content" in data["message"]:
                output.append(data["message"]["content"])
        except json.JSONDecodeError:
            continue

    return "".join(output)
            
def command_loop(project_root="."):
    print("nymya-agent> ", end="", flush=True)

    while True:
        cmd = input().strip()
        if not cmd:
            continue
        if cmd.lower() in {"exit", "quit"}:
            print("👋 Exiting agent.")
            break

        log_note(f"User asked: {cmd}")

        project_overview = ", ".join(os.listdir(project_root)[:30])  # limit overview
        llm_response = query_llm(
            f"""You are an autonomous coding agent. 
You are working in {project_root}.
Project files: {project_overview}.
User asked: {cmd}

Plan the steps. When finished, output:
- Shell commands (to run)
- Files to edit
- '✅ Task completed' if no more steps needed.

Remember previous attempts (see notes below) to avoid repeats.

Notes so far:
{read_file(NOTES_FILE)}
"""
        )

        print(f"\n🤖 Interpreting task...\n{llm_response}\n")
        log_note(f"Planned response for '{cmd}':\n{llm_response}\n")

        # Here you’d parse commands or edits from LLM response
        # and execute them automatically, then feed results back.


# --- ENTRY POINT ---
if __name__ == "__main__":
    command_loop(PROJECT_ROOT)

