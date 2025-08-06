KERNEL_VERSION := $(shell uname -r)
KERNEL_HEADERS := /usr/src/linux-headers-$(KERNEL_VERSION)

ARCH_ARM64 := arm64
ARCH_X86_64 := x86_64

CROSS_ARM64 := aarch64-linux-gnu-
CROSS_X86_64 := gcc

KS_DIR_ARM64 := kernel_syscalls/$(ARCH_ARM64)
KS_DIR_X86_64 := kernel_syscalls/$(ARCH_X86_64)

SOURCES := $(filter-out nymya_syscalls.c, $(wildcard *.c))

.PHONY: all clean clean_dir prepare userland kernel kernel_arm64 kernel_x86_64 deb

all: userland kernel deb

prepare:
	@echo "Preparing kernel syscall directories..."
	@mkdir -p $(KS_DIR_ARM64) $(KS_DIR_X86_64)

	# ARM64
	@for src in $(wildcard *.c) nymya.h; do \
		ln -sf $(abspath $$src) $(KS_DIR_ARM64)/; \
	done
	@echo "obj-m := nymya_syscalls.o" > $(KS_DIR_ARM64)/Makefile
	@echo -n "nymya_syscalls-objs := " >> $(KS_DIR_ARM64)/Makefile
	@for src in $(wildcard *.c); do \
		echo -n "$$(basename $$src .c).o " >> $(KS_DIR_ARM64)/Makefile; \
	done
	@echo "" >> $(KS_DIR_ARM64)/Makefile

	# x86_64
	@for src in $(wildcard *.c) nymya.h; do \
		ln -sf $(abspath $$src) $(KS_DIR_X86_64)/; \
	done
	@echo "obj-m := nymya_syscalls.o" > $(KS_DIR_X86_64)/Makefile
	@echo -n "nymya_syscalls-objs := " >> $(KS_DIR_X86_64)/Makefile
	@for src in $(wildcard *.c); do \
		echo -n "$$(basename $$src .c).o " >> $(KS_DIR_X86_64)/Makefile; \
	done
	@echo "" >> $(KS_DIR_X86_64)/Makefile
	
userland:
	@echo "Building userland..."
	@mkdir -p build
	@for src in *.c; do \
		obj="build/$$(basename $$src .c).o"; \
		echo "Compiling $$src -> $$obj"; \
		gcc -Wall -fPIC -O2 -I. -c $$src -o $$obj; \
	done
	gcc -shared -o libnymya.so build/*.o

kernel_arm64: prepare
	@echo "Building kernel module for ARM64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_ARM64)) ARCH=$(ARCH_ARM64) CROSS_COMPILE=$(CROSS_ARM64) modules

kernel_x86_64: prepare
	@echo "Building kernel module for x86_64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_X86_64)) ARCH=$(ARCH_X86_64) CROSS_COMPILE=$(CROSS_X86_64) modules

kernel: kernel_arm64 kernel_x86_64

deb:
	@echo "Creating .deb packages (implement as needed)..."

clean:
	@echo "Cleaning kernel build artifacts for ARM64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_ARM64)) clean || true
	@echo "Cleaning kernel build artifacts for x86_64..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR_X86_64)) clean || true
	@echo "Cleaning userland build artifacts..."
	rm -rf build/*.o libnymya.so

clean_dir:
	@echo "Cleaning symlinks and generated files..."
	rm -rf $(KS_DIR_ARM64) $(KS_DIR_X86_64) build libnymya.so

