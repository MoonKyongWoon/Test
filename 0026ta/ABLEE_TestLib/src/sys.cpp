#include <chrono>
#include <thread>
#include "dbm.hpp"
#include "dfm.hpp"
#include "devsocket.hpp"
#include "log.hpp"
#include "pps.hpp"
#include "sys.hpp"
#include "bitchart.hpp"
#include "io.hpp"

extern CLog  g_log;
extern CPcie g_pcie;
extern CPps  g_pps;
extern CDevSocket g_devsock;

CSys::CSys()
{
	m_bAutoMode = false;
	m_nSlotNo = -1;
	m_nCpuNo  = -1;
	InitSystemManagent();
    InitDefineFlag();
	InitPinFixHL();
	m_RUNMODE = false;
	m_bChamberUsage = false;
	m_nChmbSoakTime = DEFAULT_CHMB_SOAK_TIME;
	m_nChmbKeepTime = DEFAULT_CHMB_KEEP_TIME;
	(void)RC_TESTLIB_VERSION;
	m_bPfmRun = false;
	m_bPfmEnable = false;
}

void CSys::InitPinFixHL()
{
	for( int fc=0;fc<2;fc++)
	{
		for(int drv=0;drv<4;drv++ )
		{
			m_uPinDrvFixH [fc][drv] = 0x0;
			m_uPinDrvFixL [fc][drv] = 0x0;
		}
		for(int io=0;io<6;io++ )
		{
			m_uPinIoaFixH [fc][io] = 0x0;
			m_uPinIoaFixL [fc][io] = 0x0;
			m_uPinIobFixH [fc][io] = 0x0;
			m_uPinIobFixL [fc][io] = 0x0;
		}
		for(int uclk=0;uclk<11;uclk++ )
		{
			m_uPinUclkFixH[fc][uclk] = 0x0;
			m_uPinUclkFixL[fc][uclk] = 0x0;
		}
	}
}

void CSys::InitSystemManagent()
{
	memset( m_TpgMux      , 0x0, sizeof(m_TpgMux)       );
	memset( m_TpgAlarmFlag, 0x0, sizeof(m_TpgAlarmFlag) );

	m_TpgMux[0] = stSysMgnt { SYS_MUX_TEMP   , "TEMPERATURE"   , 0.00 };
	m_TpgMux[1] = stSysMgnt { SYS_MUX_VCCINT , "VCCINT(0.95V)" , 0.95 };
	m_TpgMux[2] = stSysMgnt { SYS_MUX_VCCAUX , "VCCAUX(1.8V)"  , 1.80 };
	m_TpgMux[4] = stSysMgnt { SYS_MUX_VREFP  , "VREFP(1.25V)"  , 1.25 };
	m_TpgMux[5] = stSysMgnt { SYS_MUX_VREFN  , "VREFN(0.0V)"   , 0.00 };
	m_TpgMux[6] = stSysMgnt { SYS_MUX_VCCBRAM, "VCCBRAM(0.95V)", 0.95 };

	m_TpgAlarmFlag[0] = stSysMgnt { TPG_OVER_TEMP      , "OVER TEMP ALARM" , 0.00 };
	m_TpgAlarmFlag[1] = stSysMgnt { TPG_USER_TEMP_ALRM , "USER TEMP ALARM" , 0.00 };
	m_TpgAlarmFlag[2] = stSysMgnt { TPG_VCCINT_ALRM    , "VCCINT ALARM"    , 0.00 };
	m_TpgAlarmFlag[3] = stSysMgnt { TPG_VCCAUX_ALRM    , "VCCAUX ALARM"    , 0.00 };
	m_TpgAlarmFlag[4] = stSysMgnt { TPG_VCCBRAM_ALRM   , "VCCBRAM ALARM"   , 0.00 };
	m_TpgAlarmFlag[5] = stSysMgnt { TPG_ALARM_OUT      , "ALARM OUT?"      , 0.00 };

	memset( m_PeMux      , 0x0, sizeof(m_PeMux)       );
	memset( m_PeAlarmFlag, 0x0, sizeof(m_PeAlarmFlag) );
	m_PeMux[0] = stSysMgnt { SYS_MUX_TEMP   , "TEMPERATURE"  , 0.00 };
	m_PeMux[1] = stSysMgnt { SYS_MUX_VCCINT , "VCCINT(1.0V)" , 1.00 };
	m_PeMux[2] = stSysMgnt { SYS_MUX_VCCAUX , "VCCAUX(2.5V)" , 2.50 };
	m_PeMux[4] = stSysMgnt { SYS_MUX_VREFP  , "VREFP(1.25V)" , 1.25 };
	m_PeMux[5] = stSysMgnt { SYS_MUX_VREFN  , "VREFN(0.0V)"  , 0.00 };

	m_PeAlarmFlag[0] = stSysMgnt { PE_OVER_TEMP      , "OVER TEMP ALARM" , 0.00 };
	m_PeAlarmFlag[1] = stSysMgnt { PE_USER_TEMP_ALRM , "USER TEMP ALARM" , 0.00 };
	m_PeAlarmFlag[2] = stSysMgnt { PE_VCCINT_ALRM    , "VCCINT ALARM"    , 0.00 };
	m_PeAlarmFlag[3] = stSysMgnt { PE_VCCAUX_ALRM    , "VCCAUX ALARM"    , 0.00 };

}

void CSys::InitDefineFlag()
{
    m_nDefineFlag = 0x0;
}

void CSys::SetDefineFlag(int flag)
{
    m_nDefineFlag |= flag;
}

int CSys::GetDefineFlag()
{
	return m_nDefineFlag;
}

void CSys::SetFaultBin(int fbin)
{
    m_nFaultBin = fbin;
}

void CSys::SetAutoMode( bool mode )
{
	m_bAutoMode = mode;
}

bool CSys::GetAutoMode()
{
	return m_bAutoMode;
}

int CSys::SysDelay( double delay )
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	long long wait_time = static_cast<long long>( delay * 1.0e6 );	//US
	while ( true )
	{
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//		long long elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();		//NS
		long long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();	//US
		if( elapsed_time > wait_time )
		{
			//fprintf(stdout, "%s(%d) : Elapsed-%lld, Wait:%lld\n", __FUNCTION__, __LINE__, elapsed_time, wait_time );
			break;
		}
	}
	return 0;
}

void CSys::SetVersion(const char* VER)
{
	m_sVersion = std::string(VER);
}

const char* CSys::GetVersion()
{
	return m_sVersion.c_str();
}

void CSys::SetDevice( int mode, ushort code )
{
	m_devMode = static_cast<DEVICE_TYPE>(mode);
	m_devCode = code;
	switch ( mode )
	{
		case DEVICE_TYPE::EMMC:
			g_log.Printf("SET_DEVICE : EMMC\n" );
			break;
		case DEVICE_TYPE::FLASH_DEVICE:
			g_log.Printf("SET_DEVICE : FLASH_DEVICE\n" );
			break;
		case DEVICE_TYPE::SRAM:
			g_log.Printf("SET_DEVICE : SRAM\n" );
			break;
		case DEVICE_TYPE::DRAM:
			g_log.Printf("SET_DEVICE : DRAM\n" );
			break;
		case DEVICE_TYPE::DIAGCAL:
			g_log.Printf("SET_DEVICE : DIAG/CAL\n" );
			break;
		default:
			g_log.Printf("SET_DEVICE : UNDEFINED\n" );
			break;
	}
}

int CSys::GetDevice()
{
	return static_cast<int>(m_devMode);
}

int CSys::SlotId()
{
	return g_pcie.read_user( BITCHART::DRV_UCLK::_SLOT_ID );
}

int CSys::GetLogicVerYmd( EN_LOGIC_TYPE fpga )
{
	int nVerYmd = 0x0;
	int nHYear = 0x0;
	int nLYear = 0x0;
	int nMonth = 0x0;
	int nDay = 0x0;

	switch( fpga )
	{
		case LOGIC_TPG:
			nVerYmd = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_VER_YMD );
			break;
		case LOGIC_IOA:
			nHYear = g_pcie.read_user( BITCHART::DRV_IOA::_VER_HYEAR ) & 0xFF;
			nLYear = g_pcie.read_user( BITCHART::DRV_IOA::_VER_LYEAR ) & 0xFF;
			nMonth = g_pcie.read_user( BITCHART::DRV_IOA::_VER_MONTH ) & 0xFF;
			nDay   = g_pcie.read_user( BITCHART::DRV_IOA::_VER_DAY   ) & 0xFF;
			nVerYmd= (nHYear<<24) | (nLYear<<16) | (nMonth<<8) | nDay;
			break;
		case LOGIC_IOB:
			nHYear = g_pcie.read_user( BITCHART::DRV_IOB::_VER_HYEAR ) & 0xFF;
			nLYear = g_pcie.read_user( BITCHART::DRV_IOB::_VER_LYEAR ) & 0xFF;
			nMonth = g_pcie.read_user( BITCHART::DRV_IOB::_VER_MONTH ) & 0xFF;
			nDay   = g_pcie.read_user( BITCHART::DRV_IOB::_VER_DAY   ) & 0xFF;
			nVerYmd= (nHYear<<24) | (nLYear<<16) | (nMonth<<8) | nDay;
			break;
		case LOGIC_UCLK:
			nHYear = g_pcie.read_user( BITCHART::DRV_UCLK::_VER_HYEAR ) & 0xFF;
			nLYear = g_pcie.read_user( BITCHART::DRV_UCLK::_VER_LYEAR ) & 0xFF;
			nMonth = g_pcie.read_user( BITCHART::DRV_UCLK::_VER_MONTH ) & 0xFF;
			nDay   = g_pcie.read_user( BITCHART::DRV_UCLK::_VER_DAY   ) & 0xFF;
			nVerYmd= (nHYear<<24) | (nLYear<<16) | (nMonth<<8) | nDay;
			break;
		default:
			//error
			nVerYmd = 0x0;
			break;
	}
	return nVerYmd;
}

int CSys::GetLogicVerFw ( EN_LOGIC_TYPE fpga )
{
	int nVerFw = 0x0;

	switch( fpga )
	{
		case LOGIC_TPG:
			nVerFw = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_VER_FW );
			break;
		case LOGIC_IOA:
			nVerFw = g_pcie.read_user( BITCHART::DRV_IOA::_VER_FW );
			break;
		case LOGIC_IOB:
			nVerFw = g_pcie.read_user( BITCHART::DRV_IOB::_VER_FW );
			break;
		case LOGIC_UCLK:
			nVerFw = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_VER_FW );
			break;
		default:
			//error
			nVerFw = 0x0;
			break;
	}
	return nVerFw;
}

