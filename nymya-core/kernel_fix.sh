# This script will attempt to fix common issues with missing kernel headers
# by ensuring that the kernel and headers are in sync.

# First, check your current kernel version and the installed headers.
echo "Checking current kernel and installed headers..."
echo "Current kernel version: $(uname -r)"
echo "Installed header directories:"
ls /usr/src | grep linux-headers

# Option 1: The most common and recommended fix for Raspberry Pi OS.
# This command performs a full upgrade to make sure your kernel and headers
# are aligned to the latest version available in the official repositories.
echo "Attempting to sync kernel and headers with a full system upgrade..."
sudo apt update -y
sudo apt full-upgrade -y

# After the upgrade, a reboot might be necessary to boot into the new kernel.
echo "The system may need a reboot to apply the new kernel."
echo "You can check the kernel version again after rebooting with: uname -r"

# Option 2: If you are trying to build inside a Docker container.
# The container needs access to the host's kernel headers.
# You must run the container with bind mounts for /lib/modules and /usr/src.
# Example command:
#
# docker run --privileged -it -v /lib/modules:/lib/modules -v /usr/src:/usr/src your_image_name
#
# The --privileged flag is required to compile and install kernel modules,
# and the -v flags expose the necessary directories from the host.

echo "Script finished. Please check your system and reboot if necessary."
