obj-m += sendsignal.o
KDIR=/lib/modules/$(shell uname -r)/build

all: uclean clean ubuild modules
ubuild:
	gcc -ggdb3 -o usendsignal usendsignal.c
modules:
	make -C $(KDIR) M=$(PWD) modules
uclean:
	rm -rf usendsignal.o udrv 25.log
clean:
	make -C $(KDIR) M=$(PWD) clean