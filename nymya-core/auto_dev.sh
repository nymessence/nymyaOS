#!/bin/bash

# --- Configuration ---
LOG_FILE="gemini_automation_log.txt"
GIT_COMMIT_MSG="Automated kernel fix: "
BUILD_COMMAND="./compile.sh"
BUILD_LOG="build_errors.log"
DIFF_FILE="fix.diff"

# --- Setup ---
# Check if we are in the nymya-core directory
if [[ ! -d "kernel_syscalls" ]]; then
    echo "Error: This script must be run from the 'nymya-core' directory." | tee -a "$LOG_FILE"
    exit 1
fi
echo "Running from the correct directory." | tee -a "$LOG_FILE"

# Initialize a new Git branch for this process
# Ensure we are on a branch that can be reverted easily
git checkout -b gemini-autofix-$(date +%s) || { echo "Error: Failed to create or checkout git branch."; exit 1; }
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

        # The prompt is now updated to request a diff for the original source file.
        GEMINI_PROMPT="You are an autonomous code-fixing agent. I have a failing kernel build. My task is to fix all errors and successfully compile.
- The C source files are in the current directory, not in any subdirectories. The build process copies them.
- I will provide the build log containing the errors.
- Your sole output must be a single, unified diff to fix the **first** error that directly prevents compilation or linking.
- Do not provide any prose, explanations, or code blocks. Just the diff content.
- The diff's file path must be relative to the current directory (e.g., '--- a/nymya_event_class_syscall_enter.c').
- Example format:
--- a/file.c
+++ b/file.c
@@ -1,3 +1,3 @@
- old line
+ new line

Here is the build log for analysis:
$ERROR_SUMMARY
"
        
        GEMINI_RESPONSE=$(gemini -p "$GEMINI_PROMPT")
        
        if [[ -z "$GEMINI_RESPONSE" ]]; then
            echo "Gemini provided no response. Exiting the autonomous loop." | tee -a "$LOG_FILE"
            exit 1
        fi
        
        echo "$GEMINI_RESPONSE" > "$DIFF_FILE"
        echo "Received diff from Gemini and saved to $DIFF_FILE." | tee -a "$LOG_FILE"

        if patch -p1 -N --binary < "$DIFF_FILE"; then
            echo "Successfully applied Gemini's fix." | tee -a "$LOG_FILE"
            git add .
            COMMIT_SUBJECT=$(head -n 1 "$DIFF_FILE" | sed 's/--- a\///' | sed 's/+++ b\///')
            git commit -m "$GIT_COMMIT_MSG$COMMIT_SUBJECT"
            echo "Committed changes: $GIT_COMMIT_MSG$COMMIT_SUBJECT" | tee -a "$LOG_FILE"
        else
            echo "Failed to apply Gemini's fix. Reverting and trying again." | tee -a "$LOG_FILE"
            git reset --hard HEAD
        fi

        rm -f "$DIFF_FILE"
        echo "Cleaned up temporary diff file: $DIFF_FILE." | tee -a "$LOG_FILE"

    else
        echo "Build completed successfully with zero errors!" | tee -a "$LOG_FILE"
        git add .
        git commit -m "Final: Kernel build successful for all architectures."
        echo "Final commit made. Exiting the autonomous loop." | tee -a "$LOG_FILE"
        break
    fi
done
