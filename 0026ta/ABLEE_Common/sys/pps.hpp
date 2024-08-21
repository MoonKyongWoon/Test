#ifndef CPPS_HPP
#define CPPS_HPP

#include "testlib_global.h"
#include <map>
#include <string>
#include <vector>
#include <math.h>
#include "basestruct.hpp"
#include "sysdef.hpp"

#define MAX_PPS_CH					(     8)	//PPS & S1PS
#define EXP_BITS					(  0x17)
#define EXP_INVALID					(  9999)

#define PMBUS_MAX_VOUT				( 14.00)
#define PMBUS_MAX_VOUT_SCALE_LOOP	(     4)

#define PPS_MIN_VOUT				(  0.40)	//0.4 V
#define PPS_MAX_VOUT				(  5.00)	//5.0 V
#define PPS_MIN_IOUT				(  1.00)	//1 A
#define PPS_MAX_IOUT				( 25.00)	//20 A

#define S1PS_MIN_CUSTOMER			(  0.50)	//0.5 V
#define S1PS_MIN_VOUT				(  0.50)	//0.4 V
#define S1PS_MAX_VOUT				( 15.01)	//15.0 V
#define S1PS_MIN_IOUT				(  1.00)	//1 A
#define S1PS_MAX_IOUT				( 25.00)	//( 5.0)	//5 A
#define S1PS_VIN_OV_FAULT			( 22.00)	//5 A
#define S1PS_VIN_UV_WARN			(  8.00)	//5 A

#define PG_ON_VOLTAGE				(  0.30)
#define PG_OFF_VOLTAGE				(  0.24)
#define TON_RISE_TIME				( 10.00)	//10 MS
#define TON_MAX_TIME				( 20.00)	//20 MS
#define TOFF_FALL_TIME				( 50.00)	//50 MS
#define TOFF_MAX_TIME				( 60.00)	//100 MS

#define POLYNOMIAL					( 0x107)	//CRC(PEC-SMBUS Spec)
#define CHECKSUM_ERROR				(  0x99)

#define S1PS_ATT_GAIN0				(1.0000)	//2023.02.27 - KWMOON (1.2637) -> 1.0
#define S1PS_ATT_GAIN1				(1.2666)	//2023.02.27 - KWMOON (1.2634) -> 1.2666
#define S1PS_ATT_GAIN2				(1.2689)	//2023.02.27 - KWMOON (1.2657) -> 1.2689
#define S1PS_ATT_GAIN3				(1.3805)	//2023.02.27 - KWMOON (1.3771) -> 1.3805

#define S1PS_ATT_OFFSET0			( 0.0000)	//2023.02.27 - KWMOON ( 0.0067) -> 0.0000
#define S1PS_ATT_OFFSET1			( 0.0051)	//2023.02.27 - KWMOON ( 0.0094) -> 0.0051
#define S1PS_ATT_OFFSET2			(-0.0017)	//2023.02.27 - KWMOON ( 0.0026) ->-0.0017
#define S1PS_ATT_OFFSET3			(-0.6284)	//2023.02.27 - KWMOON (-0.6241) ->-0.6284

#define MAX_OV_SCALE_FACTOR1		(   1.99)
#define MAX_OV_SCALE_FACTOR0_5		(   3.99)
#define MAX_OV_SCALE_FACTOR0_25		(   7.99)

#define S1PS_IM_VSET_GAIN0			( -0.4885)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_GAIN1			( -0.3313)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_GAIN2			( -0.5921)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_GAIN3			( -1.4022)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )

#define S1PS_IM_VSET_OFFSET0		(  7.807)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_OFFSET1		(  7.010)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_OFFSET2		(  9.320)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )
#define S1PS_IM_VSET_OFFSET3		( 21.311)	//2023.02.27 - KWMOON( VSET GAIN/OFFSET )

#define S1PS_OC_WARN_LIMIT_OFF		( 30.0)		//2023.07.19 - KWMOON( yhsong - oc warn off )

//PPS & S1PS
enum PMBUS_ADDRESS
{
	CH1_ADDRESS   = 0x10,
	CH2_ADDRESS   = 0x11,
	CH3_ADDRESS   = 0x12,
	CH4_ADDRESS   = 0x13,
	CH5_ADDRESS   = 0x14,
	CH6_ADDRESS   = 0x15,
	CH7_ADDRESS   = 0x16,
	CH8_ADDRESS   = 0x17,
};

enum PROTOCOL_SELECT
{
	SEND_BYTE       = 0x0,
	WRITE_BYTE           ,
	WRITE_WORD           ,
	READ_BYTE            ,
	READ_WORD            ,
	SMBALERT_MASK_WRITE  ,
	SMBALERT_MASK_READ   ,
};

enum PMBUS_DATA_FORMAT
{
	NONE = 0x0,
	BYTE      ,
	WORD      ,
	L11       ,
	L16       ,
	BLOCKS    ,
};

enum PMBUS_TYPE
{
	PMBUS_W   = 0x0,
	PMBUS_R        ,
	PMBUS_WR       ,
};

enum TURN_ONOFF
{
	TURN_OFF    = 0x0,
	TURN_ON          ,
};

enum VOUT_SCALE_LOOP_IDX
{
	V1_0		= 0x0   ,
	V0_5		= 0x1   ,
	V0_25		= 0x2   ,
	V0_125	    = 0x3   ,
};

enum VOUT_SCALE_LOOP
{
	L11_V1_0	= 0xE808,
	L11_V0_5	= 0xE804,
	L11_V0_25	= 0xE802,
	L11_V0_125  = 0xE801,
};

enum PMBUS_CMD
{
	OPERATION               = 0x1,
	ON_OFF_CONFIG           = 0x2,
	CLEAR_FAULTS            = 0x3,
	WRITE_PROTECT           = 0x4,
	STORE_USER_ALL          = 0x15,
	RESTORE_USER_ALL        = 0x16,
	CAPABILITY              = 0x19,
	SMBALERT_MASK           = 0x1B,
	VOUT_MODE               = 0x20,
	VOUT_COMMAND            = 0x21,
	VOUT_TRIM               = 0x22,
	VOUT_MAX                = 0x24,
	VOUT_MARGIN_HIGH        = 0x25,
	VOUT_MARGIN_LOW         = 0x26,
	VOUT_TRANSITION_RATE    = 0x27,
	VOUT_SCALE_LOOP         = 0x29,
	FREQUENCY_SWITCH        = 0x33,
	VIN_ON                  = 0x35,
	VIN_OFF                 = 0x36,
	INTERLEAVE              = 0x37,
	VOUT_OV_FAULT_LIMIT     = 0x40,
	VOUT_OV_FAULT_RESPONSE  = 0x41,
	VOUT_OV_WARN_LIMIT      = 0x42,
	VOUT_UV_WARN_LIMIT      = 0x43,
	VOUT_UV_FAULT_LIMIT     = 0x44,
	VOUT_UV_FAULT_RESPONSE  = 0x45,
	IOUT_OC_FAULT_LIMIT     = 0x46,
	IOUT_OC_FAULT_RESPONSE  = 0x47,
	IOUT_OC_WARN_LIMIT      = 0x4A,
	OT_FAULT_LIMIT          = 0x4F,
	OT_FAULT_RESPONSE       = 0x50,
	OT_WARN_LIMIT           = 0x51,
	VIN_OV_FAULT_LIMIT      = 0x55,
	VIN_OV_FAULT_RESPONSE   = 0x56,
	VIN_UV_WARN_LIMIT       = 0x58,
	IIN_OC_WARN_LIMIT       = 0x5D,
	POWER_GOOD_ON           = 0x5E,
	POWER_GOOD_OFF          = 0x5F,
	TON_DELAY               = 0x60,
	TON_RISE                = 0x61,
	TON_MAX_FAULT_LIMIT     = 0x62,
	TON_MAX_FAULT_RESPONSE  = 0x63,
	TOFF_DELAY              = 0x64,
	TOFF_FALL               = 0x65,
	TOFF_MAX_WARN_LIMIT     = 0x66,
	STATUS_BYTE             = 0x78,
	STATUS_WORD             = 0x79,
	STATUS_VOUT             = 0x7A,
	STATUS_IOUT             = 0x7B,
	STATUS_INPUT            = 0x7C,
	STATUS_TEMPERATURE      = 0x7D,
	STATUS_CML              = 0x7E,
	STATUS_MFR_SPECIFIC     = 0x80,
	READ_VIN                = 0x88,
	READ_IIN                = 0x89,
	READ_VOUT               = 0x8B,
	READ_IOUT               = 0x8C,
	READ_TEMPERATURE        = 0x8D,
	READ_DUTY_CYCLE         = 0x94,
	READ_POUT               = 0x96,
	READ_PIN                = 0x97,
	PMBUS_REVISION          = 0x98,
	MFR_SERIAL              = 0x9E,
	IC_DEVICE_ID            = 0xAD,
	MFR_BASE_ADDRESS        = 0xD7,
	MFR_BASE_ADDRESS_2      = 0xE2,
	///////////////////////////////
	/// Hidden Register - 20221115
	///////////////////////////////
	IOUT_OCP_GAIN           = 0xCA,
	IOUT_OCP_OFFSET         = 0xCB,
	VDS_OFFSET              = 0xCD,
	VDS_GAIN                = 0xCE,
	KT1                     = 0xCF,
	KT2                     = 0xD0,
	KVGS1                   = 0xD1,
	KVGS2                   = 0xD2,
	KVDS                    = 0xD3,
	IOUT_CAL_GM             = 0xD4,
};

