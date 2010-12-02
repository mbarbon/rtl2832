
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

//enable /disable rc 
#define RTL2832U_REMOTE_CONTROL_ENABLE       	0x00
#define RT_use_rc_protocol 			0	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int rtl2832u_remote_control_protocol=RT_use_rc_protocol;
#define RT_RC_POLLING_INTERVAL_TIME_MS	287				//rc polling interval:default 287mSec	

static struct dvb_usb_rc_key rtl2832u_rc_keys_map_table[] = {// realtek Key map   	
		{ 0x0400, KEY_0 },           // 0 
		{ 0x0401, KEY_1 },           // 1 
		{ 0x0402, KEY_2 },           // 2 
		{ 0x0403, KEY_3 },           // 3 
		{ 0x0404, KEY_4 },           // 4 
		{ 0x0405, KEY_5 },           // 5 
		{ 0x0406, KEY_6 },           // 6 
		{ 0x0407, KEY_7 },           // 7 
		{ 0x0408, KEY_8 },           // 8 
		{ 0x0409, KEY_9 },           // 9 
		{ 0x040c, KEY_POWER },       // POWER 
		{ 0x040e, KEY_MUTE },        // MUTE 
		{ 0x0410, KEY_VOLUMEUP },    // VOL UP 
		{ 0x0411, KEY_VOLUMEDOWN },  // VOL DOWN 
		{ 0x0412, KEY_CHANNELUP },   // CH UP 
		{ 0x0413, KEY_CHANNELDOWN }, // CH DOWN 
		{ 0x0416, KEY_PLAY },        // PLAY 
		{ 0x0417, KEY_RECORD },      // RECORD 
		{ 0x0418, KEY_PLAYPAUSE },   // PAUSE 
		{ 0x0419, KEY_STOP },        // STOP 
		{ 0x041e, KEY_UP},	     // UP
		{ 0x041f, KEY_DOWN},	     // DOWN
		{ 0x0420, KEY_LEFT },        // LEFT
		{ 0x0421, KEY_RIGHT },       // RIGHT
		{ 0x0422, KEY_ZOOM },        // FULL SCREEN  -->OK 
		{ 0x0447, KEY_AUDIO },       // MY AUDIO 
		{ 0x045b, KEY_MENU},         // RED 
		{ 0x045c, KEY_EPG },         // GREEN 
		{ 0x045d, KEY_FIRST },       // YELLOW
		{ 0x045e, KEY_LAST },        // BLUE
		{ 0x045a, KEY_TEXT },        // TEXT TV
	 	{ 0x0423, KEY_BACK },        // <- BACK
		{ 0x0414, KEY_FORWARD }    // >> 
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////
enum   rc_status_define{
	RC_FUNCTION_SUCCESS =0,
	RC_FUNCTION_UNSUCCESS
};

static int rtl2832u_remote_control_state=0;
static int SampleNum2TNum[] = 
{
	0,0,0,0,0,				
	1,1,1,1,1,1,1,1,1,			
	2,2,2,2,2,2,2,2,2,			
	3,3,3,3,3,3,3,3,3,			
	4,4,4,4,4,4,4,4,4,			
	5,5,5,5,5,5,5,5,			
	6,6,6,6,6,6,6,6,6,			
	7,7,7,7,7,7,7,7,7,			
	8,8,8,8,8,8,8,8,8,			
	9,9,9,9,9,9,9,9,9,			
	10,10,10,10,10,10,10,10,10,	
	11,11,11,11,11,11,11,11,11,	
	12,12,12,12,12,12,12,12,12,	
	13,13,13,13,13,13,13,13,13,	
	14,14,14,14,14,14,14		
};
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
	
int rtl2832u_remoto_control_initial_setting(struct dvb_usb_device *d)
{ 
	


	//begin setting
	int ret = RC_FUNCTION_SUCCESS;
	u8 data=0,i=0,NumberOfRcInitialTable=0;


	deb_rc("+rc_%s\n", __FUNCTION__);

	NumberOfRcInitialTable = sizeof(p_rtl2832u_rc_initial_table)/sizeof(RT_rc_set_reg_struct);
	

	for (i=0;i<NumberOfRcInitialTable;i++)
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


static int frt0(u8* rt_uccode,u8 byte_num,u8 *p_uccode)
{
	u8 *pCode = rt_uccode;
	int TNum =0;
	u8   ucBits[frt0_BITS_NUM];
	//u16  ucdata[frt0_BITS_NUM];
	u8  i=0,state=WAITING_6T;
	int  LastTNum = 0,CurrentBit = 0;
	int ret=RC_FUNCTION_SUCCESS;
	u8 highestBit = 0,lowBits=0;
	u32 scancode=0;
	
	if(byte_num < frt0_para1)
	{
		deb_rc("Bad rt uc code received, byte_num is error\n");
		ret= RC_FUNCTION_UNSUCCESS;
		goto error;
	}
	while(byte_num > 0)
	{

		highestBit = (*pCode)&0x80;
		lowBits = (*pCode) & 0x7f;
		TNum=SampleNum2TNum[lowBits];
		
		if(highestBit != 0)	TNum = -TNum;

		pCode++;
		byte_num--;

		if(TNum <= -6)	 state = WAITING_6T;

		if(WAITING_6T == state)
		{
			if(TNum <= -6)	state = WAITING_2T_AFTER_6T;
		}
		else if(WAITING_2T_AFTER_6T == state)
		{
			if(2 == TNum)	
			{
				state = WAITING_NORMAL_BITS;
				LastTNum   = 0;
				CurrentBit = 0;
			}
			else 	state = WAITING_6T;
		} 
		else if(WAITING_NORMAL_BITS == state)
		{
			if(0 == LastTNum)	LastTNum = TNum;
			else
			{
				if(LastTNum < 0)	ucBits[CurrentBit]=1;
				else			ucBits[CurrentBit]=0;

				CurrentBit++;

				if(CurrentBit >= frt0_BITS_NUM)
				{
 					deb_rc("Bad frame received, bits num is error\n");
					CurrentBit = frt0_BITS_NUM -1 ;

				}
				if(TNum > 3)
				{
						for(i=0;i<frt0_para2;i++){
							if (ucBits[i+frt0_para4])	scancode  |= (0x01 << (frt0_para2-i-1));
						}	
				}
				else{
					LastTNum += TNum;	
				}							
			}			
		}	

	}
	//output

	deb_info("-info_%s 2::rc6:scancode = %x  \n", __FUNCTION__,scancode);
	p_uccode[0]=(u8)((scancode>>24)  &  frt0_BITS_mask0);
	p_uccode[1]=(u8)((scancode>>16)  &  frt0_BITS_mask1);
	p_uccode[2]=(u8)((scancode>>8)  & frt0_BITS_mask2);
	p_uccode[3]=(u8)((scancode>>0)  & frt0_BITS_mask3);
	
	deb_info("-info_%s 3::rc6:%x %x %x %x \n", __FUNCTION__,p_uccode[0],p_uccode[1],p_uccode[2],p_uccode[3]);
	ret= RC_FUNCTION_SUCCESS;
error:

	return ret;
}


static int frt1(u8* rt_uccode,u8 byte_num,u8 *p_uccode)
{
	u8 *pCode = rt_uccode;
	u8  ucBits[frt1_BITS_NUM];
	u8 i=0,CurrentBit=0,index=0;
	u32 scancode=0;
	int ret= RC_FUNCTION_SUCCESS;

	deb_rc("+info_%s \n", __FUNCTION__);
	if(byte_num < frt1_para1)
	{
		deb_rc("Bad rt uc code received, byte_num = %d is error\n",byte_num);
		ret = RC_FUNCTION_UNSUCCESS;
		goto error;
	}
	
	memset(ucBits,0,frt1_BITS_NUM);		

	for(i = 0; i < byte_num; i++)
	{
		if ((pCode[i] & frt1_para2)< frt1_para3)    index=frt1_para5 ;   
		else 					    index=frt1_para6 ;  

		ucBits[i]= (pCode[i] & 0x80) + index;
	}
	deb_rc("+rc_%s,%x %x %x [1]\n", __FUNCTION__,ucBits[0],ucBits[1],ucBits[2]);
	if(ucBits[0] !=frt1_para_uc_1 && ucBits[0] !=frt1_para_uc_2 )   {ret= RC_FUNCTION_UNSUCCESS; goto error;}

	if(ucBits[1] !=frt1_para5  && ucBits[1] !=frt1_para6)   	{ret= RC_FUNCTION_UNSUCCESS;goto error;}

	if(ucBits[2] >= frt1_para_uc_1)  				ucBits[2] -= 0x01;
	else			 					{ret= RC_FUNCTION_UNSUCCESS;goto error;}

	
   	i = 0x02;
	CurrentBit = 0x00;

	while(i < byte_num-1)
	{
		if(CurrentBit >= 32)		break;

		if((ucBits[i] & 0x0f) == 0x0)
		{
			i++;
			continue;
		}
		if(ucBits[i++] == 0x81)  
		{
			if(ucBits[i] >=0x01)	scancode |= 0x00 << (31 - CurrentBit++); 
				

						
			
		}
		else
		{
			if(ucBits[i] >=0x81)	scancode |= 0x01 << (31 - CurrentBit++); 
			
		}
				
		ucBits[i] -= 0x01;
		continue;
	}
       //mask process
	p_uccode[3]=(u8)((scancode>>16)  &  frt1_BITS_mask3);
	p_uccode[2]=(u8)((scancode>>24)  &  frt1_BITS_mask2);
	p_uccode[1]=(u8)((scancode>>8)   &  frt1_BITS_mask1);
	p_uccode[0]=(u8)((scancode>>0)   &  frt1_BITS_mask0);

	
	deb_rc("-info_%s rc5:%x %x %x %x -->scancode =%x\n", __FUNCTION__,p_uccode[0],p_uccode[1],p_uccode[2],p_uccode[3],scancode);
	ret= RC_FUNCTION_SUCCESS;
error:


	return ret;
}

static int frt2(u8* rt_uccode,u8 byte_num,u8 *p_uccode)
{
	u8 *pCode = rt_uccode;
	u8  i=0;
	u32 scancode=0;
	u8  out_io=0;
			
	int ret= RC_FUNCTION_SUCCESS;

	deb_info("+info_%s \n", __FUNCTION__);

	if(byte_num < frt2_para1)  				goto error;
    	if(pCode[0] != frt2_para2) 				goto error;
	if((pCode[1] <frt2_para3 )||(pCode[1] >frt2_para4))	goto error;	


	if( (pCode[2] <frt2_para5 ) && (pCode[2] >frt2_para6) )   
	{ 

		if( (pCode[3] <frt2_para7 ) && (pCode[3] >frt2_para8 ) &&(pCode[4]==frt2_para9 ))  scancode=0xffff;
		else goto error;

	}
	else if( (pCode[2] <frt2_para10  ) && (pCode[2] >frt2_para11 ) ) 
	{

	 	for (i = 3; i <68; i++)
		{  
                        if ((i% 2)==1)
			{
				if( (pCode[i]>frt2_para7 ) || (pCode[i] <frt2_para8 ) )  
				{ 
					deb_rc("Bad rt uc code received[4]\n");
					ret= RC_FUNCTION_UNSUCCESS;
					goto error;
				}			
			}
			else
			{
				if(pCode[i]<frt2_para12  )  out_io=0;
				else			    out_io=1;
				scancode |= (out_io << (31 -(i-4)/2) );
			}
		} 



	}
	else  	goto error;
	deb_rc("-info_%s nec:%x\n", __FUNCTION__,scancode);
	p_uccode[0]=(u8)((scancode>>24)  &  frt2_BITS_mask0);
	p_uccode[1]=(u8)((scancode>>16)  &  frt2_BITS_mask1);
	p_uccode[2]=(u8)((scancode>>8)   &  frt2_BITS_mask2);
	p_uccode[3]=(u8)((scancode>>0)   &  frt2_BITS_mask3);
	ret= RC_FUNCTION_SUCCESS;
error:	

	return ret;
}
#define receiveMaskFlag1  0x80
#define receiveMaskFlag2  0x03
#define flush_step_Number 0x05
#define rt_code_len       0x80  
static int rtl2832u_rc_query(struct dvb_usb_device *d, u32 *event, int *state)
{

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
				
			//deb_rc("%s : get rc code len = %d \n", __FUNCTION__,byte_count);
			if ((byte_count%LEN_2) == 1)   byte_count+=LEN_1;	
			if (byte_count > rt_code_len)  byte_count=rt_code_len;	
					
			memset(rt_u8_code,0,rt_code_len);
			
			if ( read_rc_char_bytes( d ,RTD2832U_RC,IR_RX_BUF,rt_u8_code ,0x80) ) 
			{
				//deb_rc("%s : rc -ir register read error! \n", __FUNCTION__);
				ret=-1;
				goto error;
			}
				
			memset(ucode,0,4);
		
			
			ret=0;
			if (rtl2832u_remote_control_protocol == RT_RC6)			ret =frt0(rt_u8_code,byte_count,ucode);
			else if (rtl2832u_remote_control_protocol == RT_RC5)		ret =frt1(rt_u8_code,byte_count,ucode);
			else if (rtl2832u_remote_control_protocol == RT_NEC)		ret =frt2(rt_u8_code,byte_count,ucode);	
			else  
			{
					//deb_rc("%s : rc - unknow rc protocol set ! \n", __FUNCTION__);
					ret=-1;
					goto error;	
			}
			
			if((ret != RC_FUNCTION_SUCCESS) || (ucode[0] ==0 && ucode[1] ==0 && ucode[2] ==0 && ucode[3] ==0))   
 			{
					//deb_rc("%s : rc-rc is error scan code ! %x %x %x %x \n", __FUNCTION__,ucode[0],ucode[1],ucode[2],ucode[3]);
					ret=-1;
					goto error;	
			}
			scancode=(ucode[2]<<8) | ucode[3] ;
			deb_info("-%s scan code %x %x %x %x,(0x%x) -- len=%d\n", __FUNCTION__,ucode[0],ucode[1],ucode[2],ucode[3],scancode,byte_count);
			////////// map/////////////////////////////////////////////////////////////////////////////////////////////////////
			for (i = 0; i < ARRAY_SIZE(rtl2832u_rc_keys_map_table); i++) 
			{
				if(rtl2832u_rc_keys_map_table[i].scan ==scancode )
				{
					*event = rtl2832u_rc_keys_map_table[i].event;
					*state = REMOTE_KEY_PRESSED;
					deb_rc("%s : map number = %d \n", __FUNCTION__,i);	
					break;
				}		
				
			}

			memset(rt_u8_code,0,rt_code_len);
			byte_count=0;
			//deb_info("-info_%s :2:%x %x %x %x -- LEN=%d\n", __FUNCTION__,ucode[0],ucode[1],ucode[2],ucode[3],byte_count);
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
///////////////////////////////////////////////////////////////////////////////////////////////////////
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


static struct dvb_usb_device_properties rtl2832u_1st_properties;
static struct dvb_usb_device_properties rtl2832u_2nd_properties;
static struct dvb_usb_device_properties rtl2832u_3th_properties;
static struct dvb_usb_device_properties rtl2832u_4th_properties;


//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
static int rtl2832u_usb_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	if ( ( 0== dvb_usb_device_init(intf,&rtl2832u_1st_properties,THIS_MODULE,NULL,adapter_nr) )||
		( 0== dvb_usb_device_init(intf,&rtl2832u_2nd_properties,THIS_MODULE,NULL,adapter_nr) ) ||
		( 0== dvb_usb_device_init(intf,&rtl2832u_3th_properties,THIS_MODULE,NULL,adapter_nr) ) ||
		( 0== dvb_usb_device_init(intf,&rtl2832u_4th_properties,THIS_MODULE,NULL,adapter_nr) ) )
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

static struct dvb_usb_device_properties rtl2832u_1st_properties = {

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
	#if (RTL2832U_REMOTE_CONTROL_ENABLE == 1) 
	
	//remote control
	.rc_interval      = RT_RC_POLLING_INTERVAL_TIME_MS,		
	.rc_key_map       = rtl2832u_rc_keys_map_table,			//user define key map
	.rc_key_map_size  = ARRAY_SIZE(rtl2832u_rc_keys_map_table),	//user define key map size	
	.rc_query         = rtl2832u_rc_query,				//use define quary function
	#endif
	
	.num_device_descs = 6,
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
		{ NULL },
	}
};


static struct dvb_usb_device_properties rtl2832u_2nd_properties = {

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
	#if (RTL2832U_REMOTE_CONTROL_ENABLE == 1) 
	//remote control
	.rc_interval      = RT_RC_POLLING_INTERVAL_TIME_MS,		
	.rc_key_map       = rtl2832u_rc_keys_map_table,			//user define key map
	.rc_key_map_size  = ARRAY_SIZE(rtl2832u_rc_keys_map_table),	//user define key map size	
	.rc_query         = rtl2832u_rc_query,				//use define quary function
	#endif
	.num_device_descs = 6,
	.devices = {
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
		{ NULL },
	}
};



static struct dvb_usb_device_properties rtl2832u_3th_properties = {

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
	

	#if (RTL2832U_REMOTE_CONTROL_ENABLE == 1) 
	//remote control
	.rc_interval      = RT_RC_POLLING_INTERVAL_TIME_MS,		
	.rc_key_map       = rtl2832u_rc_keys_map_table,			//user define key map
	.rc_key_map_size  = ARRAY_SIZE(rtl2832u_rc_keys_map_table),	//user define key map size	
	.rc_query         = rtl2832u_rc_query,				//use define quary function
	#endif
	.num_device_descs = 9,
	.devices = {
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
		}
	}
};


static struct dvb_usb_device_properties rtl2832u_4th_properties = {

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
	#if (RTL2832U_REMOTE_CONTROL_ENABLE == 1) 
	//remote control
	.rc_interval      = RT_RC_POLLING_INTERVAL_TIME_MS,		
	.rc_key_map       = rtl2832u_rc_keys_map_table,			//user define key map
	.rc_key_map_size  = ARRAY_SIZE(rtl2832u_rc_keys_map_table),	//user define key map size	
	.rc_query         = rtl2832u_rc_query,				//use define quary function
	#endif	
	.num_device_descs = 6,
	.devices = {
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
