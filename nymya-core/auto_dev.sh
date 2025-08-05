#!/bin/bash

# --- Configuration ---
LOG_FILE="gemini_automation_log.txt"
GIT_COMMIT_MSG="Automated kernel fix: "
BUILD_COMMAND="./compile.sh"
BUILD_LOG="build_errors.log"
GEMINI_MODEL="gemini-2.5-flash"

# --- Setup ---
echo "--- $(date) ---" | tee -a "$LOG_FILE"
echo "Starting autonomous kernel fix agent." | tee -a "$LOG_FILE"

# Check if we are in the nymya-core directory
if [[ ! -d "kernel_syscalls" ]]; then
    echo "Error: This script must be run from the 'nymya-core' directory." | tee -a "$LOG_FILE"
    exit 1
fi
echo "Running from the correct directory." | tee -a "$LOG_FILE"

# Initialize a new Git branch for this process
BRANCH_NAME="gemini-autofix"
git checkout -b "$BRANCH_NAME" || { echo "Error: Failed to create or checkout git branch."; exit 1; }
echo "Created and checked out new branch: $BRANCH_NAME" | tee -a "$LOG_FILE"

# Initialize log files
echo "" > "$LOG_FILE"
echo "" > "$BUILD_LOG"

# --- Autonomous Fix Loop ---
while true; do
    echo "--- $(date) ---" | tee -a "$LOG_FILE"
    echo "Starting build attempt with command: $BUILD_COMMAND" | tee -a "$LOG_FILE"

    # Clean and build. Redirect all output to the build log.
    $BUILD_COMMAND clean > /dev/null 2>&1
    $BUILD_COMMAND > "$BUILD_LOG" 2>&1

    # Check for build errors
    if grep -q "Error" "$BUILD_LOG" || grep -q "error:" "$BUILD_LOG"; then
        echo "Build failed. Sending errors to Gemini for a fix..." | tee -a "$LOG_FILE"

        ERROR_SUMMARY=$(tail -n 200 "$BUILD_LOG")

        # The prompt now requests a sed command to fix the first error
        GEMINI_PROMPT="You are an autonomous code-fixing agent. I have a failing kernel build. My task is to fix all errors and successfully compile.
- The C source files are in the current directory, not in any subdirectories.
- I will provide the build log containing the errors.
- Your sole output must be a single 'sed' command to fix the **first** error that directly prevents compilation or linking.
- Do not provide any prose, explanations, or code blocks. Just the command.
- Example command format:
sed -i 's/old_pattern/new_pattern/' filename.c

Here is the build log for analysis:
$ERROR_SUMMARY
"
        
        # Call Gemini CLI
        GEMINI_RESPONSE=$(gemini -m "$GEMINI_MODEL" -p "$GEMINI_PROMPT")
        
        if [[ -z "$GEMINI_RESPONSE" ]]; then
            echo "Gemini provided no response. Exiting the autonomous loop." | tee -a "$LOG_FILE"
            exit 1
        fi
        
        echo "Received command from Gemini: $GEMINI_RESPONSE" | tee -a "$LOG_FILE"

        # Check if the response is a sed command before executing
        if [[ "$GEMINI_RESPONSE" == "sed -i"* ]]; then
            eval "$GEMINI_RESPONSE"
            if [ $? -eq 0 ]; then
                echo "Successfully executed Gemini's fix." | tee -a "$LOG_FILE"
                git add .
                # Create a commit message from the sed command
                COMMIT_MSG=$(echo "$GEMINI_RESPONSE" | cut -d' ' -f4 | sed 's/\\//g' | cut -d'/' -f1)
                git commit -m "$GIT_COMMIT_MSG$COMMIT_MSG"
                echo "Committed changes." | tee -a "$LOG_FILE"
            else
                echo "Failed to execute Gemini's fix. Reverting and trying again." | tee -a "$LOG_FILE"
                git reset --hard HEAD
                exit 1 # Exit to prevent an infinite loop if the fix is bad
            fi
        else
            echo "Gemini's response was not a valid sed command. Exiting." | tee -a "$LOG_FILE"
            exit 1
        fi
        
    else
        echo "Build completed successfully with zero errors!" | tee -a "$LOG_FILE"
        git add .
        git commit -m "Final: Kernel build successful for all architectures."
        echo "Final commit made. Exiting the autonomous loop." | tee -a "$LOG_FILE"
        break
    fi
done

echo "Autonomous fix agent finished." | tee -a "$LOG_FILE"