enum PMBUS_INIT_VAL
{
	OPERATION_ON             = 0xC8,
	OPERATION_OFF            = 0x48,
	ON_OFF_CONFIG_V          = 0x1B,
	ENABLE_ALL               = 0b00000000,	//WRITE PROTECT
	DISABLE_ALL              = 0b10000000,	//WRITE PROTECT
	ENABLE_OPR               = 0b01000000,	//WRITE PROTECT
	ENABLE_WP_OPR_ONOFF_VOUT = 0b00100000,	//WRITE PROTECT
	VOUT_OV_FAULT_RESPONSE_V = 0xC0,
	VOUT_UV_FAULT_RESPONSE_V = 0xC0,
	IOUT_OC_FAULT_RESPONSE_V = 0xF6,
	OT_FAULT_RESPONSE_V      = 0xC0,
	VIN_OV_FAULT_RESPONSE_V  = 0xC0,
	TON_MAX_FAULT_RESPONSE_V = 0xB9,
	S1PS_VDS_OFFSET			 = 0x41,	//2023.02.27 - KWMOON( S1PS Only )
	S1PS_IOUT_OCP_OFFSET	 = 0x00,	//2023.02.27 - KWMOON( S1PS Only )
};

typedef struct _stPMBusCmd
{
	PMBUS_DATA_FORMAT format;
	PMBUS_TYPE wr;
	std::string name;
	int exponent;
}stPMBusCmd;

typedef struct _stAlertMask
{
	PMBUS_CMD cmd;
	int mask_byte;
}stAlertMask;

class TESTLIBSHARED_EXPORT CPMBus
{
public:
	explicit CPMBus(PPS_UNIT);
public:
	bool PMBusWrite(int ch, PMBUS_CMD cmd, int data );
	bool PMBusRead (int ch, PMBUS_CMD cmd, int& data );
	bool PMBusAlertMaskWrite( uint, PMBUS_CMD xcmd, int  mask ); //2022.12.09 - kwmoon
	bool PMBusAlertMaskRead ( uint, PMBUS_CMD xcmd, int& mask ); //2022.12.09 - kwmoon
	void SetEnable(bool);
	int  GetAlert();
	double L11ToDouble(ushort);
	double L16ToDouble(ushort);
	ushort DoubleToL11(PMBUS_CMD, double);
	ushort DoubleToL16(PMBUS_CMD, double);
private:
	int GetPMBusAddress(int);
	void CreateCmdMap();
//PEC
private:
	uchar GenerateCRC8( uchar data[], uchar bytes );
	uchar CheckCRC8( uchar data[], uchar bytes, int checksum );
public:
	std::string GetPMBusCommandStr(PMBUS_CMD);
	int GetPMBusCommandExp(PMBUS_CMD);
private:
	PPS_UNIT m_BaseType;
	std::map<PMBUS_CMD,stPMBusCmd> m_PMBusCmd;
};

class TESTLIBSHARED_EXPORT CPps
{
public:
	CPps();
public:
	bool PpsInit();
	bool PpsInitAlertMask();
	bool SetVoutRange(uint,uint,double);	//2022.12.05 - kwmoon
	bool SetVs(uint,uint,double,double,double,double);
	bool SetVsOn(uint,int);
	bool SetVsOff(uint);
	bool ReadVout(uint,int,double&);
	bool ReadIout(uint,int,double&);
	bool ReadVf(uint,int,double&);
	bool ReadVs(uint,int,double&);
	bool ReadGnd(uint,double&);
	bool ReadStatusVout (uint, int, R_STATUS_VOUT&);
	bool ReadStatusIout (uint, int, R_STATUS_IOUT&);	//2022.12.14 - kwmoon
	bool ReadStatusWord (uint, int, R_STATUS_WORD&);
	bool ReadStatusByte (uint, int, R_STATUS_BYTE&);
	bool ReadStatusInput(uint, int, R_STATUS_INPUT&);
	bool ReadVdsOffset(uint,uint,int&);	//2022.12.03 - kwmoon
	bool ReadScaleloop(uint,uint,int&);	//2022.12.06 - kwmoon
	bool ClearFaults(uint,int);
	bool SetDgsControl(uint,int);
	bool SetSenseControl( uint,int );
	bool SetLocalRemoteSensing();
	bool GetS1psIoutVsetGainOffset(double, double&, double& );
public:
	bool IsPpsOn();
	bool IsS1psOn();
public:	//for debug
	void SetPpsEnable(uint,bool);
	bool GetPpsAlert (uint);
	bool SetPpsRequest();
	ushort DtoL11(PMBUS_CMD, double);
	ushort DtoL16(PMBUS_CMD, double);
	double L11toD(ushort);
	double L16toD(ushort);
	std::string GetPMBusCommandStr(PMBUS_CMD);
	bool PMBusWrite( uint, int, PMBUS_CMD, int );
	bool PMBusRead ( uint, int, PMBUS_CMD, int& );
	bool PMBusAlertMaskWrite( uint, int, PMBUS_CMD xcmd, int  mask ); //2022.12.09 - kwmoon
	bool PMBusAlertMaskRead ( uint, int, PMBUS_CMD xcmd, int& mask ); //2022.12.09 - kwmoon

	bool GetS1psAttVoltage(uint,double,double&);
	bool SetVoutScaleLoop(uint,uint,ushort);
	bool GetS1psOcFaultLimit( double, double, double& );

private:
	bool IsValidSetVoltage( PPS_UNIT, uint, double, double );
	bool IsSetVoutRange( PPS_UNIT, uint );
	bool GetVoutScaleLoop(uint,double,ushort&);	//2022.12.05 - kwmoon
	bool GetS1psFreqSwitch( double, ushort& );
	char GetS1psVdsOffset( double );
	bool SetVBump( uint, uint, double, double, double, double );
	void PrintVoutScaleLoop(uint);	//2022.12.05 - kwmoon
	uint GetVoutScaleLoopIndex(uint);
	bool GetValidOverVoltage( PPS_UNIT, uint, double& );	//2022.12.09 - kwmoon
private:
	CPMBus pps;
	CPMBus s1ps;
	std::vector<stAlertMask> m_vecAlertMask;	//2022.12.09 - kwmoon

private:
	R_STATUS_BYTE m_StatusByte[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_WORD m_StatusWord[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_VOUT m_StatusVout[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_IOUT m_StatusIout[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_INPUT m_StatusInput[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_TEMPERATURE m_StatusTemperature[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_CML m_StatusCml[MAX_PPS_TYPE][MAX_PPS_CH];
	R_STATUS_MFR m_StatusMfr[MAX_PPS_TYPE][MAX_PPS_CH];	
private:	
	double m_dS1psAttGain  [4];
	double m_dS1psAttOffset[4];	
	double m_dVsVoltage [2][MAX_PPS_CH];
	bool m_bValidVs[2][MAX_PPS_CH];
	bool m_bVoutRangeSet[2][MAX_PPS_CH];
	bool m_bVsOn[2][MAX_PPS_CH];
	int  m_nVsOnBit[2];
	int m_nLvMode;
	char m_VdsOffset[2][MAX_PPS_CH];		//2022.12.03 - kwmoon
	ushort m_VoutScaleLoop[2][MAX_PPS_CH];	//2022.12.03 - kwmoon
	double m_dVoutRange   [2][PMBUS_MAX_VOUT_SCALE_LOOP][2];
	int VOUT_SCALE_LOOP_FACTOR[4];
};

#endif // CPPS_HPP
