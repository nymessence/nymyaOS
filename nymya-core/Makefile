# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------
VERSION ?= 0.1.0
LIB_NAME ?= nymya
DESCRIPTION ?= NymyaOS Core SDK
MAINTAINER ?= NymyaOS Team <team@nymyaos.org>
PKG_ARCH ?= $(shell uname -m | sed -e 's/x86_64/amd64/g' -e 's/aarch64/arm64/g' -e 's/riscv64/riscv64/g')
KERNEL_MODULE ?= nymya_core.ko
KERNEL_SRC_DIR ?= /lib/modules/$(shell uname -r)/build

# Userland library configuration
LIB_FILE ?= libnymya.so
LIB_HEADERS_DIR ?= nymya

# -----------------------------------------------------------------------------
# Userland targets
# -----------------------------------------------------------------------------
.PHONY: userland
userland:
	@echo "🔨 Building userland SDK for $(PKG_ARCH)"
	@mkdir -p $(LIB_HEADERS_DIR)
	@echo "/* NymyaOS syscall interface */" > $(LIB_HEADERS_DIR)/nymya.h
	@echo "#ifndef NYMYA_H" >> $(LIB_HEADERS_DIR)/nymya.h  # Fixed typo: NYMAYA -> NYMYA
	@echo "#define NYMYA_H" >> $(LIB_HEADERS_DIR)/nymya.h  # Fixed typo: NYMAYA -> NYMYA
	@echo "" >> $(LIB_HEADERS_DIR)/nymya.h
	@echo "int nymya_syscall(int syscall_number, ...);" >> $(LIB_HEADERS_DIR)/nymya.h
	@echo "" >> $(LIB_HEADERS_DIR)/nymya.h
	@echo "#endif /* NYMYA_H */" >> $(LIB_HEADERS_DIR)/nymya.h  # Fixed typo: NYMAYA -> NYMYA
	@gcc -shared -o $(LIB_FILE) -fPIC nymya.c
	@echo "✅ Built $(LIB_FILE)"

# -----------------------------------------------------------------------------
# Kernel module targets
# -----------------------------------------------------------------------------
.PHONY: kernel
kernel:
	@echo "🔨 Building kernel module for $(PKG_ARCH)"
	@mkdir -p kernel_syscalls/$(PKG_ARCH)
	@echo "/* Kernel syscall interface for $(PKG_ARCH) */" > kernel_syscalls/$(PKG_ARCH)/nymya.h
	@echo "#ifndef NYMYA_KERNEL_H" >> kernel_syscalls/$(PKG_ARCH)/nymya.h  # Fixed typo
	@echo "#define NYMYA_KERNEL_H" >> kernel_syscalls/$(PKG_ARCH)/nymya.h  # Fixed typo
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya.h
	@echo "/* Add your kernel syscall definitions here */" >> kernel_syscalls/$(PKG_ARCH)/nymya.h
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya.h
	@echo "#endif /* NYMYA_KERNEL_H */" >> kernel_syscalls/$(PKG_ARCH)/nymya.h  # Fixed typo
	@echo "obj-m := $(KERNEL_MODULE:.ko=).o" > kernel_syscalls/$(PKG_ARCH)/Makefile
	@echo "$(KERNEL_MODULE:.ko=)-y := nymya_kernel_module.o" >> kernel_syscalls/$(PKG_ARCH)/Makefile
	@echo "/* Kernel module implementation */" > kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "#include <linux/module.h>" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "#include <linux/kernel.h>" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "#include <linux/init.h>" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "static int __init nymya_init(void) {" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "    pr_info(\"Nymya Core: Module loaded\\n\");" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "    return 0;" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "}" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "static void __exit nymya_exit(void) {" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "    pr_info(\"Nymya Core: Module unloaded\\n\");" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "}" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "module_init(nymya_init);" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "module_exit(nymya_exit);" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "MODULE_LICENSE(\"GPL\");" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "MODULE_AUTHOR(\"NymyaOS Team\");" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "MODULE_DESCRIPTION(\"NymyaOS Core Kernel Module\");" >> kernel_syscalls/$(PKG_ARCH)/nymya_kernel_module.c
	@echo "✅ Built kernel syscall interface for $(PKG_ARCH)"

# -----------------------------------------------------------------------------
# .deb packaging (MUST RUN INSIDE DOCKER)
# -----------------------------------------------------------------------------
.PHONY: deb-all
deb-all: deb-userland deb-kernel

.PHONY: deb-userland
deb-userland:
	@echo "📦 Creating userland .deb for $(PKG_ARCH)"
	@rm -rf nymya-core-userland_$(VERSION)_$(PKG_ARCH).deb
	@mkdir -p nymya-core-userland_$(VERSION)_$(PKG_ARCH)/DEBIAN \
		nymya-core-userland_$(VERSION)_$(PKG_ARCH)/usr/lib \
		nymya-core-userland_$(VERSION)_$(PKG_ARCH)/usr/include/$(LIB_NAME)
	@cp $(LIB_FILE) nymya-core-userland_$(VERSION)_$(PKG_ARCH)/usr/lib/
	@cp nymya.h nymya-core-userland_$(VERSION)_$(PKG_ARCH)/usr/include/$(LIB_NAME)/
	@printf "Package: $(LIB_NAME)-core-userland\nVersion: $(VERSION)\nArchitecture: $(PKG_ARCH)\nMaintainer: $(MAINTAINER)\nDescription: $(DESCRIPTION) (Userland)\n" > nymya-core-userland_$(VERSION)_$(PKG_ARCH)/DEBIAN/control
	@dpkg-deb --build nymya-core-userland_$(VERSION)_$(PKG_ARCH) 2>/dev/null || \
		{ echo "❌ Failed to build userland .deb"; exit 1; }
	@echo "✅ Built nymya-core-userland_$(VERSION)_$(PKG_ARCH).deb"

.PHONY: deb-kernel
deb-kernel:
	@echo "📦 Creating kernel .deb for $(PKG_ARCH)"
	@ARCH=$(shell uname -m | sed -e 's/x86_64/amd64/g' -e 's/aarch64/arm64/g' -e 's/riscv64/riscv64/g') \
	KERNEL_RELEASE=$(shell cat /lib/modules/$(uname -r)/build/include/config/kernel.release 2>/dev/null || echo "unknown") \
	DEB_DIR=nymya-core-kernel_$(VERSION)_$$ARCH.deb \
	&& rm -rf $$DEB_DIR \
	&& mkdir -p $$DEB_DIR/DEBIAN \
	&& mkdir -p $$DEB_DIR/lib/modules/$$KERNEL_RELEASE/extra \
	&& cp /nymyaOS/nymya-core/kernel_syscalls/$$ARCH/$(KERNEL_MODULE) $$DEB_DIR/lib/modules/$$KERNEL_RELEASE/extra/ \
	&& printf "Package: $(LIB_NAME)-core-kernel\\nVersion: $(VERSION)\\nArchitecture: $$ARCH\\nMaintainer: $(MAINTAINER)\\nDepends: linux-image-$$KERNEL_RELEASE\\nDescription: $(DESCRIPTION) (Kernel for $$ARCH)\\n" > $$DEB_DIR/DEBIAN/control \
	&& dpkg-deb --build $$DEB_DIR 2>/dev/null || \
		{ echo "❌ Failed to build $$DEB_DIR"; exit 1; } \
	&& echo "✅ Built $$DEB_DIR"
