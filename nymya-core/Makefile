# --- Toolchain & flags ---
CC             := gcc
KERNEL_BUILD_DIR := /lib/modules/$(shell uname -r)/build
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
.PHONY: all userland
all: userland

userland: $(LIB_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# -----------------------------------------------------------------------------
# Kernel module sources & objects
# -----------------------------------------------------------------------------
# Filter out userland-only or generated files from kernel build
KERNEL_SRCS := $(filter-out complex_conj.c complex_exp_i.c complex_im.c complex_mul.c complex_re.c fixed_complex_multiply.c fixed_conj.c fixed_cos.c fixed_point_cos.c fixed_point_sin.c fixed_sin.c, $(wildcard $(KERNEL_SRC_DIR)/*.c))
KERNEL_OBJS := $(patsubst %.c,%.o,$(notdir $(KERNEL_SRCS)))

# -----------------------------------------------------------------------------
# Kernel module build for each architecture
# -----------------------------------------------------------------------------
.PHONY: kernel $(ARCHS)
kernel: $(ARCHS)

$(ARCHS):
	$(eval CCP := $(CROSS_COMPILE_$@))
	$(eval OUT_DIR := $(abspath $(KERNEL_SRC_DIR)/$@))
	@echo "Building kernel module for $@ with CROSS_COMPILE='$(CCP)'"
	@mkdir -p $(OUT_DIR)
	@rm -f $(OUT_DIR)/*.c $(OUT_DIR)/*.h
	# Link all required C files into the architecture-specific directory using absolute paths
	@for f in $(KERNEL_SRCS); do \
	  ln -sf $(abspath $$f) $(OUT_DIR)/; \
	done
	# Link the nymya.h header file using absolute path
	@ln -sf $(abspath nymya.h) $(OUT_DIR)/
	# Generate the Makefile for the kernel build system
	@printf "obj-m := nymya_syscalls.o\n" > $(OUT_DIR)/Makefile
	@printf "nymya_syscalls-objs := " >> $(OUT_DIR)/Makefile
	@for obj in $(KERNEL_OBJS); do printf "$$obj "; done >> $(OUT_DIR)/Makefile
	@printf "\n" >> $(OUT_DIR)/Makefile
	@$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(OUT_DIR) ARCH=$@ CROSS_COMPILE=$(CCP) modules

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
.PHONY: clean
clean:
	@echo ">> Cleaning userland artifacts"
	@rm -rf $(OBJ_DIR) $(LIB_FILE)
	@echo ">> Cleaning kernel build artifacts"
	@for ARCH in $(ARCHS); do \
	  DIR=$(KERNEL_SRC_DIR)/$$ARCH; \
	  if [ -d "$$DIR" ]; then \
	    make -C $(KERNEL_BUILD_DIR) M=$$DIR ARCH=$$ARCH clean || true; \
	    rm -rf $$DIR; \
	  fi; \
	done

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
