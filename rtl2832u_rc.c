#include <linux/module.h>
#include <linux/version.h>

#include "rtl2832u.h"
#include "rtl2832u_io.h"

#if 0

////////////////////////////////////////////////////////////////////////////////////////////////////////
enum   rc_status_define{
	RC_FUNCTION_SUCCESS =0,
	RC_FUNCTION_UNSUCCESS
};

//IRRC register table
static const RT_rc_set_reg_struct rc_setup_table[]=
{
	{RTD2832U_SYS,RC_USE_DEMOD_CTL1		,0x00,OP_AND,0xfb},
	{RTD2832U_SYS,RC_USE_DEMOD_CTL1		,0x00,OP_AND,0xf7},
	{RTD2832U_USB,USB_CTRL			,0x00,OP_OR ,0x20},
	{RTD2832U_SYS,SYS_GPD			,0x00,OP_AND,0xf7},
	{RTD2832U_SYS,SYS_GPOE			,0x00,OP_OR ,0x08},
	{RTD2832U_SYS,SYS_GPO			,0x00,OP_OR ,0x08},

	{RTD2832U_RC,IR_RX_CTRL			,0x20,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_BUFFER_CTRL		,0x80,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_IF			,0xff,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_IE			,0xff,OP_NO ,0xff},

	/* this register seems not to do anything */
	{RTD2832U_RC,IR_MAX_DURATION0		,0xd0,OP_NO ,0xff},
	{RTD2832U_RC,IR_MAX_DURATION1		,0x07,OP_NO ,0xff},


	{RTD2832U_RC,IR_IDLE_LEN0		,0xc0,OP_NO ,0xff},
	{RTD2832U_RC,IR_IDLE_LEN1		,0x00,OP_NO ,0xff},


	{RTD2832U_RC,IR_GLITCH_LEN		,0x03,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_CLK			,0x09,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_CONFIG		,0x1c,OP_NO ,0xff},
	{RTD2832U_RC,IR_MAX_H_Tolerance_LEN	,0x1e,OP_NO ,0xff},
	{RTD2832U_RC,IR_MAX_L_Tolerance_LEN	,0x1e,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_CTRL			,0x80,OP_NO ,0xff}
		
};

static const RT_rc_set_reg_struct rc_error_flush_table[]={
	{RTD2832U_RC,IR_RX_CTRL			,0x20,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_BUFFER_CTRL		,0x80,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_IF			,0xff,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_IE			,0xff,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_CTRL			,0x80,OP_NO ,0xff}

};
static const RT_rc_set_reg_struct rc_flush_table[]={
	{RTD2832U_RC,IR_RX_IF			,0x03,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_BUFFER_CTRL		,0x80,OP_NO ,0xff},
	{RTD2832U_RC,IR_RX_CTRL			,0x80,OP_NO ,0xff}

};

int rtl2832u_rc_open(void *ptr)
{
	int ret = RC_FUNCTION_SUCCESS;
	u8 data,i;
	struct dvb_usb_device *d = (struct dvb_usb_device *)ptr;

	deb_rc("+rc_%s\n", __FUNCTION__);

	for (i=0; i < ARRAY_SIZE(rc_setup_table); i++) {
		switch(rc_setup_table[i].type) {
		case RTD2832U_SYS:
		case RTD2832U_USB:
			data=rc_setup_table[i].data;
			if (rc_setup_table[i].op != OP_NO) {
				if ( read_usb_sys_char_bytes( d ,
					rc_setup_table[i].type ,
					rc_setup_table[i].address,
					&data ,
					1))
					goto error;

				if (rc_setup_table[i].op == OP_AND)
					data &=  rc_setup_table[i].op_mask;
				else//OP_OR
					data |=  rc_setup_table[i].op_mask;
			}
			if ( write_usb_sys_char_bytes( d ,
					rc_setup_table[i].type ,
					rc_setup_table[i].address,
					&data ,
					1))
				goto error;

		break;
		case RTD2832U_RC:
			data= rc_setup_table[i].data;
			if (rc_setup_table[i].op != OP_NO)
			{
				if ( read_rc_register( d ,
					rc_setup_table[i].type ,
					rc_setup_table[i].address,
					&data ,
					1))
					goto error;

				if (rc_setup_table[i].op == OP_AND)
					data &=  rc_setup_table[i].op_mask;
				else//OP_OR
					data |=  rc_setup_table[i].op_mask;
			}

			if ( write_rc_register( d ,
					rc_setup_table[i].type ,
					rc_setup_table[i].address,
					&data ,
					1))
				goto error;
		break;
		default:
			deb_rc("+%s : rc table error! \n", __FUNCTION__);
			ret=RC_FUNCTION_UNSUCCESS;
			goto error;
		break;
		}	
	}
	ret=RC_FUNCTION_SUCCESS;
	ir_raw_event_set_idle(d->rc_input_dev, true);
error: 
	deb_rc("-rc_%s ret = %d \n", __FUNCTION__, ret);
	return ret;

	
}

