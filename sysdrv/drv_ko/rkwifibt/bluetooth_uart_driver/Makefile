MODULE_NAME = hci_uart

ifneq ($(KERNELRELEASE),)
	obj-m :=$(MODULE_NAME).o
	$(MODULE_NAME)-y := hci_ldisc.o hci_h4.o hci_rtk_h5.o rtk_coex.o
	#cc1: all warnings being treated as errors解决办法
	CFLAGS = -Wall -Wpointer-arith -Wno-unused
	KBUILD_CFLAGS += -w
else
	PWD := $(shell pwd)
	KDIR := ../../../source/kernel

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	#$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(shell pwd) modules -j16

clean:
	rm -rf *.o *.mod *.mod.c *.mod.o *.ko *.symvers *.order *.a
endif

