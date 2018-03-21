KDIR:=/home/wuyu/linux-lts-trusty-3.13.0
obj-m:=hello.o
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.symvers *.order *.mod.c
