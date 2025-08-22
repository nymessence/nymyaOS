# -----------------------------------------------------------------------------
# NymyaOS Core Makefile
# -----------------------------------------------------------------------------

# --- Toolchain & flags ---
CC               := gcc
CFLAGS           := -Wall -fPIC -O2
LDFLAGS          := -shared

# --- Supported architectures ---
ARCHS := arm64 x86_64 riscv64

# --- Cross-compilers per arch ---
CROSS_COMPILE_arm64  := aarch64-linux-gnu-
CROSS_COMPILE_x86_64 :=
CROSS_COMPILE_riscv64 := riscv64-linux-gnu-

# --- Paths & names ---
INCLUDE_DIR  := .
SRC_DIR      := .
KERNEL_SRC_DIR := kernel_syscalls
OBJ_DIR      := build
LIB_NAME     := nymya
LIB_FILE     := lib$(LIB_NAME).so
KERNEL_MODULE  := nymya_core.ko

INSTALL_LIB_DIR     := /usr/lib
INSTALL_INCLUDE_DIR := /usr/include/$(LIB_NAME)
INSTALL_KERNEL_DIR  := /lib/modules/$$(uname -r)/extra

# --- Package metadata ---
VERSION      ?= 0.1.0
PKG_ARCH     := $(shell uname -m | sed -e 's/x86_64/amd64/g' -e 's/aarch64/arm64/g')
MAINTAINER   ?= Unknown <unknown@example.com>
DESCRIPTION  := NymyaOS Core Syscall SDK

# --- Userland sources & objects ---
USERLAND_SRCS := $(wildcard *.c)
USERLAND_SRCS := $(filter-out nymya_kernel_module.c nymya_syscalls.c $(wildcard *_test.c), $(USERLAND_SRCS))
OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(USERLAND_SRCS))

# Kernel module sources & objects
KERNEL_SRCS := $(filter-out $(USERLAND_SRCS), $(wildcard *.c))
KERNEL_OBJS := $(patsubst %.c,%.o,$(KERNEL_SRCS))

# -----------------------------------------------------------------------------
# Default target
# -----------------------------------------------------------------------------
.PHONY: all userland kernel deb-all clean install
all: userland

# -----------------------------------------------------------------------------
# Userland build
# -----------------------------------------------------------------------------
userland: $(OBJ_DIR) $(LIB_FILE)

$(OBJ_DIR):
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# -----------------------------------------------------------------------------
# Kernel module build for each architecture
# -----------------------------------------------------------------------------
.PHONY: kernel $(ARCHS)
kernel: $(ARCHS)

$(ARCHS):
	$(eval CCP := $(CROSS_COMPILE_$@))
	$(eval OUT_DIR := $(KERNEL_SRC_DIR)/$@)
	@echo "🐳 Building kernel module for $@ with CROSS_COMPILE='$(CCP)'"

	# Verify kernel headers are installed
	@if [ ! -d "/lib/modules/$$(uname -r)/build" ]; then \
		echo "❌ Kernel headers not found. Install with:"; \
		echo "   sudo apt install linux-headers-$$(uname -r)"; \
		exit 1; \
	fi

	# Prepare output directory
	@rm -rf $(OUT_DIR)
	@mkdir -p $(OUT_DIR)
	@for f in $(KERNEL_SRCS); do cp $$f $(OUT_DIR)/; done
	@cp nymya.h $(OUT_DIR)/

	# Generate kernel Makefile (NO deb-all or custom targets)
	@echo "obj-m := nymya_core.o" > $(OUT_DIR)/Makefile
	@echo "nymya_core-objs := $(notdir $(KERNEL_OBJS))" >> $(OUT_DIR)/Makefile
	@echo "all:" >> $(OUT_DIR)/Makefile
	@printf '\t$$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(PWD) ARCH=$@ CROSS_COMPILE=$(CCP) modules\n' >> $(OUT_DIR)/Makefile
	@echo "clean:" >> $(OUT_DIR)/Makefile
	@printf '\t$$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$(PWD) ARCH=$@ CROSS_COMPILE=$(CCP) clean\n' >> $(OUT_DIR)/Makefile

	# Build module with non-interactive config
	@cd $(OUT_DIR) && \
		$(MAKE) -C /lib/modules/$$(uname -r)/build ARCH=$@ olddefconfig && \
		$(MAKE) -j$(nproc)

# -----------------------------------------------------------------------------
# .deb packaging (SEPARATE FROM KERNEL BUILD)
# -----------------------------------------------------------------------------
.PHONY: deb-all
deb-all: deb-userland deb-kernel

DEB_USERLAND_DIR := nymya-core-userland_$(VERSION)_$(PKG_ARCH)
DEB_KERNEL_DIR   := nymya-core-kernel_$(VERSION)_#ARCH#

