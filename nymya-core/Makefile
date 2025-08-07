# --- Toolchain & flags ---
CC             := gcc
# KERNEL_BUILD_DIR is now dynamically determined per-architecture for kernel modules
CFLAGS         := -Wall -fPIC -O2
LDFLAGS        := -shared

# --- Supported architectures ---
ARCHS          := arm64 x86_64

# --- Cross-compilers per arch ---
CROSS_COMPILE_arm64   := aarch64-linux-gnu-
CROSS_COMPILE_x86_64  :=

# --- Paths & names ---
INCLUDE_DIR    := .
SRC_DIR        := .
KERNEL_SRC_DIR := kernel_syscalls
OBJ_DIR        := build
LIB_NAME       := nymya
LIB_FILE       := lib$(LIB_NAME).so
KERNEL_MODULE  := nymya_syscalls.ko

INSTALL_LIB_DIR     := /usr/lib
INSTALL_INCLUDE_DIR := /usr/include/$(LIB_NAME)
INSTALL_KERNEL_DIR  := /lib/modules/$(shell uname -r)/extra

# --- Package metadata (from version.conf) ---
# NOTE: version.conf is not provided, so we assume these are set
VERSION       ?= 0.1.0
PKG_ARCH      ?= amd64
MAINTAINER    ?= Unknown <>
DESCRIPTION   := NymyaOS Core Syscall SDK

# --- .deb output dirs template ---
DEB_USERLAND_DIR := nymya-core-userland_$(VERSION)_$(PKG_ARCH)
DEB_KERNEL_DIR   := nymya-core-kernel_$(VERSION)_#ARCH#

# --- Userland sources & objects ---
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# -----------------------------------------------------------------------------
# Default: build userland library
# -----------------------------------------------------------------------------
.PHONY: all userland $(OBJ_DIR)
all: userland

userland: $(OBJ_DIR) $(LIB_FILE)

$(OBJ_DIR):
	@chmod -R u+rwX $(OBJ_DIR) || true # Attempt to fix permissions on existing contents before removal
	@rm -rf $(OBJ_DIR) || true # Clean and recreate the build directory to avoid permission issues, allow rm to fail gracefully
	@mkdir -p $(OBJ_DIR)
	@chmod u+rwX $(OBJ_DIR) || true # Ensure writable permissions on the newly created directory

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# -----------------------------------------------------------------------------
# Kernel module sources & objects
# -----------------------------------------------------------------------------
# Get all C files for the kernel module, excluding the ones for userland
KERNEL_SRCS := $(filter-out complex_conj.c complex_exp_i.c complex_im.c complex_mul.c complex_re.c fixed_complex_multiply.c fixed_conj.c fixed_cos.c fixed_point_cos.c fixed_point_sin.c fixed_sin.c, $(wildcard *.c))
KERNEL_OBJS := $(patsubst %.c,%.o,$(KERNEL_SRCS))

# --- Helper to get KERNEL_BUILD_DIR for a given architecture ---
# Standard and most reliable way to get the kernel build directory for external modules.
define GET_KERNEL_BUILD_DIR
  /lib/modules/$(shell uname -r)/build
endef

# -----------------------------------------------------------------------------
# Kernel module build for each architecture
# -----------------------------------------------------------------------------
.PHONY: kernel $(ARCHS)
kernel: $(ARCHS)

