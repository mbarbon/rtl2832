
#include <linux/module.h>
#include <linux/version.h>

#include "rtl2832u.h"
#include "rtl2832u_io.h"


int dvb_usb_rtl2832u_debug =1;
module_param_named(debug,dvb_usb_rtl2832u_debug, int, 0644);
MODULE_PARM_DESC(debug, "Set debugging level (1=info,xfer=2 (or-able))." DVB_USB_DEBUG_STATUS);


int demod_default_type=0;
module_param_named(demod, demod_default_type, int, 0644);
MODULE_PARM_DESC(demod, "Set default demod type(0=dvb-t, 1=dtmb, 2=dvb-c)"DVB_USB_DEBUG_STATUS);

int dtmb_error_packet_discard;
module_param_named(dtmb_err_discard, dtmb_error_packet_discard, int, 0644);
MODULE_PARM_DESC(dtmb_err_discard, "Set error packet discard type(0=not discard, 1=discard)"DVB_USB_DEBUG_STATUS);


//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);
//#endif

static int rtl2832u_streaming_ctrl(struct dvb_usb_adapter *adap , int onoff)
{
	u8 data[2];	
	//3 to avoid  scanning  channels loss
	if(onoff)
	{
		data[0] = data[1] = 0;		
		if ( write_usb_sys_char_bytes( adap->dev , RTD2832U_USB , USB_EPA_CTL , data , 2) ) goto error;				
	}
	else
	{
		data[0] = 0x10;	//3stall epa, set bit 4 to 1
		data[1] = 0x02;	//3reset epa, set bit 9 to 1
		if ( write_usb_sys_char_bytes( adap->dev , RTD2832U_USB , USB_EPA_CTL , data , 2) ) goto error;		
	}

	return 0;
error: 
	return -1;
}


static int rtl2832u_frontend_attach(struct dvb_usb_adapter *adap)
{
	adap->fe = rtl2832u_fe_attach(adap->dev); 
	return 0;
}


static void rtl2832u_usb_disconnect(struct usb_interface *intf)
{
	try_module_get(THIS_MODULE);
	dvb_usb_device_exit(intf);	
}


static struct dvb_usb_device_properties rtl2832u_properties;


//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
static int rtl2832u_usb_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	if (!intf->altsetting->desc.bNumEndpoints)
		return -ENODEV;

	if (!dvb_usb_device_init(intf,&rtl2832u_properties,THIS_MODULE,NULL,adapter_nr))
		return 0;

	return -ENODEV;
}

static struct usb_device_id rtl2832u_usb_table [] = {
	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTD2832U_WARM) },
	{ USB_DEVICE(USB_VID_AZUREWAVE_2, USB_PID_AZUREWAVE_USB_WARM) },
	{ USB_DEVICE(USB_VID_KWORLD_1ST, USB_PID_KWORLD_WARM_6) },
	{ USB_DEVICE(USB_VID_DEXATEK, USB_PID_DEXATEK_USB_WARM) },
	{ USB_DEVICE(USB_VID_DEXATEK, USB_PID_DEXATEK_MINIUSB_WARM) },
	{ USB_DEVICE(USB_VID_DEXATEK, USB_PID_DEXATEK_5217_WARM) },

	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTD2832U_2ND_WARM) },
	{ USB_DEVICE(USB_VID_GOLDENBRIDGE, USB_PID_GOLDENBRIDGE_WARM) },
	{ USB_DEVICE(USB_VID_YUAN, USB_PID_YUAN_WARM) },
	{ USB_DEVICE(USB_VID_AZUREWAVE_2, USB_PID_AZUREWAVE_MINI_WARM) },
	{ USB_DEVICE(USB_VID_AZUREWAVE_2, USB_PID_AZUREWAVE_GPS_WARM) },	
	{ USB_DEVICE(USB_VID_KWORLD_1ST, USB_PID_KWORLD_WARM_7) },
	
	{ USB_DEVICE(USB_VID_KWORLD_1ST, USB_PID_KWORLD_WARM_3) },
	{ USB_DEVICE(USB_VID_KWORLD_1ST, USB_PID_KWORLD_WARM_8) },
	{ USB_DEVICE(USB_VID_GTEK, USB_PID_GTEK_WARM) },	
	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTL2836_WARM)},	
	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTL2836_2ND_WARM)},	
	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTD2832U_3RD_WARM)},
	{ USB_DEVICE(USB_VID_REALTEK, USB_PID_RTD2832U_4TH_WARM)},
       { USB_DEVICE(USB_VID_KWORLD_1ST, USB_PID_KWORLD_WARM_4)},      
	{ USB_DEVICE(USB_VID_DEXATEK, USB_PID_DEXATEK_WARM_4)},
	
	{ USB_DEVICE(USB_VID_GTEK, USB_PID_GTEK_WARM_A)},
	{ USB_DEVICE(USB_VID_GTEK, USB_PID_GTEK_WARM_B)},
	{ USB_DEVICE(USB_VID_GTEK, USB_PID_GTEK_WARM_C)},
	{ USB_DEVICE(USB_VID_GTEK, USB_PID_GTEK_WARM_D)},
	{ USB_DEVICE(USB_VID_THP, USB_PID_THP )},
	{ USB_DEVICE(USB_VID_THP, USB_PID_THP2)},	
	{ 0 },
};


