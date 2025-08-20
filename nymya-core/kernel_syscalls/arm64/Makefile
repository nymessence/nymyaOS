obj-m := nymya_core.o
nymya_core-objs := nymya_kernel_module.o

KDIR := /usr/src/linux-headers-6.8.0-1031-raspi
PWD := /nymyaOS/nymya-core/kernel_syscalls/arm64

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
