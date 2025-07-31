#!/bin/bash

# --- Configuration ---
LOG_FILE="gemini_automation_log.txt"
GIT_COMMIT_MSG="Automated kernel fix: "
BUILD_COMMAND="make"
BUILD_LOG="build_errors.log"
DIFF_FILE="fix.diff"

# --- Setup ---
# Check if we are in the nymya-core directory
if [[ ! -d "kernel_syscalls" ]]; then
    echo "Error: This script must be run from the 'nymya-core' directory."
    exit 1
fi
echo "Running from the correct directory."

# Initialize a new Git branch for this process
git checkout -b gemini-autofix-$(date +%s)
echo "" > "$LOG_FILE"
echo "" > "$BUILD_LOG"

# --- Autonomous Fix Loop ---
while true; do
    echo "--- $(date) ---" | tee -a "$LOG_FILE"
    echo "Starting build attempt..." | tee -a "$LOG_FILE"

    # Clean and build. Redirect all output to the build log.
    $BUILD_COMMAND clean > /dev/null 2>&1
    $BUILD_COMMAND > "$BUILD_LOG" 2>&1

    # Check for build errors
    if grep -q "Error" "$BUILD_LOG" || grep -q "error:" "$BUILD_LOG"; then
        echo "Build failed. Sending errors to Gemini for a fix..." | tee -a "$LOG_FILE"

        # Capture the last 50 lines of the error log for the prompt
        ERROR_SUMMARY=$(tail -n 50 "$BUILD_LOG")

        # The prompt is designed to force Gemini to output a machine-readable diff
        GEMINI_PROMPT="You are an autonomous code-fixing agent. I have a failing kernel build. My task is to fix all errors and successfully compile.
- I will provide the last 50 lines of the build log containing the errors.
- Your sole output must be a single, unified diff to fix the **first** error.
- Do not provide any prose, explanations, or code blocks. Just the diff content.
- Example format:
--- a/path/to/file.c
+++ b/path/to/file.c
@@ -1,3 +1,3 @@
- old line
+ new line

Here are the build errors:
$ERROR_SUMMARY
"
        
        # Get the diff from Gemini and attempt to apply it
        GEMINI_RESPONSE=$(gemini -p "$GEMINI_PROMPT")
        
        # Check if Gemini provided a valid response
        if [[ -z "$GEMINI_RESPONSE" ]]; then
            echo "Gemini provided no response. Exiting." | tee -a "$LOG_FILE"
            exit 1
        fi
        
        # Save the diff to a temporary file
        echo "$GEMINI_RESPONSE" > "$DIFF_FILE"

        # Attempt to apply the patch. Use `-p1` for relative paths.
        if patch -p1 -N --binary < "$DIFF_FILE"; then
            echo "Successfully applied Gemini's fix." | tee -a "$LOG_FILE"
            # Commit the change
            git add .
            git commit -m "$GIT_COMMIT_MSG$(head -n 1 "$DIFF_FILE")"
            echo "Committed changes." | tee -a "$LOG_FILE"
        else
            echo "Failed to apply Gemini's fix. The diff might be malformed or conflict with existing code. Reverting and trying again." | tee -a "$LOG_FILE"
            # Revert the diff file and continue the loop, hoping the next prompt is better.
            git reset --hard HEAD
        fi

        # Always delete the temporary diff file
        rm -f "$DIFF_FILE"

    else
        # Build succeeded
        echo "Build completed successfully with no errors!" | tee -a "$LOG_FILE"
        git add .
        git commit -m "Final: Kernel build successful for all architectures."
        echo "Final commit made. Exiting." | tee -a "$LOG_FILE"
        break
    fi
done