MODULE_DEVICE_TABLE(usb, rtl2832u_usb_table);

static struct dvb_usb_device_properties rtl2832u_properties = {

	.num_adapters = 1,
	.adapter = 
	{
		{
			.streaming_ctrl = rtl2832u_streaming_ctrl,
			.frontend_attach = rtl2832u_frontend_attach,
			//parameter for the MPEG2-data transfer 
			.stream = 
			{
				.type = USB_BULK,
				.count = RTD2831_URB_NUMBER,
				.endpoint = 0x01,		//data pipe
				.u = 
				{
					.bulk = 
					{
						.buffersize = RTD2831_URB_SIZE,
					}
				}
			},
		}
	},
#if 0
	.rc.core = {
		.rc_codes         = RC_MAP_EMPTY,
		.protocol         = IR_TYPE_OTHER,
		.module_name      = "rtl2832",
		.rc_query         = rtl2832u_rc_query,
		.rc_interval      = 500,

		.rc_props = {
			.open = rtl2832u_rc_open,
			.driver_type = RC_DRIVER_IR_RAW,
			.allowed_protos = IR_TYPE_ALL,
			//.max_timeout = 10000 * 1000,
			//.min_timeout = 1     * 1000,
			//.timeout     = 10000 * 1000,
			
		}
	},
#endif
	
	.num_device_descs = 27,
	.devices = {
		{ .name = "RTL2832U DVB-T USB DEVICE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[0], NULL },
		},
		{ .name = "DVB-T USB Dongle",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[1], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[2], NULL },
		},
		{ .name = "DK DVBT DONGLE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[3], NULL },
		},
		{ .name = "DK mini DVBT DONGLE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[4], NULL },
		},
		{ .name = "DK 5217 DVBT DONGLE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[5], NULL },
		},
		{ .name = "RTL2832U DVB-T USB DEVICE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[6], NULL },
		},
		{ .name = "RTL2832U DVB-T USB DEVICE",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[7], NULL },
		},
		{ .name = "Digital TV Tuner Card",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[8], NULL },
		},
		{ .name = "DVB-T FTA USB Half Minicard",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[9], NULL },
		},
		{ .name = "DVB-T + GPS Minicard",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[10], NULL },
		},
		{ .name = "UB450-T",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[11], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[12], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[13], NULL },
		},
		{ .name = "RT DTV 2832U",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[14], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[15], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[16], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[17], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[18], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[19], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[20], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[21], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[22], NULL },
		},
		{ .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[23], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[24], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[25], NULL },
		},
		{
		  .name = "USB DVB-T Device",
		  .cold_ids = { NULL, NULL },
		  .warm_ids = { &rtl2832u_usb_table[26], NULL },
		},
		{ NULL },
	}
};



static struct usb_driver rtl2832u_usb_driver = {
	.name		= "dvb_usb_rtl2832u",
	.probe		= rtl2832u_usb_probe,
	.disconnect	= rtl2832u_usb_disconnect,
	.id_table		= rtl2832u_usb_table,
};


static int __init rtl2832u_usb_module_init(void)
{
	int result =0 ;
	
	deb_info("+info debug open_%s\n", __FUNCTION__);
	if ((result = usb_register(&rtl2832u_usb_driver))) {
		err("usb_register failed. (%d)",result);
		return result;
	}

	return 0;
}

static void __exit rtl2832u_usb_module_exit(void)
{
	usb_deregister(&rtl2832u_usb_driver);

	return ;	
}



module_init(rtl2832u_usb_module_init);
module_exit(rtl2832u_usb_module_exit);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Remake : base on linux kernel :2.6.32  ubuntu 10.04 	Test						//	
//////////////////////////////////////////////////////////////////////////////////////////////////////////

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("Driver for the RTL2832U DVB-T / RTL2836 DTMB USB2.0 device");
MODULE_VERSION("2.0.1");
MODULE_LICENSE("GPL");