deb-userland: userland
	@echo "📦 Creating userland .deb"
	@rm -rf $(DEB_USERLAND_DIR)
	@mkdir -p $(DEB_USERLAND_DIR)/DEBIAN \
		$(DEB_USERLAND_DIR)/usr/lib \
		$(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)
	@cp $(LIB_FILE) $(DEB_USERLAND_DIR)/usr/lib/
	@cp nymya.h $(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)/
	@printf "Package: $(LIB_NAME)-core-userland\nVersion: $(VERSION)\nArchitecture: $(PKG_ARCH)\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Userland)\n" > $(DEB_USERLAND_DIR)/DEBIAN/control
	@dpkg-deb --build $(DEB_USERLAND_DIR) 2>/dev/null || \
		{ echo "❌ Failed to build userland .deb"; exit 1; }
	@echo "✅ Built $(DEB_USERLAND_DIR).deb"

deb-kernel: kernel
	@echo "📦 Creating kernel .deb for each architecture"
	@for ARCH in $(ARCHS); do \
		DEB_DIR=$$(echo $(DEB_KERNEL_DIR) | sed "s/#ARCH#/$$ARCH/"); \
		MOD_SRC=$(KERNEL_SRC_DIR)/$$ARCH/$(KERNEL_MODULE); \
		rm -rf $$DEB_DIR; \
		mkdir -p $$DEB_DIR/DEBIAN; \
		if [ -f "$$MOD_SRC" ]; then \
			KERNEL_RELEASE=$$(cd /lib/modules/$$(uname -r)/build && $(MAKE) -s ARCH=$$ARCH kernelrelease); \
			mkdir -p $$DEB_DIR/lib/modules/$$KERNEL_RELEASE/extra; \
			cp $$MOD_SRC $$DEB_DIR/lib/modules/$$KERNEL_RELEASE/extra/; \
			printf "Package: $(LIB_NAME)-core-kernel\nVersion: $(VERSION)\nArchitecture: $$ARCH\nMaintainer: $(MAINTAINER)\nDepends: linux-image-$$KERNEL_RELEASE\nDescription: $(DESCRIPTION) (Kernel for $$ARCH)\n" > $$DEB_DIR/DEBIAN/control; \
			dpkg-deb --build $$DEB_DIR 2>/dev/null || \
				{ echo "❌ Failed to build $$DEB_DIR.deb"; exit 1; }; \
			echo "✅ Built $$DEB_DIR.deb"; \
		else \
			echo "❌ Missing $$MOD_SRC, skipping"; \
		fi; \
	done

# -----------------------------------------------------------------------------
# Install & Uninstall
# -----------------------------------------------------------------------------
.PHONY: install
install: userland deb-kernel
	@echo "📦 Installing userland library"
	@sudo mkdir -p $(INSTALL_LIB_DIR)
	@sudo cp $(LIB_FILE) $(INSTALL_LIB_DIR)/
	@sudo mkdir -p $(INSTALL_INCLUDE_DIR)
	@sudo cp nymya.h $(INSTALL_INCLUDE_DIR)/
	@echo "📦 Installing kernel modules via .deb packages"
	@for ARCH in $(ARCHS); do \
		DEB_DIR=nymya-core-kernel_$(VERSION)_$$ARCH.deb; \
		if [ -f "$$DEB_DIR" ]; then \
			sudo dpkg -i $$DEB_DIR; \
		else \
			echo "⚠️  $$DEB_DIR not found, run 'make deb-kernel' first"; \
		fi; \
	done
	@sudo depmod -a

.PHONY: uninstall
uninstall:
	@echo "🧹 Removing userland components"
	@sudo rm -f $(INSTALL_LIB_DIR)/$(LIB_FILE)
	@sudo rm -f $(INSTALL_INCLUDE_DIR)/nymya.h
	@echo "🧹 Removing kernel module package"
	@sudo dpkg --remove nymya-core-kernel-module 2>/dev/null || true
	@sudo depmod -a

# -----------------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------------
.PHONY: clean
clean:
	@echo "🧹 Cleaning build artifacts"
	@rm -rf $(OBJ_DIR) $(LIB_FILE)
	@for ARCH in $(ARCHS); do \
		DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
		if [ -d "$$DIR" ]; then \
			$(MAKE) -C /lib/modules/$$(uname -r)/build M=$$DIR ARCH=$$ARCH clean 2>/dev/null || true; \
			rm -rf $$DIR; \
		fi; \
	done
	@rm -rf $(DEB_USERLAND_DIR).deb $(DEB_KERNEL_DIR)*.deb
	@rm -rf nymya-core-userland_*.deb nymya-core-kernel_*.deb
	@rm -rf pkg/  # in case you add it later
	@echo "✅ Clean complete"

# -----------------------------------------------------------------------------
# Convenience targets
# -----------------------------------------------------------------------------
.PHONY: rebuild
rebuild: clean all kernel deb-all

.PHONY: pkg
pkg: deb-all
