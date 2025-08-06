KERNEL_HEADERS := /usr/src/linux-headers-6.8.0-1031-raspi
ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-
KS_DIR := kernel_syscalls/$(ARCH)

# Get all .c sources except the main nymya_syscalls.c
SOURCES := $(filter-out nymya_syscalls.c, $(wildcard *.c))

.PHONY: all clean clean_dir prepare userland kernel deb

all: userland kernel deb

# Prepare kernel_syscalls/arm64 directory
prepare:
	mkdir -p $(KS_DIR)
	# Link all source files there
	@for src in $(SOURCES) nymya_syscalls.c; do \
		ln -sf $(abspath $$src) $(KS_DIR)/; \
	done
	ln -sf $(abspath nymya.h) $(KS_DIR)/

	# Generate kernel Makefile
	@echo "obj-m := nymya_syscalls.o" > $(KS_DIR)/Makefile
	@echo -n "nymya_syscalls-objs := " >> $(KS_DIR)/Makefile
	@for src in $(SOURCES); do \
		echo -n "$$(basename $$src .c).o " >> $(KS_DIR)/Makefile; \
	done
	@echo >> $(KS_DIR)/Makefile

# Build userland shared library
userland:
	@echo "Building userland..."
	gcc -Wall -fPIC -O2 -I. -c *.c -o build/
	gcc -shared -o libnymya.so build/*.o

# Build kernel module for arm64
kernel: prepare
	@echo "Building kernel module for $(ARCH) with CROSS_COMPILE='$(CROSS_COMPILE)'"
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR)) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

# Create deb packages (dummy target, adjust as needed)
deb:
	@echo "Creating .deb packages (implement as needed)..."

# Clean build artifacts
clean:
	@echo "Cleaning kernel build artifacts..."
	make -C $(KERNEL_HEADERS) M=$(abspath $(KS_DIR)) clean || true
	@echo "Cleaning userland build artifacts..."
	rm -rf build/*.o libnymya.so

clean_dir:
	@echo "Cleaning symlinks and generated files in $(KS_DIR)..."
	rm -rf $(KS_DIR)

