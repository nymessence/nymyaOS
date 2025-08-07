#!/usr/bin/env python3

import os
import subprocess
import time
import json
import sys
import difflib
import argparse

# --- Configuration ---
GEMINI_API_KEY_PATH = os.path.expanduser("~/.gemini_token")
MODEL_NAME = "gemini-2.5-flash"
MODEL_URL_BASE = "https://generativelanguage.googleapis.com/v1beta/models/"
BUILD_COMMAND = ["./compile.sh"]
BRANCH_NAME = "gemini-autofix"
API_CALL_DELAY = 60 # 1 minute delay between API calls

# Files to check in order of priority. The script will try to fix the first file
# in this list that exists and is relevant to the error.
FILES_TO_CHECK = ["Makefile", "build.sh", "Dockerfile.x86_64", "Dockerfile.arm64"]

def get_api_key():
    """Retrieves the API key from a file."""
    try:
        with open(GEMINI_API_KEY_PATH, 'r') as f:
            return f.read().strip()
    except FileNotFoundError:
        print(f"Error: Please place your Gemini API key in {GEMINI_API_KEY_PATH}")
        sys.exit(1)

def run_git_command(command, fail_on_error=True):
    """Executes a Git command and handles errors."""
    try:
        result = subprocess.run(command, capture_output=True, text=True, check=fail_on_error)
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        print(f"Git command failed: {' '.join(command)}")
        print(e.stderr)
        if fail_on_error:
            sys.exit(1)
        return ""

