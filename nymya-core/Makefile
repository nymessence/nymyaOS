KERNEL_HEADERS := /usr/src/linux-headers-6.8.0-1031-raspi

ARCH_arm64 := arm64
CROSS_COMPILE_arm64 := aarch64-linux-gnu-

ARCH_x86_64 := x86_64
CROSS_COMPILE_x86_64 :=

KS_DIR_arm64 := kernel_syscalls/$(ARCH_arm64)
KS_DIR_x86_64 := kernel_syscalls/$(ARCH_x86_64)

# Get all .c sources except main syscall file
SOURCES := $(filter-out nymya_syscalls.c, $(wildcard *.c))

.PHONY: all clean clean_dir prepare userland kernel_arm64 kernel_x86_64 deb

all: userland kernel_arm64 kernel_x86_64 deb

# Prepare directory and links for a given arch
prepare_%:
	mkdir -p $(KS_DIR_$*)
	@for src in $(SOURCES) nymya_syscalls.c; do \
		ln -sf $(abspath $$src) $(KS_DIR_$*)/; \
	done
	ln -sf $(abspath nymya.h) $(KS_DIR_$*)/

	@echo "obj-m := nymya_syscalls.o" > $(KS_DIR_$*)/Makefile
	@echo -n "nymya_syscalls-objs := " >> $(KS_DIR_$*)/Makefile
	@for src in $(SOURCES); do \
		echo -n "$$(basename $$src .c).o " >> $(KS_DIR_$*)/Makefile; \
	done
	@echo >> $(KS_DIR_$*)/Makefile

# Build userland shared library
userland:
	@echo "Building userland..."
	@mkdir -p build
	gcc -Wall -fPIC -O2 -I. -c *.c -o build/
	gcc -shared -o libnymya.so build/*.o

# Build kernel module for arm64
kernel_arm64: prepare_arm64
	@echo "Building kernel module for arm64"
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_arm64)) ARCH=$(ARCH_arm64) CROSS_COMPILE=$(CROSS_COMPILE_arm64) modules

# Build kernel module for x86_64 native
kernel_x86_64: prepare_x86_64
	@echo "Building kernel module for x86_64"
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_x86_64)) ARCH=$(ARCH_x86_64) CROSS_COMPILE=$(CROSS_COMPILE_x86_64) modules

# Create deb packages placeholder
deb:
	@echo "Creating .deb packages (implement as needed)..."

clean:
	@echo "Cleaning kernel build artifacts for arm64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_arm64)) clean || true
	@echo "Cleaning kernel build artifacts for x86_64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_x86_64)) clean || true
	@echo "Cleaning userland build artifacts..."
	rm -rf build/*.o libnymya.so

clean_dir:
	@echo "Cleaning symlinks and generated files..."
	rm -rf $(KS_DIR_arm64) $(KS_DIR_x86_64)

