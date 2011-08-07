NOSTDINC_FLAGS := -I$(KERNELDIR)/drivers/media/dvb/dvb-usb/ \
		-I$(KERNELDIR)/drivers/media/dvb/dvb-core/ \
		-I$(KERNELDIR)/drivers/media/dvb/frontends/ \
		-I$(KERNELDIR)/drivers/media/common/tuners \
		-I$(M) \
		-I$(M)/tuners \
		-I$(M)/dtmb \
		-I$(M)/dvbc \
		-I$(M)/dvbt

obj-y += dtmb/
obj-y += dvbc/
obj-y += dvbt/
obj-y += tuners/

rtl2832u-objs += dtmb/built-in.o \
		 dvbc/built-in.o \
		 dvbt/built-in.o \
		 tuners/built-in.o \
		 foundation.o  math_mpi.o \
		 main.o     rtl2832u_io.o \
		 rtl2832u_fe.o  rtl2832u_rc.o \
		 rtl2832u_ioctl.o

obj-m += rtl2832u.o


KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all: build install

build:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean

install:
	sudo $(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules_install
	sudo depmod -a
