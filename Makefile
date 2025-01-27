obj-m += SRMC_Module.o
SRMC_Module-objs := SRMC.o utilities.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
 	
