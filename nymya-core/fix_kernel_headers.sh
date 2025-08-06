#!/bin/bash

# Define parameters for persistence
MAX_REINSTALL_ATTEMPTS=3
MAX_PREPARE_ATTEMPTS=5
SLEEP_TIME_AFTER_PREPARE_FAIL=5
SLEEP_TIME_AFTER_REINSTALL=10
SLEEP_TIME_AFTER_SOURCE_EXTRACT=15 # Added sleep after extracting source

echo "--- Starting Automated Kernel Header Fix ---"

# Get the current kernel version
KERNEL_VERSION=$(uname -r)
KERNEL_HEADERS_DIR="/usr/src/linux-headers-${KERNEL_VERSION}"
LINUX_SOURCE_PACKAGE_SPECIFIC="linux-source-${KERNEL_VERSION}"

echo "Detected kernel version: ${KERNEL_VERSION}"
echo "Kernel headers expected at: ${KERNEL_HEADERS_DIR}"

reinstall_attempt=0
while [ ${reinstall_attempt} -lt ${MAX_REINSTALL_ATTEMPTS} ]; do
    reinstall_attempt=$((reinstall_attempt + 1))
    echo "--- Reinstallation/Source Prep Attempt: ${reinstall_attempt}/${MAX_REINSTALL_ATTEMPTS} ---"

    # Step 1: Update apt lists and reinstall kernel headers
    echo "Running: sudo apt update"
    if sudo apt update; then
        echo "✅ apt update succeeded."
    else
        echo "❌ apt update failed. This might indicate network or repository issues. Continuing anyway."
        sleep 3
    fi

    echo "Running: sudo apt install --reinstall linux-headers-${KERNEL_VERSION}"
    if sudo apt install --reinstall linux-headers-${KERNEL_VERSION}; then
        echo "✅ Kernel headers reinstalled successfully. Waiting ${SLEEP_TIME_AFTER_REINSTALL} seconds."
        sleep ${SLEEP_TIME_AFTER_REINSTALL}
    else
        echo "❌ Failed to reinstall kernel headers. Please check your internet connection, apt sources, and disk space."
        echo "Waiting ${SLEEP_TIME_AFTER_PREPARE_FAIL} seconds before next attempt."
        sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
        continue # Skip prepare and go to next reinstall attempt
    fi

    # Install SELinux development libraries which might provide classmap.h
    echo "Running: sudo apt install -y libselinux1-dev"
    if sudo apt install -y libselinux1-dev; then
        echo "✅ libselinux1-dev installed successfully."
        sleep 3 # Small pause after installation
    else
        echo "❌ Failed to install libselinux1-dev. This might be a dependency issue. Continuing anyway."
        sleep 3
    fi

    # Step 2: Attempt 'make prepare' in KERNEL_HEADERS_DIR first
    echo "Navigating to ${KERNEL_HEADERS_DIR} to run 'make prepare'..."
    PREPARE_STATUS=1 # Default to failed

    ( # Subshell for prepare in headers directory
        if [ ! -d "${KERNEL_HEADERS_DIR}" ]; then
            echo "Error: Kernel headers directory ${KERNEL_HEADERS_DIR} does not exist or is not accessible."
            exit 1
        fi
        cd "${KERNEL_HEADERS_DIR}" || { echo "Error: Could not change to directory ${KERNEL_HEADERS_DIR} in subshell."; exit 1; }
        
        prepare_attempt=0
        while [ ${prepare_attempt} -lt ${MAX_PREPARE_ATTEMPTS} ]; do
            prepare_attempt=$((prepare_attempt + 1))
            echo "Running 'sudo make prepare' in headers dir (Attempt ${prepare_attempt}/${MAX_PREPARE_ATTEMPTS})..."
            if sudo make prepare; then
                echo "✅ 'sudo make prepare' in headers dir succeeded."
                exit 0 # Success, exit subshell
            else
                echo "❌ 'sudo make prepare' in headers dir failed."
                sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
            fi
        done
        echo "Max 'make prepare' attempts reached in headers dir. Exiting subshell with failure."
        exit 1
    )
    PREPARE_STATUS=$?

    if [ ${PREPARE_STATUS} -eq 0 ]; then
        echo "✅ Kernel headers successfully prepared using the headers directory. Proceeding."
        break # Break from outer reinstall loop, successful prepare
    else
        echo "❌ 'make prepare' failed in headers directory. Attempting with full kernel source."
    fi

    # Step 3: If headers dir prepare failed, try with full kernel source
    echo "--- Attempting to use full kernel source for 'make prepare' ---"
    
    SOURCE_PACKAGE_INSTALLED=false
    ACTUAL_FULL_KERNEL_SOURCE_TARBALL="/usr/src/linux-source-${KERNEL_VERSION}.tar.xz"

    # Try installing the version-specific source package via apt
    echo "Running: sudo apt install -y ${LINUX_SOURCE_PACKAGE_SPECIFIC}"
    if sudo apt install -y "${LINUX_SOURCE_PACKAGE_SPECIFIC}"; then
        echo "✅ Version-specific full kernel source package installed successfully via apt."
        SOURCE_PACKAGE_INSTALLED=true
        sleep 3
    else
        echo "❌ Failed to install version-specific full kernel source package (${LINUX_SOURCE_PACKAGE_SPECIFIC}) via apt."
        
        # --- Direct Download Attempts ---
        if command -v wget &> /dev/null; then
            echo "Attempting direct download from common Ubuntu ARM archives."
            # Try Ubuntu Ports archive
            KERNEL_PATCH_VERSION=$(echo ${KERNEL_VERSION} | cut -d'-' -f1) # e.g., 6.8.0
            DIRECT_DOWNLOAD_URL_UBUNTU_PORTS="http://ports.ubuntu.com/ubuntu-ports/pool/main/l/linux/linux-source-${KERNEL_PATCH_VERSION}.tar.xz"
            
            echo "Trying URL: ${DIRECT_DOWNLOAD_URL_UBUNTU_PORTS}"
            if sudo wget -q --show-progress -O "${ACTUAL_FULL_KERNEL_SOURCE_TARBALL}" "${DIRECT_DOWNLOAD_URL_UBUNTU_PORTS}"; then
                echo "✅ Full kernel source tarball downloaded successfully from Ubuntu Ports."
                SOURCE_PACKAGE_INSTALLED=true
                sleep 3
            else
                echo "❌ Download failed from Ubuntu Ports. Trying Raspberry Pi archive."
                # Try Raspberry Pi archive
                DIRECT_DOWNLOAD_URL_RASPI_ARCHIVE="http://archive.raspberrypi.org/debian/pool/main/l/linux/linux-source-${KERNEL_PATCH_VERSION}.tar.xz"
                
                echo "Trying URL: ${DIRECT_DOWNLOAD_URL_RASPI_ARCHIVE}"
                if sudo wget -q --show-progress -O "${ACTUAL_FULL_KERNEL_SOURCE_TARBALL}" "${DIRECT_DOWNLOAD_URL_RASPI_ARCHIVE}"; then
                    echo "✅ Full kernel source tarball downloaded successfully from Raspberry Pi archive."
                    SOURCE_PACKAGE_INSTALLED=true
                    sleep 3
                else
                    echo "❌ Direct download failed from Raspberry Pi archive. Tarball might not be at these URLs."
                fi
            fi
        else
            echo "❌ wget command not found. Cannot attempt direct download."
        fi
    fi

    if [ "${SOURCE_PACKAGE_INSTALLED}" = true ]; then
        if [ -f "${ACTUAL_FULL_KERNEL_SOURCE_TARBALL}" ]; then
            ACTUAL_FULL_KERNEL_SOURCE_DIR="/usr/src/$(basename "${ACTUAL_FULL_KERNEL_SOURCE_TARBALL}" .tar.xz)"
            
            echo "Extracting full kernel source from ${ACTUAL_FULL_KERNEL_SOURCE_TARBALL} to ${ACTUAL_FULL_KERNEL_SOURCE_DIR}..."
            sudo rm -rf "${ACTUAL_FULL_KERNEL_SOURCE_DIR}"
            sudo mkdir -p "${ACTUAL_FULL_KERNEL_SOURCE_DIR}"
            if sudo tar -xf "${ACTUAL_FULL_KERNEL_SOURCE_TARBALL}" -C "${ACTUAL_FULL_KERNEL_SOURCE_DIR}" --strip-components=1; then
                echo "✅ Full kernel source extracted successfully. Waiting ${SLEEP_TIME_AFTER_SOURCE_EXTRACT} seconds."
                sleep ${SLEEP_TIME_AFTER_SOURCE_EXTRACT}
                
                ( # Subshell for prepare in full source directory
                    echo "Navigating to ${ACTUAL_FULL_KERNEL_SOURCE_DIR} to run 'make prepare'..."
                    cd "${ACTUAL_FULL_KERNEL_SOURCE_DIR}" || { echo "Error: Could not change to directory ${ACTUAL_FULL_KERNEL_SOURCE_DIR} in subshell."; exit 1; }
                    
                    prepare_attempt=0
                    while [ ${prepare_attempt} -lt ${MAX_PREPARE_ATTEMPTS} ]; do
                        prepare_attempt=$((prepare_attempt + 1))
                        echo "Running 'sudo make prepare' in full source dir (Attempt ${prepare_attempt}/${MAX_PREPARE_ATTEMPTS})..."
                        if sudo make prepare; then
                            echo "✅ 'sudo make prepare' in full source dir succeeded."
                            exit 0
                        else
                            echo "❌ 'sudo make prepare' in full source dir failed."
                            sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
                        fi
                    done
                    echo "Max 'make prepare' attempts reached in full source dir. Exiting subshell with failure."
                    exit 1
                )
                PREPARE_STATUS=$?

                if [ ${PREPARE_STATUS} -eq 0 ]; then
                    echo "✅ Kernel headers successfully prepared using the full source directory."
                    break
                else
                    echo "❌ 'make prepare' failed in full source directory. Retrying reinstallation cycle."
                fi

            else
                echo "❌ Failed to extract full kernel source. Check tarball integrity or disk space."
                echo "Waiting ${SLEP_TIME_AFTER_PREPARE_FAIL} seconds before next attempt."
                sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
            fi
        else
            echo "❌ Full kernel source tarball not found at ${ACTUAL_FULL_KERNEL_SOURCE_TARBALL} after package installation/download. Skipping full source prepare attempt."
            echo "Waiting ${SLEEP_TIME_AFTER_PREPARE_FAIL} seconds before next attempt."
            sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
        fi
    else
        echo "Skipping full kernel source prepare attempt as the correct source package could not be installed or downloaded."
        echo "Waiting ${SLEEP_TIME_AFTER_PREPARE_FAIL} seconds before next attempt."
        sleep ${SLEEP_TIME_AFTER_PREPARE_FAIL}
    fi
