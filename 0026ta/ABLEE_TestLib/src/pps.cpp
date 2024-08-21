#include "hw.hpp"
#include "io.hpp"
#include "log.hpp"
#include "pps.hpp"
#include "sys.hpp"

extern CPcie g_pcie;
extern CSys  g_sys;
extern CHw   g_hw;
extern CLog  g_log;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PMBUS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPMBus::CPMBus(PPS_UNIT t) : m_BaseType(t)
{
	CreateCmdMap();
}

void CPMBus::CreateCmdMap()
{
	m_PMBusCmd.clear();
	m_PMBusCmd[OPERATION             ] = stPMBusCmd{ BYTE  , PMBUS_WR, "OPERATION              ",  0 };
	m_PMBusCmd[ON_OFF_CONFIG         ] = stPMBusCmd{ BYTE  , PMBUS_WR, "ON_OFF_CONFIG          ",  0 };
	m_PMBusCmd[CLEAR_FAULTS          ] = stPMBusCmd{ NONE  , PMBUS_W , "CLEAR_FAULTS           ",  0 };
	m_PMBusCmd[WRITE_PROTECT         ] = stPMBusCmd{ BYTE  , PMBUS_W , "WRITE_PROTECT          ",  0 };
	m_PMBusCmd[STORE_USER_ALL        ] = stPMBusCmd{ NONE  , PMBUS_W , "STORE_USER_ALL         ",  0 };
	m_PMBusCmd[RESTORE_USER_ALL      ] = stPMBusCmd{ NONE  , PMBUS_W , "RESTORE_USER_ALL       ",  0 };
	m_PMBusCmd[CAPABILITY            ] = stPMBusCmd{ BYTE  , PMBUS_R , "CAPABILITY             ",  0 };
	m_PMBusCmd[SMBALERT_MASK         ] = stPMBusCmd{ BLOCKS, PMBUS_WR, "SMBALERT_MASK          ",  0 };
	m_PMBusCmd[VOUT_MODE             ] = stPMBusCmd{ L16   , PMBUS_R , "VOUT_MODE              ", -9 };
	m_PMBusCmd[VOUT_COMMAND          ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_COMMAND           ", -9 };
	m_PMBusCmd[VOUT_TRIM             ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_TRIM              ", -9 };
	m_PMBusCmd[VOUT_MAX              ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_MAX               ", -9 };
	m_PMBusCmd[VOUT_MARGIN_HIGH      ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_MARGIN_HIGH       ", -9 };
	m_PMBusCmd[VOUT_MARGIN_LOW       ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_MARGIN_LOW        ", -9 };
	m_PMBusCmd[VOUT_TRANSITION_RATE  ] = stPMBusCmd{ L11   , PMBUS_WR, "VOUT_TRANSITION_RATE   ", -4 };
	m_PMBusCmd[VOUT_SCALE_LOOP       ] = stPMBusCmd{ L11   , PMBUS_WR, "VOUT_SCALE_LOOP        ", -3 };
	m_PMBusCmd[FREQUENCY_SWITCH      ] = stPMBusCmd{ L11   , PMBUS_WR, "FREQUENCY_SWITCH       ",  0 };
	m_PMBusCmd[VIN_ON                ] = stPMBusCmd{ L11   , PMBUS_WR, "VIN_ON                 ", -1 };
	m_PMBusCmd[VIN_OFF               ] = stPMBusCmd{ L11   , PMBUS_WR, "VIN_OFF                ", -1 };
	m_PMBusCmd[INTERLEAVE            ] = stPMBusCmd{ WORD  , PMBUS_WR, "INTERLEAVE             ",  0 };
	m_PMBusCmd[VOUT_OV_FAULT_LIMIT   ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_OV_FAULT_LIMIT    ", -9 };
	m_PMBusCmd[VOUT_OV_FAULT_RESPONSE] = stPMBusCmd{ BYTE  , PMBUS_WR, "VOUT_OV_FAULT_RESPONSE ",  0 };
	m_PMBusCmd[VOUT_OV_WARN_LIMIT    ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_OV_WARN_LIMIT     ", -9 };
	m_PMBusCmd[VOUT_UV_WARN_LIMIT    ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_UV_WARN_LIMIT     ", -9 };
	m_PMBusCmd[VOUT_UV_FAULT_LIMIT   ] = stPMBusCmd{ L16   , PMBUS_WR, "VOUT_UV_FAULT_LIMIT    ", -9 };
	m_PMBusCmd[VOUT_UV_FAULT_RESPONSE] = stPMBusCmd{ BYTE  , PMBUS_WR, "VOUT_UV_FAULT_RESPONSE ",  0 };
	m_PMBusCmd[IOUT_OC_FAULT_LIMIT   ] = stPMBusCmd{ L11   , PMBUS_WR, "IOUT_OC_FAULT_LIMIT    ", -1 };
	m_PMBusCmd[IOUT_OC_FAULT_RESPONSE] = stPMBusCmd{ BYTE  , PMBUS_WR, "IOUT_OC_FAULT_RESPONSE ",  0 };
	m_PMBusCmd[IOUT_OC_WARN_LIMIT    ] = stPMBusCmd{ L11   , PMBUS_WR, "IOUT_OC_WARN_LIMIT     ", -1 };
	m_PMBusCmd[OT_FAULT_LIMIT        ] = stPMBusCmd{ L11   , PMBUS_WR, "OT_FAULT_LIMIT         ",  0 };
	m_PMBusCmd[OT_FAULT_RESPONSE     ] = stPMBusCmd{ BYTE  , PMBUS_WR, "OT_FAULT_RESPONSE      ",  0 };
	m_PMBusCmd[OT_WARN_LIMIT         ] = stPMBusCmd{ L11   , PMBUS_WR, "OT_WARN_LIMIT          ",  0 };
	m_PMBusCmd[VIN_OV_FAULT_LIMIT    ] = stPMBusCmd{ L11   , PMBUS_WR, "VIN_OV_FAULT_LIMIT     ", -1 };
	m_PMBusCmd[VIN_OV_FAULT_RESPONSE ] = stPMBusCmd{ BYTE  , PMBUS_WR, "VIN_OV_FAULT_RESPONSE  ",  0 };
	m_PMBusCmd[VIN_UV_WARN_LIMIT     ] = stPMBusCmd{ L11   , PMBUS_WR, "VIN_UV_WARN_LIMIT      ", -1 };
	m_PMBusCmd[IIN_OC_WARN_LIMIT     ] = stPMBusCmd{ L11   , PMBUS_WR, "IIN_OC_WARN_LIMIT      ", -1 };
	m_PMBusCmd[POWER_GOOD_ON         ] = stPMBusCmd{ L16   , PMBUS_WR, "POWER_GOOD_ON          ", -9 };
	m_PMBusCmd[POWER_GOOD_OFF        ] = stPMBusCmd{ L16   , PMBUS_WR, "POWER_GOOD_OFF         ", -9 };
	m_PMBusCmd[TON_DELAY             ] = stPMBusCmd{ L11   , PMBUS_WR, "TON_DELAY              ",  0 };
	m_PMBusCmd[TON_RISE              ] = stPMBusCmd{ L11   , PMBUS_WR, "TON_RISE               ",  0 };
	m_PMBusCmd[TON_MAX_FAULT_LIMIT   ] = stPMBusCmd{ L11   , PMBUS_WR, "TON_MAX_FAULT_LIMIT    ",  0 };
	m_PMBusCmd[TON_MAX_FAULT_RESPONSE] = stPMBusCmd{ BYTE  , PMBUS_WR, "TON_MAX_FAULT_RESPONSE ",  0 };
	m_PMBusCmd[TOFF_DELAY            ] = stPMBusCmd{ L11   , PMBUS_WR, "TOFF_DELAY             ",  0 };
	m_PMBusCmd[TOFF_FALL             ] = stPMBusCmd{ L11   , PMBUS_WR, "TOFF_FALL              ",  0 };
	m_PMBusCmd[TOFF_MAX_WARN_LIMIT   ] = stPMBusCmd{ L11   , PMBUS_WR, "TOFF_MAX_WARN_LIMIT    ",  0 };
	m_PMBusCmd[STATUS_BYTE           ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_BYTE            ",  0 };
	m_PMBusCmd[STATUS_WORD           ] = stPMBusCmd{ WORD  , PMBUS_WR, "STATUS_WORD            ",  0 };
	m_PMBusCmd[STATUS_VOUT           ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_VOUT            ",  0 };
	m_PMBusCmd[STATUS_IOUT           ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_IOUT            ",  0 };
	m_PMBusCmd[STATUS_INPUT          ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_INPUT           ",  0 };
	m_PMBusCmd[STATUS_TEMPERATURE    ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_TEMPERATURE     ",  0 };
	m_PMBusCmd[STATUS_CML            ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_CML             ",  0 };
	m_PMBusCmd[STATUS_MFR_SPECIFIC   ] = stPMBusCmd{ BYTE  , PMBUS_R , "STATUS_MFR_SPECIFIC    ",  0 };
	m_PMBusCmd[READ_VIN              ] = stPMBusCmd{ L11   , PMBUS_R , "READ_VIN               ",  0 };
	m_PMBusCmd[READ_IIN              ] = stPMBusCmd{ L11   , PMBUS_R , "READ_IIN               ",  0 };
	m_PMBusCmd[READ_VOUT             ] = stPMBusCmd{ L16   , PMBUS_R , "READ_VOUT              ",  0 };
	m_PMBusCmd[READ_IOUT             ] = stPMBusCmd{ L11   , PMBUS_R , "READ_IOUT              ",  0 };
	m_PMBusCmd[READ_TEMPERATURE      ] = stPMBusCmd{ L11   , PMBUS_R , "READ_TEMPERATURE       ",  0 };
	m_PMBusCmd[READ_DUTY_CYCLE       ] = stPMBusCmd{ L11   , PMBUS_R , "READ_DUTY_CYCLE        ",  0 };
	m_PMBusCmd[READ_POUT             ] = stPMBusCmd{ L11   , PMBUS_R , "READ_POUT              ",  0 };
	m_PMBusCmd[READ_PIN              ] = stPMBusCmd{ L11   , PMBUS_R , "READ_PIN               ",  0 };
	m_PMBusCmd[PMBUS_REVISION        ] = stPMBusCmd{ BYTE  , PMBUS_R , "PMBUS_REVISION         ",  0 };
	m_PMBusCmd[MFR_SERIAL            ] = stPMBusCmd{ BLOCKS, PMBUS_R , "MFR_SERIAL             ",  0 };
	m_PMBusCmd[IC_DEVICE_ID          ] = stPMBusCmd{ BLOCKS, PMBUS_R , "IC_DEVICE_ID           ",  0 };
	//m_PMBusCmd[MFR_BASE_ADDRESS      ] = stPMBusCmd{ BYTE, "MFR_BASE_ADDRESS       "};
	//m_PMBusCmd[MFR_BASE_ADDRESS_2    ] = stPMBusCmd{ BYTE, "MFR_BASE_ADDRESS_2     "};
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Hidden Register - 20221115
	////////////////////////////////////////////////////////////////////////////////////////////////////
	m_PMBusCmd[IOUT_OCP_GAIN         ] = stPMBusCmd{ WORD  , PMBUS_R , "IOUT_OCP_GAIN          ",  0 };
	m_PMBusCmd[IOUT_OCP_OFFSET       ] = stPMBusCmd{ BYTE  , PMBUS_WR, "IOUT_OCP_OFFSET        ",  0 };
	m_PMBusCmd[VDS_OFFSET            ] = stPMBusCmd{ BYTE  , PMBUS_WR, "VDS_OFFSET             ",  0 };
	m_PMBusCmd[VDS_GAIN              ] = stPMBusCmd{ WORD  , PMBUS_R , "VDS_GAIN               ",  0 };
	m_PMBusCmd[KT1                   ] = stPMBusCmd{ BYTE  , PMBUS_R , "KT1                    ",  0 };
	m_PMBusCmd[KT2                   ] = stPMBusCmd{ BYTE  , PMBUS_R , "KT2                    ",  0 };
	m_PMBusCmd[KVGS1                 ] = stPMBusCmd{ BYTE  , PMBUS_R , "KVGS1                  ",  0 };
	m_PMBusCmd[KVGS2                 ] = stPMBusCmd{ BYTE  , PMBUS_R , "KVGS2                  ",  0 };
	m_PMBusCmd[KVDS                  ] = stPMBusCmd{ BYTE  , PMBUS_R , "KVDS                   ",  0 };
	m_PMBusCmd[IOUT_CAL_GM           ] = stPMBusCmd{ WORD  , PMBUS_R , "IOUT_CAL_GM            ",  0 };
}

std::string CPMBus::GetPMBusCommandStr(PMBUS_CMD cmd )
{
	std::map<PMBUS_CMD,stPMBusCmd>::iterator it;
	it = m_PMBusCmd.find( cmd );
	if( it == m_PMBusCmd.end() )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
		return std::string("UNKNOWN");
	}
	return it->second.name;
}

int CPMBus::GetPMBusCommandExp(PMBUS_CMD cmd)
{
	std::map<PMBUS_CMD,stPMBusCmd>::iterator it;
	it = m_PMBusCmd.find( cmd );
	if( it == m_PMBusCmd.end() )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
		return EXP_INVALID;
	}
	if( it->second.wr == PMBUS_R )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is read-only command!\n", cmd );
		return EXP_INVALID;
	}
	return it->second.exponent;
}

int CPMBus::GetPMBusAddress(int ch)
{
	switch (ch)
	{
		case 0:	 return CH1_ADDRESS; break;
		case 1:	 return CH2_ADDRESS; break;
		case 2:	 return CH3_ADDRESS; break;
		case 3:	 return CH4_ADDRESS; break;
		case 4:	 return CH5_ADDRESS; break;
		case 5:	 return CH6_ADDRESS; break;
		case 6:	 return CH7_ADDRESS; break;
		case 7:	 return CH8_ADDRESS; break;
		default: return CH1_ADDRESS; break;
	}
}

double CPMBus::L11ToDouble(ushort L11)
{
	// extract exponent as MS 5 bits
	char exponent = L11 >> 11;
	// extract mantissa as LS 11 bits
	ushort mantissa = L11 & 0x7ff;
	// sign extend exponent from 5 to 8 bits
	if (exponent > 0x0F) exponent |= 0xE0;
	// sign extend mantissa from 11 to 16 bits
	//if (mantissa > 0x03FF) mantissa |= 0xF800;

	// compute value as mantissa * 2^(exponent)
	return mantissa * pow(2, exponent);
}

double CPMBus::L16ToDouble(ushort L16)
{
	char  exponent = EXP_BITS;
	ushort mantissa = L16;
	// sign extend exponent
	if( exponent > 0x0F ) exponent |= 0xE0;
	//UL16 - unsigned mantissa
	//if( mantissa > 0x03FF ) mantissa |= 0xF800;
	// compute value as mantissa * 2^(exponent)
	return mantissa * pow(2,exponent);
}

ushort CPMBus::DoubleToL11(PMBUS_CMD cmd, double dL11 )
{
	int exponent = GetPMBusCommandExp(cmd);
	if( exponent == EXP_INVALID )
	{
		fprintf( stderr, "CMD(0x%04X) is not L11/L16 Writable Command!\n", cmd );
		return 0;
	}

	int mantissa = static_cast<int>( dL11 / pow(2.0, exponent) );
	// Format the exponent of the L11
	ushort uExponent = (exponent << 11);

	// Format the mantissa of the L11
	ushort uMantissa = mantissa & 0x07FF;
	// Compute value as exponent | mantissa
	return uExponent | uMantissa;
}

ushort CPMBus::DoubleToL16( PMBUS_CMD cmd, double dL16 )
{
	//LINEAR16 format for VOUT related commands
	//[4 : 0] 1,0111: five bit two is complement exponent equals -9 for VOUT related commands
	int L16_Length = GetPMBusCommandExp(cmd);
	if( L16_Length == EXP_INVALID )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is not L11/L16 Writable Command!\n", cmd );
		return 0;
	}

	// set exponent to 2^L16_Length
	float Exponent = pow(2.0, L16_Length);
	// convert value to uint16 and return
	return static_cast<ushort>(dL16 / Exponent);
}

uchar CPMBus::GenerateCRC8( uchar data[], uchar bytes )
{
	uchar crc     = 0x0;
	uchar byteCtr = 0x0;
	for( byteCtr = 0; byteCtr < bytes; ++byteCtr )
	{
		crc ^= (data[byteCtr]);
		for (uchar bit = 8; bit > 0; --bit)
		{
			if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
			else crc = (crc << 1);
		}
	}
	return crc;
}

uchar CPMBus::CheckCRC8(uchar data[], uchar bytes, int checksum )
{
	uchar crc     = 0x0;
	uchar byteCtr = 0x0;
	for( byteCtr = 0; byteCtr < bytes; ++byteCtr )
	{
		crc ^= (data[byteCtr]);
		for (uchar bit = 8; bit > 0; --bit)
		{
			if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
			else crc = (crc << 1);
		}
	}
	if (crc != checksum) return CHECKSUM_ERROR;
	else return 0;
}

bool CPMBus::PMBusWrite(int ch, PMBUS_CMD cmd, int d)
{
	std::map<PMBUS_CMD,stPMBusCmd>::iterator it;
	it = m_PMBusCmd.find( cmd );
	if( it == m_PMBusCmd.end() )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
		return false;
	}
	stPMBusCmd stPMBus = it->second;
	if( stPMBus.wr == PMBUS_R )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is Read Only command!\n", cmd );
		return false;
	}
	uchar pecs[5] = { 0x0, };
	uchar crc     = 0x0;
	int saddr     = GetPMBusAddress(ch);
	int data      = d&0xFFFF;
	int nWrite    = (saddr<<24) | (cmd<<16) | data;
	int protocol  = PROTOCOL_SELECT::READ_BYTE;
	switch ( stPMBus.format)
	{
		case NONE:
			protocol = SEND_BYTE;
			pecs[0] = static_cast<uchar>(saddr)<<1;
			pecs[1]  = static_cast<uchar>(cmd);
			crc      = GenerateCRC8( pecs, 2 );
			if( 0 ) fprintf( stdout, "\t%-15s SA:0x%02X, CMD:0x%02X, CRC:0x%02X\n", GetPMBusCommandStr(cmd).c_str(), pecs[0], pecs[1], crc );
			break;
		case BYTE:
			protocol = WRITE_BYTE;
			pecs[0] = static_cast<uchar>(saddr)<<1;
			pecs[1]  = static_cast<uchar>(cmd);
			pecs[2]  = static_cast<uchar>((data>>0)&0xff);
			crc      = GenerateCRC8( pecs, 3 );
			if( 0 ) fprintf( stdout, "\t%-15s SA:0x%02X, CMD:0x%02X, DATA:0x%02X, CRC:0x%02X\n", GetPMBusCommandStr(cmd).c_str(), pecs[0], pecs[1], pecs[2], crc );
			break;
		case WORD:
		case L11:
		case L16:
			protocol = WRITE_WORD;
			pecs[0] = static_cast<uchar>(saddr)<<1;
			pecs[1]  = static_cast<uchar>(cmd);
			pecs[2]  = static_cast<uchar>((data>>0)&0xff);
			pecs[3]  = static_cast<uchar>((data>>8)&0xff);
			crc      = GenerateCRC8( pecs, 4 );
			if( 0 ) fprintf( stdout, "\t%-15s SA:0x%02X, CMD:0x%02X, DATA_L:0x%02X, DATA_H:0x%02X, CRC:0x%02X\n", GetPMBusCommandStr(cmd).c_str(), pecs[0], pecs[1], pecs[2],  pecs[3], crc );
			break;
		case BLOCKS:
			protocol = SMBALERT_MASK_WRITE;
			fprintf( stderr, "%s is not support cmd!", GetPMBusCommandStr(cmd).c_str() );
			break;
	}
	if( m_BaseType == PPS )
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_WRITE_PROTOCOL , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_WRITE_PEC, crc );	//2022.11.07 - KWMOON
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_REQUEST, 0x1 );
		//fprintf( stdout, "%s : W(0x%08X:0x%08X) P(0x%08X:0x%04X)\n",
		//		 __FUNCTION__, BITCHART::PPS_S1PS::_PPS_WRITE_PROTOCOL, nWrite, BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
	}
	else
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_WRITE_PROTOCOL , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_WRITE_PEC, crc );	//2022.11.07 - KWMOON
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_REQUEST, 0x1 );
		//fprintf( stdout, "%s : W(0x%08X:0x%08X) P(0x%08X:0x%04X)\n",
		//		 __FUNCTION__, BITCHART::PPS_S1PS::_S1PS_WRITE_PROTOCOL, nWrite, BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
	}
	g_sys.SysDelay( 1 MS );
	return true;
}

bool CPMBus::PMBusRead (int ch, PMBUS_CMD cmd, int& d )
{
	std::map<PMBUS_CMD,stPMBusCmd>::iterator it;
	it = m_PMBusCmd.find( cmd );
	if( it == m_PMBusCmd.end() )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
		return false;
	}
	stPMBusCmd stPMBus = it->second;
	if( stPMBus.wr == PMBUS_W )
	{
		fprintf( stderr, "PPS CMD(0x%04X) is Write Only command!\n", cmd );
		return false;
	}

	int protocol  = PROTOCOL_SELECT::READ_BYTE;
	switch ( stPMBus.format )
	{
		case NONE:
		{
			fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
			return false;
		}
		break;

		case BYTE:
			protocol = READ_BYTE;
			break;

		case WORD:
		case L11:
		case L16:
			protocol = READ_WORD;
			break;

		case BLOCKS:
			protocol = SMBALERT_MASK_READ;
			break;
	}

	int nWrite = (GetPMBusAddress(ch)<<24) | (cmd<<16);

	if( m_BaseType == PPS )
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_READ_PROTOCOL  , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_REQUEST, 0x1 );
		//fprintf( stdout, "%s : W(0x%08X:0x%08X) P(0x%08X:0x%04X)\n",
		//		 __FUNCTION__, BITCHART::PPS_S1PS::_PPS_WRITE_PROTOCOL, nWrite, BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
	}
	else
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_READ_PROTOCOL  , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_REQUEST, 0x1 );
		//fprintf( stdout, "%s : W(0x%08X:0x%08X) P(0x%08X:0x%04X)\n",
		//		 __FUNCTION__, BITCHART::PPS_S1PS::_S1PS_WRITE_PROTOCOL, nWrite, BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
	}
	//2023.08.03 - KWMOON( 2 MS -> 1 MS )
	g_sys.SysDelay( 1 MS );

	if( m_BaseType == PPS )
	{
		d = g_pcie.read_user( BITCHART::PPS_S1PS::_PPS_READ )&0xFFFF;
		if( stPMBus.format == BYTE ) d = d & 0xFF;
		//fprintf( stdout, "%s(%d) : 0x%08X-0x%04X\n", __FUNCTION__, __LINE__, BITCHART::PPS_S1PS::_PPS_READ, d );
	}
	else
	{
		d = g_pcie.read_user( BITCHART::PPS_S1PS::_S1PS_READ)&0xFFFF;
		if( stPMBus.format == BYTE ) d = d & 0xFF;
		//fprintf( stdout, "%s(%d) : 0x%08X-0x%04X\n", __FUNCTION__, __LINE__, BITCHART::PPS_S1PS::_S1PS_READ, d );
	}
	if( 0 )
	{
		uchar pecs[5]  = { 0x0, };
		uchar crc      = 0x0;
		uchar checksum = 0x0;
		int saddr      = GetPMBusAddress(ch);
		switch ( stPMBus.format )
		{
			case NONE:
			{
				fprintf( stderr, "PPS CMD(0x%04X) is invalid command!\n", cmd );
				return false;
			}
			break;

			case BYTE:
				pecs[0] = saddr;
				pecs[1] = cmd;
				pecs[2] = (d&0xff);
				pecs[3] = crc;
				checksum = CheckCRC8( pecs, 4, 0x0 );
				break;

			case WORD:
			case L11:
			case L16:
				pecs[0] = saddr;
				pecs[1] = cmd;
				pecs[2] = (d&0xff);
				pecs[3] = ((d>>8)&0xff);
				pecs[4] = crc;
				checksum = CheckCRC8( pecs, 5, 0x0 );
				break;

			case BLOCKS:
				break;
		}

		if( 0 )
		{
			if( checksum == CHECKSUM_ERROR )
				fprintf( stderr, "%s PEC : NG", GetPMBusCommandStr(cmd).c_str() );
			else
				fprintf( stdout, "%s PEC : OK", GetPMBusCommandStr(cmd).c_str() );
		}
	}
	return true;
}