$(ARCHS):
	$(eval CCP := $(CROSS_COMPILE_$@))
	$(eval OUT_DIR := $(KERNEL_SRC_DIR)/$@)
	@echo "Building kernel module for $@ with CROSS_COMPILE='$(CCP)'"
	@chmod -R u+rwX $(OUT_DIR) || true # Attempt to fix permissions on existing contents before removal
	@rm -rf $(OUT_DIR) || true # Ensure the architecture-specific directory is completely removed, allow rm to fail gracefully
	@mkdir -p $(OUT_DIR) # Recreate it fresh to avoid permission and stale file issues
	@chmod u+rwX $(OUT_DIR) || true # Ensure writable permissions on the newly created directory
	# Link all required C files into the architecture-specific directory
	@for f in $(KERNEL_SRCS); do ln -sf $(PWD)/$$f $(OUT_DIR)/; done
	# Link the nymya.h header file
	@ln -sf $(PWD)/nymya.h $(OUT_DIR)/
	# Generate the Makefile for the kernel build system
	@printf "obj-m := nymya_syscalls.o\n" > $(OUT_DIR)/Makefile
	@printf "nymya_syscalls-objs := " >> $(OUT_DIR)/Makefile
	@for obj in $(KERNEL_OBJS); do printf "$$obj "; done >> $(OUT_DIR)/Makefile
	@printf "\n" >> $(OUT_DIR)/Makefile

	# Get the correct KERNEL_BUILD_DIR for the target architecture
	$(eval CURRENT_ARCH_KBUILD_DIR := $(call GET_KERNEL_BUILD_DIR,$@))

	@echo "Using KERNEL_BUILD_DIR for $@: $(CURRENT_ARCH_KBUILD_DIR)"
	@$(MAKE) -C $(CURRENT_ARCH_KBUILD_DIR) M=$(PWD)/$(OUT_DIR) ARCH=$@ CROSS_COMPILE=$(CCP) modules

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
.PHONY: clean print-kbuild-dir
clean:
	@echo ">> Cleaning userland artifacts"
	@chmod -R u+rwX $(OBJ_DIR) || true # Attempt to fix permissions before cleaning
	@rm -rf $(OBJ_DIR) $(LIB_FILE) || true
	@echo ">> Cleaning kernel build artifacts"
	@for ARCH in $(ARCHS); do \
	  DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
	  KBUILD_DIR_FOR_CLEAN=$$(/usr/bin/make -s -f $(MAKEFILE_LIST) print-kbuild-dir ARCH=$$ARCH); \
	  if [ -d "$$DIR" ]; then \
	    make -C $$KBUILD_DIR_FOR_CLEAN M=$$PWD/$$DIR ARCH=$$ARCH clean || true; \
	    chmod -R u+rwX $$DIR || true; # Attempt to fix permissions before cleaning
	    rm -rf $$DIR || true; \
	  fi; \
	done

# Helper target to print KERNEL_BUILD_DIR for a given ARCH for use in shell scripts
# This allows shell commands to query the Makefile for the correct KBUILD_DIR
print-kbuild-dir:
	@echo $(call GET_KERNEL_BUILD_DIR,$(ARCH))

# -----------------------------------------------------------------------------
# .deb packaging for userland
# -----------------------------------------------------------------------------
.PHONY: deb-userland
deb-userland: userland
	@echo ">> Creating userland .deb"
	@rm -rf $(DEB_USERLAND_DIR)
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
	  DEB_DIR=$$(echo $(DEB_KERNEL_DIR) | sed "s/#ARCH#/$${ARCH}/"); \
	  MOD_SRC=$(KERNEL_SRC_DIR)/$$ARCH/$(KERNEL_MODULE); \
	  echo "Building $$DEB_DIR.deb..."; \
	  rm -rf $$DEB_DIR; \
	  mkdir -p $$DEB_DIR/DEBIAN $$DEB_DIR/lib/modules/$(shell uname -r)/extra; \
	  if [ -f $$MOD_SRC ]; then \
	    cp $$MOD_SRC $$DEB_DIR/lib/modules/$(shell uname -r)/extra/; \
	    printf "Package: $(LIB_NAME)-core-kernel\nVersion: $(VERSION)\nArchitecture: $${ARCH}\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Kernel for $${ARCH})\n" > $$DEB_DIR/DEBIAN/control; \
	    dpkg-deb --build $$DEB_DIR; \
	    echo "✅ Built $$DEB_DIR.deb"; \
	  else \
	    echo "❌ Missing $$MOD_SRC, skipping"; \
	  fi; \
	done

# -----------------------------------------------------------------------------
# Meta-targets
# -----------------------------------------------------------------------------
.PHONY: deb-all deb
deb-all: deb-userland deb-kernel
deb: deb-all