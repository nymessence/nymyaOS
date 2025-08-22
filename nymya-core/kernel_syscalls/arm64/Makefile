obj-m := nymya_core.o
nymya_core-objs := nymya_kernel_module.o
KDIR := /lib/modules/6.8.0-1031-raspi/build
PWD := /nymyaOS/nymya-core/kernel_syscalls/arm64
all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean
