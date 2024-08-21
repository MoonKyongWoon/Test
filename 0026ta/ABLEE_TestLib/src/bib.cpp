#include "bib.hpp"
#include "dutmap.hpp"
#include "log.hpp"
#include "io.hpp"
#include "sys.hpp"

extern CPcie g_pcie;
extern CSys  g_sys;
extern CLog  g_log;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CDut
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDut::CDut( int dut, int x, int y )
{
	m_DutNo = dut;
	m_X = x;
	m_Y = y;
}

//SCAN | IO GROUP | CPU | FC | IO
//   4          8     4    4    4
void CDut::SetSiInfo( int si, uint scan, uint grp, uint cpu, uint fc, uint ioab )
{
	m_nSi        = si;
	m_nSiScanNo  = scan;
	m_nSiIoGroup = grp;
	m_nSiCpuNo   = cpu;
	m_nSiFcNo    = fc;
	m_nSiIoAb    = ioab;
	//qfprintf( stdout, "DUT:%03d CPU%02d FC%02d SCAN:%02d IOGRP:%02d\n", m_DutNo, cpu, fc, scan, grp );
}

void CDut::SetIoSize(uint size)
{
	m_uIoSize = size;
}

int  CDut::GetSi()
{
	return m_nSi;
}

uint CDut::GetDutNo()
{
	return m_DutNo;
}

uint CDut::GetSiScanNo()
{
	return m_nSiScanNo;
}

uint CDut::GetSiIoGroup()
{
	return m_nSiIoGroup;
}

uint CDut::GetSiCpuNo()
{
	return m_nSiCpuNo;
}

uint CDut::GetSiFcNo()
{
	return m_nSiFcNo;
}

uint CDut::GetSiIoAb()
{
	return m_nSiIoAb;
}

uint CDut::GetX()
{
	return m_X;
}

