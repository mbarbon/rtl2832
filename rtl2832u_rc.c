#include <linux/module.h>
#include <linux/version.h>

#include "rtl2832u.h"
#include "rtl2832u_io.h"


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
static const RT_rc_set_reg_struct p_flush_table2[]={
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
					{
						deb_rc("+%s : rc- usb or sys register read error! \n", __FUNCTION__);
						ret=RC_FUNCTION_UNSUCCESS;
						goto error;
					}					
				
					if (rc_setup_table[i].op == OP_AND)
					{
					        data &=  rc_setup_table[i].op_mask;	
					}
					else//OP_OR
					{
						data |=  rc_setup_table[i].op_mask;
					}			
				}
				if ( write_usb_sys_char_bytes( d , 
							      rc_setup_table[i].type , 
							      rc_setup_table[i].address,
							      &data , 
							      1))
				{
						deb_rc("+%s : rc- usb or sys register write error! \n", __FUNCTION__);
						ret= RC_FUNCTION_UNSUCCESS;
						goto error;
				}
		
			break;
			case RTD2832U_RC:
				data= rc_setup_table[i].data;
				if (rc_setup_table[i].op != OP_NO)
				{
					if ( read_rc_char_bytes( d , 
								 rc_setup_table[i].type , 
								 rc_setup_table[i].address,
								 &data , 
								 1))
					{
						deb_rc("+%s : rc -ir register read error! \n", __FUNCTION__);
						ret=RC_FUNCTION_UNSUCCESS;
						goto error;
					}					
				
					if (rc_setup_table[i].op == OP_AND)
					{
					        data &=  rc_setup_table[i].op_mask;	
					}
					else//OP_OR
					{
						data |=  rc_setup_table[i].op_mask;
					}			
				}
				if ( write_rc_char_bytes( d , 
							      rc_setup_table[i].type , 
							      rc_setup_table[i].address,
							      &data , 
							      1))
				{
					deb_rc("+%s : rc -ir register write error! \n", __FUNCTION__);
					ret=RC_FUNCTION_UNSUCCESS;
					goto error;
				}

			break;
			default:
				deb_rc("+%s : rc table error! \n", __FUNCTION__);
				ret=RC_FUNCTION_UNSUCCESS;
				goto error;
			break;	
		}	
	}
	ret=RC_FUNCTION_SUCCESS;
error: 
	deb_rc("-rc_%s ret = %d \n", __FUNCTION__, ret);
	return ret;

	
}

#define flush_step_Number 0x05
#define rt_code_len       0x80  
int rtl2832u_rc_query(struct dvb_usb_device *d)
{
	u8  data,i,byte_count;
	u8  rt_u8_code[rt_code_len];

	if ( read_rc_char_bytes( d ,RTD2832U_RC, IR_RX_IF,&data ,1))
		goto error;

	if (!(data & 0x80) || !(data & 0x03))
		goto error;
	
	if ( read_rc_char_bytes( d ,RTD2832U_RC,IR_RX_BC,&byte_count ,1))
		goto error;

	if (byte_count == 0)
		goto error;

	deb_info("%s : get rc code len = %d \n", __FUNCTION__,byte_count);

	if ((byte_count % 2) == 1)
		byte_count+=1;

	if (byte_count > rt_code_len)
		byte_count=rt_code_len;

	memset(rt_u8_code,0,rt_code_len);

	if ( read_rc_char_bytes( d ,RTD2832U_RC,IR_RX_BUF,rt_u8_code ,0x80))
		goto error;

	for (i=0; i < ARRAY_SIZE(p_flush_table2); i++)
	{
		data= p_flush_table2[i].data;
		if ( write_rc_char_bytes( d ,RTD2832U_RC, p_flush_table2[i].address,&data,1))
			goto error;
	}

	return 0;

error:
	for (i=0;i<flush_step_Number;i++)
	{
		data = rc_error_flush_table[i].data;
		if ( write_rc_char_bytes( d ,RTD2832U_RC, rc_error_flush_table[i].address,&data,1))
			return -1;

	}
	return 0;

}