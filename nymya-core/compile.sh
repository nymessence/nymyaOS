#!/bin/bash
set -euo pipefail

# --------------------------
# Configuration
# --------------------------
HOST_KERNEL_VERSION=$(uname -r)
HOST_ARCH=$(uname -m)
echo "Using host kernel version: ${HOST_KERNEL_VERSION}"
echo "Detected host architecture: ${HOST_ARCH}"

TARGETS=("arm64")
declare -A CROSS_PREFIX=(
    ["x86_64"]="x86_64-linux-gnu-"
    ["arm64"]="aarch64-linux-gnu-"
    ["riscv64"]="riscv64-linux-gnu-"
)
declare -A DOCKER_PLATFORM=(
    ["x86_64"]="linux/amd64"
    ["arm64"]="linux/arm64"
    ["riscv64"]="linux/riscv64"
)

SCRIPT_DIR=$(dirname "$(realpath "$0")")
RPI_KERNEL_SRC="${HOME}/raspi-kernel-src"

# --------------------------
# Create architecture-specific directories first
# --------------------------
echo -e "\n🔧 Creating architecture-specific directories..."
for TARGET in "${TARGETS[@]}"; do
    # Map target to Debian architecture name
    case "$TARGET" in
        "x86_64") ARCH_DIR="amd64" ;;
        "arm64") ARCH_DIR="arm64" ;;
        "riscv64") ARCH_DIR="riscv64" ;;
        *) ARCH_DIR="$TARGET" ;;
    esac
    
    echo "📁 Creating directory structure for ${TARGET} (${ARCH_DIR})"
    mkdir -p "kernel_syscalls/${ARCH_DIR}"
    
    # Create the necessary files for the kernel module
    echo "/* Kernel syscall interface for ${TARGET} */" > "kernel_syscalls/${ARCH_DIR}/nymya.h"
    echo "#ifndef NYMYA_KERNEL_H" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"  # Fixed typo
    echo "#define NYMYA_KERNEL_H" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"  # Fixed typo
    echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
    echo "/* Add your kernel syscall definitions here */" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
    echo "" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"
    echo "#endif /* NYMYA_KERNEL_H */" >> "kernel_syscalls/${ARCH_DIR}/nymya.h"  # Fixed typo
    
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
done

# --------------------------
# Build for each target
# --------------------------
echo -e "\n🚀 Starting build for all targets..."
for TARGET in "${TARGETS[@]}"; do
    IMAGE_NAME="nymya-build-env-${TARGET}"
    CROSS_COMPILE=${CROSS_PREFIX[$TARGET]}
    CC="${CROSS_COMPILE}gcc"
    KERNEL_OUT_DIR="/tmp/build-${TARGET}"
    KERNEL_SRC_COPY="/tmp/linux-src-${TARGET}"

    echo -e "\n🐳 Building for target: ${TARGET} using CROSS_COMPILE=${CROSS_COMPILE}"
    echo "🐳 Building docker image for ${TARGET}"
    docker build --platform=${DOCKER_PLATFORM[$TARGET]} -t ${IMAGE_NAME} -f Dockerfile.${TARGET} .

    if [ "$TARGET" = "arm64" ]; then
        KERNEL_SRC="${RPI_KERNEL_SRC}"
    else
        if [ ! -d "/tmp/linux-src" ]; then
            echo "Cloning generic linux kernel..."
            git clone --depth 1 --branch v6.8 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git /tmp/linux-src
        fi
        KERNEL_SRC="/tmp/linux-src"
    fi

    # Create clean kernel source copy for this target
    echo "📁 Creating clean kernel source copy for ${TARGET}"
    rm -rf "${KERNEL_SRC_COPY}"
    cp -r "${KERNEL_SRC}" "${KERNEL_SRC_COPY}"

    # Determine correct ARCH name (riscv vs riscv64)
    if [ "$TARGET" = "riscv64" ]; then
        ARCH_NAME="riscv"
    else
        ARCH_NAME="${TARGET}"
    fi

    # Map target to Debian architecture name
    case "$TARGET" in
        "x86_64") DEB_ARCH="amd64" ;;
        "arm64") DEB_ARCH="arm64" ;;
        "riscv64") DEB_ARCH="riscv64" ;;
        *) DEB_ARCH="$TARGET" ;;
    esac

    docker run --rm --privileged --platform=${DOCKER_PLATFORM[$TARGET]} \
        -w /nymyaOS/nymya-core \
        -v "$(pwd)":/nymyaOS/nymya-core \
        -v "${KERNEL_SRC_COPY}":/nymyaOS/kernel-src:rw \
        "${IMAGE_NAME}" \
        bash -c "cd /nymyaOS/kernel-src && make mrproper && mkdir -p ${KERNEL_OUT_DIR} && make O=${KERNEL_OUT_DIR} ARCH=${ARCH_NAME} CROSS_COMPILE=${CROSS_COMPILE} defconfig && make O=${KERNEL_OUT_DIR} modules_prepare && make O=${KERNEL_OUT_DIR} modules && make O=${KERNEL_OUT_DIR} M=/nymyaOS/nymya-core/kernel_syscalls/${DEB_ARCH} modules && make -C /nymyaOS/nymya-core deb-kernel"
done

# --------------------------
# Copy .deb files from Docker containers
# --------------------------
echo -e "\n📦 Copying .deb files from Docker containers..."
for TARGET in "${TARGETS[@]}"; do
    # Map target architecture to Debian architecture
    case "$TARGET" in
        "x86_64") DEB_ARCH="amd64" ;;
        "arm64") DEB_ARCH="arm64" ;;
        "riscv64") DEB_ARCH="riscv64" ;;
        *) DEB_ARCH="$TARGET" ;;
    esac
    
    # Copy the .deb file
    docker run --rm \
        -v "$(pwd)":/output \
        "nymya-build-env-${TARGET}" \
        bash -c "cp /nymyaOS/nymya-core/nymya-core-kernel_*.deb /output/ 2>/dev/null || true"
done

echo -e "\n🎉 All builds completed. Artifacts:"
ls -lh *.deb || true
