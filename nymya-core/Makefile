# --- Toolchain & flags ---
CC               := gcc
CFLAGS           := -Wall -fPIC -O2
LDFLAGS          := -shared

# --- Supported architectures ---
ARCHS            := arm64 x86_64

# --- Cross-compilers per arch ---
CROSS_COMPILE_arm64  := aarch64-linux-gnu-
CROSS_COMPILE_x86_64 :=

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
INSTALL_KERNEL_DIR  := /lib/modules/$(shell uname -r)/extra

# --- Package metadata (from version.conf) ---
VERSION      ?= 0.1.0
PKG_ARCH     := $(shell uname -m | sed -e 's/x86_64/amd64/g' -e 's/aarch64/arm64/g')
MAINTAINER   ?= Unknown <>
DESCRIPTION  := NymyaOS Core Syscall SDK

# --- .deb output dirs template ---
DEB_USERLAND_DIR := nymya-core-userland_$(VERSION)_$(PKG_ARCH)
DEB_KERNEL_DIR   := nymya-core-kernel_$(VERSION)_#ARCH#

# --- Userland sources & objects ---
# Get all C files
USERLAND_SRCS := $(wildcard *.c)
# IMPORTANT: Explicitly filter out the kernel module source and any test files.
USERLAND_SRCS := $(filter-out nymya_kernel_module.c nymya_syscalls.c $(wildcard *_test.c), $(USERLAND_SRCS))
OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(USERLAND_SRCS))

# --- Kernel module sources & objects ---
# Kernel sources are now correctly identified as the files NOT in the userland list.
KERNEL_SRCS := $(filter-out $(USERLAND_SRCS), $(wildcard *.c))
KERNEL_OBJS := $(patsubst %.c,%.o,$(KERNEL_SRCS))

# --- Set the kernel build directory (can be overridden by command line) ---
KBUILD_DIR ?= /lib/modules/$(shell uname -r)/build

# -----------------------------------------------------------------------------
# Default: build userland library
# -----------------------------------------------------------------------------
.PHONY: all userland $(OBJ_DIR) prepare deb-all
all: userland

# New target to build all .deb packages
deb-all: deb-userland deb-kernel

userland: $(OBJ_DIR) $(LIB_FILE)

$(OBJ_DIR):
	@rm -rf $(OBJ_DIR) || sudo rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

prepare:
	@echo ">> Preparing build directories by cleaning and recreating..."
	@rm -rf $(OBJ_DIR) || sudo rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	@for ARCH in $(ARCHS); do \
	  OUT_DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
	  echo "  - Cleaning and creating $$OUT_DIR for $$ARCH"; \
	  rm -rf $$OUT_DIR || sudo rm -rf $$OUT_DIR; \
	  mkdir -p $$OUT_DIR; \
	done
	@echo "✅ All build directories prepared."

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
	@echo "Building kernel module for $@ with CROSS_COMPILE='$(CCP)'"
	@rm -rf $(OUT_DIR) || sudo rm -rf $(OUT_DIR)
	@mkdir -p $(OUT_DIR)
	@for f in $(KERNEL_SRCS); do cp $$f $(OUT_DIR)/; done
	@cp nymya.h $(OUT_DIR)/
	@echo "obj-m := nymya_core.o" > $(OUT_DIR)/Makefile
	@echo "nymya_core-objs := $(notdir $(KERNEL_OBJS))" >> $(OUT_DIR)/Makefile
	@echo "" >> $(OUT_DIR)/Makefile
	@echo "KDIR := /usr/src/linux-headers-6.8.0-1031-raspi" >> $(OUT_DIR)/Makefile
	@echo "PWD := $(shell pwd)/$(OUT_DIR)" >> $(OUT_DIR)/Makefile
	@echo "" >> $(OUT_DIR)/Makefile
	@echo "all:" >> $(OUT_DIR)/Makefile
	@printf '\t$$(MAKE) -C $$(KDIR) M=$$(PWD) modules\n' >> $(OUT_DIR)/Makefile
	@echo "" >> $(OUT_DIR)/Makefile
	@echo "clean:" >> $(OUT_DIR)/Makefile
	@printf '\t$$(MAKE) -C $$(KDIR) M=$$(PWD) clean\n' >> $(OUT_DIR)/Makefile
	@cd $(OUT_DIR) && $(MAKE) -j$(nproc) \
	        CROSS_COMPILE=$(CCP) \
	        CC=$(CCP)gcc \
	        KBUILD_NOPEDANTIC=1

