#include <linux/module.h>
#include <linux/version.h>

#include "rtl2832u.h"
#include "rtl2832u_io.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
enum   rc_status_define{
	RC_FUNCTION_SUCCESS =0,
	RC_FUNCTION_UNSUCCESS
};

static int rtl2832u_remote_control_state=0;

//IRRC register table
static const RT_rc_set_reg_struct p_rtl2832u_rc_initial_table[]= 
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

static const RT_rc_set_reg_struct p_flush_table1[]={
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

	
int rtl2832u_remoto_control_initial_setting(struct dvb_usb_device *d)
{ 
	int ret = RC_FUNCTION_SUCCESS;
	u8 data,i;


	deb_rc("+rc_%s\n", __FUNCTION__);

	for (i=0;i<ARRAY_SIZE(p_rtl2832u_rc_initial_table); i++)
	{	
		switch(p_rtl2832u_rc_initial_table[i].type)
		{
			case RTD2832U_SYS:
			case RTD2832U_USB:
				data=p_rtl2832u_rc_initial_table[i].data;
				if (p_rtl2832u_rc_initial_table[i].op != OP_NO)
				{
					if ( read_usb_sys_char_bytes( d , 
								      p_rtl2832u_rc_initial_table[i].type , 
								      p_rtl2832u_rc_initial_table[i].address,
								      &data , 
								      LEN_1) ) 
					{
						deb_rc("+%s : rc- usb or sys register read error! \n", __FUNCTION__);
						ret=RC_FUNCTION_UNSUCCESS;
						goto error;
					}					
				
					if (p_rtl2832u_rc_initial_table[i].op == OP_AND)
					{
					        data &=  p_rtl2832u_rc_initial_table[i].op_mask;	
					}
					else//OP_OR
					{
						data |=  p_rtl2832u_rc_initial_table[i].op_mask;
					}			
				}
				if ( write_usb_sys_char_bytes( d , 
							      p_rtl2832u_rc_initial_table[i].type , 
							      p_rtl2832u_rc_initial_table[i].address,
							      &data , 
							      LEN_1) ) 
				{
						deb_rc("+%s : rc- usb or sys register write error! \n", __FUNCTION__);
						ret= RC_FUNCTION_UNSUCCESS;
						goto error;
				}
		
			break;
			case RTD2832U_RC:
				data= p_rtl2832u_rc_initial_table[i].data;
				if (p_rtl2832u_rc_initial_table[i].op != OP_NO)
				{
					if ( read_rc_char_bytes( d , 
								 p_rtl2832u_rc_initial_table[i].type , 
								 p_rtl2832u_rc_initial_table[i].address,
								 &data , 
								 LEN_1) ) 
					{
						deb_rc("+%s : rc -ir register read error! \n", __FUNCTION__);
						ret=RC_FUNCTION_UNSUCCESS;
						goto error;
					}					
				
					if (p_rtl2832u_rc_initial_table[i].op == OP_AND)
					{
					        data &=  p_rtl2832u_rc_initial_table[i].op_mask;	
					}
					else//OP_OR
					{
						data |=  p_rtl2832u_rc_initial_table[i].op_mask;
					}			
				}
				if ( write_rc_char_bytes( d , 
							      p_rtl2832u_rc_initial_table[i].type , 
							      p_rtl2832u_rc_initial_table[i].address,
							      &data , 
							      LEN_1) ) 
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
	rtl2832u_remote_control_state=RC_INSTALL_OK;
	ret=RC_FUNCTION_SUCCESS;
error: 
	deb_rc("-rc_%s ret = %d \n", __FUNCTION__, ret);
	return ret;

	
}

#define receiveMaskFlag1  0x80
#define receiveMaskFlag2  0x03
#define flush_step_Number 0x05
#define rt_code_len       0x80  
int rtl2832u_rc_query(struct dvb_usb_device *d)
{

	u8  data=0,i=0,byte_count=0;
	int ret=0;
	u8  rt_u8_code[rt_code_len];
	u8  ucode[4];
	u16 scancode=0;

	if(rtl2832u_remote_control_state == RC_NO_SETTING)
	{
		ret=rtl2832u_remoto_control_initial_setting(d);	

	}
	if ( read_rc_char_bytes( d ,RTD2832U_RC, IR_RX_IF,&data ,LEN_1) ) 
	{
		//deb_rc("%s : rc -ir register read error! \n", __FUNCTION__);
		ret=-1;
		goto error;
	}	
	if (!(data & receiveMaskFlag1))
	{
		ret =0 ;
		goto error;
	}	
	
	if (data & receiveMaskFlag2)
	{
			if ( read_rc_char_bytes( d ,RTD2832U_RC,IR_RX_BC,&byte_count ,LEN_1) ) 
			{
				//deb_rc("%s : rc -ir register read error! \n", __FUNCTION__);
				ret=-1;
				goto error;
			}	
			if (byte_count == 0 )  
			{	
				//ret=0;
				goto error;
			}
				
			deb_info("%s : get rc code len = %d \n", __FUNCTION__,byte_count);

			if ((byte_count%LEN_2) == 1)   byte_count+=LEN_1;	
			if (byte_count > rt_code_len)  byte_count=rt_code_len;	
					
			memset(rt_u8_code,0,rt_code_len);
			
			if ( read_rc_char_bytes( d ,RTD2832U_RC,IR_RX_BUF,rt_u8_code ,0x80) ) 
			{
				//deb_rc("%s : rc -ir register read error! \n", __FUNCTION__);
				ret=-1;
				goto error;
			}

			for (i=0;i<3;i++)
			{
				data= p_flush_table2[i].data;
				if ( write_rc_char_bytes( d ,RTD2832U_RC, p_flush_table2[i].address,&data,LEN_1) ) 
				{
					deb_rc("+%s : rc -ir register write error! \n", __FUNCTION__);
					ret=-1;
					goto error;
				}		

			}

			ret =0;
			return ret;
	}
error:
			memset(rt_u8_code,0,rt_code_len);
			byte_count=0;
			for (i=0;i<flush_step_Number;i++)
			{
				data= p_flush_table1[i].data;
				if ( write_rc_char_bytes( d ,RTD2832U_RC, p_flush_table1[i].address,&data,LEN_1) ) 
				{
					deb_rc("+%s : rc -ir register write error! \n", __FUNCTION__);
					ret=-1;
					goto error;
				}		

			}		   			
			ret =0;       					

			//deb_info("-info_%s \n", __FUNCTION__);
			return ret;
	


}