#!/bin/bash
set -e

# Detect host kernel version and architecture
HOST_KERNEL_VERSION=$(uname -r)
HOST_ARCH=$(uname -m)

echo "Using host kernel version: ${HOST_KERNEL_VERSION}"
echo "Detected host architecture: ${HOST_ARCH}"

# --- Kernel Headers Management ---
echo -e "\n🔍 Checking kernel headers status..."

# Special handling for Raspberry Pi kernels
if [[ "$HOST_KERNEL_VERSION" == *"raspi"* ]]; then
    # For Raspberry Pi: Use specific package naming
    KERNEL_HEADERS_PKG="raspberrypi-kernel-headers"
    CURRENT_HEADERS_PKG="raspberrypi-kernel-headers-${HOST_KERNEL_VERSION}"
else
    # For standard Ubuntu/Debian
    KERNEL_HEADERS_PKG="linux-headers-${HOST_KERNEL_VERSION}"
fi

# Check if kernel build directory exists and is valid
if [ ! -d "/lib/modules/${HOST_KERNEL_VERSION}/build" ] || \
   [ ! -f "/lib/modules/${HOST_KERNEL_VERSION}/build/Makefile" ] || \
   [ ! -f "/lib/modules/${HOST_KERNEL_VERSION}/build/scripts/Makefile.extrawarn" ]; then
    
    echo -e "\n⚠️  Kernel headers for ${HOST_KERNEL_VERSION} are missing or incomplete!"
    
    # Check if we have sudo access for installation
    if ! command -v sudo &> /dev/null; then
        echo "❌ Error: sudo is required to install kernel headers but not found"
        echo "Please install sudo or run this script with root privileges"
        exit 1
    fi
    
    # Update package lists
    echo "🔄 Updating package lists..."
    sudo apt update
    
    # Install appropriate kernel headers
    if [[ "$HOST_KERNEL_VERSION" == *"raspi"* ]]; then
        echo "📦 Installing Raspberry Pi kernel headers..."
        sudo apt install -y --reinstall raspberrypi-kernel-headers
    else
        echo "📦 Installing standard kernel headers..."
        sudo apt install -y --reinstall "linux-headers-${HOST_KERNEL_VERSION}"
    fi
    
    # Verify installation
    if [ ! -d "/lib/modules/${HOST_KERNEL_VERSION}/build" ]; then
        echo "❌ Failed to install kernel headers for ${HOST_KERNEL_VERSION}"
        echo "Please check your package sources and try again"
        exit 1
    fi
    
    echo "✅ Kernel headers for ${HOST_KERNEL_VERSION} installed successfully"
else
    echo "✅ Kernel headers for ${HOST_KERNEL_VERSION} are complete and ready"
fi

# --- Docker Setup ---
DOCKER_ARCH="arm64"
if [[ "$HOST_ARCH" == "x86_64" ]]; then
    DOCKER_ARCH="x86_64"
fi

IMAGE_NAME="nymya-build-env-${DOCKER_ARCH}"

# Check if the Docker image exists locally
echo -e "\n🐳 Checking Docker image: ${IMAGE_NAME}..."
if [[ "$(docker images -q ${IMAGE_NAME} 2> /dev/null)" == "" ]]; then
  echo "🐳 Docker image not found. Building it now..."
  docker build -t ${IMAGE_NAME} -f Dockerfile.${DOCKER_ARCH} .
  echo "✅ Docker image built successfully."
else
  echo "🐳 Docker image found. Skipping build."
fi

# --- Build Process ---
echo -e "\n🚀 Performing full compile (make clean && make deb-all)..."
docker run --rm \
    -v $(pwd):/nymyaOS/nymya-core \
    -v /lib/modules/${HOST_KERNEL_VERSION}/build:/lib/modules/${HOST_KERNEL_VERSION}/build:ro \
    -v /usr/src:/usr/src:ro \
    -v /lib/modules:/lib/modules:ro \
    ${IMAGE_NAME} \
    bash -c "cd /nymyaOS/nymya-core && make clean && make deb-all \
             CROSS_COMPILE=aarch64-linux-gnu- \
             CC=aarch64-linux-gnu-gcc \
             KBUILD_NOPEDANTIC=1"

# --- Cleanup Old Kernel Headers ---
echo -e "\n🧹 Cleaning up old kernel headers..."
if command -v sudo &> /dev/null; then
    # Get list of installed kernel headers (excluding current)
    if [[ "$HOST_KERNEL_VERSION" == *"raspi"* ]]; then
        # Raspberry Pi specific cleanup
        installed_headers=$(dpkg --list | grep 'raspberrypi-kernel-headers' | awk '{print $2}' | grep -v "${HOST_KERNEL_VERSION}" | grep -v "raspberrypi-kernel-headers")
    else
        # Standard Ubuntu/Debian cleanup
        installed_headers=$(dpkg --list | grep 'linux-headers-[0-9]' | awk '{print $2}' | grep -v "${HOST_KERNEL_VERSION}")
    fi
    
    # Remove old headers if any
    if [ -n "$installed_headers" ]; then
        echo "📦 Removing old kernel headers: $installed_headers"
        sudo apt purge -y $installed_headers
        sudo apt autoremove -y
        echo "✅ Old kernel headers cleaned up"
    else
        echo "✅ No old kernel headers to remove"
    fi
else
    echo "⚠️  Skipping old kernel header cleanup (sudo not available)"
fi

# --- Final Output ---
echo -e "\n🎉 All builds completed successfully!"
echo "📦 Artifacts in current directory:"
ls -lh *.deb
