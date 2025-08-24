#!/usr/bin/env bash
set -e

# Clone kernel source if it doesn't exist
if [ ! -d "linux-kernel-src" ]; then
    echo "Cloning linux kernel..."
    git clone --depth 1 --branch v6.8 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git linux-kernel-src
fi

echo "Building Docker images..."
docker build --platform linux/amd64 -t nymyaos-x86_64 -f Dockerfile.x86_64 .
docker build --platform linux/arm64 -t nymyaos-arm64 -f Dockerfile.arm64 .
docker build --platform linux/riscv64 -t nymyaos-riscv64 -f Dockerfile.riscv64 .

echo "Cross-compiling nymya-core..."
docker run --rm --platform linux/amd64 -v $(pwd):/nymyaOS -v $(pwd)/linux-kernel-src:/kernel-src --user root nymyaos-x86_64 /bin/bash -c "cd /nymyaOS/nymya-core && ./compile_x86_64.sh"
docker run --rm --platform linux/arm64 -v $(pwd):/nymyaOS -v $(pwd)/linux-kernel-src:/kernel-src --user root nymyaos-arm64 /bin/bash -c "cd /nymyaOS/nymya-core && ./compile_arm64_generic.sh"
docker run --rm --platform linux/riscv64 -v $(pwd):/nymyaOS -v $(pwd)/linux-kernel-src:/kernel-src --user root nymyaos-riscv64 /bin/bash -c "cd /nymyaOS/nymya-core && ./compile_riscv64.sh"

echo "Cross-compiling nymya_runtime..."
docker run --rm --platform linux/amd64 -v $(pwd):/nymyaOS --user root nymyaos-x86_64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a amd64"
docker run --rm --platform linux/arm64 -v $(pwd):/nymyaOS --user root nymyaos-arm64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a arm64"
docker run --rm --platform linux/riscv64 -v $(pwd):/nymyaOS --user root nymyaos-riscv64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a riscv64"