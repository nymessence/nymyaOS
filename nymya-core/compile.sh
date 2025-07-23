#!/bin/bash
export CROSS_COMPILE_arm64=aarch64-linux-gnu4

make prepare            # Creates symlink
make ARCH=x86_64        # Default build for amd64 userland
make ARCH=arm64         # Cross-compile for ARM64 userland
make ARCH=x86_64 kernel # Build amd64 kernel module
make ARCH=arm64 kernel  # Build arm64 kernel module
make deb                # Build all 4 .debs
make clean_dir          # Cleanup build files
