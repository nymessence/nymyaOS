#!/usr/bin/env bash
set -e
set -x
# Enables debugging output

# === Normalize architecture ===
detect_arch() {
  local input="$1"
  case "$input" in
    x86_64|amd64)
      echo "x86_64"
      ;;
    aarch64|arm64)
      echo "arm64"
      ;;
    *)
      echo "unknown"
      ;;
  esac
}

# === Config ===
RAW_ARG="${1:-$(uname -m)}"
TARGET_ARCH=$(detect_arch "$(echo "$RAW_ARG" | tr '[:upper:]' '[:lower:]')")
HOST_ARCH=$(detect_arch "$(uname -m)")
KERNEL_VERSION="$(uname -r)"
WORKDIR="/tmp"
LINUX_SRC_DIR="$WORKDIR/linux"

if [[ "$TARGET_ARCH" == "unknown" ]]; then
  echo "Error: Unsupported or unknown target architecture: $RAW_ARG"
  exit 1
fi

echo "Detected host architecture: $HOST_ARCH"
echo "Target architecture set to:  $TARGET_ARCH"
echo "Kernel version:             $KERNEL_VERSION"

# === Determine cross-compiler ===
if [[ "$TARGET_ARCH" == "$HOST_ARCH" ]]; then
  CROSS_COMPILE=""
  ARCH="$TARGET_ARCH"
  echo "Building for host architecture: $ARCH (no cross-compiler needed)"
else
  ARCH="$TARGET_ARCH"
  echo "Setting up cross-compiler for target architecture: $ARCH"

  sudo apt update || { echo "Error: apt update failed"; exit 1; }

  case "$ARCH" in
    arm64)
      CROSS_COMPILE="aarch64-linux-gnu-"
      echo "Installing aarch64 cross-compiler..."
      sudo apt install -y gcc-aarch64-linux-gnu || { echo "Error: install failed"; exit 1; }
      ;;
    x86_64)
      CROSS_COMPILE="x86_64-linux-gnu-"
      echo "Installing x86_64 cross-compiler..."
      sudo apt install -y gcc-x86-64-linux-gnu || { echo "Error: install failed"; exit 1; }
      ;;
    *)
      echo "Error: Unknown target architecture for cross-compilation: $ARCH"
      exit 1
      ;;
  esac

  echo "Cross-compiler prefix is: $CROSS_COMPILE"
fi

# === Essential build tools ===
echo "Installing essential build tools..."
sudo apt update || { echo "Error: apt update failed"; exit 1; }
sudo apt install -y git make build-essential flex bison libncurses-dev libelf-dev bc || { echo "Error: failed to install build tools"; exit 1; }
echo "Essential build tools installed."

# === Try to install kernel headers ===
NEED_KERNEL_SRC=0
if [[ "$TARGET_ARCH" == "$HOST_ARCH" ]]; then
  echo "Installing kernel headers for host ($KERNEL_VERSION) via apt..."
  if ! sudo apt install -y "linux-headers-$KERNEL_VERSION"; then
    echo "Headers not found in apt, will build from source"
    NEED_KERNEL_SRC=1
  else
    echo "Kernel headers installed via apt"
  fi
else
  echo "Cross-compiling target differs from host, will build headers from source"
  NEED_KERNEL_SRC=1
fi

# === Build kernel headers from source if needed ===
if [[ "$NEED_KERNEL_SRC" -eq 1 ]]; then
  echo "Building kernel headers from source..."
  cd "$WORKDIR" || { echo "Error: cd $WORKDIR failed"; exit 1; }

  if [[ ! -d "$LINUX_SRC_DIR" ]]; then
    git clone --depth=1 https://github.com/torvalds/linux.git linux || { echo "Error: git clone failed"; exit 1; }
  else
    echo "Updating existing linux source"
    cd linux || exit 1
    git pull || { echo "Error: git pull failed"; exit 1; }
    cd ..
  fi

  cd linux || exit 1
  echo "Preparing .config"
  if [[ -f /proc/config.gz ]]; then
    zcat /proc/config.gz > .config
  elif [[ -f "/boot/config-$KERNEL_VERSION" ]]; then
    cp "/boot/config-$KERNEL_VERSION" .config
  else
    make ARCH="$ARCH" defconfig
  fi

  make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" olddefconfig
  make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" prepare
  make ARCH="$ARCH" CROSS_COMPILE="$CROSS_COMPILE" modules_prepare

  echo "Installing headers to /usr/src/linux-headers-$KERNEL_VERSION"
  sudo make ARCH="$ARCH" headers_install INSTALL_HDR_PATH="/usr/src/linux-headers-$KERNEL_VERSION"
  sudo ln -sf "/usr/src/linux-headers-$KERNEL_VERSION" "/lib/modules/$KERNEL_VERSION/build"
else
  echo "Kernel headers are already in place"
fi

# === Convenience symlink ===
if [[ ! -L /tmp/build ]]; then
  echo "Creating /tmp/build → $LINUX_SRC_DIR"
  ln -s "$LINUX_SRC_DIR" /tmp/build || { echo "Error: symlink failed"; exit 1; }
else
  echo "/tmp/build symlink already exists"
fi

# === Done ===
echo "✅ Setup complete for kernel module development!"
echo "To build your module, run:"
echo "  make -C /tmp/build M=\$(pwd)/kernel_syscalls ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE modules"