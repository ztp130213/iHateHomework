#Makefile
ifneq ($(KERNELRELEASE),)
	obj-m := clock.o	#Output Name
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif 
