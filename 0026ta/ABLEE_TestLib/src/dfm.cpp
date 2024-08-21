#include "bib.hpp"
#include "bitchart.hpp"
#include "dfm.hpp"
#include "hw.hpp"
#include "io.hpp"
#include "log.hpp"
#include "sys.hpp"
#include "pg.hpp"
#include "util.hpp"

extern CBib  g_bib;
extern CHw   g_hw;
extern CLog  g_log;
extern CPcie g_pcie;
extern CPg   g_pg;
extern CSys  g_sys;

CDfm::CDfm()
{
	m_FmMode = FAIL_WRITE_MODE;
	m_pFmMemory[0] = nullptr;
	m_pFmMemory[1] = nullptr;
}

CDfm::~CDfm()
{
	ClearFmPointer();
}

void CDfm::DfmCtrlReset()
{
	//YBKIM
	return;

	//g_log.Printf( DEV_LOG_TYPE::_DFM, "CTRL", "DFM CTRL RESET(0x%08X) WAIT 1 S\n", BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET );
	int delay = 1000;
	g_pcie.write_user(BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET, 0x1 );
	g_sys.SysDelay( delay MS );		//must
	//g_log.Printf( "DFM CTRL RESET ( %d MS )", delay );
}

void CDfm::ClearFmPointer(uint fc)
{
	if( fc <FC00 || fc> ALL_FC )
		fc = ALL_FC;

	if( fc == ALL_FC )
	{
		for( int i=0;i<USE_FC;i++ )
		{
			if( m_pFmMemory[i]!=nullptr )
			{
				delete [] m_pFmMemory[i];
				m_pFmMemory[i] = nullptr;
			}
			m_nFmFailCount[i] = 0;
		}
	}
	else
	{
		if( m_pFmMemory[fc]!=nullptr )
		{
			delete [] m_pFmMemory[fc];
			m_pFmMemory[fc] = nullptr;
		}
		m_nFmFailCount[fc] = 0;
	}
}

//DFM Memory write by DFM(Pattern)
void CDfm::FmWriteGlobal(bool bWrite )
{
	if( bWrite )
	{
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x1 );	//DBM:0, DFM:1
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_SAVE_EN  , 0x1 );	//0:DISABLE, 1:ENABLE
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//D[0] : 0 Write Operation, 1 Read Operation
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x1 );	//0:DMA WRITE, 1:DFM WRITE
// kyb		DfmCtrlReset();
		g_pg.CmpClear();
		g_pg.DfmClear();
	}
	else
	{
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_FM_LAST_READ, 0x1 );
// kyb		g_sys.SysDelay( 1 MS );
		g_sys.SysDelay( 1 US );

		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x1 );	//DBM:0, DFM:1
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_SAVE_EN  , 0x0 );	//0:DISABLE, 1:ENABLE
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//0:DMA WRITE, 1:DFM WRITE
	}
// kyb	g_sys.SysDelay( 10 MS );
	g_sys.SysDelay( 1 US );
	m_bFmWrite = bWrite;
}

bool CDfm::GetFmWrite()
{
	return m_bFmWrite;
}

void CDfm::SetFmWriteMode()
{
	const int ALL_MODE   = (0x1<<0x0);		//D[2:0] : 0x1
	const int COMP_MODE  = (0x1<<0x1);		//D[2:0] : 0x2
	const int COMP_LIMIT = (0x1<<0x2);		//D[2:0] : 0x4 - 0x6
//	const int AUTO_MASK  = (0x1<<0x12);		//D[ 12] : AUTO SCAN MASK
	const int AUTO_MASK  = (0x1<<  12);		//D[ 12] : AUTO SCAN MASK	//2023-02-24 fatric
	int WriteMode        = 0x0;

	//FAIL SAVE MODE
	if( m_FmMode == FAIL_WRITE_MODE )
	{
		WriteMode = FAIL_WRITE_MODE;
	}
	//CPE
	if( m_FmMode & ALL_WRITE_MODE )
	{
		WriteMode |= ALL_MODE;
		g_pg.SetEdqSel(1);		//Expect Low
		if( 0 ) g_log.Printf("SetEdqSel:1");
	}
	else
	{
		g_pg.SetEdqSel(0);		//EDQ
		if( 0 ) g_log.Printf("SetEdqSel:0");
	}
	//FM COMPRESS
	if( m_FmMode & EFM_COMP_WRITE_MODE )
	{
		WriteMode |= COMP_MODE;
	}
	//FM COMPRESS + LIMIT
	if( m_FmMode & EFM_FAIL_LIMIT_MODE )
	{
		WriteMode |= COMP_LIMIT;
	}
	if( m_FmMode & EFM_AUTO_MASK_MODE )
	{
		WriteMode |= AUTO_MASK;
	}
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_FM_WRITE_MODE, WriteMode );
	if( 0 ) g_log.Printf( DEV_LOG_TYPE::_DFM, "SET_FMMODE_GLOBAL", "0x%X[0x%X]", m_FmMode, WriteMode );
	if( 0 ) PrintFmMode();
}