def setup_git_branch():
    """Checks out or creates the autofix branch."""
    print(f"--- Setting up Git branch: {BRANCH_NAME} ---")
    current_branch = run_git_command(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    branches = run_git_command(["git", "branch"])

    if BRANCH_NAME not in branches:
        print(f"Branch '{BRANCH_NAME}' not found. Creating and switching to it...")
        run_git_command(["git", "checkout", "-b", BRANCH_NAME])
    else:
        print(f"Branch '{BRANCH_NAME}' found. Switching to it...")
        run_git_command(["git", "checkout", BRANCH_NAME])

def run_build():
    """Executes the build command and captures output."""
    print(f"--- Running build command: {' '.join(BUILD_COMMAND)} ---")
    try:
        result = subprocess.run(BUILD_COMMAND, capture_output=True, text=True, check=False)
        print(result.stdout)
        if result.returncode != 0:
            print("Build failed.")
            return False, result.stdout + result.stderr
        print("Build succeeded.")
        return True, result.stdout
    except FileNotFoundError:
        print("Error: Build command not found. Please check your BUILD_COMMAND setting.")
        return False, "Build command not found."

def call_gemini_api(api_key, model_name, prompt):
    """Makes a request to the Gemini API."""
    try:
        payload = {
            "contents": [
                {
                    "role": "user",
                    "parts": [{"text": prompt}]
                }
            ]
        }
        api_url = f"{MODEL_URL_BASE}{model_name}:generateContent?key={api_key}"
        
        # Use curl to make the request as it's universally available
        result = subprocess.run(
            ["curl", "-s", "-X", "POST", api_url, "-H", "Content-Type: application/json", "-d", json.dumps(payload)],
            capture_output=True, text=True, check=True
        )
        response_json = json.loads(result.stdout)
        
        if 'candidates' in response_json and response_json['candidates']:
            return response_json['candidates'][0]['content']['parts'][0]['text']
        else:
            print("API call failed: No candidates in response.")
            print(f"Full response: {response_json}")
            return None
    except Exception as e:
        print(f"Error during API call: {e}")
        return None

def get_file_content(file_path):
    """Reads file content safely."""
    try:
        with open(file_path, 'r') as f:
            return f.read()
    except FileNotFoundError:
        print(f"Warning: File not found: {file_path}")
        return ""

def create_diff(original_content, new_content, file_path):
    """Generates and prints a colored diff of the changes."""
    print(f"\n--- Proposed changes for {file_path} ---")
    diff = difflib.unified_diff(
        original_content.splitlines(),
        new_content.splitlines(),
        fromfile=f'a/{file_path}',
        tofile=f'b/{file_path}',
        lineterm=''
    )
    for line in diff:
        if line.startswith('+'):
            print(f"\033[92m{line}\033[0m")
        elif line.startswith('-'):
            print(f"\033[91m{line}\033[0m")
        else:
            print(line)

def handle_initial_errors(args):
    """Handles the most common initial error: missing build script."""
    build_script_path = BUILD_COMMAND[0]
    if not os.path.exists(build_script_path):
        print(f"Error: Build script '{build_script_path}' not found. Creating a basic one.")
        starter_script_content = (
            "#!/bin/bash\n"
            "# This is a basic build script created by the autofix tool.\n"
            "# Please replace this with your actual build commands.\n"
            "echo 'Build script running...'\n"
            "# Replace the following line with your build command, e.g., make\n"
            "make\n"
        )
        create_diff("", starter_script_content, build_script_path)
        
        if args.yes_to_all:
            confirmation = 'y'
        else:
            confirmation = input(f"Apply this fix to '{build_script_path}' and commit? (y/n): ")
        
        if confirmation.lower() == 'y':
            with open(build_script_path, 'w') as f:
                f.write(starter_script_content)
            os.chmod(build_script_path, 0o755) # Make the script executable
            
            commit_message = f"autofix: Created initial {build_script_path}"
            run_git_command(["git", "add", build_script_path])
            run_git_command(["git", "commit", "-m", commit_message])
            print(f"Initial '{build_script_path}' created and committed. Retrying build...\n")
            time.sleep(5)
            return True
        else:
            print("Fix not applied. Exiting script.")
            return False
    return True

def main():
    parser = argparse.ArgumentParser(description="Autofix script for build errors using Gemini API.")
    parser.add_argument('-y', '--yes-to-all', action='store_true',
                        help="Automatically apply all proposed fixes without prompting.")
    args = parser.parse_args()
    
    if args.yes_to_all:
        print("Running in fully automated mode. All fixes will be applied without confirmation.")

    api_key = get_api_key()
    setup_git_branch()
    
    if not handle_initial_errors(args):
        sys.exit(1)

    start_time = time.time()
    
    while True:
        is_success, log_output = run_build()
        if is_success:
            print("\nBuild is successful. All errors cleared. Exiting.")
            break
            
        print("--- Build failed. Analyzing error log... ---")
        
        # Determine which file to check and fix
        error_file_to_fix = None
        for file_name in FILES_TO_CHECK:
            if os.path.exists(file_name):
                error_file_to_fix = file_name
                break

        if not error_file_to_fix:
            print("Could not find a valid file to fix. Exiting.")
            break

        # Construct the prompt for the API
        file_contents = "\n".join([
            f"Current `{file}` content:\n```{file_ext}\n{get_file_content(file)}\n```\n"
            for file in FILES_TO_CHECK if os.path.exists(file)
            for file_ext in (['makefile'] if file == 'Makefile' else ['bash'] if file == 'build.sh' else ['dockerfile'])
        ])

        prompt = (
            "You are an expert software engineer. The following is a build error log "
            "from a project. Identify the root cause, provide a fix, and then output "
            f"the complete, corrected code for the file that needs to be changed, which is `{error_file_to_fix}`. "
            "The fix should address the specific error in the log and maintain the existing functionality of the file. "
            "Do not provide any explanations, just the complete, fixed code block. "
            "Ensure the output is a single markdown code block.\n\n"
            "Build Log:\n"
            f"```\n{log_output}\n```\n\n"
            f"{file_contents}\n"
            f"Please provide the complete, corrected `{error_file_to_fix}` code. "
            f"Enclose the code in a single markdown block, starting with ```{error_file_to_fix.split('.')[-1]}` and ending with ```."
        )

        # Wait for the next API call to respect the rate limit
        elapsed_time = time.time() - start_time
        if elapsed_time < API_CALL_DELAY:
            wait_time = API_CALL_DELAY - elapsed_time
            print(f"Waiting {wait_time:.0f} seconds before next API call...")
            time.sleep(wait_time)
            
        fixed_code_md = call_gemini_api(api_key, MODEL_NAME, prompt)
        start_time = time.time()

        if fixed_code_md:
            try:
                code_lang = error_file_to_fix.split('.')[-1] if '.' in error_file_to_fix else 'bash' if error_file_to_fix == 'build.sh' else 'makefile'
                fixed_code = fixed_code_md.split(f'```{code_lang}')[1].strip().split('```')[0].strip()
                original_content = get_file_content(error_file_to_fix)
                
                create_diff(original_content, fixed_code, error_file_to_fix)

                if args.yes_to_all:
                    confirmation = 'y'
                else:
                    confirmation = input(f"Apply this fix to '{error_file_to_fix}' and commit? (y/n): ")
                
                if confirmation.lower() == 'y':
                    with open(error_file_to_fix, 'w') as f:
                        f.write(fixed_code)
                    
                    commit_message = f"autofix: Corrected build error in {error_file_to_fix}"
                    run_git_command(["git", "add", error_file_to_fix])
                    run_git_command(["git", "commit", "-m", commit_message])
                    print(f"Fix applied and committed. Retrying build...\n")
                    time.sleep(5)
                else:
                    print("Fix not applied. Exiting script.")
                    break
            except IndexError:
                print("Failed to parse code block from API response. Retrying build...")
                time.sleep(5)
        else:
            print("Failed to get a fix from the API. Exiting.")
            break
            
if __name__ == "__main__":
    main()
