
include ../version.conf

# Toolchain
CC               = gcc
KERNEL_BUILD_DIR = /lib/modules/$(shell uname -r)/build
CFLAGS           = -Wall -fPIC -O2
LDFLAGS          = -shared

# Paths and names
INCLUDE_DIR        = .
SRC_DIR            = .
KERNEL_SRC_DIR     = kernel_syscalls
OBJ_DIR            = build
LIB_NAME           = nymya
LIB_FILE           = lib$(LIB_NAME).so
KERNEL_MODULE      = nymya_syscalls.ko

INSTALL_LIB_DIR     = /usr/lib
INSTALL_INCLUDE_DIR = /usr/include/nymya
INSTALL_KERNEL_DIR  = /lib/modules/$(shell uname -r)/extra

# Package parameters (filled from version.conf)
VERSION     ?= 0.0.0
ARCH        ?= amd64
MAINTAINER  ?= Unknown <>

DESCRIPTION = NymyaOS Core Syscall SDK

# Deb folder names
DEB_USERLAND_DIR = nymya-core-userland_$(VERSION)_$(ARCH)
DEB_KERNEL_DIR   = nymya-core-kernel_$(VERSION)_$(ARCH)

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Default build
all: $(LIB_FILE)

# Compile userland objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link shared library
$(LIB_FILE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: kernel
kernel:
ifeq ("$(wildcard $(KERNEL_BUILD_DIR))","")
	@echo "Warning: Kernel headers not found at $(KERNEL_BUILD_DIR). Skipping kernel module build."
else
	$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(PWD)/$(KERNEL_SRC_DIR) modules
endif

install: all kernel
	@echo Installing $(LIB_FILE) to $(INSTALL_LIB_DIR)
	@mkdir -p $(INSTALL_LIB_DIR)
	@cp $(LIB_FILE) $(INSTALL_LIB_DIR)/

	@echo Installing headers to $(INSTALL_INCLUDE_DIR)
	@mkdir -p $(INSTALL_INCLUDE_DIR)
	@cp nymya.h $(INSTALL_INCLUDE_DIR)/

	@if [ -f $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) ]; then \
		echo Installing kernel module $(KERNEL_MODULE); \
		mkdir -p $(INSTALL_KERNEL_DIR); \
		cp $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) $(INSTALL_KERNEL_DIR)/; \
		depmod -a; \
	else \
		echo "Kernel module not built, skipping install."; \
	fi

uninstall:
	@echo Uninstalling $(LIB_FILE)
	@rm -f $(INSTALL_LIB_DIR)/$(LIB_FILE)
	@echo Removing header
	@rm -f $(INSTALL_INCLUDE_DIR)/nymya.h
	@echo Removing kernel module
	@rm -f $(INSTALL_KERNEL_DIR)/$(KERNEL_MODULE)
	@depmod -a

prepare:
	@mkdir -p $(INCLUDE_DIR)
	@ln -sf ./nymya.h $(INCLUDE_DIR)/nymya.h

clean_dir:
	@rm -rf $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR) $(LIB_FILE) $(DEB_USERLAND_DIR) $(DEB_KERNEL_DIR)
	@if [ -d $(KERNEL_SRC_DIR) ]; then \
		$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(PWD)/$(KERNEL_SRC_DIR) clean || true; \
	fi
	@rm -f $(KERNEL_SRC_DIR)/$(KERNEL_MODULE)

# Package: userland-only .deb
.PHONY: deb-userland
deb-userland: all
	@echo "Creating userland .deb package..."
	rm -rf $(DEB_USERLAND_DIR)
	mkdir -p $(DEB_USERLAND_DIR)/DEBIAN
	mkdir -p $(DEB_USERLAND_DIR)/usr/lib
	mkdir -p $(DEB_USERLAND_DIR)/usr/include/nymya

	cp $(LIB_FILE) $(DEB_USERLAND_DIR)/usr/lib/
	cp nymya.h    $(DEB_USERLAND_DIR)/usr/include/nymya/

	# Control file
	echo "Package: nymya-core-userland"           >  $(DEB_USERLAND_DIR)/DEBIAN/control
	echo "Version: $(VERSION)"                    >> $(DEB_USERLAND_DIR)/DEBIAN/control
	echo "Architecture: $(ARCH)"                  >> $(DEB_USERLAND_DIR)/DEBIAN/control
	echo "Maintainer: $(MAINTAINER)"              >> $(DEB_USERLAND_DIR)/DEBIAN/control
	echo "Description: $(DESCRIPTION) (Userland)" >> $(DEB_USERLAND_DIR)/DEBIAN/control

	dpkg-deb --build $(DEB_USERLAND_DIR)
	@echo "✅ Built $(DEB_USERLAND_DIR).deb"

# Package: kernel-only .deb
.PHONY: deb-kernel
deb-kernel: kernel
	@echo "Creating kernel .deb package..."
	rm -rf $(DEB_KERNEL_DIR)
	mkdir -p $(DEB_KERNEL_DIR)/DEBIAN
	mkdir -p $(DEB_KERNEL_DIR)/lib/modules/$(shell uname -r)/extra

	@if [ -f $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) ]; then \
		cp $(KERNEL_SRC_DIR)/$(KERNEL_MODULE) $(DEB_KERNEL_DIR)/lib/modules/$(shell uname -r)/extra/; \
	else \
		echo "❌ Kernel module not found! Aborting."; \
		exit 1; \
	fi

	# Control file
	echo "Package: nymya-core-kernel"           >  $(DEB_KERNEL_DIR)/DEBIAN/control
	echo "Version: $(VERSION)"                  >> $(DEB_KERNEL_DIR)/DEBIAN/control
	echo "Architecture: $(ARCH)"                >> $(DEB_KERNEL_DIR)/DEBIAN/control
	echo "Maintainer: $(MAINTAINER)"            >> $(DEB_KERNEL_DIR)/DEBIAN/control
	echo "Description: $(DESCRIPTION) (Kernel)" >> $(DEB_KERNEL_DIR)/DEBIAN/control

	dpkg-deb --build $(DEB_KERNEL_DIR)
	@echo "✅ Built $(DEB_KERNEL_DIR).deb"

# Meta-target to build all four .debs
.PHONY: deb-all
deb-all:
	$(MAKE) ARCH=amd64 deb-userland
	$(MAKE) ARCH=amd64 deb-kernel
	$(MAKE) ARCH=arm64 deb-userland
	$(MAKE) ARCH=arm64 deb-kernel