#define rt_code_len       0xFF
int rtl2832u_rc_query(struct dvb_usb_device *d)
{
	u8  data,i,len;
	u8  rlc_code[rt_code_len];
	u8 hw_sample;
	DEFINE_IR_RAW_EVENT(ev);

	if ( read_rc_register( d ,RTD2832U_RC, IR_RX_IF,&data ,1))
		goto error;

	if (!(data & 0x80) || !(data & 0x03))
		return 0;
	
	if ( read_rc_register( d ,RTD2832U_RC,IR_RX_BC,&len ,1))
		goto error;

	if (len == 0)
		return 0;

	if ((len % 2) == 1)
		len+=1;

	if (len > rt_code_len) {
		deb_info("truncated len\n");
		len=rt_code_len;
	}

	deb_info("got IR packet with len= %d\n", len);

	memset(rlc_code,0,rt_code_len);

	if ( read_rc_register( d ,RTD2832U_RC,IR_RX_BUF,rlc_code ,0xFF))
		goto error;

	for (i = 0 ; i < len ; i++) {
		hw_sample = rlc_code[i];

		//deb_info("got hw sample: %02x\n", hw_sample);

		ev.pulse = !!(hw_sample & 0x80);
		hw_sample &= ~0x80;

		ev.duration = hw_sample * 50 * 1000;


		deb_info("got %05dms %s\n", ev.duration / 1000, ev.pulse ? "pulse" : "space" );

		if (!hw_sample)
			break;

		ir_raw_event_store_with_filter(d->rc_input_dev, &ev);
	}


	/* gross hack below */
	/* I don't yet know if its possible to make hardware split
	 * ir packets between 2 or more buffers
	 *currently each there are missing samples between
	 * each read
	 * In addition to that there is no stream of spaces
	 * at the end of keypress, thus, just send one big fake
	 * sample here. Yes I know repeat is very broken,
	 * but this is the smallest reason why it is... drat */

	init_ir_raw_event(&ev);

	ev.timeout = 1;
	ev.duration = 40000 * 1000;
	ir_raw_event_store_with_filter(d->rc_input_dev, &ev);

	ir_raw_event_handle(d->rc_input_dev);
	ir_raw_event_set_idle(d->rc_input_dev, true);

	for (i=0; i < ARRAY_SIZE(rc_flush_table); i++)
	{
		data= rc_flush_table[i].data;
		if ( write_rc_register( d ,RTD2832U_RC, rc_flush_table[i].address,&data,1))
			goto error;
	}

 return 0;
error:

	deb_info("error place");
	for (i=0; i < ARRAY_SIZE(rc_error_flush_table); i++)
	{
		data = rc_error_flush_table[i].data;
		if ( write_rc_register( d ,RTD2832U_RC, rc_error_flush_table[i].address,&data,1))
			return -1;

	}
	return 0;

}

#endif
