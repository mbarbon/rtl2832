
#ifndef _RTL2832U_H_
#define _RTL2832U_H_



#include "dvb-usb.h"

#define	USB_VID_REALTEK					0x0BDA
#define	USB_PID_RTD2832U_WARM			0x2832 
#define	USB_PID_RTD2832U_2ND_WARM		0x2838 
#define	USB_PID_RTD2832U_3RD_WARM		0x2834 
#define   USB_PID_RTD2832U_4TH_WARM          0x2837
#define   USB_PID_RTL2836_WARM                     0x2836
#define   USB_PID_RTL2836_2ND_WARM             0x2839

#define	USB_VID_GOLDENBRIDGE				0x1680
#define	USB_PID_GOLDENBRIDGE_WARM		0xA332

#define	USB_VID_YUAN						0x1164
#define	USB_PID_YUAN_WARM				0x6601

#define	USB_VID_AZUREWAVE_2				0x13D3
#define	USB_PID_AZUREWAVE_MINI_WARM	0x3234
#define	USB_PID_AZUREWAVE_USB_WARM		0x3274
#define	USB_PID_AZUREWAVE_GPS_WARM		0x3282

#define	USB_VID_KWORLD_1ST					0x1B80
#define	USB_PID_KWORLD_WARM_4				0xD394
#define	USB_PID_KWORLD_WARM_6				0xD396
#define	USB_PID_KWORLD_WARM_3				0xD393
#define	USB_PID_KWORLD_WARM_7				0xD397
#define	USB_PID_KWORLD_WARM_8				0xD398

#define	USB_VID_DEXATEK					0x1D19
#define	USB_PID_DEXATEK_USB_WARM		0x1101
#define	USB_PID_DEXATEK_MINIUSB_WARM	0x1102
#define	USB_PID_DEXATEK_5217_WARM		0x1103
#define	USB_PID_DEXATEK_WARM_4		       0x1104

#define	USB_VID_GTEK					       0x1F4D
#define	USB_PID_GTEK_WARM			       0x0837
#define	USB_PID_GTEK_WARM_A			       0xA803
#define	USB_PID_GTEK_WARM_B			       0xB803
#define	USB_PID_GTEK_WARM_C			       0xC803
#define	USB_PID_GTEK_WARM_D			       0xD803

#define	USB_VID_THP					       0x1554
#define   USB_PID_THP                                         0x5013
#define	USB_PID_THP2					       0x5020

#define RTD2831_URB_SIZE					4096// 39480
#define RTD2831_URB_NUMBER				10 //  4

///////////////////////////////////////////////////////////////////////////////////////////////////////
//			remote control 
///////////////////////////////////////////////////////////////////////////////////////////////////////


//define rtl283u rc register address
#define IR_RX_BUF			0xFC00
#define IR_RX_IE			0xFD00
#define IR_RX_IF			0xFD01
#define IR_RX_CTRL			0xFD02
#define IR_RX_CONFIG			0xFD03
#define IR_MAX_DURATION0		0xFD04
#define IR_MAX_DURATION1		0xFD05
#define IR_IDLE_LEN0			0xFD06
#define IR_IDLE_LEN1			0xFD07
#define IR_GLITCH_LEN			0xFD08
#define IR_RX_BUFFER_CTRL		0xFD09
#define IR_RX_BUFFER_DATA		0xFD0A
#define IR_RX_BC			0xFD0B
#define IR_RX_CLK			0xFD0C
#define IR_RX_C_COUNT_L			0xFD0D
#define IR_RX_C_COUNT_H			0xFD0E

#define IR_SUSPEND_CTRL			0xFD10
#define IR_Err_Tolerance_CTRL		0xFD11
#define IR_UNIT_LEN			0xFD12
#define IR_Err_Tolerance_LEN		0xFD13
#define IR_MAX_H_Tolerance_LEN		0xFD14
#define IR_MAX_L_Tolerance_LEN		0xFD15
#define IR_MASK_CTRL			0xFD16
#define IR_MASK_DATA			0xFD17
#define IR_RESUME_MASK_ADDR		0xFD18
#define IR_RESUME_MASK_T_LEN		0xFD19

#define USB_CTRL			0x0010
#define SYS_GPD				0x0004
#define SYS_GPOE			0x0003
#define SYS_GPO				0x0001
#define RC_USE_DEMOD_CTL1		0x000B

//define use len 
#define LEN_1				0x01
#define LEN_2				0x02
#define LEN_3				0x03
#define LEN_4				0x04



//function
int rtl2832u_remoto_control_initial_setting(struct dvb_usb_device *d);
#define	USB_EPA_CTL	0x0148

///////////////////////////////////////////////////////////////////////////////////////////////////////
//decode
#define frt0_para1 	0x3c
#define frt0_para2 	0x20
#define frt0_para3	0x7f
#define frt0_para4      0x05
#define frt0_BITS_NUM 	0x80
#define frt0_BITS_mask 	0x01
#define frt0_BITS_mask0	0x00
#define frt0_BITS_mask1	0x00
#define frt0_BITS_mask2	0x0f
#define frt0_BITS_mask3	0xff

#define frt1_para1 0x12
#define frt1_para3 0x1a
#define frt1_para2 0x7f
#define frt1_para4 0x80
#define frt1_para5 0x01
#define frt1_para6 0x02
#define frt1_BITS_NUM 0x80
#define frt1_para_uc_1 0x81
#define frt1_para_uc_2 0x82
#define frt1_BITS_mask0	0x00
#define frt1_BITS_mask1	0x00
#define frt1_BITS_mask2	0x7f
#define frt1_BITS_mask3	0xff

#define frt2_para1  0x0a
#define frt2_para2  0xFF
#define frt2_para3  0xb0
#define frt2_para4  0xc6
#define frt2_para5  0x30
#define frt2_para6  0x1b
#define frt2_para7  0x8f
#define frt2_para8  0x89
#define frt2_para9  0x7f
#define frt2_para10 0x60
#define frt2_para11 0x38
#define frt2_para12  0x15
#define frt2_BITS_NUM	0x80
#define frt2_BITS_mask0	0x00
#define frt2_BITS_mask1	0x00
#define frt2_BITS_mask2	0xff
#define frt2_BITS_mask3	0xff
///////////////////////////////////////////////////////////////////////////////////////////////////////
//			remote control 
///////////////////////////////////////////////////////////////////////////////////////////////////////
enum protocol_type_for_RT{
				RT_RC6=0,
				RT_RC5,
				RT_NEC,
				RT_TEST
};
// op define	
enum OP{
	OP_NO	=0,
	OP_AND	  ,
	OP_OR
};

typedef enum RT_UC_CODE_STATE{
	WAITING_6T,
	WAITING_2T_AFTER_6T,
	WAITING_NORMAL_BITS
}RT_UC_CODE_STATE;

//struct define
typedef struct RT_rc_set_reg_struct{	
		u8	type;	
		u16 	address;
		u8      data;
		u8	op;
		u8	op_mask;	
}RT_rc_set_reg_struct;

enum RTL2832U_RC_STATE{
	RC_NO_SETTING=0,
	RC_INSTALL_OK,
	RC__POLLING_OK,
	RC_STOP_OK
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct dvb_frontend * rtl2832u_fe_attach(struct dvb_usb_device *d);

#endif



