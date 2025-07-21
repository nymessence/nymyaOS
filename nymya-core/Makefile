include ../version.conf

# --- Toolchain & flags ---
CC               := gcc
KERNEL_BUILD_DIR := /lib/modules/$(shell uname -r)/build
CFLAGS           := -Wall -fPIC -O2
LDFLAGS          := -shared

# --- Paths & names ---
INCLUDE_DIR      := .
SRC_DIR          := .
KERNEL_SRC_DIR   := kernel_syscalls
OBJ_DIR          := build
LIB_NAME         := nymya
LIB_FILE         := lib$(LIB_NAME).so
KERNEL_MODULE    := nymya_syscalls.ko

INSTALL_LIB_DIR     := /usr/lib
INSTALL_INCLUDE_DIR := /usr/include/$(LIB_NAME)
INSTALL_KERNEL_DIR  := /lib/modules/$(shell uname -r)/extra

# --- Package metadata (from version.conf) ---
VERSION     ?= 0.0.0
ARCH        ?= amd64
MAINTAINER  ?= Unknown <>
DESCRIPTION := NymyaOS Core Syscall SDK

# --- .deb output dirs ---
DEB_USERLAND_DIR := nymya-core-userland_$(VERSION)_$(ARCH)
DEB_KERNEL_DIR   := nymya-core-kernel_$(VERSION)_$(ARCH)

# --- Userland sources & objects ---
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# -----------------------------------------------------------------------------
# Default target: build userland library
# -----------------------------------------------------------------------------
.PHONY: all
all: $(LIB_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# -----------------------------------------------------------------------------
# prepare: old prepare behavior
# -----------------------------------------------------------------------------
.PHONY: prepare
prepare:
	@mkdir -p $(INCLUDE_DIR)
	@ln -sf ./nymya.h $(INCLUDE_DIR)/nymya.h

# -----------------------------------------------------------------------------
# Kernel module build — sync sources & header, then auto-generate Kbuild
# -----------------------------------------------------------------------------
.PHONY: kernel
kernel:
	@echo ">> Syncing sources to $(KERNEL_SRC_DIR)"
	@mkdir -p $(KERNEL_SRC_DIR)
	@rm -f $(KERNEL_SRC_DIR)/*.c $(KERNEL_SRC_DIR)/nymya.h
	@for f in $(wildcard nymya_*.c); do \
	  ln -sf $(PWD)/$$f $(KERNEL_SRC_DIR)/; \
	done
	@ln -sf $(PWD)/nymya.h $(KERNEL_SRC_DIR)/nymya.h

	@echo ">> Generating $(KERNEL_SRC_DIR)/Makefile"
	@printf "obj-m := nymya_syscalls.o\n" > $(KERNEL_SRC_DIR)/Makefile
	@printf "nymya_syscalls-objs := "   >> $(KERNEL_SRC_DIR)/Makefile
	@for src in $(wildcard nymya_*.c); do \
	  base=$$(basename $$src .c); \
	  printf "$$base.o " >> $(KERNEL_SRC_DIR)/Makefile; \
	done
	@printf "\n" >> $(KERNEL_SRC_DIR)/Makefile

	@echo ">> Building kernel module"
	@$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(PWD)/$(KERNEL_SRC_DIR) modules

# -----------------------------------------------------------------------------
# Install & uninstall
# -----------------------------------------------------------------------------
.PHONY: install
install: all kernel
	@echo ">> Installing userland library to $(INSTALL_LIB_DIR)"
	@mkdir -p $(INSTALL_LIB_DIR)
	@cp $(LIB_FILE) $(INSTALL_LIB_DIR)/
	@echo ">> Installing headers to $(INSTALL_INCLUDE_DIR)"
	@mkdir -p $(INSTALL_INCLUDE_DIR)
	@cp nymya.h $(INSTALL_INCLUDE_DIR)/
	@if [ -f $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) ]; then \
	  echo ">> Installing kernel module"; \
	  mkdir -p $(INSTALL_KERNEL_DIR); \
	  cp $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) $(INSTALL_KERNEL_DIR)/; \
	  depmod -a; \
	else \
	  echo ">> Kernel module not built, skipping"; \
	fi

.PHONY: uninstall
uninstall:
	@echo ">> Uninstalling userland library"
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_FILE)
	@echo ">> Removing headers"
	@rm -f $(INSTALL_INCLUDE_DIR)/nymya.h
	@echo ">> Removing kernel module"
	@rm -f $(INSTALL_KERNEL_DIR)/$(KERNEL_MODULE)
	@depmod -a

# -----------------------------------------------------------------------------
# Cleaning
# -----------------------------------------------------------------------------
.PHONY: clean clean_dir
clean:
	@echo ">> Cleaning userland artifacts"
	@rm -rf $(OBJ_DIR) $(LIB_FILE)
	@echo ">> Cleaning kernel build artifacts"
	@$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(PWD)/$(KERNEL_SRC_DIR) clean || true
	@rm -f $(KERNEL_SRC_DIR)/Makefile $(KERNEL_SRC_DIR)/$(KERNEL_MODULE)

clean_dir: clean

# -----------------------------------------------------------------------------
# .deb packaging for userland
# -----------------------------------------------------------------------------
.PHONY: deb-userland
deb-userland: all
	@echo ">> Creating userland .deb"
	@rm -rf $(DEB_USERLAND_DIR)
	@mkdir -p $(DEB_USERLAND_DIR)/DEBIAN \
	         $(DEB_USERLAND_DIR)/usr/lib \
	         $(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)
	@cp $(LIB_FILE) $(DEB_USERLAND_DIR)/usr/lib/
	@cp nymya.h    $(DEB_USERLAND_DIR)/usr/include/$(LIB_NAME)/
	@printf "Package: $(LIB_NAME)-core-userland\nVersion: $(VERSION)\nArchitecture: $(ARCH)\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Userland)\n" \
	         > $(DEB_USERLAND_DIR)/DEBIAN/control
	@dpkg-deb --build $(DEB_USERLAND_DIR)
	@echo "✅ Built $(DEB_USERLAND_DIR).deb"

# -----------------------------------------------------------------------------
# .deb packaging for kernel module
# -----------------------------------------------------------------------------
.PHONY: deb-kernel
deb-kernel: kernel
	@echo ">> Creating kernel .deb"
	@rm -rf $(DEB_KERNEL_DIR)
	@mkdir -p $(DEB_KERNEL_DIR)/DEBIAN \
	         $(DEB_KERNEL_DIR)/lib/modules/$(shell uname -r)/extra
	@if [ -f $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) ]; then \
	  cp $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) $(DEB_KERNEL_DIR)/lib/modules/$(shell uname -r)/extra/; \
	else \
	  echo "❌ Kernel module missing, aborting"; exit 1; \
	fi
	@printf "Package: $(LIB_NAME)-core-kernel\nVersion: $(VERSION)\nArchitecture: $(ARCH)\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Kernel)\n" \
	         > $(DEB_KERNEL_DIR)/DEBIAN/control
	@dpkg-deb --build $(DEB_KERNEL_DIR)
	@echo "✅ Built $(DEB_KERNEL_DIR).deb"

# -----------------------------------------------------------------------------
# Meta-targets
# -----------------------------------------------------------------------------
.PHONY: deb-all deb
deb-all: deb-userland deb-kernel
deb: deb-all