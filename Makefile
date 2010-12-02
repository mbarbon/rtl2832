dvb-usb-rtl2832u-objs = demod_rtl2832.o	dvbt_demod_base.o \
			dvbt_nim_base.o foundation.o math_mpi.o \
			nim_rtl2832_mxl5007t.o nim_rtl2832_fc2580.o \
			nim_rtl2832_mt2266.o rtl2832u.o rtl2832u_fe.o \
			rtl2832u_io.o tuner_mxl5007t.o tuner_fc2580.o \
			tuner_mt2266.o tuner_tua9001.o nim_rtl2832_tua9001.o \
			tuner_fc0012.o nim_rtl2832_fc0012.o demod_rtl2836.o \
			dtmb_demod_base.o dtmb_nim_base.o nim_rtl2836_fc2580.o \
			nim_rtl2836_mxl5007t.o tuner_e4000.o \
			nim_rtl2832_e4000.o tuner_mt2063.o demod_rtl2840.o \
			tuner_max3543.o nim_rtl2832_mt2063.o \
			nim_rtl2832_max3543.o nim_rtl2840_mt2063.o \
			nim_rtl2840_max3543.o qam_demod_base.o qam_nim_base.o

obj-m += dvb-usb-rtl2832u.o

EXTRA_CFLAGS += -I$(KBUILD_SRC)/drivers/media/dvb/dvb-usb/ \
		-I$(KBUILD_SRC)/drivers/media/dvb/dvb-core/ \
		-I$(KBUILD_SRC)/drivers/media/dvb/frontends/ \
		-I$(KBUILD_SRC)/drivers/media/common/tuners


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#KERNELDIR = /lib/modules/2.6.31-22-generic/build
#KERNELDIR = /lib/modules/2.6.32-18-generic/build




build:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean



install:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules_install
	depmod -a

