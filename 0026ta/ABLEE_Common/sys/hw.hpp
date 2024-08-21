#ifndef CHW_HPP
#define CHW_HPP

#include <iostream>
#include <string>
#include "basestruct.hpp"
#include "sysdef.hpp"
#include "testlib_global.h"

#define ADC_5V_RESOLUTION           (10.0/65535.0)
#define ADC_10V_RESOLUTION          (20.0/65535.0)
#define ADC_RESOLUTION              (ADC_10V_RESOLUTION)
#define MAX_ADC                     (8)

#define DAC_DEFAULT_MIN				(0.1)
#define DAC_VREFIN					(5.0)
#define DAC_VOUT(v)					(v/DAC_VREFIN)*65535.0
#define DAC_VIH2_GAIN				(4.0)

#define DAC_VIH1_MIN				(0.0)	//2023.09.15 - KWMOON(0.8 -> 0.0)
#define DAC_VIH2_MIN				(0.0)
#define DAC_VOH1_MIN				(0.3)
#define DAC_VOH2_MIN				(0.3)
#define DAC_CMP_IN_MIN              (0.0)	//2022.09.28

#define DAC_VIH1_MAX				( 3.8)
#define DAC_VIH2_MAX				(12.1)
#define DAC_VOH1_MAX				( 5.0)	//( 2.1)
#define DAC_VOH2_MAX				( 5.0)	//( 2.1)
#define DAC_CMP_IN_MAX              ( 3.7)	//2022.09.28

#define MAX_EEPROM_ADDR				(0x7fff)

enum ADC_MUX
{
	MUX_AGND              = 0x0,
	MUX_V2_5P                  ,
	MUX_V5_0P                  ,
	MUX_GND                    ,
	MUX_SYS_V20P               ,
	MUX_SYS_V48P               ,
	MUX_SYS_V1_0P              ,
	MUX_VIH                    ,
	MUX_PPS_F                  ,
	MUX_PPS_S                  ,
	MUX_S1PS_F                 ,
	MUX_S1PS_S                 ,
	MUX_PPS_GND1               ,
	MUX_PPS_GND2               ,
	MUX_PPS_GND3               ,
	MUX_PPS_GND4               ,
	ADC_MUX_MAX                ,
};

enum ADC_CH
{
	ADC_CH0              = 0x0,
	ADC_CH1                   ,
	ADC_CH2                   ,
	ADC_CH3                   ,
	ADC_CH4                   ,
	ADC_CH5                   ,
	ADC_CH6                   ,
	ADC_CH7                   ,
	ADC_CH_MAX                ,
};

enum DAC_COMMAND
{
	WRITE_TO_INPUT_REG       = 0x0,
	UPDATE_DAC_REGISTER           ,
	WRITE_TO_INPUT_REG_UPATE_ALL  ,
	WRITE_TO_AND_UPDATE_CH        ,
	POWER_DOWN_UP_DAC             ,
	LOAD_CLEAR_CODE               ,
	LOAD_LDAC_REGISTER            ,
	RESET                         ,
	SETUP_INTERNAL_REF            ,
};

enum DAC_ADDRESS
{
	DAC_A               = 0x0 ,
	DAC_VIH1            = 0x0 ,
	DAC_B               = 0x1 ,
	DAC_VIH2            = 0x1 ,
	DAC_C               = 0x2 ,
	DAC_VOH1            = 0x2 ,
	DAC_D               = 0x3 ,
	DAC_VOH2            = 0x3 ,
	DAC_E               = 0x4 ,
	DAC_CMP_IN          = 0x4 ,
	DAC_F                     ,
	DAC_G                     ,
	DAC_H                     ,
	DAC_ALL            = 0xFF ,
};

enum DAC_CLEAR_CODE
{
	ZERO_SCALE        = 0x0  ,
	MID_SCALE                ,
	FULL_SCALE               ,
	NO_OPERATION             ,
};

enum DAC_POWER_DOWN_MODE
{
	POWER_DOWN_MODE     = 0x0  ,
	R_1KOHM_TO_GND          ,
	R_100KOM_TO_GND         ,
	THREE_STATE             ,
};

enum EEPROM_INSTRUCTION
{
	CMD_WREN          = 0x1,
	CMD_WRDI          = 0x2,
	CMD_RDSR          = 0x3,
	CMD_WRSR          = 0x4,
	CMD_READ          = 0x5,
	CMD_WRITE         = 0x6,
};

/////////////////////////////////////////////////////////////////////////
/// \brief The CHw class
class TESTLIBSHARED_EXPORT CHw
{
public:
    CHw();
public:
	bool HwInit();

//ADC(AD7606)
public:
	void AdcInit();
	void AdcTransfer();
	void AdcReset();
	void AdcClear();
	void AdcMuxSet(int);
	bool AdcConvStart();
	bool IsAdcBusy();
	void AdcRead(uint,double [MAX_ADC], uint sampleing=1);
	double AdcRead(uint,uint,uint sampleing=1);
	stAdcEmt GetAdcEmt(uint,uint,uint);
private:
	void AdcEmtInit();
	stAdcEmt m_stAdcEmt[ALL_FC][ADC_MUX_MAX][ADC_CH_MAX];
//DAC(AD5668 - MAX 5V:VREFIN )
public:
	void DacInit();
	void DacWrite( int, int, int );
	void SetDacReset();
	bool SetDacVout(int,double);
	bool SetDacUpdate(int ch=DAC_ALL);
	bool SetDacPwrDown(int, DAC_POWER_DOWN_MODE);
	void SetDacHwLoad();
	void SetDacSwClear(DAC_CLEAR_CODE);
	void SetDacHwClear();
	bool IsValidDacCh(int);
	bool IsValidVout(int,double);
public:
	bool MeasVih1(double&);
	bool MeasVth1(double&);
	bool MeasMioVolt(double&,double&);
	bool MeasVih2(double&,double&);
	bool MeasVoutDiag(double&);
	bool MeasMioVcc(double&);
	double GetSetVih();		//2023.10.10 - KWMOON
	double GetSetVth();		//2023.10.10 - KWMOON
	bool IsVihChanged();	//2023.10.23 - KWMOON
//EEPROM( AT25256B - 256kbit/32KByte)
public:
	void EepromWren();
	void EepromWrite( int, int, int bytes=1 );
	int  EepromRead (int, int bytes=1 );
	bool IsEepromReady();
	bool IsValidEepromAddr(int);
	void EepromWrdi();		//2024.02.13 - KWMOON

//SDRAM(DBM)
public:
	//void DramCtrlReset();		//2022.12.01 - KWMOON(Comment)
	void TpgIDelayCtrlReset();

private:
	double m_dVih;				//2023.09.22 - KWMOON(RFU)
	double m_dVth;				//2023.10.10 - KWMOON(RFU)
	bool   m_bVihChanged;		//2023.10.23 - KWMOON
};

#endif // HW_HPP