# -----------------------------------------------------------------------------
# Install & uninstall targets
# -----------------------------------------------------------------------------
.PHONY: install uninstall
install: userland kernel
	@echo ">> Installing userland library to $(INSTALL_LIB_DIR)"
	@mkdir -p $(INSTALL_LIB_DIR)
	@cp $(LIB_FILE) $(INSTALL_LIB_DIR)/
	@echo ">> Installing headers to $(INSTALL_INCLUDE_DIR)"
	@mkdir -p $(INSTALL_INCLUDE_DIR)
	@cp nymya.h $(INSTALL_INCLUDE_DIR)/
	@for ARCH in $(ARCHS); do \
	  MOD=$(KERNEL_SRC_DIR)/$$ARCH/$(KERNEL_MODULE); \
	  if [ -f $$MOD ]; then \
	    echo ">> Installing $$ARCH kernel module"; \
	    mkdir -p $(INSTALL_KERNEL_DIR); \
	    cp $$MOD $(INSTALL_KERNEL_DIR)/; \
	  else \
	    echo ">> $$ARCH kernel module not built, skipping"; \
	  fi; \
	done

uninstall:
	@echo ">> Uninstalling userland library"
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_FILE)
	@echo ">> Removing headers"
	@rm -f $(INSTALL_INCLUDE_DIR)/nymya.h
	@echo ">> Removing kernel modules"
	@rm -f $(INSTALL_KERNEL_DIR)/$(KERNEL_MODULE)
	@depmod -a

# -----------------------------------------------------------------------------
# Cleaning build artifacts
# -----------------------------------------------------------------------------
.PHONY: clean print-kbuild-dir clean_dir
clean:
	@echo ">> Cleaning userland artifacts"
	@rm -rf $(OBJ_DIR) $(LIB_FILE) || sudo rm -rf $(OBJ_DIR) $(LIB_FILE)
	@echo ">> Cleaning kernel build artifacts"
	@for ARCH in $(ARCHS); do \
	  DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
	  if [ -d "$$DIR" ]; then \
	    $(MAKE) -C $(KBUILD_DIR) M=$(PWD)/$$DIR ARCH=$$ARCH clean || true; \
	    rm -rf $$DIR || sudo rm -rf $$DIR; \
	  fi; \
	done

clean_dir:
	@echo ">> Cleaning build directories only..."
	@rm -rf $(OBJ_DIR) || sudo rm -rf $(OBJ_DIR)
	@for ARCH in $(ARCHS); do \
	  DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
	  rm -rf $$DIR || sudo rm -rf $$DIR; \
	done
	@echo "✅ All build directories removed."

# -----------------------------------------------------------------------------
# .deb packaging for userland
# -----------------------------------------------------------------------------
.PHONY: deb-userland
deb-userland: userland
	@echo ">> Creating userland .deb"
	@rm -rf $(DEB_USERLAND_DIR) || sudo rm -rf $(DEB_USERLAND_DIR)
	@mkdir -p $(DEB_USERLAND_DIR)/DEBIAN \
	           $(DEB_USERLAND_DIR)/usr/lib \
	           $(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)
	@cp $(LIB_FILE) $(DEB_USERLAND_DIR)/usr/lib/
	@cp nymya.h    $(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)/
	@printf "Package: $(LIB_NAME)-core-userland\nVersion: $(VERSION)\nArchitecture: $(PKG_ARCH)\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Userland)\n" > $(DEB_USERLAND_DIR)/DEBIAN/control
	@dpkg-deb --build $(DEB_USERLAND_DIR)
	@echo "✅ Built $(DEB_USERLAND_DIR).deb"

# -----------------------------------------------------------------------------
# .deb packaging for kernel module
# -----------------------------------------------------------------------------
.PHONY: deb-kernel
deb-kernel: kernel
	@echo ">> Creating kernel .deb for each architecture"
	@for ARCH in $(ARCHS); do \
	  DEB_DIR=$$(echo $(DEB_KERNEL_DIR) | sed "s/#ARCH#/$$ARCH/"); \
	  MOD_SRC=$(KERNEL_SRC_DIR)/$$ARCH/$(KERNEL_MODULE); \
	  rm -rf $$DEB_DIR || sudo rm -rf $$DEB_DIR; \
	  mkdir -p $$DEB_DIR/DEBIAN $$DEB_DIR/lib/modules/$(shell uname -r)/extra; \
	  if [ -f $$MOD_SRC ]; then \
	    cp $$MOD_SRC $$DEB_DIR/lib/modules/$(shell uname -r)/extra/; \
	    printf "Package: $(LIB_NAME)-core-kernel\nVersion: $(VERSION)\nArchitecture: $$ARCH\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Kernel for $$ARCH)\n" > $$DEB_DIR/DEBIAN/control; \
	    dpkg-deb --build $$DEB_DIR; \
	    echo "✅ Built $$DEB_DIR.deb"; \
	  else \
	    echo "❌ Missing $$MOD_SRC, skipping"; \
	  fi; \
	done
