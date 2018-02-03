ifneq ($(KERNELRELEASE),)
	obj-m := module2.o
else
	KDIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif

