/**

@file

@brief   RTL2836 MxL5007T NIM module definition

One can manipulate RTL2836 MxL5007T NIM through RTL2836 MxL5007T NIM module.
RTL2836 MxL5007T NIM module is derived from DTMB NIM module.

*/


#include "nim_rtl2836_mxl5007t.h"





/**

@brief   RTL2836 MxL5007T NIM module builder

Use BuildRtl2836Mxl5007tModule() to build RTL2836 MxL5007T NIM module, set all module function pointers with the
corresponding functions, and initialize module private variables.


@param [in]   ppNim                        Pointer to RTL2836 MxL5007T NIM module pointer
@param [in]   pDtmbNimModuleMemory         Pointer to an allocated DTMB NIM module memory
@param [in]   I2cReadingByteNumMax         Maximum I2C reading byte number for basic I2C reading function
@param [in]   I2cWritingByteNumMax         Maximum I2C writing byte number for basic I2C writing function
@param [in]   I2cRead                      Basic I2C reading function pointer
@param [in]   I2cWrite                     Basic I2C writing function pointer
@param [in]   WaitMs                       Basic waiting function pointer
@param [in]   pRtl2836ExtraModuleMemory    Pointer to an allocated RTL2836 extra module memory
@param [in]   DemodDeviceAddr              RTL2836 I2C device address
@param [in]   DemodCrystalFreqHz           RTL2836 crystal frequency in Hz
@param [in]   DemodTsInterfaceMode         RTL2836 TS interface mode
@param [in]   DemodUpdateFuncRefPeriodMs   RTL2836 update function reference period in millisecond
@param [in]   DemodIsFunc1Enabled          RTL2836 Function 1 enabling status for setting
@param [in]   pMxl5007tExtraModuleMemory   Pointer to an allocated MxL5007T extra module memory
@param [in]   TunerDeviceAddr              MxL5007T I2C device address
@param [in]   TunerCrystalFreqHz           MxL5007T crystal frequency in Hz
@param [in]   TunerLoopThroughMode         MxL5007T loop-through mode
@param [in]   TunerClkOutMode              MxL5007T clock output mode
@param [in]   TunerClkOutAmpMode           MxL5007T clock output amplitude mode


@note
	-# One should call BuildRtl2836Mxl5007tModule() to build RTL2836 MxL5007T NIM module before using it.

*/
void
BuildRtl2836Mxl5007tModule(
	DTMB_NIM_MODULE **ppNim,									// DTMB NIM dependence
	DTMB_NIM_MODULE *pDtmbNimModuleMemory,

	unsigned char I2cReadingByteNumMax,							// Base interface dependence
	unsigned char I2cWritingByteNumMax,
	BASE_FP_I2C_READ I2cRead,
	BASE_FP_I2C_WRITE I2cWrite,
	BASE_FP_WAIT_MS WaitMs,

	RTL2836_EXTRA_MODULE *pRtl2836ExtraModuleMemory,			// Demod dependence
	unsigned char DemodDeviceAddr,
	unsigned long DemodCrystalFreqHz,
	int DemodTsInterfaceMode,
	unsigned long DemodUpdateFuncRefPeriodMs,
	int DemodIsFunc1Enabled,

	MXL5007T_EXTRA_MODULE *pMxl5007tExtraModuleMemory,			// Tuner dependence
	unsigned char TunerDeviceAddr,
	unsigned long TunerCrystalFreqHz,
	int TunerLoopThroughMode,
	int TunerClkOutMode,
	int TunerClkOutAmpMode
	)
{
	DTMB_NIM_MODULE *pNim;



	// Set NIM module pointer with NIM module memory.
	*ppNim = pDtmbNimModuleMemory;
	
	// Get NIM module.
	pNim = *ppNim;

	// Set I2C bridge module pointer with I2C bridge module memory.
	pNim->pI2cBridge = &pNim->I2cBridgeModuleMemory;

	// Set NIM extra module pointer.
	pNim->pExtra = INVALID_POINTER_VALUE;


	// Set NIM type.
	pNim->NimType = DTMB_NIM_RTL2836_MXL5007T;


	// Build base interface module.
	BuildBaseInterface(
		&pNim->pBaseInterface,
		&pNim->BaseInterfaceModuleMemory,
		I2cReadingByteNumMax,
		I2cWritingByteNumMax,
		I2cRead,
		I2cWrite,
		WaitMs
		);

	// Build RTL2836 demod module.
	BuildRtl2836Module(
		&pNim->pDemod,
		&pNim->DtmbDemodModuleMemory,
		pRtl2836ExtraModuleMemory,
		&pNim->BaseInterfaceModuleMemory,
		&pNim->I2cBridgeModuleMemory,
		DemodDeviceAddr,
		DemodCrystalFreqHz,
		DemodUpdateFuncRefPeriodMs,
		DemodIsFunc1Enabled
		);

	// Build MxL5007T tuner module.
	BuildMxl5007tModule(
		&pNim->pTuner,
		&pNim->TunerModuleMemory,
		pMxl5007tExtraModuleMemory,
		&pNim->BaseInterfaceModuleMemory,
		&pNim->I2cBridgeModuleMemory,
		TunerDeviceAddr,
		TunerCrystalFreqHz,
		RTL2836_MXL5007T_STANDARD_MODE_DEFAULT,
		RTL2836_MXL5007T_IF_FREQ_HZ_DEFAULT,
		RTL2836_MXL5007T_SPECTRUM_MODE_DEFAULT,
		TunerLoopThroughMode,
		TunerClkOutMode,
		TunerClkOutAmpMode,
		RTL2836_MXL5007T_QAM_IF_DIFF_OUT_LEVEL_DEFAULT
		);


	// Set NIM module variables.
	pNim->DemodTsInterfaceMode = DemodTsInterfaceMode;


	// Set NIM module function pointers with default functions.
	pNim->GetNimType        = dtmb_nim_default_GetNimType;
	pNim->GetParameters     = dtmb_nim_default_GetParameters;
	pNim->IsSignalPresent   = dtmb_nim_default_IsSignalPresent;
	pNim->IsSignalLocked    = dtmb_nim_default_IsSignalLocked;
	pNim->GetSignalStrength = dtmb_nim_default_GetSignalStrength;
	pNim->GetSignalQuality  = dtmb_nim_default_GetSignalQuality;
	pNim->GetBer            = dtmb_nim_default_GetBer;
	pNim->GetPer            = dtmb_nim_default_GetPer;
	pNim->GetSnrDb          = dtmb_nim_default_GetSnrDb;
	pNim->GetTrOffsetPpm    = dtmb_nim_default_GetTrOffsetPpm;
	pNim->GetCrOffsetHz     = dtmb_nim_default_GetCrOffsetHz;
	pNim->GetSignalInfo     = dtmb_nim_default_GetSignalInfo;
	pNim->UpdateFunction    = dtmb_nim_default_UpdateFunction;

	// Set NIM module function pointers with particular functions.
	pNim->Initialize     = rtl2836_mxl5007t_Initialize;
	pNim->SetParameters  = rtl2836_mxl5007t_SetParameters;


	return;
}





/**

@see   DTMB_NIM_FP_INITIALIZE

*/
int
rtl2836_mxl5007t_Initialize(
	DTMB_NIM_MODULE *pNim
	)
{
	typedef struct
	{
		int RegBitName;
		unsigned long Value;
	}
	REG_VALUE_ENTRY;


	static const REG_VALUE_ENTRY AdditionalInitRegValueTable[RTL2836_MXL5007T_ADDITIONAL_INIT_REG_TABLE_LEN] =
	{
		// RegBitName,				Value
		{DTMB_TARGET_VAL,			0x38	},
	};


	TUNER_MODULE *pTuner;
	DTMB_DEMOD_MODULE *pDemod;

	MXL5007T_EXTRA_MODULE *pTunerExtra;

	int i;

	int RegBitName;
	unsigned long Value;



	// Get tuner module and demod module.
	pTuner = pNim->pTuner;
	pDemod = pNim->pDemod;

	// Get tuner extra module.
	pTunerExtra = (MXL5007T_EXTRA_MODULE *)pTuner->pExtra;


	// Enable demod DTMB_I2CT_EN_CTRL.
	if(pDemod->SetRegBitsWithPage(pDemod, DTMB_I2CT_EN_CTRL, 0x1) != FUNCTION_SUCCESS)
		goto error_status_set_registers;

	// Initialize tuner.
	if(pTuner->Initialize(pTuner) != FUNCTION_SUCCESS)
		goto error_status_execute_function;

	// Set tuner bandwidth mode with 8 MHz.
	if(pTunerExtra->SetBandwidthMode(pTuner, MXL5007T_BANDWIDTH_8000000HZ) != FUNCTION_SUCCESS)
		goto error_status_execute_function;

	// Disable demod DTMB_I2CT_EN_CTRL.
	if(pDemod->SetRegBitsWithPage(pDemod, DTMB_I2CT_EN_CTRL, 0x0) != FUNCTION_SUCCESS)
		goto error_status_set_registers;


	// Initialize demod.
	if(pDemod->Initialize(pDemod) != FUNCTION_SUCCESS)
		goto error_status_execute_function;

	// Set demod IF frequency with NIM default.
	if(pDemod->SetIfFreqHz(pDemod, RTL2836_MXL5007T_IF_FREQ_HZ_DEFAULT) != FUNCTION_SUCCESS)
		goto error_status_execute_function;

	// Set demod spectrum mode with NIM default.
	if(pDemod->SetSpectrumMode(pDemod, RTL2836_MXL5007T_SPECTRUM_MODE_DEFAULT) != FUNCTION_SUCCESS)
		goto error_status_execute_function;


	// Set demod registers.
	for(i = 0; i < RTL2836_MXL5007T_ADDITIONAL_INIT_REG_TABLE_LEN; i++)
	{
		// Get register bit name and its value.
		RegBitName = AdditionalInitRegValueTable[i].RegBitName;
		Value      = AdditionalInitRegValueTable[i].Value;

		// Set demod registers
		if(pDemod->SetRegBitsWithPage(pDemod, RegBitName, Value) != FUNCTION_SUCCESS)
			goto error_status_set_registers;
	}


	// Set TS interface according to TS interface mode.
	switch(pNim->DemodTsInterfaceMode)
	{
		case TS_INTERFACE_PARALLEL:

			// Set demod TS interface with parallel mode.
			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_SERIAL,   0) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_CDIV_PH0, 15) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_CDIV_PH1, 15) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			break;


		default:
		case TS_INTERFACE_SERIAL:

			// Set demod TS interface with serial mode.
			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_SERIAL,   1) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_CDIV_PH0, 1) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			if(pDemod->SetRegBitsWithPage(pDemod, DTMB_CDIV_PH1, 1) != FUNCTION_SUCCESS)
				goto error_status_set_registers;

			break;
	}


	return FUNCTION_SUCCESS;


error_status_execute_function:
error_status_set_registers:
	return FUNCTION_ERROR;
}





/**

@see   DTMB_NIM_FP_SET_PARAMETERS

*/
int
rtl2836_mxl5007t_SetParameters(
	DTMB_NIM_MODULE *pNim,
	unsigned long RfFreqHz
	)
{
	TUNER_MODULE *pTuner;
	DTMB_DEMOD_MODULE *pDemod;
    MXL5007T_EXTRA_MODULE *pTunerExtra;

	// Get tuner module and demod module.
	pTuner = pNim->pTuner;
	pDemod = pNim->pDemod;

	// Get tuner extra module.
	pTunerExtra = (MXL5007T_EXTRA_MODULE *)pTuner->pExtra;


	// Enable demod DTMB_I2CT_EN_CTRL.
	if(pDemod->SetRegBitsWithPage(pDemod, DTMB_I2CT_EN_CTRL, 0x1) != FUNCTION_SUCCESS)
		goto error_status_set_registers;

	// Set tuner RF frequency in Hz.
	if(pTuner->SetRfFreqHz(pTuner, RfFreqHz) != FUNCTION_SUCCESS)
		goto error_status_execute_function;


        // Set tuner bandwidth mode with TunerBandwidthMode.
	if(pTunerExtra->SetBandwidthMode(pTuner, MXL5007T_BANDWIDTH_8000000HZ) != FUNCTION_SUCCESS)
		goto error_status_execute_function;

	// Disable demod DTMB_I2CT_EN_CTRL.
	if(pDemod->SetRegBitsWithPage(pDemod, DTMB_I2CT_EN_CTRL, 0x0) != FUNCTION_SUCCESS)
		goto error_status_set_registers;


	// Reset demod particular registers.
	if(pDemod->ResetFunction(pDemod) != FUNCTION_SUCCESS)
		goto error_status_execute_function;


	// Reset demod by software reset.
	if(pDemod->SoftwareReset(pDemod) != FUNCTION_SUCCESS)
		goto error_status_execute_function;


	return FUNCTION_SUCCESS;


error_status_execute_function:
error_status_set_registers:
	return FUNCTION_ERROR;
}





