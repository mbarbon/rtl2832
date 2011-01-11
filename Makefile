EXTRA_CFLAGS += -I$(KBUILD_SRC)/drivers/media/dvb/dvb-usb/ \
		-I$(KBUILD_SRC)/drivers/media/dvb/dvb-core/ \
		-I$(KBUILD_SRC)/drivers/media/dvb/frontends/ \
		-I$(KBUILD_SRC)/drivers/media/common/tuners \


obj-y += dtmb/
obj-y += dvbc/
obj-y += dvbt/
obj-y += tuners/

dvb-usb-rtl2832u-objs += dtmb/rtl2832-dtmb.o \
			 dvbc/rtl2832-dvbc.o \
			 dvbt/rtl2832-dvbt.o \
			 tuners/rtl2832-tuners.o \
			 foundation.c  math_mpi.c \
			 rtl2832u.c     rtl2832u_io.c \
			 rtl2832u_fe.c  rtl2832u_rc.c

obj-m += dvb-usb-rtl2832u.o


KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all: build install

build:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean



install:
	sudo $(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules_install
	sudo depmod -a

