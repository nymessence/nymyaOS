#!/bin/bash
set -e

# Update and install cross-compiler, git, make, build tools
sudo apt update
sudo apt install -y gcc-aarch64-linux-gnu git make build-essential
sudo apt install -y flex bison libncurses-dev libelf-dev

# Install generic headers and image as fallback
sudo apt install -y linux-image-generic linux-headers-generic

# Try to install headers for current kernel (may fail if custom kernel)
if ! sudo apt install -y linux-headers-$(uname -r); then
  echo "Could not find headers for kernel $(uname -r), building manually..."
  
  cd /tmp

  # Clone Linux kernel source if not present
  if [ ! -d linux ]; then
    echo "Cloning Linux kernel source..."
    git clone --depth=1 https://github.com/torvalds/linux.git
  else
    echo "Linux source already cloned, pulling latest..."
    cd linux
    git pull
    cd ..
  fi

  cd linux

  # Copy current kernel config if available
  if [ -f /proc/config.gz ]; then
    echo "Extracting current kernel config..."
    zcat /proc/config.gz > .config
  elif [ -f /boot/config-$(uname -r) ]; then
    echo "Copying kernel config from /boot..."
    cp /boot/config-$(uname -r) .config
  else
    echo "No kernel config found; using default config."
    make defconfig
  fi

  # Prepare kernel source for building modules
  make olddefconfig
  make prepare
  make modules_prepare

  # Install headers to system location (you may need sudo)
  sudo make headers_install INSTALL_HDR_PATH=/usr/src/linux-headers-$(uname -r)

  # Link the headers to the expected modules build directory
  sudo ln -sf /usr/src/linux-headers-$(uname -r) /lib/modules/$(uname -r)/build

  cd ..
fi

# Setup build directory symlink for kernel build
if [ ! -L /tmp/build ]; then
  echo "Creating symlink /tmp/build -> /tmp/linux"
  ln -s /tmp/linux /tmp/build
else
  echo "/tmp/build symlink already exists, skipping."
fi

echo "Setup complete."
echo "You can now build your kernel module with:"
echo "make -C /tmp/build M=$(pwd)/kernel_syscalls modules"