uint CDut::GetY()
{
	return m_Y;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CBib( SCAN / BIB / CE / LIMIT / ADDR )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBib::CBib()
{
	m_sBibId = "";
	m_nBibId = 0x0;
	m_vecDuts.clear();
	m_mapSiIndex.clear();	//2023.07.09 - kwmoon
	m_mapSiIndex0.clear();	//2023.07.12 - kwmoon
	m_mapSiIndex1.clear();	//2023.07.12 - kwmoon
	ClearScan();
	ClearBibSet();
	ClearSetCe();
}

CBib::~CBib()
{
	ClearBib();
}

void CBib::SetBibSel(BIB_SEL sel)
{
	g_pcie.write_user( BITCHART::DRV_IOA::_BIB_SEL, sel );
	//printf("[BIB] SEL(0x%08X) SELECT:0x%02X\n", BITCHART::DRV_IOA::_BIB_SEL, sel );
	//printf("PAUSE"); getchar();
	g_sys.SysDelay( 1 MS );
}

bool CBib::ValidBibParity(int nBibId)
{
	bool bValid = true;
	int nTotalOneSum = 0x0;
	int nDiodeSel    = (nBibId >> 28) & 0xF;
	int nBitSize     = 0x0;
	for( int i=0;i<MAX_BIB_SEL;i++ )
	{
		int nSelOneSum = 0x0;
		int nSelByte   = (nBibId >> (i * 8)) & 0xFF;
		int nSelParity = (nDiodeSel >> i) & 0x1;

		if (i == 3) nBitSize = 0x4;
		else        nBitSize = 0x8;

		for (int bit = 0; bit<nBitSize; bit++)
		{
			if ((1 << bit) & nSelByte)
				nSelOneSum++;
		}
		if (nSelParity == 0)
		{
			if ((nSelOneSum % 2) == 0)
			{
				bValid = false;
				//sLogMsg.sprintf("[ ERR] BIB ID SEL%d Parity Error [ Parity : %d, Diode Count : %d]", nSel, nSelParity, nSelOneSum);
			}
		}
		else
		{
			if ((nSelOneSum % 2) == 1)
			{
				bValid = false;
				//sLogMsg.sprintf("[ ERR] BIB ID SEL%d Parity Error [ Parity : %d, Diode Count : %d]", nSel, nSelParity, nSelOneSum);
			}
		}
		nTotalOneSum += nSelOneSum;
	}
	return bValid;
}

bool CBib::GetBibId( std::string& sBibId, std::string& sBibType, int& nBibNo )
{
	char szBib [16]= {0x0,};
	char szType[8] = {0x0,};
	char szNum [8] = {0x0,};

//	m_nBibId = 0x0;
//	BIB_SEL sels[MAX_BIB_SEL] = { BIB_SEL::SEL0, BIB_SEL::SEL1, BIB_SEL::SEL2, BIB_SEL::SEL3 };
//	for( int i=0;i<MAX_BIB_SEL;i++ )
//	{
//		SetBibSel( sels[i] );
//		int nRead = g_pcie.read_user( BITCHART::DRV_IOA::_BIB_ID );
//		//printf("[BIB] SEL%d DATA(0x%08X):0x%02X\n", i, BITCHART::DRV_IOA::_BIB_ID, nRead );
//		m_nBibId |= ( nRead<<(i*8) );
//		g_sys.SysDelay( 100 US );
//	}
	m_nBibId = GetBibId();
	snprintf( szType, sizeof(szType), "%03XT", (m_nBibId>>16)&0xFFF  );
	snprintf( szNum , sizeof(szNum ), "%d"   , (m_nBibId>> 0)&0xFFFF );
	snprintf( szBib , sizeof(szBib ), "%s %s", szType, szNum );
	m_sBibId = sBibId = std::string(szBib);
	sBibType = std::string( szType );
	nBibNo   = (m_nBibId>> 0)&0xFFFF ;
	return ValidBibParity( m_nBibId );
}

int CBib::GetBibId()
{
	int nBibId = 0x0;
	BIB_SEL sels[MAX_BIB_SEL] = { BIB_SEL::SEL0, BIB_SEL::SEL1, BIB_SEL::SEL2, BIB_SEL::SEL3 };
	for( int i=0;i<MAX_BIB_SEL;i++ )
	{
		SetBibSel( sels[i] );
		int nRead = g_pcie.read_user( BITCHART::DRV_IOA::_BIB_ID )&0xFF;
		if( 0 ) printf("[BIB] SEL%d DATA(0x%08X):0x%02X\n", i, BITCHART::DRV_IOA::_BIB_ID, nRead&0xFF );
		nBibId |= ( nRead<<(i*8) );
		g_sys.SysDelay( 100 US );
	}

//	printf("BibId=%03X%05d\n", (nBibId>>16)&0xffff, nBibId&0xffff);
	return nBibId;
}

int  CBib::GetBibIf()
{
	return g_pcie.read_user( BITCHART::DRV_IOA::_BIB_IF_READ );
}

void CBib::SetBibIf(int enable, int data )
{
	int wData = ((enable&0xF)<<4) | (data&0xF);
	g_pcie.write_user( BITCHART::DRV_IOA::_BIB_IF, wData );
	//fprintf( stdout, "%s(%d) : write(0x%08X,0x%02X)\n", __FUNCTION__, __LINE__, BITCHART::DRV_IOA::_BIB_IF, wData );
	g_sys.SysDelay( 100 US );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_SCAN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::ClearScan()
{
	m_nMemOffset = 0;
	m_ScanOr = 0x0;
	m_ScanAnd = 0xFFFFFFFFFFFFFFFFLL;
	memset( m_ScanMem, 0x0, sizeof(m_ScanMem) );
}

void CBib::ClearBibSet()
{
	m_stBibSet.visible_scan   = 1;
	m_stBibSet.io_size        = 8;
	m_stBibSet.bib_x          = 16;
	m_stBibSet.bib_y          = 12;
	m_stBibSet.f_16mode       = false;
	m_stBibSet.invisible_scan = false;
	m_stBibSet.cpu2x          = true;
}

void CBib::SetScanOr()
{
	int nHigh = 0x0, nLow = 0x0;
	nHigh = static_cast<int>( (m_ScanOr>>32) & 0xFFFFFFFF );
	nLow  = static_cast<int>( (m_ScanOr>> 0) & 0xFFFFFFFF );
	if( 0 ) fprintf( stdout, "ALL SCAN OR : 0x%08X:%08X\n", nHigh, nLow );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_ALL_SCAN_OR_b31_b0 , nLow  );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_ALL_SCAN_OR_b63_b32, nHigh );
}

void CBib::SetScanAnd()
{
	int nHigh = 0x0, nLow = 0x0;
	nHigh = static_cast<int>( (m_ScanAnd>>32) & 0xFFFFFFFF );
	nLow  = static_cast<int>( (m_ScanAnd>> 0) & 0xFFFFFFFF );
	if( 0 ) fprintf( stdout, "ALL SCAN AND : 0x%08X:%08X\n", nHigh, nLow );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_ALL_SCAN_AND_b31_b0 , nLow  );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_ALL_SCAN_AND_b63_b32, nHigh );
}

void CBib::SetScan( int idx, unsigned int long long value )
{
    m_ScanMem[idx] = value;
    m_nMemOffset = idx;
	m_ScanOr |= value;
	m_ScanAnd &= value;
}

void CBib::GetScan( int fc, int line )
{
	(void)fc;
	if( line > (SCAN_MEM_DEPTH-1) )
	{
		g_log.EPrintf( _BIB, __FUNCTION__, "%s Scan memory address %d over than %d",
				 line, SCAN_MEM_DEPTH );
		return;
	}
	g_log.EPrintf( _BIB, __FUNCTION__, "%s line[%05d] : 0x%llX", line, m_ScanMem[line] );
	//read scan memory
	//printf scan
}

bool CBib::WriteScan()
{
    //write Scan Meory
	int address = 0x0;
	for( int m=0;m<SCAN_MEM_DEPTH;m++ )
	{
		int scan_h = static_cast<int>( (m_ScanMem[m] >> 32)&0xffffffff);
		int scan_l = static_cast<int>( (m_ScanMem[m] >>  0)&0xffffffff);
		g_pcie.write_user( BITCHART::ALPG::_SCAN_DATA_b31_b0  + address, scan_l );
		g_pcie.write_user( BITCHART::ALPG::_SCAN_DATA_b63_b32 + address, scan_h );
		if( 0 ) printf("SET_SCAN %03d (0x%08X : 0x%X)\n", m, BITCHART::ALPG::_SCAN_DATA_b31_b0  + address, scan_l );
		if( 0 ) printf("SET_SCAN %03d (0x%08X : 0x%X)\n", m, BITCHART::ALPG::_SCAN_DATA_b63_b32 + address, scan_h );
		address += 4;
	}
	//OR
	SetScanOr();

	//AND
	SetScanAnd();
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_ADD_SIZE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::SetAddSize( uint block_shift, uint page_shift, uint colsize, uint mcp, uint nchip )
{
	m_stAddSize.block_shift = block_shift;
	m_stAddSize.page_shift  = page_shift;
	m_stAddSize.col_size    = colsize;
	m_stAddSize.mcp         = mcp;
	m_stAddSize.nchip       = nchip;
}

uint CBib::GetBlockShift()
{
	return m_stAddSize.block_shift;
}

uint CBib::GetPageShift()
{
	return m_stAddSize.page_shift;
}

uint CBib::GetColumnSize()
{
	return m_stAddSize.col_size;
}

uint CBib::GetCeCount()
{
	return m_stAddSize.mcp;
}

uint CBib::GetChipCount()
{
	return m_stAddSize.nchip;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_LIMIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::SetLimit( uint block, uint fail_block_limit, uint fail_add_limit )
{
	m_stSetLimit.block_size_dut = block;
	m_stSetLimit.fail_block_limit_per_ce = fail_block_limit;
	m_stSetLimit.fail_add_limit_per_ce = fail_add_limit;
}

uint CBib::GetBlockLimit()
{
	return m_stSetLimit.block_size_dut;
}

uint CBib::GetCeBlockLimit()
{
	return m_stSetLimit.block_size_dut / m_stAddSize.mcp;
}

uint CBib::GetChipBlockLimit()
{
	return GetCeBlockLimit() / m_stAddSize.nchip;
}

uint CBib::GetFailBlockLimit()	//FAIL BLOCK LIMIT
{
	return m_stSetLimit.fail_block_limit_per_ce;
}

uint CBib::GetFailAddressLimit()	//FAIL ADDR  LIMIT
{
	return m_stSetLimit.fail_add_limit_per_ce;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_BIB
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::SetBib( int visible_scan, int io_size, int x, int y, bool f16, bool invisible, bool cpu2x )
{
	m_stBibSet.visible_scan = visible_scan;
	m_stBibSet.io_size  = io_size;
	m_stBibSet.bib_x    = x;
	m_stBibSet.bib_y    = y;
	m_stBibSet.f_16mode = f16;
	m_stBibSet.invisible_scan = invisible;
	m_stBibSet.cpu2x = cpu2x;

	//Make Duts
	ClearBib();
	m_vecDuts.resize( x * y );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_CE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::ClearSetCe()
{
	memset( m_stSetCe, 0x0, sizeof(m_stSetCe) );
	for( int idx=0;idx<MAX_CE_NUM*2;idx++ )
	{
		m_stSetCe[idx].bUse = false;
	}
	m_stSetCeCond1Dut.bUse = false;
}

bool CBib::SetCe( unsigned int mcp, unsigned int ioab, unsigned int ioab_order, unsigned int offset, unsigned int ce_order )
{
	if( (mcp<0) || (mcp>m_stAddSize.mcp) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, "SET_CE", "MCP order %d is bigger than %d", mcp, m_stAddSize.mcp );
		return false;
	}
	if( ce_order > m_stAddSize.mcp )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, "SET_CE", "CE order %d is bigger than %d", ce_order, m_stAddSize.mcp );
		return false;
	}

	bool bFound = false;
	for( int idx=0;idx<MAX_CE_NUM*2;idx++ )
	{
		if( !m_stSetCe[mcp].bUse ) continue;
		if( (m_stSetCe[mcp].mcp_order == mcp) && (m_stSetCe[mcp].ioab_order == ioab_order) && (m_stSetCe[mcp].ce_start == offset) && (m_stSetCe[mcp].ce_no == ce_order) )
		{
			m_stSetCe[mcp].ioab |= ioab;
			bFound = true;
		}
	}
	if( !bFound )
	{
		m_stSetCe[mcp].mcp_order   = mcp;
		m_stSetCe[mcp].ioab        = ioab;
		m_stSetCe[mcp].ioab_order  = ioab_order;
		m_stSetCe[mcp].ce_start    = offset;
		m_stSetCe[mcp].ce_no       = ce_order;
		m_stSetCe[mcp].bUse        = true;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET_CE_COND_1DUT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBib::SetCeCond1Dut(unsigned int ioa_bit, unsigned int iob_bit, unsigned int ioa_ce, unsigned int iob_ce )
{
	m_stSetCeCond1Dut.ioa_dutbits = ioa_bit;
	m_stSetCeCond1Dut.iob_dutbits = iob_bit;
	m_stSetCeCond1Dut.ioa_cecnt   = ioa_ce;
	m_stSetCeCond1Dut.iob_cecnt   = iob_ce;
	m_stSetCeCond1Dut.bUse        = true;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BIB( SI(), )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::ClearBib()
{
	for( CDut* p : m_vecDuts )
	{
		if( p )	delete p;
	}
	m_vecDuts.clear();
	m_mapSiIndex.clear();	//2023.07.09 - kwmoon
	m_mapSiIndex0.clear();	//2023.07.12 - kwmoon
	m_mapSiIndex1.clear();	//2023.07.12 - kwmoon
}

/*
[SI]
 0         1       2     3       4      5   ......      15
16        17      18    19      20     21   ......      31

[BIB INDEX]
(0,11) (1,11) (2,11) (3,11)  (4,11) (5,11)  ......  (15,11)
(0,10) (1,10) (2,10) (3,10)  (4,10) (5,10)  ......  (15,10)
*/
bool CBib::Bib( int index, int si )
{
	int bib_x = index % m_stBibSet.bib_x;
	int bib_y = m_stBibSet.bib_y - (index/m_stBibSet.bib_x+1);
	int arr_x = index % m_stBibSet.bib_x;
	int arr_y = index / m_stBibSet.bib_x;
	int dut = DUTMAP_12x16[arr_y][arr_x];

	uint ioab  = (si>> 0) & 0xF;	//IO_AB
	uint fcno  = (si>> 0) & 0xF0;	//BIB_FC
	uint cpu   = (si>> 0) & 0xF00;	//BIB_CPU
	uint iogrp = (si>>12) & 0xFF;
	uint scan  = (si>>20) & 0xF;

	if( (ioab != IOA) && (ioab != IOB) && (ioab != IOAB) )
	{
		g_log.EPrintf( _BIB, "BIB", "DUT%03d IOA/IOB/IOAB use error!", dut+1 );
		return false;
	}

	if( (fcno != FC1) && (fcno != FC2) )
	{
		g_log.EPrintf( _BIB, "BIB", "DUT%03d FC1/FC2 use error!", dut+1 );
		return false;
	}

	if( (cpu != CPU1) && (cpu != CPU2) )
	{
		g_log.EPrintf( _BIB, "BIB", "DUT%03d CPU1/CPU2 use error!", dut+1 );
		return false;
	}

	uint max_io_grp = (MAX_SYS_IO_BIT / m_stBibSet.io_size);
	if( (iogrp < 1) && (iogrp > max_io_grp) )
	{
		g_log.EPrintf( _BIB, "BIB", "DUT%03d SI IO GROUP(%d) error, use [%d~%d]!", dut+1, iogrp, 1, max_io_grp );
		return false;
	}

	if( (scan < 1) && (scan > MAX_SCAN) )
	{
		g_log.EPrintf( _BIB, "BIB", "DUT%03d SI SCAN NO(%d) error, use [%d~%d]!", dut+1, scan, 1, m_stBibSet.visible_scan );
		return false;
	}
	iogrp = iogrp - 1;	//zero index
	scan  = scan  - 1;	//zero index

	CDut* pDut = new CDut(dut, bib_x, bib_y);
	pDut->SetIoSize( m_stBibSet.io_size );
	pDut->SetSiInfo( si, scan, iogrp, cpu, fcno, ioab );
	if( dut < static_cast<int>(m_vecDuts.size()) )
	{
		m_vecDuts[dut] = pDut;
		m_mapSiIndex.insert( std::make_pair( (scan<<8) | iogrp, pDut) );	//2023.07.09 - KWMOON
		//g_log.Printf("m_vecDuts[%03d] insert", dut );

		if( cpu == CPU1 )
		{
			//g_log.Printf("[CPU0] MAP INSERT SCAN:%02d IOGRP:%02d DUT:%03d", scan, iogrp, dut );
			m_mapSiIndex0.insert( std::make_pair( (scan<<8) | iogrp, pDut) );	//2023.07.09 - KWMOON
		}
		else
		{
			//g_log.Printf("[CPU1] MAP INSERT SCAN:%02d IOGRP:%02d DUT:%03d", scan, iogrp, dut );
			m_mapSiIndex1.insert( std::make_pair( (scan<<8) | iogrp, pDut) );	//2023.07.09 - KWMOON
		}
		//g_log.Printf(NULL);
	}
	else
	{
		delete pDut;
		g_log.EPrintf( _BIB, "BIB", "DUT%03d is invalid, valid dut is [%d~%d]!", dut+1, 0, (m_stBibSet.bib_x * m_stBibSet.bib_y)-1 );
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SCAN MASK
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBib::ClrScanMask()
{
	//SetScanEnableAll();	//2023.12.26 - KWMOON( FOR TEST )
	//g_sys.SysDelay( 10 US );
	g_pcie.write_user( BITCHART::ALPG::RESULT_REG::_FM_SCAN_MASK_CLEAR, 0x1 );
	g_sys.SysDelay( 10 US );
}

void CBib::SetScanEnableAll()
{
	int nScanOffset = 0x10;	//bitchart address offset
	for( int scan=0;scan<SCAN_MEM_DEPTH;scan++ )
	{
		int nAddrOffset = (scan * nScanOffset);
		//FC1
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_0_15  + nAddrOffset, 0x0 );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_16_31 + nAddrOffset, 0x0 );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_32_47 + nAddrOffset, 0x0 );
		//FC2
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_0_15  + nAddrOffset, 0x0 );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_16_31 + nAddrOffset, 0x0 );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_32_47 + nAddrOffset, 0x0 );
	}
}