void CDfm::ResetFmModeGlobal(unsigned int mode)
{
	uint mask_mode = ((~mode)&0xFFFFFFFF);
	m_FmMode &= mask_mode;
	if( 0 ) g_log.Printf( DEV_LOG_TYPE::_DFM, "RESET_FMMODE_GLOBAL", "0x%X", m_FmMode );
	//SetFmWriteMode();
}

void CDfm::SetFmModeGlobal(unsigned int mode )
{
	m_FmMode |= mode;
	SetFmWriteMode();
}

//for debug
void CDfm::PrintFmMode()
{
	g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "[PRINT FM MODE]" );
	if( m_FmMode == FAIL_WRITE_MODE )
	{
		g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FAIL_SAVE_MODE" );
	}
	else
	{
		if( m_FmMode & ALL_WRITE_MODE      ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "ALL_WRITE_MODE"      );
		if( m_FmMode & FM_MODE_D01         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D01"         );
		if( m_FmMode & EFM_COMP_WRITE_MODE ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_COMP_WRITE_MODE" );
		if( m_FmMode & EFM_FAIL_LIMIT_MODE ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_FAIL_LIMIT_MODE" );
		if( m_FmMode & EFM_BG_WRITE        ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_BG_WRITE"        );
		if( m_FmMode & EFM_RUN             ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_RUN"             );
		if( m_FmMode & EFM_RUN_CLEAR       ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_RUN_CLEAR"       );
		if( m_FmMode & FM_MODE_007         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_007"         );
		if( m_FmMode & EDBM_BG_WRITE       ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EDBM_BG_WRITE"       );
		if( m_FmMode & EDBM_RUN            ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EDBM_RUN"            );
		if( m_FmMode & EDBM_RUN_CLEAR      ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EDBM_RUN_CLEAR"      );
		if( m_FmMode & EDBM_XY_MODE        ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EDBM_XY_MODE"        );
		if( m_FmMode & EDBM_TP_MODE        ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EDBM_TP_MODE"        );
		if( m_FmMode & FM_MODE_D13         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D13"         );
		if( m_FmMode & FM_MODE_D14         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D14"         );
		if( m_FmMode & FM_MODE_D15         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D15"         );
		if( m_FmMode & MATCH_IOnRB         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "MATCH_IOnRB"         );
		if( m_FmMode & MATCH_DIAG_MODE     ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "MATCH_DIAG_MODE"     );
		if( m_FmMode & FM_MODE_D18         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D18"         );
		if( m_FmMode & FM_MODE_D19         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D19"         );
		if( m_FmMode & FM_MODE_D20         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D20"         );
		if( m_FmMode & FM_MODE_D21         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D21"         );
		if( m_FmMode & FM_MODE_D22         ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "FM_MODE_D22"         );
		if( m_FmMode & EFM_ECC_16BIT_FCNT  ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_ECC_16BIT_FCNT"  );
		if( m_FmMode & EFM_IO_COMP_MODE    ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_IO_COMP_MODE"    );
		if( m_FmMode & EFM_FCNT_SAVE_MODE  ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_FCNT_SAVE_MODE"  );
		if( m_FmMode & EFM_AUTO_MASK_MODE  ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_AUTO_MASK_MODE"  );
		if( m_FmMode & EFM_REAL_TIME_MODE  ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_REAL_TIME_MODE"  );
		if( m_FmMode & EFM_BLOCK_MASK_MODE ) g_log.Printf( DEV_LOG_TYPE::_DFM, __FUNCTION__, "EFM_BLOCK_MASK_MODE"  );
	}
}

//for debug
void CDfm::ClrExtFmMemory(uint count)
{
	ClearFmPointer();
	FmWriteGlobal( true );
	char szEmpty[DMA_WR_MAX_SIZE] = { 0x0, };
	memset( szEmpty, 0x0, DMA_WR_MAX_SIZE );

	int loop_count      = count;
	uint64_t fc_dfm_address = FC1_DFM_STA_ADDR;
	for( int fc=0;fc<USE_FC;fc++ )
	{
		if( fc == FC00 ) fc_dfm_address  = FC1_DFM_STA_ADDR;
		else             fc_dfm_address  = FC2_DFM_STA_ADDR;
		for( int n=0;n<loop_count;n++ )
		{
			//fprintf( stdout, "FC%d CLR FM ADDRESS:0x%lX : Size:%d\n", fc, fc_dfm_address, DMA_WR_MAX_SIZE );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
			g_pcie.write_dma ( BITCHART::ALPG::_DMA_DATA   , szEmpty, DMA_WR_MAX_SIZE );
			fc_dfm_address += DMA_WR_MAX_SIZE;
		}
	}
	FmWriteGlobal( false);
}

//api
void CDfm::ClrExtFmGlobal()
{
	ClearFmPointer();
	FmWriteGlobal( true );
	char szEmpty[DMA_WR_MAX_SIZE] = { 0x0, };
	memset( szEmpty, 0x0, DMA_WR_MAX_SIZE );

	UTIL::TIMER timer;
	timer.start();

	uint64_t fc_dfm_address = FC1_DFM_STA_ADDR;	
	for( int fc=0;fc<2;fc++ )
	{
		if( fc == FC00 ) fc_dfm_address  = FC1_DFM_STA_ADDR;
		else             fc_dfm_address  = FC2_DFM_STA_ADDR;

		for( unsigned long int m=0;m<EXTFM_FC_MAX_LIMIT_BYTES;m+=DMA_WR_MAX_SIZE )
		//for( unsigned long int m=0;m<MEM_SIZE;m+=DMA_WRITE_MAX_SIZE )
		{
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
			g_pcie.write_dma ( BITCHART::ALPG::_DMA_DATA   , szEmpty, DMA_WR_MAX_SIZE );
			fc_dfm_address += DMA_WR_MAX_SIZE;
		}
	}
	double dElapTime = timer.getElapsedTime();
	if( 0 ) g_log.Printf( "DFM(FC1/FC2) %d GByte memory clear time : %f sec\n", static_cast<int>(EXTFM_MAX_LIMIT_BYTES/1.0e9), dElapTime );

	FmWriteGlobal( false );
}

//api
void CDfm::SetFailLimitGlobal(uint limit)
{
	m_nFailLimitGlobal = limit;
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_FM_FAIL_LIMIT, limit );
}

uint CDfm::GetFailLimitGlobal()
{
	return m_nFailLimitGlobal;
}

//api
void CDfm::SetFailMultiIoGlobal(uint multio)
{
	m_nFailMultiIo = multio;
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_FM_MULTI_IO, multio );
}

uint CDfm::GetFailMultiIoGlobal()
{
	return m_nFailMultiIo;
}

bool CDfm::ReadExtFm(uint fc)
{
	ClearFmPointer(fc);
	uint64_t fc_dfm_address = FC1_DFM_STA_ADDR;
	if( fc == FC00 ) fc_dfm_address  = FC1_DFM_STA_ADDR;
	else             fc_dfm_address  = FC2_DFM_STA_ADDR;

	m_nFmFailCount[fc] = ReadExtFmFcnt(fc);
	int nExtramCount   = m_nFmFailCount[fc] % FM_READ_MINIMUM_COUNT;						//ex)191, mod:3
	int nCreateCount   = m_nFmFailCount[fc] + (FM_READ_MINIMUM_COUNT - nExtramCount);		//ex)191 + (4-3)

	if ( m_nFmFailCount[fc] < 1 ) return true;	//2023.01.02 - KWMOON

	m_pFmMemory[fc] = new FM_MEM[ nCreateCount ];
	char* pDfmData = (char*)m_pFmMemory[fc];

	int loop_count      = nCreateCount / ONCE_FM_WR_COUNT;
	int remain_count    = nCreateCount - ( loop_count * ONCE_FM_WR_COUNT );
	int extra_read_size = remain_count * sizeof(FM_MEM);

	if( 0 ) g_log.Printf( "READ_FM FC%d FailCount:%d Loop:%d(Count:%d), ExtraCount:%d\n", fc, m_nFmFailCount[fc], loop_count, DMA_WR_MAX_SIZE, remain_count );
	for( int n=0;n<loop_count;n++ )
	{
		//fprintf( stdout, "FC%d %03d READ FM ADDRESS:0x%lX : ReadSize:%d\n", fc, n, fc_dfm_address, DMA_WR_MAX_SIZE );
		g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
		g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , pDfmData, DMA_WR_MAX_SIZE       );
		pDfmData       += DMA_WR_MAX_SIZE;
		fc_dfm_address += DMA_WR_MAX_SIZE;
	}
	if( remain_count > 0 )
	{
		//fprintf( stdout, "FC%d EXTRA READ FM ADDRESS:0x%lX : ReadSize:%d\n", fc, fc_dfm_address, extra_read_size );
		g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
		g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , (char*)pDfmData, extra_read_size         );
	}
	return true;
}

bool CDfm::ReadExtFm()
{
	bool bOk = true;
	ClearFmPointer();
	uint64_t fc_dfm_address = FC1_DFM_STA_ADDR;
	for( int nFc=FC00;nFc<USE_FC;nFc++ )
	{
		m_nFmFailCount[nFc] = ReadExtFmFcnt(nFc);
		int nExtramCount = m_nFmFailCount[nFc] % FM_READ_MINIMUM_COUNT;						//ex)191%4, mod:3
		int nCreateCount = m_nFmFailCount[nFc] + (FM_READ_MINIMUM_COUNT - nExtramCount);	//ex)191 + (4-3) = 192

		if ( m_nFmFailCount[nFc] < 1 ) continue;	//2023.01.02 KWMOON

		m_pFmMemory[nFc] = new FM_MEM[ nCreateCount ];
		char* pDfmData = (char*)m_pFmMemory[nFc];

		if( nFc == FC00 ) fc_dfm_address  = FC1_DFM_STA_ADDR;
		else              fc_dfm_address  = FC2_DFM_STA_ADDR;

		int loop_count      = nCreateCount / ONCE_FM_WR_COUNT;
		int remain_count    = nCreateCount - ( loop_count * ONCE_FM_WR_COUNT );
		int extra_read_size = remain_count * sizeof(FM_MEM);

		if( 0 ) g_log.Printf( "FC%d READ_FM Loop:%d(Count:%d), ExtraCount:%d", nFc, loop_count, DMA_WR_MAX_SIZE, remain_count );
		for( int n=0;n<loop_count;n++ )
		{
			if( 0 ) g_log.Printf( "FC%d %03d READ FM ADDRESS:0x%lX : ReadSize:%d", nFc, n, fc_dfm_address, DMA_WR_MAX_SIZE );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
			if( !g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , pDfmData, DMA_WR_MAX_SIZE ) ) bOk = false;
			pDfmData       += DMA_WR_MAX_SIZE;
			fc_dfm_address += DMA_WR_MAX_SIZE;
		}
		if( remain_count > 0 )
		{
			if( 0 ) g_log.Printf( "FC%d EXTRA READ FM ADDRESS:0x%lX : ReadSize:%d", nFc, fc_dfm_address, extra_read_size );
			g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
			if( !g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , (char*)pDfmData, extra_read_size ) ) bOk = false;
		}
	}
	return bOk;
}

//2022.12.21 - kwmoon(compress mode)
bool CDfm::ReadExtFmComp()
{
	bool bOk = true;
	ClearFmPointer();
	uint64_t comp_fm_address = FC1_DFM_STA_ADDR;
	uint fcnt_gain = ReadExtFmCompFcnt() * FM_COMP_MODE_GAIN;
	uint fcnt_real = ReadExtFmCompFcnt();
//	int nExtramCount = fcnt_gain % FM_READ_MINIMUM_COUNT;						//2023.01.02 - kwmoon(comment)
	int nCreateCount = fcnt_gain; // + (FM_READ_MINIMUM_COUNT - nExtramCount);	//ex)191 + (4-3) = 192

	if ( fcnt_real < 1 ) return true;		//2023.01.02 - kwmoon

	FM_MEM* tmpFm    = new FM_MEM[ nCreateCount ];
	char* pDfmData   = (char*)tmpFm;

	int loop_count      = nCreateCount / ONCE_FM_WR_COUNT;
	int remain_count    = nCreateCount - ( loop_count * ONCE_FM_WR_COUNT );
	int extra_read_size = remain_count * sizeof(FM_MEM);

	if( 0 ) g_log.Printf( "READ_COMP_FM Loop:%d(Count:%d), ExtraCount:%d", loop_count, DMA_WR_MAX_SIZE, remain_count );
	for( int n=0;n<loop_count;n++ )
	{
		if( 0 ) g_log.Printf( "READ_COMP_FM CNT:%05d ADDRESS:0x%lX : ReadSize:%d", n, comp_fm_address, DMA_WR_MAX_SIZE );
		g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&comp_fm_address, sizeof(uint64_t) );
		if( !g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , pDfmData, DMA_WR_MAX_SIZE ) ) bOk = false;
		pDfmData        += DMA_WR_MAX_SIZE;
		comp_fm_address += DMA_WR_MAX_SIZE;
	}
	if( remain_count > 0 )
	{
		if( 0 ) g_log.Printf( "EXTRA READ_COMP_FM ADDRESS:0x%lX : ReadSize:%d", comp_fm_address, extra_read_size );
		g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&comp_fm_address, sizeof(uint64_t) );
		if( !g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , (char*)pDfmData, extra_read_size ) ) bOk = false;
	}

//	int nCompExtra  = (fcnt_real/FM_COMP_MODE_GAIN) % FM_READ_MINIMUM_COUNT;	//2023.01.02 - kwmoon(comment)
	int nCompCreate = fcnt_real;	// + (FM_READ_MINIMUM_COUNT - nCompExtra);	//ex)191 + (4-3) = 192
	m_pFmMemory[FC00] = new FM_MEM[ nCompCreate ];
	m_pFmMemory[FC01] = new FM_MEM[ nCompCreate ];
	uint offset = 0x0;
	for( int i=0;i<nCreateCount;i+=4 )
	{
//		if( i< 128 )
//		{
//			PrintCompExtFm( FC00, &tmpFm[i+0], i );
//			PrintCompExtFm( FC01, &tmpFm[i+1], i );
//		}
		m_pFmMemory[FC00][offset] = tmpFm[i+0];
		m_pFmMemory[FC01][offset] = tmpFm[i+1];
		offset++;
	}
	if( tmpFm!=nullptr ) { delete[] tmpFm; tmpFm=nullptr; }

	return bOk;
}

uint CDfm::GetFmModeGlobal()
{
	return m_FmMode;
}

//api
uint CDfm::ReadExtFmFcnt(uint fc)
{
	if( fc<FC00 || fc>FC01 )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_DFM, "FCNT", "FC%d is invalid, use %d ~%d", fc, FC00, FC01 );
		return 0;
	}

	uint failcount = 0x0;
	if( fc == FC00 )
		failcount = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_FC1_FAIL_COUNT );
	else
		failcount = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_FC2_FAIL_COUNT );
	//g_log.Printf( DEV_LOG_TYPE::_DFM, "FCNT", "FC%d fail count : %d", fc, failcount );
	m_nFmFailCount[fc] = failcount;
	return failcount;
}

//2022.12.21 - kwmoon
uint CDfm::ReadExtFmCompFcnt()
{
	uint failcount = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_FC12_FAIL_COUNT );
	//failcount *= FM_COMP_MODE_GAIN;
	//g_log.Printf( DEV_LOG_TYPE::_DFM, "COMP_FCNT", "FC%d fail count : %d", fc, failcount );
	m_nFmCompFailCount[FC00] = failcount;
	m_nFmCompFailCount[FC01] = failcount;
	return failcount;
}

//program api
bool CDfm::ReadExtFmData( uint fc, uint failth, unsigned long long data[4])
{
	bool bOk = true;
	uint index = failth;
	uint mem_index  = index / FM_READ_MINIMUM_COUNT;	//0,1,2,3:0  4,5,6,7:1 , 8,9,10,11:2
	uint mem_offset = index % FM_READ_MINIMUM_COUNT;	//0:0, 1:1, ..., 4:0, 5:1, ..., 8:0, 9:1
	uint64_t fc_dfm_address = FC1_DFM_STA_ADDR;

	FM_MEM mem[FM_READ_MINIMUM_COUNT];
	uint read_size = sizeof(FM_MEM) * FM_READ_MINIMUM_COUNT;
	memset( mem, 0x0, sizeof(mem) );

	if( fc == FC00 ) fc_dfm_address  = FC1_DFM_STA_ADDR + ( mem_index*read_size );	//16BYTE(128bit) * 4
	else             fc_dfm_address  = FC2_DFM_STA_ADDR + ( mem_index*read_size );	//16BYTE(128bit) * 4

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x1 );	//0:DBM, 1:DFM
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_SAVE_EN  , 0x0 );	//0:DISABLE, 1:ENABLE
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//0:Write Operation, 1:Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//0:DMA WRITE, 1:DFM WRITE
//kyb	g_sys.SysDelay(100 US);
	g_sys.SysDelay(1 US);

	g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&fc_dfm_address, sizeof(uint64_t) );
	if( !g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA, (char*)mem, read_size ) )
	{
		bOk = false;
	}
	else
	{
		data[0] = mem[mem_offset].header.header;
		data[1] = mem[mem_offset].data[0].ioab;
		data[2] = mem[mem_offset].data[1].ioab;
		data[3] = mem[mem_offset].data[2].ioab;
	}
	return bOk;
}

FM_MEM* CDfm::GetFmFailMemory(uint fc)
{
	if( fc > (USE_FC-1) ) return nullptr;
	return m_pFmMemory[fc];
}

void CDfm::PrintExtFm(uint fc, uint limit)
{
	FM_MEM* pFmMem = m_pFmMemory[fc];
//	uint nCount = limit;
	uint nCount = m_nFmFailCount[fc];
	//if( nCount > 8192 ) nCount = 8192;
//	g_log.Printf( "PRINTF_EXTFM::FC[%d] FAIL_COUNT:%d", fc, limit );
	g_log.Printf( "PRINTF_EXTFM::FC:%d READ_EXTFM_FCNT():%d\n", fc, nCount*4 );	//*4??

	if ( nCount>limit )
		nCount = limit;

	if ( pFmMem==nullptr ) return;

	for( uint i=0;i<nCount;i++ )
	{
//		if( pFmMem!=nullptr )
		{
//			int iogrp[3] = { 0x0, };
//			for( int grp=0;grp<3;grp++ )
//			{
//				iogrp[grp] = pFmMem->data[grp].ioab;
//			}

//			g_log.Printf( "PRINTF_EXTFM::FC[%d] SCAN:%02d ADDR:%06X", fc, pFmMem->header.scan, pFmMem->header.address );
//			g_log.Printf( "EXT_FM_ADD[0x%06X]=2:0x%08X 1:0x%08X 0:0x%08X" , i, iogrp[2], iogrp[1], iogrp[0] );

			//PRINTF_EXTFM::FC[0] EXT_FM_ADD[0x00000000] [Scan:0x03 Addr:0x003000] ,[0x01000000] ,[0x00000000] ,[0x00000000]

			g_log.Printf( "PRINTF_EXTFM::FC[%d] EXT_FM_ADD[0x%08d] [Scan:0x%02x Addr:0x%06x] ,[0x%08x] ,[0x%08x] ,[0x%08x]\n",
						  fc, i, pFmMem->header.scan, pFmMem->header.address, pFmMem->data[2].ioab, pFmMem->data[1].ioab, pFmMem->data[0].ioab );
		}
		pFmMem++;
	}
}

void CDfm::PrintCompExtFm(uint fc, FM_MEM* tmpFm, uint index)
{
	if( tmpFm!=nullptr )
	{
		int iogrp[3] = { 0x0, };
		for( int grp=0;grp<3;grp++ )
		{
			iogrp[grp] = tmpFm->data[grp].ioab;
		}
//		if ( iogrp[0]==0 && iogrp[1]==0 && iogrp[2]==0 )
//			return;

		g_log.Printf( "COMP_EXTFM::FC[%d] SCAN:%02d ADDR:%06X", fc, tmpFm->header.scan, tmpFm->header.address );
		g_log.Printf( "COMP_FM_ADD[0x%06X]=2:0x%08X 1:0x%08X 0:0x%08X" , index, iogrp[2], iogrp[1], iogrp[0] );
	}
}
