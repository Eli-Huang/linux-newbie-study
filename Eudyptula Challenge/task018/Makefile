obj-m :=helloworld.o
KERNEK_BUILD = "/home/elihuang/storage/linux-4.18-rc7"
all:
	make -C $(KERNEK_BUILD)  SUBDIRS=$(PWD) modules
clean:
	make -C  SUBDIRS=$(PWD) clean
