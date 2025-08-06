#!/bin/bash

export CROSS_COMPILE_arm64=aarch64-linux-gnu-

make prepare            # Creates arch directories and Makefiles first
make clean              # Clean build artifacts safely

make ARCH=x86_64        # Build amd64 userland
make ARCH=arm64         # Cross-compile ARM64 userland

make ARCH=x86_64 kernel # Build amd64 kernel module
make ARCH=arm64 kernel  # Build ARM64 kernel module

make deb                # Build all .deb packages

# Optional: remove if not defined in Makefile
# make clean_dir       # Only if this target exists