bool CSys::IsConnBackplane()
{
	int nConn = g_pcie.read_user( BITCHART::DRV_IOA::_BOARD_CONTACT );
	if( nConn & EN_BOARD_CONTACT::BACKPLANE_BD )
		return true;
	return false;
}

bool CSys::IsConnExtend()
{
	int nConn = g_pcie.read_user( BITCHART::DRV_IOA::_BOARD_CONTACT );
	if( nConn & EN_BOARD_CONTACT::EXTENT_BD )
		return true;
	return false;
}

bool CSys::IsConnBib()
{
	int nConn = g_pcie.read_user( BITCHART::DRV_IOA::_BOARD_CONTACT );
	if( nConn & EN_BOARD_CONTACT::BIB_BD )
		return true;
	return false;
}


int CSys::GetFpgaAdcRead ( EN_LOGIC_TYPE pe, EN_SYSTEM_MUX  mux )
{
	int nRead = 0x0;
	switch( pe )
	{
		case LOGIC_TPG:
		{
			g_pcie.write_user( BITCHART::SYSTEM_MANAGEMENT::_SYSTEM_ADDR, mux );
			g_pcie.write_user( BITCHART::SYSTEM_MANAGEMENT::_STATUS_REQ , 0x1 );
			SysDelay( 500 US );
			nRead = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_STATUS_READ ) & 0xFFFF;
		}
		break;

		case LOGIC_PE1:
		{
			g_pcie.write_user( BITCHART::DRV_IOA::_SYSTEM_MANAGE_ADDR, mux );
			g_pcie.write_user( BITCHART::DRV_IOA::_SYSTEM_STATUS_REQ , 0x1 );
			SysDelay( 500 US );
			int nHigh = g_pcie.read_user( BITCHART::DRV_IOA::_SYSTEM_STATUS_READ_H ) & 0xFF;
			int nLow  = g_pcie.read_user( BITCHART::DRV_IOA::_SYSTEM_STATUS_READ_L ) & 0xFF;
			nRead = (nHigh<<8) | nLow;
			nRead = (nRead>>6) & 0x3FF;
		}
		break;

		case LOGIC_PE2:
		{
			g_pcie.write_user( BITCHART::DRV_IOB::_SYSTEM_MANAGE_ADDR, mux );
			g_pcie.write_user( BITCHART::DRV_IOB::_SYSTEM_STATUS_REQ , 0x1 );
			SysDelay( 500 US );
			int nHigh = g_pcie.read_user( BITCHART::DRV_IOB::_SYSTEM_STATUS_READ_H ) & 0xFF;
			int nLow  = g_pcie.read_user( BITCHART::DRV_IOB::_SYSTEM_STATUS_READ_L ) & 0xFF;
			nRead = (nHigh<<8) | nLow;
			nRead = (nRead>>6) & 0x3FF;
		}
		break;

		case LOGIC_PE3:
		{
			g_pcie.write_user( BITCHART::DRV_UCLK::_SYSTEM_MANAGE_ADDR, mux );
			g_pcie.write_user( BITCHART::DRV_UCLK::_SYSTEM_STATUS_REQ , 0x1 );
			SysDelay( 500 US );
			int nHigh = g_pcie.read_user( BITCHART::DRV_UCLK::_SYSTEM_STATUS_READ_H ) & 0xFF;
			int nLow  = g_pcie.read_user( BITCHART::DRV_UCLK::_SYSTEM_STATUS_READ_L ) & 0xFF;
			nRead = (nHigh<<8) | nLow;
			nRead = (nRead>>6) & 0x3FF;
		}
		break;

		default:
			//error
			nRead = 0x0;
			break;
	}
	return nRead;
}

double CSys::GetFpgaTemperature(EN_LOGIC_TYPE type)
{
	double dTemp = 0.0;
	int nAdcRead = 0x0;
	switch( type )
	{
		case LOGIC_TPG:
			nAdcRead = GetFpgaAdcRead( type, SYS_MUX_TEMP );
			dTemp = (nAdcRead * (502.9098/65536.0)) - 273.8195;
			//fprintf( stdout, "TPG Temperatue :  %5.2f[0x%04X]\n", dTemp, nAdcRead );
			break;
		case LOGIC_PE1:
		case LOGIC_PE2:
		case LOGIC_PE3:
			nAdcRead = GetFpgaAdcRead( type, SYS_MUX_TEMP );
			dTemp = (nAdcRead * (503.975 / 1024.0)) - 273.15;
			//fprintf( stdout, "PE%d Temperatue :  %5.2f[0x%04X]\n", static_cast<int>(type), dTemp, nAdcRead );
			break;
		default:
			fprintf( stderr, "%s : invalid LOGIC_TYPE parameter!\n", __FUNCTION__ );
			break;
	}
	//fprintf( stdout, "Fpga%d Temperature Hex:%04X\n", type, nAdcRead );
	return dTemp;
}

double CSys::GetFpgaVoltage(EN_LOGIC_TYPE type, EN_SYSTEM_MUX mux, atstring& rName )
{
	double dVolt = 0.0;
	int nAdcRead = 0;
	switch( type )
	{
		case LOGIC_TPG:
			nAdcRead = GetFpgaAdcRead( type, mux );
			dVolt = (nAdcRead / 65535.0 ) * 3.0;
			rName = atstring( m_TpgMux[mux].szName );
			break;
		case LOGIC_PE1:
		case LOGIC_PE2:
		case LOGIC_PE3:
			nAdcRead = GetFpgaAdcRead( type, mux );
			dVolt = ( nAdcRead / 1024.0 ) * 3.0;
			rName = atstring( m_PeMux[mux].szName );
			break;
		default:
			break;
	}
	return dVolt;
}


bool CSys::GetFpgaAlarmFlag(EN_LOGIC_TYPE pe, int& rFlag)
{
	bool bAlarm = false;
	switch ( pe )
	{
		case LOGIC_TPG:
			rFlag = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_STATUS_ALARM_FLAG )&0xFF;
			if( rFlag ) bAlarm = true;
			break;
		case LOGIC_PE1:
			rFlag  = g_pcie.read_user( BITCHART::DRV_IOA::_SYSTEM_ALARM_FLAG ) & 0xFF;
			if( rFlag ) bAlarm = true;
			break;

		case LOGIC_PE2:
			rFlag  = g_pcie.read_user( BITCHART::DRV_IOB::_SYSTEM_ALARM_FLAG ) & 0xFF;
			if( rFlag ) bAlarm = true;
			break;

		case LOGIC_PE3:
			rFlag  = g_pcie.read_user( BITCHART::DRV_UCLK::_SYSTEM_ALARM_FLAG ) & 0xFF;
			if( rFlag ) bAlarm = true;
			break;
		default:
			break;
	}
	return bAlarm;
}

const char* CSys::GetLogicName( EN_LOGIC_TYPE type )
{
	switch( type )
	{
		case  LOGIC_TPG:
			return "TPG";
			break;
		case  LOGIC_PE1:
			return "PE1";
			break;
		case  LOGIC_PE2:
			return "PE2";
			break;
		case  LOGIC_PE3:
			return "PE3";
			break;
		default:
			return "UNKNOWN";
			break;
	}
}

int CSys::GetReadSiteNo()
{
	return SlotId();
}

int CSys::GetCurrentSlotNo()
{
	return m_nSlotNo;
}

int CSys::GetCurrentCpuNo()
{
	return m_nCpuNo;
	//if( m_nCpuNo == CPU1 ) return CPU00;
	//else                   return CPU01;
}

void CSys::SetCurrentSlotNo(int no)
{
	m_nSlotNo = no;
}

void CSys::SetCurrentCpuNo(int no)
{
	m_nCpuNo = no;
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinDrvFixH ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 31  ) nEnd   = 31;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinDrvFixH[fc][nArray]  = (m_uPinDrvFixH[fc][nArray] & ~(1<<nOffset));
		m_uPinDrvFixH[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b7_b0  , m_uPinDrvFixH[FC00][0] ); if( 0 ) { fprintf( stdout, "FC1 DRV[0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXH_b7_b0  , m_uPinDrvFixH[FC00][0] );  }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b15_b8 , m_uPinDrvFixH[FC00][1] ); if( 0 ) { fprintf( stdout, "FC1 DRV[1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXH_b15_b8 , m_uPinDrvFixH[FC00][1] );  }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b23_b16, m_uPinDrvFixH[FC00][2] ); if( 0 ) { fprintf( stdout, "FC1 DRV[2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXH_b23_b16, m_uPinDrvFixH[FC00][2] );  }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24, m_uPinDrvFixH[FC00][3] ); if( 0 ) { fprintf( stdout, "FC1 DRV[3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24, m_uPinDrvFixH[FC00][3] );  }
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b7_b0  , m_uPinDrvFixH[FC01][0] ); if( 0 ) { fprintf( stdout, "FC2 DRV[0] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXH_b7_b0  , m_uPinDrvFixH[FC01][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b15_b8 , m_uPinDrvFixH[FC01][1] ); if( 0 ) { fprintf( stdout, "FC2 DRV[1] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXH_b15_b8 , m_uPinDrvFixH[FC01][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b23_b16, m_uPinDrvFixH[FC01][2] ); if( 0 ) { fprintf( stdout, "FC2 DRV[2] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXH_b23_b16, m_uPinDrvFixH[FC01][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24, m_uPinDrvFixH[FC01][3] ); if( 0 ) { fprintf( stdout, "FC2 DRV[3] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24, m_uPinDrvFixH[FC01][3] ); }
	}
	//if( bEn ) SetPinIoaDreFixh( true  );
	//else      SetPinIoaDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinDrvFixL ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 31  ) nEnd   = 31;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinDrvFixL[fc][nArray]  = (m_uPinDrvFixL[fc][nArray] & ~(1<<nOffset));
		m_uPinDrvFixL[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b7_b0  , m_uPinDrvFixL[FC00][0] ); if( 0 ) { fprintf( stdout, "FC1 DRV[0] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXL_b7_b0  , m_uPinDrvFixL[FC00][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b15_b8 , m_uPinDrvFixL[FC00][1] ); if( 0 ) { fprintf( stdout, "FC1 DRV[1] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXL_b15_b8 , m_uPinDrvFixL[FC00][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b23_b16, m_uPinDrvFixL[FC00][2] ); if( 0 ) { fprintf( stdout, "FC1 DRV[2] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXL_b23_b16, m_uPinDrvFixL[FC00][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24, m_uPinDrvFixL[FC00][3] ); if( 0 ) { fprintf( stdout, "FC1 DRV[3] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24, m_uPinDrvFixL[FC00][3] ); }
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b7_b0  , m_uPinDrvFixL[FC01][0] ); if( 0 ) { fprintf( stdout, "FC2 DRV[0] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXL_b7_b0  , m_uPinDrvFixL[FC01][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b15_b8 , m_uPinDrvFixL[FC01][1] ); if( 0 ) { fprintf( stdout, "FC2 DRV[1] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXL_b15_b8 , m_uPinDrvFixL[FC01][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b23_b16, m_uPinDrvFixL[FC01][2] ); if( 0 ) { fprintf( stdout, "FC2 DRV[2] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXL_b23_b16, m_uPinDrvFixL[FC01][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24, m_uPinDrvFixL[FC01][3] ); if( 0 ) { fprintf( stdout, "FC2 DRV[3] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24, m_uPinDrvFixL[FC01][3] ); }
	}
	//if( bEn ) SetPinIoaDreFixh( true  );
	//else      SetPinIoaDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinIoaFixH ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinIoaFixH[fc][nArray]  = (m_uPinIoaFixH[fc][nArray] & ~(1<<nOffset));
		m_uPinIoaFixH[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b7_b0  , m_uPinIoaFixH[FC00][0] ); if( 0 ) { fprintf( stdout, "FC1 IOA[0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b7_b0    , m_uPinIoaFixH[FC00][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b15_b8 , m_uPinIoaFixH[FC00][1] ); if( 0 ) { fprintf( stdout, "FC1 IOA[1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b15_b8   , m_uPinIoaFixH[FC00][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b23_b16, m_uPinIoaFixH[FC00][2] ); if( 0 ) { fprintf( stdout, "FC1 IOA[2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b23_b16  , m_uPinIoaFixH[FC00][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b31_b24, m_uPinIoaFixH[FC00][3] ); if( 0 ) { fprintf( stdout, "FC1 IOA[3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b31_b24  , m_uPinIoaFixH[FC00][3] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b39_b32, m_uPinIoaFixH[FC00][4] ); if( 0 ) { fprintf( stdout, "FC1 IOA[4] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b39_b32  , m_uPinIoaFixH[FC00][4] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b47_b40, m_uPinIoaFixH[FC00][5] ); if( 0 ) { fprintf( stdout, "FC1 IOA[5] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXH_b47_b40  , m_uPinIoaFixH[FC00][5] ); }
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b7_b0  , m_uPinIoaFixH[FC01][0] ); if( 0 ) { fprintf( stdout, "FC2 IOA[0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b7_b0    , m_uPinIoaFixH[FC01][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b15_b8 , m_uPinIoaFixH[FC01][1] ); if( 0 ) { fprintf( stdout, "FC2 IOA[1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b15_b8   , m_uPinIoaFixH[FC01][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b23_b16, m_uPinIoaFixH[FC01][2] ); if( 0 ) { fprintf( stdout, "FC2 IOA[2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b23_b16  , m_uPinIoaFixH[FC01][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b31_b24, m_uPinIoaFixH[FC01][3] ); if( 0 ) { fprintf( stdout, "FC2 IOA[3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b31_b24  , m_uPinIoaFixH[FC01][3] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b39_b32, m_uPinIoaFixH[FC01][4] ); if( 0 ) { fprintf( stdout, "FC2 IOA[4] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b39_b32  , m_uPinIoaFixH[FC01][4] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b47_b40, m_uPinIoaFixH[FC01][5] ); if( 0 ) { fprintf( stdout, "FC2 IOA[5] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXH_b47_b40  , m_uPinIoaFixH[FC01][5] ); }
	}
	//if( bEn ) SetPinIoaDreFixh( true  );
	//else      SetPinIoaDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinIoaFixL ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinIoaFixL[fc][nArray]  = (m_uPinIoaFixL[fc][nArray] & ~(1<<nOffset));
		m_uPinIoaFixL[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b7_b0  , m_uPinIoaFixL[FC00][0] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[0] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b7_b0    , m_uPinIoaFixL[FC00][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b15_b8 , m_uPinIoaFixL[FC00][1] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[1] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b15_b8   , m_uPinIoaFixL[FC00][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b23_b16, m_uPinIoaFixL[FC00][2] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[2] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b23_b16  , m_uPinIoaFixL[FC00][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b31_b24, m_uPinIoaFixL[FC00][3] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[3] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b31_b24  , m_uPinIoaFixL[FC00][3] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b39_b32, m_uPinIoaFixL[FC00][4] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[4] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b39_b32  , m_uPinIoaFixL[FC00][4] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b47_b40, m_uPinIoaFixL[FC00][5] ); 	if( 0 ) { fprintf( stdout, "FC1 IOA[5] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_FIXL_b47_b40  , m_uPinIoaFixL[FC00][5] ); }
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b7_b0  , m_uPinIoaFixL[FC01][0] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[0] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b7_b0    , m_uPinIoaFixL[FC01][0] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b15_b8 , m_uPinIoaFixL[FC01][1] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[1] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b15_b8   , m_uPinIoaFixL[FC01][1] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b23_b16, m_uPinIoaFixL[FC01][2] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[2] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b23_b16  , m_uPinIoaFixL[FC01][2] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b31_b24, m_uPinIoaFixL[FC01][3] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[3] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b31_b24  , m_uPinIoaFixL[FC01][3] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b39_b32, m_uPinIoaFixL[FC01][4] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[4] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b39_b32  , m_uPinIoaFixL[FC01][4] ); }
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b47_b40, m_uPinIoaFixL[FC01][5] ); 	if( 0 ) { fprintf( stdout, "FC2 IOA[5] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_FIXL_b47_b40  , m_uPinIoaFixL[FC01][5] ); }
	}
	//SetPinIoaDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinIobFixH ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinIobFixH[fc][nArray]  = (m_uPinIobFixH[fc][nArray] & ~(1<<nOffset));
		m_uPinIobFixH[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b7_b0  , m_uPinIobFixH[FC00][0] ); if( 0 ) fprintf( stdout, "FC1 IOB[0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b7_b0    , m_uPinIobFixH[FC00][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b15_b8 , m_uPinIobFixH[FC00][1] ); if( 0 ) fprintf( stdout, "FC1 IOB[1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b15_b8   , m_uPinIobFixH[FC00][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b23_b16, m_uPinIobFixH[FC00][2] ); if( 0 ) fprintf( stdout, "FC1 IOB[2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b23_b16  , m_uPinIobFixH[FC00][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b31_b24, m_uPinIobFixH[FC00][3] ); if( 0 ) fprintf( stdout, "FC1 IOB[3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b31_b24  , m_uPinIobFixH[FC00][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b39_b32, m_uPinIobFixH[FC00][4] ); if( 0 ) fprintf( stdout, "FC1 IOB[4] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b39_b32  , m_uPinIobFixH[FC00][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b47_b40, m_uPinIobFixH[FC00][5] ); if( 0 ) fprintf( stdout, "FC1 IOB[5] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXH_b47_b40  , m_uPinIobFixH[FC00][5] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b7_b0  , m_uPinIobFixH[FC01][0] ); if( 0 ) fprintf( stdout, "FC2 IOB[0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b7_b0    , m_uPinIobFixH[FC01][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b15_b8 , m_uPinIobFixH[FC01][1] ); if( 0 ) fprintf( stdout, "FC2 IOB[1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b15_b8   , m_uPinIobFixH[FC01][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b23_b16, m_uPinIobFixH[FC01][2] ); if( 0 ) fprintf( stdout, "FC2 IOB[2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b23_b16  , m_uPinIobFixH[FC01][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b31_b24, m_uPinIobFixH[FC01][3] ); if( 0 ) fprintf( stdout, "FC2 IOB[3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b31_b24  , m_uPinIobFixH[FC01][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b39_b32, m_uPinIobFixH[FC01][4] ); if( 0 ) fprintf( stdout, "FC2 IOB[4] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b39_b32  , m_uPinIobFixH[FC01][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b47_b40, m_uPinIobFixH[FC01][5] ); if( 0 ) fprintf( stdout, "FC2 IOB[5] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXH_b47_b40  , m_uPinIobFixH[FC01][5] );
	}
	//if( bEn ) SetPinIobDreFixh( true  );
	//else      SetPinIobDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinIobFixL ( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinIobFixL[fc][nArray]  = (m_uPinIobFixL[fc][nArray] & ~(1<<nOffset));
		m_uPinIobFixL[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b7_b0  , m_uPinIobFixL[FC00][0] ); if( 0 ) fprintf( stdout, "FC1 IOB[0] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b7_b0    , m_uPinIobFixL[FC00][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b15_b8 , m_uPinIobFixL[FC00][1] ); if( 0 ) fprintf( stdout, "FC1 IOB[1] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b15_b8   , m_uPinIobFixL[FC00][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b23_b16, m_uPinIobFixL[FC00][2] ); if( 0 ) fprintf( stdout, "FC1 IOB[2] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b23_b16  , m_uPinIobFixL[FC00][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b31_b24, m_uPinIobFixL[FC00][3] ); if( 0 ) fprintf( stdout, "FC1 IOB[3] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b31_b24  , m_uPinIobFixL[FC00][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b39_b32, m_uPinIobFixL[FC00][4] ); if( 0 ) fprintf( stdout, "FC1 IOB[4] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b39_b32  , m_uPinIobFixL[FC00][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b47_b40, m_uPinIobFixL[FC00][5] ); if( 0 ) fprintf( stdout, "FC1 IOB[5] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_FIXL_b47_b40  , m_uPinIobFixL[FC00][5] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b7_b0  , m_uPinIobFixL[FC01][0] ); if( 0 ) fprintf( stdout, "FC2 IOB[0] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b7_b0    , m_uPinIobFixL[FC01][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b15_b8 , m_uPinIobFixL[FC01][1] ); if( 0 ) fprintf( stdout, "FC2 IOB[1] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b15_b8   , m_uPinIobFixL[FC01][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b23_b16, m_uPinIobFixL[FC01][2] ); if( 0 ) fprintf( stdout, "FC2 IOB[2] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b23_b16  , m_uPinIobFixL[FC01][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b31_b24, m_uPinIobFixL[FC01][3] ); if( 0 ) fprintf( stdout, "FC2 IOB[3] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b31_b24  , m_uPinIobFixL[FC01][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b39_b32, m_uPinIobFixL[FC01][4] ); if( 0 ) fprintf( stdout, "FC2 IOB[4] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b39_b32  , m_uPinIobFixL[FC01][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b47_b40, m_uPinIobFixL[FC01][5] ); if( 0 ) fprintf( stdout, "FC2 IOB[5] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_IOB::_FC2_IOB_FIXL_b47_b40  , m_uPinIobFixL[FC01][5] );
	}
	//SetPinIobDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinUclkFixH( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 87  ) nEnd   = 87;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinUclkFixH[fc][nArray]  = (m_uPinUclkFixH[fc][nArray] & ~(1<<nOffset));
		m_uPinUclkFixH[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b7_b0   , m_uPinUclkFixH[FC00][ 0] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 0] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b7_b0   , m_uPinUclkFixH[FC00][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b15_b8  , m_uPinUclkFixH[FC00][ 1] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 1] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b15_b8  , m_uPinUclkFixH[FC00][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b23_b16 , m_uPinUclkFixH[FC00][ 2] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 2] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b23_b16 , m_uPinUclkFixH[FC00][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b31_b24 , m_uPinUclkFixH[FC00][ 3] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 3] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b31_b24 , m_uPinUclkFixH[FC00][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b39_b32 , m_uPinUclkFixH[FC00][ 4] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 4] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b39_b32 , m_uPinUclkFixH[FC00][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b47_b40 , m_uPinUclkFixH[FC00][ 5] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 5] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b47_b40 , m_uPinUclkFixH[FC00][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b55_b48 , m_uPinUclkFixH[FC00][ 6] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 6] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b55_b48 , m_uPinUclkFixH[FC00][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b63_b56 , m_uPinUclkFixH[FC00][ 7] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 7] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b63_b56 , m_uPinUclkFixH[FC00][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b71_b64 , m_uPinUclkFixH[FC00][ 8] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 8] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b71_b64 , m_uPinUclkFixH[FC00][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b79_b72 , m_uPinUclkFixH[FC00][ 9] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 9] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b79_b72 , m_uPinUclkFixH[FC00][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80 , m_uPinUclkFixH[FC00][10] ); if( 0 ) fprintf( stdout, "FC1 UCLK[10] FIXH FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80 , m_uPinUclkFixH[FC00][10] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b7_b0   , m_uPinUclkFixH[FC01][ 0] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 0] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b7_b0   , m_uPinUclkFixH[FC01][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b15_b8  , m_uPinUclkFixH[FC01][ 1] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 1] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b15_b8  , m_uPinUclkFixH[FC01][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b23_b16 , m_uPinUclkFixH[FC01][ 2] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 2] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b23_b16 , m_uPinUclkFixH[FC01][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b31_b24 , m_uPinUclkFixH[FC01][ 3] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 3] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b31_b24 , m_uPinUclkFixH[FC01][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b39_b32 , m_uPinUclkFixH[FC01][ 4] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 4] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b39_b32 , m_uPinUclkFixH[FC01][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b47_b40 , m_uPinUclkFixH[FC01][ 5] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 5] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b47_b40 , m_uPinUclkFixH[FC01][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b55_b48 , m_uPinUclkFixH[FC01][ 6] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 6] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b55_b48 , m_uPinUclkFixH[FC01][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b63_b56 , m_uPinUclkFixH[FC01][ 7] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 7] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b63_b56 , m_uPinUclkFixH[FC01][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b71_b64 , m_uPinUclkFixH[FC01][ 8] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 8] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b71_b64 , m_uPinUclkFixH[FC01][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b79_b72 , m_uPinUclkFixH[FC01][ 9] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 9] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b79_b72 , m_uPinUclkFixH[FC01][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80 , m_uPinUclkFixH[FC01][10] ); if( 0 ) fprintf( stdout, "FC2 UCLK[10] FIXH FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80 , m_uPinUclkFixH[FC01][10] );
	}
	//if( bEn ) SetPinUclkDreFixh( true  );
	//else      SetPinUclkDreFixh( false );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinUclkFixL( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 87  ) nEnd   = 87;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinUclkFixL[fc][nArray]  = (m_uPinUclkFixL[fc][nArray] & ~(1<<nOffset));
		m_uPinUclkFixL[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b7_b0   , m_uPinUclkFixL[FC00][ 0] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 0] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b7_b0   , m_uPinUclkFixL[FC00][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b15_b8  , m_uPinUclkFixL[FC00][ 1] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 1] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b15_b8  , m_uPinUclkFixL[FC00][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b23_b16 , m_uPinUclkFixL[FC00][ 2] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 2] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b23_b16 , m_uPinUclkFixL[FC00][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b31_b24 , m_uPinUclkFixL[FC00][ 3] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 3] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b31_b24 , m_uPinUclkFixL[FC00][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b39_b32 , m_uPinUclkFixL[FC00][ 4] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 4] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b39_b32 , m_uPinUclkFixL[FC00][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b47_b40 , m_uPinUclkFixL[FC00][ 5] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 5] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b47_b40 , m_uPinUclkFixL[FC00][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b55_b48 , m_uPinUclkFixL[FC00][ 6] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 6] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b55_b48 , m_uPinUclkFixL[FC00][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b63_b56 , m_uPinUclkFixL[FC00][ 7] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 7] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b63_b56 , m_uPinUclkFixL[FC00][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b71_b64 , m_uPinUclkFixL[FC00][ 8] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 8] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b71_b64 , m_uPinUclkFixL[FC00][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b79_b72 , m_uPinUclkFixL[FC00][ 9] ); if( 0 ) fprintf( stdout, "FC1 UCLK[ 9] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b79_b72 , m_uPinUclkFixL[FC00][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 , m_uPinUclkFixL[FC00][10] ); if( 0 ) fprintf( stdout, "FC1 UCLK[10] FIXL FC1 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 , m_uPinUclkFixL[FC00][10] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b7_b0   , m_uPinUclkFixL[FC01][ 0] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 0] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b7_b0   , m_uPinUclkFixL[FC01][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b15_b8  , m_uPinUclkFixL[FC01][ 1] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 1] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b15_b8  , m_uPinUclkFixL[FC01][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b23_b16 , m_uPinUclkFixL[FC01][ 2] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 2] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b23_b16 , m_uPinUclkFixL[FC01][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b31_b24 , m_uPinUclkFixL[FC01][ 3] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 3] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b31_b24 , m_uPinUclkFixL[FC01][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b39_b32 , m_uPinUclkFixL[FC01][ 4] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 4] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b39_b32 , m_uPinUclkFixL[FC01][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b47_b40 , m_uPinUclkFixL[FC01][ 5] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 5] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b47_b40 , m_uPinUclkFixL[FC01][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b55_b48 , m_uPinUclkFixL[FC01][ 6] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 6] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b55_b48 , m_uPinUclkFixL[FC01][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b63_b56 , m_uPinUclkFixL[FC01][ 7] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 7] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b63_b56 , m_uPinUclkFixL[FC01][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b71_b64 , m_uPinUclkFixL[FC01][ 8] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 8] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b71_b64 , m_uPinUclkFixL[FC01][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b79_b72 , m_uPinUclkFixL[FC01][ 9] ); if( 0 ) fprintf( stdout, "FC2 UCLK[ 9] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b79_b72 , m_uPinUclkFixL[FC01][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 , m_uPinUclkFixL[FC01][10] ); if( 0 ) fprintf( stdout, "FC2 UCLK[10] FIXL FC2 W:0x%08X - 0x%02X\n", BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 , m_uPinUclkFixL[FC01][10] );
	}
	//SetPinUclkDreFixh( false );
}

//Pin FixH/FixL for Diagnostic --> //2023.02.27 - Fc1/2 IOA DRE FIXH
void CSys::SetPinIoaDreFixh(bool bEn)
{
	int nEn = (bEn)?0x1:0x0;
	g_pcie.write_user( BITCHART::DRV_IOA::_FC12_IOA_DRE_FIXH, nEn );
	if( 0 ) fprintf( stdout, "IOA DreFixH Write(0x%08X,0x%02X)\n", BITCHART::DRV_IOA::_FC12_IOA_DRE_FIXH, nEn );
	SysDelay( 100 US );
}

//Pin FixH/FixL for Diagnostic --> //2023.02.27 - Fc1/2 IOB DRE FIXH
void CSys::SetPinIobDreFixh(bool bEn)
{
	int nEn = (bEn)?0x1:0x0;
	g_pcie.write_user( BITCHART::DRV_IOB::_FC12_IOB_DRE_FIXH, nEn );
	if( 0 ) fprintf( stdout, "IOB DreFixH Write(0x%08X,0x%02X)\n", BITCHART::DRV_IOB::_FC12_IOB_DRE_FIXH, nEn );
	SysDelay( 100 US );
}

void CSys::SetPinFc1DrvFixh(bool bEn)
{
	int nEn = (bEn)?0x1:0x0;
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_DRE_FIXH, nEn );
	if( 0 ) fprintf( stdout, "IOA DreFixH Write(0x%08X,0x%02X)\n", BITCHART::DRV_IOA::_FC1_DRV_DRE_FIXH, nEn );
	SysDelay( 100 US );
}

void CSys::SetPinFc2DrvFixh(bool bEn)
{
	int nEn = (bEn)?0x1:0x0;
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_DRE_FIXH, nEn );
	if( 0 ) fprintf( stdout, "IOA DreFixH Write(0x%08X,0x%02X)\n", BITCHART::DRV_IOB::_FC2_DRV_DRE_FIXH, nEn );
	SysDelay( 100 US );
}

//Pin FixH/FixL for Diagnostic
void CSys::SetPinUclkDreFixh(bool bEn)
{
	int nEn = (bEn)?0x1:0x0;
	g_pcie.write_user( BITCHART::DRV_UCLK::_UCLK_DRE_FIXH, nEn );
	if( 0 ) fprintf( stdout, "UCLK DreFixH Write(0x%08X,0x%02X)\n", BITCHART::DRV_UCLK::_UCLK_DRE_FIXH, nEn );
	SysDelay( 100 US );
}

//Pin FixH/FixL for Diagnostic
void CSys::GetIoaReadData(uint fc, bool* p, uint size )
{
	uint uio[ 6] = {0x0,};
	bool bio[48] = {false,};
	if( size < 1  ) size = 1;
	if( size > 48 ) size = 48;
	if( fc == FC00 )
	{
		uio[0] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b7_b0   , uio[0] );	}
		uio[1] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b15_b8  , uio[1] );	}
		uio[2] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b23_b16 , uio[2] );	}
		uio[3] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b31_b24 , uio[3] );	}
		uio[4] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b39_b32 ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b39_b32 , uio[4] );	}
		uio[5] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_IOA_RDATA_b47_b40 ); if( 0 ) { fprintf( stdout, "READ FC1 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_IOA_RDATA_b47_b40 , uio[5] );	}
	}
	else
	{
		uio[0] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b7_b0   , uio[0] );	}
		uio[1] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b15_b8  , uio[1] );	}
		uio[2] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b23_b16 , uio[2] );	}
		uio[3] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b31_b24 , uio[3] );	}
		uio[4] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b39_b32 ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b39_b32 , uio[4] );	}
		uio[5] = g_pcie.read_user( BITCHART::DRV_IOA::_FC2_IOA_RDATA_b47_b40 ); if( 0 ) { fprintf( stdout, "READ FC2 IOA(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC2_IOA_RDATA_b47_b40 , uio[5] );	}
	}
	for( int i=0;i<6;i++)
	{
		for(int r=0;r<8;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bio[offset] = ((1<<r) & uio[i])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bio[i];
}

void CSys::GetIobReadData(uint fc, bool* p, uint size )
{
	uint uio[ 6] = {0x0,};
	bool bio[48] = {false,};
	if( size < 1  ) size = 1;
	if( size > 48 ) size = 48;
	if( fc == FC00 )
	{
		uio[0] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b7_b0   , uio[0] ); }
		uio[1] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b15_b8  , uio[1] ); }
		uio[2] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b23_b16 , uio[2] ); }
		uio[3] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b31_b24 , uio[3] ); }
		uio[4] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b39_b32 ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b39_b32 , uio[4] ); }
		uio[5] = g_pcie.read_user( BITCHART::DRV_IOB::_FC1_IOB_RDATA_b47_b40 ); if( 0 ) { fprintf( stdout, "READ FC1 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b47_b40 , uio[5] ); }
	}
	else
	{
		uio[0] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b7_b0   , uio[0] ); }
		uio[1] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b15_b8  , uio[1] ); }
		uio[2] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b23_b16 , uio[2] ); }
		uio[3] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b31_b24 , uio[3] ); }
		uio[4] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b39_b32 ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b39_b32 , uio[4] ); }
		uio[5] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_IOB_RDATA_b47_b40 ); if( 0 ) { fprintf( stdout, "READ FC2 IOB(0x%08X) : 0x%02X\n", BITCHART::DRV_IOB::_FC1_IOB_RDATA_b47_b40 , uio[5] ); }
	}
	for( int i=0;i<6;i++)
	{
		for(int r=0;r<8;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bio[offset] = ((1<<r) & uio[i])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bio[i];
}

void CSys::GetDrvReadData(uint fc, bool* p, uint size )
{
	uint udrv[ 4] = {0x0,};
	bool bdrv[32] = {false,};
	if( size < 1  ) size = 1;
	if( size > 32 ) size = 32;
	if( fc == FC00 )
	{
		udrv[0] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_DRV_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC1 DRV0(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b7_b0  , udrv[0] ); }
		udrv[1] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_DRV_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC1 DRV1(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b15_b8 , udrv[1] ); }
		udrv[2] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_DRV_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC1 DRV2(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b23_b16, udrv[2] ); }
		udrv[3] = g_pcie.read_user( BITCHART::DRV_IOA::_FC1_DRV_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC1 DRV3(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b31_b24, udrv[3] ); }
	}
	else
	{
		udrv[0] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_DRV_RDATA_b7_b0   ); if( 0 ) { fprintf( stdout, "READ FC2 DRV0(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b7_b0  , udrv[0] ); }
		udrv[1] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_DRV_RDATA_b15_b8  ); if( 0 ) { fprintf( stdout, "READ FC2 DRV1(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b15_b8 , udrv[1] ); }
		udrv[2] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_DRV_RDATA_b23_b16 ); if( 0 ) { fprintf( stdout, "READ FC2 DRV2(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b23_b16, udrv[2] ); }
		udrv[3] = g_pcie.read_user( BITCHART::DRV_IOB::_FC2_DRV_RDATA_b31_b24 ); if( 0 ) { fprintf( stdout, "READ FC2 DRV3(0x%08X) : 0x%02X\n", BITCHART::DRV_IOA::_FC1_DRV_RDATA_b31_b24, udrv[3] ); }
	}
	for( int i=0;i<4;i++)
	{
		for(int r=0;r<8;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bdrv[offset] = ((1<<r) & udrv[i])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bdrv[i];
}

void CSys::GetUclkReadData(uint fc, bool* p, uint size )
{
	uint uuclk[11] = {0x0,};
	bool buclk[88] = {false,};
	if( size < 1  ) size = 1;
	if( size > 88 ) size = 88;
	if( fc == FC00 )
	{
		uuclk[ 0] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b7_b0   ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  0, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b7_b0  , uuclk[ 0] ); }
		uuclk[ 1] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b15_b8  ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  1, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b15_b8 , uuclk[ 1] ); }
		uuclk[ 2] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b23_b16 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  2, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b23_b16, uuclk[ 2] ); }
		uuclk[ 3] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b31_b24 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  3, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b31_b24, uuclk[ 3] ); }
		uuclk[ 4] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b39_b32 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  4, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b39_b32, uuclk[ 4] ); }
		uuclk[ 5] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b47_b40 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  5, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b47_b40, uuclk[ 5] ); }
		uuclk[ 6] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b55_b48 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  6, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b55_b48, uuclk[ 6] ); }
		uuclk[ 7] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b63_b56 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  7, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b63_b56, uuclk[ 7] ); }
		uuclk[ 8] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b71_b64 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  8, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b71_b64, uuclk[ 8] ); }
		uuclk[ 9] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b79_b72 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n",  9, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b79_b72, uuclk[ 9] ); }
		uuclk[10] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b87_b80 ); if( 0 ) { fprintf(stdout, "READ FC1 UCLK%d(0x%08X) : 0x%02X\n", 10, BITCHART::DRV_UCLK::_FC1_UCLK_RDATA_b87_b80, uuclk[10] ); }
	}
	else
	{
		uuclk[ 0] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b7_b0   ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  0, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b7_b0  , uuclk[ 0] ); }
		uuclk[ 1] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b15_b8  ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  1, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b15_b8 , uuclk[ 1] ); }
		uuclk[ 2] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b23_b16 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  2, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b23_b16, uuclk[ 2] ); }
		uuclk[ 3] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b31_b24 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  3, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b31_b24, uuclk[ 3] ); }
		uuclk[ 4] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b39_b32 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  4, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b39_b32, uuclk[ 4] ); }
		uuclk[ 5] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b47_b40 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  5, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b47_b40, uuclk[ 5] ); }
		uuclk[ 6] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b55_b48 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  6, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b55_b48, uuclk[ 6] ); }
		uuclk[ 7] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b63_b56 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  7, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b63_b56, uuclk[ 7] ); }
		uuclk[ 8] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b71_b64 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  8, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b71_b64, uuclk[ 8] ); }
		uuclk[ 9] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b79_b72 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n",  9, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b79_b72, uuclk[ 9] ); }
		uuclk[10] = g_pcie.read_user( BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b87_b80 ); if( 0 ) { fprintf(stdout, "READ FC2 UCLK%d(0x%08X) : 0x%02X\n", 10, BITCHART::DRV_UCLK::_FC2_UCLK_RDATA_b87_b80, uuclk[10] ); }
	}
	for( int i=0;i<11;i++)
	{
		for(int r=0;r<8;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			buclk[offset] = ((1<<r) & uuclk[i])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = buclk[i];
}

//TPG-PE DC CHECK
void CSys::SetIoaReadSignal(uint uExpect, bool bEn)
{
	uint uRead = (uExpect<<1)&0x3;
	uint uEnab = (bEn)?0x1<<2:0x0<<2;
	g_pcie.write_user( BITCHART::DRV_IOA::_READ_SIGNAL_ENABLE, uEnab|uRead );
}

//TPG-PE DC CHECK
void CSys::SetIobReadSignal(uint uExpect, bool bEn)
{
	uint uRead = (uExpect<<1)&0x3;
	uint uEnab = (bEn)?0x1<<2:0x0<<2;
	g_pcie.write_user( BITCHART::DRV_IOB::_READ_SIGNAL_ENABLE, uEnab|uRead );
}

//TPG-PE DC CHECK
void CSys::SetUclkReadSignal(uint uExpect, bool bEn)
{
	uint uRead = (uExpect<<1)&0x3;
	uint uEnab = (bEn)?0x1<<2:0x0<<2;
	g_pcie.write_user( BITCHART::DRV_UCLK::_READ_SIGNAL_ENABLE, uEnab|uRead );
}


//TPG-PE DC CHECK
void CSys::GetTpgIoaReadData(uint fc, bool* p, uint size )
{
	uint uio[ 2] = {0x0,};
	bool bio[48] = {false,};
	if( size < 1  ) size = 1;
	if( size > 48 ) size = 48;
	if( fc == FC00 )
	{
		uio[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC1_IOA_RDATA_b31_b0  );
		uio[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC1_IOA_RDATA_b47_b32 );
	}
	else
	{
		uio[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC2_IOA_RDATA_b31_b0  );
		uio[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC2_IOA_RDATA_b47_b32 );
	}
	for( int i=0;i<2;i++)
	{
		for(int r=0;r<32;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bio[offset] = ((1<<r) & uio[r])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bio[i];
}

//TPG-PE DC CHECK
void CSys::GetTpgIobReadData(uint fc, bool* p, uint size )
{
	uint uio[ 2] = {0x0,};
	bool bio[48] = {false,};
	if( size < 1  ) size = 1;
	if( size > 48 ) size = 48;
	if( fc == FC00 )
	{
		uio[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC1_IOB_RDATA_b31_b0  );
		uio[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC1_IOB_RDATA_b47_b32 );
	}
	else
	{
		uio[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC2_IOB_RDATA_b31_b0  );
		uio[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC2_IOB_RDATA_b47_b32 );
	}
	for( int i=0;i<2;i++)
	{
		for(int r=0;r<32;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bio[offset] = ((1<<r) & uio[r])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bio[i];
}

//TPG-PE DC CHECK
void CSys::GetTpgDrvReadData(uint fc, bool* p, uint size )
{
	uint udrv[ 1] = {0x0,};
	bool bdrv[32] = {false,};
	if( size < 1  ) size = 1;
	if( size > 32 ) size = 32;
	if( fc == FC00 )
	{
		udrv[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_DRV_RDATA_b31_b0  );
	}
	else
	{
		udrv[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_DRV_RDATA_b31_b0  );
	}
	for( int i=0;i<1;i++)
	{
		for(int r=0;r<32;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			bdrv[offset] = ((1<<r) & udrv[r])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = bdrv[i];
}

//TPG-PE DC CHECK
void CSys::GetTpgUclkReadData(uint fc, bool* p, uint size )
{
	uint uuclk[ 3] = {0x0,};
	bool buclk[88] = {false,};
	if( size < 1  ) size = 1;
	if( size > 88 ) size = 88;
	if( fc == FC00 )
	{
		uuclk[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b31_b0  );
		uuclk[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b63_b32  );
		uuclk[2] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b87_b64  );
	}
	else
	{
		uuclk[0] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b31_b0  );
		uuclk[1] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b63_b32  );
		uuclk[2] = g_pcie.read_user( BITCHART::SYSTEM_MANAGEMENT::_FC_UCLK_RDATA_b87_b64  );
	}
	for( int i=0;i<3;i++)
	{
		for(int r=0;r<32;r++)
		{
			uint offset = (i*8)+r;
			if( offset >= size ) break;
			buclk[offset] = ((1<<r) & uuclk[r])?0x1:0x0;
		}
	}
	for( uint i=0;i<size;i++)
		p[i] = buclk[i];
}

bool CSys::SysWriteDbm( uint start, char* p, uint size )
{
	char* pDbm = p;
	uint64_t dbm_main_addr   = start;
	uint64_t dbm_address     = DBM_START_ADDR + dbm_main_addr;

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	SysDelay( 100 US );

	int total_size = size;
	int loop_count = total_size / DMA_WR_MAX_SIZE;
	int remain_size= total_size - ( loop_count * DMA_WR_MAX_SIZE);

	if( 0 ) printf("SysWriteDbm Size : %d Byte, Loop : %d, Remain : %d struct_size : %ld\n", total_size, loop_count, remain_size, sizeof(DBM_STRUCT) );
	for( int i=0;i<loop_count;i++ )
	{
		if( pDbm )
		{
			bool bWPass = true;
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) );
			bWPass = g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA , pDbm, DMA_WR_MAX_SIZE );
		}
		dbm_address += DMA_WR_MAX_SIZE;
		pDbm += DMA_WR_MAX_SIZE;
	}
	if( remain_size > 0 )
	{
		if( pDbm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA   , pDbm, remain_size );
		}
	}
	return true;
}

bool CSys::SysReadDbm ( uint start, char* p, uint size )
{
	char* pDbm = p;
	uint64_t dbm_main_addr   = start;
	uint64_t dbm_address     = DBM_START_ADDR + dbm_main_addr;

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	SysDelay( 100 US );

	int total_size = size;
	int loop_count = total_size / DMA_WR_MAX_SIZE;
	int remain_size= total_size - ( loop_count * DMA_WR_MAX_SIZE);

	if( 0 ) printf("SysReadDbm Size : %d Byte, Loop : %d, Remain : %d struct_size : %ld\n", total_size, loop_count, remain_size, sizeof(DBM_STRUCT) );
	for( int i=0;i<loop_count;i++ )
	{
		if( pDbm )
		{
			bool bWPass = true;
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) );
			bWPass = g_pcie.read_dma( BITCHART::ALPG::_DMA_DATA , pDbm, DMA_WR_MAX_SIZE );
		}
		dbm_address += DMA_WR_MAX_SIZE;
		pDbm += DMA_WR_MAX_SIZE;
	}
	if( remain_size > 0 )
	{
		if( pDbm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) );
			g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , pDbm, remain_size );
		}
	}
	return true;
}

bool CSys::SysWriteExtfm( uint fc, uint64_t start, char* p, uint size )
{
	char* pExtFm = p;
	uint64_t dfm_main_addr   = start;
	uint64_t dfm_address     = 0LL;
	if( fc == FC00 ) dfm_address = FC1_DFM_STA_ADDR + dfm_main_addr;
	else             dfm_address = FC2_DFM_STA_ADDR + dfm_main_addr;

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//0:DMA WRITE, 1:DFM WRITE
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_SAVE_EN  , 0x0 );	//0:DISABLE, 1:ENABLE
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x1 );	//DBM:0, DFM:1
	SysDelay( 100 US );

	int total_size = size;
	int loop_count = total_size / DMA_WR_MAX_SIZE;
	int remain_size= total_size - ( loop_count * DMA_WR_MAX_SIZE);

	if( 0 ) printf("SysWriteExtfm Address 0x%12lX Size : %d Byte, Loop : %d, Remain : %d\n", dfm_address, total_size, loop_count, remain_size );
	for( int i=0;i<loop_count;i++ )
	{
		if( pExtFm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dfm_address, sizeof(uint64_t) );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA , pExtFm, DMA_WR_MAX_SIZE );
		}
		dfm_address += DMA_WR_MAX_SIZE;
		pExtFm += DMA_WR_MAX_SIZE;
	}
	if( remain_size > 0 )
	{
		if( pExtFm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dfm_address, sizeof(uint64_t) );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA   , pExtFm, remain_size );
		}
	}
	SysDelay( 1 MS );
	return true;
}

bool CSys::SysReadExtfm ( uint fc, uint64_t start, char* p, uint size )
{
	char* pExtFm = p;
	uint64_t dfm_main_addr   = start;
	uint64_t dfm_address     = 0LL;
	if( fc == FC00 ) dfm_address = FC1_DFM_STA_ADDR + dfm_main_addr;
	else             dfm_address = FC2_DFM_STA_ADDR + dfm_main_addr;

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//0:DMA WRITE, 1:DFM WRITE
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x1 );	//DBM:0, DFM:1
	SysDelay( 100 US );

	int total_size = size;
	int loop_count = total_size / DMA_WR_MAX_SIZE;
	int remain_size= total_size - ( loop_count * DMA_WR_MAX_SIZE);

	if( 0 ) printf("SysReadExtfm Size : %d Byte, Loop : %d, Remain : %d\n", total_size, loop_count, remain_size );
	for( int i=0;i<loop_count;i++ )
	{
		if( pExtFm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dfm_address, sizeof(uint64_t) );
			g_pcie.read_dma( BITCHART::ALPG::_DMA_DATA , pExtFm, DMA_WR_MAX_SIZE );
		}
		dfm_address += DMA_WR_MAX_SIZE;
		pExtFm += DMA_WR_MAX_SIZE;
	}
	if( remain_size > 0 )
	{
		if( pExtFm )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dfm_address, sizeof(uint64_t) );
			g_pcie.read_dma( BITCHART::ALPG::_DMA_DATA   , pExtFm, remain_size );
		}
	}
	return true;
}


void CSys::SetRunmode( bool run )
{
	m_RUNMODE = run;
}
bool CSys::GetRunmode()
{
	return m_RUNMODE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 2023.05.09 - KWMOON(CHAMBER ADD)
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSys::ChamberSetUsage(bool usage)
{
	m_bChamberUsage = usage;
}

bool CSys::ChamberIsUse()
{
	return 	m_bChamberUsage;
}

//2023.07.09 - KWMOON
void CSys::SetChmbSoakTime( int min )
{
	m_nChmbSoakTime = min;
}

//2023.07.09 - KWMOON
void CSys::SetChmbKeepTime( int min )
{
	m_nChmbKeepTime = min;
}

//2023.07.09 - KWMOON
int  CSys::GetChmbSoakTime()
{
	return m_nChmbSoakTime;
}

//2023.07.09 - KWMOON
int  CSys::GetChmbKeepTime()
{
	return m_nChmbKeepTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CSys::ChamberInitTemp
/// \return
/// \details STEP_TEMP first and Last init temp call
PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberInitTemp()
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
	int nRetry = 0;
	bool bSuccess = false;
	do
	{
		if( g_devsock.WriteElement( PROTOCOL::CMD_CHMB_INIT_TEMP_REQ, nullptr, 0x0 ) )
			break;
		SysDelay( 1 MS );
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry >= MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "INIT_TEMP", "Init temperature request error!" );
		return PROTOCOL::PGM_CHMB_FAULT;
	}
	//g_log.Printf( _TEMP, "INIT_TEMP",  "[%10s][S#%02d-R#%03d] %20s", "->MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_INIT_TEMP_REQ).toStdString().c_str() );

	stChmbCmdResponse response;
	stTcpElement tcpelement;
	nRetry = 0;
	do
	{
		//g_log.Printf( _TEMP, "INIT_TEMP",  "[%10s][S#%02d-R#%03d] %20s", "<-MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_INIT_TEMP_ACK).toStdString().c_str() );
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}
		PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_INIT_TEMP_ACK;
		PROTOCOL::TCP_COMMAND recv_cmd = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
		if( recv_cmd != expect_cmd )
		{
			g_log.EPrintf( _TEMP, "INIT_TEMP", "Expect response cmd[%s] recv[%s]",
                           UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_INIT_TEMP_ACK).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
		if( tcpelement.tcpHeader.datalen == sizeof(stChmbCmdResponse) )
		{
			memcpy( (char*)&response , tcpelement.pData, sizeof(stChmbCmdResponse)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( response.nAck );
            break;
		}
		else
		{
			g_log.EPrintf( _TEMP, "INIT_TEMP", "Expect receive data length[%d], but received[%d]",
						   sizeof(stChmbCmdResponse), tcpelement.tcpHeader.datalen );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry >= MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "INIT_TEMP", "Initial result receive error!" );
		ack = PROTOCOL::PGM_CHMB_FAULT;
	}
    return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberEnter()
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
    if( !ChamberIsUse() )
    {
        return PROTOCOL::PGM_CHMB_NOUSE;
    }

	bool bSuccess = false;
	stChmbCmdResponse response;
	stTcpElement tcpelement;
	int nWriteRetry = 0;
	int nReadRetry  = 0;

	//Request Enter
	do
	{
		//g_log.Printf( _TEMP, "ENTER",  "[%10s][S#%02d-R#%03d] %20s", "->MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_ENTER_SYNC_REQ).toStdString().c_str() );
		if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_ENTER_SYNC_REQ, nullptr, 0x0 ) )
		{
			SysDelay( 10 MS );
			continue;
		}
		break;
	}while( ++nWriteRetry < MAX_RETRY_CONN_CNT );

	if( nWriteRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "ENTER", "Enter request error!" );
		return PROTOCOL::PGM_CHMB_FAULT;
	}
	SysDelay( 100 MS );		//2023.08.18 - KWMOON

	//Response Enter
	do
	{
		//g_log.Printf( _TEMP, "ENTER",  "[%10s][S#%02d-R#%03d] %20s", "<-MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_ENTER_SYNC_ACK).toStdString().c_str() );
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}
		break;
	}while( ++nReadRetry < MAX_RETRY_CONN_CNT );

	if( nReadRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "ENTER", "Enter response error!" );
		return PROTOCOL::PGM_CHMB_FAULT;
	}

	PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_ENTER_SYNC_ACK;
	PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
	if( recv_cmd != expect_cmd )
	{
		g_log.EPrintf( _TEMP, "ENTER", "Expect response cmd[%s] recv[%s]",
                       UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
		bSuccess = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
	}

	if( tcpelement.tcpHeader.datalen == sizeof(stChmbCmdResponse) )
	{
		memcpy( (char*)&response , tcpelement.pData, sizeof(stChmbCmdResponse)  );
        ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( response.nAck );
		bSuccess = true;
	}
	else
	{
		g_log.EPrintf( _TEMP, "ENTER", "Expect receive data length[%d], but received[%d]",
					   sizeof(stChmbCmdResponse), tcpelement.tcpHeader.datalen );
		bSuccess = false;
        return PROTOCOL::PGM_CHMB_FAULT;
	}

    if( ack == PROTOCOL::PGM_CHMB_FAULT )
	{
		g_log.EPrintf( _TEMP, "ENTER", "[ERRMSG] %s!", response.szMsg );
	}
    return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberSetSv( double dTemp )
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
	if( !ChamberIsUse() )
    {
        return PROTOCOL::PGM_CHMB_NOUSE;
    }

	stSetTemp SetTemp;
	short sSv = static_cast<short>( dTemp * 10 );
	SetTemp.dTemp = dTemp;
	SetTemp.nTemp = static_cast<int>(sSv&0xFFFF);
	int nRetry = 0;
	bool bSuccess = false;
	do
	{
		if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_SETSV_VALID_REQ, (char*)&SetTemp, sizeof(stSetTemp) ) )
		{
			SysDelay( 10 MS );
			continue;
		}
		break;
	}while( ++nRetry < MAX_RETRY_CONN_CNT );
	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "SET_SV", "Set Temperature request error!" );
		return ack;
	}

	bSuccess = false;
	nRetry = 0;
	stChmbCmdResponse response;
	stTcpElement tcpelement;
	do
	{
		//g_log.Printf( _TEMP, "INIT_TEMP",  "[%10s][S#%02d-R#%03d] %20s", "<-MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_RUN_SYNC_ACK).toStdString().c_str() );
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}

		PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_SETSV_VALID_ACK;
		PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
		if( recv_cmd != expect_cmd )
		{
			g_log.EPrintf( _TEMP, "SET_SV", "Expect response cmd[%s] recv[%s]",
						   UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}

		if( tcpelement.tcpHeader.datalen == sizeof(stChmbCmdResponse) )
		{
			memcpy( (char*)&response , tcpelement.pData, sizeof(stChmbCmdResponse)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( response.nAck );
            break;
		}
		else
		{
			g_log.EPrintf( _TEMP, "SET_SV", "Expect receive data length[%d], but received[%d]",
						   sizeof(bool), tcpelement.tcpHeader.datalen );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "SET_SV", "result receive error!" );
        return PROTOCOL::PGM_CHMB_FAULT;
	}

    if( ack == PROTOCOL::PGM_CHMB_FAULT )
	{
		g_log.EPrintf( _TEMP, "SET_SV", "[ERRMSG] %s!", response.szMsg );
	}
    return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberRun()
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
	if( !ChamberIsUse() )
        return PROTOCOL::PGM_CHMB_NOUSE;

	int nRetry = 0;
	do
	{
		if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_RUN_SYNC_REQ, nullptr, 0 ) )
		{
			SysDelay( 10 MS );
			continue;
		}
		break;
	}while( ++nRetry < MAX_RETRY_CONN_CNT );
	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "CHAMBER_RUN", "sync request error!" );
		return ack;
	}
	//g_log.Printf( _TEMP, "INIT_TEMP",  "[%10s][S#%02d-R#%03d] %20s", "->MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_RUN_REQ).toStdString().c_str() );

	nRetry = 0;
	stChmbCmdResponse response;
	stTcpElement tcpelement;
	do
	{
		//g_log.Printf( _TEMP, "INIT_TEMP",  "[%10s][S#%02d-R#%03d] %20s", "<-MGR", GetCurrentSlotNo()+1, GetCurrentCpuNo()+1, UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_RUN_ACK).toStdString().c_str() );
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}

		PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_RUN_SYNC_ACK;
		PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
		if( recv_cmd != expect_cmd )
		{
			g_log.EPrintf( _TEMP, "CHAMBER_RUN", "Expect response cmd[%s] recv[%s]",
						   UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}

		if( tcpelement.tcpHeader.datalen == sizeof(stChmbCmdResponse) )
		{
			memcpy( (char*)&response , tcpelement.pData, sizeof(stChmbCmdResponse)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( response.nAck );
            break;
		}
		else
		{
			g_log.EPrintf( _TEMP, "CHAMBER_RUN", "Expect receive data length[%d], but received[%d]",
						   sizeof(bool), tcpelement.tcpHeader.datalen );
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "CHAMBER_RUN", "result receive error!" );
        return PROTOCOL::PGM_CHMB_FAULT;
	}
    if( ack == PROTOCOL::PGM_CHMB_FAULT )
	{
		g_log.EPrintf( _TEMP, "CHAMBER_RUN", "[ERRMSG] %s!", response.szMsg );
	}
    return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberReadTemp(double& rSv,double& rPv)
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
	if( !ChamberIsUse() )
    {
        rSv = 0.0;
        rPv = 0.0;
        return PROTOCOL::PGM_CHMB_NOUSE;
    }
	int nRetry = 0;
	bool bSuccess = false;
	do
	{
		if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_READ_TEMP_REQ, nullptr, 0 ) )
		{
			SysDelay( 10 MS );
			continue;
		}
		break;
	}while( ++nRetry < MAX_RETRY_CONN_CNT );
	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "TEMP_READ", "sync request error!" );
		return ack;
	}

	stGetTemp GetTemp;
	memset( &GetTemp, 0x0, sizeof(stGetTemp) );

	bSuccess = false;
	nRetry = 0;
	stTcpElement tcpelement;
	do
	{
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}

		PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_READ_TEMP_ACK;
		PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
		if( recv_cmd != expect_cmd )
		{
			g_log.EPrintf( _TEMP, "TEMP_READ", "Expect response cmd[%s] recv[%s]",
						   UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}

		if( tcpelement.tcpHeader.datalen == sizeof(stGetTemp) )
		{
			memcpy( (char*)&GetTemp , tcpelement.pData, sizeof(stGetTemp)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( GetTemp.nAck );
            break;
		}
		else
		{
			g_log.EPrintf( _TEMP, "TEMP_READ", "Expect receive data length[%d], but received[%d]",
						   sizeof(bool), tcpelement.tcpHeader.datalen );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
		if( bSuccess ) break;
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "TEMP_READ", "result receive error!" );
        return PROTOCOL::PGM_CHMB_FAULT;
	}
	rPv = GetTemp.dPv;
	rSv = GetTemp.dSv;
	ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( GetTemp.nAck );
    if( (ack != PROTOCOL::PGM_CHMB_ACK) && (ack != PROTOCOL::PGM_CHMB_TRUE) )
	{
		g_log.EPrintf( _TEMP, "TEMP_READ", "[ERRMSG] %s!", GetTemp.szMsg );
	}
	return ack;
}

//D101
PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberReadStatus(bool& rRun, bool& rStop, bool& rAmb)
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
	if( !ChamberIsUse() )
    {
        return PROTOCOL::PGM_CHMB_NOUSE;
    }

	int nRetry = 0;
	bool bSuccess = false;
	do
	{
		if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_READ_STAT2_REQ, nullptr, 0 ) )
		{
			SysDelay( 10 MS );
			continue;
		}
		break;
	}while( ++nRetry < MAX_RETRY_CONN_CNT );
	if( nRetry > MAX_RETRY_CONN_CNT )
	{
        g_log.EPrintf( _TEMP, "CHMB_STAT", "status read request error!" );
		return ack;
	}

	stGetChmbWord GetStatus;
	memset( &GetStatus, 0x0, sizeof(stGetChmbWord) );

	bSuccess = false;
	nRetry = 0;
	stTcpElement tcpelement;
	do
	{
		if( !g_devsock.ReadElement( tcpelement ) )
		{
			SysDelay( 100 MS );
			continue;
		}

		PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_READ_STAT2_ACK;
		PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
		if( recv_cmd != expect_cmd )
		{
			g_log.EPrintf( _TEMP, "CHMB_STAT", "Expect response cmd[%s] recv[%s]",
                           UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}

		if( tcpelement.tcpHeader.datalen == sizeof(stGetChmbWord) )
		{
			memcpy( (char*)&GetStatus , tcpelement.pData, sizeof(stGetChmbWord)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( GetStatus.nAck );
            break;
		}
		else
		{
			g_log.EPrintf( _TEMP, "CHMB_STAT", "Expect receive data length[%d], but received[%d]",
						   sizeof(bool), tcpelement.tcpHeader.datalen );
			bSuccess = false;
			ack = PROTOCOL::PGM_CHMB_FAULT;
			break;
		}
	}while( ++nRetry < MAX_RETRY_CONN_CNT );

	if( nRetry > MAX_RETRY_CONN_CNT )
	{
		g_log.EPrintf( _TEMP, "CHMB_STAT", "result receive error!" );
        return PROTOCOL::PGM_CHMB_FAULT;
	}
	rRun  = GetStatus.nStatus & (1<<CHMB_STAT2_RUN);
	rStop = GetStatus.nStatus & (1<<CHMB_STAT2_STOP);
	rAmb  = GetStatus.nStatus & (1<<CHMB_STAT2_AMB);

    if( (ack != PROTOCOL::PGM_CHMB_ACK) && (ack != PROTOCOL::PGM_CHMB_TRUE) )
	{
        //ack = PROTOCOL::PGM_CHMB_FAULT;
		g_log.EPrintf( _TEMP, "CHMB_STAT", "[ERRMSG] %s!", GetStatus.szMsg );
	}
	return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberReadStatus1(bool& open,bool& close,bool& upper,bool& bottom,bool& rackin, bool& rackout)
{
    PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_FALSE;
    if( !ChamberIsUse() )
        return ack;

    int nRetry = 0;
    bool bSuccess = false;
    do
    {
        if( !g_devsock.WriteElement( PROTOCOL::CMD_CHMB_READ_STAT1_REQ, nullptr, 0 ) )
        {
            SysDelay( 10 MS );
            continue;
        }
        break;
    }while( ++nRetry < MAX_RETRY_CONN_CNT );
    if( nRetry > MAX_RETRY_CONN_CNT )
    {
        g_log.EPrintf( _TEMP, "CHMB_STAT", "status read request error!" );
        return ack;
    }

    stGetChmbWord GetStatus;
    memset( &GetStatus, 0x0, sizeof(stGetChmbWord) );

    bSuccess = false;
    nRetry = 0;
    stTcpElement tcpelement;
    do
    {
        if( !g_devsock.ReadElement( tcpelement ) )
        {
            SysDelay( 100 MS );
            continue;
        }

        PROTOCOL::TCP_COMMAND expect_cmd = PROTOCOL::CMD_CHMB_READ_STAT1_ACK;
        PROTOCOL::TCP_COMMAND recv_cmd   = static_cast<PROTOCOL::TCP_COMMAND>(tcpelement.tcpHeader.cmd);
        if( recv_cmd != expect_cmd )
        {
            g_log.EPrintf( _TEMP, "CHMB_STAT", "Expect response cmd[%s] recv[%s]",
                           UTIL::TCPIP::CMD_STR(expect_cmd).toStdString().c_str(), UTIL::TCPIP::CMD_STR(recv_cmd).toStdString().c_str() );
            bSuccess = false;
            ack = PROTOCOL::PGM_CHMB_FAULT;
            break;
        }

        if( tcpelement.tcpHeader.datalen == sizeof(stGetChmbWord) )
        {
            memcpy( (char*)&GetStatus , tcpelement.pData, sizeof(stGetChmbWord)  );
            ack = static_cast<PROTOCOL::PGM_CHMB_REQ_ACK>( GetStatus.nAck );
            break;
        }
        else
        {
            g_log.EPrintf( _TEMP, "CHMB_STAT", "Expect receive data length[%d], but received[%d]",
                           sizeof(bool), tcpelement.tcpHeader.datalen );
            bSuccess = false;
            ack = PROTOCOL::PGM_CHMB_FAULT;
            break;
        }
    }while( ++nRetry < MAX_RETRY_CONN_CNT );

    if( nRetry > MAX_RETRY_CONN_CNT )
    {
        g_log.EPrintf( _TEMP, "CHMB_STAT", "result receive error!" );
        return PROTOCOL::PGM_CHMB_FAULT;
    }
    open    = GetStatus.nStatus & (1<<CHMB_STAT1_DOOR_OPEN);
    close   = GetStatus.nStatus & (1<<CHMB_STAT1_DOOR_CLOSE);
    upper   = GetStatus.nStatus & (1<<CHMB_STAT1_UPPER_RACK_DETECT);
    bottom  = GetStatus.nStatus & (1<<CHMB_STAT1_BOTTOM_RACK_DETECT);
    rackin  = GetStatus.nStatus & (1<<CHMB_STAT1_RACK_IN_COMPLETE);
    rackout = GetStatus.nStatus & (1<<CHMB_STAT1_RACK_OUT_COMPLETE);

    if( (ack != PROTOCOL::PGM_CHMB_ACK) && (ack != PROTOCOL::PGM_CHMB_TRUE) )
    {
        //ack = PROTOCOL::PGM_CHMB_FAULT;
        g_log.EPrintf( _TEMP, "CHMB_STAT", "[ERRMSG] %s!", GetStatus.szMsg );
    }
    return ack;
}

PROTOCOL::PGM_CHMB_REQ_ACK CSys::ChamberIsAlarm()
{
	return PROTOCOL::PGM_CHMB_FAULT;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFM
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSys::SetPfmIniInit()
{
	std::string run_path = "";
	char* env_run  = getenv(ATRUN_DIR_ENV);
	if( env_run != NULL )
	{
		run_path = std::string( env_run );
	}
	else
	{
		run_path = std::string( DIR_ATRUN_PATH );
	}
	char szIni[256] = { 0x0, };
	memset( szIni, 0x0, sizeof(szIni) );

	if (GetCurrentCpuNo() == CPU00)
	{
		snprintf( szIni, sizeof(szIni), "%s/%s/%s", run_path.c_str(), CONFIG_DIR, PFM_SETVS_INI_1 );
		m_sPfmSetVsIni = std::string(szIni);
	}
	else
	{
		snprintf( szIni, sizeof(szIni), "%s/%s/%s", run_path.c_str(), CONFIG_DIR, PFM_SETVS_INI_2 );
		m_sPfmSetVsIni = std::string(szIni);
	}

	memset( szIni, 0x0, sizeof(szIni) );
	snprintf( szIni, sizeof(szIni), "%s/%s/%s", run_path.c_str(), CONFIG_DIR, PFM_FAULT_INI );
	m_sPfmFalultIni = std::string(szIni);
}

void CSys::SetPfmSetVs(uint unit, uint ch, double v)
{
	char szBuf[256] = { 0x0, };
	std::string section = "";
	std::string key = "";
	std::string section_key = "";
	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "UNIT");
	section = std::string( szBuf );

	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "%s%d", (unit==PPS)?"PPS":"S1PS", ch+1 );
	key = std::string( szBuf );
	section_key = section + "/" + key;
	//PRINTF( "%s : %s - %5.1f ", m_sPfmSetVsIni.c_str(), section_key.c_str(), v ); PRINTF(NULL);
	UTIL::INI::writeSettings( m_sPfmSetVsIni, section_key, v );
}

void CSys::SetPfmFalultClear()
{
	char szBuf[256] = { 0x0, };
	std::string section = "";
	std::string key = "";
	std::string section_key = "";
	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "CPU%d", GetCurrentCpuNo()+1 );
	section = std::string( szBuf );

	for( uint unit=PPS;unit<=S1PS;unit++ )
	{
		for( uint ch=0;ch<MAX_PPS_CH;ch++ )
		{
			memset( szBuf, 0x0, sizeof(szBuf) );
			snprintf( szBuf, sizeof(szBuf), "%s%d", (unit==PPS)?"PPS":"S1PS", ch+1 );
			key = std::string( szBuf );
			section_key = section + "/" + key;
			UTIL::INI::writeSettings( m_sPfmFalultIni, section_key, 0x0 );
		}
	}
}

bool CSys::GetPfmFault(uint unit, uint ch)
{
	char szBuf[256] = { 0x0, };
	std::string section = "";
	std::string key = "";
	std::string section_key = "";
	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "CPU%d", GetCurrentCpuNo()+1 );
	section = std::string( szBuf );

	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "%s%d", (unit==PPS)?"PPS":"S1PS", ch+1 );
	key = std::string( szBuf );
	section_key = section + "/" + key;
	int nFault = 0;
	bool bRead = UTIL::INI::readSettings( m_sPfmFalultIni, section_key, nFault );
	if( nFault != 0 )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "%s %s %d READ:%s", m_sPfmFalultIni.c_str(), section_key.c_str(), nFault, bRead?"YES":"NO" );
		return true;
	}
	return false;
}

void CSys::SetPfmEnable(bool use)
{
	m_bPfmEnable = use;
}

bool CSys::IsPfmEnable()
{
	return m_bPfmEnable;
}

void CSys::SetPfmStop()
{
    if( !m_bPfmEnable ) return;
	g_pcie.write_user( BITCHART::PFM::PFM_CLEAR, 0x1 );
	m_bPfmRun = false;
	SysDelay( 100 US );
    //g_log.Printf("PFM STOP\n");
}

void CSys::SetPfmStart()
{
    //return;
	if( !m_bPfmEnable )
	{
		m_bPfmRun = false;
		return;
	}
	g_pcie.write_user( BITCHART::PFM::PFM_REQUEST, 0x1 );
	m_bPfmRun = true;
	SysDelay( 1 US );
	//g_log.Printf("PFM START\n");
}

bool CSys::IsPfmStart()
{
	if( !m_bPfmEnable ) return false;
	return 	m_bPfmRun;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CPSClient is not use code, only for compile
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPSClient::CPSClient
/// \details for compile & measure pps/s1ps
CPSClient::CPSClient()
{

}

CPSClient::~CPSClient()
{

}

bool CPSClient::getCurrentVOUT( int id, int ch, float& rMeas )
{
	double dMeas = 0.0;
	if( (id<PPS) && (id>S1PS) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid id(%d), use 0 ~ 1", id );
		return false;
	}
	if( (ch<0) && (ch>MAX_PPS_CH) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid ch(%d), use 0 ~ %d", id, MAX_PPS_CH-1 );
		return false;
	}

	bool bOk = g_pps.ReadVout(id, ch, dMeas);
	rMeas =  static_cast<float>(dMeas);
	return bOk;
}

bool CPSClient::getCurrentIOUT( int id, int ch, float& rMeas )
{
	double dMeas = 0.0;
	if( (id<PPS) && (id>S1PS) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid id(%d), use 0 ~ 1", id );
		return false;
	}
	if( (ch<0) && (ch>MAX_PPS_CH) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid ch(%d), use 0 ~ %d", id, MAX_PPS_CH-1 );
		return false;
	}

	bool bOk = g_pps.ReadIout(id, ch, dMeas);
	rMeas =  static_cast<float>(dMeas);
	return bOk;
}

bool CPSClient::getSTATUSWORD( int id, int ch, unsigned short& rStat )
{
	R_STATUS_WORD rword;
	if( (id<PPS) && (id>S1PS) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid id(%d), use 0 ~ 1", id );
		return false;
	}
	if( (ch<0) && (ch>MAX_PPS_CH) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid ch(%d), use 0 ~ %d", id, MAX_PPS_CH-1 );
		return false;
	}
	bool bOk = g_pps.ReadStatusWord( id, ch, rword );
	rStat = rword.word;
	return bOk;
}

bool CPSClient::getSTATUSBYTE( int id, int ch, unsigned char& rStat )
{
	R_STATUS_BYTE rbyte;
	if( (id<PPS) && (id>S1PS) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid id(%d), use 0 ~ 1", id );
		return false;
	}
	if( (ch<0) && (ch>MAX_PPS_CH) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_PPS, __FUNCTION__, "invalid ch(%d), use 0 ~ %d", id, MAX_PPS_CH-1 );
		return false;
	}
	bool bOk = g_pps.ReadStatusByte( id, ch, rbyte );
	rStat = rbyte.byte;
	return bOk;
}
