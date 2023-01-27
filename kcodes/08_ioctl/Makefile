obj-m += ioctl.o
KDIR=/lib/modules/$(shell uname -r)/build

all: uclean clean ubuild modules
ubuild:
	gcc -ggdb3 -o uioctl uioctl.c
modules:
	make -C $(KDIR) M=$(PWD) modules
uclean:
	rm -rf uioctl.o uioctl
clean:
	make -C $(KDIR) M=$(PWD) clean