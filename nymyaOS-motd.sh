#!/bin/bash

# ==============================================================================
# SCRIPT: nymyaOS-motd.sh
# AUTHOR: Gemini
# DATE:   August 2025
# DESCRIPTION:
# This script automates the process of customizing the Message of the Day (MOTD)
# on an Ubuntu-based system. It disables the default MOTD scripts and creates a
# new one with a custom welcome message.
# ==============================================================================

MOTD_DIR="/etc/update-motd.d"

# Ensure the script is run with root privileges
if [ "$(id -u)" -ne 0 ]; then
    echo "This script must be run as root." >&2
    exit 1
fi

# ==============================================================================
# Customization settings
# ==============================================================================
CUSTOM_WELCOME_TEXT="Welcome to nymyaOS 0.1.0 Pre-Alpha $(uname -a)"
CUSTOM_SOURCE_CODE_LINK=" * Source Code:   https://github.com/nymessence/nymyaOS"
CUSTOM_SCRIPT_NAME="99-nymyaos-welcome"

# ==============================================================================
# Main functions
# ==============================================================================

# Function to disable default MOTD scripts
disable_defaults() {
    echo "Disabling default MOTD scripts..."
    # Find all default scripts and remove their executable permission
    for SCRIPT in "$MOTD_DIR"/??-*; do
        if [ -x "$SCRIPT" ] && [ "$(basename "$SCRIPT")" != "$CUSTOM_SCRIPT_NAME" ]; then
            sudo chmod -x "$SCRIPT"
            echo " - Disabled: $(basename "$SCRIPT")"
        fi
    done
    echo "Default scripts disabled."
}

# Function to create and enable the custom MOTD script
create_custom_motd() {
    echo "Creating custom MOTD script: $MOTD_DIR/$CUSTOM_SCRIPT_NAME"

    # Create the new script with the custom content
    cat > "$MOTD_DIR/$CUSTOM_SCRIPT_NAME" << EOF
#!/bin/sh
echo
echo "$CUSTOM_WELCOME_TEXT"
echo
echo "$CUSTOM_SOURCE_CODE_LINK"
echo
EOF

    # Make the new script executable
    chmod +x "$MOTD_DIR/$CUSTOM_SCRIPT_NAME"
    echo "Custom MOTD script created and enabled."
}

# Function to check if customization has already been applied
check_status() {
    if [ -f "$MOTD_DIR/$CUSTOM_SCRIPT_NAME" ] && [ -x "$MOTD_DIR/$CUSTOM_SCRIPT_NAME" ]; then
        echo "Custom MOTD is already in place. Nothing to do."
        return 0
    else
        return 1
    fi
}

# ==============================================================================
# Script execution
# ==============================================================================

check_status
if [ $? -eq 0 ]; then
    exit 0
fi

disable_defaults
create_custom_motd

echo
echo "Setup complete. The new welcome message will appear on your next login."

# Optional: show a preview of the new MOTD
echo "To preview the new MOTD, run:"
echo "sudo run-parts $MOTD_DIR"
echo
