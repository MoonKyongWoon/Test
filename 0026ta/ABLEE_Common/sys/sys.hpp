#ifndef CSYS_HPP
#define CSYS_HPP

#include "basestruct.hpp"
#include "testlib_global.h"
#include "protocol.hpp"
#include "systype.hpp"

#define SYS_TPG_MGNT_EMT_COUNT		(6)
#define SYS_PE_MGNT_EMT_COUNT		(6)

enum EN_BOARD_CONTACT
{
	BACKPLANE_BD        = (0x1<<0),
	BIB_BD              = (0x1<<1),
	EXTENT_BD           = (0x1<<2),
};

enum EN_LOGIC_TYPE
{
	LOGIC_TPG           = 0x0,
	LOGIC_IOA           = 0x1,
	LOGIC_PE1           = 0x1,
	LOGIC_IOB           = 0x2,
	LOGIC_PE2           = 0x2,
	LOGIC_UCLK          = 0x3,
	LOGIC_PE3           = 0x3,
	LOGIC_MAX_TYPE           ,
};

enum EN_LOGIC_CPE_MASK
{
	IO_CPE_DISABLE      = (0x0<<0),
	IO_CPE_ENABLE       = (0x1<<0),
	DR_CPE_DISABLE      = (0x0<<0),
	DR_CPE_ENABLE       = (0x1<<1),
};

enum EN_SYSTEM_MUX
{
	SYS_MUX_TEMP    = 0x0,
	SYS_MUX_VCCINT  = 0x1,
	SYS_MUX_VCCAUX  = 0x2,
	SYS_MUX_NONE    = 0x3,
	SYS_MUX_VREFP   = 0x4,
	SYS_MUX_VREFN   = 0x5,
	SYS_MUX_VCCBRAM = 0x6,
};

enum EN_TPG_ALARM_FLAG
{
	TPG_OVER_TEMP       = (0x1<<0),
	TPG_USER_TEMP_ALRM  = (0x1<<1),
	TPG_VCCINT_ALRM     = (0x1<<2),
	TPG_VCCAUX_ALRM     = (0x1<<3),
	TPG_VCCBRAM_ALRM    = (0x1<<4),
	TPG_ALARM_OUT       = (0x1<<5),
};

enum EN_PE_ALARM_FLAG
{
	PE_OVER_TEMP       = (0x1<<0),
	PE_USER_TEMP_ALRM  = (0x1<<1),
	PE_VCCINT_ALRM     = (0x1<<2),
	PE_VCCAUX_ALRM     = (0x1<<3),
};

enum EEPROM_CAL_TYPE
{
	CAL_ID         = (0x0<<11),
	CAL_ADC        = (0x1<<11),
	CAL_DAC        = (0x2<<11),
	CAL_PPS        = (0x3<<11),
	CAL_S1PS       = (0x4<<11),
	CAL_IO_SKEW    = (0x5<<11),
	CAL_DR_SKEW    = (0x6<<11),
	CAL_UCLK_SKEW  = (0x7<<11),
	CAL_IO_GROUP   = (0x8<<11),
	//........
	CAL_DIAG       = (0xF<<11),
};

enum EEPROM_CAL_OFFSET
{
	ID_OFFSET         = 0x7,
	ADC_CH_OFFSET     = 0x7,
	DAC_CH_OFFSET     = 0x7,
	PPS_CH_OFFSET     = 0x7,
	S1PS_CH_OFFSET    = 0x7,
	IO_FC_OFFSET      = 0x9,
	IO_FC_CH_OFFSET   = 0x2,
	DR_FC_OFFSET      = 0x9,
	DR_FC_CH_OFFSET   = 0x2,
	UCLK_CH_OFFSET    = 0x9,
	UCLK_FC_CH_OFFSET = 0x2,
	IO_GROUP_OFFSET   = 0x2,
};

class TESTLIBSHARED_EXPORT CSys
{
public:
	CSys();
	void InitSystemManagent();
public:
	int SlotId();
	int SysDelay( double );
	void SetDevice( int, ushort );
	void SetVersion(const char*);
	const char* GetVersion();
	void SetAutoMode(bool);	//2023.01.19 - kwmoon
	bool GetAutoMode();
	int  GetDevice();
public:
    void SetDefineFlag(int);
    void InitDefineFlag();
	int  GetDefineFlag();
    void SetFaultBin(int);
	void InitPinFixHL();
public:
	bool IsConnBackplane();
	bool IsConnExtend();
	bool IsConnBib();
	bool GetFpgaAlarmFlag(EN_LOGIC_TYPE,int&);
	double GetFpgaTemperature(EN_LOGIC_TYPE);
	double GetFpgaVoltage(EN_LOGIC_TYPE, EN_SYSTEM_MUX, atstring&);
	void SetCurrentSlotNo(int);	//2023.01.19
	void SetCurrentCpuNo(int);
	int GetLogicVerYmd( EN_LOGIC_TYPE );
	int GetLogicVerFw ( EN_LOGIC_TYPE );
	int GetFpgaAdcRead( EN_LOGIC_TYPE,  EN_SYSTEM_MUX);
	int GetCurrentSlotNo();
	int GetCurrentCpuNo();
	int GetReadSiteNo();
	const char* GetLogicName( EN_LOGIC_TYPE );
public:
	void SetPinFixHLInit();
	//Pin FixH/FixL for Diagnostic
	void SetPinDrvFixH   (uint, bool, uint, uint);
	void SetPinDrvFixL   (uint, bool, uint, uint);
	void SetPinIoaFixH   (uint, bool, uint, uint);
	void SetPinIoaFixL   (uint, bool, uint, uint);
	void SetPinIobFixH   (uint, bool, uint, uint);
	void SetPinIobFixL   (uint, bool, uint, uint);
	void SetPinUclkFixH  (uint, bool, uint, uint);
	void SetPinUclkFixL  (uint, bool, uint, uint);
	void SetPinIoaDreFixh(bool);
	void SetPinIobDreFixh(bool);
	void SetPinFc1DrvFixh(bool);
	void SetPinFc2DrvFixh(bool);
	void SetPinUclkDreFixh(bool);
	void GetIoaReadData(uint, bool*, uint);
	void GetIobReadData(uint, bool*, uint);
	void GetDrvReadData(uint, bool*, uint);
	void GetUclkReadData(uint, bool*, uint);
//TPG-PE DC
public:
	void SetIoaReadSignal(uint,bool);
	void SetIobReadSignal(uint,bool);
	void SetUclkReadSignal(uint,bool);
	void GetTpgIoaReadData(uint, bool*, uint);
	void GetTpgIobReadData(uint, bool*, uint);
	void GetTpgDrvReadData(uint, bool*, uint);
	void GetTpgUclkReadData(uint, bool*, uint);
public:
	bool SysWriteDbm( uint start, char* p, uint size );
	bool SysReadDbm ( uint start, char* p, uint size );
	bool SysWriteExtfm( uint fc, uint64_t start, char* p, uint size );
	bool SysReadExtfm ( uint fc, uint64_t start, char* p, uint size );
//2023.04.27 - kwmoon
public:
	void SetRunmode( bool );
	bool GetRunmode();
//2023.05.09 - KWMOON(CHAMBER)
public:
	void SetPfmIniInit();
	void SetPfmSetVs(uint,uint,double);
	void SetPfmFalultClear();
	bool GetPfmFault(uint,uint);
	void SetPfmEnable(bool);
	bool IsPfmEnable();
	void SetPfmStop();
	void SetPfmStart();
	bool IsPfmStart();

public:
	void ChamberSetUsage(bool);
	bool ChamberIsUse();
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberInitTemp();
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberEnter();
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberSetSv( double );
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberRun();
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberReadTemp(double&,double&);
	PROTOCOL::PGM_CHMB_REQ_ACK ChamberReadStatus(bool&,bool&,bool&);
    PROTOCOL::PGM_CHMB_REQ_ACK ChamberReadStatus1(bool& open,bool& close,bool& upper,bool& bottom,bool& rackin, bool& rackout);
    PROTOCOL::PGM_CHMB_REQ_ACK ChamberIsAlarm();
	void SetChmbSoakTime( int min );	//2023.07.09 - KWMOON
	void SetChmbKeepTime( int min );	//2023.07.09 - KWMOON
	int  GetChmbSoakTime();				//2023.07.09 - KWMOON
	int  GetChmbKeepTime();				//2023.07.09 - KWMOON
private:
	bool m_bChamberUsage;
	bool m_sChamberErrMsg;
	int m_nChmbSoakTime;
	int m_nChmbKeepTime;
private:
    int m_nDefineFlag;
    int m_nFaultBin;
	int m_bBibParityError;
private:
	uint m_uPinDrvFixH [ALL_FC][4];
	uint m_uPinDrvFixL [ALL_FC][4];
	uint m_uPinIoaFixH [ALL_FC][6];
	uint m_uPinIoaFixL [ALL_FC][6];
	uint m_uPinIobFixH [ALL_FC][6];
	uint m_uPinIobFixL [ALL_FC][6];
	uint m_uPinUclkFixH[ALL_FC][11];
	uint m_uPinUclkFixL[ALL_FC][11];
private:
	std::string m_sVersion;
	DEVICE_TYPE m_devMode;
	ushort m_devCode;
	int m_nSlotNo;
	int m_nCpuNo;
	stSysMgnt m_TpgMux[SYS_TPG_MGNT_EMT_COUNT+1];
	stSysMgnt m_PeMux [SYS_PE_MGNT_EMT_COUNT+1];
	stSysMgnt m_TpgAlarmFlag[SYS_TPG_MGNT_EMT_COUNT];
	stSysMgnt m_PeAlarmFlag [SYS_PE_MGNT_EMT_COUNT-1];
	bool m_bAutoMode;
private:
	bool m_RUNMODE;
private:
	bool m_bPfmEnable;
	bool m_bPfmRun;
	std::string m_sPfmSetVsIni;
	std::string m_sPfmFalultIni;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The CPSClient class
/// \details compile & measure pps/s1ps
/// \date 2022.11.30
/// \author kwmoon
class TESTLIBSHARED_EXPORT CPSClient
{
public:
	CPSClient();
	~CPSClient();
public:
	bool getCurrentVOUT( int id, int ch, float& rMeas );
	bool getCurrentIOUT( int id, int ch, float& rMeas );
	bool getSTATUSWORD( int id, int ch, unsigned short& rStat );
	bool getSTATUSBYTE( int id, int ch, unsigned char& rStat );
};


#endif // CSYS_HPP