bool CPMBus::PMBusAlertMaskWrite( uint ch, PMBUS_CMD xcmd, int mask )
{
	uchar pecs[5] = { 0x0, };
	uchar crc     = 0x0;
	int protocol  = PROTOCOL_SELECT::SMBALERT_MASK_WRITE;
	int saddr     = GetPMBusAddress(ch);
	int data      = mask & 0xff;
	pecs[0]  = static_cast<uchar>(saddr)<<1;
	pecs[1]  = static_cast<uchar>(SMBALERT_MASK);
	pecs[2]  = static_cast<uchar>(xcmd);
	pecs[3]  = static_cast<uchar>(data);
	crc      = GenerateCRC8( pecs, 4 );
	int nWrite = (GetPMBusAddress(ch)<<24) | (SMBALERT_MASK<<16) | (xcmd<<8) | (data&0xff);
	if( m_BaseType == PPS )
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_SMBALERT_MASK_W , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_WRITE_PEC, crc );	//2022.11.07 - KWMOON
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_REQUEST, 0x1 );
	}
	else
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_SMBALERT_MASK_W , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_WRITE_PEC, crc );	//2022.11.07 - KWMOON
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_REQUEST, 0x1 );
	}
	g_sys.SysDelay( 1 MS );
	return true;
}

bool CPMBus::PMBusAlertMaskRead ( uint ch, PMBUS_CMD xcmd, int& mask )
{
	int protocol  = PROTOCOL_SELECT::SMBALERT_MASK_READ;
	int blockcount= 0x1;
	int nWrite = (GetPMBusAddress(ch)<<24) | (SMBALERT_MASK<<16) | (blockcount<<8) | (xcmd&0xff);
	if( m_BaseType == PPS )
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_SMBALERT_MASK_R , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_REQUEST, 0x1 );
	}
	else
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_SMBALERT_MASK_R , nWrite );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_PROTOCOL_SELECT, protocol );
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_REQUEST, 0x1 );
	}
	g_sys.SysDelay( 1 MS );

	if( m_BaseType == PPS )
	{
		mask = g_pcie.read_user( BITCHART::PPS_S1PS::_PPS_READ )&0xFF;
	}
	else
	{
		mask = g_pcie.read_user( BITCHART::PPS_S1PS::_S1PS_READ)&0xFF;
	}
	return true;
}


void CPMBus::SetEnable(bool bEnable)
{
	int nEnable = (bEnable)?0x1:0x0;
	if( m_BaseType == PPS )
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_ENABLE, nEnable );
	}
	else
	{
		g_pcie.write_user( BITCHART::PPS_S1PS::_S1PS_ENABLE, nEnable );
	}
	//2023.08.03 - KWMOON( 250 US -> 50 US )
	g_sys.SysDelay( 50 US );
}

int CPMBus::GetAlert()
{
	int nAlert = 0x0;
	if( m_BaseType == PPS )
	{
		nAlert = g_pcie.read_user( BITCHART::PPS_S1PS::_PPS_ALERT ) & 0x1;
		//fprintf( stdout, "PPS ALERT(0x%08X) : %d\n", BITCHART::PPS_S1PS::_PPS_ALERT, nAlert );
	}
	else
	{
		nAlert = g_pcie.read_user( BITCHART::PPS_S1PS::_S1PS_ALERT ) & 0x1;
		//fprintf( stdout, "PPS ALERT(0x%08X) : %d\n", BITCHART::PPS_S1PS::_S1PS_ALERT, nAlert );
	}
	return nAlert;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PPS MAIN
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPps::CPps():pps(PPS_UNIT::PPS), s1ps(PPS_UNIT::S1PS)
{
	double vMax = 0.0, vMin = 0.0;
	for( uint unit=0;unit<MAX_PPS_TYPE;unit++ )
	{
			 if( unit == PPS_UNIT::PPS  ) { vMin = PPS_MIN_VOUT ; vMax = PPS_MAX_VOUT;  }
		else if( unit == PPS_UNIT::S1PS ) { vMin = S1PS_MIN_VOUT; vMax = S1PS_MAX_VOUT; }
	}

	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		m_bVsOn[ PPS][ch] = false;
		m_bVsOn[S1PS][ch] = false;
		m_dVsVoltage[ PPS][ch] = 0.0;
		m_dVsVoltage[S1PS][ch] = 0.0;
		m_bVoutRangeSet[ PPS][ch] = false;
		m_bVoutRangeSet[S1PS][ch] = false;
		m_bValidVs[PPS ][ch] = false;		//2022.12.26 - kwmoon
		m_bValidVs[S1PS][ch] = false;		//2022.12.26 - kwmoon
	}

	//2022.12.03 - kwmoon
	m_dS1psAttGain[0] = S1PS_ATT_GAIN0;	m_dS1psAttOffset[0] = S1PS_ATT_OFFSET0;
	m_dS1psAttGain[1] = S1PS_ATT_GAIN1;	m_dS1psAttOffset[1] = S1PS_ATT_OFFSET1;
	m_dS1psAttGain[2] = S1PS_ATT_GAIN2;	m_dS1psAttOffset[2] = S1PS_ATT_OFFSET2;
	m_dS1psAttGain[3] = S1PS_ATT_GAIN3;	m_dS1psAttOffset[3] = S1PS_ATT_OFFSET3;

	//2022.12.05 - kwmoon
//	m_dVoutRange[PPS][V1_0 ][0]   = 0.40;	m_dVoutRange[PPS][V1_0 ][1]   = 1.80;
	m_dVoutRange[PPS][V1_0 ][0]   = 0.30;	m_dVoutRange[PPS][V1_0 ][1]   = 1.80;	//2023-01-30 fatric
	m_dVoutRange[PPS][V0_5 ][0]   = 0.60;	m_dVoutRange[PPS][V0_5 ][1]   = 3.60;
	m_dVoutRange[PPS][V0_25][0]   = 1.20;	m_dVoutRange[PPS][V0_25][1]   = 5.00;

//	m_dVoutRange[S1PS][V1_0  ][0] = 0.38;	m_dVoutRange[S1PS][V1_0  ][1] = 2.00;	//2023-01-30 fatric
	m_dVoutRange[S1PS][V1_0  ][0] = 0.30;	m_dVoutRange[S1PS][V1_0  ][1] = 1.80;	//2023-02-27 - KWMOON
	m_dVoutRange[S1PS][V0_5  ][0] = 1.20;	m_dVoutRange[S1PS][V0_5  ][1] = 3.80;
	m_dVoutRange[S1PS][V0_25 ][0] = 3.00;	m_dVoutRange[S1PS][V0_25 ][1] = 7.50;
	m_dVoutRange[S1PS][V0_125][0] = 7.00;	m_dVoutRange[S1PS][V0_125][1] =16.00;

	//2022.12.06 - kwmoon
	VOUT_SCALE_LOOP_FACTOR[V1_0  ] = L11_V1_0;
	VOUT_SCALE_LOOP_FACTOR[V0_5  ] = L11_V0_5;
	VOUT_SCALE_LOOP_FACTOR[V0_25 ] = L11_V0_25;
	VOUT_SCALE_LOOP_FACTOR[V0_125] = L11_V0_125;

	m_nLvMode = 0x1;
}

//2022.12.06 - kwmoon
uint CPps::GetVoutScaleLoopIndex( uint factor)
{
	switch ( factor )
	{
		case L11_V1_0:
			return V1_0;
		case L11_V0_5:
			return V0_5;
		case L11_V0_25:
			return V0_25;
		case L11_V0_125:
			return V0_125;
		default:
			return V1_0;
			break;
	}
}

ushort CPps::DtoL11( PMBUS_CMD cmd, double value )
{
	ushort d = pps.DoubleToL11( cmd, value );
	if( d == EXP_INVALID ) return 0;
	return d;
}

double CPps::L11toD( ushort value )
{
	double d = pps.L11ToDouble( value );
	return d;
}

ushort CPps::DtoL16( PMBUS_CMD cmd, double value )
{
	ushort d = pps.DoubleToL16( cmd, value );
	if( d == EXP_INVALID ) return 0;
	return d;
}

double CPps::L16toD( ushort value )
{
	double d = pps.L16ToDouble( value );
	return d;
}

std::string CPps::GetPMBusCommandStr(PMBUS_CMD cmd)
{
	return pps.GetPMBusCommandStr( cmd );
}

bool CPps::PpsInit()
{
	SetDgsControl  ( PPS , 0 );	//Default Status Hw.
	SetDgsControl  ( S1PS, 0 );	//Default Status Hw.
	SetSenseControl( PPS , 1 );	//Default Status Hw.
	SetSenseControl( S1PS, 1 );	//Default Status Hw.

	SetLocalRemoteSensing();
	//PPS
	//fprintf( stdout, "PPS INIT\n" );

	PpsInitAlertMask();
	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		pps.PMBusWrite( ch, OPERATION              , OPERATION_OFF            );
		pps.PMBusWrite( ch, FREQUENCY_SWITCH       , pps.DoubleToL11( FREQUENCY_SWITCH   ,   600  ) );	//600 KHZ
		pps.PMBusWrite( ch, VOUT_OV_FAULT_RESPONSE , VOUT_OV_FAULT_RESPONSE_V );
		pps.PMBusWrite( ch, VOUT_UV_FAULT_RESPONSE , VOUT_UV_FAULT_RESPONSE_V );
		pps.PMBusWrite( ch, IOUT_OC_FAULT_RESPONSE , IOUT_OC_FAULT_RESPONSE_V );
		pps.PMBusWrite( ch, OT_FAULT_RESPONSE      , OT_FAULT_RESPONSE_V      );
		pps.PMBusWrite( ch, VIN_OV_FAULT_RESPONSE  , VIN_OV_FAULT_RESPONSE_V  );
		pps.PMBusWrite( ch, POWER_GOOD_ON          , pps.DoubleToL16( POWER_GOOD_ON      , PG_ON_VOLTAGE  ) );
		pps.PMBusWrite( ch, POWER_GOOD_OFF         , pps.DoubleToL16( POWER_GOOD_OFF     , PG_OFF_VOLTAGE ) );
		pps.PMBusWrite( ch, TON_RISE               , pps.DoubleToL11( TON_RISE           , TON_RISE_TIME  ) );
		pps.PMBusWrite( ch, TON_MAX_FAULT_LIMIT    , pps.DoubleToL11( TON_MAX_FAULT_LIMIT, TON_MAX_TIME   ) );
		pps.PMBusWrite( ch, TOFF_FALL              , pps.DoubleToL11( TOFF_FALL          , TOFF_FALL_TIME ) );
		pps.PMBusWrite( ch, TOFF_MAX_WARN_LIMIT    , pps.DoubleToL11( TOFF_MAX_WARN_LIMIT, TOFF_MAX_TIME  ) );
		pps.PMBusWrite( ch, VOUT_MAX               , pps.DoubleToL16( VOUT_MAX           , PMBUS_MAX_VOUT ) );
		pps.PMBusWrite( ch, TON_MAX_FAULT_RESPONSE , TON_MAX_FAULT_RESPONSE_V );
		pps.PMBusWrite( ch, VOUT_TRANSITION_RATE   , pps.DoubleToL11( VOUT_TRANSITION_RATE , 1.0  ) );		//2022.12.08 - Add
		pps.PMBusWrite( ch, IIN_OC_WARN_LIMIT      , pps.DoubleToL11( IIN_OC_WARN_LIMIT    , 15.0 ) );		//2023.08.15 - H/W Inform
		pps.PMBusWrite( ch, CLEAR_FAULTS, 0x0 );
		g_sys.SysDelay( 3 MS );
	}

	//S1PS
	//fprintf( stdout, "S1PS INIT\n" );
	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		s1ps.PMBusWrite( ch, OPERATION              , OPERATION_OFF             );
		s1ps.PMBusWrite( ch, FREQUENCY_SWITCH       , s1ps.DoubleToL11( FREQUENCY_SWITCH     ,   600  ) );	//600 KHZ
		s1ps.PMBusWrite( ch, VOUT_OV_FAULT_RESPONSE , VOUT_OV_FAULT_RESPONSE_V  );
		s1ps.PMBusWrite( ch, VOUT_UV_FAULT_RESPONSE , VOUT_UV_FAULT_RESPONSE_V  );
		s1ps.PMBusWrite( ch, IOUT_OC_FAULT_RESPONSE , IOUT_OC_FAULT_RESPONSE_V  );
		s1ps.PMBusWrite( ch, OT_FAULT_RESPONSE      , OT_FAULT_RESPONSE_V       );
		s1ps.PMBusWrite( ch, VIN_OV_FAULT_RESPONSE  , VIN_OV_FAULT_RESPONSE_V   );
		s1ps.PMBusWrite( ch, VIN_OV_FAULT_LIMIT     , s1ps.DoubleToL11( VIN_OV_FAULT_LIMIT   , S1PS_VIN_OV_FAULT ) );
		s1ps.PMBusWrite( ch, VIN_UV_WARN_LIMIT      , s1ps.DoubleToL11( VIN_UV_WARN_LIMIT    , S1PS_VIN_UV_WARN  ) );
		s1ps.PMBusWrite( ch, POWER_GOOD_ON          , s1ps.DoubleToL16( POWER_GOOD_ON        , PG_ON_VOLTAGE     ) );
		s1ps.PMBusWrite( ch, POWER_GOOD_OFF         , s1ps.DoubleToL16( POWER_GOOD_OFF       , PG_OFF_VOLTAGE    ) );
		s1ps.PMBusWrite( ch, TON_RISE               , s1ps.DoubleToL11( TON_RISE             , TON_RISE_TIME     ) );
		s1ps.PMBusWrite( ch, TON_MAX_FAULT_LIMIT    , s1ps.DoubleToL11( TON_MAX_FAULT_LIMIT  , TON_MAX_TIME      ) );
		s1ps.PMBusWrite( ch, TOFF_FALL              , s1ps.DoubleToL11( TOFF_FALL            , TOFF_FALL_TIME    ) );
		s1ps.PMBusWrite( ch, TOFF_MAX_WARN_LIMIT    , s1ps.DoubleToL11( TOFF_MAX_WARN_LIMIT  , TOFF_MAX_TIME     ) );
		s1ps.PMBusWrite( ch, VOUT_MAX               , s1ps.DoubleToL16( VOUT_MAX             , PMBUS_MAX_VOUT    ) );
		s1ps.PMBusWrite( ch, VOUT_TRANSITION_RATE   , s1ps.DoubleToL11( VOUT_TRANSITION_RATE , 0.1 ) );		//2024.06.24 - KWMOON( 1.0 -> 0.1 )
		s1ps.PMBusWrite( ch, IIN_OC_WARN_LIMIT      , s1ps.DoubleToL11( IIN_OC_WARN_LIMIT    , 15.0 ) );	//2023.08.15 - H/W Inform
		s1ps.PMBusWrite( ch, TON_MAX_FAULT_RESPONSE , TON_MAX_FAULT_RESPONSE_V  );
		s1ps.PMBusWrite( ch, VDS_OFFSET             , S1PS_VDS_OFFSET  );	//2023.02.27 - KWMOON
		s1ps.PMBusWrite( ch, IOUT_OCP_OFFSET        , S1PS_IOUT_OCP_OFFSET  );	//2023.02.27 - KWMOON
		//VDS OFFSET READ : 2022.12.03 - kwmoon(only s1ps)
		//int vds = 0x0;
		//if( s1ps.PMBusRead( ch, VDS_OFFSET, vds ) )
		//{
		//	m_VdsOffset[S1PS][ch] = vds&0xff;
		//	if( 0 ) g_log.Printf( "%s S1PS CH%02d VDS_OFFSET:0x%02X", __FUNCTION__, ch, m_VdsOffset[S1PS][ch]&0xFF );
		//}
		//else
		//{
		//	m_VdsOffset[S1PS][ch] = vds;
		//	if( 0 ) g_log.Printf( "%s S1PS CH%02d VDS_OFFSET read error!", __FUNCTION__, ch );
		//}
		s1ps.PMBusWrite( ch, CLEAR_FAULTS, 0x0 );
		g_sys.SysDelay( 3 MS );
	}

	//All enable
	pps.SetEnable ( false );
	s1ps.SetEnable( false );

	return true;
}

//2022.12.09 - kwmoon
bool CPps::PpsInitAlertMask()
{
	m_vecAlertMask.push_back( stAlertMask {STATUS_VOUT, 0x2} );

	int i =0;
	for( stAlertMask alertmask : m_vecAlertMask )
	{
		//g_log.Printf("[%02d] SMBALERT_MASK CMD:%-20s BYTE:0x%02X",
		//			 i++, pps.GetPMBusCommandStr(alertmask.cmd).c_str(), alertmask.mask_byte );

		for( int ch=0;ch<MAX_PPS_CH;ch++ )
		{
			pps.PMBusAlertMaskWrite ( ch, alertmask.cmd, alertmask.mask_byte );
			s1ps.PMBusAlertMaskWrite( ch, alertmask.cmd, alertmask.mask_byte );			
		}
		//2023.08.03 - KWMOON( 10 MS -> 5 MS )
		g_sys.SysDelay( 5 MS );
	}
	return true;
}

bool CPps::IsValidSetVoltage( PPS_UNIT t, uint ch, double v, double oc )
{
	double dVoutMin  = 0.0;
	double dVoutMax  = 0.0;
	double dIoutMin  = 0.0;
	double dIoutMax  = 0.0;
	ushort scaleloop = 0x0;
	std::string s = "NORMAL";

	if( t == PPS_UNIT::PPS )
	{
		scaleloop = m_VoutScaleLoop[PPS][ch];
		if( m_bVsOn[PPS][ch] || IsSetVoutRange(PPS,ch) )	//vout range set
		{
			uint idx = GetVoutScaleLoopIndex( m_VoutScaleLoop[PPS][ch] );
			dVoutMin = m_dVoutRange[PPS][idx][0];
			dVoutMax = m_dVoutRange[PPS][idx][1];
			s = "BUMP";
		}
		else
		{
			dVoutMin = PPS_MIN_VOUT;
			dVoutMax = PPS_MAX_VOUT;
		}
		dIoutMin = PPS_MIN_IOUT;
		dIoutMax = PPS_MAX_IOUT;
	}
	else
	{
		scaleloop = m_VoutScaleLoop[S1PS][ch];
		if( m_bVsOn[S1PS][ch] || IsSetVoutRange(S1PS,ch) )	//vout range set
		{
			uint idx = GetVoutScaleLoopIndex( m_VoutScaleLoop[S1PS][ch] );
			dVoutMin = m_dVoutRange[S1PS][idx][0];
			dVoutMax = m_dVoutRange[S1PS][idx][1];
			s = "BUMP";
		}
		else
		{
			dVoutMin = S1PS_MIN_VOUT;
			dVoutMax = S1PS_MAX_VOUT;
		}
		dIoutMin = S1PS_MIN_IOUT;
		dIoutMax = S1PS_MAX_IOUT;
	}
	//if( v < dVoutMin ) v = dVoutMin;		//2022.12.06 - kwmoon
	if( (v < dVoutMin) || (v > dVoutMax) )
	{
		m_bValidVs[t][ch] = false;		//2022.12.26 - kwmoon
		g_log.EPrintf( _PPS, "SET_VS", "%s %s set(%5.2f V) error : scale[0x%04X] Min:%5.2f V ~ Max:%5.2f V",
					   (t==PPS)?"PPS ":"S1PS", s.c_str(), v, scaleloop, dVoutMin, dVoutMax );
		return false;
	}
	if( (oc < dIoutMin) || (oc > dIoutMax) )
	{
		m_bValidVs[t][ch] = false;		//2022.12.26 - kwmoon
		g_log.EPrintf( _PPS, "SET_VS", "%s %s oc(%5.2f A) error : scale[0x%04X] Min:%5.2f A ~ Max:%5.2f A",
					   (t==PPS)?"PPS ":"S1PS", s.c_str(), oc, scaleloop, dIoutMin, dIoutMax );
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPps::IsValidOverVoltage
/// \param unit
/// \param ch
/// \param rOv
/// \return
/// \date 2022.12.09
/// \details ScaleLoop 1.0   , OV Fault MAX 1.99
/// \details ScaleLoop 0.5   , OV Fault MAX 3.99
/// \details ScaleLoop 0.125 , OV Fault MAX 7.99
bool CPps::GetValidOverVoltage( PPS_UNIT unit, uint ch, double& rOv )
{
	if( !IsSetVoutRange(unit,ch) )
	{
		g_log.EPrintf( _PPS, "SET_VS_ON", "%s %s CH:%02d IsSetVoutRange error!",
					   __FUNCTION__, (unit==PPS)?" PPS":"S1PS", ch );
		m_bValidVs[unit][ch] = false;
		return false;
	}

	switch ( m_VoutScaleLoop[unit][ch] )
	{
		case L11_V1_0	:
			//printf("FACTOR 1.0 OV CHANGE: %5.2f -> %5.2f", rOv, MAX_OV_SCALE_FACTOR1 );
			if( rOv > MAX_OV_SCALE_FACTOR1    ) rOv = MAX_OV_SCALE_FACTOR1;
			break;
		case L11_V0_5	:
			if( rOv > MAX_OV_SCALE_FACTOR0_5  ) rOv = MAX_OV_SCALE_FACTOR0_5;
			break;
		case L11_V0_25	:
			if( rOv > MAX_OV_SCALE_FACTOR0_25 ) rOv = MAX_OV_SCALE_FACTOR0_25;
			break;
		case L11_V0_125 :
			break;
	}
	return true;
}

bool CPps::IsSetVoutRange( PPS_UNIT unit, uint ch )
{
	if( !m_bVoutRangeSet[unit][ch] )
	{
		return false;
	}
	return true;
}

//2022.12.05 - kwmoon
void CPps::PrintVoutScaleLoop(uint unit)
{
	switch( unit )
	{
		case PPS:
		{
			g_log.Printf("[PPS VOUT RANGE]");
			g_log.Printf(" 1. %5.2f V ~ %5.2f V ---- E808h/1.0 ", m_dVoutRange[PPS][0][0], m_dVoutRange[PPS][0][1]);
			g_log.Printf(" 2. %5.2f V ~ %5.2f V ---- E804h/0.5 ", m_dVoutRange[PPS][1][0], m_dVoutRange[PPS][1][1]);
			g_log.Printf(" 3. %5.2f V ~ %5.2f V ---- E802h/0.25", m_dVoutRange[PPS][2][0], m_dVoutRange[PPS][2][1]);
		}
		break;

		case S1PS:
		{
			g_log.Printf("[S1PS VOUT RANGE]");
			g_log.Printf(" 1. %5.2f V ~ %5.2f V ---- E808h/1.0 ", m_dVoutRange[S1PS][0][0], m_dVoutRange[S1PS][0][1]);
			g_log.Printf(" 2. %5.2f V ~ %5.2f V ---- E804h/0.5 ", m_dVoutRange[S1PS][1][0], m_dVoutRange[S1PS][1][1]);
			g_log.Printf(" 3. %5.2f V ~ %5.2f V ---- E802h/0.25", m_dVoutRange[S1PS][2][0], m_dVoutRange[S1PS][2][1]);
			g_log.Printf(" 4. %5.2f V ~ %5.2f V ---- E801h/0.25", m_dVoutRange[S1PS][3][0], m_dVoutRange[S1PS][3][1]);
		}
		break;
	}
}

//2022.12.05 - kwmoon
bool CPps::GetVoutScaleLoop(uint unit, double set, ushort& rScaleIdx)
{
	switch( unit )
	{
		case PPS:
		{
				 if( (set >= m_dVoutRange[PPS][V0_25][0]) && ( set <= m_dVoutRange[PPS][V0_25][1] ) ) rScaleIdx = V0_25;	//E802h
			else if( (set >= m_dVoutRange[PPS][V0_5 ][0]) && ( set <= m_dVoutRange[PPS][V0_5 ][1] ) ) rScaleIdx = V0_5;		//E804h
			else if( (set >= m_dVoutRange[PPS][V1_0 ][0]) && ( set <= m_dVoutRange[PPS][V1_0 ][1] ) ) rScaleIdx = V1_0;		//E808h
			else
			{
				//if( (set<PPS_MIN_VOUT) || (set>PPS_MAX_VOUT) )
				//	g_log.Printf( "PPS VOUT_SCALE_LOOP error set(%5.2f V), use %5.2f V ~ %5.2f V", set, PPS_MIN_VOUT, PPS_MAX_VOUT );
				rScaleIdx = V0_25;	//default
				return false;
			}			return true;
		}
		break;

		case S1PS:
		{
				 if( (set >= m_dVoutRange[S1PS][V1_0  ][0]) && ( set <= m_dVoutRange[S1PS][V1_0  ][1] ) ) rScaleIdx = V1_0;	//E808h
			else if( (set >= m_dVoutRange[S1PS][V0_5  ][0]) && ( set <= m_dVoutRange[S1PS][V0_5  ][1] ) ) rScaleIdx = V0_5;	//E804h
			else if( (set >= m_dVoutRange[S1PS][V0_25 ][0]) && ( set <= m_dVoutRange[S1PS][V0_25 ][1] ) ) rScaleIdx = V0_25;	//E802h
			else if( (set >= m_dVoutRange[S1PS][V0_125][0]) && ( set <= m_dVoutRange[S1PS][V0_125][1] ) ) rScaleIdx = V0_125;	//E801h
			else
			{
				//if( (set<S1PS_MIN_VOUT) || (set>S1PS_MAX_VOUT) )
				//g_log.Printf( "S1PS VOUT_SCALE_LOOP error set(%5.2f V), use %5.2f V ~ %5.2f V", set, S1PS_MIN_VOUT, S1PS_MAX_VOUT );
				rScaleIdx = V1_0;	//default
				return false;
			}
			return true;
			//if( 0 ) fprintf( stdout, "\n[S1PS VOUT_SCALE_LOOP MAX:%5.2f V ~ MIN:%5.2f V - 0x%04X]", max, min, rScale );
		}
		break;
	}
	return false;
}

bool CPps::SetVoutScaleLoop(uint unit, uint ch, ushort uscaleloop )
{
	if( m_bVsOn[unit][ch] )
	{
		g_log.EPrintf( _PPS, "SCALE", "%s CH:%d is POWER ON, can't change VOUT_SCALE_LOOP!",
					   (unit==PPS)?" PPS":"S1PS", ch );
		return false;
	}

	switch( unit )
	{
		case PPS:
		{
			if( !pps.PMBusWrite( ch, VOUT_SCALE_LOOP, uscaleloop  ) )
			{
				g_log.EPrintf( _PPS, __FUNCTION__, "\tPPS  CH#%d set VOUT_SCALE_LOOP error!", ch+1 );
				return false;
			}
			m_VoutScaleLoop[PPS][ch] = uscaleloop;
			m_bVoutRangeSet[PPS][ch] = true;
			if( 0 ) g_log.Printf("SET PPS  VOUT_SCALE_LOOP : 0x%04X", uscaleloop );
		}
		break;

		case S1PS:
		{
			if( !s1ps.PMBusWrite( ch, VOUT_SCALE_LOOP, uscaleloop  ) )
			{
				g_log.EPrintf( _PPS, __FUNCTION__, "\tS1PS CH#%d set VOUT_SCALE_LOOP error!", ch+1 );
				return false;
			}
			m_VoutScaleLoop[S1PS][ch] = uscaleloop;
			m_bVoutRangeSet[S1PS][ch] = true;

			if( 0 ) g_log.Printf("SET S1PS  VOUT_SCALE_LOOP : 0x%04X", uscaleloop );
			//2023.02.27 - KWMOON(S1PS SCALE FACTOR 1 ==> S1PS##_LV Low Set)
			if( uscaleloop == L11_V1_0 )
			{
				m_nLvMode &= (~(0x1<<ch)&0xFF);
				//fprintf( stdout, "[0x%04X]LV MODE : 0x%02X\n", uscaleloop, m_nLvMode );
                //g_log.Printf( "[0x%04X]LV MODE  ON : 0x%02X\n", uscaleloop, m_nLvMode );
				g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_VOLTAGE_LV, m_nLvMode );
			}
			else
			{
				m_nLvMode |= (0x1<<ch);
				//fprintf( stdout, "[0x%04X]LV MODE : 0x%02X\n", uscaleloop, m_nLvMode );
                //g_log.Printf( "[0x%04X]LV MODE ATT : 0x%02X\n", uscaleloop, m_nLvMode );
				g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_VOLTAGE_LV, m_nLvMode );
			}
		}
		break;

		default:
			g_log.EPrintf( _PPS, "SCALE", "PPS VOUT_SCALE_LOOP Unit type '%d' is invalid, use 0/1", unit );
			return false;
			break;
	}
	return true;
}

bool CPps::GetS1psAttVoltage(uint ch, double v, double& rv)
{
	rv = 0.0;

	switch ( m_VoutScaleLoop[S1PS][ch] )
	{
		case L11_V1_0:
			rv = ( v - m_dS1psAttOffset[V1_0  ] ) / m_dS1psAttGain[V1_0  ];
			break;
		case L11_V0_5:
			rv = ( v  - m_dS1psAttOffset[V0_5  ]) / m_dS1psAttGain[V0_5  ];
			break;
		case L11_V0_25:
			rv = ( v - m_dS1psAttOffset[V0_25 ] ) / m_dS1psAttGain[V0_25 ];
			break;
		case L11_V0_125:
			rv = ( v - m_dS1psAttOffset[V0_125] ) / m_dS1psAttGain[V0_125];
			break;
		default:
			g_log.EPrintf( _PPS, "VOLTAGE", "S1PS VOUT_SCALE_LOOP[0x%04X] value error select one [%04X/%04X/%04X/%04X])",
					 m_VoutScaleLoop[S1PS][ch], L11_V1_0, L11_V0_5, L11_V0_25, L11_V0_125 );
			m_bValidVs[S1PS][ch] = false;		//2022.12.26 - kwmoon
			return false;
			break;
	}
	return true;
}

//2023.02.27 - KWMOON(IOUT_OC_FAULT_LIMIT)
//reference to AT6200_PMBUS.ppt IOUT_OC_FAULT_LIMIT
bool CPps::GetS1psOcFaultLimit( double dVset, double dOcFault, double& rOcFault )
{
	rOcFault = dOcFault;
	if( (dVset>=0.3) && (dVset<=1.0) )
	{
		rOcFault = dOcFault + 0.0;
	}
	else if( (dVset>1.0) && (dVset<=4.0) )
	{
		rOcFault = dOcFault + 0.0;
	}
	else if( (dVset>4.0) && (dVset<=10.0) )
	{
		rOcFault = dOcFault + 0.0;				//2023.03.27 - kwmoon* 0.5 -> 0.0 )
	}
	else if( (dVset>10.0) && (dVset<=15.0) )
	{
		rOcFault = dOcFault + 0.5;				//2023.03.27 - kwmoon* 1.0 -> 0.5 )
	}
	return true;
}

bool CPps::GetS1psFreqSwitch( double v, ushort& uFreq)
{
	//uFreq = DtoL11( FREQUENCY_SWITCH, 450 );	//450 KHZ
	//return true;

		 if( v <= 1.0  ) uFreq = DtoL11( FREQUENCY_SWITCH, 450 );	//450 KHZ
	else if( v <= 13.0 ) uFreq = DtoL11( FREQUENCY_SWITCH, 900 );	//900 KHZ
	else if( v >  13.0 ) uFreq = DtoL11( FREQUENCY_SWITCH, 700 );	//700 KHZ
	else
	{
		g_log.EPrintf( _PPS, "FREQ", "S1PS FREQUENCY_SWITCH set(%5.2f V) is range error!!", v );
		uFreq = DtoL11( FREQUENCY_SWITCH, 900 );	//450 KHZ
		return false;
	}
	return true;
}

//2022.12.05 - kwmoon(s1ps vds offset per vout range )
char CPps::GetS1psVdsOffset( double set )
{
		 if( (set>= 0.1) && (set< 1.0) ) return   4;
	else if( (set>= 1.0) && (set< 2.0) ) return   2;
	else if( (set>= 2.0) && (set< 3.0) ) return  -3;
	else if( (set>= 3.0) && (set< 4.0) ) return  -6;
	else if( (set>= 4.0) && (set< 5.0) ) return  -8;
	else if( (set>= 5.0) && (set< 6.0) ) return -10;
	else if( (set>= 6.0) && (set< 7.0) ) return -12;
	else if( (set>= 7.0) && (set< 8.0) ) return -14;
	else if( (set>= 8.0) && (set< 9.0) ) return -16;
	else if( (set>= 9.0) && (set<10.0) ) return -19;
	else if( (set>=10.0) && (set<11.0) ) return -22;
	else if( (set>=11.0) && (set<12.0) ) return -26;
	else if( (set>=12.0) && (set<13.0) ) return -30;
	else if( (set>=13.0) && (set<14.0) ) return -27;
	else if( (set>=14.0) && (set<15.0) ) return -32;
	else if( (set>=15.0) && (set<16.0) ) return -41;
	return 0;
}

//2022.12.05 - kwmoon
bool CPps::SetVoutRange(uint unit, uint ch, double set)
{
	ushort uScaleIdx  = 0x0;
	ushort uScaleLoop = 0x0;

	if( IsSetVoutRange( static_cast<PPS_UNIT>(unit), ch) ) return true;

	if( !GetVoutScaleLoop( unit, set, uScaleIdx ) )
	{
		m_bValidVs[unit][ch] = false;		//2022.12.26 - kwmoon
		if( 0 ) PrintVoutScaleLoop( unit );
		return false;
	}
	uScaleLoop = VOUT_SCALE_LOOP_FACTOR[uScaleIdx];
	if( !SetVoutScaleLoop( unit, ch, uScaleLoop ) )
	{
		m_bValidVs[unit][ch] = false;		//2022.12.26 - kwmoon
		return false;
	}
	//2023.02.27 - KWMOON(S1PS SCALE FACTOR 1 ==> S1PS##_LV Low Set)
	if( unit == S1PS )
	{
		if( uScaleLoop == L11_V1_0 )
		{
			m_nLvMode &= (~(0x1<<ch)&0xFF);
            //g_log.Printf( "[0x%04X]LV MODE  ON : 0x%02X\n", uScaleLoop, m_nLvMode );
			//fprintf( stdout, "[0x%04X]LV MODE : 0x%02X\n", uscaleloop, m_nLvMode );
			g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_VOLTAGE_LV, m_nLvMode );
		}
		else
		{
			m_nLvMode |= (0x1<<ch);
            //g_log.Printf( "[0x%04X]LV MODE ATT : 0x%02X\n", uScaleLoop, m_nLvMode );
			//fprintf( stdout, "[0x%04X]LV MODE : 0x%02X\n", uscaleloop, m_nLvMode );
			g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_VOLTAGE_LV, m_nLvMode );
		}
	}

	//comment double dVoutMax = m_dVoutRange[unit][uScaleIdx][1];
	//comment if( unit == PPS )
	//comment 	pps.PMBusWrite( ch, VOUT_MAX, DtoL16(VOUT_MAX, dVoutMax*1.3 ) );
	//comment else
	//comment 	s1ps.PMBusWrite( ch, VOUT_MAX, DtoL16(VOUT_MAX, dVoutMax*1.3 ));
	//if( 1 ) g_log.Printf("%s CH:%d VOUT_MAX:%6.2f V SCALE_LOOP[%d]:0x%04X", (unit == PPS)?"PPS ":"S1PS", ch, dVoutMax*1.3, uScaleIdx, uScaleLoop );

//	printf("%s CH:%d uScaleIdx:%d\n", (unit == PPS)?"PPS ":"S1PS", ch, uScaleIdx );
	return true;
}

bool CPps::SetVs(uint unit, uint ch, double v, double ov, double uv, double oc )
{
	double dv  = v;
	double dov = ov;
	double duv = uv;
	double doc = oc;

	//2023.08.03 - KWMOON
	if( g_sys.IsPfmEnable() )
	{
		if( g_sys.IsPfmStart() )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, %6.2f V set skip!\n", (unit==PPS)?" PPS":"S1PS", ch+1, v );
			return false;
		}
	}

	if( uv<0.0)  uv = 0.0;	//2024.01.04 - fatric
	if(duv<0.0) duv = 0.0;	//2022.12.26 - kwmoon

	m_bValidVs[unit][ch] = true;		//2022.12.26 - kwmoon

	//2024.07.25 - KWMOON(BUMP PFM SET UPDATE)
	if( m_bVsOn[unit][ch] )
	{
		bool bBump = SetVBump( unit, ch, dv, dov, duv, doc );
		if( g_sys.IsPfmEnable() )
		{
			if( !g_sys.IsPfmStart() )
			{
				g_sys.SetPfmStart();
			}
		}
		return bBump;
	}

	//normal vs sequence
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusWrite( ch, OPERATION          , OPERATION_OFF                           );

		if( !SetVoutRange( PPS, ch, dv ) ) return false;				//2022.12.06 - kwmoon
		if( !IsValidSetVoltage ( PPS, ch, dv, doc ) ) return false;

		if( !GetValidOverVoltage( PPS, ch, dov ) ) return false;							//2022.12.09 - kwmoon
		pps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );	//dv*1.15 ) );
		pps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) );	//2022.12.08 - kwmoon(same ov)
		pps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.02  ) );	//dv*1.05 ) );
		pps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.98  ) );
		pps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
		g_sys.SysDelay( 1 MS );
		pps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
		pps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , 0.2      ) ); //2022.12.14 - yhsong confirm.
		pps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, doc      ) );
		pps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , doc*1.5  ) ); //2022.12.14 - yhsong confirm.
		m_dVsVoltage[PPS][ch] = v;
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusWrite( ch, OPERATION          , OPERATION_OFF                          );
		ushort uFreq = 0;
		if( !SetVoutRange( S1PS, ch, v )      ) return false;			//2022.12.06 - kwmoon
		if( !IsValidSetVoltage ( S1PS, ch, v, oc  ) ) return false;
		if( !GetS1psAttVoltage (ch,  v, dv   ) ) return false;
		if( !GetS1psAttVoltage (ch, ov, dov  ) ) return false;
		if( !GetS1psAttVoltage (ch, uv, duv  ) ) return false;
		if( !GetS1psFreqSwitch ( v, uFreq) ) return false;
		if( !GetS1psOcFaultLimit ( v, oc, doc) ) return false;			//2023.03.03 - KWMOON

		if( !GetValidOverVoltage( S1PS, ch, dov ) ) return false;							//2022.12.09 - kwmoon
		s1ps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );
		s1ps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) ); //2022.12.08 - kwmoon(same ov)
		s1ps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.02  ) );
		s1ps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.98  ) );
		s1ps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
		g_sys.SysDelay( 1 MS );
		s1ps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
		s1ps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , 0.2      ) ); //2022.12.14 - yhsong confirm.
		s1ps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, doc      ) );
		s1ps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , S1PS_OC_WARN_LIMIT_OFF  ) ); //2022.07.19 - kwmoon( yhsong confirm - oc warn limit set )
		s1ps.PMBusWrite( ch, FREQUENCY_SWITCH   , uFreq                                   );
		m_dVsVoltage[S1PS][ch] = v;
		//2022.12.07 - kwmoon(vds offset mask)
		//char vds_offset = m_VdsOffset[S1PS][ch] + GetS1psVdsOffset(v);
		//if( 1 )
		//{
		//	g_log.Printf( "SET_VS S1PS CH%d SET:%02X[%d] DEFAULT:0x%02X[%d] OFFSET:0x%02X[%d]",
		//			  ch, vds_offset, vds_offset, m_VdsOffset[S1PS][ch], m_VdsOffset[S1PS][ch], GetS1psVdsOffset(v), GetS1psVdsOffset(v) );
		//}
		//s1ps.PMBusWrite( ch, VDS_OFFSET         , vds_offset );	//2022.12.06 - KWMOON
	}
	//2023.08.03 - KWMOON(Comment - 1 MS )
	//g_sys.SysDelay( 1 MS );
	return true;
}

bool CPps::SetVBump( uint unit, uint ch, double v, double ov, double uv, double oc )
{
	double dv  = v;
	double dov = ov;
	double duv = uv;
	double doc = oc;
	if( unit == PPS_UNIT::PPS )
	{
		if( !IsSetVoutRange( PPS, ch ) )
		{
			m_bValidVs[unit][ch] = false;		//2022.12.26 - kwmoon
			g_log.EPrintf( _PPS, "BUMP", "%s CH:%d VOUT_SCALE_LOOP is not set!", (unit==PPS)?"PPS":"S1PS", ch );
			return false;
		}
		if( !IsValidSetVoltage ( PPS, ch, v, doc ) ) return false;

		if( !GetValidOverVoltage( PPS, ch, dov    ) ) return false;							//2022.12.09 - kwmoon
		if( v > m_dVsVoltage[PPS][ch] )	//Set voltage is bigger than current voltage
		{
			//fprintf( stdout, "CURRENT PPS%d Voltage:%6.3f V, Bump Voltage:%6.3F V\n", ch+1, m_dVsVoltage[PPS][ch], v );
			pps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );	//dv*1.15 ) );
			pps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) );	//dv*1.10 ) );
			pps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.02  ) );	//dv*1.05 ) );
			pps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
			g_sys.SysDelay( 50 MS );
			pps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.98  ) );
			pps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , 0.2      ) ); //2022.12.14 - yhsong confirm.
			pps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
			pps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, oc       ) );
			pps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , oc*0.97  ) );
		}
		else		//Set voltage is less than current voltage
		{
			//fprintf( stdout, "CURRENT PPS%d Voltage:%6.3f V, Bump Voltage:%6.3F V\n", ch, m_dVsVoltage[PPS][ch], v );
			pps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , 0.2      ) ); //2022.12.14 - yhsong confirm.
			pps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
			pps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.98  ) );
			pps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
			g_sys.SysDelay( 50 MS );
			pps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.02  ) );	//dv*1.05 ) );
			pps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );	//dv*1.15 ) );
			pps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) );	//dv*1.10 ) );
			pps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, oc       ) );
			pps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , oc*0.97  ) );
		}
		m_dVsVoltage[PPS][ch] = v;
	}
	else	//S1PS
	{
		ushort uFreq = 0;
		if( !IsSetVoutRange( S1PS, ch ) )
		{
			m_bValidVs[unit][ch] = false;		//2022.12.26 - kwmoon
			g_log.EPrintf( _PPS, "BUMP", "%s CH:%d VOUT_SCALE_LOOP is not set!", (unit==PPS)?"PPS":"S1PS", ch );
			return false;
		}
		if( !IsValidSetVoltage ( S1PS, ch, v, oc ) ) return false;
		if( !GetS1psAttVoltage ( ch, v, dv   ) ) return false;
		if( !GetS1psAttVoltage ( ch, ov, dov  ) ) return false;
		if( !GetS1psAttVoltage ( ch, uv, duv  ) ) return false;
		if( !GetS1psFreqSwitch ( v, uFreq) ) return false;
		if( !GetS1psOcFaultLimit( v, oc, doc) ) return false;				//2023.03.03 - KWMOON
		if( !GetValidOverVoltage( S1PS, ch, dov    ) ) return false;		//2022.12.09 - kwmoon

		if( v > m_dVsVoltage[S1PS][ch] )	//Set voltage is bigger than current voltage
		{
			//printf("BUMP UP %6.2f -> %6.2f\n", m_dVsVoltage[S1PS][ch], v );
			s1ps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );
			s1ps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) );	//2022.12.08 - kwmoon(ov warn modify)
			s1ps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.01  ) );
			s1ps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
			g_sys.SysDelay( 50 MS );
			s1ps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.99  ) );
			s1ps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , duv*1.01 ) );
			s1ps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
			s1ps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, doc      ) );
			s1ps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , S1PS_OC_WARN_LIMIT_OFF ) ); //2022.07.19 - kwmoon( yhsong confirm - oc warn limit set )
			s1ps.PMBusWrite( ch, FREQUENCY_SWITCH   , uFreq                                   );
			//2022.12.07 - kwmoon(vds offset mask )
			//char vds_offset = m_VdsOffset[S1PS][ch] + GetS1psVdsOffset(v);
			//if( 1 )
			//{
			//	g_log.Printf( "VBUMP S1PS CH%d SET:%02X[%d] DEFAULT:0x%02X[%d] OFFSET:0x%02X[%d]",
			//			  ch, vds_offset, vds_offset, m_VdsOffset[S1PS][ch], m_VdsOffset[S1PS][ch], GetS1psVdsOffset(v), GetS1psVdsOffset(v) );
			//}
			//s1ps.PMBusWrite( ch, VDS_OFFSET         , vds_offset );	//2022.12.06 - KWMOON
		}
		else		//Set voltage is less than current voltage
		{
			//printf("BUMP DOWN %6.2f -> %6.2f\n", m_dVsVoltage[S1PS][ch], v );
			s1ps.PMBusWrite( ch, VOUT_UV_WARN_LIMIT , DtoL16( VOUT_UV_WARN_LIMIT , duv*1.01 ) );
			s1ps.PMBusWrite( ch, VOUT_UV_FAULT_LIMIT, DtoL16( VOUT_UV_FAULT_LIMIT, duv      ) );
			s1ps.PMBusWrite( ch, VOUT_MARGIN_LOW    , DtoL16( VOUT_MARGIN_LOW    , dv*0.99  ) );
			s1ps.PMBusWrite( ch, VOUT_COMMAND       , DtoL16( VOUT_COMMAND       , dv       ) );
			g_sys.SysDelay( 50 MS );
			s1ps.PMBusWrite( ch, VOUT_MARGIN_HIGH   , DtoL16( VOUT_MARGIN_HIGH   , dv*1.01  ) );
			s1ps.PMBusWrite( ch, VOUT_OV_FAULT_LIMIT, DtoL16( VOUT_OV_FAULT_LIMIT, dov      ) );
			s1ps.PMBusWrite( ch, VOUT_OV_WARN_LIMIT , DtoL16( VOUT_OV_WARN_LIMIT , dov*0.99 ) );	//2022.12.08 - kwmoon(ov warn modify)
			s1ps.PMBusWrite( ch, IOUT_OC_FAULT_LIMIT, DtoL11( IOUT_OC_FAULT_LIMIT, doc      ) );
			s1ps.PMBusWrite( ch, IOUT_OC_WARN_LIMIT , DtoL11( IOUT_OC_WARN_LIMIT , S1PS_OC_WARN_LIMIT_OFF ) );	//2022.07.19 - kwmoon( yhsong confirm - oc warn limit set )
			s1ps.PMBusWrite( ch, FREQUENCY_SWITCH   , uFreq                                   );
		}
		m_dVsVoltage[S1PS][ch] = v;
	}
	//2023.08.03 - kwmoon( Comment )
	//g_sys.SysDelay( 1 MS );
	return true;
}

bool CPps::SetVsOn( uint unit, int bits )
{
	//2023.08.03 - KWMOON
	if( g_sys.IsPfmEnable() )
	{
		if( g_sys.IsPfmStart() )
		{
			g_sys.SetPfmStop();
		}
	}
	if( bits != 0 )
	{
		m_nVsOnBit[unit] = 0x0;
		for( int ch=0;ch<MAX_PPS_CH;ch++ )
		{
			//2023.10.23 - KWMOON
			bool bOnCh = true;
			if( !(bits & (0x1<<ch) ) )
				bOnCh = false;

			//if( !(bits & (0x1<<ch) ) ) continue;
			if( m_bVsOn[unit][ch] ) continue;
			//2023.08.03 - KWMOON
			if( g_sys.GetPfmFault( unit, ch) )
			{
				g_log.Printf( "%s CH%02d is fault ch, Vs On skip!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
				continue;
			}

			if( !m_bValidVs[unit][ch] )		//2022.12.16 - kwmoon
			{
				if( unit == PPS_UNIT::PPS )	pps.PMBusWrite ( ch, OPERATION  , OPERATION_OFF );
				else                        s1ps.PMBusWrite( ch, OPERATION  , OPERATION_OFF );
			}
			else
			{
				if( unit == PPS_UNIT::PPS )	//2023.10.23 - KWMOON( bOnCh Check )
				{
					if( bOnCh )  pps.PMBusWrite ( ch, OPERATION  , OPERATION_ON );
					else         pps.PMBusWrite ( ch, OPERATION  , OPERATION_OFF);
				}
				else
				{
					if( bOnCh )  s1ps.PMBusWrite( ch, OPERATION  , OPERATION_ON );
					else         s1ps.PMBusWrite( ch, OPERATION  , OPERATION_OFF);
				}			}
		}
		if( unit == PPS_UNIT::PPS ) pps.SetEnable ( true );
		else                        s1ps.SetEnable( true );
		g_sys.SysDelay( ( TON_RISE_TIME * 2) MS );	//MAX TOFF WARN LIMIT

		//2023.08.03 - KWMOON
		if( g_sys.IsPfmEnable() )
		{
			if( g_sys.IsPfmStart() )
			{
				g_sys.SetPfmStop();
			}
		}
		for( int ch=0;ch<MAX_PPS_CH;ch++ )
		{
			m_bVsOn[unit][ch] = false;

			//2023.10.23 - KWMOON
			bool bOnCh = true;
			if( !(bits & (0x1<<ch) ) )
				bOnCh = false;

			//if( (bits & (0x1<<ch) ) )	//2023.10.23 - KWMOON( COMMENT )
			{
				if( g_sys.GetPfmFault( unit, ch) )
				{
					continue;
				}

				int d = 0x0;
				if( unit == PPS_UNIT::PPS ) pps.PMBusRead ( ch, PMBUS_CMD::OPERATION, d );
				else                        s1ps.PMBusRead( ch, PMBUS_CMD::OPERATION, d );
				if( d == OPERATION_ON )
				{
					m_nVsOnBit[unit] |= ( 0x1<<ch);
					m_bVsOn[unit][ch] = true;
					//fprintf( stdout, "%s%02d power on : %s [OPERATION:0x%X]", (unit==PPS)?" PPS":"S1PS", ch, "OK", d );
				}
				else
				{
					if( m_bValidVs[unit][ch] && bOnCh )	//2023.10.23 - KWMOON( bOnCh Add )
					{
						if( unit == PPS_UNIT::PPS )	pps.PMBusWrite ( ch, OPERATION  , OPERATION_OFF );
						else                        s1ps.PMBusWrite( ch, OPERATION  , OPERATION_OFF );
						m_bVsOn[unit][ch] = false;
						g_log.EPrintf( _PPS, "SET_VS_ON", "%s CH:%02d ON Fail [OPERATION:0x%X]", (unit==PPS)?" PPS":"S1PS", ch, d );
					}
					else
					{
						//g_log.Printf( _PPS, "SET_VS_ON", "%s CH:%02d OPERATION SKIP [OPERATION:0x%X]\n", (unit==PPS)?" PPS":"S1PS", ch, d );
					}
				}
			}
		}
	}
	else		//bits == 0
	{
		m_nVsOnBit[unit] = 0x0;
		if( unit == PPS_UNIT::PPS )	SetVsOff( PPS  );
		else						SetVsOff( S1PS );
	}

	return true;
}

bool CPps::SetVsOff( uint unit )
{
	//2023.08.03 - KWMOON
	if( g_sys.IsPfmEnable() )
	{
		if( g_sys.IsPfmStart() )
		{
			g_sys.SetPfmStop();
		}
	}
	m_nVsOnBit[unit] = 0x0;
	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		//2023.08.03 - KWMOON
		if( g_sys.GetPfmFault( unit, ch) )
		{
			m_bVsOn[unit][ch] = false;
			m_bVoutRangeSet[unit][ch]  = false;
			g_log.Printf( "%s CH%02d is fault ch, VsOff!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			continue;
		}

		if( unit == PPS_UNIT::PPS )
			pps.PMBusWrite ( ch, OPERATION  , OPERATION_OFF );
		else
			s1ps.PMBusWrite( ch, OPERATION  , OPERATION_OFF );
		m_bVsOn[unit][ch] = false;
		m_bVoutRangeSet[unit][ch]  = false;
	}
	g_sys.SysDelay( (TOFF_FALL_TIME + 10.0 ) MS );	//MAX TOFF WARN LIMIT

	//if( unit == PPS_UNIT::PPS )	pps.SetEnable( false );
	//else						s1ps.SetEnable( false );
	return true;
}

bool CPps::IsPpsOn()
{
	bool bOn = true;
	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		if( g_sys.IsPfmEnable() )
		{
			if( g_sys.GetPfmFault( PPS, ch) )
				continue;
		}
		if( m_nVsOnBit[PPS] & (0x1<<ch) )
		{
			if( !m_bVsOn[PPS][ch] )
				bOn = false;
		}
	}
	if( m_nVsOnBit[PPS] == 0x0 ) bOn = false;
	//g_log.Printf("IS  PPS ON  : 0x%02X[%s]\n", m_nVsOnBit[PPS], bOn?" ON":"OFF" );
	return bOn;
}

bool CPps::IsS1psOn()
{
	bool bOn = true;
	for( int ch=0;ch<MAX_PPS_CH;ch++ )
	{
		if( g_sys.IsPfmEnable() )
		{
			if( g_sys.GetPfmFault( S1PS, ch) )
				continue;
		}
		if( m_nVsOnBit[S1PS] & (0x1<<ch) )
		{
			if( !m_bVsOn[S1PS][ch] )
				bOn = false;
		}
	}
	if( m_nVsOnBit[S1PS] == 0x0 ) bOn = false;
	//g_log.Printf("IS S1PS ON  : 0x%02X[%s]\n", m_nVsOnBit[S1PS], bOn?" ON":"OFF" );
	return bOn;
}

bool CPps::ClearFaults(uint unit, int ch)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, clear faults skip!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			g_sys.SetPfmEnable( bPfmEnb );
			if( bPfmRun )	g_sys.SetPfmStart();
			else            g_sys.SetPfmStop();
			return false;
		}
	}

	if( unit == PPS )
		return pps.PMBusWrite( ch, CLEAR_FAULTS, 0x0 );
	else
		return s1ps.PMBusWrite( ch, CLEAR_FAULTS, 0x0 );
	g_sys.SysDelay( 1 MS );

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	return true;
}

bool CPps::SetDgsControl( uint unit, int high_low)
{
	int data = ( high_low == 0)?0x0:0x1;
	if( unit == PPS )
		return g_pcie.write_user( BITCHART::DRV_IOA::_PPS_DGS_CHECK , data );
	else
		return g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_DGS_CHECK, data );
	g_sys.SysDelay( 100 US );
	return true;
}

bool CPps::SetSenseControl( uint unit, int high_low )
{
	int data = ( high_low == 0)?0x0:0x1;
	if( unit == PPS )
		return g_pcie.write_user( BITCHART::DRV_IOA::_PPS_SENSE_CHECK , data );
	else
		return g_pcie.write_user( BITCHART::DRV_IOA::_S1PS_SENSE_CHECK, data );
	g_sys.SysDelay( 100 US );
	return true;
}

bool CPps::SetLocalRemoteSensing()
{
	const int REMOTE_SENSING = (0x1<<1) | (0x1);
	const int LOCAL_SENSING  = (0x1<<1) | (0x0);
	if( g_sys.IsConnBib() )
	{
		//Remote Sensing
		g_pcie.write_user( BITCHART::DRV_IOA::_PPS_SENSEING_CONTROL, REMOTE_SENSING );
		if( 0 ) g_log.Printf( _PPS, "PPS_SENSE", "PPS/S1PS SET REMOTE SENSING\n");
	}
	else
	{
		//Local Sensing
		g_pcie.write_user( BITCHART::DRV_IOA::_PPS_SENSEING_CONTROL, LOCAL_SENSING  );
		if( 0 ) g_log.Printf( _PPS, "PPS_SENSE", "PPS/S1PS SET LOCAL SENSING\n");
	}
	g_sys.SysDelay( 100 US );
	return true;
}

//ADC Pps Force Meas
bool CPps::ReadVf(uint unit, int ch, double& rVs)
{
	if( (unit<PPS) || (unit>S1PS) )
	{
		g_log.EPrintf( _PPS, "READ_VF", "%s(%d) error : unit %d index error [use :%d or %d]\n", __FUNCTION__, __LINE__, unit, PPS, S1PS );
		return false;
	}
	if( (ch<PPS_CH0) || (ch>PPS_CH7) )
	{
		g_log.EPrintf( _PPS, "READ_VF", "%s(%d) error : ch %d index error [use :%d or %d]\n", __FUNCTION__, __LINE__, ch, PPS_CH0, PPS_CH7 );
		return false;
	}
	ADC_MUX mux;
	if( unit == PPS_UNIT::PPS ) mux = ADC_MUX::MUX_PPS_F;
	else                        mux = ADC_MUX::MUX_S1PS_F;
	rVs = g_hw.AdcRead( mux, ch, 10 );
	if( unit == PPS_UNIT::S1PS ) rVs = rVs*2;
	return true;
}

//ADC Pps Sense Meas
bool CPps::ReadVs(uint unit, int ch, double& rVs)
{
	if( (unit<PPS) || (unit>S1PS) )
	{
		g_log.EPrintf( _PPS, "READ_VS", "%s(%d) error : unit %d index error [use :%d or %d]\n", __FUNCTION__, __LINE__, unit, PPS, S1PS );
		return false;
	}
	if( (ch<PPS_CH0) || (ch>PPS_CH7) )
	{
		g_log.EPrintf( _PPS, "READ_VS", "%s(%d) error : ch %d index error [use :%d or %d]\n", __FUNCTION__, __LINE__, ch, PPS_CH0, PPS_CH7 );
		return false;
	}

	ADC_MUX mux;
	if( unit == PPS_UNIT::PPS ) mux = ADC_MUX::MUX_PPS_S;
	else                        mux = ADC_MUX::MUX_S1PS_S;
	rVs = g_hw.AdcRead( mux, ch, 10 );
	if( unit == PPS_UNIT::S1PS ) rVs = rVs*2;
	return true;
}

bool CPps::ReadGnd(uint unit, double& rGnd )
{
	int ch = 0;
	if( (unit<PPS) || (unit>S1PS) )
	{
		g_log.EPrintf( _PPS, "READ_GND", "%s(%d) error : unit %d index error [use :%d or %d]\n", __FUNCTION__, __LINE__, unit, PPS, S1PS );
		return false;
	}
	if( unit == PPS ) ch = 0x0;
	else              ch = 0x1;

	ADC_MUX mux = ADC_MUX::MUX_PPS_GND1;	//GND1 ~ 4 ANYTHING SAME
	rGnd = g_hw.AdcRead( mux, ch, 10 );
	return true;
}

//PPS Internal Vout Meas
bool CPps::ReadVout(uint unit,int ch, double& rVout)
{
	int word = 0;
	int vout = 0x0;
	bool normal = true;
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, read vout!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			//g_sys.SetPfmEnable( bPfmEnb );
			//if( bPfmRun )	g_sys.SetPfmStart();
			//else            g_sys.SetPfmStop();
			//return false;
		}
	}
	R_STATUS_WORD status_word;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, READ_VOUT, vout );
		rVout = L16toD( vout );
		//fprintf( stdout, "\t ==> %s(%d) PPS  VOUT_Hex:%04X[Vout:%7.2fV]\n", __FUNCTION__, __LINE__, vout, rVout );
		pps.PMBusRead( ch, STATUS_WORD, word );
		status_word.word = static_cast<short>(word);
	}
	else
	{
		s1ps.PMBusRead( ch, READ_VOUT, vout );
		rVout = L16toD(vout);
		//fprintf( stdout, "\t ==> %s(%d) S1PS VOUT_Hex:%04X[Vout:%7.2fV]\n", __FUNCTION__, __LINE__, vout, rVout );
		s1ps.PMBusRead( ch, STATUS_WORD, word );
		status_word.word = static_cast<short>(word);
	}
	if( 0 )
	{
		g_log.Printf( _PPS, "READ_VOUT", "CH#%d VOUT:%5.2f V STATUS_WORD:0x%04X\n", ch, rVout, status_word.word );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	if( status_word.word != 0x0 ) normal = false;
	return normal;
}

//2023.02.27 - KWMOON( S1PS CURRENT MONITORING )
//dVset Range reference AT6200_PMBUS.ppt(VDS_OFFSET)
bool CPps::GetS1psIoutVsetGainOffset(double dVset, double& rGain, double& rOffset )
{
	rGain   = S1PS_IM_VSET_GAIN0;
	rOffset = S1PS_IM_VSET_OFFSET0;
	if( (dVset>=0.3) && (dVset<=1.0) )
	{
		rGain   = S1PS_IM_VSET_GAIN0;
		rOffset = S1PS_IM_VSET_OFFSET0;
	}
	else if( (dVset>1.0) && (dVset<=7.5) )
	{
		rGain   = S1PS_IM_VSET_GAIN1;
		rOffset = S1PS_IM_VSET_OFFSET1;
	}
	else if( (dVset>7.5) && (dVset<=13.0) )
	{
		rGain   = S1PS_IM_VSET_GAIN2;
		rOffset = S1PS_IM_VSET_OFFSET2;
	}
	else if( (dVset>13.0) && (dVset<=15.0) )
	{
		rGain   = S1PS_IM_VSET_GAIN3;
		rOffset = S1PS_IM_VSET_OFFSET3;
	}
	else
	{
		return false;
	}
	return true;
}

//PPS Internal Iout Meas
bool CPps::ReadIout(uint unit,int ch, double& rIout)
{
	int word = 0;
	int iout = 0x0;
	bool normal = true;
	double dIout = 0.0;
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, read iout!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			//g_sys.SetPfmEnable( bPfmEnb );
			//if( bPfmRun )	g_sys.SetPfmStart();
			//else            g_sys.SetPfmStop();
			//return false;
		}
	}

	R_STATUS_WORD status_word;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, READ_IOUT, iout );
		rIout = L11toD(iout);
		pps.PMBusRead( ch, STATUS_WORD, word );
		status_word.word = static_cast<short>(word);
	}
	else
	{
		s1ps.PMBusRead( ch, READ_IOUT, iout );
		dIout = L11toD(iout);
		//2023.02.27 - KWMOON(VDS_OFFSET / CURRENT MEASURE)
		if( m_bValidVs[unit][ch] )
		{
			double dGain   = 0.0;
			double dOffset = 0.0;
			if( !GetS1psIoutVsetGainOffset( m_dVsVoltage[unit][ch], dGain, dOffset ) )
			{
				rIout = dIout;
			}
			else
			{
				rIout = dIout - ( m_dVsVoltage[unit][ch] * dGain + dOffset );
			}
		}
		else
		{
			rIout = dIout;
		}
		s1ps.PMBusRead( ch, STATUS_WORD, word );
		status_word.word = static_cast<short>(word);
	}
	if( 0 )
	{
		g_log.Printf( _PPS, "READ_IOUT", "CH#%d IOUT:%5.2f A STATUS_WORD:0x%04X\n", ch, rIout, status_word.word );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	if( status_word.word != 0x0 ) normal = false;
	return normal;
}

bool CPps::SetPpsRequest()
{
	bool bOk = g_pcie.write_user( BITCHART::PPS_S1PS::_PPS_REQUEST, 0x1 );
	g_sys.SysDelay( 100 US );
	return bOk;
}

void CPps::SetPpsEnable( uint unit, bool bEnable )
{
	if( unit == PPS ) pps.SetEnable( bEnable );
	else              s1ps.SetEnable( bEnable );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPps::PpsWrite
/// \param ch
/// \param cmd
/// \param data
/// \return
/// \details for Diag & debug
bool CPps::PMBusWrite( uint unit, int ch, PMBUS_CMD cmd, int data )
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, PMBusWrite skip!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			g_sys.SetPfmEnable( bPfmEnb );
			if( bPfmRun )	g_sys.SetPfmStart();
			else            g_sys.SetPfmStop();
			return false;
		}
	}
	bool bOk = false;
	if( unit == PPS ) bOk = pps.PMBusWrite ( ch, cmd, data );
	else              bOk = s1ps.PMBusWrite( ch, cmd, data );

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPps::PpsRead
/// \param ch
/// \param cmd
/// \param rdata
/// \return
/// \details for Diag & debug
bool CPps::PMBusRead ( uint unit, int ch, PMBUS_CMD cmd, int& rdata )
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
		if( g_sys.GetPfmFault( unit, ch) )
		{
			g_log.Printf( "%s CH%02d is fault ch, PMBusRead!\n", (unit==PPS)?" PPS":"S1PS", ch+1 );
			//g_sys.SetPfmEnable( bPfmEnb );
			//if( bPfmRun )	g_sys.SetPfmStart();
			//else            g_sys.SetPfmStop();
			//return false;
		}
	}
	bool bOk = false;
	if( unit == PPS ) bOk = pps.PMBusRead ( ch, cmd, rdata );
	else              bOk = s1ps.PMBusRead( ch, cmd, rdata );

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return bOk;
}

bool CPps::PMBusAlertMaskWrite(uint unit, int ch, PMBUS_CMD xcmd, int mask)
{
	if( unit == PPS ) return pps.PMBusAlertMaskWrite ( ch, xcmd, mask );
	else              return s1ps.PMBusAlertMaskWrite( ch, xcmd, mask );
}

bool CPps::PMBusAlertMaskRead(uint unit, int ch, PMBUS_CMD xcmd, int& mask)
{
	if( unit == PPS ) return pps.PMBusAlertMaskRead ( ch, xcmd, mask );
	else              return s1ps.PMBusAlertMaskRead( ch, xcmd, mask );
}

bool CPps::GetPpsAlert(uint unit)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	bool bAlert = false;
	if( unit == PPS ) bAlert = pps.GetAlert();
	else              bAlert = s1ps.GetAlert();

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return bAlert;
}

bool CPps::ReadStatusByte(uint unit, int ch, R_STATUS_BYTE& status_byte)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	int byte = 0;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, STATUS_BYTE, byte );
		m_StatusByte[unit][ch].byte = status_byte.byte = static_cast<unsigned char>(byte);
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusRead( ch, STATUS_BYTE, byte );
		m_StatusByte[unit][ch].byte = status_byte.byte = static_cast<unsigned char>(byte);
	}
	else
	{
		m_StatusByte[unit][ch].byte = 0x0;
		m_StatusByte[unit][ch].none = 0x1;
		g_log.EPrintf( _PPS, "STATUS_BYTE", "READ Error : Unit(%d) is invalid, use %d or %d\n", unit, PPS_UNIT::PPS, PPS_UNIT::S1PS );
		return false;
	}

	if( 0 )
	{
		fprintf( stdout, "[%s CH%d STATUS BYTE]\n", unit==PPS_UNIT::PPS?"PPS":"S1PS", ch );
		fprintf( stdout, "\t%-20s : [%d]\n", "BUSY", m_StatusByte[unit][ch].busy );
		fprintf( stdout, "\t%-20s : [%d]\n", "OFF", m_StatusByte[unit][ch].off );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_OV_FAULT", m_StatusByte[unit][ch].vout_ov_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "IOUT_OC_FAULT", m_StatusByte[unit][ch].iout_oc_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "VIN_UV_FAULT", m_StatusByte[unit][ch].vin_uv_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "TEMPERATURE", m_StatusByte[unit][ch].temperature );
		fprintf( stdout, "\t%-20s : [%d]\n", "CML", m_StatusByte[unit][ch].cml );
		fprintf( stdout, "\t%-20s : [%d]\n", "NONE", m_StatusByte[unit][ch].none );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return true;
}

bool CPps::ReadStatusWord(uint unit, int ch, R_STATUS_WORD& status_word)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}
	int word = 0;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, STATUS_WORD, word );
		m_StatusWord[unit][ch].word = status_word.word = static_cast<short>(word);
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusRead( ch, STATUS_WORD, word );
		m_StatusWord[unit][ch].word = status_word.word = static_cast<short>(word);
	}
	else
	{
		m_StatusWord[unit][ch].word = 0x0;
		m_StatusWord[unit][ch].unknown = 0x1;
		fprintf( stderr, "READ_STATUS_WORD error : Unit(%d) is invalid, use %d or %d\n", unit, PPS_UNIT::PPS, PPS_UNIT::S1PS );
		return false;
	}

	if( 0 )
	{
		fprintf( stdout, "[%s CH%d STATUS WORD]\n", unit==PPS_UNIT::PPS?"PPS":"S1PS", ch );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT", m_StatusWord[unit][ch].vout );
		fprintf( stdout, "\t%-20s : [%d]\n", "IOUT/POUT", m_StatusWord[unit][ch].iout_pout );
		fprintf( stdout, "\t%-20s : [%d]\n", "INPUT", m_StatusWord[unit][ch].input );
		fprintf( stdout, "\t%-20s : [%d]\n", "MFR_SPEC", m_StatusWord[unit][ch].mfr_spec );
		fprintf( stdout, "\t%-20s : [%d]\n", "PG_STATUS", m_StatusWord[unit][ch].pg_status );
		fprintf( stdout, "\t%-20s : [%d]\n", "FANS", m_StatusWord[unit][ch].fans );
		fprintf( stdout, "\t%-20s : [%d]\n", "OTHER", m_StatusWord[unit][ch].other );
		fprintf( stdout, "\t%-20s : [%d]\n", "UNKNOWN", m_StatusWord[unit][ch].unknown );
		fprintf( stdout, "\t%-20s : [%d]\n", "BUSY", m_StatusWord[unit][ch].busy );
		fprintf( stdout, "\t%-20s : [%d]\n", "OFF", m_StatusWord[unit][ch].off );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_OV_FAULT", m_StatusWord[unit][ch].vout_ov_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "IOUT_OC_FAULT", m_StatusWord[unit][ch].iout_oc_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "VIN_UV_FAULT", m_StatusWord[unit][ch].vin_uv_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "TEMPERATURE", m_StatusWord[unit][ch].temperature );
		fprintf( stdout, "\t%-20s : [%d]\n", "CML", m_StatusWord[unit][ch].cml );
		fprintf( stdout, "\t%-20s : [%d]\n", "NONE", m_StatusWord[unit][ch].none );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return true;
}

bool CPps::ReadStatusVout(uint unit, int ch, R_STATUS_VOUT& status_vout)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	int word = 0;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, STATUS_VOUT, word );
		m_StatusVout[unit][ch].byte = status_vout.byte = static_cast<unsigned char>(word);
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusRead( ch, STATUS_VOUT, word );
		m_StatusVout[unit][ch].byte = status_vout.byte = static_cast<unsigned char>(word);
	}
	else
	{
		m_StatusVout[unit][ch].byte = 0x0;
		g_log.EPrintf( _PPS, "STATUS_VOUT", "READ Error : Unit(%d) is invalid, use %d or %d\n", unit, PPS_UNIT::PPS, PPS_UNIT::S1PS );
		return false;
	}

	if( 0 )
	{
		fprintf( stdout, "[%s CH%d STATUS VOUT]\n", unit==PPS_UNIT::PPS?"PPS":"S1PS", ch );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_OV_FAULT", m_StatusVout[unit][ch].vout_ov_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_OV_WARN" , m_StatusVout[unit][ch].vout_ov_warn  );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_UV_WARN" , m_StatusVout[unit][ch].vout_ov_warn  );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_UV_FAULT", m_StatusVout[unit][ch].vout_uv_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "VOUT_MAX_MIN" , m_StatusVout[unit][ch].vout_max_min  );
		fprintf( stdout, "\t%-20s : [%d]\n", "TON_MAX_FAULT", m_StatusVout[unit][ch].ton_max_fault );
		fprintf( stdout, "\t%-20s : [%d]\n", "TON_MAX_WARN" , m_StatusVout[unit][ch].toff_max_warn );
		fprintf( stdout, "\t%-20s : [%d]\n", "NOT_AVAILABLE", m_StatusVout[unit][ch].not_avail     );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	return true;
}

bool CPps::ReadStatusIout(uint unit, int ch, R_STATUS_IOUT& status_iout)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	int word = 0;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, STATUS_IOUT, word );
		m_StatusIout[unit][ch].byte = status_iout.byte = static_cast<unsigned char>(word);
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusRead( ch, STATUS_VOUT, word );
		m_StatusIout[unit][ch].byte = status_iout.byte = static_cast<unsigned char>(word);
	}
	else
	{
		m_StatusIout[unit][ch].byte = 0x0;
		g_log.EPrintf( _PPS, "STATUS_IOUT", "READ Error : Unit(%d) is invalid, use %d or %d\n", unit, PPS_UNIT::PPS, PPS_UNIT::S1PS );
		return false;
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	return true;
}

bool CPps::ReadStatusInput(uint unit, int ch, R_STATUS_INPUT& status_input)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	int byte = 0;
	if( unit == PPS_UNIT::PPS )
	{
		pps.PMBusRead( ch, STATUS_INPUT, byte );
		m_StatusInput[unit][ch].byte = status_input.byte = static_cast<unsigned char>(byte);
	}
	else if( unit == PPS_UNIT::S1PS )
	{
		s1ps.PMBusRead( ch, STATUS_INPUT, byte );
		m_StatusInput[unit][ch].byte = status_input.byte = static_cast<unsigned char>(byte);
	}
	else
	{
		m_StatusByte[unit][ch].byte = 0x0;
		m_StatusByte[unit][ch].none = 0x1;
		g_log.EPrintf( _PPS, "STATUS_WORD", "READ Error : Unit(%d) is invalid, use %d or %d\n", unit, PPS_UNIT::PPS, PPS_UNIT::S1PS );
		return false;
	}

	if( 0 )
	{
		fprintf( stdout, "[%s CH%d INPUT STATUS]\n", unit==PPS_UNIT::PPS?"PPS":"S1PS", ch );
		fprintf( stdout, "\t%-30s : [%d]\n", "VIN OV FAULT", m_StatusInput[unit][ch].vin_ov_fault );
		fprintf( stdout, "\t%-30s : [%d]\n", "VIN UV WARN ", m_StatusInput[unit][ch].vin_uv_warn  );
		fprintf( stdout, "\t%-30s : [%d]\n", "UNIT OFF FOR INSUFFCIENT...", m_StatusInput[unit][ch].unit_off_for_insufficient );
		fprintf( stdout, "\t%-30s : [%d]\n", "IIN OC WARN", m_StatusInput[unit][ch].iin_oc_warn );
	}
	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();

	return true;
}

//not use, rfu
bool CPps::ReadVdsOffset(uint unit, uint ch, int& value)	//2022.12.03 - kwmoon
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	if( unit == PPS_UNIT::PPS ) return false;
	bool bRead = s1ps.PMBusRead( ch, PMBUS_CMD::VDS_OFFSET, value );

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return bRead;
}

bool CPps::ReadScaleloop(uint unit, uint ch, int& v)
{
	//2023.08.03 - KWMOON
	bool bPfmEnb = g_sys.IsPfmEnable();
	bool bPfmRun = g_sys.IsPfmStart();
	if( bPfmEnb )
	{
		if( bPfmRun )
		{
			g_sys.SetPfmStop();
		}
	}

	bool bOk = false;
	if( unit == PPS_UNIT::PPS )
		bOk = pps.PMBusRead ( ch, VOUT_SCALE_LOOP, v );
	else
		bOk = s1ps.PMBusRead( ch, VOUT_SCALE_LOOP, v );

	//2023.08.03 - KWMOON
	g_sys.SetPfmEnable( bPfmEnb );
	if( bPfmRun )	g_sys.SetPfmStart();
	else            g_sys.SetPfmStop();
	return bOk;
}
