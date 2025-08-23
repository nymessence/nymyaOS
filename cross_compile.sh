#!/usr/bin/env bash
set -e

echo "Building Docker images..."
docker build -t nymyaos-x86_64 -f Dockerfile.x86_64 .
docker build -t nymyaos-arm64 -f Dockerfile.arm64 .
docker build -t nymyaos-riscv64 -f Dockerfile.riscv64 .

echo "Cross-compiling nymya-core..."
docker run --rm -v $(pwd):/nymyaOS nymyaos-x86_64 /bin/bash -c "cd nymya-core && ./compile_x86_64.sh"
docker run --rm -v $(pwd):/nymyaOS nymyaos-arm64 /bin/bash -c "cd nymya-core && ./compile_arm64_generic.sh"
docker run --rm -v $(pwd):/nymyaOS nymyaos-riscv64 /bin/bash -c "cd nymya-core && ./compile_riscv64.sh"

echo "Cross-compiling nymya_runtime..."
docker run --rm -v $(pwd):/nymyaOS nymyaos-x86_64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a amd64"
docker run --rm -v $(pwd):/nymyaOS nymyaos-arm64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a arm64"
docker run --rm -v $(pwd):/nymyaOS nymyaos-riscv64 /bin/bash -c "cd nymya_runtime && dpkg-buildpackage -us -uc -b -a riscv64"
