#!/bin/bash
set -euo pipefail

# This script is run inside a Docker container by cross_compile.sh
# It assumes the nymya-core directory is mounted at /nymyaOS and the
# kernel source for the target architecture is at /kernel-src

ARCH="x86_64"
CROSS_COMPILE="x86_64-linux-gnu-"
DEB_ARCH="amd64"
KERNEL_SRC_DIR="/kernel-src"
KERNEL_OUT_DIR="/build/kernel-x86_64"

echo "🚀 Starting build for ${ARCH}..."

# Prepare kernel headers
echo "🔧 Preparing kernel headers..."
cd "${KERNEL_SRC_DIR}"
make mrproper
export RETPOLINE_CFLAGS="-mretpoline"
make O="${KERNEL_OUT_DIR}" ARCH="${ARCH}" defconfig
make O="${KERNEL_OUT_DIR}" ARCH="${ARCH}" modules_prepare

# Build the kernel module
echo "🔨 Building kernel module..."
cd /nymyaOS/nymya-core
make kernel PKG_ARCH=${DEB_ARCH}
make -C "${KERNEL_OUT_DIR}" M="$(pwd)/kernel_syscalls/${DEB_ARCH}" ARCH="${ARCH}" modules

echo "✅ Build completed for ${ARCH}."