done

# If we reached here, it means prepare_kernel_headers was not successful
echo "--- Automated Kernel Header Fix Failed ---"
echo "Max reinstallation/source preparation attempts reached. The kernel headers could not be prepared automatically."
echo "Please manually inspect the errors above and consider seeking further system administration help."
exit 1

# --- Commands to run after successful kernel header preparation ---
# These commands will only execute if the prepare_kernel_headers function returns 0 (success)
echo "--- Kernel headers prepared. Proceeding with nymya-core build and commit. ---"

# Define the nymyaOS/nymya-core directory
NYMYA_CORE_DIR="/home/erick/nymyaOS/nymya-core"

echo "Navigating to ${NYMYA_CORE_DIR}..."
cd "${NYMYA_CORE_DIR}" || { echo "Error: Could not change to directory ${NYMYA_CORE_DIR}. Exiting."; exit 1; }
echo "Current directory: $(pwd)"

echo "Running ./compile..."
./compile.sh || { echo "Error: ./compile.sh failed. Please check its output."; exit 1; }

echo "Adding fix_kernel_headers.sh to git and committing..."
# Ensure the script itself is added for tracking
git add fix_kernel_headers.sh || { echo "Error: Could not add fix_kernel_headers.sh to git."; exit 1; }
git commit -a -m "Added kernel fix script for compiles" || { echo "Error: Could not commit changes."; exit 1; }

echo "All post-preparation steps completed."
