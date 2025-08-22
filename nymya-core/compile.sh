#!/bin/bash
set -euo pipefail

# --------------------------
# Configuration
# --------------------------
HOST_KERNEL_VERSION=$(uname -r)
HOST_ARCH=$(uname -m)
echo "Using host kernel version: ${HOST_KERNEL_VERSION}"
echo "Detected host architecture: ${HOST_ARCH}"

TARGETS=("x86_64" "arm64" "riscv64")
declare -A CROSS_PREFIX=(
    ["x86_64"]="x86_64-linux-gnu-"
    ["arm64"]="aarch64-linux-gnu-"
    ["riscv64"]="riscv64-linux-gnu-"
)
declare -A DOCKER_PLATFORM=(
    ["x86_64"]=""
    ["arm64"]="linux/arm64"
    ["riscv64"]="linux/riscv64"
)

SCRIPT_DIR=$(dirname "$(realpath "$0")")
RPI_KERNEL_SRC="${HOME}/raspi-kernel-src"

# --------------------------
# Build for each target
# --------------------------
echo -e "\n🚀 Starting build for all targets..."
for TARGET in "${TARGETS[@]}"; do
    IMAGE_NAME="nymya-build-env-${TARGET}"
    CROSS_COMPILE=${CROSS_PREFIX[$TARGET]}
    CC="${CROSS_COMPILE}gcc"
    KERNEL_OUT_DIR="/tmp/build-${TARGET}"
    KERNEL_SRC_COPY="/tmp/rpi-kernel-src-${TARGET}"

    echo -e "\n🐳 Building for target: ${TARGET} using CROSS_COMPILE=${CROSS_COMPILE}"

    # Create clean kernel source copy for this target
    echo "📁 Creating clean kernel source copy for ${TARGET}"
    rm -rf "${KERNEL_SRC_COPY}"
    cp -r "${RPI_KERNEL_SRC}" "${KERNEL_SRC_COPY}"

    if [ "$TARGET" = "x86_64" ]; then
        # Host-arch image with cross-toolchain (no QEMU)
        docker run --rm \
            -w /nymyaOS/nymya-core \
            -v "$(pwd)":/nymyaOS/nymya-core \
            -v "/lib/modules/${HOST_KERNEL_VERSION}/build":/lib/modules/${HOST_KERNEL_VERSION}/build:ro \
            -v /usr/src:/usr/src:ro \
            -v "${KERNEL_SRC_COPY}":/nymyaOS/rpi-kernel-src:rw \
            "${IMAGE_NAME}" \
            bash -c "cd /nymyaOS/rpi-kernel-src && make mrproper && mkdir -p ${KERNEL_OUT_DIR} && make O=${KERNEL_OUT_DIR} ARCH=${TARGET} CROSS_COMPILE=${CROSS_COMPILE} defconfig && make O=${KERNEL_OUT_DIR} modules_prepare && make O=${KERNEL_OUT_DIR} M=/nymyaOS/nymya-core modules"
    else
        # Platform-specific (may use QEMU)
        docker run --rm --platform=${DOCKER_PLATFORM[$TARGET]} \
            -w /nymyaOS/nymya-core \
            -v "$(pwd)":/nymyaOS/nymya-core \
            -v "/lib/modules/${HOST_KERNEL_VERSION}/build":/lib/modules/${HOST_KERNEL_VERSION}/build:ro \
            -v /usr/src:/usr/src:ro \
            -v "${KERNEL_SRC_COPY}":/nymyaOS/rpi-kernel-src:rw \
            "${IMAGE_NAME}" \
            bash -c "cd /nymyaOS/rpi-kernel-src && make mrproper && mkdir -p ${KERNEL_OUT_DIR} && make O=${KERNEL_OUT_DIR} ARCH=${TARGET} CROSS_COMPILE=${CROSS_COMPILE} defconfig && make O=${KERNEL_OUT_DIR} modules_prepare && make O=${KERNEL_OUT_DIR} M=/nymyaOS/nymya-core modules"
    fi
done

# --------------------------
# Package .deb files on host
# --------------------------
echo -e "\n📦 Building .deb packages..."
make deb-kernel

echo -e "\n🎉 All builds completed. Artifacts:"
ls -lh *.deb || true

