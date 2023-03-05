obj-m += export_sym1.o export_sym2.o
KDIR=/lib/modules/$(shell uname -r)/build

all: clean modules
modules:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean