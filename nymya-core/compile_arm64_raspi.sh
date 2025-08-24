#!/bin/bash
set -euo pipefail

# NOTE: REQUIRES A RASPERRY PI 4 OR LATER TO BUILD & RUN

# --------------------------
# Configuration
# --------------------------
HOST_KERNEL_VERSION=$(uname -r)
HOST_ARCH=$(uname -m)
echo "Using host kernel version: ${HOST_KERNEL_VERSION}"
echo "Detected host architecture: ${HOST_ARCH}"

TARGET="arm64"
CROSS_PREFIX="aarch64-linux-gnu-"
DOCKER_PLATFORM="linux/arm64"

SCRIPT_DIR=$(dirname "$(realpath "$0")")
RPI_KERNEL_SRC="${HOME}/raspi-kernel-src"

# --------------------------
# Create architecture-specific directories first
# --------------------------
echo -e "\n🔧 Creating architecture-specific directories..."
ARCH_DIR="arm64"
echo "📁 Creating directory structure for ${TARGET} (${ARCH_DIR})"
mkdir -p "kernel_syscalls/${ARCH_DIR}"

# Create the necessary files for the kernel module
echo "/* Kernel syscall interface for ${TARGET} */" > "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "#ifndef NYMYA_KERNEL_H" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "#define NYMYA_KERNEL_H" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "/* Add your kernel syscall definitions here */" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
echo "#endif /* NYMYA_KERNEL_H */" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"

echo "obj-m := nymya_core.o" > "kernel_syscalls/${ARCH_DIR}/Makefile"
echo "nymya_core-objs := nymya_kernel_module.o" >> "kernel_syscalls/${ARCH_DIR}/Makefile"

echo "/* Kernel module implementation */" > "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "#include <linux/module.h>" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "#include <linux/kernel.h>" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "#include <linux/init.h>" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "static int __init nymya_init(void) {" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "    pr_info(\"Nymya Core: Module loaded\\n\");" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "    return 0;" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "}" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "static void __exit nymya_exit(void) {" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "    pr_info(\"Nymya Core: Module unloaded\\n\");" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "}" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "module_init(nymya_init);" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "module_exit(nymya_exit);" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "MODULE_LICENSE(\"GPL\");" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "MODULE_AUTHOR(\"NymyaOS Team\");" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"
echo "MODULE_DESCRIPTION(\"NymyaOS Core Kernel Module\");" >> "kernel_syscalls/${ARCH_DIR}/nymya_kernel_module.c"

# --------------------------
# Build for arm64 raspi target
# --------------------------
echo -e "\n🚀 Starting build for ${TARGET}..."
IMAGE_NAME="nymya-build-env-${TARGET}"
CROSS_COMPILE=${CROSS_PREFIX}
CC="${CROSS_COMPILE}gcc"
KERNEL_OUT_DIR="/tmp/build-${TARGET}"
KERNEL_SRC_COPY="/tmp/linux-src-${TARGET}"

echo -e "\n🐳 Building for target: ${TARGET} using CROSS_COMPILE=${CROSS_COMPILE}"
echo "🐳 Building docker image for ${TARGET}"
docker build --platform=${DOCKER_PLATFORM} -t ${IMAGE_NAME} -f Dockerfile.${TARGET} .

KERNEL_SRC="${RPI_KERNEL_SRC}"

# Create clean kernel source copy for this target
echo "📁 Creating clean kernel source copy for ${TARGET}"
rm -rf "${KERNEL_SRC_COPY}"
cp -r "${KERNEL_SRC}" "${KERNEL_SRC_COPY}"

ARCH_NAME="arm64"
DEB_ARCH="arm64"

docker run --rm --privileged --platform=${DOCKER_PLATFORM} \
    -w /nymyaOS/nymya-core \
    -v "$(pwd)":/nymyaOS/nymya-core \
    -v "${KERNEL_SRC_COPY}":/nymyaOS/kernel-src:rw \
    "${IMAGE_NAME}" \
    bash -c "cd /nymyaOS/kernel-src && make mrproper && mkdir -p ${KERNEL_OUT_DIR} && make O=${KERNEL_OUT_DIR} ARCH=${ARCH_NAME} CROSS_COMPILE=${CROSS_COMPILE} defconfig && make O=${KERNEL_OUT_DIR} modules_prepare && make O=${KERNEL_OUT_DIR} -j$(nproc) && make O=${KERNEL_OUT_DIR} modules && make O=${KERNEL_OUT_DIR} M=/nymyaOS/nymya-core/kernel_syscalls/${DEB_ARCH} modules && make -C /nymyaOS/nymya-core deb-kernel"

# --------------------------
# Copy .deb files from Docker containers
# --------------------------
echo -e "\n📦 Copying .deb files from Docker containers..."
DEB_ARCH="arm64"
docker run --rm \
    -v "$(pwd)":/output \
    "nymya-build-env-${TARGET}" \
    bash -c "cp /nymyaOS/nymya-core/nymya-core-kernel_*.deb /output/ 2>/dev/null || true"

echo -e "\n🎉 Build completed for ${TARGET}. Artifacts:"
ls -lh *.deb || true
