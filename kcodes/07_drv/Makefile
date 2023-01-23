obj-m += drv.o
KDIR=/lib/modules/$(shell uname -r)/build

all: uclean clean ubuild modules
ubuild:
	gcc -ggdb3 -o udrv udrv.c
modules:
	make -C $(KDIR) M=$(PWD) modules
uclean:
	rm -rf udrv.o udrv
clean:
	make -C $(KDIR) M=$(PWD) clean