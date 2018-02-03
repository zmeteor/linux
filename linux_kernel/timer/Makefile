ifneq ($(KERNELRELEASE),)
	obj-m :=timer_mod.o
else
	KDIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
