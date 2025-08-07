# Define the build directories.
X86_64_BUILD_DIR := $(shell pwd)/kernel_syscalls/x86_64
ARM64_BUILD_DIR := $(shell pwd)/kernel_syscalls/arm64
KDIR := /lib/modules/$(shell uname -r)/build

# A simple target to clean up build artifacts.
.PHONY: clean
clean:
	@echo "Cleaning up..."
	rm -rf $(X86_64_BUILD_DIR) $(ARM64_BUILD_DIR) build libnymya.so

# This target prepares the directories and creates the sub-Makefiles.
.PHONY: prepare
prepare:
	@echo "Preparing build directories and sub-Makefiles..."
	mkdir -p $(X86_64_BUILD_DIR) $(ARM64_BUILD_DIR)
	# Create sub-Makefiles with obj-m directive for the kernel build system.
	@echo "obj-m := nymya_syscalls.o" > $(X86_64_BUILD_DIR)/Makefile
	@echo "obj-m := nymya_syscalls.o" > $(ARM64_BUILD_DIR)/Makefile
	@echo "All sub-Makefiles created."

# Target for the x86_64 kernel module.
# The `make -C` command now uses the dynamic KDIR.
.PHONY: kernel_x86_64
kernel_x86_64: prepare
	@echo "Building kernel module for x86_64..."
	$(MAKE) -C $(KDIR) M=$(X86_64_BUILD_DIR) ARCH=x86_64 modules

# Target for the ARM64 kernel module.
# The `make -C` command now uses the dynamic KDIR and a cross-compiler.
.PHONY: kernel_arm64
kernel_arm64: prepare
	@echo "Building kernel module for ARM64..."
	$(MAKE) -C $(KDIR) M=$(ARM64_BUILD_DIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

# Target for userland code.
.PHONY: userland
userland:
	@echo "Building userland code..."
	mkdir -p build
	# We need to compile the C file into an object file first.
	gcc -c -fPIC -o build/nymya_syscalls.o nymya_syscalls.c
	# Then we link the object file to create the shared library.
	gcc -shared -o libnymya.so build/*.o

# Target for creating .deb packages.
.PHONY: deb
deb:
	@echo "Creating .deb packages (implement as needed)..."

# Default target for a full build.
.PHONY: all
all: clean prepare userland kernel_x86_64 kernel_arm64 deb