void CBib::SetScanMaskAll()
{
	int nScanOffset = 0x10;	//bitchart address offset
	for( int scan=0;scan<SCAN_MEM_DEPTH;scan++ )
	{
		int nAddrOffset = (scan * nScanOffset);
		//FC1
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_0_15  + nAddrOffset, 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_16_31 + nAddrOffset, 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_32_47 + nAddrOffset, 0xFFFFFFFF );
		//FC2
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_0_15  + nAddrOffset, 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_16_31 + nAddrOffset, 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_32_47 + nAddrOffset, 0xFFFFFFFF );
	}
}

void CBib::WriteScanMask( uint fc, uint realscan, uint index, int scanvalue )
{
	int nScanOffset  = 0x10;
	int nScanMemory  = (realscan * nScanOffset);	//real scan offset
	int nScanAddress = 0x0;

	if( fc == FC00 )
	{
		nScanAddress = BITCHART::ALPG::FM_SCAN_MASK::_FC1_SCAN0_IOAB_0_15;
		nScanAddress = nScanAddress + nScanMemory + (index*4);
	}
	else
	{
		nScanAddress = BITCHART::ALPG::FM_SCAN_MASK::_FC2_SCAN0_IOAB_0_15;
		nScanAddress = nScanAddress + nScanMemory + (index*4);
	}
	//if( 1 && scanvalue )	//test
	if( 0 )
	{
		g_log.Printf( DEV_LOG_TYPE::_SCAN, "WRITE SCAN MASK", "FC%02d SCAN%03d Index%02d (0x%08X,0x%08X)",
					  fc, realscan, index, nScanAddress, scanvalue );
	}
	g_pcie.write_user( nScanAddress, scanvalue );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// API sub method
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int  CBib::GetBibX()
{
	return m_stBibSet.bib_x;
}

int  CBib::GetBibY()
{
	return m_stBibSet.bib_y;
}

int  CBib::GetBibIo()
{
	return m_stBibSet.io_size;
}

bool CBib::IsBibF16Mode()
{
	return m_stBibSet.f_16mode;
}

bool CBib::IsBibScanInvisible()
{
	return m_stBibSet.invisible_scan;
}

bool CBib::IsBib2Cpu()
{
	return m_stBibSet.cpu2x;
}

bool CBib::GetFc(int dut, int& rFc)
{
	if( (dut<0) || (dut>m_stBibSet.bib_x*m_stBibSet.bib_y) )
	{
		g_log.EPrintf( _BIB, "GET_FC", "DUT%03d is invalid, valid dut is [%d~%d]!", dut, 0, (m_stBibSet.bib_x * m_stBibSet.bib_y)-1 );
		return false;
	}

		 if( m_vecDuts[dut]->GetSiFcNo() == BIB_FC::FC1 ) rFc = 0;
	else if( m_vecDuts[dut]->GetSiFcNo() == BIB_FC::FC2 ) rFc = 1;
	else
	{
		g_log.EPrintf( _BIB, "GET_FC", "DUT%03d FC is invalid!", dut );
		rFc = -1;
		return false;
	}
	return true;
}

bool CBib::IsMyDut( BIB_CPU cpu, int dut, bool& bMyDut )
{
	bMyDut = false;
	if( (dut<0) || (dut>m_stBibSet.bib_x*m_stBibSet.bib_y) )
	{
		g_log.EPrintf( _BIB, "GET_FC", "DUT%03d is invalid, valid dut is [%d~%d]!", dut, 0, (m_stBibSet.bib_x * m_stBibSet.bib_y)-1 );
		return false;
	}
	if( m_vecDuts[dut]->GetSiCpuNo() == cpu ) bMyDut = true;
	else                                      bMyDut = false;
	return true;
}

bool CBib::GetCeOrder( uint mcp, uint& ce_order )
{
	bool bFound = false;
	for( int idx=0;idx<MAX_CE_NUM*2;idx++ )
	{
		if( !m_stSetCe[idx].bUse ) continue;
		if( mcp == m_stSetCe[idx].mcp_order )
		{
			bFound = true;
			ce_order = m_stSetCe[idx].ce_no;
			break;
		}
	}
	return bFound;
}

/*
		SET_CE( 0, IOA, 0, 0x0 	 , CE1 );
		SET_CE( 1, IOA, 1, 0x1000, CE2 );
		SET_CE( 2, IOA, 2, 0x2000, CE3 );
		SET_CE( 3, IOA, 3, 0x3000, CE4 );
		SET_CE( 4, IOA, 4, 0x4000, CE5 );
		SET_CE( 5, IOA, 5, 0x5000, CE6 );
		SET_CE( 6, IOA, 6, 0x6000, CE7 );
		SET_CE( 7, IOA, 7, 0x7000, CE8 );

		SET_CE( 0, IOA, 0, 0x0    , CE1 );
		SET_CE( 1, IOA, 1, 0x8000 , CE3 );
		SET_CE( 2, IOB, 0, 0x10000, CE2 );
		SET_CE( 3, IOB, 1, 0x18000, CE4 );
 */
bool CBib::GetCeOfBlock( uint blockaddr, int& ceno )
{
	bool bFound = false;
	for( int idx=0;idx<MAX_CE_NUM*2-1;idx++ )
	{
		if( !m_stSetCe[idx].bUse ) continue;
		if( blockaddr >= m_stSetCe[idx].ce_start )		//blockaddr = 0x1005
		{
			if( blockaddr < m_stSetCe[idx+1].ce_start )	//blockaddr < 0x2000
			{
				ceno = m_stSetCe[idx].ce_no;
				bFound = true;
				break;
			}
		}
	}
	return bFound;
}

bool CBib::GetCeAddrMcp( uint mcp, int& ce_addr )
{
	bool bFound = false;

	for( int idx=0;idx<MAX_CE_NUM*2;idx++ )
	{
		if( !m_stSetCe[idx].bUse ) continue;
		if( mcp == m_stSetCe[idx].mcp_order )
		{
			bFound = true;
			ce_addr = m_stSetCe[idx].ce_start;
			break;
		}
	}
	return bFound;
}

bool CBib::GetXYOfDut( int dut, uint& x, uint& y )
{
	x = 0;
	y = 0;
	if( dut > static_cast<int>(m_vecDuts.size()-1) )
		return false;
	x = m_vecDuts[dut]->GetX();
	y = m_vecDuts[dut]->GetY();
	return true;
}

bool CBib::GetSiabOfDut( int dut, uint& scan, uint& iogrp, uint& ioab )
{
	scan  = 0;
	iogrp = 0;
	ioab  = 0;
	if( dut > static_cast<int>(m_vecDuts.size()-1) )
		return false;
	scan  = m_vecDuts[dut]->GetSiScanNo();
	iogrp = m_vecDuts[dut]->GetSiIoGroup();
	ioab  = m_vecDuts[dut]->GetSiIoAb() - 1;
	return true;
}

bool CBib::GetXyToIndex( uint x, uint y, uint& index )
{
	bool bFound = false;
	index = 0;
	for( CDut* pdut : m_vecDuts )
	{
		if( (pdut->GetX() == x) && (pdut->GetY() == y) )
		{
			bFound = true;
			index = pdut->GetDutNo();
		}
	}
	return bFound;
}

bool CBib::GetXyToSiab( uint x, uint y, uint& scan, uint& iogrp, uint& ioab )
{
	bool bFound = false;
	scan  = 0;
	iogrp = 0;
	ioab  = IOA;
	for( CDut* pdut : m_vecDuts )
	{
		if( (pdut->GetX() == x) && (pdut->GetY() == y) )
		{
			bFound = true;
			scan  = pdut->GetSiScanNo();
			iogrp = pdut->GetSiIoGroup();
			ioab  = pdut->GetSiIoAb();
		}
	}
	return bFound;
}

bool CBib::GetSiToIndex( uint realscan, uint iogrp, uint& index, bool is_bibscan )
{
	bool bFound = false;
	uint scan   = realscan;
	if( !is_bibscan ) scan = realscan / GetCeCount();	//realscan(0~31) / ce count(8) :  0 ~ 3

	//2023.07.09 - KWMOON
	if( 1 )			//07.12 new
	{
		if( 1 )		//7.12 new
		{
			std::unordered_map<uint, CDut*>* pMap;
			uint sikey = (scan<<8) | iogrp;
			if( g_sys.GetCurrentCpuNo() == CPU00 )
				pMap = &m_mapSiIndex0;
			else
				pMap = &m_mapSiIndex1;

			std::unordered_map<uint, CDut*>::iterator iter = pMap->find( sikey );
			if( iter != pMap->end() )
			{
				CDut* pDut = iter->second;
				if( pDut )
					index  = pDut->GetDutNo();
				bFound = true;
			}
			return bFound;
		}
		else	//7.10 back-do
		{
			uint sikey = (scan<<8) | iogrp;
			std::unordered_map<uint, CDut*>::iterator iter = m_mapSiIndex.find( sikey );
			if( iter != m_mapSiIndex.end() )
			{
				CDut* pDut = iter->second;
				if( pDut )
					index  = pDut->GetDutNo();
				bFound = true;
			}
			return bFound;
		}
	}
	else
	{
		for( CDut* pdut : m_vecDuts )
		{
			if( (pdut->GetSiScanNo() == scan ) && (pdut->GetSiIoGroup() == iogrp) )
			{
				bFound = true;
				index  = pdut->GetDutNo();
				break;
			}
		}
		if( !bFound )
		{
			//fprintf( stdout, "%s error : realscan:%02d iogrp:%02d not found!\n", __FUNCTION__, realscan, iogrp );
		}
	}
	return bFound;
}

bool CBib::GetSiToXy( uint scan, uint iogrp, uint& x, uint& y )
{
	bool bFound = false;
	//2023.07.09 - KWMOON
	if( 1 )
	{
		uint sikey = (scan<<8) | iogrp;
		std::unordered_map<uint, CDut*>::iterator iter = m_mapSiIndex.find(sikey);
		if( iter != m_mapSiIndex.end() )
		{
			CDut* pDut = iter->second;
			if( pDut )
			{
				x = pDut->GetX();
				y = pDut->GetY();
				bFound = true;
			}
		}
		return bFound;
	}
	else
	{
		for( CDut* pdut : m_vecDuts )
		{
			if( (pdut->GetSiScanNo() == scan ) && (pdut->GetSiIoGroup() == iogrp) )
			{
				bFound = true;
				x = pdut->GetX();
				y = pdut->GetY();
			}
		}
	}
	return bFound;
}

//cpu no, fc no, ioab, iopin[0-47]
bool CBib::GetIoGrp( uint cpu , uint fc, uint ioab, uint iopin, uint& iogrp )
{
	uint max_group   = MAX_SYS_IO_BIT / GetBibIo();  //48(or24) = 384 / 8(or16)
	uint ioab_offset = max_group / 2;                //24(or12)
	uint cpu_offset  = ioab_offset / 2;              //12(or 6)
	uint fc_offset   = cpu_offset  / USE_FC;         // 6(or3)
	uint pin_offset  = iopin / GetBibIo();           //0 = 0, 8 = 1, 16 = 2, 24 = 3

	if( ioab == IOA ) ioab_offset = 0;

	iogrp = ioab_offset + (cpu_offset*cpu) + (fc*fc_offset) + pin_offset;
	if( iogrp > (max_group-1) ) return false;
	return true;
}

uint CBib::GetDutPerFc()
{
	uint max_group   = MAX_SYS_IO_BIT / GetBibIo();  //48(or24) = 384 / 8(or16)
	uint ioab_offset = max_group / 2;                //24(or12)
	uint cpu_offset  = ioab_offset / 2;              //12(or 6)
	uint fc_offset   = cpu_offset  / USE_FC;         // 6(or3)
	return 	fc_offset;
}

uint CBib::GetDutPerCpu()
{
	uint max_group   = MAX_SYS_IO_BIT / GetBibIo();  //48(or24) = 384 / 8(or16)
	uint ioab_offset = max_group / 2;                //24(or12)
	uint cpu_offset  = ioab_offset / 2;              //12(or 6)
	return 	cpu_offset;
}

uint CBib::GetDutPerIoAb()
{
	uint max_group   = MAX_SYS_IO_BIT / GetBibIo();  //48(or24) = 384 / 8(or16)
	uint ioab_offset = max_group / 2;                //24(or12)
	return 	ioab_offset;
}

int  CBib::GetSi(int index)
{
	if( index > static_cast<int>(m_vecDuts.size()-1) )
		return false;
	return m_vecDuts[index]->GetSi();
}
