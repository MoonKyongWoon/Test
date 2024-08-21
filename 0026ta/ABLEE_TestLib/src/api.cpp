#include <QThread>
#include "api.hpp"
#include "CompType.h"

CPcie   g_pcie;
CDfm    g_dfm;
CBib    g_bib;
CDbm    g_dbm;
CDle    g_dle;
CLog    g_log;
CHw     g_hw;
CPg     g_pg;
CPps    g_pps;
CTg     g_tg;
CUbm    g_ubm;
CSys    g_sys;
CVector g_vector;
CDevSocket  g_devsock;	//2023.01.19
CDevSocket  g_logsock;	//2023.08.13

CApi::CApi()
{
//	DCI_OFFSET = 0.0;
	DCI_USE  = true;
	DCI_IO   = true;
	DCI_UCLK = true;
	m_FCAdd = 0 ;
	m_fail_mode = 0 ;
	dut_per_cpu = 0 ;
	m_set_step_count = 0 ;
	m_set_merge_type = 0 ;
	m_bib_x = 0 ;
	m_bib_y = 0 ;
	m_nchip = 0 ;
	m_scan = m_bibScan = 0 ;
	m_scan_invisible = 0 ;
	m_block_shift = 0 ;
	m_block_limit = 0 ;
	_CurrentCpuNo = 0;
	m_use_mode = 0;

	m_lmtBlkPerDut    = 0;
	m_lmtBlkPerCe     = 0;
	m_lmtBlkPerCeStep = 0;

	m_lmtAddFail        = 1;

	m_addBlkShift     = 0;
	m_addPageShift    = 0;
	m_addColSize      = 0;
	m_addMaxCe        = 0;
	m_addMaxChip      = 0;

	m_nMaxDutPerCpu       = MAX_DUT;
	m_nCePerDut           = 0;
	m_nChipPerCe          = 0;
	m_nBlockPerCe         = 0;
	m_nBlockPerChip       = 0;
	m_nBlockPerDut        = 0;
	m_nBlockPerDutBl      = 0;
	m_nChipPerDut         = 0;
	m_nChipPerDutBl       = 0;

	m_iVarConf            = 0;
	m_bVarAvail           = false;
	m_now_step_fail_block = NULL;
	m_cum_fail_block      = NULL;
	m_init_fail_block     = NULL;
	m_get_step_fail_block = NULL;
	m_step_fail_chip      = NULL;
	m_cum_fail_chip       = NULL;
	m_ext_fm_fail         = NULL;
	m_ext_fm_fail_count   = NULL;
	m_pStChipId           = NULL;
	m_cum_exclusive       = false;

	m_nBiTime    = 0;
	m_nAgingTime = 0;
	m_nFaultBin  = 9;
	memset( m_szPc, 0, sizeof(m_szPc) );

	m_bReadNandChipId = false;
	m_nFmReadDataIndex = 0x0;
	memset( _bi_find, 0x0, sizeof(_bi_find) );
	_bi_find[0].read = true;	//2023-01-13 fatric.

	m_debug_mode  = 0;
	m_bBlockFailAddr  = false;
	m_bInitStep       = false;
	m_bProcExtFmBlock = false;	//2023.05.12 - fatric

	m_ioabboth = false;

	//2022.02.20 - KWMOON
	m_nStepNo = 0;
	memset( &m_stMakeItem, 0x0, sizeof(stTestItem) );
	memset( &m_stTestItem, 0x0, sizeof(stTestItem) );
	memset( &m_stCommonData, 0, sizeof(stCommonData) );

	m_iTsNo = 0;

	SET_RUNMODE( false );
	MAKEITEM = false;
	JUMPMODE = false;
	TESTDONE = true;
	SET_TESTDONE( true );	//2023.03.23 - KWMOON(false -> true )
	SET_STOPMODE( true );	//2023.05.22 - KWMOON
	TERMINATED = false;	//2023.04.03 - KWMOON

	//2022.03.08 - KWMOON
	memset( &m_stPgmVariables, 0x0, sizeof(stDevPgmVariables) );
	m_stUiInitInfo.ui_automode  = false;
	m_stUiInitInfo.is_set = false;
	m_stUiInitInfo.stBid.nBibId  = 0x0;
	memset( m_stUiInitInfo.stBid.szBibId, 0x0, sizeof(m_stUiInitInfo.stBid.szBibId) );

	m_stUiTestInfo.is_set = false;
	m_stUiTestInfo.ui_chamberuse = false;

	//2023.03.16 - KWMOON
	m_nCurrentStepNo = 0;
	//2023.04.21 - fatric
	m_iPpsStat        = 2;
	m_iPinOutDrStat   = 2;
	m_iPinOutUclkStat = 2;

	//2023.05.25 - fatric
	for(int i =  0; i<10                ; ++i) m_bin_map  [i] = i;
	for(int i = 10; i<ASIZE(m_bin_map  ); ++i) m_bin_map  [i] = 1;
	for(int i =  0; i<ASIZE(m_bin_limit); ++i) m_bin_limit[i] = 100.0;
}

CApi::~CApi()
{
	if ( m_pStChipId ) { delete [] m_pStChipId; m_pStChipId = NULL; }
	delVars();
	DisconnectToEwsMgr();       //2023.06.02 - kwmoon
	printf("%s Terminated....\n", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SYSTEM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SetProgramName(const char* pszName)	//2023.05.03 - kwmoon
{
	//snprintf( m_szProgramName, sizeof(m_stPgmVariables.szProgram), "%s", pszName );	//2023.05.03 - kwmoon
}

void CApi::SetAutoMode(bool mode)	//2023.01.19 - Auto/Manual Mode
{
	g_sys.SetAutoMode( mode );
	if( g_sys.GetAutoMode() )
	{
		if( g_devsock.IsConn() ) return;
		if( g_logsock.IsConn() ) return;	//2023.08.13 - KWMOON
	}
}

bool CApi::GetAutoMode()	//2022.02.20 - Auto/Manual Mode
{
	return g_sys.GetAutoMode();
}

//2023.04.03 - kwmoon
bool CApi::SetToolPgm(bool bTool)
{
	g_devsock.SetDevice( bTool, EN_DEV_DATA_SOCKET );
	g_logsock.SetDevice( bTool, EN_DEV_LOG_SOCKET  );
	return true;
}

bool CApi::ConnectToEwsMgr()
{
	//PRINTF( "%s TOOL : %d\n", __FUNCTION__, bTool );
	//g_devsock.SetDevice( bTool );
	bool bData = g_devsock.ConnectToHost();
	bool bLog  = g_logsock.ConnectToHost();
	return ( bData && bLog );
}

//2023.04.03 - kwmoon
bool CApi::DisconnectToEwsMgr()
{
	bool bData = g_devsock.CloseSocket();
	bool bLog  = g_logsock.CloseSocket();
	return ( bData && bLog );
}


bool CApi::HwOpen(int slot, int cpuno)
{
	if( cpuno == CPU00 )
	{
		g_sys.SetCurrentSlotNo( slot-1 );
		g_sys.SetCurrentCpuNo ( CPU00  );
		g_pcie.SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
	}
	else
	{
		g_sys.SetCurrentSlotNo( slot-1 );
		g_sys.SetCurrentCpuNo ( CPU01  );
		g_pcie.SetPcieTPG( PCIE_DEVICE::PCIE_TPG1 );
	}
	m_stCommonData.iSlot = slot;
	return g_pcie.pcie_open();
}

bool CApi::HwInit( bool bInit )
{
	PRINTF( "HwInit" );
	g_sys.SetPfmStop();	//2023.07.25 - KWMOON(Fix Stop)
	g_hw.HwInit();
	g_pps.PpsInit();
	SET_VS_OFF( S1PS );	//2023.05.22 - KWMOON
	SET_VS_OFF( PPS  );	//2023.05.22 - KWMOON
	return true;
}

bool CApi::SwInit( bool bInit )
{
	PRINTF( "SwInit" );
	//SwInit
	g_pg.InitPg();
	g_tg.InitTg();
	//CLR_AUTOMASK_MEM_GLOBAL();
	CLR_SCANMASK();
	CLR_ALLREG_GLOBAL();
	CLR_FM_GLOBAL();
	SET_IOMASK_GLOBAL(0,0,0);
	g_sys.SetPfmIniInit();		//2023.08.02 - KWMOON
	g_sys.SetPfmFalultClear();	//2023.08.02 - KWMOON
	return true;
}

bool CApi::SysWrite(uint32_t addr, uint32_t data )
{
	return g_pcie.write_user( addr, data);
}

int  CApi::SysRead(uint32_t addr)
{
	return g_pcie.read_user( addr );
}

bool CApi::sysWrite(uint32_t addr, uint32_t data )
{
	return SysWrite( addr, data);
}

int  CApi::sysRead(uint32_t addr)
{
	return SysRead( addr );
}

int  CApi::Read(uint32_t addr)
{
	return SysRead( addr );
}

bool CApi::SysWrites( uint32_t addr, char* pwdata, uint32_t size  )
{
	return g_pcie.write_dma( addr, pwdata, size );
}

bool CApi::SysReads ( uint32_t addr, char* prdata, uint32_t size )
{
	return g_pcie.read_dma( addr, prdata, size );
}

bool CApi::GET_BIBID(std::string& sBibId)
{
	bool bOk = true;
	std::string type;
	int no = 0x0;

	//2023.03.20 - KWMOON
	if( m_stUiInitInfo.is_set )
	{
		sBibId = string( m_stUiInitInfo.stBid.szBibId );
	}
	else
	{
		if( !g_bib.GetBibId( sBibId, type, no ) )
		{
			bOk = false;
			EPRINTF( "BIB_ID(%s) Parity Error!", sBibId.c_str() );
		}
	}
	return bOk;
}

bool CApi::GET_BIBID(std::string& sBibId, std::string& sBibType, int& rNo)
{
	bool bOk = true;
	//2023.03.20 - KWMOON
	if( m_stUiInitInfo.is_set )
	{
		char szType[8] = {0x0,};
		sBibId = string( m_stUiInitInfo.stBid.szBibId );
		rNo    = m_stUiInitInfo.stBid.nBibId;
		snprintf( szType, sizeof(szType), "%03XT", (rNo>>16)&0xFFF  );
		sBibType = string( szType );
	}
	else
	{
		if( !g_bib.GetBibId( sBibId, sBibType, rNo ) )
		{
			bOk = false;
			EPRINTF( "BIB_ID(%s) Parity Error!", sBibId.c_str() );
		}
	}
	return bOk;
}

int CApi::GET_BIBID()
{
	//2023.07.06 - kwmoon( m_stUiTestInfo.is_set -> m_stUiInitInfo.is_set change )
	if( m_stUiInitInfo.is_set )
	{
		m_stCommonData.nBibId = m_stUiInitInfo.stBid.nBibId;
	}
	else
	{
		m_stCommonData.nBibId = g_bib.GetBibId();
	}
//	PRINTF("BIB_ID=%03X%05d", (m_stCommonData.nBibId>>16)&0xffff, m_stCommonData.nBibId&0xffff);
	return m_stCommonData.nBibId;
}

//START_DEVICE( s )
void CApi::SetDeviceProgram(std::string s)
{
	m_sDeviceProgram = s;
	//2023.03.08 - KWMOON
	snprintf( m_stPgmVariables.szDevice, sizeof(m_stPgmVariables.szDevice), "%s", s.c_str() );
}

//2023.03.08 - KWMOON
bool CApi::IS_SET_VARIABLES()
{
	if( m_bVarAvail ) return true;
	else
	{
		EPRINTF( "SET_BIB() / SET_CE() / SET_LIMIT() setting please!", __FUNCTION__ );
		return false;
	}
}

void CApi::initVars()
{
	if ( m_iVarConf != 0x7 ) return;

	m_nBlockPerCe    = m_lmtBlkPerDut / m_nCePerDut;
	m_nBlockPerDut   = m_lmtBlkPerDut;
	m_nBlockPerDutBl = m_nBlockPerDut / 8 + 1;	//char 1byte -> 8bit(8ea block)
	m_nChipPerDut    = m_nCePerDut    * m_nChipPerCe;
	m_nChipPerDutBl  = m_nChipPerDut / 8 + 1;	//char 1byte -> 8bit(8ea block)

	m_init_fail           = new char[ m_nMaxDutPerCpu                  ];

	m_now_step_fail_block = new char[ m_nMaxDutPerCpu * m_nBlockPerDutBl ];
	m_cum_fail_block      = new char[ m_nMaxDutPerCpu * m_nBlockPerDutBl ];
	m_init_fail_block     = new char[ m_nMaxDutPerCpu * m_nBlockPerDutBl ];
	m_get_step_fail_block = new char[ m_nMaxDutPerCpu * m_nBlockPerDutBl ];
	m_step_fail_chip      = new char[ m_nMaxDutPerCpu * m_nChipPerDutBl  ];
	m_cum_fail_chip       = new char[ m_nMaxDutPerCpu * m_nChipPerDutBl  ];

	m_test_fail           = new char[ m_nMaxDutPerCpu                  ];
	m_step_fail           = new char[ m_nMaxDutPerCpu                  ];
	m_cum_fail            = new char[ m_nMaxDutPerCpu                  ];

/////////////////////////////////////////////////////////////////////////
/// Modify by fatric 2022-11-19
//	m_fail_cate           = new uint[ m_nMaxDutPerCpu * MAX_CATEGORY   ];
//	m_pass_cate           = new uint[ m_nMaxDutPerCpu * MAX_CATEGORY   ];
	m_nCatePerDut         = MAX_CATEGORY_BIT_L;
	m_fail_cate           = new char[ m_nMaxDutPerCpu * m_nCatePerDut  ];
	m_pass_cate           = new char[ m_nMaxDutPerCpu * m_nCatePerDut  ];
/////////////////////////////////////////////////////////////////////////
	m_bin                 = new uint[ m_nMaxDutPerCpu                  ];
	m_step_fail_cate      = new uint[ m_nMaxDutPerCpu                  ];	//2024.07.22 fatric
	m_step_pass_cate      = new uint[ m_nMaxDutPerCpu                  ];	//2024.07.22 fatric
	m_step_fail_block_cnt = new uint[ m_nMaxDutPerCpu * m_nCePerDut    ];	//2023.07.12 - kwmoon

	memset( m_init_fail          , 0, sizeof(char) * m_nMaxDutPerCpu                    );

	memset( m_now_step_fail_block, 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
	memset( m_cum_fail_block     , 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
	memset( m_init_fail_block    , 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
	memset( m_get_step_fail_block, 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
	memset( m_step_fail_chip     , 0, sizeof(char) * m_nMaxDutPerCpu * m_nChipPerDutBl  );
	memset( m_cum_fail_chip      , 0, sizeof(char) * m_nMaxDutPerCpu * m_nChipPerDutBl  );

	memset( m_test_fail          , 0, sizeof(char) * m_nMaxDutPerCpu                    );
	memset( m_step_fail          , 0, sizeof(char) * m_nMaxDutPerCpu                    );
	memset( m_cum_fail           , 0, sizeof(char) * m_nMaxDutPerCpu                    );

	memset( m_fail_cate          , 0, sizeof(char) * m_nMaxDutPerCpu * m_nCatePerDut    );
	memset( m_pass_cate          , 0, sizeof(char) * m_nMaxDutPerCpu * m_nCatePerDut    );
//	memset( m_bin                , 0, sizeof(uint) * m_nMaxDutPerCpu                    );
	memset( m_step_fail_block_cnt, 0, sizeof(uint) * m_nMaxDutPerCpu * m_nCePerDut      );	//2023.07.12 - kwmoon
	for(uint i=0; i<m_nMaxDutPerCpu; ++i)
		m_bin[i] = 1;
	memset( m_step_fail_cate     , 0, sizeof(uint) * m_nMaxDutPerCpu                    );	//2024.07.22 fatric
	memset( m_step_pass_cate     , 0, sizeof(uint) * m_nMaxDutPerCpu                    );	//2024.07.22 fatric

	for(int i=0; i<256; ++i )
	{
		int nCnt = 0;
		for (int iBit=0; iBit<8; ++iBit)
		{
			if ( (i>>iBit)&1 )
				++nCnt;
		}
		m_nBitCnt[i] = nCnt;
	}

	for(int i=0; i<256; ++i )
	{
		int nPos = 0;
		for (int iBit=0; iBit<8; ++iBit)
		{
			if ( (i>>iBit)&1 )
			{
				nPos = iBit;
				break;
			}
		}
		m_nBitPos[i] = nPos;
	}

////////////////////////////////////////////////////////////////////////
////EXTFM
////////////////////////////////////////////////////////////////////////
//	m_ext_fm_fail = new uint*[m_nMaxDutPerCpu];
//	for( uint d=0;d<m_nMaxDutPerCpu;d++ )
	m_ext_fm_fail = new uint*[m_nMaxDutPerCpu*2];
	for( uint d=0;d<m_nMaxDutPerCpu*2;d++ )
	{
		m_ext_fm_fail[d] = new uint[EXTFM_SW_MAX_LIMIT];
		memset( m_ext_fm_fail[d], 0x0, sizeof(int)*EXTFM_SW_MAX_LIMIT );
	}
	m_ext_fm_fail_count = new uint[m_nMaxDutPerCpu];
	memset( m_ext_fm_fail_count, 0x0, sizeof(int)*m_nMaxDutPerCpu );

////////////////////////////////////////////////////////////////////////
////for compile & match for reference pgm
////////////////////////////////////////////////////////////////////////
	dut_per_fc = g_bib.GetDutPerFc();			//2022.12.02 - KWMOON
	dut_per_cpu= g_bib.GetDutPerCpu();			//2022.12.02 - KWMOON
	m_ioab_cnt[0][0] = g_bib.GetDutPerIoAb();	//2022.12.02 - KWMOON

////////////////////////////////////////////////////////////////////////
////device program variable to ews
/// 2023.03.08 - KWMOON
////////////////////////////////////////////////////////////////////////
	//snprintf( m_stPgmVariables.szProgram, sizeof(m_stPgmVariables.szProgram), "%s", m_szProgramName );	//2023.05.03 - kwmoon
	m_stPgmVariables.nMaxDutPerCpu = m_nMaxDutPerCpu;
	m_stPgmVariables.nBibX = g_bib.GetBibX();
	m_stPgmVariables.nBibY = g_bib.GetBibY();
	m_stPgmVariables.nCe   = g_bib.GetCeCount();
	m_stPgmVariables.nChip = g_bib.GetChipCount();
	m_stPgmVariables.nBlockPerCe = m_nBlockPerCe;
	m_stPgmVariables.nBlockPerDut = m_nBlockPerDut;
	m_stPgmVariables.nBlockPerDutBl = m_nBlockPerDutBl;
	m_stPgmVariables.nChipPerDut = m_nChipPerDut;
	m_stPgmVariables.nChipPerDutBl = m_nChipPerDutBl;
	m_stPgmVariables.nDutPerFc = dut_per_fc;
	m_stPgmVariables.nDutPerCpu = dut_per_cpu;
	m_stPgmVariables.nBlockShift = m_block_shift;
	m_stPgmVariables.nPageShift = m_addPageShift;
	m_stPgmVariables.nColSize = m_addColSize;
	m_stPgmVariables.nblockLimit = m_lmtBlkPerDut;
	m_stPgmVariables.nblockLimitPerCe = m_lmtBlkPerCe;
	m_stPgmVariables.nAddrLimit = m_lmtAddFail;
	m_stPgmVariables.nFailMode = m_fail_mode;
	m_stPgmVariables.nScanInvisible = m_scan_invisible;
	m_stPgmVariables.nBibScan = m_bibScan;
	m_stPgmVariables.nIoSize = m_bibIo;
	m_stPgmVariables.nCpu2X  = m_bibCpu;
	m_stPgmVariables.nMaxCategoryBit = MAX_CATEGORY_BIT_L;

	m_bVarAvail = true;

}

//2023.06.11 - KWMOON
void CApi::SYS_SEND_DEVPGM_VARS()
{
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_DEV_PGM_VARIABLE, (char*)&m_stPgmVariables, sizeof(stDevPgmVariables) ) )
		{
			EPRINTF("DEV PGM VARIABLES SEND ERROR!" );
		}
		//sysDelay( 500 MS );
	}
	//PRINTF( "%s done", __FUNCTION__ );
}

void CApi::delVars()
{
	if ( m_init_fail           ) { delete [] m_init_fail          ; m_init_fail = NULL; }

	if ( m_now_step_fail_block ) { delete [] m_now_step_fail_block; m_now_step_fail_block = NULL; }
	if ( m_cum_fail_block      ) { delete [] m_cum_fail_block     ; m_cum_fail_block      = NULL; }
	if ( m_init_fail_block     ) { delete [] m_init_fail_block    ; m_init_fail_block     = NULL; }
	if ( m_get_step_fail_block ) { delete [] m_get_step_fail_block; m_get_step_fail_block = NULL; }
	if ( m_step_fail_chip      ) { delete [] m_step_fail_chip     ; m_step_fail_chip      = NULL; }
	if ( m_cum_fail_chip       ) { delete [] m_cum_fail_chip      ; m_cum_fail_chip       = NULL; }

	if ( m_test_fail           ) { delete [] m_test_fail          ; m_test_fail           = NULL; }
	if ( m_step_fail           ) { delete [] m_step_fail          ; m_step_fail           = NULL; }
	if ( m_cum_fail            ) { delete [] m_cum_fail           ; m_cum_fail            = NULL; }

	if ( m_fail_cate           ) { delete [] m_fail_cate          ; m_fail_cate           = NULL; }
	if ( m_pass_cate           ) { delete [] m_pass_cate          ; m_pass_cate           = NULL; }
	if ( m_bin                 ) { delete [] m_bin                ; m_bin                 = NULL; }
	if ( m_step_fail_cate      ) { delete [] m_step_fail_cate     ; m_step_fail_cate      = NULL; }		//2024.07.22 fatric
	if ( m_step_pass_cate      ) { delete [] m_step_pass_cate     ; m_step_pass_cate      = NULL; }		//2024.07.22 fatric
	if ( m_step_fail_block_cnt ) { delete [] m_step_fail_block_cnt; m_step_fail_block_cnt = NULL; }		//2023.07.12 - kwmoon

	//EXT FM
	if( m_ext_fm_fail_count    ) { delete[] m_ext_fm_fail_count   ; m_ext_fm_fail_count   = NULL; }
	if( m_ext_fm_fail )
	{
//		for( uint d=0;d<m_nMaxDutPerCpu;d++ )
		for( uint d=0;d<m_nMaxDutPerCpu*2;d++ )
			delete []  m_ext_fm_fail[d];
		delete[] m_ext_fm_fail;
		m_ext_fm_fail = NULL;
	}
}

void CApi::clrStepVars()
{
	memset( m_now_step_fail_block, 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
	memset( m_step_fail_chip     , 0, sizeof(char) * m_nMaxDutPerCpu * m_nChipPerDutBl  );
	memset( m_step_fail          , 0, sizeof(char) * m_nMaxDutPerCpu                    );

	memset( m_step_fail_cate     , 0, sizeof(uint) * m_nMaxDutPerCpu                    );	//2024.07.22 fatric
	memset( m_step_pass_cate     , 0, sizeof(uint) * m_nMaxDutPerCpu                    );	//2024.07.22 fatric

	m_use_mode = 0;	//2024.08.18 fatric
}

void CApi::SetDevStatus( PROTOCOL::DEV_STATUS stat )
{
	if( !GET_RUNMODE() ) return;

	m_DevStat = stat;
	switch ( stat )
	{
		//2023.04.26 - KWMOON
		case PROTOCOL::DEV_STATUS::DEV_CYCLE_IN:
		{
			m_tpCycleBegin = std::chrono::steady_clock::now();
			m_stCommonData.tCycleBegin = time(NULL);
			if( g_sys.GetDevice() == DIAGCAL ) return;

			stCycleInfo cycleinfo;
			memset( (char*)&cycleinfo, 0x0, sizeof(stCycleInfo) );
			cycleinfo.bStart = true;
			cycleinfo.bEnd   = false;
			cycleinfo.tCycleBegin = m_stCommonData.tCycleBegin;
			cycleinfo.tCycleEnd   = 0x0;
			if( g_sys.GetAutoMode() )
			{
				g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CYCLE_IN_INFO, (char*)&cycleinfo, sizeof(stCycleInfo) );
			}
		}
		break;

		//2023.04.26 - KWMOON
		case PROTOCOL::DEV_STATUS::DEV_CYCLE_OUT:
		{
			m_tpCycleEnd = std::chrono::steady_clock::now();
			m_stCommonData.tCycleEnd = time(NULL);
			if( g_sys.GetDevice() == DIAGCAL ) return;

			stCycleInfo cycleinfo;
			memset( (char*)&cycleinfo, 0x0, sizeof(stCycleInfo) );
			cycleinfo.bStart = true;
			cycleinfo.bEnd   = true;
			cycleinfo.tCycleBegin = m_stCommonData.tCycleBegin;
			cycleinfo.tCycleEnd   = m_stCommonData.tCycleEnd;
			if( g_sys.GetAutoMode() )
			{
				g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CYCLE_OUT_INFO, (char*)&cycleinfo, sizeof(stCycleInfo) );
			}
		}
		break;

		//2023.04.24 - KWMOON
		case PROTOCOL::DEV_STATUS::DEV_STEP_IN:
		{
			m_tpStepBegin    = std::chrono::steady_clock::now();
			m_tStepBeginTime = time(NULL);

			//if( g_sys.GetDevice() == DIAGCAL ) return;
			if ( IsInitStep() ) return;

			stStepInInfo stepin;
			memset( (char*)&stepin, 0x0, sizeof(stStepInInfo) );
			stepin.nStepNo = m_nCurrentStepNo;
			snprintf(stepin.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );
			stepin.tStepBegin = time(NULL);
			if( g_sys.GetAutoMode() )
			{
				g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_STEP_IN_INFO, (char*)&stepin, sizeof(stStepInInfo) );
			}
		}
		break;

		//2023.04.24 - KWMOON
		case PROTOCOL::DEV_STATUS::DEV_STEP_OUT:
		{
			m_tStepEndTime = time(NULL);
			m_tpStepEnd    = std::chrono::steady_clock::now();
			//if( g_sys.GetDevice() == DIAGCAL ) return;

			if ( IsInitStep() ) return;
			double dPv = 0.0, dSv = 0.0;

			//2023.06.14 - KWMOON(ADD TEMP READ)
			PROTOCOL::PGM_CHMB_REQ_ACK ack = g_sys.ChamberReadTemp( dSv, dPv );
			if( (ack != PROTOCOL::PGM_CHMB_ACK ) && (ack != PROTOCOL::PGM_CHMB_TRUE ) )
			{
				dSv = m_stSetTemp.dTemp;
				dPv = 0.0;
			}
			stStepOutInfo stepout;
			memset( (char*)&stepout, 0x0, sizeof(stStepOutInfo) );
			stepout.tStepEnd = time(NULL);
			stepout.nStepNo  = m_nCurrentStepNo;
			snprintf(stepout.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );
			stepout.dVcc = 0.0;
			stepout.dBiTime = m_nBiTime;
			stepout.nTsNo = m_nCurrentStepNo;
			stepout.dTemp = dPv;
			//2023.06.14 - KWMOON(ADD EXT_FM_BLOCK CHECK)
			stepout.bProcExtFmBlock = m_bProcExtFmBlock;
			if ( strlen(m_szPc)<1 )
				strncpy( stepout.szPc, m_sStepName.c_str(), m_sStepName.length() );	//2023.04.24 - KWMOON(strcpy -> strncpy)
			else
				strncpy( stepout.szPc, m_szPc, strlen(m_szPc) );	//2023.04.24 - KWMOON(strcpy -> strncpy)

			if( g_sys.GetAutoMode() )
			{
				g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_STEP_OUT_INFO, (char*)&stepout, sizeof(stStepOutInfo) );
			}
		}
		break;

		default:
			break;
	}
}

PROTOCOL::DEV_STATUS CApi::GetDevStatus()
{
	return m_DevStat;
}

//GET_EXTFM_SCAN_ADDR -- scan & address duplicate check
bool CApi::IsDuplicateFailData(uint dut, uint address)
{
	for( uint i=0;i<GET_EXTFM_FCNT(dut);i++ )
	{
		uint faddr = m_ext_fm_fail[dut%m_nMaxDutPerCpu][i];
		if( address == faddr )
		{
			return true;
		}
	}
	return false;
}

void CApi::SET_UIINITINFO(stUiInitInfo initInfo)
{
	memcpy( &m_stUiInitInfo, &initInfo, sizeof(stUiInitInfo) );
	m_stUiInitInfo.is_set = true;
	if( 1 )
	{
		PRINTF( "[UI INITINFO]" );
		PRINTF( "BIBID    : %s", m_stUiInitInfo.stBid.szBibId );
		PRINTF( "UI MODE  : %s", m_stUiInitInfo.ui_automode?"AUTO" :"MANUAL" );
		PRINTF( "[/UI INITINFO]" );
		PRINTF(NULL);
	}
}

void CApi::SET_UITESTINFO(stUiTestInfo testInfo)
{
	memcpy( &m_stUiTestInfo, &testInfo, sizeof(stUiTestInfo) );
	m_stUiTestInfo.is_set = true;

	SET_UICHMBUSE( m_stUiTestInfo.ui_chamberuse );
	g_sys.SetChmbSoakTime( m_stUiTestInfo.chmb_soak_time );		//2023.07.09 - KWMOON
	g_sys.SetChmbKeepTime( m_stUiTestInfo.chmb_keep_time );		//2023.07.09 - KWMOON

	if( 0 )
	{
		PRINTF( "[UI TESTINFO]" );
		PRINTF( "CHMB USE : %s", g_sys.ChamberIsUse()?"YES":"NO" );
		PRINTF( "CHMB SOAK: %d min", g_sys.GetChmbSoakTime() );		//2023.07.09 - KWMOON
		PRINTF( "CHMB KEEP: %d min", g_sys.GetChmbKeepTime() );		//2023.07.09 - KWMOON
		PRINTF(NULL);
	}
}

void CApi::SET_UICHMBUSE(bool buse)	//2023.05.22 - kwmoon
{
	g_sys.ChamberSetUsage( buse );			//2023.05.22 - kwmoon
	if( 0 )
	{
		PRINTF( "CHMB USE : %s", buse?"YES":"NO" );
	}
}

void CApi::SET_TESTITEM(stTestItem items)
{
	memcpy( &m_stTestItem, &items, sizeof(stTestItem) );

	SET_UICHMBUSE( m_stTestItem.bChamberUse );					//2023.07.09 - KWMOON
	g_sys.SetChmbSoakTime( m_stTestItem.chmb_soak_time );		//2023.07.09 - KWMOON
	g_sys.SetChmbKeepTime( m_stTestItem.chmb_keep_time );		//2023.07.09 - KWMOON

	if( 1 )
	{
		PRINTF( "[JUMP TESTINFO]" );
		PRINTF( "CHMB USE : %s", g_sys.ChamberIsUse()?"YES":"NO" );
		PRINTF( "CHMB SOAK: %d min", g_sys.GetChmbSoakTime() );		//2023.07.09 - KWMOON
		PRINTF( "CHMB KEEP: %d min", g_sys.GetChmbKeepTime() );		//2023.07.09 - KWMOON
		if( 0 )
		{
			for( int i=0;i<m_stTestItem.nItemCount;i++ )
			{
				PRINTF( "STEP.%03d %-20s --- TEST[%c]",
						m_stTestItem.ItemStep[i].nStepNo+1, m_stTestItem.ItemStep[i].szItemName, m_stTestItem.ItemStep[i].bTestItem?'Y':'N' );
			}
			PRINTF(NULL);
		}
	}
}

void CApi::SET_MAKTEITEM(bool onoff)
{
	MAKEITEM = onoff;
	if( onoff )
	{
		m_nStepNo = 0;
		memset( &m_stMakeItem, 0x0, sizeof(stTestItem) );
	}
}

void CApi::SET_RUNMODE(bool onoff)
{
	RUNMODE = onoff;
	if( onoff )
		m_nStepNo = 0;
	g_sys.SetRunmode( onoff );	//2023.04.27
}

bool CApi::GET_RUNMODE()
{
	return RUNMODE;
}

void CApi::SET_JUMPMODE(bool onoff)
{
	JUMPMODE = onoff;
}

bool CApi::IS_TESTDONE()
{
	return TESTDONE;
}

void CApi::SET_TESTDONE(bool onoff)
{
	TESTDONE = onoff;
	if( onoff )
	{
		if( g_sys.GetAutoMode() )
		{
			if( !g_devsock.IsConn() )
				g_devsock.ConnectToHost();
			if( g_devsock.IsConn() )
			{
				PRINTF( "TEST_DONE" );
				g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_TEST_DONE, nullptr, 0x0 );
			}
		}
	}
}

//2023.05.22 - kwmoon
void CApi::SET_STOPMODE( bool bonoff )
{
	STOPMODE = bonoff;
}

//2023.05.22 - kwmoon
bool CApi::GET_STOPMODE()
{
	return STOPMODE;
}

//2023.04.03 - kwmoon
void CApi::SET_TERMINATED(bool term)
{
	PRINTF("%s",__FUNCTION__);
	TERMINATED = term;

	SET_MAKTEITEM( false );
	SET_RUNMODE  ( false );
	SET_JUMPMODE ( false );
}

//2023.04.03 - kwmoon
bool CApi::IS_TERMINATED()
{
	return TERMINATED;
}

//2023.02.24 - kwmoon (make item & runmode & jumpmode merge )
bool CApi::IS_TEST(char* stepname)
{
	//PRINTF( "%-10s  NO.%03d STEP:%30s", __FUNCTION__, m_nStepNo+1, stepname );
	m_sStepName = std::string(stepname);
	if( MAKEITEM )
	{
		if( m_nStepNo > MAX_STEP )
		{
			RUNTIME_ERROR( __FUNCTION__, "STEP COUNT LIMIT OVER %d", MAX_STEP );
			return false;
		}
		ADD_ITEM( m_nStepNo, m_sStepName.c_str() );
		m_nCurrentStepNo = m_nStepNo;
		m_nStepNo++;
		return false;
	}

	if( !GET_RUNMODE() )
	{
		if( !GetAutoMode() )
		{
			if( IS_TERMINATED() ) return false;
			return true;		//Manual Mode
		}
		return false;
	}
	else	//RUNMODE
	{
		bool is_test = true;
		if( JUMPMODE )
		{
			if( m_stTestItem.ItemStep[m_nStepNo].bTestItem )
			{
				//PRINTF( "%-10s  NO.%03d STEP:%-30s --- TEST", __FUNCTION__, m_nStepNo+1, m_stTestItem.ItemStep[m_nStepNo].szItemName );
				is_test = true;
			}
			else
			{
				//PRINTF( "%-10s  NO.%03d STEP:%-30s --- SKIP", __FUNCTION__, m_nStepNo+1, m_stTestItem.ItemStep[m_nStepNo].szItemName );
				is_test = false;
			}
		}
		m_nCurrentStepNo = m_nStepNo;
		m_nStepNo++;
		return is_test;
	}
	return false;
}

//2022.02.20 - kwmoon
bool CApi::ADD_ITEM(int stepno, const char * stepname )
{
	m_stMakeItem.nItemCount++;
	m_stMakeItem.ItemStep[stepno].bTestItem = false;
	m_stMakeItem.ItemStep[stepno].nStepNo   = stepno;
	strncpy( m_stMakeItem.ItemStep[stepno].szItemName, stepname, sizeof(m_stMakeItem.ItemStep[stepno].szItemName) );
	//PRINTF( "[ADD_ITEM] NO.%d NAME:%s\n", stepno, stepname );
	return true;
}

bool CApi::PRINT_ITEM()
{
	if( m_stMakeItem.nItemCount < 1 ) return false;

	PRINTF( "ITEM COUNT:%d", m_stMakeItem.nItemCount );
	for( int i=0;i<m_stMakeItem.nItemCount;i++ )
	{
		if( m_stMakeItem.bItemCategory )
			PRINTF( "CATEGORY:%-20s STEP NO.%03d STEP_NAME:%-30s",
					m_stMakeItem.ItemStep[i].szItemCategory, m_stMakeItem.ItemStep[i].nStepNo,m_stMakeItem.ItemStep[i].szItemName );
		else
			PRINTF( "STEP NO.%03d STEP_NAME:%-30s",
					m_stMakeItem.ItemStep[i].nStepNo,m_stMakeItem.ItemStep[i].szItemName );
	}
	return true;
}

bool CApi::SEND_ITEM()
{
	if( !g_sys.GetAutoMode() ) return false;
	if( MAKEITEM ) return false;

	if( !g_devsock.IsConn() )
		g_devsock.ConnectToHost();
	if( g_devsock.IsConn() )
	{
		g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_PGM_ITEM_LISTUP, (char*)&m_stMakeItem, sizeof(stTestItem) );
	}
	return true;
}

const char* CApi::GET_STEP_NAME()
{
	return m_sStepName.c_str();
}

//2023.03.16 - KWMOON
int CApi::GET_STEP_NO()
{
	return m_nCurrentStepNo;
}

//2022.11.23 - kwmoon ( STEP_RUN - start )
void CApi::STEP_BEGIN()
{
	if( !GET_RUNMODE() ) return;		//2023.04.03 - KWMOON
	SetDevStatus( PROTOCOL::DEV_STATUS::DEV_STEP_IN  );

	if( g_sys.GetDevice() != DIAGCAL )
	{
		SET_FAILMODE( NORMAL );
		SET_CHIPID_STEP( false );
		clrStepVars();
		m_bBlockFailAddr = false;
		m_bProcExtFmBlock = false;	//2023.05.12 - fatric
		vecStepName.push_back( m_sStepName );
		//strncpy( m_szPc, m_sStepName.c_str(), m_sStepName.length() );	//2023.04.24 - KWMOON(strcpy -> strncpy)
		memset( m_szPc, 0, sizeof(m_szPc) );							//2023.07.12 - KWMOON(Modify)
		PRINTF("[%s]", GET_STEP_NAME());
		PRINTF(NULL);
	}
}

//2022.11.23 - kwmoon ( STEP_RUN - end )
void CApi::STEP_END()
{
	if( !GET_RUNMODE() ) return;		//2023.04.03 - KWMOON

	SetDevStatus( PROTOCOL::DEV_STATUS::DEV_STEP_OUT );

	//2023.03.23 - KWMOON( SET_DEVICE(DIAGCAL) SKIP )
	if( g_sys.GetDevice() != DIAGCAL )
	{
		if ( IS_CHIPID_STEP() )
		{
			CreateChipData();
			if ( m_pStChipId ) { delete [] m_pStChipId; m_pStChipId = NULL; }
		}

		if ( IsInitStep() )
		{
			CreateInitFailData();

			for(uint i=0; i<m_vStepData.size(); ++i )
				delete m_vStepData[i];
			m_vStepData.clear();
			m_vStepData.push_back( new CStepData( ++m_iTsNo, m_tStepBeginTime, m_tStepEndTime, m_stSetTemp.dTemp, m_nBiTime, m_sStepName.c_str(), m_sStepName.c_str(), 0.0 ) );
		}
		else
		{
			AppendStepFailData();
			AppendStepFailBlockData();
			AppendStepFailChipData();
			AppendStepTimeStamp();
			CreateCumFailBlockData();			//2024.06.27 - KWMOON(EVERY STEP MAKE CUM FAIL BLOCK AND SEND TO EWS)
			//2023.06.13 - kwmoon(add fatric)
			if ( strlen(m_szPc)<1 )
				strncpy( m_szPc, m_sStepName.c_str(), m_sStepName.length() );	//2023.04.24 - KWMOON(strcpy -> strncpy)
			m_vStepData.push_back( new CStepData( ++m_iTsNo, m_tStepBeginTime, m_tStepEndTime, m_stSetTemp.dTemp, m_nBiTime, m_sStepName.c_str(), m_szPc, 0.0 ) );
			AppendProcExtFmBlock();		//2023.05.12 - fatric
			AppendFCateData();
			AppendPCateData();
			AppendStepFailMode();
			AppendStepUseMode();
		}

		//PRINTF("%s %s %s", GET_STEP_NAME(), __FUNCTION__, IS_CHIPID_STEP()?"CHIPID_STEP":" " );
		DISPLAY_STEPFAIL_DUTMAP();

	//	int failcount1 = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_FC1_FAIL_COUNT );
	//	fprintf(stdout, "STEP_END::_FC1_FAIL_COUNT1=%d\n", failcount1);
	//	g_pg.CmpClear();
	//	int failcount2 = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_FC1_FAIL_COUNT );
	//	fprintf(stdout, "STEP_END::_FC1_FAIL_COUNT1=%d   _FC1_FAIL_COUNT2=%d\n", failcount1, failcount2);

	//fprintf(stdout, "STEP_END  - 1\n");
	//	GET_EXTFM_BLOCK();
	//fprintf(stdout, "STEP_END  - 2\n");
	//	DISPLAY_BLOCK_FAIL_ADDRESS();
	//	DISPLAY_BLOCK_FAIL_ADDRESS_CE();
	//	DISPLAY_STEP_FAIL_CHIP();
		DISPLAY_BLOCK_FAIL_ADDRESS_STACK();
	//fprintf(stdout, "STEP_END  - 2\n");
		DISPLAY_FCAT();
	//fprintf(stdout, "STEP_END  - 3\n");
	//	DISPLAY_EXTFM_FCNT();
	}
	long long mil  = std::chrono::duration_cast<std::chrono::milliseconds>(m_tpStepEnd - m_tpStepBegin).count();
	PRINTF("STEP_NO.%03d %-30s Elapsed Time : %lld sec %lld msec", GET_STEP_NO()+1, GET_STEP_NAME(), mil/1000, mil%1000 );
	PRINTF(NULL);
}

void CApi::SET_CHIPID_STEP(bool bchipid)
{
	m_bReadNandChipId = bchipid;
}

bool CApi::IS_CHIPID_STEP()
{
	return m_bReadNandChipId;
}

//for compile
void CApi::Send( int cmd, char* buf, int size )
{
	(void)buf;
	PRINTF("%s cmd:%d size:%d", __FUNCTION__, cmd, size );
}

void CApi::SET_PE_READ_ENABLE_SEL(int sel)
{
	g_pg.SetPeReadEnableSel( sel );
}

void CApi::SET_EDQ_SEL(uint sel)
{
	g_pg.SetEdqSel(sel);
}
void CApi::SET_DRDLY(uint cycle)
{
	g_pg.SetDrDelay( cycle );
}

void CApi::SET_IODLY(uint index, uint cycle)
{
	if( (index<0) || (index>IOAB_PIN_PER_FC) )
	{
		EPRINTF("%s error : io index(%d), use 0 ~ %d", __FUNCTION__, index, IOAB_PIN_PER_FC-1 );
		return;
	}
	g_pg.SetIoDelay( index, cycle );
}

void CApi::SET_DREDLY(uint index, uint cycle)
{
	if( (index<0) || (index>7) )
	{
		EPRINTF("%s error : io index(%d), use 0 ~ %d", __FUNCTION__, index, 7 );
		return;
	}
	g_pg.SetDreDelay( index, cycle );
}

void CApi::SET_CPEDLY(uint index, uint cycle)
{
	if( (index<0) || (index>3) )
	{
		EPRINTF("%s error : io index(%d), use 0 ~ %d", __FUNCTION__, index, 3 );
		return;
	}
	g_pg.SetCpeDelay( index, cycle );
}

void CApi::SET_UCLKDLY(uint cycle)
{
	g_pg.SetUclkDelay( cycle );
}

bool CApi::SYS_DBM_CTRL_RESET()
{
	g_dbm.DbmCtrlReset();
	return true;
}

bool CApi::SYS_WRITE_DBM( uint start, char* p, uint size )
{
	return g_sys.SysWriteDbm( start, p, size );
}

bool CApi::SYS_READ_DBM( uint start, char* p, uint size )
{
	return g_sys.SysReadDbm( start, p, size );
}

bool CApi::SYS_EXTFM_CTRL_RESET()
{
	g_dfm.DfmCtrlReset();
	g_pg.DfmClear();
	return true;
}

bool CApi::SYS_WRITE_EXTFM( uint fc, uint64_t start, char* p, uint size )
{
	return g_sys.SysWriteExtfm( fc, start, p, size );
}

bool CApi::SYS_READ_EXTFM ( uint fc, uint64_t start, char* p, uint size )
{
	return g_sys.SysReadExtfm( fc, start, p, size );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TEST RUN FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::programStart()
{
	RUNTIME_ERROR( __FUNCTION__, "virtual function %s called!", __FUNCTION__ );
}

void CApi::testEntry()
{
	RUNTIME_ERROR( __FUNCTION__, "virtual function %s called!", __FUNCTION__ );
}

void CApi::initTest()
{
	RUNTIME_ERROR( __FUNCTION__, "virtual function %s called!", __FUNCTION__ );
}

void CApi::device_default(const char* name)
{
	EPRINTF( "virtual function %s(%s)called!", __FUNCTION__, name );
}

void CApi::define_flag()
{
	EPRINTF( "virtual function called!", __FUNCTION__ );
}

bool CApi::BURST( const char* pattern, int burst )
{
	EPRINTF("A virtual function '%s(%s,%x)' has been called. Implement the BURST function.",
			__FUNCTION__, pattern, burst );
	return false;
}

void CApi::CLR_ALLREG_GLOBAL()
{
	g_pg.ClearGlobalRegister();
}

void CApi::DEFINE_FLAG_A()
{
	g_sys.SetDefineFlag( FLAG_A );
}

void CApi::DEFINE_FLAG_B()
{
	g_sys.SetDefineFlag( FLAG_B );
}

void CApi::DEFINE_FLAG_C()
{
	g_sys.SetDefineFlag( FLAG_C );
}

void CApi::DEFINE_FLAG_D()
{
	g_sys.SetDefineFlag( FLAG_D );
}

void CApi::DEFINE_FLAG_E()
{
	g_sys.SetDefineFlag( FLAG_E );
}

void CApi::DEFINE_FLAG_F()
{
	g_sys.SetDefineFlag( FLAG_F );
}

void CApi::DEFINE_FLAG_G()
{
	g_sys.SetDefineFlag( FLAG_G );
}

void CApi::DEFINE_FLAG_H()
{
	g_sys.SetDefineFlag( FLAG_H );
}

void CApi::DEFINE_FLAG_I()
{
	g_sys.SetDefineFlag( FLAG_I );
}

void CApi::DEFINE_FLAG_J()
{
	g_sys.SetDefineFlag( FLAG_J );
}

void CApi::DEFINE_FLAG_K()
{
	g_sys.SetDefineFlag( FLAG_K );
}

void CApi::DEFINE_FLAG_L()
{
	g_sys.SetDefineFlag( FLAG_L );
}

void CApi::DEFINE_FLAG_M()
{
	g_sys.SetDefineFlag( FLAG_M );
}

void CApi::DEFINE_FLAG_N()
{
	g_sys.SetDefineFlag( FLAG_N );
}

void CApi::DEFINE_FLAG_O()
{
	g_sys.SetDefineFlag( FLAG_O );
}

void CApi::DEFINE_FLAG_P()
{
	g_sys.SetDefineFlag( FLAG_P );
}

void CApi::DEFINE_FLAG_Q()
{
	g_sys.SetDefineFlag( FLAG_Q );
}

void CApi::DEFINE_FLAG_R()
{
	g_sys.SetDefineFlag( FLAG_R );
}

void CApi::DEFINE_FLAG_S()
{
	g_sys.SetDefineFlag( FLAG_S );
}

void CApi::DEFINE_FLAG_T()
{
	g_sys.SetDefineFlag( FLAG_T );
}

void CApi::DEFINE_FLAG_U()
{
	g_sys.SetDefineFlag( FLAG_U );
}

void CApi::DEFINE_FLAG_V()
{
	g_sys.SetDefineFlag( FLAG_V );
}

void CApi::DEFINE_FLAG_W()
{
	g_sys.SetDefineFlag( FLAG_W );
}

void CApi::DEFINE_FLAG_X()
{
	g_sys.SetDefineFlag( FLAG_X );
}

void CApi::DEFINE_FLAG_Y()
{
	g_sys.SetDefineFlag( FLAG_Y );
}

void CApi::DEFINE_FLAG_Z()
{
	g_sys.SetDefineFlag( FLAG_Z );
}

void CApi::setVersion(const char* VER)
{
	sprintf(m_stCommonData.szRemark, "%s", VER );
	snprintf( m_stPgmVariables.szVersion, sizeof(m_stPgmVariables.szVersion), "%s", VER );
	PRINTF( "setVersion : %s", VER );
}

int CApi::parseCommand( int cmd_c, char* cmd_v[] )
{
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	(void)cmd_c;
	(void)cmd_v;
	return 0;
}

void CApi::RESET_TESTER()
{
	SET_VS_OFF( S1PS );
	SET_VS_OFF( PPS  );

	SwInit();
	HwInit();
	//DisconnectToEwsMgr();
}

int CApi::DELAY_TIME( double delay )
{
	return g_sys.SysDelay( delay );
}

int CApi::sysDelay( double delay )
{
	return g_sys.SysDelay( delay );
}

int CApi::GetCurrentCpuNo()
{
	_CurrentCpuNo =  g_sys.GetCurrentCpuNo();
	return _CurrentCpuNo;
}

int CApi::GetSiteNo()
{
	return g_sys.GetReadSiteNo()&0xFF;
}

const char* CApi::GetLibVersion()
{
	return TESTLIB_VERSION;
}

int CApi::GET_FC( int dut )
{
	int fc = 0x0;
	if( !g_bib.GetFc(dut,fc) )
	{
		//RUNTIME_ERROR( __FUNCTION__, "DUT%d error!", dut );
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "DUT%d error!", dut );
	}
	return fc;
}

void CApi::SET_FC(int fc)
{
	m_FCAdd = fc;
}

bool CApi::IS_MYDUT(unsigned int dut )
{
	bool bMyDut = false;
	BIB_CPU cpu = CPU1;
	if( GetCurrentCpuNo() == 0 ) cpu = CPU1;
	else                         cpu = CPU2;

	if( !g_bib.IsMyDut( cpu, dut, bMyDut ) )
	{
		//RUNTIME_ERROR( __FUNCTION__, "DUT%d error!", dut );
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "DUT%d error!", dut );
	}
	return bMyDut;
}

int CApi::ALLSYNC()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DEVICE CONFIG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_DEVICE(int mode, ushort code)
{
	m_stPgmVariables.nDeviceType = mode;	//2023.03.08 - KWMOON
	g_sys.SetDevice(mode, code);
	if( mode == DEVICE_TYPE::DIAGCAL )
	{
		SET_PFM_ENABLE( false );
		PRINTF("PFM DISABLE");
	}
	else
	{
		//SET_PFM_ENABLE( false );
		//PRINTF("PFM DISABLE");
		SET_PFM_ENABLE( true );
		PRINTF("PFM ENABLE");
	}
}

void CApi::SET_ADD_SIZE( uint block_shift, uint page_shift, uint colsize, uint mcp, uint nchip )
{
	g_bib.SetAddSize( block_shift, page_shift, colsize, mcp, nchip );

	m_addBlkShift   = block_shift;
	m_block_shift   = block_shift;
	m_addPageShift  = page_shift;
	m_addColSize    = colsize;
	m_addMaxCe      = mcp;
	m_mcp           = mcp;		//2022.11.29 - KWMOON
	m_addMaxChip    = nchip;

	m_nCePerDut     = m_addMaxCe;
	m_nChipPerCe    = m_addMaxChip;
	m_nBlockPerCe   = m_lmtBlkPerDut / m_nCePerDut;
	m_nBlockPerChip = m_nBlockPerCe  / m_nChipPerCe;

	m_iVarConf |= 0x2;
	initVars();
}

void CApi::SET_CE( uint mcporder, IO_AB ioab, int ioaborder, uint addr_ce, CE_NUM ceorder )
{
	if( !g_bib.SetCe( mcporder, ioab, ioaborder, addr_ce, ceorder ) )
	{
		EPRINTF( "%s error : mcp(%d) ioab(%d) ioab_order(%d) ce_addr(0x%04X) ce_order(%d)",
				 __FUNCTION__, mcporder, ioab, ioaborder, addr_ce, ceorder );
	}
	else
	{
		int _ioab = (ioab == IO_AB::IOA)?IOAA:IOBB;
		_ceaddress[_ioab][mcporder] = addr_ce;	//2022.12.02 - KWMOON
	}
}

void CApi::SET_CE_COND_1DUT( int dutbit_ioa, int dutbit_iob, int cecnt_ioa, int cecnt_iob)
{
	g_bib.SetCeCond1Dut( dutbit_ioa, dutbit_iob, cecnt_ioa, cecnt_iob );
}

int CApi::GET_CE_ORDER( int mcp )
{
	uint ce_order = 0x0;
	if( !g_bib.GetCeOrder( mcp, ce_order ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "Get ce order error by mcp:%d", mcp );
	}
	return ce_order;
}

int CApi::GET_CE_OF_BLOCK( unsigned int blockaddr )
{
	int ce = 0x0;
	if( !g_bib.GetCeOfBlock( blockaddr, ce ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "Get ce order error by block address:0x%x", blockaddr );
	}
	return ce;
}

int CApi::GET_CE_ADDR_MCP( unsigned int mcp )
{
	int ce = 0x0;
	if( !g_bib.GetCeAddrMcp( mcp, ce ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "Get ce address error by mcp:%d", mcp );
	}
	return ce;
}

unsigned int CApi::GET_CHIP_INDEX( int mcp , int chip )
{
	return mcp * g_bib.GetChipCount() + chip;	//CE * CHIP_CNT + chip
}

/*
SET_CE(0        ,IOA   ,0      ,0x0 	,CE1);
SET_CE(1        ,IOA   ,1      ,0x2000	,CE2);
SET_CE(2        ,IOA   ,2      ,0x4000	,CE3);
SET_CE(3        ,IOA   ,3      ,0x6000	,CE4);
SET_CE(4        ,IOA   ,4      ,0x8000	,CE5);
SET_CE(5        ,IOA   ,5      ,0xA000	,CE6);
SET_CE(6        ,IOA   ,6      ,0xC000	,CE7);
SET_CE(7        ,IOA   ,7      ,0xE000	,CE8);
ex)
blockaddr = 0x2010
ce = CE2(1)
ce_block_start = 1*0x2000
ce_internal_addr = 0x2010 - 0x2000 = 0x10
GetChipBlockLimit = GetCeBlockLimit() / chip_count
chip = ce_internal_addr / GetChipBlockLimit
*/
int CApi::GET_CHIP_OF_BLOCK( unsigned int blockaddr )
{
	uint ce               = GET_CE_OF_BLOCK( blockaddr );		//ce order
	uint ce_block_start   = ce * g_bib.GetCeBlockLimit();		//ce start block address
	uint ce_internal_addr = blockaddr - ce_block_start;
	uint chip = ce_internal_addr / g_bib.GetChipBlockLimit();
	return chip;
}

void CApi::SET_LIMIT( uint blperdev, uint fblperce , uint ceaddrlimit )
{
	g_bib.SetLimit( blperdev, fblperce, ceaddrlimit );

	m_lmtBlkPerDut    = blperdev;
	m_lmtBlkPerCe     = fblperce;
	m_lmtBlkPerCeStep = ceaddrlimit;
	m_lmtAddFail      = ceaddrlimit;

	m_iVarConf |= 0x1;
	initVars();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BIB
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_BIB( int scan, int io_size, int bibx, int biby, bool f16mode, bool invisible, bool cpux2 )
{
	g_bib.SetBib( scan, io_size, bibx, biby, f16mode, invisible, cpux2 );

	m_scan      = scan;
	m_bibScan   = scan;
	m_bibIo     = io_size;
	m_bib_x     = bibx;
	m_bib_y     = biby;
	m_bib16mode = f16mode;
	m_scan_invisible = invisible;
	m_bibCpu    = cpux2;

	m_nMaxDutPerCpu = bibx * biby;
	if ( m_bibCpu ) m_nMaxDutPerCpu /= 2;

	m_iVarConf     |= 0x4;
	initVars();
}

void CApi::BIB(int si, ...)
{
	int bib_duts = g_bib.GetBibX() * g_bib.GetBibY();
	int set_count = 0;
	memset( m_index_si_map, 0x0, sizeof(m_index_si_map) );

	if( 0 ) PRINTF("%s DUT:%d", __FUNCTION__ , bib_duts );

	va_list list;
	va_start( list, si );
	int value = si;
	if( value != 0 )
	{
		//g_bib.ClearBib(); -->MOVE TO SET_BIB
		while( value != 0 )
		{
			//printf("%s : %08X\n", __FUNCTION__, value );
			if( !g_bib.Bib( set_count, value ) )
			{
				//RUNTIME_ERROR( __FUNCTION__, "BIB error!" );
				EPRINTF( "%s error!", __FUNCTION__ );
				break;
			}
			value = va_arg( list, ull64 );
			if( set_count > (bib_duts-1) )
			{
				va_end(list);
				EPRINTF( "Dut count mismatch[SET_BIB:%d, BIB:%d]", bib_duts, set_count );
				return;
			}
			set_count++;
		}
	}
	va_end(list);
	if( bib_duts != set_count )
	{
		 EPRINTF( "Dut count mismatch[SET_BIB:%d, BIB:%d]", bib_duts, set_count );
	}
	else
	{
		for( int dut=0;dut<bib_duts;dut++ )
			m_index_si_map[dut] = g_bib.GetSi(dut);
	}
}

void CApi::CLR_SCAN()
{
	g_bib.ClearScan();
}

void CApi::READ_SCAN( int fc, int line )
{
	g_bib.GetScan( fc, line );
}

void CApi::SET_SCAN( unsigned long long scan, ... )
{
	va_list list;
	va_start( list, scan );
	unsigned long long value = scan;
	int idx = 0;
	g_bib.ClearScan();
	while( value != 0 )
	{
		if( idx > (SCAN_MEM_DEPTH-1) )
		{
			EPRINTF( "%s Scan memory address %d over than %d",
					 __FUNCTION__, idx, SCAN_MEM_DEPTH );
			break;
		}
		if( 0 ) PRINTF("%s %03d : %llX", __FUNCTION__, idx, value );
		g_bib.SetScan( idx, value );
		value = va_arg( list, unsigned long long );
		idx++;
	}
	va_end(list);
	g_bib.WriteScan();
}

void CApi::SET_FCSCAN_BYDUT(unsigned int dut_index)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_FCSCAN_BYDUTINDEX(unsigned int dut, int index,	unsigned int long long value )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_CONCURRENT_MODE(bool concurrent)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_JUDGE(int rack )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

bool CApi::IS_SITE_FAULT()
{
	bool bBibCon = g_sys.IsConnBib();
	_bi_find[MAX_NSLOT].read = bBibCon;
	//PRINTF( "%s BIB CON : %s", __FUNCTION__,  bBibCon?"YES":"FALSE" );
	if( bBibCon ) return false;
	return true;
}

void CApi::SET_STROBE_DELAY( double ps )
{
	for( int r=0;r<16;r++ )
		g_tg.SET_STROBE_DELAY( r, ps*10 );
}

void CApi::SET_STROBE_DELAY( int row, double ps )
{
	g_tg.SET_STROBE_DELAY( row, ps*10 );
}

void CApi::SET_STROBE_DELAY( int begin, int end, int delay )
{
	(void)begin;
	(void)end;
	for( int r=0;r<16;r++ )
		g_tg.SET_STROBE_DELAY( r, delay*10 );
}

int CApi::index_to_xy(int dut, unsigned int &x, unsigned int &y)
{
	if( !g_bib.GetXYOfDut( dut, x, y ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "DUT%d get x, y error!", dut );
	}
	return -1;
}

int CApi::index_to_siab(int index, unsigned int &rScan, unsigned int &rIoGrp, unsigned int &rIoAb )
{
	if( !IS_MYDUT(index) ) return -1;

	if( !g_bib.GetSiabOfDut( index, rScan, rIoGrp, rIoAb ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "DUT%d get scan/iogrp/ioab error!", index );
		return -1;
	}
	return 0;
}

int CApi::xy_to_index(unsigned int x, unsigned int y, unsigned int &index)
{
	if( !g_bib.GetXyToIndex( x, y, index ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "X:%d, Y:%d index get error!", x, y );
		return -1;
	}
	return 0;
}

int CApi::xy_to_siab(unsigned int x, unsigned int y, unsigned int &rScan, unsigned int &rIoGrp,unsigned int &rIoAb)
{
	if( !g_bib.GetXyToSiab( x, y, rScan, rIoGrp, rIoAb ) )
	{
		g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "X:%d, Y:%d siab get error!", x, y );
		return -1;
	}
	return 0;
}

int CApi::si_to_index(unsigned int realscan, unsigned int iogrp, unsigned int &rDutIdx, bool is_bibscan )
{
	if( !g_bib.GetSiToIndex( realscan, iogrp, rDutIdx, is_bibscan ) )
	{
		//2023.07.13 - KWMOON(comment - Si map split cpu0 and 1)
		//g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "Scan:%d, IoGrp:%d dut index get error!", realscan, iogrp );
		return -1;
	}
	return 0;
}

int CApi::si_to_xy(unsigned int scan, unsigned int iogrp, unsigned int &x, unsigned int &y)
{
	if( !g_bib.GetSiToXy( scan, iogrp, x, y ) )
	{
		//g_log.EPrintf( DEV_LOG_TYPE::_BIB, __FUNCTION__, "Scan:%d, IoGrp:%d x/y index get error!", scan, iogrp );
		return -1;
	}
	return 0;
}

unsigned int CApi::BIB_X()
{
	return g_bib.GetBibX();
}

unsigned int CApi::BIB_Y()
{
	return g_bib.GetBibY();
}

unsigned int CApi::BIB_IO()
{
	uint bib_io = 0;
	if( !m_bVarAvail ) bib_io = 8;
	else               bib_io = g_bib.GetBibIo();
	return MAX_SYS_IO_BIT / bib_io;
}

unsigned int CApi::NCHIP()
{
	return g_bib.GetChipCount();
}

unsigned int CApi::BLOCK_LIMIT()
{
	return g_bib.GetBlockLimit();
}

unsigned int CApi::MCP()
{
	return g_bib.GetCeCount();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::CLR_SCANMASK()
{
	PRINTF("%s", __FUNCTION__ );
	//if( !GET_RUNMODE() ) return;					//2023.03.14 - KWMOON
	for( int fc=0;fc<USE_FC;fc++ )
	{
		for( int m=0;m<SCAN_MEM_DEPTH;m++ )
		{
			for( int d=0;d<3;d++ )
			{
				m_scan_mask[fc][m][d] = 0x0;
			}
		}
	}
	g_pg.AlpgClear();		//2022.12.21 - kwmoon
	g_bib.ClrScanMask();
}

void CApi::clearScanMask()
{
	//if( !GET_RUNMODE() ) return;					//2023.03.14 - KWMOON
	CLR_SCANMASK();
}

void CApi::CLR_SCANMASK(unsigned int dut)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_SCANMASK_ALL()
{
	//if( !GET_RUNMODE() ) return;					//2023.03.14 - KWMOON

	for( int fc=0;fc<USE_FC;fc++ )
	{
		for( int m=0;m<SCAN_MEM_DEPTH;m++ )
		{
			for( int d=0;d<3;d++ )
			{
				m_scan_mask[fc][m][d] = 0xffffffff;
			}
		}
	}
	g_bib.SetScanMaskAll();
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

//virtual function - user implement
void CApi::SET_SCANMASK(unsigned int dut)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

//virtual function - user implement
void CApi::SET_SCANMASK()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::WRITE_SCANMASK(bool bprint, uint realscan)
{
	if( MAKEITEM ) return;					//2023.03.17 - KWMOON
	if( !GET_RUNMODE() ) return;					//2023.03.14 - KWMOON

	(void)realscan;
	if ( m_debug_mode&0x80 ) bprint = true;

	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		for(uint scan=0;scan<SCAN_MEM_DEPTH;scan++ )
		{
			for( uint m=0;m<3;m++ )
			{
				g_bib.WriteScanMask( fc, scan, m, m_scan_mask[fc][scan][m] );
				//if( bprint && (scan==realscan) )
				if( bprint && m_scan_mask[fc][scan][m] )
				{
					PRINTF( "%s Realscan[%02d] Index[%02d] ScanMask[IOB:%04X IOA:%04X]",
							__FUNCTION__, scan, m, (m_scan_mask[fc][scan][m]>>16)&0xffff, m_scan_mask[fc][scan][m]&0xffff );
				}
			}
		}
	}
}

void CApi::setScanMask(bool bprint, uint realscan)
{
	if( MAKEITEM ) return;					//2023.03.17 - KWMOON
	if( !GET_RUNMODE() ) return;					//2023.03.14 - KWMOON
	WRITE_SCANMASK( bprint, realscan );
}

void CApi::SET_IOMASK_GLOBAL(int memaddr, long long sdr_rising, long long ddr_falling )
{
	if( (memaddr<0) || (memaddr>IO_MASK_DEPTH-1) )
	{
		EPRINTF( "%s memory address is invalid %d, use %d ~ %d", __FUNCTION__, memaddr, 0, IO_MASK_DEPTH-1 );
		return;
	}
	if( !g_pg.SetIoMaskGlobal( memaddr, sdr_rising, ddr_falling ) )
	{
		EPRINTF( "%s error!", __FUNCTION__ );
	}
}

void CApi::RESET_DRIVER()
{
	//g_pg.DrvClear();
	//g_pg.CmpClear();
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

//2023.04.21 - fatric ( all modify )
void CApi::SET_PINOUT_DRV ( bool bEnable, uint uBegin, uint uEnd )
{
	g_pg.SetDrPinFixL();		//en, dis - fixl
	if ( bEnable )
	{
		g_pg.SetPinOutDrv( bEnable, uBegin, uEnd );
		m_iPinOutDrStat = 1;
	}
	else
	{
			 if ( m_iPpsStat==1 ) { g_pg.SetPinOutDrvVal( bEnable, uBegin, uEnd ); }//PRINTF("SetPinOutDrvVal");}
		else if ( m_iPpsStat==0 ) { g_pg.SetPinOutDrv   ( bEnable, uBegin, uEnd ); }//PRINTF("SetPinOutDrv");}

		m_iPinOutDrStat = 0;
	}
}

void CApi::SET_PINOUT_IOA ( bool bEnable, uint uBegin, uint uEnd )
{
	g_pg.SetPinOutIoA( bEnable, uBegin, uEnd );
}

void CApi::SET_PINOUT_IOB ( bool bEnable, uint uBegin, uint uEnd )
{
	g_pg.SetPinOutIoB( bEnable, uBegin, uEnd );
}

//2023.04.21 - fatric ( all modify )
void CApi::SET_PINOUT_UCLK ( bool bEnable, uint uBegin, uint uEnd )
{
	g_pg.SetUclkPinFixL();	// EN , DIS - ALL
	if ( bEnable )
	{
		g_pg.SetPinOutUclk( bEnable, uBegin, uEnd );
		m_iPinOutUclkStat = 1;
	}
	else
	{
			 if ( m_iPpsStat==1 ) { g_pg.SetPinOutUclkVal( bEnable, uBegin, uEnd );  }
		else if ( m_iPpsStat==0 ) { g_pg.SetPinOutUclk   ( bEnable, uBegin, uEnd );  }
		else {}

		m_iPinOutUclkStat = 0;
	}
}

void CApi::SET_PINOUT_MIO ( bool bEnable, uint uBegin, uint uEnd )
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_PINOUT_SCANMASK_1ST ( bool bEnable, uint uBegin, uint uEnd )
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_PINOUT_SCANMASK_2ND ( bool bEnable, uint uBegin, uint uEnd )
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_PINOUT_SCAN_INV ( bool bEnable, uint uBegin, uint uEnd )
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_PINOUT( bool bDrv, bool bIoa, bool bIob, bool bUclk, bool bMio, bool bScan1st, bool bScan2nd, bool bScanInv)
{
	if( bDrv )
	{
		//fprintf( stdout, "\n\n-------------------%s--------------------\n\n", __FUNCTION__ );
		SET_PINOUT_DRV( true, 0, 31 );
	}
}

void CApi::SET_IODDRMODE_GLOBAL(bool bDdr)
{
	g_pg.SetIoDdrModeGlobal( bDdr );
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_ADDRDDRMODE_GLOBAL(bool bDdr)
{
	g_pg.SetAddrDdrModeGlobal( bDdr );
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_MATCHRB_GLOBAL()
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::FM_WRITE_GLOBAL(bool bEnable)
{
	g_dfm.FmWriteGlobal( bEnable );
}

void CApi::SET_FMMODE( int fc, unsigned int mode)
{
	//g_dfm.SetFmModeGlobal( mode );
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::RESET_FMMODE(int fc, unsigned int mode)
{
	//g_dfm.ResetFmModeGlobal( mode );
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_FMMODE_GLOBAL(unsigned int mode)
{
	g_dfm.SetFmModeGlobal( mode );
	if( g_dfm.GetFmModeGlobal() & EDBM_BG_WRITE )
	{
		//PRINTF( "SET_FMMODE_GLOBAL : EDBM_BG_WRITE" );
		if( !g_dbm.CreateBgMemory() )
		{
			EPRINTF( "DBM BG Memory create error!" );
		}
	}
}

void CApi::RESET_FMMODE_GLOBAL(unsigned int mode)
{
	g_dfm.ResetFmModeGlobal( mode );

	if ( mode==0xffffffff ) return;	//2023-03-21

	if( mode & EDBM_BG_WRITE )
	{
//		PRINTF( "RESET_FMMODE_GLOBAL : EDBM_BG_WRITE" );
		if( !g_dbm.WriteDbmMemory( true ) )
		{
			//EPRINTF( "DBM BGWrite error!" );
		}
	}
}

void CApi::DBG_CLR_DFM_MEMORY( int count_2048 )
{
	g_dfm.ClrExtFmMemory( count_2048 );
}

//External Memory Clear
void CApi::CLR_EXTFM_GLOBAL()
{
	if( m_bVarAvail )
	{
//		for( uint d=0;d<m_nMaxDutPerCpu;d++ )
		for( uint d=0;d<m_nMaxDutPerCpu*2;d++ )
			memset( m_ext_fm_fail[d], 0x0, sizeof(int)*EXTFM_SW_MAX_LIMIT );
		memset( m_ext_fm_fail_count, 0x0, sizeof(int)*m_nMaxDutPerCpu );
		memset( m_tmp_fm_data, 0x0, sizeof(m_tmp_fm_data) );
	}
	g_pg.DfmClear();			//DFM CLEAR
	g_pg.CmpClear();			//Comparator & Latch Clear
	g_dfm.ClearFmPointer();
}

//External Memory Clear & Latch Memory Clear
void CApi::CLR_FM_GLOBAL()
{
	CLR_EXTFM_GLOBAL();
}

void CApi::GET_EXTFM_INIT(int limit , bool bprint )
{
	uint fc_fm_count[USE_FC] = { 0x0, };

	bool is_pgpause = IS_PGPAUSE();
	bool bWrite = g_dfm.GetFmWrite();

	if( is_pgpause )
		g_dfm.FmWriteGlobal( false );	//EXT FM READ MODE CHANGE

	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
//		PRINTF("%s COMP MODE");
		PRINTF("COMP MODE");
		fc_fm_count[FC00] = g_dfm.ReadExtFmCompFcnt();
		fc_fm_count[FC01] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
//		PRINTF("%s NORM MODE");
		PRINTF("NORM MODE");
		fc_fm_count[FC00] = g_dfm.ReadExtFmFcnt(FC00);
		fc_fm_count[FC01] = g_dfm.ReadExtFmFcnt(FC01);
	}

	if ( m_debug_mode&0x40 )
	{
		PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, fc_fm_count[FC00] );
		PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, fc_fm_count[FC01] );
	}

	if( static_cast<int>(fc_fm_count[FC00]) > limit )
	{
		RUNTIME_ERROR( __FUNCTION__, "runtime-error : FC0 FM fail count(%d) limit over %d\n", fc_fm_count[FC00], limit );
	}
	if( static_cast<int>(fc_fm_count[FC01]) > limit )
	{
		RUNTIME_ERROR( __FUNCTION__, "runtime-error : FC1 FM fail count(%d) limit over %d\n", fc_fm_count[FC01], limit );
	}

	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		if( !g_dfm.ReadExtFmComp() )
		{
			RUNTIME_ERROR( __FUNCTION__, "error : Ext FM COMP read!" );
			return;
		}
	}
	else
	{
		if( !g_dfm.ReadExtFm() )
		{
			RUNTIME_ERROR( __FUNCTION__, "error : Ext FM read!" );
			return;
		}
	}
	//clear pass/fail
	memset( m_init_fail , 0, sizeof(char) * m_nMaxDutPerCpu );
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		PRINTF( "FC1 COMP Fail Count:%d", fc_fm_count[FC00] );
		PRINTF( "FC2 COMP Fail Count:%d", fc_fm_count[FC01] );
	}
	else
	{
		PRINTF( "FC1 NORM Fail Count:%d", fc_fm_count[FC00] );
		PRINTF( "FC2 NORM Fail Count:%d", fc_fm_count[FC01] );
	}
	if( bprint )
	{
		g_dfm.PrintExtFm( FC00, fc_fm_count[FC00] );
		g_dfm.PrintExtFm( FC01, fc_fm_count[FC01] );
	}

	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon

		int nFailCount = fc_fm_count[fc];
		FM_MEM* pFm    = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFailCount;f++ )
		{
			STOP_BREAK;		//2023.04.27 - kwmoon

			uint iopin    = 0x0;
			uint realscan = pFm[f].header.scan;
			//YBKIM
//			if ( m_debug_mode&0x100 && f < 1000 )
			if ( m_debug_mode&0x100 )
			{
				if ( m_debug_mode&0x200 )
				{
					if ( f < 1000 )
						PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, fc_fm_count[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
				}
				else
					PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, fc_fm_count[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
			}

			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon

				const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					if( pFm[f].data[seq].ioa_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						SET_PRE_FAIL( BIB0, dut, true );
						m_scan_mask[fc][realscan][seq] |= (0xFF);
						if( 0 ) printf( "%s : SCAN:%d FC%02d IOA IOPIN:%02d IOGRP:%02d DUT:%03d\n", __FUNCTION__, realscan, fc, iopin, iogrp, dut );
					}
					if( pFm[f].data[seq].ioa_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						SET_PRE_FAIL( BIB0, dut, true );
						m_scan_mask[fc][realscan][seq] |= (0xFF<<8);
						if( 0 ) printf( "%s : SCAN:%d FC%02d IOA IOPIN:%02d IOGRP:%02d DUT:%03d\n", __FUNCTION__, realscan, fc, iopin, iogrp, dut );
					}
					if( pFm[f].data[seq].iob_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}

						SET_PRE_FAIL( BIB0, dut, true );
						m_scan_mask[fc][realscan][seq] |= (0xFF<<16);
						if( 0 ) printf( "%s : SCAN:%d FC%02d IOB IOPIN:%02d IOGRP:%02d DUT:%03d\n", __FUNCTION__, realscan, fc, iopin, iogrp, dut );
					}
					if( pFm[f].data[seq].iob_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						SET_PRE_FAIL( BIB0, dut, true );
						m_scan_mask[fc][realscan][seq] |= (0xFF<<24);
						if( 0 ) printf( "%s : SCAN:%d FC%02d IOB IOPIN:%02d IOGRP:%02d DUT:%03d\n", __FUNCTION__, realscan, fc, iopin, iogrp, dut );
					}
				}
				//rfu
				else if( g_bib.GetBibIo() == 16 )
				{
				}
			}
		}
	}

	//debug comment
	if( is_pgpause )
		g_dfm.FmWriteGlobal( bWrite );	//EXT FM READ MODE CHANGE
	STOP_RETURN;		//2023.04.27 - kwmoon
	WRITE_SCANMASK( false, 31);
	//WRITE_AUTO_SCANMASK( true, 16 );
}

//Based on reference company d's manual
int CApi::READ_EXTFM_FCNT(int fc)
{
	return g_dfm.ReadExtFmFcnt( fc ) * 2;
}

int CApi::GET_EXTFM_BLOCK( int limit, int count_limit, bool printf )
{
	(void)count_limit;	//not used

	m_use_mode |= 0x2;

	bool bFC[USE_FC] = { false, false };
	int  nFC[USE_FC] = { 0x0, 0x0 };
//fprintf(stdout, "GET_EXTFM_BLOCK::g_dfm.GetFmModeGlobal=%x\n", g_dfm.GetFmModeGlobal() );

	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		nFC[0] = g_dfm.ReadExtFmCompFcnt();
		nFC[1] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
		nFC[0] = g_dfm.ReadExtFmFcnt(FC00);
		nFC[1] = g_dfm.ReadExtFmFcnt(FC01);
	}
	//PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, nFC[0] );
	//PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, nFC[1] );
	if ( m_debug_mode&0x40 )
	{
		PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, nFC[0] );
		PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, nFC[1] );
	}

	for( int fc=0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon

		if( nFC[fc] > limit )
		{
//fprintf(stdout, "GET_EXTFM_BLOCK-1\n");
			for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
			{
				if( IS_PRE_FAIL( BIB0, dut) ) continue;
				if( GET_FC(dut) != fc       ) continue;

				SET_STEP_FAIL( BIB0, dut, true );
				SET_CUM_FAIL ( BIB0, dut ,true );
				SET_TEST_FAIL( BIB0, dut ,true );
			}
			bFC[fc] = true;
		}
	}
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		g_dfm.ReadExtFmComp();
	}
	else
	{
//fprintf(stdout, "GET_EXTFM_BLOCK-2\n");
//fprintf(stdout, "GET_EXTFM_BLOCK-2 bFC[FC00]=%s bFC[FC01]=%s\n", (bFC[FC00]?"true":"false"), (bFC[FC01]?"true":"false"));
		if( !bFC[FC00] ) g_dfm.ReadExtFm(FC00);
		if( !bFC[FC01] ) g_dfm.ReadExtFm(FC01);
	}

	if( printf )
	{
		PRINTF("[%s - FM DATA]", __FUNCTION__ );
		if( !bFC[FC00] ) g_dfm.PrintExtFm( FC00, nFC[FC00] );
		if( !bFC[FC01] ) g_dfm.PrintExtFm( FC01, nFC[FC01] );
	}

//	PRINTF("DEBUG::GET_EXTFM_BLOCK 1");
	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
//		PRINTF("DEBUG::GET_EXTFM_BLOCK 2");
		if( bFC[fc] ) continue;	//fail fc skip

		if ( nFC[fc] < 1 ) continue;;	//2023.01.02 KWMOON
//		PRINTF("DEBUG::GET_EXTFM_BLOCK 3 bFC[fc]=%d fc=%d", nFC[fc], fc);
		FM_MEM* pFm    = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFC[fc];f++ )
		{
			STOP_BREAK;		//2023.04.27 - kwmoon

			uint iopin    = 0x0;
			uint realscan = pFm[f].header.scan;
			uint block    = pFm[f].header.address;
			uint iCe      = realscan % m_mcp;

			block  = block >> m_addBlkShift;
			if ( block>=m_nBlockPerCe )
			{
				EPRINTF("%s error : fc:%d Block Address(0x%04x) Invalid.", __FUNCTION__, fc, block );
				EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
				continue;;
			}

			block += iCe * m_nBlockPerCe;
//			if ( (m_debug_mode&0x100) && (f < 1000) )
			if ( m_debug_mode&0x100 )
			{
				if ( m_debug_mode&0x200 )
				{
					if ( f < 1000 )
						PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
				}
				else
					PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
			}

//			PRINTF("DEBUG::GET_EXTFM_BLOCK  realscan=#%08X   block=#%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", realscan, block, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon

				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					if( pFm[f].data[seq].ioa_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
						}

						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);

//							PRINTF("\tDEBUG::GET_EXTFM_BLOCK::SET_CUM_BLOCK_FAIL scan:%d  address:%08X   D:%d B:%08X    ioa_l", pFm[f].header.scan, pFm[f].header.address, dut, block);
						}
					}
					if( pFm[f].data[seq].ioa_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
						}
						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);

//							PRINTF("\tDEBUG::GET_EXTFM_BLOCK::SET_CUM_BLOCK_FAIL scan:%d  address:%08X   D:%d B:%08X    ioa_l", pFm[f].header.scan, pFm[f].header.address, dut, block);
						}
					}
					if( pFm[f].data[seq].iob_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
						}

						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);

//							PRINTF("\tDEBUG::GET_EXTFM_BLOCK::SET_CUM_BLOCK_FAIL scan:%d  address:%08X   D:%d B:%08X    ioa_l", pFm[f].header.scan, pFm[f].header.address, dut, block);
						}
					}
					if( pFm[f].data[seq].iob_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   block=#%08X header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, block, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
						}

						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);

//							PRINTF("\tDEBUG::GET_EXTFM_BLOCK::SET_CUM_BLOCK_FAIL scan:%d  address:%08X   D:%d B:%08X    ioa_l", pFm[f].header.scan, pFm[f].header.address, dut, block);
						}
					}
				}
				//rfu
				else if( g_bib.GetBibIo() == 16 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo();
					if( pFm[f].data[seq].ioab&0x0000ffff )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						}

						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);
						}
					}
					if( pFm[f].data[seq].ioab&0xffff0000 )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						}

						if ( m_fail_mode == INIT_FAIL )
							SET_INIT_BLOCK_FAIL(BIB0, dut, block, true);
						else
						{
							SET_STEP_BLOCK_FAIL(BIB0, dut, block, true);
							SET_CUM_BLOCK_FAIL (BIB0, dut, block, true);
						}
					}
				}
			}
		}
	}

	m_bBlockFailAddr = true;
	m_bProcExtFmBlock = true;	//2023.05.12 - fatric
	g_pg.DfmClear();			//2023.05.01 - fatric

	//2023.07.12 - kwmoon
	for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
	{
		if ( ! IS_MYDUT(iDut)         ) continue;
		if ( IS_PRE_FAIL (BIB0, iDut) ) continue;

		for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
		{
			uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe * m_nBlockPerCe)/8;
			uint nEnd   = nStart + (m_nBlockPerCe/8);
			if ( m_nBlockPerCe&0x7 ) nEnd += 1;

			uint iFailCnt = 0;
			for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
			{
				iFailCnt += m_nBitCnt[ (int)m_now_step_fail_block[iBlock]&0xff ];
			}
			m_step_fail_block_cnt[(iDut%m_nMaxDutPerCpu) * m_nCePerDut + iCe] = iFailCnt;
		}
		//2023.08.30 - KWMOON(debug)
		if( 0 )
		{
			uint iDutFailCnt = 0;
			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				iDutFailCnt += m_step_fail_block_cnt[(iDut%m_nMaxDutPerCpu) * m_nCePerDut + iCe];
			}
			PRINTF("----- DUT%03d m_step_fail_block_cnt = %d", iDut, iDutFailCnt );
		}
	}
	return 0;
}

int CApi::GET_EXTFM_L1_RESULT(int limit, bool fulladdr, int count_limit, bool all_address_processing, int chip_limit, bool noceaddr )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

int CApi::UnitMajority(std::vector<uint> &vBlkAddr, uint uAddrCnt)
{
	if ( vBlkAddr.size() < 1 ) return -1;


	for( uint iCriteria=0; iCriteria<vBlkAddr.size(); ++iCriteria)
	{
		uint uMatch = 0;
		for( uint i=0; i<vBlkAddr.size(); ++i)
		{
			if ( iCriteria==i ) continue;
			if ( vBlkAddr[iCriteria] == vBlkAddr[i] )
			{
				++uMatch;
				if ( uMatch >= (uAddrCnt-1) )
				{
					return vBlkAddr[iCriteria];
				}
			}
		}
	}

	return -1;
}

bool CApi::IsSetBlk( std::vector<int> &lstSettedBlk, int iBlk)
{
	for(uint i=0; i<lstSettedBlk.size(); ++i )
	{
		if ( lstSettedBlk[i] == iBlk )
			return true;
	}

	return false;
}

int CApi::GET_EXTFM_L1_RESULT_MAJORITY(int limit, bool fulladdr, int count_limit, bool all_address_processing, int chip_limit, bool printf, int addrcnt, int failcountrange )
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	bool bFC[USE_FC] = { false, false };
	int  nFC[2] = { 0x0, 0x0 };
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		nFC[0] = g_dfm.ReadExtFmCompFcnt();
		nFC[1] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
		nFC[0] = g_dfm.ReadExtFmFcnt(FC00);
		nFC[1] = g_dfm.ReadExtFmFcnt(FC01);
	}
//	PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, nFC[0] );
//	PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, nFC[1] );

	for( int fc=0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon

		if( nFC[fc] > limit )
		{
			for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
			{
				if( IS_PRE_FAIL( BIB0, dut) ) continue;
				if( GET_FC(dut) != fc       ) continue;

				//2023.05.12 - fatric
				for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
				{
					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
					{
						SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
					}
				}
				SET_STEP_FAIL( BIB0, dut, true );
				SET_CUM_FAIL ( BIB0, dut ,true );
				//SET_TEST_FAIL( BIB0, dut, true );		//2023.05.12 - fatric(mask)
			}
			bFC[fc] = true;
		}

		if ( nFC[fc] < failcountrange*addrcnt )
		{
			//EPRINTF("%s error : fc:%d FAIL_COUNT: %d   addrcnt:%d  failcountrange: %d error!",
			//		__FUNCTION__, fc, nFC[fc], addrcnt, failcountrange);
			bFC[fc] = true;
		}

	}
	if ( bFC[FC00] && bFC[FC01] ) return 0;


	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		if ( !bFC[FC00] || !bFC[FC01] ) g_dfm.ReadExtFmComp();
	}
	else
	{
		if( !bFC[FC00] ) g_dfm.ReadExtFm(FC00);
		if( !bFC[FC01] ) g_dfm.ReadExtFm(FC01);
	}

//	if( bprint )
//	{
//		g_dfm.PrintExtFm( FC00, nFC[0] );
//		g_dfm.PrintExtFm( FC01, nFC[1] );
//	}

	std::vector<int> vSettedBlk[MAX_DUT];
	uint uBlkMask = (1 << m_addBlkShift) - 1;	//m_addBlkShift: 11 -> 0x7ff
	for( int fc=0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
		if ( nFC[fc] < 1 ) continue;	//2023.01.02 - kwmoon

		FM_MEM* pFm = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFC[fc];f++ )
		{
			STOP_BREAK;		//2023.04.27 - kwmoon
			if ( printf )
			{
				PRINTF("%s:: FC[%d] cnt:%8d scan:%02x EXT_FM_ADD[%4d(0x%04x)]= IOB:%04x%04x%04x IOA:%04x%04x%04x",
					__FUNCTION__, fc, f/2*8, pFm[f].header.scan, 0, 0, pFm[f].data[2].ioab>>16   , pFm[f].data[1].ioab>>16   , pFm[f].data[0].ioab>>16,
																	   pFm[f].data[2].ioab&0xffff, pFm[f].data[1].ioab&0xffff, pFm[f].data[0].ioab&0xffff );
			}

//PRINTF("MAJ-------1");
			if ( f < failcountrange                               ) continue;
			if ( ! all_address_processing && (f%failcountrange)<2 ) continue;

//PRINTF("MAJ-------2");

			int iCurrIdx  = f;
			int iReadUnit = 2;
			if ( ! all_address_processing ) iCurrIdx -= 2;
			if ( fulladdr                 ) iReadUnit = 3;

			if ( (iCurrIdx%iReadUnit)      <  (iReadUnit-1) ) continue;
			if ( (iCurrIdx%failcountrange) >=  count_limit  ) continue;

//PRINTF("MAJ-------3");
			int iCurrentUnit = iCurrIdx / failcountrange;
			if ( (iCurrentUnit+1) < addrcnt ) continue;;
//PRINTF("MAJ-------4");

			uint realscan = pFm[f].header.scan;
			uint iCe      = realscan % m_mcp;
			uint iCeBlk   = iCe * m_nBlockPerCe;

			//pFm[f].data[seq].ioa_l
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					uint iopin = 0x0;
//					if( pFm[f].data[seq].ioa_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						//2023.05.12 - fatric(modify)
						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							std::vector<uint>	vBlkAddr;
							for(int iUnit=0; iUnit<=iCurrentUnit; ++iUnit)
							{
								uint uBlkAddr = 0;
								for( int iRead=0; iRead<iReadUnit; ++iRead )
								{
									uBlkAddr |= pFm[ (f-iRead) - (iUnit*failcountrange) ].data[seq].ioa_l << (iRead*8);
//									PRINTF("\tpFm[%d]", (f-iRead) - (iUnit*failcountrange) );
								}

								if ( (uBlkAddr&uBlkMask) > (uint)chip_limit ) continue;	//MAX Check

								vBlkAddr.push_back( uBlkAddr );
							}

//							PRINTF("ioa_l: vBlkAddr.size=%d  addrcnt=%d", (int)vBlkAddr.size(), addrcnt );

							if ((int)vBlkAddr.size() >= addrcnt )
							{
								int iBlkAddr = UnitMajority(vBlkAddr, addrcnt);
//								PRINTF("ioa_l: iBlkAddr=%x  IsSetBlk=%s", iBlkAddr, (IsSetBlk(vSettedBlk[dut], iBlkAddr) ? "true" : "false") );
								if ( iBlkAddr > 0 && ! IsSetBlk(vSettedBlk[dut], iBlkAddr+iCeBlk) )
								{
if ( 1 && (uint)iBlkAddr+iCeBlk >= m_nBlockPerDut )
	PRINTF("%s::%d error  iBlkAddr=%04X iCeBlk=%04X m_nBlockPerDut=%04x ioa_l: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", __FUNCTION__, __LINE__, iBlkAddr, iCeBlk, m_nBlockPerDut, f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									SET_STEP_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);
									if ( ! IS_CUM_EXCL_BLOCK() )
										SET_CUM_BLOCK_FAIL (BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( m_fail_mode == INIT_FAIL )
										SET_INIT_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( printf )
									{
										PRINTF("ioa_l: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									}
									vSettedBlk[dut].push_back(iBlkAddr+iCeBlk);
								}
							}
						}
					}

//					if( pFm[f].data[seq].ioa_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						//2023.05.12 - fatric(modify)
						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							std::vector<uint>	vBlkAddr;
							for(int iUnit=0; iUnit<=iCurrentUnit; ++iUnit)
							{
								uint uBlkAddr = 0;
								for( int iRead=0; iRead<iReadUnit; ++iRead )
								{
									uBlkAddr |= pFm[ (f-iRead) - (iUnit*failcountrange) ].data[seq].ioa_h << (iRead*8);
//									PRINTF("\tpFm[%d]", (f-iRead) - (iUnit*failcountrange) );
								}

								if ( (uBlkAddr&uBlkMask) > (uint)chip_limit ) continue;	//MAX Check

								vBlkAddr.push_back( uBlkAddr );
							}

//							PRINTF("ioa_h: vBlkAddr.size=%d  addrcnt=%d", (int)vBlkAddr.size(), addrcnt );
							if ((int)vBlkAddr.size() >= addrcnt )
							{
								int iBlkAddr = UnitMajority(vBlkAddr, addrcnt);
//								PRINTF("ioa_h: iBlkAddr=%x  IsSetBlk=%s", iBlkAddr, (IsSetBlk(vSettedBlk[dut], iBlkAddr) ? "true" : "false") );
								if ( iBlkAddr > 0 && ! IsSetBlk(vSettedBlk[dut], iBlkAddr+iCeBlk) )
								{
if ( 1 && (uint)iBlkAddr+iCeBlk >= m_nBlockPerDut )
	PRINTF("%s::%d error  iBlkAddr=%04X iCeBlk=%04X m_nBlockPerDut=%04x ioa_h: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", __FUNCTION__, __LINE__, iBlkAddr, iCeBlk, m_nBlockPerDut, f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									SET_STEP_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);
									if ( ! IS_CUM_EXCL_BLOCK() )
										SET_CUM_BLOCK_FAIL (BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( m_fail_mode == INIT_FAIL )
										SET_INIT_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( printf )
									{
										PRINTF("ioa_h: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									}
									vSettedBlk[dut].push_back(iBlkAddr+iCeBlk);
								}
							}
						}
					}
				}
			}

			//pFm[f].data[seq].iob_l
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon

				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					uint iopin = 0x0;
//					if( pFm[f].data[seq].iob_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						//2023.05.12 - fatric(modify)
						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							std::vector<uint>	vBlkAddr;
							for(int iUnit=0; iUnit<=iCurrentUnit; ++iUnit)
							{
								uint uBlkAddr = 0;
								for( int iRead=0; iRead<iReadUnit; ++iRead )
								{
									uBlkAddr |= pFm[ (f-iRead) - (iUnit*failcountrange) ].data[seq].iob_l << (iRead*8);
//									PRINTF("\tpFm[%d]", (f-iRead) - (iUnit*failcountrange) );
								}

								if ( (uBlkAddr&uBlkMask) > (uint)chip_limit ) continue;	//MAX Check

								vBlkAddr.push_back( uBlkAddr );
							}

//							PRINTF("iob_l: vBlkAddr.size=%d  addrcnt=%d", (int)vBlkAddr.size(), addrcnt );
							if ((int)vBlkAddr.size() >= addrcnt )
							{
								int iBlkAddr = UnitMajority(vBlkAddr, addrcnt);
//								PRINTF("iob_l: iBlkAddr=%x  IsSetBlk=%s", iBlkAddr, (IsSetBlk(vSettedBlk[dut], iBlkAddr) ? "true" : "false") );
								if ( iBlkAddr > 0 && ! IsSetBlk(vSettedBlk[dut], iBlkAddr+iCeBlk) )
								{
if ( 1 && (uint)iBlkAddr+iCeBlk >= m_nBlockPerDut )
	PRINTF("%s::%d error  iBlkAddr=%04X iCeBlk=%04X m_nBlockPerDut=%04x iob_l: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", __FUNCTION__, __LINE__, iBlkAddr, iCeBlk, m_nBlockPerDut, f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									SET_STEP_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);
									if ( ! IS_CUM_EXCL_BLOCK() )
										SET_CUM_BLOCK_FAIL (BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( m_fail_mode == INIT_FAIL )
										SET_INIT_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( printf )
									{
//										PRINTF("cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:08x setted", f/2*8, f, 0, dut, iBlkAddr);
										PRINTF("iob_l: index:%d dut_index:%d ceblock:%08x setted", 0, dut, iBlkAddr);
									}
									vSettedBlk[dut].push_back(iBlkAddr);
								}
							}
						}
					}

//					if( pFm[f].data[seq].iob_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							std::vector<uint>	vBlkAddr;
							for(int iUnit=0; iUnit<=iCurrentUnit; ++iUnit)
							{
								uint uBlkAddr = 0;
								for( int iRead=0; iRead<iReadUnit; ++iRead )
								{
									uBlkAddr |= pFm[ (f-iRead) - (iUnit*failcountrange) ].data[seq].iob_h << (iRead*8);
//									PRINTF("\tpFm[%d]", (f-iRead) - (iUnit*failcountrange) );
								}

								if ( (uBlkAddr&uBlkMask) > (uint)chip_limit ) continue;	//MAX Check

								vBlkAddr.push_back( uBlkAddr );
							}

//							PRINTF("iob_h: vBlkAddr.size=%d  addrcnt=%d", (int)vBlkAddr.size(), addrcnt );
							if ((int)vBlkAddr.size() >= addrcnt )
							{
								int iBlkAddr = UnitMajority(vBlkAddr, addrcnt);
//								PRINTF("iob_h: iBlkAddr=%x  IsSetBlk=%s", iBlkAddr, (IsSetBlk(vSettedBlk[dut], iBlkAddr) ? "true" : "false") );
								if ( iBlkAddr > 0 && ! IsSetBlk(vSettedBlk[dut], iBlkAddr+iCeBlk) )
								{
if ( 1 && (uint)iBlkAddr+iCeBlk >= m_nBlockPerDut )
	PRINTF("%s::%d error  iBlkAddr=%04X iCeBlk=%04X m_nBlockPerDut=%04x iob_h: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", __FUNCTION__, __LINE__, iBlkAddr, iCeBlk, m_nBlockPerDut, f/2*8, f/2*8/4, 0, dut, iBlkAddr);
									SET_STEP_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);
									if ( ! IS_CUM_EXCL_BLOCK() )
										SET_CUM_BLOCK_FAIL (BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( m_fail_mode == INIT_FAIL )
										SET_INIT_BLOCK_FAIL(BIB0, dut, (uint)iBlkAddr+iCeBlk, true);

									if ( printf )
									{
//										PRINTF("cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:08x setted", f/2*8, f, 0, dut, iBlkAddr);
										PRINTF("iob_h: index:%d dut_index:%d ceblock:%08x setted", 0, dut, iBlkAddr);
									}
									vSettedBlk[dut].push_back(iBlkAddr);
								}
							}
						}
					}
				}
			}

		}
	}

//	SET_PF_CE( NORMAL );

	m_bBlockFailAddr = true;
	return 0;
}

//2023.05.12 - fatric
int CApi::GET_EXTFM_L1_RESULT_SP(int limit, bool fulladdr, int count_limit, bool all_address_processing, int chip_limit, bool printf, bool special_add_ce_addr)
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	bool bFC[USE_FC] = { false, false };
	int  nFC[2] = { 0x0, 0x0 };
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		nFC[0] = g_dfm.ReadExtFmCompFcnt();
		nFC[1] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
		nFC[0] = g_dfm.ReadExtFmFcnt(FC00);
		nFC[1] = g_dfm.ReadExtFmFcnt(FC01);
	}

	for( int fc=0;fc<USE_FC;fc++ )
	{
		if( printf )
			PRINTF("%s::FC:%d READ_EXTFM_FCNT():%d ", __FUNCTION__, fc, nFC[fc]);

		if( nFC[fc] > limit )
		{
			PRINTF("External FM Count is overflow(MAX=). FC:%d CNT:%d",limit , fc, nFC[fc]);

			for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
			{
				if( IS_PRE_FAIL( BIB0, dut) ) continue;
				if( GET_FC(dut) != fc       ) continue;

				for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
				{
					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
					{
						SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
					}
				}
				SET_STEP_FAIL( BIB0, dut, true );
				SET_CUM_FAIL ( BIB0, dut ,true );
			}
			bFC[fc] = true;
		}
	}
	if ( bFC[FC00] && bFC[FC01] ) return 0;


	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		if ( !bFC[FC00] || !bFC[FC01] ) g_dfm.ReadExtFmComp();
	}
	else
	{
		if( !bFC[FC00] ) g_dfm.ReadExtFm(FC00);
		if( !bFC[FC01] ) g_dfm.ReadExtFm(FC01);
	}

//	if( bprint )
//	{
//		g_dfm.PrintExtFm( FC00, nFC[0] );
//		g_dfm.PrintExtFm( FC01, nFC[1] );
//	}

	std::vector<int> vSettedBlk[MAX_DUT];
	uint uBlkMask = (1 << m_addBlkShift) - 1;	//m_addBlkShift: 11 -> 0x7ff
	for( int fc=0;fc<USE_FC;fc++ )
	{
		if ( nFC[fc] < 1 ) continue;	//2023.01.02 - kwmoon

		FM_MEM* pFm = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFC[fc];f++ )
		{
			if ( printf )
			{
				PRINTF("%s:: FC[%d] cnt:%8d scan:%02x Addr:0x%06x EXT_FM_ADD[%4d]= IOB:%04x%04x%04x IOA:%04x%04x%04x",
					__FUNCTION__, fc, f/2*8, pFm[f].header.scan, pFm[f].header.address, f, pFm[f].data[2].ioab>>16   , pFm[f].data[1].ioab>>16   , pFm[f].data[0].ioab>>16,
																	   pFm[f].data[2].ioab&0xffff, pFm[f].data[1].ioab&0xffff, pFm[f].data[0].ioab&0xffff );
			}

//PRINTF("MAJ-------1");
			if ( ! all_address_processing && f<2 ) continue;

//PRINTF("MAJ-------2");

			int iCurrIdx  = f;
			int iReadUnit = 2;
			if ( ! all_address_processing ) iCurrIdx -= 2;
			if ( fulladdr                 ) iReadUnit = 3;

			if ( (iCurrIdx%iReadUnit)      <  (iReadUnit-1) ) continue;

//PRINTF("MAJ-------4");

			uint realscan = pFm[f].header.scan;
			uint iCe      = realscan % m_mcp;
			uint iCeBlk   = iCe * m_nBlockPerCe;
			if ( special_add_ce_addr ) iCeBlk = 0;
			else                       iCeBlk = 0;	//??

			//pFm[f].data[seq].ioa_l
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					uint iopin = 0x0;
//					if( pFm[f].data[seq].ioa_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							uint uBlkAddr = 0;
							for( int iRead=0; iRead<iReadUnit; ++iRead )
							{
								uBlkAddr |= pFm[ f-iRead ].data[seq].ioa_l << (iRead*8);
//								PRINTF("\tpFm[%d]", f-iRead );
							}

							if ( (uBlkAddr&uBlkMask) <= (uint)chip_limit )	//MAX Check
							{
								SET_STEP_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);
								if ( ! IS_CUM_EXCL_BLOCK() )
									SET_CUM_BLOCK_FAIL (BIB0, dut, uBlkAddr-iCeBlk, true);

								if ( m_fail_mode == INIT_FAIL )
									SET_INIT_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);

//								if ( printf )
//								{
//									PRINTF("ioa_l: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, uBlkAddr);
//								}
							}
							else
							{
								if ( (fulladdr && uBlkAddr!=0xffffff) || (!fulladdr && uBlkAddr!=0xffff) )
									PRINTF("%s:: Error!! dut_index:%d cnt:%8d block:0x%0x is chip limit:0x%x over chip_limit_mask:0x%x" , __FUNCTION__, dut, f, (uBlkAddr&uBlkMask), chip_limit, uBlkMask);
							}
						}
					}

//					if( pFm[f].data[seq].ioa_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							uint uBlkAddr = 0;
							for( int iRead=0; iRead<iReadUnit; ++iRead )
							{
								uBlkAddr |= pFm[ f-iRead ].data[seq].ioa_h << (iRead*8);
//								PRINTF("\tpFm[%d]", f-iRead );
							}

							if ( (uBlkAddr&uBlkMask) <= (uint)chip_limit )	//MAX Check
							{
								SET_STEP_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);
								if ( ! IS_CUM_EXCL_BLOCK() )
									SET_CUM_BLOCK_FAIL (BIB0, dut, uBlkAddr-iCeBlk, true);

								if ( m_fail_mode == INIT_FAIL )
									SET_INIT_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);

//								if ( printf )
//								{
//									PRINTF("ioa_h: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, uBlkAddr);
//								}
							}
							else
							{
								if ( (fulladdr && uBlkAddr!=0xffffff) || (!fulladdr && uBlkAddr!=0xffff) )
									PRINTF("%s:: Error!! dut_index:%d cnt:%8d block:0x%0x is chip limit:0x%x over chip_limit_mask:0x%x" , __FUNCTION__, dut, f, (uBlkAddr&uBlkMask), chip_limit, uBlkMask);
							}
						}
					}
				}
			}

			//pFm[f].data[seq].iob_l
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					uint iopin = 0x0;
//					if( pFm[f].data[seq].iob_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							uint uBlkAddr = 0;
							for( int iRead=0; iRead<iReadUnit; ++iRead )
							{
								uBlkAddr |= pFm[ f-iRead ].data[seq].iob_l << (iRead*8);
//								PRINTF("\tpFm[%d]", f-iRead );
							}

							if ( (uBlkAddr&uBlkMask) <= (uint)chip_limit )	//MAX Check
							{
								SET_STEP_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);
								if ( ! IS_CUM_EXCL_BLOCK() )
									SET_CUM_BLOCK_FAIL (BIB0, dut, uBlkAddr-iCeBlk, true);

								if ( m_fail_mode == INIT_FAIL )
									SET_INIT_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);

//								if ( printf )
//								{
//									PRINTF("iob_l: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, uBlkAddr);
//								}
							}
							else
							{
								if ( (fulladdr && uBlkAddr!=0xffffff) || (!fulladdr && uBlkAddr!=0xffff) )
									PRINTF("%s:: Error!! dut_index:%d cnt:%8d block:0x%0x is chip limit:0x%x over chip_limit_mask:0x%x" , __FUNCTION__, dut, f, (uBlkAddr&uBlkMask), chip_limit, uBlkMask);
							}
						}
					}

//					if( pFm[f].data[seq].iob_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( ! IS_PRE_FAIL( BIB0, dut) )
						{
							uint uBlkAddr = 0;
							for( int iRead=0; iRead<iReadUnit; ++iRead )
							{
								uBlkAddr |= pFm[ f-iRead ].data[seq].iob_h << (iRead*8);
//								PRINTF("\tpFm[%d]", f-iRead );
							}

							if ( (uBlkAddr&uBlkMask) <= (uint)chip_limit )	//MAX Check
							{
								SET_STEP_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);
								if ( ! IS_CUM_EXCL_BLOCK() )
									SET_CUM_BLOCK_FAIL (BIB0, dut, uBlkAddr-iCeBlk, true);

								if ( m_fail_mode == INIT_FAIL )
									SET_INIT_BLOCK_FAIL(BIB0, dut, uBlkAddr-iCeBlk, true);

//								if ( printf )
//								{
//									PRINTF("iob_h: cnt:%8d cnt/4:%d index:%d dut_index:%d ceblock:%08x setted", f/2*8, f/2*8/4, 0, dut, uBlkAddr);
//								}
							}
							else
							{
								if ( (fulladdr && uBlkAddr!=0xffffff) || (!fulladdr && uBlkAddr!=0xffff) )
									PRINTF("%s:: Error!! dut_index:%d cnt:%8d block:0x%0x is chip limit:0x%x over chip_limit_mask:0x%x" , __FUNCTION__, dut, f, (uBlkAddr&uBlkMask), chip_limit, uBlkMask);
							}
						}
					}
				}
			}

		}
	}

//	SET_PF_CE( NORMAL );

	m_bBlockFailAddr = true;
	return 0;
}

void CApi::GET_EXTFM_DATA(int limit, bool bprint)
{
	//temp
	//UTIL::TIMER timer1, timerFunc;
	//timerFunc.start();

	bool bFC[2] = { false, false };
	int  nFC[2] = { 0x0, 0x0 };
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		nFC[0] = g_dfm.ReadExtFmCompFcnt();
		nFC[1] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
		nFC[0] = g_dfm.ReadExtFmFcnt(FC00);
		nFC[1] = g_dfm.ReadExtFmFcnt(FC01);
	}

	for( int fc=0;fc<USE_FC;fc++ )
	{
		if( nFC[fc] > limit )
		{
			for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
			{
				if( IS_PRE_FAIL(BIB0, dut) ) continue;
				if( GET_FC(dut) != fc ) continue;
				SET_STEP_FAIL( BIB0, dut, true );
				SET_CUM_FAIL ( BIB0, dut ,true );
				SET_TEST_FAIL( BIB0, dut, true );
			}
			bFC[fc] = true;
		}
	}

	//2023.04.21 - fatric
	if( ! m_bVarAvail )
	{
		RUNTIME_ERROR( __FUNCTION__, "error : m_ext_fm_fail is not available!" );
		return;
	}

	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		g_dfm.ReadExtFmComp();
	}
	else
	{
		if( !bFC[FC00] )
		{
			g_dfm.ReadExtFm(FC00);
		}
		if( !bFC[FC01] )
		{
			g_dfm.ReadExtFm(FC01);
		}
	}

//	//Clear ext fm data
//	if( m_bVarAvail )
//	{
//		for( uint d=0;d<m_nMaxDutPerCpu;d++ )
//		for( uint d=0;d<m_nMaxDutPerCpu*2;d++ )
//			memset( m_ext_fm_fail[d], 0x0, sizeof(int)*EXTFM_SW_MAX_LIMIT );
//		memset( m_ext_fm_fail_count, 0x0, sizeof(int)*m_nMaxDutPerCpu );
//	}

	if( bprint )
	{
		PRINTF("[%s - FM DATA]", __FUNCTION__ );
		g_dfm.PrintExtFm( FC00, nFC[FC00] );
		g_dfm.PrintExtFm( FC01, nFC[FC01] );
	}
	//double dGetsiTime = 0;
	for( int fc=0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
		if( bFC[fc] ) continue;	//fail fc skip
		FM_MEM* pFm = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFC[fc];f++ )
		{
			uint iopin    = 0x0;
			uint realscan = pFm[f].header.scan;
			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
//					if( pFm[f].data[seq].ioa_l )
					if( pFm[f].data[seq].ioa_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						//timer1.start();
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						//dGetsiTime += timer1.getElapsedTime();

						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].ioa_l;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].ioa_l;	//append for copatible
//printf("GET_EXTFM_DATA ioa_l :m_ext_fm_fail[%d][%d]=0x%08X\n", dut%m_nMaxDutPerCpu, m_ext_fm_fail_count[dut%m_nMaxDutPerCpu], m_ext_fm_fail[dut%m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] );
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;

					}
//					if( pFm[f].data[seq].ioa_h )
					if( pFm[f].data[seq].ioa_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}
						//timer1.start();
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						//dGetsiTime += timer1.getElapsedTime();

						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].ioa_h;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].ioa_h;	//append for copatible
//printf("GET_EXTFM_DATA ioa_h :m_ext_fm_fail[%d][%d]=0x%08X\n", dut%m_nMaxDutPerCpu, m_ext_fm_fail_count[dut%m_nMaxDutPerCpu], m_ext_fm_fail[dut%m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] );
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
//					if( pFm[f].data[seq].iob_l )
					if( pFm[f].data[seq].iob_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						//timer1.start();
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						//dGetsiTime += timer1.getElapsedTime();

						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].iob_l;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].iob_l;	//append for copatible
//printf("GET_EXTFM_DATA iob_l :m_ext_fm_fail[%d][%d]=0x%08X\n", dut%m_nMaxDutPerCpu, m_ext_fm_fail_count[dut%m_nMaxDutPerCpu], m_ext_fm_fail[dut%m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] );
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
//					if( pFm[f].data[seq].iob_h )
					if( pFm[f].data[seq].iob_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}
						//timer1.start();
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						//dGetsiTime += timer1.getElapsedTime();

						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].iob_h;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = pFm[f].data[seq].iob_h;	//append for copatible
//printf("GET_EXTFM_DATA iob_h :m_ext_fm_fail[%d][%d]=0x%08X\n", dut%m_nMaxDutPerCpu, m_ext_fm_fail_count[dut%m_nMaxDutPerCpu], m_ext_fm_fail[dut%m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] );
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
				}
			}
		}
	}
	//if( 0 )
	//{
	//	m_dTmpTimeSum += dGetsiTime;
	//	PRINTF("%s GetSiToIndex Time:%10.5f US", __FUNCTION__, dGetsiTime * 1.0e6 );
	//}
}

void CApi::GET_EXTFM_SCAN_ADDR(int limit, bool dupcheck, int escape, bool bprint)
{
	int  nFC[2] = { 0x0, 0x0 };
	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		nFC[0] = g_dfm.ReadExtFmCompFcnt();
		nFC[1] = g_dfm.ReadExtFmCompFcnt();
	}
	else
	{
		nFC[0] = g_dfm.ReadExtFmFcnt(FC00);
		nFC[1] = g_dfm.ReadExtFmFcnt(FC01);
	}

	if ( m_debug_mode&0x40 )
	{
		PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, nFC[0] );
		PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, nFC[1] );
	}

	if( static_cast<int>(nFC[FC00]) > limit )
	{
		RUNTIME_ERROR( __FUNCTION__, "runtime-error : FC0 FM fail count(%d) limit over %d\n", nFC[FC00], limit );
	}
	if( static_cast<int>(nFC[FC01]) > limit )
	{
		RUNTIME_ERROR( __FUNCTION__, "runtime-error : FC1 FM fail count(%d) limit over %d\n", nFC[FC01], limit );
	}

	//2023.04.21 - fatric
	if( ! m_bVarAvail )
	{
		RUNTIME_ERROR( __FUNCTION__, "error : m_ext_fm_fail is not available!" );
		return;
	}

	if( g_dfm.GetFmModeGlobal() & EFM_COMP_WRITE_MODE )
	{
		if( !g_dfm.ReadExtFmComp() )
		{
			RUNTIME_ERROR( __FUNCTION__, "error : Ext FM COMP read!" );
			return;
		}
	}
	else
	{
		if( !g_dfm.ReadExtFm() )
		{
			RUNTIME_ERROR( __FUNCTION__, "error : Ext FM read!" );
			return;
		}
	}

//	//Clear ext fm data
//	if( m_bVarAvail )
//	{
////		for( uint d=0;d<m_nMaxDutPerCpu;d++ )
//		for( uint d=0;d<m_nMaxDutPerCpu*2;d++ )
//			memset( m_ext_fm_fail[d], 0x0, sizeof(int)*EXTFM_SW_MAX_LIMIT );
//		memset( m_ext_fm_fail_count, 0x0, sizeof(int)*m_nMaxDutPerCpu );
//	}

//	if( bprint )
//	{
//		g_dfm.PrintExtFm( FC00, nFC[0] );
//		g_dfm.PrintExtFm( FC01, nFC[1] );
//	}

	for( int fc=0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
		if ( bprint )
			PRINTF("%s::FC:%d CNT:%d", __FUNCTION__, fc, nFC[fc]*4 );

		if ( nFC[fc] < 1 ) continue;	//2023.01.02 - kwmoon
		FM_MEM* pFm = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFC[fc];f++ )
		{
			STOP_BREAK;		//2023.04.27 - kwmoon
			uint scanaddress  = pFm[f].header.header;
			uint address      = pFm[f].header.address;
			uint realscan     = pFm[f].header.scan;
			bool bProc[3] = { false, false, false };

			//YBKIM
//			if ( m_debug_mode&0x100 && f < 1000 )
			if ( m_debug_mode&0x100 )
			{
				if ( m_debug_mode&0x200 )
				{
					if ( f < 1000 )
						PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
				}
				else
					PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
			}

			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
					uint iopin = 0x0;
//					if( pFm[f].data[seq].ioa_l )
					if( pFm[f].data[seq].ioa_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= static_cast<uint>(escape) ) continue;
						if( dupcheck) if( IsDuplicateFailData(dut%m_nMaxDutPerCpu, scanaddress) ) continue;
						bProc[seq] = true;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
//					if( pFm[f].data[seq].ioa_h )
					if( pFm[f].data[seq].ioa_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= static_cast<uint>(escape) ) continue;
						if( dupcheck) if( IsDuplicateFailData(dut%m_nMaxDutPerCpu, scanaddress) ) continue;
						bProc[seq] = true;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
//					if( pFm[f].data[seq].iob_l )
					if( pFm[f].data[seq].iob_l || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= static_cast<uint>(escape) ) continue;
						if( dupcheck) if( IsDuplicateFailData(dut%m_nMaxDutPerCpu, scanaddress) ) continue;
						bProc[seq] = true;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
//					if( pFm[f].data[seq].iob_h )
					if( pFm[f].data[seq].iob_h || g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							EPRINTF("   fc=%d nFC[fc]=%d f=[%6d] seq=[%d] realscan=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, nFC[fc], f, seq, realscan, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
							continue;
						}
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= EXTFM_SW_MAX_LIMIT ) continue;
						if( m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] >= static_cast<uint>(escape) ) continue;
						if( dupcheck) if( IsDuplicateFailData(dut%m_nMaxDutPerCpu, scanaddress) ) continue;
						bProc[seq] = true;
						m_ext_fm_fail[ dut%m_nMaxDutPerCpu                 ][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail[(dut%m_nMaxDutPerCpu)+m_nMaxDutPerCpu][ m_ext_fm_fail_count[dut%m_nMaxDutPerCpu] ] = scanaddress;
						m_ext_fm_fail_count[dut%m_nMaxDutPerCpu]++;
					}
				}
			}
////			if ( bProc[0] || bProc[1] || bProc[2] )
//			{
//				printf("GET_EXTFM_SCAN_ADDR:FC[%d] SCAN:%02d ADDR:%06X", fc, realscan, address);
//				printf("  [%6d] 2:0x%08X 1:0x%08X 0:0x%08X\n", f, pFm[f].data[2].ioab, pFm[f].data[1].ioab, pFm[f].data[0].ioab);
//			}

			if ( bprint && ( (pFm[f].data[2].ioab || pFm[f].data[1].ioab || pFm[f].data[0].ioab) || (g_dfm.GetFmModeGlobal() & ALL_WRITE_MODE) ) )
			{
				PRINTF("%s:: FC[%d] cnt:%8d scan:%02x Addr:0x%06x EXT_FM_ADD[%6d]= IOB:%04x%04x%04x IOA:%04x%04x%04x",
					__FUNCTION__, fc, f/2*8, realscan, address, f, pFm[f].data[2].ioab>>16   , pFm[f].data[1].ioab>>16   , pFm[f].data[0].ioab>>16,
																   pFm[f].data[2].ioab&0xffff, pFm[f].data[1].ioab&0xffff, pFm[f].data[0].ioab&0xffff );

			}
		}
	}

	m_bBlockFailAddr = true;
}

void CApi::SET_EXTFM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int address, unsigned int scan,bool dupcheck)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_EXTFM_FAIL(unsigned int board, unsigned int dut, unsigned int address, unsigned int scan, bool dupcheck)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned int CApi::GET_EXTFM_FCNT(unsigned int dut_index )
{
	if( !IS_MYDUT(dut_index) ) return 0;

	if( !m_bVarAvail || !m_ext_fm_fail_count )
	{
		EPRINTF( "%s please SET_BIB/SET_CE/SET_LIMIT setting please!", __FUNCTION__ );
		return 0;
	}

	if( dut_index > BIB_X()*BIB_Y() )
	{
		EPRINTF( "%s dut index %d error, use[0~%d]", __FUNCTION__, dut_index, BIB_X()*BIB_Y() );
		return 0;
	}
	return m_ext_fm_fail_count[dut_index%m_nMaxDutPerCpu];
}

unsigned int CApi::GET_EXTFM_FAIL(unsigned int board, unsigned int dut_index, unsigned int cnt )
{
	if( !IS_MYDUT(dut_index) ) return 0;

	if(( m_iVarConf != 7 ) || (!m_ext_fm_fail) )
	{
		EPRINTF( "%s please SET_BIB/SET_CE/SET_LIMIT setting please!", __FUNCTION__ );
		return 0;
	}
	if( dut_index > BIB_X()*BIB_Y() )
	{
		EPRINTF( "%s dut index %d error, use[0~%d]", __FUNCTION__, dut_index, BIB_X()*BIB_Y() );
		return 0;
	}
	if( cnt > EXTFM_SW_MAX_LIMIT )
	{
		EPRINTF( "%s fail cnt %d error, use[0~%d]", __FUNCTION__, dut_index, EXTFM_SW_MAX_LIMIT-1 );
		return 0;
	}
	return m_ext_fm_fail[dut_index%m_nMaxDutPerCpu][cnt];
}

unsigned int* CApi::GET_EXTFM_FAIL_POINTER(unsigned int board, unsigned int dut)
{
	if( !m_bVarAvail ) return nullptr;
	if( !IS_MYDUT(dut) ) return nullptr;
	return m_ext_fm_fail[dut%m_nMaxDutPerCpu];
}

bool CApi::IS_EXTFM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int column, unsigned int scan)
{
	uint dut = 0x0;
	if( !g_bib.GetXyToIndex( x_pos, y_pos, dut ) ) return false;
	return IS_EXTFM_FAIL( board, dut, column, scan );
}

//after GET_EXTFM_SCAN_ADDR
bool CApi::IS_EXTFM_FAIL(unsigned int board, unsigned int dut, unsigned int address, unsigned int scan)
{
	if( !IS_MYDUT(dut) ) return false;
	if( !m_bVarAvail ) return false;

	(void )board;
	uint loop_count = GET_EXTFM_FCNT(dut);
	for( uint i=0;i<loop_count;i++ )
	{
		uint addr  = GET_EXTFM_FAIL(BIB0, dut, i);
		uint rscan = (addr >> 24) & 0xFF;
		uint fma   = addr & 0xFFFFFF;
		if( (address == fma) && (scan == rscan) ) return true;
	}
	return false;
}

//Refrence df equip spec operation
void CApi::READ_EXTFM_DATA(int failth, unsigned int long long data[4])
{
	int nExtFmOffset = failth / 4;
	g_dfm.ReadExtFmData( m_FCAdd, nExtFmOffset, data );
}

void CApi::getExtFailData( int failth, unsigned int long long data[4] )
{
	READ_EXTFM_DATA( failth, data );
}

void CApi::READ_LATCH(int board, int ddr, unsigned int long long data[6])
{
	(void)board;
	if( (m_FCAdd != FC00) && (m_FCAdd != FC01) ) return;

	int fc = m_FCAdd;
	data[0] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_0_15  + fc*0x20 );
	data[1] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_16_31 + fc*0x20 );
	data[2] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_32_47 + fc*0x20 );
	if( ddr )
	{
		data[3] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_0_15  + fc*0x20 );
		data[4] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_16_31 + fc*0x20 );
		data[5] = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_32_47 + fc*0x20 );
	}
}

void CApi::SET_RT_MODE_GLOBAL( int mode )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_FAILLIMIT_GLOBAL(unsigned int limit)
{
	g_dfm.SetFailLimitGlobal( limit );
}

void CApi::SET_FAILMULTIIO_GLOBAL(unsigned int multiio)
{
	g_dfm.SetFailMultiIoGlobal( multiio );
}


int CApi::GET_NBYTE_MATCH(double &result, char *base, unsigned int chip_index, unsigned int byte_index, unsigned int max_byte)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

void CApi::GET_EXTFM_ECC_FCNT(int limit, bool printf, bool dont_clear )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::CLR_CHUNK_FCNT()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::GET_EXTFM_TMPFM(int limit, bool printf)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::GET_EXTFM_DUTFM(int limit, bool printf )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_EXTFM_ECC_GLOBAL(int fail_limit, int multi_io, bool fail_count_save_mode)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned short CApi::GET_CHUNK_FCNT( unsigned int dut_index, int chunk )
{
	unsigned short fcnt = 0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return fcnt;
}

void CApi::SET_EXTFM_AUTOMASK_GLOBAL(int fail_limit, int multi_io )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned char CApi::TMP_FM_DATA( int board, unsigned int io, unsigned index )
{
	if( board != BIB0  ) board = BIB0;
	if( (io < 0) || (io>BIB_IO()) )
	{
		EPRINTF( "%s error : io index (%d) invalid, use [%d ~ %d]", __FUNCTION__, io, 0, BIB_IO()-1 );
		return 0x0;
	}
	if( (index < 0) || (index>1023) )
	{
		EPRINTF( "%s error : data index (%d) invalid, use [%d ~ %d]", __FUNCTION__, index, 0, 1023 );
		return 0x0;
	}
	return m_tmp_fm_data[board][io][index];
}

//Added function for get io group by io pin
bool CApi::GET_IO_GRP( uint fc, uint ioab, uint iopin, uint& iogrp )
{
	return g_bib.GetIoGrp( GetCurrentCpuNo(), fc, ioab, iopin, iogrp );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OUT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::out_init_result()
{
	bool is_pgpause = IS_PGPAUSE();
	bool bWrite = g_dfm.GetFmWrite();

	if( is_pgpause )
		g_dfm.FmWriteGlobal( false );	//EXT FM READ MODE CHANGE

	//Set step fail, cum fail
	FM_MEM	lData;	//LATCH DATA
	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
		//1. Get Latch Data
		lData.header.header = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_REAL_SCAN    + fc*0x20 );
		lData.data[0].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_0_15  + fc*0x20 );
		lData.data[1].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_16_31 + fc*0x20 );
		lData.data[2].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_32_47 + fc*0x20 );
		if ( m_fail_mode == MUST_FAIL )
		{
			lData.data[0].ioab = ~lData.data[0].ioab;
			lData.data[1].ioab = ~lData.data[1].ioab;
			lData.data[2].ioab = ~lData.data[2].ioab;
		}

		if( g_pg.GetIoDdrModeGlobal() == DDR )
		{
			FM_MEM	lDataF;
			lDataF.data[0].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_0_15  + fc*0x20 );
			lDataF.data[1].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_16_31 + fc*0x20 );
			lDataF.data[2].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_32_47 + fc*0x20 );
			if ( m_fail_mode == MUST_FAIL )
			{
				lDataF.data[0].ioab = ~lDataF.data[0].ioab;
				lDataF.data[1].ioab = ~lDataF.data[1].ioab;
				lDataF.data[2].ioab = ~lDataF.data[2].ioab;
			}
			lData.data[0].ioab |= lDataF.data[0].ioab;
			lData.data[1].ioab |= lDataF.data[1].ioab;
			lData.data[2].ioab |= lDataF.data[2].ioab;
		}

		uint iopin    = 0x0;
		uint realscan = lData.header.header;	//Real Scan Value: low 8bit
		uint iCe      = realscan % m_mcp;

		if ( m_debug_mode&0x100 ) PRINTF("fc=%d realscan=#%08X   iCe=%02X   ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, realscan, iCe, lData.data[0].ioab, lData.data[1].ioab, lData.data[2].ioab );

		for( int seq=0;seq<3;seq++ )	//ioa/b 96io
		{
			STOP_BREAK;		//2023.04.27 - kwmoon
			uint iogrp = 0x0;
			uint dut   = 0x0;
			uint seq_io_offset = 16;
			if( g_bib.GetBibIo() == 8 )
			{
				if( lData.data[seq].ioa_l )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}
					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
				if( lData.data[seq].ioa_h )
				{
					iopin = seq_io_offset*seq + g_bib.GetBibIo();
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}
					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
				if( lData.data[seq].iob_l )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}

					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}

//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
				if( lData.data[seq].iob_h )
				{
					iopin = seq_io_offset*seq + g_bib.GetBibIo();
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}

					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
			}
			else if( g_bib.GetBibIo() == 16 )
			{
				if( lData.data[seq].ioab&0x0000ffff )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}
					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
					}
//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
				if( lData.data[seq].ioab&0xffff0000 )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}

					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
					}

//PRINTF("%s:%4d dut%03d-------", __FUNCTION__, __LINE__, dut );
					if ( ! IS_PRE_FAIL(BIB0, dut) )
					{
						SET_STEP_FAIL( BIB0, dut, true );
						SET_CUM_FAIL ( BIB0, dut, true );
//						SET_TEST_FAIL( BIB0, dut, true );
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, dut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, dut, iCe*m_nChipPerCe+iChip, true);
						}
					}
				}
			}
		}
	}
	DISPLAY_LATCH();
	m_bBlockFailAddr = true;
	g_pg.CmpClear();

	if( is_pgpause )
		g_dfm.FmWriteGlobal( bWrite );	//EXT FM READ MODE CHANGE
}

void CApi::out_fm_read_data()
{
	uint fc_fm_count[USE_FC] = { 0x0, };
	uint fail_count[MAX_IO]  = { 0x0, };
	bool is_pgpause = IS_PGPAUSE();
	bool bWrite = g_dfm.GetFmWrite();
	if( is_pgpause )
	{
		g_dfm.FmWriteGlobal( false );	//EXT FM READ MODE CHANGE
		//PRINTF("%s FM_WRITE_GLOBAL(%s)", __FUNCTION__, is_pgpause?"FALSE":"TRUE" );
	}

	fc_fm_count[FC00] = g_dfm.ReadExtFmFcnt(FC00);
	fc_fm_count[FC01] = g_dfm.ReadExtFmFcnt(FC01);
	if( !g_dfm.ReadExtFm() )
	{
		RUNTIME_ERROR( __FUNCTION__, "error : Ext FM read!" );
		return;
	}
	if( 0 )
	{
		g_dfm.PrintExtFm( FC00, fc_fm_count[FC00] );
		g_dfm.PrintExtFm( FC01, fc_fm_count[FC01] );
	}

	if ( m_debug_mode&0x40 )
	{
		PRINTF("%s FC1 FAIL_COUNT : %d", __FUNCTION__, fc_fm_count[FC00] );
		PRINTF("%s FC2 FAIL_COUNT : %d", __FUNCTION__, fc_fm_count[FC01] );
	}

	memset( fail_count, 0x0, sizeof(fail_count) );
	uint realscan  = 0x0;
	uint blockaddr = 0x0;
	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		STOP_BREAK;		//2023.04.27 - kwmoon
		int nFailCount = fc_fm_count[fc];
		FM_MEM* pFm    = g_dfm.GetFmFailMemory(fc);
		for( int f=0;f<nFailCount;f++ )
		{
			STOP_BREAK;		//2023.04.27 - kwmoon
			uint iopin = 0x0;
			realscan   = pFm[f].header.scan;
			blockaddr  = pFm[f].header.address;

			//YBKIM
//			if ( m_debug_mode&0x100 && f < 1000 )
			if ( m_debug_mode&0x100 )
			{
				if ( m_debug_mode&0x200 )
				{
					if ( f < 1000 )
						PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   blockaddr=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, fc_fm_count[fc], f, realscan, blockaddr, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
				}
				else
					PRINTF("fc=%d nFC[fc]=%d f=[%6d] realscan=#%08X   blockaddr=#%08X   header=%08X ioab[0]=%08X   ioab[1]=%08X   ioab[2]=%08X", fc, fc_fm_count[fc], f, realscan, blockaddr, pFm[f].header.header, pFm[f].data[0].ioab, pFm[f].data[1].ioab, pFm[f].data[2].ioab );
			}

			for( int seq=0;seq<3;seq++ )	//ioa/b 96io
			{
				STOP_BREAK;		//2023.04.27 - kwmoon
				const uint SEQ_PIN_OFFSET = g_bib.GetBibIo() * 2;
				uint iogrp = 0x0;
				uint dut   = 0x0;
				if( g_bib.GetBibIo() == 8 )
				{
					//if( pFm[f].data[seq].ioa_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						if( IS_CHIPID_STEP() )
						{
							if( 0 ) PRINTF("%s REALSCAN:%02d IOA_L IOGRP:%02d IOGRP_FCNT:%03d IO:%02X", __FUNCTION__, realscan, iogrp, fail_count[iogrp], pFm[f].data[seq].ioa_l );
							m_tmp_fm_data[BIB0][iogrp][fail_count[iogrp]] = pFm[f].data[seq].ioa_l;
							fail_count[iogrp]++;
						}
					}
					//if( pFm[f].data[seq].ioa_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}
						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						if( IS_CHIPID_STEP() )
						{
							if( 0 ) PRINTF("%s REALSCAN:%02d IOA_H IOGRP:%02d IOGRP_FCNT:%03d IO:%02X", __FUNCTION__, realscan, iogrp, fail_count[iogrp], pFm[f].data[seq].ioa_h );
							m_tmp_fm_data[BIB0][iogrp][fail_count[iogrp]] = pFm[f].data[seq].ioa_h;
							fail_count[iogrp]++;
						}
					}
					//if( pFm[f].data[seq].iob_l )
					{
						iopin = SEQ_PIN_OFFSET*seq + 0;
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						if( IS_CHIPID_STEP() )
						{
							if( 0 ) PRINTF("%s REALSCAN:%02d IOB_L IOGRP:%02d IOGRP_FCNT:%03d IO:%02X", __FUNCTION__, realscan, iogrp, fail_count[iogrp], pFm[f].data[seq].iob_l );
							m_tmp_fm_data[BIB0][iogrp][fail_count[iogrp]] = pFm[f].data[seq].iob_l;
							fail_count[iogrp]++;
						}
					}
					//if( pFm[f].data[seq].iob_h )
					{
						iopin = SEQ_PIN_OFFSET*seq + g_bib.GetBibIo();
						if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
						{
							EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
							continue;
						}

						if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
						{
							EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
							continue;
						}
						if( IS_CHIPID_STEP() )
						{
							if( 0 ) PRINTF("%s REALSCAN:%02d IOB_H IOGRP:%02d IOGRP_FCNT:%03d IO:%02X", __FUNCTION__, realscan, iogrp, fail_count[iogrp], pFm[f].data[seq].iob_l );
							m_tmp_fm_data[BIB0][iogrp][fail_count[iogrp]] = pFm[f].data[seq].iob_h;
							fail_count[iogrp]++;
						}
					}
				}
				//rfu
				else if( g_bib.GetBibIo() == 16 )
				{
				}
			}
		}
	}
	if( is_pgpause )
	{
		g_dfm.FmWriteGlobal( bWrite );	//EXT FM READ MODE CHANGE
		//PRINTF("%s FM_WRITE_GLOBAL(%s)", __FUNCTION__, bWrite?"TRUE":"FALSE" );
	}
	//PRINTF( "IS_CHIPID_STEP() : %d", IS_CHIPID_STEP() );
	if( IS_CHIPID_STEP() )
	{
//		SET_FM_READ_DATA( realscan, blockaddr, m_nFmReadDataIndex );
		if ( fc_fm_count[FC00] > 0 || fc_fm_count[FC01] > 0 )	//2023-12-18 fatric
			SET_FM_READ_DATA( realscan, blockaddr, m_nFmReadDataIndex );
	}
	//PRINTF( "%s done!!", __FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DRV
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_DRV_IOMODE( bool enb )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

int CApi::READ_DRV_FCNT(int fc)
{
	int fcnt = 0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return fcnt;
}

int CApi::READ_DRV_SCAN(int fc, int order)
{
	int scan = 0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return scan;
}

int CApi::READ_DRV_DATA(int fc, int order)
{
	int data = 0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return data;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UBM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_UDATA_0x00()
{
	g_ubm.SetUdata0x00();
}

void CApi::SET_UDATA_0xFF()
{
	g_ubm.SetUdata0xFF();
}

void CApi::SET_UDATA32( unsigned int fc, int index, int index2, unsigned int data )
{
	if( (fc<0) || (fc>1) )
	{
		EPRINTF( "%s error : invalid FC number %d, use 0/1", __FUNCTION__, fc );
		return;
	}
	if( (index<0) || (index>UBM_DEPTH) )
	{
		EPRINTF( "%s error : invalid UBM address %d, use 0~%d", __FUNCTION__, index, UBM_DEPTH );
		return;
	}

	if( (index2<0) || (index2>2) )
	{
		EPRINTF( "%s error : invalid Sub UBM address %d, use 0/1/2", __FUNCTION__, index2 );
		return;
	}
	g_ubm.SetUdata32( fc, index, index2, data );
}

void CApi::SET_UDATA( unsigned int fc, unsigned int ioab, unsigned int index, unsigned long long int data )
{
	if( (fc<0) || (fc>1) )
	{
		EPRINTF( "%s error : invalid FC number %d, use 0/1", __FUNCTION__, fc );
		return;
	}
	if( (index<0) || (index>UBM_DEPTH) )
	{
		EPRINTF( "%s error : invalid UBM address %d, use 0~%d", __FUNCTION__, index, UBM_DEPTH );
		return;
	}
	if( (ioab<0) || (ioab>2) )
	{
		EPRINTF( "%s error : invalid UBM IOAB %d, use 0/1", __FUNCTION__, ioab );
		return;
	}
	g_ubm.SetUdata( fc, ioab, index, data );
}

void CApi::SET_UDATAOR( unsigned int fc, unsigned int ioab, int index, unsigned long long int data )
{
	if( (fc<0) || (fc>1) )
	{
		EPRINTF( "%s error : invalid FC number %d, use 0/1", __FUNCTION__, fc );
		return;
	}
	if( (index<0) || (index>UBM_DEPTH) )
	{
		EPRINTF( "%s error : invalid UBM address %d, use 0~%d", __FUNCTION__, index, UBM_DEPTH );
		return;
	}
	if( (ioab<0) || (ioab>2) )
	{
		EPRINTF( "%s error : invalid UBM IOAB %d, use 0/1", __FUNCTION__, ioab );
		return;
	}
	g_ubm.SetUdataOr( fc, ioab, index, data );
}

unsigned int CApi::GET_UDATA32( unsigned int fc, int index, int index2 )
{
	if( (fc<0) || (fc>1) )
	{
		EPRINTF( "%s error : invalid FC number %d, use 0/1", __FUNCTION__, fc );
		return 0;
	}
	if( (index<0) || (index>UBM_DEPTH) )
	{
		EPRINTF( "%s error : invalid UBM address %d, use 0~%d", __FUNCTION__, index, UBM_DEPTH );
		return 0;
	}

//	if( (index<0) || (index>2) )
	if( (index2<0) || (index2>2) )	//2023.01.02	fatric
	{
		EPRINTF( "%s error : invalid Sub UBM address %d, use 0/1/2", __FUNCTION__, index2 );
		return 0;
	}
	return g_ubm.GetUdata32( fc, index, index2 );
}

unsigned long long CApi::GET_UDATA( unsigned int fc, unsigned int ioab, int index )
{
	if( (fc<0) || (fc>1) )
	{
		EPRINTF( "%s error : invalid FC number %d, use 0/1", __FUNCTION__, fc );
		return 0LL;
	}
	if( (index<0) || (index>UBM_DEPTH) )
	{
		EPRINTF( "%s error : invalid UBM address %d, use 0~%d", __FUNCTION__, index, UBM_DEPTH );
		return 0LL;
	}
	if( (ioab<0) || (ioab>2) )
	{
		EPRINTF( "%s error : invalid UBM IOAB %d, use 0/1", __FUNCTION__, ioab );
		return 0LL;
	}
	return g_ubm.GetUdata( fc, ioab, index );
}

int CApi::SET_UBM( unsigned int dut )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

void CApi::SET_UDATADUT( unsigned int dut_index, int real_scan, unsigned int cnt, unsigned int data, int jump )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::PRINTF_UBM( unsigned int fc, unsigned int size )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VECTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::CLR_VECTOR()
{
	g_vector.ClearVector();
}

void CApi::SET_VECTOR_DATA( unsigned char* pData , unsigned int size )
{
	if( !g_vector.SetVectorData( pData, size ) )
	{
		return;
	}
	g_vector.WriteVector();
}

void CApi::SET_VECTOR_DATA( unsigned short* pData, unsigned int size )
{
	if( !g_vector.SetVectorData( pData, size ) )
	{
		return;
	}
	g_vector.WriteVector();
}

void CApi::SET_VECTOR( unsigned int* pData , unsigned int size )
{
	if( !g_vector.SetVector( pData, size ) )
	{
		//error
		return;
	}
	g_vector.WriteVector();
}

void CApi::PRINTF_VECTOR( unsigned int fc, unsigned int start, unsigned int size )
{
	g_vector.PrintVector(fc, start, size );
}

void CApi::SYS_READ_VECTOR( unsigned char* p, unsigned int size )
{
	g_vector.GetVectorData( p, size );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DBM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::CLR_DBM()
{
	g_dbm.ClearDbm();
}

void CApi::SET_DBM_DATA( unsigned char* s, unsigned int size, bool ddr )
{
	g_dbm.SetDbmData( s, size, ddr );
}

void CApi::SET_DBM_DATA( unsigned short* s, unsigned int size, bool ddr )
{
	g_dbm.SetDbmData( s, size, ddr );
}

void CApi::setDbm( uint index, uint data )
{
	if( !g_dbm.SetBgDbm( index, data) )
	{
		RUNTIME_ERROR( __FUNCTION__, " error : first SET_FMMODE_GLOBAL(EDBM_BG_WRITE)" );
	}
}

uint CApi::getDbm( uint index )
{
	return g_dbm.ReadDbm(index);
}

void CApi::SET_DBM( unsigned int index, unsigned int v, ... )
{
	va_list list;
	va_start( list, v );
	unsigned int value = v;
	unsigned int addr  = index;
	while( value != 0 )
	{
		WRITE_DBM( addr, value );
		value = va_arg( list, unsigned int );
		addr++;
	}
	va_end(list);
}

void CApi::PRINTF_DBM( unsigned int fc, unsigned start, unsigned int count )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::WRITE_DBM(unsigned int index, unsigned int v)
{
	if( (index < 0 ) || (index>MAX_DBM_SIZE_32M) )
	{
		EPRINTF( "%s error : DBM memory address 0x%x, use [0~0x%x]"
			, __FUNCTION__, index, MAX_DBM_SIZE_32M );
		return;
	}
	g_dbm.WriteDbm( index, v );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ECMD
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::CLR_ECMD()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_ECMD(unsigned int index, unsigned long long ecmd_data )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_ECMD_DATA(unsigned long long *s, unsigned int size)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::PRINTF_ECMD(unsigned int fc, unsigned int start, unsigned int count )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SCRAMBLE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SCRAMBLE (const char * name)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_SCRAMBLE (const char *grp)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SCAN MASK
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::CLR_SCM()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_SCM_DATA(int dut, unsigned int realscan_blockaddr )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::PRINTF_SCM(unsigned int fc, unsigned int start, unsigned int count )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PG(PATTERN)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CApi::SET_PATTERN()
{
	if( !g_pg.SetPattern() )
	{
		EPRINTF( "%s error!!", __FUNCTION__ );
		return false;
	}
	return true;
}

void CApi::SET_JNCFLAG(uint Jnc)
{
	CFLAG = Jnc;
}

bool CApi::MEAS_MPAT( const char* name, double wait )
{
//	g_pg.AlpgClear();	//2023-05-16 fatric
	g_tg.TgMemClear();	//2024-01-04 fatric
	g_pg.CmpClear();
	g_pg.DrvClear();
	g_pg.DfmClear();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 2023.10.10 - KWMOON(VIH/VTH)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const double GAP = 0.02;	//50MV
	if( m_debug_mode&1 )
	{
		double dSetVih = 0.0;
		double dSetVth = 0.0;
		double dGetVih = 0.0;
		double dGetVth = 0.0;
		dSetVih = g_hw.GetSetVih();
		dSetVth = g_hw.GetSetVth();
		MEAS_VIH( dGetVih );
		MEAS_VTH( dGetVth );
		double dGapVih = fabs( dSetVih - dGetVih );
		double dGapVth = fabs( dSetVth - dGetVth );
		if( (dGapVih > GAP) || (dGapVth > GAP) )
		{
			PRINTF( "ABLEE VIH/VOH GAP>%5.2f V   STEP( %-25s) BF MEAS_MPAT(%20s:%10s)    VIH[F:%6.3f V / M:%6.3f V / G:%6.3f V]    VTH[F:%6.3f V / M:%6.3f V / G:%6.3f V] ",
					GAP, GET_STEP_NAME(), g_pg.GetCurrentPattern().c_str(), name, dSetVih, dGetVih, dGapVih, dSetVth, dGetVth, dGapVth );
		}
	}
	/////////////////////////////////////////////////////////////////////////////////

	if( !g_pg.StartMpat( name ) )
	{
		EPRINTF( "%s Start pattern error!!", __FUNCTION__ );
		return false;
	}
	WAIT_MPAT( wait );
	STOP_MPAT();

	//2023.06.13 - kwmoon(modify fatric)
	if ( strlen(m_szPc)<1 )
		strncpy( m_szPc, name, strlen(name) );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 2023.10.10 - KWMOON(VIH/VTH)
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_debug_mode&1 )
	{
		double dSetVih = 0.0;
		double dSetVth = 0.0;
		double dGetVih = 0.0;
		double dGetVth = 0.0;
		dSetVih = g_hw.GetSetVih();
		dSetVth = g_hw.GetSetVth();
		MEAS_VIH( dGetVih );
		MEAS_VTH( dGetVth );
		double dGapVih = fabs( dSetVih - dGetVih );
		double dGapVth = fabs( dSetVth - dGetVth );
		if( (dGapVih > GAP) || (dGapVth > GAP) )
		{
			PRINTF( "ABLEE VIH/VOH GAP>%5.2f V   STEP( %-25s) AF MEAS_MPAT(%20s:%10s)    VIH[F:%6.3f V / M:%6.3f V / G:%6.3f V]    VTH[F:%6.3f V / M:%6.3f V / G:%6.3f V]",
					GAP, GET_STEP_NAME(), g_pg.GetCurrentPattern().c_str(), name, dSetVih, dGetVih, dGapVih, dSetVth, dGetVth, dGapVth );
		}
	}
	/////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool CApi::START_MPAT( const char* name, double wait/*=0.0*/ )
{
	bool bOk = g_pg.StartMpat( name );
	if( !bOk )
	{
		EPRINTF( "%s error!!", __FUNCTION__ );
		return false;
	}
	WAIT_MPAT( wait );
	return true;
}

void CApi::WAIT_MPAT(double wait/*=-1.0*/)
{
	if ( wait == 0.0 ) return;

	UTIL::TIMER timer( wait );
	timer.start();

	while( 1 )
	{
		if( g_pg.IsPgStop() )
		{
			//PRINTF( "%s PG STOP!", g_pg.GetCurrentPattern().c_str() );
			break;
		}
		if( g_pg.IsPgPause() )
		{
			OUT_MPAT( g_pg.GetBurstCode() );
		}
		if ( wait > 0.0 )
		{
			if ( timer.isEnd() )
				break;
		}
		//2023.03.07 - KWMOON
		if( !GET_RUNMODE() )
		{
			STOP_MPAT();
			break;
		}
		g_sys.SysDelay( 10 US );
	}
}

void CApi::STOP_MPAT()
{
	g_pg.StopMpat();
}

void CApi::CONTINUE_MPAT()
{
	if( !GET_RUNMODE() ) return;

	g_pg.ContinueMpat();
	//PRINTF( "CONTINUE MPAT" );
}

bool CApi::IS_PGPAUSE()
{
	return g_pg.IsPgPause();
}

uint CApi::GET_PGRUNTIME()
{
	return g_pg.GetPgRunTime();
}

void CApi::CLEAR_PGRUNTIME()
{
	g_pg.ClearPgRunTime();
}

bool CApi::PAUSE( const char* pattern )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return true;
}

bool CApi::OUT_MPAT( uint out)
{
	bool bOk = true;
//	PRINTF( "%s OUT 0x%08X!", g_pg.GetCurrentPattern().c_str(), out );
	switch ( out )
	{
		case pretest_result:
		case bitest_result:
		case latch:
			break;
		case pre_result:
			PRINTF("pre_result");
			GET_EXTFM_INIT();
			break;
		case fm_block:
		case extfm_result:
		case L1_result:
			break;
		case latch_result:
		case init_result:
			//Parse LATCH DATA
			//Set step fail, cum fail.
			out_init_result();
			break;
		case fm_read_data:	//chip id, wafer id, x, y
			out_fm_read_data();
			break;
		case bist_io_latch:
		case bist_result:
		case bist2_result:
		case emmc_io_latch:
		case emmc_read_drv:
		case emmc_init:
		case emmc_result:
		case emmc_block_result:
		case emmc_chip_id:
		case emmc_read_data:
			break;
		default:
			bOk = BURST( g_pg.GetCurrentPattern().c_str(), out );
			break;
	}
	if( bOk ) CONTINUE_MPAT();
	else
	{
		EPRINTF( "%s OUT 0x%08X error!", g_pg.GetCurrentPattern().c_str(), out );
		CONTINUE_MPAT();
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::RATE( double r0 , double r1, double r2, double r3,
				 double r4, double r5, double r6, double r7,
				 double r8, double r9, double r10, double r11,
				 double r12, double r13, double r14, double r15 )
{
//	g_tg.TgMemClear();	//2024-01-04 fatric
	if( !g_tg.RATE( r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15 ) )
	{
		//error print
	}
}

void CApi::SET_RATE( int t, double rate )
{
	g_tg.SET_RATE( t, rate );
}

void CApi::CLK ( TS_CLOCK_LIST pin, double r0 , double r1, double r2, double r3,
				 double r4, double r5, double r6, double r7,
				 double r8, double r9, double r10, double r11,
				 double r12, double r13, double r14, double r15 )
{
	g_tg.CLOCK( pin, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15 );
}

void CApi::SET_CLK( int pin, int ts, double tval )
{
	g_tg.SET_CLK( pin, ts, tval );
}

void CApi::SET_CLK_DELAY( int pin, double tdelay )
{
	g_tg.SET_CLK_DELAY( pin, tdelay );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CYP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_CYP()
{
	g_pg.WriteCyp();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MODULE(FMT)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_MODULE(bool bModule)
{
	g_pg.SetModule( bModule );
	if( !bModule )
	{
		g_pg.WriteModule();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BI / CHAMBER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CApi::SET_TEMP( const double sv, double wait_time, double soak_time, bool bprint )
{
	if( !GET_RUNMODE() ) return;
	if( !g_sys.ChamberIsUse() )
	{
		return;
	}

	//Chamber Temperature set
	m_stSetTemp.dTemp = sv;

	//TestMgr conection check
	if( !g_devsock.IsConn() )
	{
		if( !g_devsock.ConnectToHost() )
		{
			RUNTIME_ERROR( __FUNCTION__, "Host(TestMgr) connection error!" );
			return;
		}
	}
	//2023.07.04 - kwmoon( wait_time, soak_time divide 60 )
	PRINTF( "SET_TEMP( %6.2f, %03d MIN, %03d MIN )", sv, static_cast<int>(wait_time/60), static_cast<int>(soak_time/60) ); PRINTF(NULL);

	bool bPass = true;
	bool bTimeOut = false;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// INIT TEMP
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PROTOCOL::PGM_CHMB_REQ_ACK ack = g_sys.ChamberInitTemp();
	switch (ack) {
		case PROTOCOL::PGM_CHMB_ACK:
		case PROTOCOL::PGM_CHMB_TRUE:
			bPass = true;
			break;

		case PROTOCOL::PGM_CHMB_NACK:
		case PROTOCOL::PGM_CHMB_FALSE:
		case PROTOCOL::PGM_CHMB_FAULT:
			RUNTIME_ERROR( __FUNCTION__, "INIT_TEMP : temperature error!" );
			return;
			break;

		default:
			break;
	}
	if( bprint )
	{
		PRINTF( "%s(INIT_TEMP) : %s!", __FUNCTION__, bPass?"SUCCESS":"FAIL" );
	}
	PRINTF(NULL);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// ENTER ( ALL SLOT ENTER SYNC WAIT )
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::chrono::steady_clock::time_point	tpEnterTime;
	std::chrono::steady_clock::time_point	tpEndTime;

	tpEnterTime = std::chrono::steady_clock::now();
	bTimeOut = false;
	int nMinute = DEFAULT_ENTER_SYNC_TIME;
	do
	{
		if( !GET_RUNMODE() ) return;

		bPass = false;
		ack = g_sys.ChamberEnter();
		switch ( ack )
		{
			case PROTOCOL::PGM_CHMB_ACK:
			case PROTOCOL::PGM_CHMB_TRUE:
				bPass = true;
				break;

			case PROTOCOL::PGM_CHMB_NACK:
			case PROTOCOL::PGM_CHMB_FALSE:
				bPass = false;
				break;

			case PROTOCOL::PGM_CHMB_FAULT:
				bPass = false;
				RUNTIME_ERROR( __FUNCTION__, "ENTER : Slot synchronization request/response error!" );
				return;
				break;

			default:
				bPass = true;
				break;
		}
		if( bPass ) break;

		tpEndTime = std::chrono::steady_clock::now();
		long long seconds  = std::chrono::duration_cast<std::chrono::seconds>(tpEndTime - tpEnterTime).count();
		if( seconds > (nMinute MIN) )
		{
			bTimeOut = true;
			break;
		}
		if( bprint )
		{
			if( seconds == 0 )
			{
				PRINTF( "%s(ENTER) : Slot synchronization time is %03d minutes", __FUNCTION__, nMinute );
			}
			if( (seconds % 60) == 0 )	//display minute
			{
				PRINTF( "%s(ENTER) : Slot synchronization wait time (%03d min / %03d min) : %s", __FUNCTION__, seconds/60, nMinute, bPass?"SUCCESS":"WAIT" );
			}
			PRINTF(NULL);
		}
		sysDelay( 1000 MS );
	}while( !bPass );

	if( !GET_RUNMODE() ) return;

	if( bTimeOut )
	{
		RUNTIME_ERROR( __FUNCTION__, "ENTER : Slot synchronization error, %03d minutes over!", nMinute );
		return;
	}
	PRINTF( "%s(ENTER) : Slot synchronization done.", __FUNCTION__ );
	PRINTF(NULL);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// SET SV ( TEMPERATURE SET )
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	tpEnterTime = std::chrono::steady_clock::now();
	bTimeOut = false;
	nMinute = DEFAULT_SETSV_SYNC_TIME;	//2024.07.12 - KWMOON
	do
	{
		if( !GET_RUNMODE() ) return;

		bPass = false;
		ack = g_sys.ChamberSetSv( sv );
		switch (ack) {
			case PROTOCOL::PGM_CHMB_ACK:
			case PROTOCOL::PGM_CHMB_TRUE:
				bPass = true;
				break;

			case PROTOCOL::PGM_CHMB_NACK:
			case PROTOCOL::PGM_CHMB_FALSE:
				bPass = false;
				break;
			case PROTOCOL::PGM_CHMB_FAULT:
				bPass = false;
				RUNTIME_ERROR( __FUNCTION__, "SET_SV : %6.2f runtime error!", sv );
				return;
				break;

			default:
				bPass = true;
				break;
		}
		if( bPass ) break;

		tpEndTime = std::chrono::steady_clock::now();
		long long seconds  = std::chrono::duration_cast<std::chrono::seconds>(tpEndTime - tpEnterTime).count();
		if( seconds > (nMinute MIN) )
		{
			bTimeOut = true;
			break;
		}
		if( bprint )
		{
			if( seconds == 0 )
			{
				PRINTF( "%s(SET_SV) : Slot synchronization time is %03d minutes", __FUNCTION__, nMinute );
			}
			if( (seconds % 60) == 0 )	//display minute
			{
				PRINTF( "%s(SET_SV) : Slot synchronization wait time (%03d min / %03d min) : %s", __FUNCTION__, seconds/60, nMinute, bPass?"SUCCESS":"WAIT" );
			}
			PRINTF(NULL);
		}
		sysDelay( 1000 MS );
	}while( !bPass );

	if( bTimeOut )
	{
		RUNTIME_ERROR( __FUNCTION__, "SET_SV : Slot synchronization error, %03d minutes over!", nMinute );
		return;
	}
	PRINTF( "%s(SET_SV) : %6.2f %s!", __FUNCTION__, sv, bPass?"SUCESS":"ERROR" );
	PRINTF(NULL);
	if( !GET_RUNMODE() ) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// RUN
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PRINTF( "%s(CHMB_RUN) START!", __FUNCTION__ ); PRINTF(NULL);

	bPass = false;
	bTimeOut = false;
	tpEnterTime = std::chrono::steady_clock::now();
	nMinute = DEFAULT_RUN_SYNC_TIME;
	do
	{
		if( !GET_RUNMODE() ) return;

		bPass = false;
		ack = g_sys.ChamberRun();
		switch (ack) {
			case PROTOCOL::PGM_CHMB_ACK:
			case PROTOCOL::PGM_CHMB_TRUE:
				bPass = true;
				break;

			case PROTOCOL::PGM_CHMB_NACK:
			case PROTOCOL::PGM_CHMB_FALSE:
				bPass = false;
				break;
			case PROTOCOL::PGM_CHMB_FAULT:
				bPass = false;
				RUNTIME_ERROR( __FUNCTION__, "CHMB_RUN : runtime error!" );
				return;
				break;

			default:
				break;
		}
		if( bPass ) break;

		tpEndTime = std::chrono::steady_clock::now();
		long long seconds  = std::chrono::duration_cast<std::chrono::seconds>(tpEndTime - tpEnterTime).count();
		if( seconds > (nMinute MIN) )
		{
			bTimeOut = true;
			break;
		}
		if( bprint )
		{
			if( seconds < 1 )
			{
				PRINTF( "%s(CHMB_RUN) : Slot synchronization time is %03d minutes", __FUNCTION__, nMinute );
			}
			if( (seconds % 60) == 0 )	//display minute
			{
				PRINTF( "%s(CHMB_RUN) : Slot synchronization wait time (%03d min / %03d min) : %s", __FUNCTION__, seconds/60, nMinute, bPass?"SUCCESS":"WAIT" );
			}
			PRINTF(NULL);
		}
		sysDelay( 1000 MS );
	}while( !bPass );

	if( bTimeOut )
	{
		RUNTIME_ERROR( __FUNCTION__, "CHMB_RUN : Slot synchronization error, %03d minutes over!", nMinute );
		return;
	}
	PRINTF( "%s(CHMB_RUN) : Slot synchronization done.", __FUNCTION__ );
	PRINTF(NULL);
	if( !GET_RUNMODE() ) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// CHAMBER STATUS CHECK -- DOOR / RACK CHECK
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool bRun = false, bStop = false, bAmb = false;
	bool bDoorOpen = false, bDoorClose = false, bUpperRack = false, bBottomRack = false, bRackIn = false, bRackOut = false;
	ack = g_sys.ChamberReadStatus1( bDoorOpen, bDoorClose, bUpperRack, bBottomRack, bRackIn, bRackOut );
	switch (ack)
	{
		case PROTOCOL::PGM_CHMB_ACK:
		case PROTOCOL::PGM_CHMB_TRUE:
			bPass = true;
			break;

		case PROTOCOL::PGM_CHMB_NACK:
		case PROTOCOL::PGM_CHMB_FALSE:
		case PROTOCOL::PGM_CHMB_FAULT:
			bPass = false;
			RUNTIME_ERROR( __FUNCTION__, "STATUS : Read error!" );
			return;
			break;

		default:
			break;
	}
	if( bDoorOpen )
	{
		RUNTIME_ERROR( __FUNCTION__, "CHMB_STAT : DOOR is open!" );
		return;
	}
	//2023.06.02 - KWMOON( Rack check skip for test )
	//if( bRackOut )
	//{
	//    RUNTIME_ERROR( __FUNCTION__, "CHMB_STAT : Rack out status!" );
	//    return;
	//}
	if( bprint )
	{
		PRINTF( "%s(CHMB_STAT) : DOOR_OPEN[%c] DOOR_CLOSE[%c] UPPER_RACK[%c] BOTTOM_RACK[%c] RACK_IN[%c] RACK_OUT[%c]", __FUNCTION__,
				bDoorOpen?'Y':'N', bDoorClose?'Y':'N', bUpperRack?'Y':'N', bBottomRack?'Y':'N', bRackIn?'Y':'N', bRackOut?'Y':'N' );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// WAIT TIME
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::chrono::steady_clock::time_point tpWaitBeginTime;
	std::chrono::steady_clock::time_point tpWaitEndTime;
	int nTempError = 0;
	int nStatError = 0;
	bool bGapPass = false;
	long long elapsed_seconds = 0;
	double dSv = 0.0, dPv = 0.0;

	bPass = true;
	bTimeOut = false;
	tpWaitBeginTime = std::chrono::steady_clock::now();
	do
	{
		if( !GET_RUNMODE() ) return;
		sysDelay( 1500 MS );
		ack = g_sys.ChamberReadStatus( bRun, bStop, bAmb );
		if( ( ack != PROTOCOL::PGM_CHMB_ACK ) && ( ack != PROTOCOL::PGM_CHMB_TRUE ) )
		{
			nStatError++;
			if( nStatError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "WAIT_TIME : %03d min / %03d Chamber status read error!",
							   elapsed_seconds/60, static_cast<int>(wait_time/60) );
				return;
			}
		}
		if( bStop )
		{
			nStatError++;
			if( nStatError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "WAIT_TIME : %03d min / %03d min Chamber status is Stop!",
							   elapsed_seconds/60, static_cast<int>(wait_time/60) );
				return;
			}
		}
		if( bprint )
		{
			PRINTF( "%s(WAIT_TIME) : RUN[%c] STOP[%c] AMB[%c]", __FUNCTION__, bRun?'Y':'-', bStop?'Y':'-', bAmb?'Y':'-' );
			PRINTF( NULL );
		}

		ack = g_sys.ChamberReadTemp( dSv, dPv );
		if( ( ack != PROTOCOL::PGM_CHMB_ACK ) && ( ack != PROTOCOL::PGM_CHMB_TRUE ) )
		{
			nTempError++;
			if( nTempError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "WAIT_TIME : %03d min / %03d min Chamber temperature read error!",
							   elapsed_seconds/60, static_cast<int>(wait_time/60) );
				return;
			}
		}
		else
		{
			double dGap = fabs( dSv - dPv );
			if( dGap < DEFAULT_SV_GAP )
			{
				PRINTF( "%s(WAIT_TIME) : %03d min / %03d min  PV(%6.2f) reached SV(%6.2f)",
						__FUNCTION__, elapsed_seconds/60, static_cast<int>(wait_time/60), dPv, dSv );
				bGapPass = true;
				break;
			}
		}
		if( bprint )
		{
			PRINTF( "%s(WAIT_TIME) : PV[%6.2f] SV[%6.2f] SET[%6.2f]", __FUNCTION__, dPv, dSv, sv );
			PRINTF( NULL );
		}

		tpWaitEndTime = std::chrono::steady_clock::now();
		elapsed_seconds  = std::chrono::duration_cast<std::chrono::seconds>(tpWaitEndTime - tpWaitBeginTime).count();
		if( elapsed_seconds > wait_time )
		{
			bTimeOut = true;
			break;
		}
		if( bprint )
		{
			PRINTF( "%s(WAIT_TIME) : %03d min / %d min", __FUNCTION__, elapsed_seconds/60, static_cast<int>(wait_time/60) );
			PRINTF(NULL);
		}
	}while( 1 );

	if( bTimeOut && !bGapPass )
	{
		RUNTIME_ERROR( __FUNCTION__, "WAIT_TIME (%03d min / %03d min) PV(%6.2f) did not reach SV(%6.2f)",
					   static_cast<int>(elapsed_seconds/60), static_cast<int>(wait_time/60), dPv, dSv );
		return;
	}
	PRINTF( "%s(WAIT_TIME) : %d min done!", __FUNCTION__, static_cast<int>(elapsed_seconds/60) );
	PRINTF(NULL);
	if( !GET_RUNMODE() ) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// SOAK TIME
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PRINTF( "%s(SOAK_TIME) START!", __FUNCTION__ ); PRINTF(NULL);
	bool bSoakPass = true;
	int nSoakError = 0;
	bPass = true;
	bTimeOut = false;
	nStatError = 0;
	nTempError = 0;
	elapsed_seconds = 0;
	tpWaitBeginTime = std::chrono::steady_clock::now();
	do
	{
		if( !GET_RUNMODE() ) return;
		sysDelay( 1500 MS );
		ack = g_sys.ChamberReadStatus( bRun, bStop, bAmb );
		if( ( ack != PROTOCOL::PGM_CHMB_ACK ) && ( ack != PROTOCOL::PGM_CHMB_TRUE ) )
		{
			nStatError++;
			if( nStatError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "SOACK_TIME : %03d min / %03d min Chamber status read error!",
							   static_cast<int>(elapsed_seconds/60), static_cast<int>(soak_time/60) );
				return;
			}
		}
		if( bStop )
		{
			nStatError++;
			if( nStatError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "SOACK_TIME : %03d min / %03d min Chamber status is Stop!",
							   static_cast<int>(elapsed_seconds/60), static_cast<int>(soak_time/60) );
				return;
			}
		}
		if( bprint )
		{
			PRINTF( "%s(SOACK_TIME) : %03d min / %03d RUN[%c] STOP[%c] AMB[%c]",
					__FUNCTION__, static_cast<int>(elapsed_seconds/60), static_cast<int>(soak_time/60), bRun?'Y':'-', bStop?'Y':'-', bAmb?'Y':'-' );
			PRINTF( NULL );
		}

		ack = g_sys.ChamberReadTemp( dSv, dPv );
		if( ( ack != PROTOCOL::PGM_CHMB_ACK ) && ( ack != PROTOCOL::PGM_CHMB_TRUE ) )
		{
			nTempError++;
			if( nTempError > MAX_RETRY_COMM_CNT )
			{
				RUNTIME_ERROR( __FUNCTION__, "SOACK_TIME : %03d min / %03d Chamber temperature read error!",
							   static_cast<int>(elapsed_seconds/60), static_cast<int>(soak_time/60) );
				return;
			}
		}
		else
		{
			double dGap = fabs( fabs(dSv) - fabs(dPv) );
			if( dGap > DEFAULT_SOAK_TEMP_GAP )
			{
				EPRINTF( "%s(SOACK_TIME) : %03d min / %03d min PV(%6.2f) and SV(%6.2f) gap is bigger than %6.2f",
						__FUNCTION__, elapsed_seconds/60, static_cast<int>(soak_time/60), dPv, dSv, DEFAULT_SOAK_TEMP_GAP );
				PRINTF(NULL);
				nSoakError++;

				if( nSoakError > DEFAULT_SOAK_RETRY_COUNT )
				{
					RUNTIME_ERROR( __FUNCTION__, "SOACK_TIME : %03d min / %03d min PV(%6.2f) and SV(%6.2f) gap count over than %d",
								   elapsed_seconds/60, static_cast<int>(soak_time/60), dPv, dSv, DEFAULT_SOAK_RETRY_COUNT );
					return;
				}
			}
			else
			{
				nSoakError = 0;
				if( bprint )
				{
					PRINTF( "%s(SOACK_TIME) : PV[%6.2f] SV[%6.2f] SET[%6.2f]", __FUNCTION__, dPv, dSv, sv );
					PRINTF( NULL );
				}
			}
		}

		tpWaitEndTime = std::chrono::steady_clock::now();
		elapsed_seconds  = std::chrono::duration_cast<std::chrono::seconds>(tpWaitEndTime - tpWaitBeginTime).count();
		if( elapsed_seconds > static_cast<int>(soak_time) )
		{
			bTimeOut = true;
			break;
		}
		if( bprint )
		{
			PRINTF( "%s(SOACK_TIME) : %03d min / %03d min PV(%6.2f) and SV(%6.2f)",
					__FUNCTION__, elapsed_seconds/60, static_cast<int>(soak_time/60), dPv, dSv );
			PRINTF(NULL);
		}
	}while( 1 );

	if( bTimeOut && !bSoakPass )
	{
		RUNTIME_ERROR( __FUNCTION__, "SOACK_TIME(elapsed %03d min / soak %03d min) PV(%6.2f) did not reach SV(%6.2f)",
					   elapsed_seconds/60, static_cast<int>(soak_time/60) );
		return;
	}
	PRINTF( "%s(SOAK_TIME) : %d min done!", __FUNCTION__, static_cast<int>(soak_time/60) );
	PRINTF(NULL);
	if( !GET_RUNMODE() ) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// INIT TEMP
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ack = g_sys.ChamberInitTemp();
	//switch (ack) {
	//	case PROTOCOL::PGM_CHMB_ACK:
	//	case PROTOCOL::PGM_CHMB_TRUE:
	//		bPass = true;
	//		break;
	//
	//	case PROTOCOL::PGM_CHMB_NACK:
	//	case PROTOCOL::PGM_CHMB_FALSE:
	//	case PROTOCOL::PGM_CHMB_FAULT:
	//		RUNTIME_ERROR( __FUNCTION__, "INIT_TEMP : temperature error!" );
	//		return;
	//		break;
	//
	//	default:
	//		break;
	//}
	//if( bprint )
	//{
	//	PRINTF( "%s(INIT_TEMP) : %s!", __FUNCTION__, bPass?"SUCCESS":"FAIL" );
	//}
	//PRINTF(NULL);
}

//2023.06.29 - KWMOON
bool CApi::GET_CHMB_TEMP( double& sv, double& pv )
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = g_sys.ChamberReadTemp( sv, pv );
	//2023.07.06 - KWMOON
	if( ack == PROTOCOL::PGM_CHMB_NOUSE )
	{
		EPRINTF( "CHMABER NO USE" ); PRINTF(NULL);
		return false;
	}
	if( ack == PROTOCOL::PGM_CHMB_ACK || ack == PROTOCOL::PGM_CHMB_TRUE )
		return true;
	return false;
}

bool CApi::GET_CHMB_STAT( bool& bRun, bool& bStop, bool& bAmb )
{
	PROTOCOL::PGM_CHMB_REQ_ACK ack = g_sys.ChamberReadStatus( bRun, bStop, bAmb );
	//2023.07.06 - KWMOON
	if( ack == PROTOCOL::PGM_CHMB_NOUSE )
	{
		EPRINTF( "CHMABER NO USE" ); PRINTF(NULL);
		return false;
	}
	if( ack == PROTOCOL::PGM_CHMB_ACK || ack == PROTOCOL::PGM_CHMB_TRUE )
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// POWER(PPS/DAC)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_VOUT_RANGE( uint pps, double high, double low )
{
	STOP_RETURN;
	(void)pps;
	(void)high;
	(void)low;
	//PRINTF( "%s is a display function.", __FUNCTION__ );
}

//factor 0:1.0 / 1:0.5 / 2:0.25 / 3:0.125
//2022.12.09 - kwmoon
void CApi::SET_VOUT_RANGE( uint unit, uint ch, uint factor )
{
	STOP_RETURN;
	ushort scaleloop = L11_V1_0;
		 if( factor == 0 ) scaleloop = L11_V1_0;
	else if( factor == 1 ) scaleloop = L11_V0_5;
	else if( factor == 2 ) scaleloop = L11_V0_25;
	else if( factor == 3 ) scaleloop = L11_V0_125;
	else                   scaleloop = L11_V1_0;

	g_pps.SetVoutScaleLoop( unit, ch, scaleloop );
}

void CApi::SET_VS( uint unit, uint ch, double set, double ov, double uv, double oc )
{
	STOP_RETURN;

	switch( unit )
	{
		case PPS_UNIT::PPS:
		case PPS_UNIT::S1PS:
				if( g_pps.SetVs( unit, ch, set, ov, uv, oc ) )
					g_sys.SetPfmSetVs( unit, ch, set );				//2023.07.25 - KWMOON
			break;
		default:
			break;
	}
}

void CApi::INIT_PINS_LEVEL()
{
	PRINTF("%s",__FUNCTION__);
	//2023.10.19 - KWMOON( BEFORE VIH OFF, FIXL SET )
	g_pg.SetCeFixH();		//2023.10.12 - KWMOON
	g_pg.SetReFixH();		//2023.10.12 - KWMOON
	g_pg.SetWpFixH();		//2023.10.12 - KWMOON
	g_pg.SetWeFixH();		//2023.10.12 - KWMOON
	g_pg.SetCleAleFixL();	//2023.10.12 - KWMOON
}

void CApi::SET_VS_ON( uint unit, int ch_bits )
{
	STOP_RETURN;
	switch( unit )
	{
		//2023.04.21 - fatric( all modify )
		case PPS_UNIT::PPS:
			/*
			if ( ch_bits==0 )
			{
				if ( m_iPinOutDrStat==0 || m_iPinOutUclkStat==0 )
				{
					g_pps.SetVsOff( unit );
					if ( m_iPinOutDrStat  ==0 ) g_pg.SetPinOutDrvAct ();
					if ( m_iPinOutUclkStat==0 ) g_pg.SetPinOutUclkAct();
				}
				else
				{
					INIT_PINS_LEVEL();
					g_pps.SetVsOff( unit );
				}
				m_iPpsStat = 0;
			}
			else
			{
				if ( m_iPinOutDrStat==1 || m_iPinOutUclkStat==1 )
				{
					g_pps.SetVsOn( unit, ch_bits );
				}
				else
				{
					INIT_PINS_LEVEL();
					g_pps.SetVsOn( unit, ch_bits );
				}
				m_iPpsStat = 1;
			}
			*/
			if ( ch_bits==0 )
			{
				g_pps.SetVsOff( unit );
				m_iPpsStat = 0;
			}
			else
			{
				g_pps.SetVsOn( unit, ch_bits );
				m_iPpsStat = 1;
			}
			break;
		case PPS_UNIT::S1PS:
			if ( ch_bits==0 )
			{
				g_pps.SetVsOff( unit );
			}
			else
			{
				g_pps.SetVsOn( unit, ch_bits );
			}
			break;
		case PPS_UNIT::VCCIO:
		{
			if ( ch_bits == 0 )	//2023.10.20 - KWMOON
			{
				SET_VS_OFF( PPS_UNIT::VCCIO );
			}
			else
			{
				const double DCI_OFSET = -0.300;	//-0.3;	//-0.15;	//-0.15;
				double dVih = g_hw.GetSetVih();

				if( 0 )
				{
					sysWrite( 0x10302d8, 0x1 );
					sysWrite( 0x10342d8, 0x1 );
					sysWrite( 0x10382d8, 0x1 );
					sysDelay( 1 MS );
				}

				//2023.10.23 - KWMOON( WHEN VIH LEVEL VALUE CHANGED DCI CAL )
				if( g_hw.IsVihChanged() )
				{
					if( DCI_USE )
					{
						if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH1, dVih              ) )
						{
							EPRINTF( "********* %s : VIH error! *********", __FUNCTION__);
							return;
						}
						PRINTF( "*****[DCI_CAL] VIH [%6.3f V] DCI_CAL[%6.3f V]", dVih, dVih              );

						g_hw.SetDacUpdate( DAC_VIH1 );
						g_sys.SysDelay( 10 MS );			//2023.10.18 - KWMOON( 1 MS -> 10 MS )
						PRINTF( "*****VIH ON : %6.3f V", dVih );

						bool bCalResult = false;
						uint iResultIOA , iDciLockIOA ;
						uint iResultIOB , iDciLockIOB ;
						uint iResultUCLK, iDciLockUCLK;
						for( int i=0; i<5; ++i )
						{
							if ( DCI_IO )
							{
								g_pg.DrvCalibration( LOGIC_IOA  );
								g_pg.DrvCalibration( LOGIC_IOB  );
							}

							if ( DCI_UCLK )
							{
								g_pg.DrvCalibration( LOGIC_UCLK );
							}
							g_sys.SysDelay(  2 MS );		//2023.12.11 fatric 2MS <- 10MS

							g_pcie.write_user( BITCHART::DRV_IOA::_DRV_IOA_DCI_DATA_WRT , 0x3 );
							iResultIOA = g_pcie.read_user( BITCHART::DRV_IOA::_DRV_IOA_DCI_RESULT);
							g_pcie.write_user( BITCHART::DRV_IOA::_DRV_IOA_DCI_DATA_WRT , 0x0 );

							g_pcie.write_user( BITCHART::DRV_IOB::_DRV_IOB_DCI_DATA_WRT , 0x3 );
							iResultIOB = g_pcie.read_user( BITCHART::DRV_IOB::_DRV_IOB_DCI_RESULT);
							g_pcie.write_user( BITCHART::DRV_IOB::_DRV_IOB_DCI_DATA_WRT , 0x0 );

							g_pcie.write_user( BITCHART::DRV_UCLK::_DRV_UCLK_DCI_DATA_WRT , 0x3 );
							iResultUCLK = g_pcie.read_user( BITCHART::DRV_UCLK::_DRV_UCLK_DCI_RESULT);
							g_pcie.write_user( BITCHART::DRV_UCLK::_DRV_UCLK_DCI_DATA_WRT , 0x0 );

							if ( iResultIOA == 3 && iResultIOB == 3 && iResultUCLK == 3 )
							{
								bCalResult = true;
								break;
							}
							else
							{
								iDciLockIOA  = g_pcie.read_user( BITCHART::DRV_IOA::_DRV_IOA_DCI_LOCK   );
								iDciLockIOB  = g_pcie.read_user( BITCHART::DRV_IOB::_DRV_IOB_DCI_LOCK   );
								iDciLockUCLK = g_pcie.read_user( BITCHART::DRV_UCLK::_DRV_UCLK_DCI_LOCK );
								g_log.EPrintf( _PG, __FUNCTION__, "[%d]DCI_CAL fail. DCI_RESULT: IOA=%X IOB=%X UCLK=%X   DCI_LOCK: IOA=%X IOB=%X UCLK=%X",
											   i, iResultIOA, iResultIOB, iResultUCLK, iDciLockIOA, iDciLockIOB, iDciLockUCLK );
							}
						}

						if ( ! bCalResult )
							g_log.EPrintf( _PG, __FUNCTION__, "DCI_CAL 5 times failed.");

/////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
					else
					{
						if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH1, dVih  ) )
						{
							EPRINTF( "********* %s : VIH error! *********", __FUNCTION__);
							return;
						}
						g_hw.SetDacUpdate( DAC_VIH1 );
						g_sys.SysDelay( 10 MS );			//2023.10.18 - KWMOON( 1 MS -> 10 MS )
						PRINTF( "*****VIH ON : %6.3f V", dVih );
					}

				}
				else
				{
					g_hw.SetDacUpdate( DAC_VIH1 );
					g_sys.SysDelay( 10 MS );			//2023.10.18 - KWMOON( 1 MS -> 10 MS )
					PRINTF( "*****[DBG] VIH NO CHANGED[%6.2f V]", dVih );
				}
				INIT_PINS_LEVEL();
			}
		}
		break;

		default:
			break;
	}
	if( g_pps.IsPpsOn() && g_pps.IsS1psOn() )
	{
		if( g_sys.IsPfmEnable() )
		{
			if( !g_sys.IsPfmStart() )
			{
				g_sys.SetPfmStart();
			}
		}
	}
}

//2022.11.30 - kwmmon(vih off remove)
void CApi::SET_VS_OFF( uint unit )
{
	switch( unit )
	{
		//2023.04.21 - fatric(all modify)
		case PPS_UNIT::PPS:
			if ( m_iPinOutDrStat==0 || m_iPinOutUclkStat==0 )
			{
				g_pps.SetVsOff( unit );
				if ( m_iPinOutDrStat  ==0 ) g_pg.SetPinOutDrvAct ();
				if ( m_iPinOutUclkStat==0 ) g_pg.SetPinOutUclkAct();
			}
			else
			{
				g_pps.SetVsOff( unit );
			}
			m_iPpsStat = 0;
			break;
		case PPS_UNIT::S1PS:
			g_pps.SetVsOff( unit );
			break;
		case PPS_UNIT::VCCIO:
			g_hw.SetDacVout( DAC_VIH1, 0.0 );
			g_hw.SetDacUpdate( DAC_VIH1 );
			g_sys.SysDelay( 1 MS );
			g_hw.SetDacVout( DAC_VIH1, g_hw.GetSetVih() );
			break;
		default:
			break;
	}
}

double CApi::READ_VS( uint unit, uint ch )
{
	double dVoltage = 0;
	g_pps.ReadVs( unit, ch, dVoltage );
	return dVoltage;
}

void CApi::SET_VIH( double vih )
{
	//fprintf( stdout, "SET VIH : %5.2f V\n", vih );
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH1, vih ) )
	{
		EPRINTF( "SET_VIH error!");
		return;
	}
	g_sys.SysDelay( 100 US );
}

void CApi::SET_VTH( double vth )
{
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VOH1, vth ) )
	{
		EPRINTF( "SET_VTH error!");
		return;
	}
	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_VOH1 );
	g_sys.SysDelay( 100 US );
}

void CApi::SET_MIOVOLT( double vih, double voh )
{
	SET_VIH2( vih, voh );
}

void CApi::SET_VIH2( double vih, double voh )
{
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH2, vih ) )
	{
		EPRINTF( "SET_MIOVOLT vih error!");
		return;
	}

	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VOH2, voh ) )
	{
		EPRINTF( "SET_MIOVOLT voh error!");
		return;
	}

	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_VIH2 );
	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_VOH2 );
	g_sys.SysDelay( 100 US );
}

//no use - SET_VS Use
//void CApi::SET_VS_BUMP( uint unit, uint ch, double set, double ov, double uv, double oc )
//{
//	switch( unit )
//	{
//		case PPS_UNIT::PPS:
//		case PPS_UNIT::S1PS:
//			g_pps.SetVBump( unit, ch, set, ov, uv, oc );
//			break;
//		default:
//			break;
//	}
//}

//for Debug
void CApi::SET_VIH_ON( double vih )
{
	STOP_RETURN;
	//fprintf( stdout, "SET VIH : %5.2f V\n", vih );
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH1, vih ) )
	{
		EPRINTF( "SET_VIH error!");
		return;
	}
	g_hw.SetDacUpdate( DAC_VIH1 );
	g_sys.SysDelay( 1 MS );
}

//double CApi::GET_S1PS_ATT_VOLTAGE(double force)
//{
//	double dAtt = 0.0;
//	g_pps.GetS1psAttVoltage( force, dAtt);
//	return dAtt;
//}

void CApi::CLEAR_FAULTS(uint unit, int ch)
{
	bool bOk = g_pps.ClearFaults( unit, ch );
	if( !bOk )
	{
		EPRINTF( "%-4s CH%d Clear Fault Error!!", (unit==PPS)?"PPS":"S1PS", ch+1 );
	}
}

int CApi::READ_ALERT(uint unit)
{
	uint nAlert = g_pps.GetPpsAlert(unit);
	return nAlert;
}

bool CApi::READ_STATUS_BYTE(uint unit, int ch, R_STATUS_BYTE& rByte )
{
	bool bRead = g_pps.ReadStatusByte( unit, ch, rByte );
	return bRead;
}

bool CApi::READ_STATUS_WORD(uint unit, int ch, R_STATUS_WORD& rWord )
{
	bool bRead = g_pps.ReadStatusWord( unit, ch, rWord );
	return bRead;
}

bool CApi::READ_STATUS_VOUT(uint unit, int ch, R_STATUS_VOUT& rVout)
{
	bool bRead = g_pps.ReadStatusVout( unit, ch, rVout );
	return bRead;
}

bool CApi::READ_STATUS_IOUT(uint unit, int ch, R_STATUS_IOUT& rIout)
{
	bool bRead = g_pps.ReadStatusIout( unit, ch, rIout );
	return bRead;
}

void CApi::SET_DGS_HIGH( uint unit, bool bHigh )
{
	bool bOk = g_pps.SetDgsControl( unit, bHigh );
	if( !bOk )
	{

	}
}

void CApi::SET_SENSE_HIGH( uint unit, bool bHigh )
{
	bool bOk = g_pps.SetSenseControl( unit, bHigh );
	if( !bOk )
	{

	}
}

int CApi::READ_SCALELOOP( uint unit, uint ch )
{
	int v = 0x0;
	g_pps.ReadScaleloop( unit, ch, v );
	return v;
}

double CApi::READ_PPS_VF( uint unit, uint ch )
{
	double dVoltage = 0;
	g_pps.ReadVf( unit, ch, dVoltage );
	return dVoltage;
}

double CApi::READ_PPS_GND( uint unit )
{
	double dGnd = 0;
	g_pps.ReadGnd( unit, dGnd );
	return dGnd;
}

double CApi::READ_PPS_VOUT( uint unit, uint ch )
{
	double dVoltage = 0;
	g_pps.ReadVout( unit, ch, dVoltage );
	return dVoltage;
}

double CApi::READ_PPS_IOUT( uint unit, uint ch )
{
	double dCurrent = 0;
	g_pps.ReadIout( unit, ch, dCurrent );
	return dCurrent;
}

void CApi::PRINTF_VOLTAGE()
{
	STOP_RETURN;
	double dVm  [MAX_PPS_CH] = {0.0, };
	double dVout[MAX_PPS_CH] = {0.0, };
	double dIout[MAX_PPS_CH] = {0.0, };
	for( int ch=0;ch<MAX_PPS_CH;ch++)
	{
		dVm  [ch] = READ_VS( PPS, ch );
		dVout[ch] = READ_PPS_VOUT( PPS, ch );
		dIout[ch] = READ_PPS_IOUT( PPS, ch );
	}

	PRINTF( "[PPS MEASURE]" );
	PRINTF(  "-----------------------------------------------------------------------------" );
	PRINTF(  "%-10s %7s%02d %7s%02d %7s%02d %7s%02d %7s%02d %7s%02d %7s%02d %7s%02d",
			 ""    , "CH", 1, "CH", 2, "CH", 3, "CH", 4, "CH", 5, "CH", 6, "CH", 7, "CH", 8 );
	PRINTF(  "%-10s %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V",
			 "VM"  , dVm[0], dVm[1], dVm[2], dVm[3], dVm[4], dVm[5], dVm[6], dVm[7] );
	PRINTF(  "%-10s %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V %7.2f V",
			 "VOUT", dVout[0], dVout[1], dVout[2], dVout[3], dVout[4], dVout[5], dVout[6], dVout[7] );
	PRINTF(  "%-10s %7.2f A %7.2f A %7.2f A %7.2f A %7.2f A %7.2f A %7.2f A %7.2f A",
			 "IOUT", dIout[0], dIout[1], dIout[2], dIout[3], dIout[4], dIout[5], dIout[6], dIout[7] );
	PRINTF(  "-----------------------------------------------------------------------------" );
	PRINTF( NULL );
}

void CApi::SET_MIO_VIH(double vih)	//
{
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VIH2, vih ) )
	{
		EPRINTF( "SET_MIO_VIH error!");
		return;
	}
	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_VIH2 );
	g_sys.SysDelay( 100 US );
}

void CApi::SET_MIO_VTH(double voh)	//DEBUG
{
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_VOH2, voh ) )
	{
		EPRINTF( "SET_MIO_VOH error!");
		return;
	}
	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_VOH2 );
	g_sys.SysDelay( 100 US );
}

void CApi::SET_COMP_IN(double compin)	//DEBUG
{
	if( !g_hw.SetDacVout( DAC_ADDRESS::DAC_CMP_IN, compin ) )
	{
		EPRINTF( "SET_COMP_IN error!");
		return;
	}
	g_hw.SetDacUpdate( DAC_ADDRESS::DAC_CMP_IN );
	g_sys.SysDelay( 100 US );
}


void CApi::SET_DAC_VOUT(int ch, double vout)	//For Debug
{
	if( !g_hw.SetDacVout( ch, vout ) )
	{
		EPRINTF( "SET_DAC_VOUT error!" );
	}
	g_hw.SetDacUpdate( ch );
	g_sys.SysDelay( 100 US );
}

bool CApi::MEAS_VIH(double& rMeas)
{
	return g_hw.MeasVih1( rMeas );
}

bool CApi::MEAS_VTH(double& rMeas)
{
	return g_hw.MeasVth1( rMeas );
}

bool CApi::MEAS_VIH2(double& rMeasVih, double& rMeasVoh)
{
	return g_hw.MeasMioVolt( rMeasVih, rMeasVoh );
}

bool CApi::MEAS_DRV_OUT(double& rCompin)
{
	return g_hw.MeasVoutDiag( rCompin );
}

void CApi::SET_PFM_ENABLE(bool use)
{
	g_sys.SetPfmEnable( use );
}

bool CApi::IS_PFM_ENABLE()
{
	return g_sys.IsPfmEnable();
}

void CApi::SET_PFM_STOP()
{
	if( g_sys.IsPfmStart() )
		g_sys.SetPfmStop();
}

void CApi::SET_PFM_START()
{
	g_sys.SetPfmStart();
}

bool CApi::IS_PFM_START()
{
	return g_sys.IsPfmStart();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TEMPERATURE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_BITIME( int bitime )
{
	//g_hw.SetBiTime(bitime);
	m_nBiTime = bitime;
	m_stPgmVariables.nBiTime = bitime;		//2023.03.16 - kwmoon
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_AGING( int atime )
{
	//g_hw.SetBiTime(bitime);
	m_nAgingTime = atime;
	m_stPgmVariables.nAgingTime = atime;		//2023.03.16 - kwmoon
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TEMPERATURE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//option MULTI_AND , MULTI_OR
void CApi::SET_STEPCNT(int count, int option)
{
	m_set_step_count = count;
	m_set_merge_type = option;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FROM MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CApi::GET_PART_NAME()
{
	std::string part = "";
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return part;
}

std::string CApi::GET_LOT_NAME()
{
	std::string lot = "";
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return lot;
}

int CApi::GET_BLOCK_DATA(const char *step_name)
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	size_t nReadSize = m_nMaxDutPerCpu * m_nBlockPerDutBl;
//	const char* pszFile = "StepFailBlockData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailBlockData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead;
	char szStepNm[PATH_MAX] = { 0, };
	bool bFind = false;
	while(true)
	{
		nRead = read( fd, szStepNm, PATH_MAX );

		if ( nRead<1 ) break;
		if ( nRead != PATH_MAX )
		{
			EPRINTF("%s Data file(%s) read error", __FUNCTION__, szFile );
			break;
		}

		if ( ! strcmp(step_name, szStepNm) )
		{
			bFind = true;
			break;
		}

		if ( lseek( fd, nReadSize, SEEK_CUR) < 0 )
			break;
	}

	if ( bFind )
	{
		nRead = read( fd, m_get_step_fail_block, nReadSize );
	}
	else
	{
		EPRINTF("%s %s block data not found", __FUNCTION__, step_name );
	}

	close( fd );
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TO MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_FM_DATA(unsigned int board, unsigned int dut, unsigned int chip_index,	unsigned int byte_index, unsigned char value)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_FM_DATA(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index,unsigned int byte_index, unsigned char value)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned char CApi::FM_DATA(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int byte_index)
{
	unsigned char data=0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return data;
}

unsigned char CApi::FM_DATA(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int byte_index)
{
	unsigned char data = 0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return data;
}

int CApi::FM_READ_RESULT()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FAIL CONFIG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_FAILMODE( int mode )
{
	//if( mode == BLOCK )  return;
	m_fail_mode = mode;
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

int CApi::SET_PF_CE (unsigned int mode)
{
	if ( IS_SITE_FAULT() )
	{
		PRINTF("%s : IS_SITE_FAULT skip!", __FUNCTION__ );
		return -1;
	}

	if ( mode == BLOCK )
	{
		PRINTF("----- SET_PF_CE [BLOCK] " );
		//1 block fail ->dut fail
//		for ( uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut )
		//2023.05.12 - fatric(modify)
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	//2023-05-08 fatric

			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe*m_nBlockPerCe)/8;
				uint nEnd   = nStart + m_nBlockPerCe/8;
				if ( m_nBlockPerCe&0x7 ) nEnd += 1;

				for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
				{
					if ( m_cum_fail_block[iBlock] )
					{
						SET_STEP_FAIL(BIB0, iDut, true);
						SET_CUM_FAIL (BIB0, iDut, true);
						SET_TEST_FAIL(BIB0, iDut, true);
						break;
					}
				}
//				if ( IS_STEP_FAIL(BIB0, iDut) ) break;	2023-05-08 fatric

//				uint nStart =  iCe*m_nBlockPerCe;
//				uint nEnd   =  nStart + m_nBlockPerCe -1;
//				int nCumBlkFailCnt = CUM_BLOCK_FAIL_COUNT(BIB0, iDut, iCe, nStart, nEnd);
//				if ( nCumBlkFailCnt > 0 )
//				{
//					SET_STEP_FAIL(BIB0, iDut, true);
//					SET_CUM_FAIL (BIB0, iDut, true);
//					SET_TEST_FAIL(BIB0, iDut, true);
//					break;
//				}
			}
		}
	}
	else if ( mode == NORMAL )
	{
		//PRINTF("----- SET_PF_CE [NORMAL] " );
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			//PRINTF("----- SET_PF_CE [NORMAL] DUT%03d", iDut );
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	//2023-05-08 fatric

			//PRINTF("----- SET_PF_CE [NORMAL] CE PER DUT : %d", m_nCePerDut );
			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe * m_nBlockPerCe)/8;
				uint nEnd   = nStart + (m_nBlockPerCe/8);
				if ( m_nBlockPerCe&0x7 ) nEnd += 1;

// kyb
				if ( m_step_fail_block_cnt[(iDut%m_nMaxDutPerCpu) * m_nCePerDut + iCe] > m_lmtBlkPerCe )
				{
					//PRINTF("----- SET_PF_CE [FAIL] DUT%03d CE%02d m_step_fail_block_cnt = %d > %d", iDut, iCe, m_step_fail_block_cnt[(iDut%m_nMaxDutPerCpu) * m_nCePerDut + iCe], m_lmtBlkPerCe  );
					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
					{
						SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
						SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
					}

					SET_STEP_FAIL(BIB0, iDut, true);
					SET_CUM_FAIL (BIB0, iDut, true);
					SET_TEST_FAIL(BIB0, iDut, true);
				}
				//else
				//{
				//	//PRINTF("----- SET_PF_CE [PASS] DUT%03d CE%02d m_step_fail_block_cnt = %d", iDut, iCe, m_step_fail_block_cnt[(iDut%m_nMaxDutPerCpu) * m_nCePerDut + iCe]  );
				//}
// kyb
/*
				uint iFailCnt = 0;
				for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
				{
//					iFailCnt += m_nBitCnt[ (int)m_cum_fail_block[iBlock]&0xff ]; //2023-03-29 fatric
					iFailCnt += m_nBitCnt[ (int)m_now_step_fail_block[iBlock]&0xff ];
					if ( 0 && iDut==0 )
					{
						PRINTF("iBlock:%d  (int)m_now_step_fail_block[iBlock]&0xff=%X   m_nBitCnt[ (int)m_now_step_fail_block[iBlock]&0xff ]=%d", iBlock, (int)m_now_step_fail_block[iBlock]&0xff, m_nBitCnt[ (int)m_now_step_fail_block[iBlock]&0xff ] );
					}

					if ( iFailCnt > m_lmtBlkPerCe )
					{
//						PRINTF("%s:: SET_FAIL dut:%02d ce:%02d iFailCnt:%d   m_lmtBlkPerCe:%d m_nBlockPerCe:%d nStart:%d nEnd:%d", __FUNCTION__, iDut+1, iCe+1, iFailCnt, m_lmtBlkPerCe, m_nBlockPerCe, nStart, nEnd);
						if ( 0 && iDut==0 ) PRINTF("%s:: dut:%02d iFailCnt:%d   m_lmtBlkPerCe:%d m_nBlockPerCe:%d nStart:%d nEnd:%d", __FUNCTION__, iDut+1, iFailCnt, m_lmtBlkPerCe, m_nBlockPerCe, nStart, nEnd);

						//2025-05-03 fatric
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
						}

						SET_STEP_FAIL(BIB0, iDut, true);
						SET_CUM_FAIL (BIB0, iDut, true);
						SET_TEST_FAIL(BIB0, iDut, true);
						break;
					}
				}
				if ( 0 && iDut==0 ) PRINTF("%s:: dut:%02d iFailCnt:%d   m_lmtBlkPerCe:%d m_nBlockPerCe:%d nStart:%d nEnd:%d", __FUNCTION__, iDut+1, iFailCnt, m_lmtBlkPerCe, m_nBlockPerCe, nStart, nEnd);
*/

//				if ( IS_STEP_FAIL(BIB0, iDut) ) break;	//2023-05-08 ftric
			}

//			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
//			{
//				uint nStart =  iCe*m_nBlockPerCe;
//				uint nEnd   =  nStart + m_nBlockPerCe -1;
//				int nCumBlkFailCnt = CUM_BLOCK_FAIL_COUNT(BIB0, iDut, iCe, nStart, nEnd);
//				if ( nCumBlkFailCnt >= (int)m_lmtBlkPerCe )
//				{
////					printf("DEBUG::SET_PF_CE:: iFailCnt=%d  m_lmtBlkPerCe=%d  D:%d", iFailCnt, m_lmtBlkPerCe, iDut+1);
////					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
////					{
////						SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
////						SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
////					}
//					SET_STEP_FAIL(BIB0, iDut, true);
//					SET_CUM_FAIL (BIB0, iDut, true);
//					SET_TEST_FAIL(BIB0, iDut, true);
//
//					if ( (m_debug_mode&0x8)==8 )
//					{
//						printf("SET_PF_CE:: nCumBlkFailCnt=%d  m_lmtBlkPerCe=%d  DUT:%3d CE%d\n",
//							   nCumBlkFailCnt, m_lmtBlkPerCe, iDut+1, iCe+1);
//					}
//					break;
//				}
//			}
		}
	}
	else if ( mode == ADD_FAIL )
	{
		//PRINTF("----- SET_PF_CE [ADD_FAIL] " );
		//Addition Fail
//		for ( uint iDut=0; iDut<m_bib_x*m_bib_y; ++iDut )
//		for ( uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut )
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	//2023-05-08 fatric

			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe*m_nBlockPerCe/8);
				uint nEnd   = nStart + (m_nBlockPerCe/8);
				if ( m_nBlockPerCe&0x7 ) nEnd += 1;

				uint iFailCnt = 0;
				for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
				{
					char d = m_cum_fail_block[iBlock] & (~m_init_fail_block[iBlock]);
					iFailCnt += m_nBitCnt[ (int)d&0xff ];

					if ( iFailCnt >= m_lmtBlkPerCe )
					{
						for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
						{
							SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
						}
						SET_STEP_FAIL(BIB0, iDut, true);
						SET_CUM_FAIL (BIB0, iDut, true);
						SET_TEST_FAIL(BIB0, iDut, true);
						break;
					}
				}
//				if ( IS_STEP_FAIL(BIB0, iDut) ) break;	//2023-05-08 fatric
			}
		}
	}
	else if ( mode == ECC_FAIL )
	{
		//
	}
	else if ( mode == MUST_FAIL )
	{
		//complement PASS/FAIL
	}
	else if ( mode == AGING )
	{
		//Burn-In Item
	}
	else if ( mode == GRADE_TEST )
	{
		//Display Fail Block "GRF1"
	}

	memset( m_step_fail_block_cnt, 0, sizeof(uint) * m_nMaxDutPerCpu * m_nCePerDut );	//2023.07.12 - kwmoon
	PRINTF(NULL);
	return 0;
}

int CApi::SET_PF_REAL_CHIP (unsigned int mode)
{
	if ( IS_SITE_FAULT() ) return -1;

	if ( mode == BLOCK )
	{
		//1 block fail ->dut fail
//		for ( uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut )
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	2023-05-08 fatric

			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				for ( uint iChip=0; iChip<m_nChipPerCe; ++iChip )
				{
					uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe * m_nBlockPerCe + iChip * m_nBlockPerChip)/8;
					uint nEnd   = nStart + m_nBlockPerChip/8;
					if ( m_nBlockPerChip&0x7 ) nEnd += 1;

					for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
					{
						if ( m_cum_fail_block[iBlock] )
						{
							SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);

							SET_STEP_FAIL(BIB0, iDut, true);
							SET_CUM_FAIL (BIB0, iDut, true);
							SET_TEST_FAIL(BIB0, iDut, true);
							break;
						}
					}
				}
			}
		}
	}
	else if ( mode == NORMAL )
	{
		uint nLmtBlkPerChip = m_lmtBlkPerCe / m_nChipPerCe;
		//set STEP FAIL
//		for ( uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut )
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	//2023-05-08 fatirc

			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				for ( uint iChip=0; iChip<m_nChipPerCe; ++iChip )
				{
					uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe * m_nBlockPerCe + iChip * m_nBlockPerChip)/8;
					uint nEnd   = nStart + (m_nBlockPerChip/8);
					if ( m_nBlockPerChip&0x7 ) nEnd += 1;

					uint iFailCnt = 0;
					for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
					{
						iFailCnt += m_nBitCnt[ (int)m_cum_fail_block[iBlock]&0xff ];

						if ( iFailCnt >= nLmtBlkPerChip )
						{
							SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);

							SET_STEP_FAIL(BIB0, iDut, true);
							SET_CUM_FAIL (BIB0, iDut, true);
							SET_TEST_FAIL(BIB0, iDut, true);
							break;
						}
					}
				}
			}
		}
	}
	else if ( mode == ADD_FAIL )
	{
		uint nLmtBlkPerChip = m_lmtAddFail / m_nChipPerCe;
		//Addition Fail
//		for ( uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut )
		for ( uint iDut=0; iDut<BIB_X()*BIB_Y(); ++iDut )
		{
			if ( ! IS_MYDUT(iDut)         ) continue;
			if ( IS_PRE_FAIL (BIB0, iDut) ) continue;
//			if ( IS_STEP_FAIL(BIB0, iDut) ) continue;	//2023-05-08 fatric

			for ( uint iCe=0; iCe<m_nCePerDut; ++iCe )
			{
				for ( uint iChip=0; iChip<m_nChipPerCe; ++iChip )
				{
					uint nStart = (iDut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (iCe * m_nBlockPerCe + iChip * m_nBlockPerChip)/8;
					uint nEnd   = nStart + (m_nBlockPerChip/8);
					if ( m_nBlockPerChip&0x7 ) nEnd += 1;

					uint iFailCnt = 0;
					for( uint iBlock=nStart; iBlock<nEnd; ++iBlock )
					{
						char d = m_cum_fail_block[iBlock] & (~m_init_fail_block[iBlock]);
						iFailCnt += m_nBitCnt[ (int)d&0xff ];

						if ( iFailCnt >= nLmtBlkPerChip )
						{
							SET_STEP_FAIL_CHIP(BIB0, iDut, iCe*m_nChipPerCe+iChip, true);
							SET_CUM_FAIL_CHIP (BIB0, iDut, iCe*m_nChipPerCe+iChip, true);

							SET_STEP_FAIL(BIB0, iDut, true);
							SET_CUM_FAIL (BIB0, iDut, true);
							SET_TEST_FAIL(BIB0, iDut, true);
							break;
						}
					}
				}
			}
		}
	}
	else if ( mode == ECC_FAIL )
	{
		//
	}
	else if ( mode == MUST_FAIL )
	{
		//complement PASS/FAIL
	}
	else if ( mode == AGING )
	{
		//Burn-In Item
	}
	else if ( mode == GRADE_TEST )
	{
		//Display Fail Block "GRF1"
	}

	return 0;
}

void CApi::SET_FAULTBIN(int fbin)
{
	m_nFaultBin = fbin;
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	//g_sys.SetFaultBin( fbin );
}

void CApi::SET_PRE_FAIL(unsigned int board, unsigned int dut, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	m_init_fail[dut%m_nMaxDutPerCpu] = value;
}

void CApi::SET_PRE_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value)
{
	uint index = 0;
	if( !g_bib.GetXyToIndex(x_pos, y_pos, index ) )
	{
		EPRINTF( "%s x:%d y:%d get index error!", __FUNCTION__, x_pos, y_pos );
		return;
	}
	SET_PRE_FAIL( board, index, value );
}

bool CApi::IS_PRE_FAIL(unsigned int board, unsigned int dut)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	return static_cast<bool>( m_init_fail[dut%m_nMaxDutPerCpu] );
}

bool CApi::IS_PRE_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos)
{
	uint dut = 0x0;
	if( ! g_bib.GetXyToIndex( x_pos, y_pos, dut ) )
	{
		EPRINTF( "%s x:%d y:%d get index error!", __FUNCTION__, x_pos, y_pos );
		return true;
	}
	return IS_PRE_FAIL(board, dut);
}

void CApi::SET_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;
	if ( block >= m_nBlockPerDut )
	{
		EPRINTF("%s error : Block Address(0x%04x) Invalid.", __FUNCTION__, block );
		return;
	}

//printf("SET_STEP_BLOCK_FAIL D%03d  Block=%x", dut+1, block);
//	block = block >> m_fm2blkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut*m_nBlockPerDutBl + block/8;
	int n = block & 0x7;											//block % 8;
//printf("m=%d   n=%d\n", m, n);

	if ( value ) m_now_step_fail_block[m] |=  (1 << n);
	else         m_now_step_fail_block[m] &= ~(1 << n);

	m_use_mode |= 0x2;

}

void CApi::SET_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value)
{
	SET_STEP_BLOCK_FAIL(board, y_pos*m_bib_x + x_pos, block, value);
}

bool CApi::IS_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut*m_nBlockPerDutBl + block/8;
	int n = block & 0x7;											//block % 8;

	return static_cast<bool>( (m_now_step_fail_block[m] >> n) & 1 );
}

bool CApi::IS_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block)
{
	return IS_STEP_BLOCK_FAIL( board, y_pos*m_bib_x + x_pos, block );
}

char CApi::IS_STEP_BLOCK_FAIL8(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return 0xff;
	if ( ! IS_MYDUT(dut) ) return 0xff;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block/8;

	return m_now_step_fail_block[m];
}

int CApi::STEP_BLOCK_FAIL_COUNT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp,unsigned int minaddr, unsigned int maxaddr)
{
	return STEP_BLOCK_FAIL_COUNT( board, y_pos*m_bib_x + x_pos, mcp, minaddr, maxaddr );
}

int CApi::STEP_BLOCK_FAIL_COUNT(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr, unsigned int maxaddr )
{
	if ( ! m_bVarAvail     ) return 0;
	if ( ! IS_MYDUT(dut)   ) return 0;
	if ( maxaddr < minaddr ) return 0;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = minaddr;
	int _maxaddr = maxaddr;
//	int iOffset = dut * m_nBlockPerDutBl + ((mcp*m_nBlockPerCe)>>3);	//((mcp*m_nBlockPerCe)/8)
	int iOffset = dut * m_nBlockPerDutBl;	//((mcp*m_nBlockPerCe)/8)

	int nSum = 0;
	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (minaddr/8)==(maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		nSum = m_nBitCnt[ d&mask ];
		return nSum;
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (_minaddr>>3);			//minaddr/8;
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		nSum += m_nBitCnt[ d&mask ];
		_minaddr += 8 - nMinMod;
	}

	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			int mask = 0;
			for (int i=0; i<=nMaxMod; ++i)
				mask |= 1 << i;

			int m = iOffset + (_maxaddr>>3);		//minaddr/8;
			int d = (int)m_now_step_fail_block[m];
			d &= 0xff;
			nSum += m_nBitCnt[ d&mask ];
			_maxaddr -= nMaxMod + 1;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);	//<- i/8;
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		nSum += m_nBitCnt[ d ];
	}

	return nSum;
}

int CApi::STEP_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp, unsigned int minaddr, unsigned int maxaddr,bool cdwrite )
{
	unsigned int dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) == -1 )
	{
		//error
		dut = 0x0;
	}
	return STEP_BLOCK_FAIL_ADDRESS( board, dut, mcp, minaddr, maxaddr, cdwrite );
}

int CApi::STEP_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr,bool cdwrite )
{
//	if ( ! m_bVarAvail     ) return maxaddr;
//	if ( ! IS_MYDUT(dut)   ) return maxaddr;
//	if ( maxaddr < minaddr ) return maxaddr;
	if ( ! m_bVarAvail     ) return -1;
	if ( ! IS_MYDUT(dut)   ) return -1;
	if ( maxaddr < minaddr ) return -1;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = minaddr;
	int _maxaddr = maxaddr;
//	int iOffset = dut * m_nBlockPerDutBl + ((mcp*m_nBlockPerCe)>>3);
	int iOffset = dut * m_nBlockPerDutBl;
	int nAddr    = 0;

	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (minaddr/8)==(maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		d &= mask;

		if ( d )
		{
			nAddr = minaddr - nMinMod + m_nBitPos[ d ];

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);	//minaddr/8;
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		d &= mask;
		if ( d )
		{
			nAddr = minaddr - nMinMod + m_nBitPos[ d ];
//printf("STEP_BLOCK_FAIL_ADDRESS-1 minaddr=%d   nMinMod=%d  m_nBitPos[ %d ]=%d m=%d   d=%2x   nAddr=%x\n",
//	   minaddr, nMinMod, d, m_nBitPos[ d ], m, d, nAddr);

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
		_minaddr += 8 - nMinMod;
	}

	bool bMaxMod = false;
	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			_maxaddr -= nMaxMod + 1;
			bMaxMod   = true;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);			// i/8;
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		if ( d )
		{
			nAddr = i + m_nBitPos[ d ];
//printf("STEP_BLOCK_FAIL_ADDRESS-2 minaddr=%d   _minaddr=%d   maxaddr=%d   _maxaddr=%d   m=%d   d=%2x   nAddr=%x\n", minaddr, _minaddr, maxaddr, _maxaddr, m, (int)m_now_step_fail_block[m]&0xff, nAddr);

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	if ( bMaxMod )
	{
		int mask = 0;
		for (int i=0; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (maxaddr>>3);	//dut*m_nBlockPerDutBl + i/8;
		int d = (int)m_now_step_fail_block[m];
		d &= 0xff;
		d &= mask;

		if ( d )
		{
			nAddr = _maxaddr + m_nBitPos[ d ] + 1;
//printf("STEP_BLOCK_FAIL_ADDRESS-3 _maxaddr=%d   m=%d   d=%2x   nAddr=%x\n", _maxaddr, m, d, nAddr);

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	return -1;
}

void CApi::SET_CUM_EXCL_BLOCK(bool exclusive_cum)
{
	m_cum_exclusive = exclusive_cum;
}

bool CApi::IS_CUM_EXCL_BLOCK()
{
	return m_cum_exclusive;
}

void CApi::SET_CUM_BLOCK_FAIL (unsigned int board, unsigned int dut, unsigned int block, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;
	if ( block >= m_nBlockPerDut )
	{
		EPRINTF("%s error : Block Address(0x%04x) Invalid.", __FUNCTION__, block );
		return;
	}

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	if ( value ) m_cum_fail_block[m] |=  (1 << n);
	else         m_cum_fail_block[m] &= ~(1 << n);

//	printf("SET_CUM_BLOCK_FAIL: dut=%d  m_nMaxDutPerCpu:%d  m_nBlockPerDutBl:%d  block:#%06X\n", dut, m_nMaxDutPerCpu, m_nBlockPerDutBl, block);
//	printf("SET_CUM_BLOCK_FAIL: m=%d n=%d\n", m, n);
//	printf("SET_CUM_BLOCK_FAIL: m_cum_fail_block[%d]=%2X\n", m, m_cum_fail_block[m]);
}

void CApi::SET_CUM_BLOCK_FAIL (unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value)
{
	SET_CUM_BLOCK_FAIL(  board, y_pos*m_bib_x + x_pos, block, value );
}

bool CApi::IS_CUM_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	return static_cast<bool>( (m_cum_fail_block[m] >> n) & 1 );
}

bool CApi::IS_CUM_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block)
{
	return IS_CUM_BLOCK_FAIL( board, y_pos*m_bib_x + x_pos, block );
}

bool CApi::IS_CUM_BLOCK_FAIL8(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;

	return m_cum_fail_block[m];
}

int	CApi::CUM_BLOCK_FAIL_COUNT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp,unsigned int minaddr , unsigned int maxaddr )
{
	return CUM_BLOCK_FAIL_COUNT( board, y_pos*m_bib_x + x_pos, mcp, minaddr, maxaddr );
}

int CApi::CUM_BLOCK_FAIL_COUNT(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr )
{
	if ( ! m_bVarAvail     ) return 0;
	if ( ! IS_MYDUT(dut)   ) return 0;
	if ( maxaddr < minaddr ) return 0;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = minaddr;
	int _maxaddr = maxaddr;
//	int iOffset = dut * m_nBlockPerDutBl + ((mcp*m_nBlockPerCe)>>3);
	int iOffset = dut * m_nBlockPerDutBl;

	int nSum = 0;
	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (minaddr/8)==(maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		nSum = m_nBitCnt[ d&mask ];
		return nSum;
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		nSum += m_nBitCnt[ d&mask ];
//printf("CUM_BLOCK_FAIL_COUNT::nSum=%d    ---- 1\n", nSum);
		_minaddr += 8 - nMinMod;
	}

	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			int mask = 0;
			for (int i=0; i<=nMaxMod; ++i)
				mask |= 1 << i;

			int m = iOffset + (maxaddr>>3);
			int d = (int)m_cum_fail_block[m];
			d &= 0xff;
			nSum += m_nBitCnt[ d&mask ];
//printf("CUM_BLOCK_FAIL_COUNT::nSum=%d    ---- 2\n", nSum);
			_maxaddr -= nMaxMod + 1;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);	//<- i/8;
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		nSum += m_nBitCnt[ d ];
//printf("CUM_BLOCK_FAIL_COUNT::nSum=%d    ---- 3\n", nSum);
	}

	return nSum;
}

//int CApi::CUM_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int x_pos, unsigned int	y_pos, unsigned int mcp, unsigned int minaddr, unsigned int maxaddr,bool cdwrite)
//{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
//	return 0;
//}

int	CApi::CUM_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr,bool cdwrite)
{
//	if ( ! m_bVarAvail     ) return maxaddr;
//	if ( ! IS_MYDUT(dut)   ) return maxaddr;
//	if ( maxaddr < minaddr ) return maxaddr;
	if ( ! m_bVarAvail     ) return -1;
	if ( ! IS_MYDUT(dut)   ) return -1;
	if ( maxaddr < minaddr ) return -1;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = minaddr;
	int _maxaddr = maxaddr;
//	int iOffset = dut * m_nBlockPerDutBl + ((mcp*m_nBlockPerCe)>>3);
	int iOffset = dut * m_nBlockPerDutBl;
	int nAddr    = 0;

	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (minaddr/8)==(maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		d &= mask;

		if ( d )
		{
			nAddr = minaddr - nMinMod + m_nBitPos[ d ];

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (minaddr>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		d &= mask;
		if ( d )
		{
			nAddr = minaddr - nMinMod + m_nBitPos[ d ];

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
		_minaddr += 8 - nMinMod;
	}

	bool bMaxMod = false;
	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			_maxaddr -= nMaxMod + 1;
			bMaxMod   = true;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		if ( d )
		{
			nAddr = i + m_nBitPos[ d ];

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	if ( bMaxMod )
	{
		int mask = 0;
		for (int i=0; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (maxaddr>>3);
		int d = (int)m_cum_fail_block[m];
		d &= 0xff;
		d &= mask;

		if ( d )
		{
			nAddr = _maxaddr + m_nBitPos[ d ] + 1;

			if ( cdwrite ) return nAddr - mcp*m_nBlockPerCe;
			else           return nAddr;
		}
	}

	return -1;
}

void CApi::CLR_CUM_BLOCK_FAIL()
{
	memset( m_cum_fail_block, 0, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl );
}

void CApi::SET_ADD_LIMIT_STEP ( int block_add_limit_step )
{
	m_lmtAddFail = block_add_limit_step;
}

void CApi::SET_INIT_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value)
{
	SET_INIT_BLOCK_FAIL( board, y_pos*m_bib_x + x_pos, block, value );
}

void CApi::SET_INIT_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	if ( value ) m_init_fail_block[m] |=  (1 << n);
	else         m_init_fail_block[m] &= ~(1 << n);
}

bool CApi::IS_INIT_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	return static_cast<bool>( (m_init_fail_block[m] >> n) & 1 );
}

bool CApi::IS_INIT_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block)
{
	return IS_INIT_BLOCK_FAIL( board, y_pos*m_bib_x + x_pos, block );
}

char CApi::IS_INIT_BLOCK_FAIL8(uint board, uint dut, uint block)
{
	if ( ! m_bVarAvail   ) return 0xff;
	if ( ! IS_MYDUT(dut) ) return 0xff;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;

	return m_init_fail_block[m];
}

void CApi::SET_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value)
{
	SET_GT_STEP_BLOCK_FAIL(  board, y_pos*m_bib_x + x_pos, block, value );
}

void CApi::SET_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	if ( value ) m_get_step_fail_block[m] |=  (1 << n);
	else         m_get_step_fail_block[m] &= ~(1 << n);
}

bool CApi::IS_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block)
{
	return IS_GT_STEP_BLOCK_FAIL( board, y_pos*m_bib_x + x_pos, block );
}

bool CApi::IS_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

//	block = block >> m_addBlkShift;
	int m = (dut%m_nMaxDutPerCpu) * m_nBlockPerDutBl + (block>>3);	//dut * m_nBlockPerDutBl + block / 8;
	int n = block & 0x7;											//block % 8;

	return static_cast<bool>( (m_get_step_fail_block[m] >> n) & 1 );
}

void CApi::SET_STEP_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, bool value)
{
	SET_STEP_FAIL_CHIP(board, y_pos*m_bib_x + x_pos, chip_index, value);
}

void CApi::SET_STEP_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	int m = (dut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;											//chip_index % 8;

	if ( value ) m_step_fail_chip[m] |=  (1 << n);
	else         m_step_fail_chip[m] &= ~(1 << n);
}

bool CApi::IS_STEP_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	return IS_STEP_FAIL_CHIP( board, y_pos*m_bib_x + x_pos, chip_index );
}

bool CApi::IS_STEP_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	int m = (dut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;											//chip_index % 8;

	return static_cast<bool>( (m_step_fail_chip[m] >> n) & 1 );
}

void CApi::SET_CUM_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, bool value)
{
	SET_CUM_FAIL_CHIP(board, y_pos*m_bib_x + x_pos, chip_index, value);
}

void CApi::SET_CUM_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	int m = (dut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;											//chip_index % 8;

	if ( value ) m_cum_fail_chip[m] |=  (1 << n);
	else         m_cum_fail_chip[m] &= ~(1 << n);
}

bool CApi::IS_CUM_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	return IS_CUM_FAIL_CHIP( board, y_pos*m_bib_x + x_pos, chip_index );
}

bool CApi::IS_CUM_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	int m = (dut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;											//chip_index % 8;

	return static_cast<bool>( (m_cum_fail_chip[m] >> n) & 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DUT FAIL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_STEP_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value)
{
	SET_STEP_FAIL( board, y_pos*m_bib_x + x_pos, value );
}

void CApi::SET_STEP_FAIL(unsigned int board, unsigned int dut, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	m_step_fail[dut%m_nMaxDutPerCpu] = value;
}

bool CApi::IS_STEP_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos)
{
	return IS_STEP_FAIL(board, y_pos*m_bib_x + x_pos);
}

bool CApi::IS_STEP_FAIL(unsigned int board, unsigned int dut)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	return static_cast<bool>( m_step_fail[dut%m_nMaxDutPerCpu] );
}

void CApi::SET_CUM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value)
{
	SET_CUM_FAIL( board, y_pos*m_bib_x + x_pos, value );
}

void CApi::SET_CUM_FAIL(unsigned int board, unsigned int dut, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	m_cum_fail[dut%m_nMaxDutPerCpu] = value;
//PRINTF("%s:: m_cum_fail[%d%%%d] = %s", __FUNCTION__, dut, m_nMaxDutPerCpu, value?"true":"false");
}

bool CApi::IS_CUM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos)
{
	return IS_CUM_FAIL(board, y_pos*m_bib_x + x_pos);
}

bool CApi::IS_CUM_FAIL(unsigned int board, unsigned int dut)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	return static_cast<bool>( m_cum_fail[dut%m_nMaxDutPerCpu] );
}

void CApi::SET_TEST_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value)
{
	SET_TEST_FAIL( board, y_pos*m_bib_x + x_pos, value );
}

void CApi::SET_TEST_FAIL(unsigned int board, unsigned int dut, bool value)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	m_test_fail[dut%m_nMaxDutPerCpu] = value;
}

bool CApi::IS_TEST_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos)
{
	return IS_TEST_FAIL(board, y_pos*m_bib_x + x_pos);
}

bool CApi::IS_TEST_FAIL(unsigned int board, unsigned int dut)
{
	if ( ! m_bVarAvail   ) return true;
	if ( ! IS_MYDUT(dut) ) return true;

	return static_cast<bool>( m_test_fail[dut%m_nMaxDutPerCpu] );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CHIP ID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::INIT_READ_CHIPID()
{
	SET_CHIPID_STEP( true );
	memset( m_tmp_fm_data, 0x0, sizeof(m_tmp_fm_data) );
}

int CApi::RESULT_CHIPID_DATA(unsigned int mode)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

void CApi::SET_FM_READ_DATA_INDEX(unsigned int v)
{
	m_nFmReadDataIndex = v;
}

void CApi::SET_CHIPID_V(unsigned int board, unsigned int dut, unsigned int chip_index, char *value)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return;
	}
	strncpy( m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].szchipId, value, sizeof(m_pStChipId[(dut%m_nMaxDutPerCpu)*chip_index].szchipId) );
}

void CApi::SET_CHIPID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, char* val)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return;
	}
	SET_CHIPID_V( board, dut, chip_index, val );
}

void CApi::SET_WAFERID_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return;
	}
	m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uWaferId = value;
}

void CApi::SET_WAFERID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return;
	}
	SET_WAFERID_V( board, dut, chip_index, value );
}

void CApi::SET_XPOS_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return;
	}
	m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uXPos = value;
}

void CApi::SET_XPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return;
	}
	SET_XPOS_V( board, dut, chip_index, value );
}

void CApi::SET_YPOS_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return;
	}
	m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uYPos = value;
}

void CApi::SET_YPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return;
	}
	SET_YPOS_V( board, dut, chip_index, value );
}

char* CApi::CHIPID_V(unsigned int board, unsigned int dut, unsigned int chip_index)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return nullptr;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return nullptr;
	}
	return m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].szchipId;
}

char* CApi::CHIPID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return NULL;
	}
	return CHIPID_V( board, dut, chip_index );
}

int CApi::WAFERID_V(unsigned int board, unsigned int dut, unsigned int chip_index)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return -1;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return -1;
	}
	return m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uWaferId;
}

int CApi::WAFERID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return -1;
	}
	return WAFERID_V( board, dut, chip_index );
}

int CApi::XPOS_V(unsigned int board, unsigned int	dut, unsigned int chip_index)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return -1;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return -1;
	}
	return m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uXPos;
}

int CApi::XPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return -1;
	}
	return XPOS_V( board, dut, chip_index );
}

int CApi::YPOS_V(unsigned int board, unsigned int	dut, unsigned int chip_index)
{
	(void)board;
	if( !IS_CHIPID_STEP() )
	{
		return -1;
	}
	if( chip_index > static_cast<uint>((m_stChipData.chip_count-1)) )
	{
		return -1;
	}
	return m_pStChipId[((dut%m_nMaxDutPerCpu)*m_stChipData.chip_count)+chip_index].uYPos;
}

int CApi::YPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index)
{
	uint dut = 0x0;
	if( xy_to_index( x_pos, y_pos, dut ) < 0 )
	{
		EPRINTF( "%s x:%02d y:%02d xy_to_index error!", __FUNCTION__, x_pos, y_pos );
		return -1;
	}
	return YPOS_V( board, dut, chip_index );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NAND CHIP ID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CApi::SET_READ_NAND_CHIPID()
{
	INIT_READ_CHIPID();
}

void CApi::READ_CHIP_DATA(unsigned short chip_count, unsigned short data_length, unsigned short chip_size, const char *data_type , int chipid_shift)
{
	m_stChipData.chip_count   = chip_count;
	m_stChipData.data_length  = data_length;
	m_stChipData.chip_size    = chip_size;
	m_stChipData.chipid_shift = chipid_shift;
	strcpy( m_stChipData.data_format, data_type);
	if( !m_bVarAvail )
	{
		EPRINTF( "%s error : SET_BIB/SET_ADD_SIZE/SET_BIB first", __FUNCTION__ );
		return;
	}

	if( !m_pStChipId )
	{
		m_pStChipId = new stChipId[ m_nMaxDutPerCpu * chip_count ];
		memset( m_pStChipId, 0x0, sizeof(stChipId) * m_nMaxDutPerCpu * chip_count );
		PRINTF( "%s done.", __FUNCTION__ );
	}
}

int CApi::SET_FM_READ_DATA(unsigned int scan, unsigned int block_addr, unsigned int data_index )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DRAM CHIP ID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::DRAM_READ_CHIP_DATA(unsigned short chip_count, const char *data_type)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

int CApi::SET_DRAM_READ_CHIPID(unsigned int index )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// EMMC CHIP ID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::EMMC_READ_CHIP_DATA(unsigned short chip_count, const char *data_type)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

int CApi::SET_EMMC_READ_DATA(unsigned int index )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

void CApi::SET_EMMC_CHIPID_UNIT_COUNT(unsigned short v)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned short CApi::EMMC_CHIPID_UNIT_COUNT()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

void CApi::SET_EMMC_CE(unsigned short v)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned short CApi::EMMC_CE()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CATEGORY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_FCAT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int fail_cate)
{
	uint dut = 0x0;
	if( !g_bib.GetXyToIndex( x_pos, y_pos, dut ) ) return;
	return SET_FCAT( board, dut, fail_cate );
}

void CApi::SET_FCAT(unsigned int board, unsigned int dut, unsigned int fail_cate)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + (fail_cate>>3);	//dut * m_nCatePerDut + fail_cate / 8;
	int n = fail_cate & 0x7;										//fail_cate % 8;

	m_fail_cate[m] |= (1 << n);
	m_step_fail_cate[dut%m_nMaxDutPerCpu] = fail_cate;	//2024.07.22 fatric
}

void CApi::SET_PCAT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int pass_cate)
{
	return SET_PCAT( board, y_pos*m_bib_x + x_pos, pass_cate );
}

void CApi::SET_PCAT(unsigned int board, unsigned int dut, unsigned int pass_cate)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;

	int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + (pass_cate>>3);	//dut * m_nCatePerDut + fail_cate / 8;
	int n = pass_cate & 0x7;										//fail_cate % 8;

	m_pass_cate[m] |= (1 << n);
	m_step_pass_cate[dut%m_nMaxDutPerCpu] = pass_cate;	//2024.07.22 fatric
}

bool CApi::FCAT( int board, unsigned int x_pos, unsigned int y_pos, unsigned int fail_cate,unsigned int fail_cate_end)
{
	uint dut = 0x0;
	if( !g_bib.GetXyToIndex( x_pos, y_pos, dut ) ) return false;
	return FCAT( board, dut, fail_cate, fail_cate_end );
}

//bool CApi::FCAT( int board, unsigned int dut, unsigned int fail_cate, unsigned int fail_cate_end)
//{
//	if ( ! m_bVarAvail             ) return false;
//	if ( ! IS_MYDUT(dut)           ) return false;
//	if ( fail_cate_end < fail_cate ) return false;
//
//	dut = dut % m_nMaxDutPerCpu;
//
//	int _minaddr = fail_cate;
//	int _maxaddr = fail_cate_end;
//
//	int nMinMod = fail_cate&0x7;
//	int nMaxMod = fail_cate_end&0x7;
//
//	if ( nMinMod > 0 )
//	{
//		char mask = 0;
//		for (int i=nMinMod; i<8; ++i)
//			mask |= 1 << i;
//
//		int m = dut * m_nCatePerDut + (_minaddr>>3);	//dut * m_nCatePerDut + fail_cate / 8;
//		if ( m_fail_cate[m]&mask )
//			return true;
//		_minaddr += 8 - nMinMod;
//	}
//
//	if ( _maxaddr > _minaddr )
//	{
//		if ( nMaxMod < 7 )
//		{
//			char mask = 0;
//			for (int i=0; i<=nMaxMod; ++i)
//				mask |= 1 << i;
//
//			int m = dut * m_nCatePerDut + (_maxaddr>>3);	//dut * m_nCatePerDut + block / 8;
//			if ( m_fail_cate[m]&mask )
//				return true;
//			_maxaddr -= nMaxMod + 1;
//		}
//	}
//
//	int iDutOffset = dut * m_nCatePerDut;
//	for(int i=_minaddr; i<=_maxaddr; i+=8 )
//	{
//		int m = iDutOffset + (i>>3);	//<- i/8;
//		if ( m_fail_cate[m] )
//			return true;
//	}
//
//	return false;
//}

//2023.07.12 - KWMOON( Comment and new function add )
bool CApi::FCAT( int board, unsigned int dut, unsigned int fail_cate, unsigned int fail_cate_end)
{
	if ( ! m_bVarAvail             ) return false;
	if ( ! IS_MYDUT(dut)           ) return false;
	if ( fail_cate_end < fail_cate ) return false;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = fail_cate;
	int _maxaddr = fail_cate_end;
	int iOffset = dut * m_nCatePerDut;

	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (_minaddr/8)==(_maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (_minaddr>>3);
		int d = (int)m_fail_cate[m];
		d &= 0xff;
		d &= mask;

		if ( d != 0 ) return true;
		else          return false;
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (_minaddr>>3);
		int d = (int)m_fail_cate[m];
		d &= 0xff;
		d &= mask;

		if ( d != 0 ) return true;
		_minaddr += 8 - nMinMod;
	}

	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			int mask = 0;
			for (int i=0; i<=nMaxMod; ++i)
				mask |= 1 << i;

			int m = iOffset + (_maxaddr>>3);
			int d = (int)m_fail_cate[m];
			d &= 0xff;
			d &= mask;

			if ( d != 0 ) return true;
			_maxaddr -= nMaxMod + 1;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);	//<- i/8;
		int d = (int)m_fail_cate[m];
		d &= 0xff;

		if ( d != 0 ) return true;
	}

	return false;
}

bool CApi::PCAT( int board, unsigned int x_pos, unsigned int y_pos, unsigned int pass_cate,unsigned int pass_cate_end)
{
	uint dut = 0x0;
	if( !g_bib.GetXyToIndex( x_pos, y_pos, dut ) ) return false;
	return PCAT( board, dut, pass_cate, pass_cate_end );
}

//2023.07.12 - KWMOON( compare and modify )
bool CApi::PCAT( int board, unsigned int dut, unsigned int pass_cate, unsigned int pass_cate_end)
{
	if ( ! m_bVarAvail             ) return false;
	if ( ! IS_MYDUT(dut)           ) return false;
	if ( pass_cate_end < pass_cate ) return false;

	dut = dut % m_nMaxDutPerCpu;

	int _minaddr = pass_cate;
	int _maxaddr = pass_cate_end;
	int iOffset = dut * m_nCatePerDut;

	int nMinMod = _minaddr&0x7;
	int nMaxMod = _maxaddr&0x7;

	if ( (_minaddr/8)==(_maxaddr/8) )
	{
		int mask = 0;
		for (int i=nMinMod; i<=nMaxMod; ++i)
			mask |= 1 << i;

		int m = iOffset + (_minaddr>>3);
		int d = (int)m_pass_cate[m];
		d &= 0xff;
		d &= mask;

		if ( d != 0 ) return true;
		else          return false;
	}

	if ( nMinMod > 0 )
	{
		int mask = 0;
		for (int i=nMinMod; i<8; ++i)
			mask |= 1 << i;

		int m = iOffset + (_minaddr>>3);
		int d = (int)m_pass_cate[m];
		d &= 0xff;
		d &= mask;

		if ( d != 0 ) return true;
		_minaddr += 8 - nMinMod;
	}

	if ( _maxaddr > _minaddr )
	{
		if ( nMaxMod < 7 )
		{
			int mask = 0;
			for (int i=0; i<=nMaxMod; ++i)
				mask |= 1 << i;

			int m = iOffset + (_maxaddr>>3);
			int d = (int)m_pass_cate[m];
			d &= 0xff;
			d &= mask;

			if ( d != 0 ) return true;
			_maxaddr -= nMaxMod + 1;
		}
	}

	for(int i=_minaddr; i<=_maxaddr; i+=8 )
	{
		int m = iOffset + (i>>3);	//<- i/8;
		int d = (int)m_pass_cate[m];
		d &= 0xff;

		if ( d != 0 ) return true;
	}

	return false;
}

void CApi::SET_BIN(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int bin)
{
	return SET_BIN( board, y_pos*m_bib_x + x_pos, bin );
}

void CApi::SET_BIN(unsigned int board, unsigned int dut, unsigned int bin)
{
	if ( ! m_bVarAvail   ) return;
	if ( ! IS_MYDUT(dut) ) return;
	if ( bin > 15        ) return;					//2023.06.13 - kwmoon(add fatric)	//BIN1 ~ BIN15

//	m_bin[dut%m_nMaxDutPerCpu] = bin;				//2023.06.13 - kwmoon(comment fatric)
	m_bin[dut%m_nMaxDutPerCpu] = m_bin_map[bin];	//2023.06.13 - kwmoon(add fatric)
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// EMMC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CApi::SET_EMMC_TEST_MODE( EMMC_TEST_MODE testmode )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned int CApi::GET_EMMC_TEST_MODE()
{
	EMMC_TEST_MODE mode = BISTX1;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return static_cast<unsigned int>(mode);
}

void CApi::SET_EMMC_BIT_VALUE(unsigned char v )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_EMMC_EXPECT(unsigned int bit, unsigned long long value)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned int CApi::EMMC_EXPECT_BIT( )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

unsigned int long long CApi::EMMC_EXPECT_VALUE()
{
	unsigned int long long value = 0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return value;
}

void CApi::SET_EMMC_COMPARE(unsigned int bit)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_EMMC_COMPARE(unsigned int bit, unsigned long long value)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned int CApi::EMMC_COMPARE_BIT()
{
	unsigned int value = 0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return value;
}

unsigned int long long CApi::EMMC_COMPARE_VALUE()
{
	unsigned int long long value = 0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return value;
}

void CApi::SET_EMMC_COMPARE_STRING(const char* compare_string)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

unsigned short CApi::EMMC_COMPARE_STRING_SIZE()
{
	unsigned int value = 0x0;
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return value;
}

char* CApi::EMMC_COMPARE_STRING()
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return nullptr;
}

void CApi::GET_EXTFM_EMMC_IO(int limit, int mode, int format, bool printf)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::GET_DRV_EMMC_IO (int drvdepth, int mode, int format, bool printf)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CApi::CID_X_CAL(int x)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return x;
}

int CApi::CID_Y_CAL(int y)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return y;
}

int CApi::CID_WAFER_CAL(int wafer)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return wafer;
}

std::string CApi::CID_LOTID_CAL(std::string lotid)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return lotid;
}

void CApi::SET_BLOCK(int addr, int s, int& address, int &scan )
{
	address = (unsigned int) addr >> m_block_shift;
	scan = s ;
}

void CApi::SET_COLUMN (int addr, int &address)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_CHUNK (int addr, int scan , int &chunk )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

bool CApi::RUNTIME_ERROR( const char* func,  const char* str, ... )
{
	va_list	ap;
	char buf[2048];

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	//PRINTF( "RUNTIME ERROR : %s", str );
	if( !GetAutoMode() )
	{
		g_log.RuntimeError( COLOR_RED "%s" COLOR_NC, buf );
	}
	else
	{
		if( !g_devsock.IsConn() )
			g_devsock.ConnectToHost();
		g_log.RuntimeError( _USER, func, buf );
	}
	g_log.RuntimeError( NULL );
	sysDelay( 100 MS );

	RESET_TESTER();
	exit(-1);
	return true;
}

int CApi::OPTION_CALL(int command, int wparam, int lparam )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

int CApi::DO_EMMCDATA_FMT_USER( int format )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DEBUG
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//2023.08.24 - remove
//void CApi::PRINT ( const char* szFmt, ... )
//{
//	char cBuff[8192];
//	va_list argptr;
//
//	if( szFmt == NULL )
//	{
//		g_log.Print(NULL);
//		return;
//	}
//
//	va_start(argptr, szFmt);
//	vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
//	va_end(argptr);
//
//	if( !GetAutoMode() )
//		g_log.Print( COLOR_NC "%s" COLOR_NC, cBuff );
//	else
//		g_log.Print( "%s", cBuff );
//}

void CApi::EPRINT ( const char* szFmt, ... )
{
	char cBuff[8192];
	va_list argptr;

	if( szFmt == NULL )
	{
		g_log.Print(NULL);
		return;
	}

	va_start(argptr, szFmt);
	vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
	va_end(argptr);

	if( !GetAutoMode() )
		g_log.Print( COLOR_RED "%s" COLOR_NC, cBuff );
	else
		g_log.Print( "%s", cBuff );
}

void CApi::PRINTF ( const char* szFmt, ... )
{
	char cBuff[8192]={0x0,};
	va_list argptr;

	if( szFmt == NULL )
	{
		g_log.Printf(NULL);
		return;
	}

	va_start(argptr, szFmt);
	vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
	va_end(argptr);

	if( !GetAutoMode() )
		g_log.Printf( COLOR_NC "%s\n" COLOR_NC, cBuff );
	else
		g_log.Printf( "%s\n", cBuff );
}

void CApi::EPRINTF( const char* szFmt, ... )
{
	char cBuff[8192];
	va_list argptr;

	if( szFmt == NULL )
	{
		g_log.Printf(NULL);
		return;
	}

	va_start(argptr, szFmt);
	vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
	va_end(argptr);

	if( !GetAutoMode() )
		g_log.Printf( COLOR_RED "%s\n" COLOR_NC, cBuff );
	else
		g_log.Printf( "%s\n", cBuff );
}

void CApi::SET_PRINTF_DUT( bool enb, int printf_dut )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::SET_PRINTF_PAT( bool enb )
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::DBG_DUMP_PATTERN()
{
	g_pg.DumpPattern();
}

void CApi::DBG_DUMP_TIMESET()
{
	g_tg.PrintRate();
	g_tg.PrintClock();
}

void CApi::PRINTF_REG()
{
	g_pg.PrintGlobalReg();
}

void CApi::PRINTF_REG_WITH_M(unsigned int fc, int start, int end)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::PRINTF_PD(unsigned int fc)
{
	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::PRINTF_EXTFM(int fc , int limit )
{
	g_dfm.PrintExtFm(fc, limit);
}

void CApi::PRINTF_LATCH(int fc)
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	FM_MEM	lData;	//LATCH DATA
	if ( fc != FC00 && fc != FC01 )
		 EPRINTF("%s error : fc:%d error!", __FUNCTION__, fc );

//	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		//1. Get Latch Data
		lData.header.header = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_REAL_SCAN    + fc*0x20 );
		lData.data[0].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_0_15  + fc*0x20 );
		lData.data[1].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_16_31 + fc*0x20 );
		lData.data[2].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_32_47 + fc*0x20 );
		if ( m_fail_mode == MUST_FAIL )
		{
			lData.data[0].ioab = ~lData.data[0].ioab;
			lData.data[1].ioab = ~lData.data[1].ioab;
			lData.data[2].ioab = ~lData.data[2].ioab;
		}

		if( g_pg.GetIoDdrModeGlobal() == DDR )
		{
			FM_MEM	lDataF;
			lDataF.data[0].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_0_15  + fc*0x20 );
			lDataF.data[1].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_16_31 + fc*0x20 );
			lDataF.data[2].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_32_47 + fc*0x20 );
			if ( m_fail_mode == MUST_FAIL )
			{
				lDataF.data[0].ioab = ~lDataF.data[0].ioab;
				lDataF.data[1].ioab = ~lDataF.data[1].ioab;
				lDataF.data[2].ioab = ~lDataF.data[2].ioab;
			}
			lData.data[0].ioab |= lDataF.data[0].ioab;
			lData.data[1].ioab |= lDataF.data[1].ioab;
			lData.data[2].ioab |= lDataF.data[2].ioab;
		}

		uint realscan = lData.header.header;	//Real Scan Value: low 8bit
		PRINTF( "PRINTF_LATCH::FC%d LATCH [Scan:0x%02X] [%08X] [%08X] [%08X]", fc, realscan, lData.data[2].ioab, lData.data[1].ioab, lData.data[0].ioab );
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ETC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CApi::GET_IOA_DUTBIT()
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return g_bib.GetBibIo();
}

int CApi::GET_IOB_DUTBIT()
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
	return g_bib.GetBibIo();
}

int CApi::GET_IOA_DUTMASK()
{
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
		 if( g_bib.GetBibIo() ==  8 ) return 0xFF;
	else if( g_bib.GetBibIo() == 16 ) return 0xFFFF;
	else if( g_bib.GetBibIo() == 24 ) return 0xFFFFFF;
	return 0xFF;
}

int CApi::GET_IOB_DUTMASK()
{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
		 if( g_bib.GetBibIo() ==  8 ) return 0xFF;
	else if( g_bib.GetBibIo() == 16 ) return 0xFFFF;
	else if( g_bib.GetBibIo() == 24 ) return 0xFFFFFF;
	return 0xFF;
}

//ONE FC(FC1~4) IOA OR IOB ONE SCAN DUT COUNT
int CApi::GET_DUTPERFC()
{
	dut_per_fc = g_bib.GetDutPerFc();
	return dut_per_fc;
}

//ONE FC(FC1~4) CPU ONE SCAN DUT COUNT
int CApi::GET_DUTPERCPU()
{
	dut_per_cpu = g_bib.GetDutPerCpu();
	return dut_per_cpu;
}

void CApi::sysResetLatch()
{
	g_pg.CmpClear();
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

void CApi::setFMClear()
{
	CLR_FM_GLOBAL();
	//EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
}

//void CApi::setScanMask(bool bMask, int data)
//{
//	EPRINTF( "%s is an unimplemented function.", __FUNCTION__ );
//}

void CApi::CLR_AUTOMASK_MEM_GLOBAL()
{
	//for( int fc=0;fc<USE_FC;fc++ )
	//{
	//	for( int m=0;m<SCAN_MEM_DEPTH;m++ )
	//	{
	//		for( int d=0;d<3;d++ )
	//		{
	//			m_auto_mask[fc][m][d] = 0x0;
	//		}
	//	}
	//}
}

void CApi::WRITE_AUTO_SCANMASK(bool bprint, uint realscan)
{
	//for( uint fc = 0;fc<USE_FC;fc++ )
	//{
	//	for(uint m=0;m<SCAN_MEM_DEPTH;m++ )
	//	{
	//		for( uint d=0;d<3;d++ )
	//		{
	//			g_bib.WriteScanMask( fc, m, d, m_auto_mask[fc][m][d] );
	//			if( bprint && m_auto_mask[fc][m][d] )
	//			{
	//				PRINTF( "%s Realscan[%02d] Index[%02d] ScanMask[IOB:%04X IOA:%04X]",
	//						__FUNCTION__, realscan, m, (m_auto_mask[fc][m][d]>>16)&0xffff, m_auto_mask[fc][m][d]&0xffff );
	//			}
	//		}
	//	}
	//}
}

//2023.06.13 - kwmoon(add fatric)
void CApi::MULTIBIN_SET(int iBin10, int iBin11, int iBin12, int iBin13, int iBin14, int iBin15)
{
	if ( iBin10 > 0 ) m_bin_map[10] = iBin10;
	if ( iBin11 > 0 ) m_bin_map[11] = iBin11;
	if ( iBin12 > 0 ) m_bin_map[12] = iBin12;
	if ( iBin13 > 0 ) m_bin_map[13] = iBin13;
	if ( iBin14 > 0 ) m_bin_map[14] = iBin14;
	if ( iBin15 > 0 ) m_bin_map[15] = iBin15;
}

//2023.06.13 - kwmoon(add fatric)
void CApi::SET_BIN_REF1(double dBin1, double dBin2 , double dBin3 , double dBin4 , double dBin5 , double dBin6 , double dBin7 , double dBin8,
						double dBin9, double dBin10, double dBin11, double dBin12, double dBin13, double dBin14, double dBin15 )
{
	if ( dBin1  > 0.0 ) m_bin_limit[ 1] = dBin1 ;
	if ( dBin2  > 0.0 ) m_bin_limit[ 2] = dBin2 ;
	if ( dBin3  > 0.0 ) m_bin_limit[ 3] = dBin3 ;
	if ( dBin4  > 0.0 ) m_bin_limit[ 4] = dBin4 ;
	if ( dBin5  > 0.0 ) m_bin_limit[ 5] = dBin5 ;
	if ( dBin6  > 0.0 ) m_bin_limit[ 6] = dBin6 ;
	if ( dBin7  > 0.0 ) m_bin_limit[ 7] = dBin7 ;
	if ( dBin8  > 0.0 ) m_bin_limit[ 8] = dBin8 ;
	if ( dBin9  > 0.0 ) m_bin_limit[ 9] = dBin9 ;
	if ( dBin10 > 0.0 ) m_bin_limit[10] = dBin10;
	if ( dBin11 > 0.0 ) m_bin_limit[11] = dBin11;
	if ( dBin12 > 0.0 ) m_bin_limit[12] = dBin12;
	if ( dBin13 > 0.0 ) m_bin_limit[13] = dBin13;
	if ( dBin14 > 0.0 ) m_bin_limit[14] = dBin14;
	if ( dBin15 > 0.0 ) m_bin_limit[15] = dBin15;
}

void CApi::SET_DEBUG_MODE(int mode)
{
	m_debug_mode = mode;
}

void CApi::initTestEnd()
{
	if( g_sys.GetDevice() != DIAGCAL )
	{
		DISPLAY_PREFAIL_DUTMAP();
		/*
		if ( IsInitStep() )
		{
			CreateInitFailData();

			for(uint i=0; i<m_vStepData.size(); ++i )
				delete m_vStepData[i];
			m_vStepData.clear();
			m_vStepData.push_back( new CStepData( ++m_iTsNo, m_tStepBeginTime, m_tStepEndTime, m_stSetTemp.dTemp, m_nBiTime, m_sStepName.c_str(), m_sStepName.c_str(), 0.0 ) );
		}
		 */
	}
}

void CApi::DISPLAY_PREFAIL_DUTMAP()
{
	if ( (m_debug_mode&1)==0 ) return;
//	for( int dut=0;dut<BIB_X()*BIB_Y();dut++ )
//	{
//		if ( !IS_MYDUT(dut) )
//			continue;
//		else
//			fprintf(stdout, "PREFAIL DUT%03d = %s\n", dut, IS_PRE_FAIL(BIB0,dut)?"*FAIL":" PASS" );
//	}

	PRINTF( "[%s]", __FUNCTION__ );
	PRINTF(NULL);

	std::string strCr = "\n";
	std::string strData = "";
	char szLine[256]={0x0,};
	memset( szLine, 0x0, sizeof(szLine) );
	for ( uint r = 0;r<BIB_Y();r++ )
	{
		sprintf( szLine, "C%02d | ",  BIB_Y()-r );
		strData += szLine;
		for ( uint c = 0;c<BIB_X();c++ )
		{
			int dut = DUTMAP_12x16[r][c];
			if ( !IS_MYDUT(dut) )
			{
				sprintf( szLine, "  %s   ", "-");
				strData += szLine;
				continue;
			}
			if ( IS_PRE_FAIL( BIB0,  dut ) )
			{
				sprintf( szLine, "  %s   ", "F" );
				strData += szLine;
			}
			else
			{
				sprintf( szLine, "  %s   ", "P" );
				strData += szLine;
			}
		}
		PRINTF( "%s", strData.c_str() );
		strData = "";
	}
	sprintf( szLine, "%6s", "");
	strData += szLine;
	for ( uint c = 0;c<BIB_X();c++ )
	{
		sprintf( szLine, " R%02d  ", c+1);
		strData += szLine;
	}
	strData += "\n";
	PRINTF( "%s", strData.c_str() );
	PRINTF(NULL);
}

void CApi::DISPLAY_STEPFAIL_DUTMAP()
{
	if ( (m_debug_mode&2)==0 ) return;

//	for( int dut=0;dut<BIB_X()*BIB_Y();dut++ )
//	{
//		if ( !IS_MYDUT(dut) )
//			continue;
//		else
//			fprintf(stdout, "STEPFAIL DUT%03d = %s\n", dut, IS_STEP_FAIL(BIB0,dut)?"*FAIL":" PASS" );
//	}

	PRINTF( "[%s]", __FUNCTION__ );
	PRINTF( NULL );

	std::string strData = "\n";
	char szLine[256]={0x0,};
	for ( uint r = 0;r<BIB_Y();r++ )
	{
		snprintf( szLine, sizeof(szLine), "C%02d | ",  BIB_Y()-r );
		strData += szLine;
		for ( uint c = 0;c<BIB_X();c++ )
		{
			int dut = DUTMAP_12x16[r][c];
			if ( !IS_MYDUT(dut) )
			{
				snprintf( szLine, sizeof(szLine), "  %s   ", "-");
				strData += szLine;
				continue;
			}
			if ( IS_STEP_FAIL( BIB0,  dut ) )
			{
				snprintf( szLine, sizeof(szLine), "  %s   ", "F" );
				strData += szLine;
			}
			else
			{
				snprintf( szLine, sizeof(szLine), "  %s   ", "P" );
				strData += szLine;
			}
		}
		PRINTF( "%s", strData.c_str() );
		strData = "";
	}

	snprintf( szLine, sizeof(szLine), "%6s", "" );
	strData += szLine;

	for ( uint c = 0;c<BIB_X();c++ )
	{
		snprintf( szLine, sizeof(szLine), " R%02d  ", c+1);
		strData += "\n";
	}
	PRINTF( "%s", strData.c_str() );
	PRINTF(NULL);
}

void CApi::DISPLAY_LATCH()
{
	if ( (m_debug_mode&4)==0 ) return;

	//Set step fail, cum fail
	FM_MEM	lData;	//LATCH DATA
	for( uint fc = 0;fc<USE_FC;fc++ )
	{
		//1. Get Latch Data
		lData.header.header = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_REAL_SCAN    + fc*0x20 );
		lData.data[0].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_0_15  + fc*0x20 );
		lData.data[1].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_16_31 + fc*0x20 );
		lData.data[2].ioab  = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_R_IOAB_32_47 + fc*0x20 );
		if ( m_fail_mode == MUST_FAIL )
		{
			lData.data[0].ioab = ~lData.data[0].ioab;
			lData.data[1].ioab = ~lData.data[1].ioab;
			lData.data[2].ioab = ~lData.data[2].ioab;
		}

		if( g_pg.GetIoDdrModeGlobal() == DDR )
		{
			FM_MEM	lDataF;
			lDataF.data[0].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_0_15  + fc*0x20 );
			lDataF.data[1].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_16_31 + fc*0x20 );
			lDataF.data[2].ioab = g_pcie.read_user( BITCHART::ALPG::LATCH_DATA_MEMORY::_FC1_F_IOAB_32_47 + fc*0x20 );
			if ( m_fail_mode == MUST_FAIL )
			{
				lDataF.data[0].ioab = ~lDataF.data[0].ioab;
				lDataF.data[1].ioab = ~lDataF.data[1].ioab;
				lDataF.data[2].ioab = ~lDataF.data[2].ioab;
			}
			lData.data[0].ioab |= lDataF.data[0].ioab;
			lData.data[1].ioab |= lDataF.data[1].ioab;
			lData.data[2].ioab |= lDataF.data[2].ioab;
		}

		uint iopin    = 0x0;
		uint realscan = lData.header.header;	//Real Scan Value: low 8bit

		PRINTF( "FC%1d LATCH_DATA SCAN: %3d", fc+1, realscan );
		for( int seq=0;seq<3;seq++ )	//ioa/b 96io
		{
			int aFailDut[4] = { 0, };
			uint iogrp = 0x0;
			uint dut   = 0x0;
			uint seq_io_offset = 16;
			bool bFail = false;
			if( g_bib.GetBibIo() == 8 )
			{
				if( lData.data[seq].ioa_l )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}
					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
					bFail = true;
					aFailDut[0] = dut+1;
//					SET_STEP_FAIL( BIB0, dut, true );
//					SET_CUM_FAIL ( BIB0, dut, true );
				}
				if( lData.data[seq].ioa_h )
				{
					iopin = seq_io_offset*seq + g_bib.GetBibIo();
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOA, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}
					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
					bFail = true;
					aFailDut[1] = dut+1;
//					SET_STEP_FAIL( BIB0, dut, true );
//					SET_CUM_FAIL ( BIB0, dut, true );
				}
				if( lData.data[seq].iob_l )
				{
					iopin = seq_io_offset*seq + 0;
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}

					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}

					bFail = true;
					aFailDut[2] = dut+1;
//					SET_STEP_FAIL( BIB0, dut, true );
//					SET_CUM_FAIL ( BIB0, dut, true );
				}
				if( lData.data[seq].iob_h )
				{
					iopin = seq_io_offset*seq + g_bib.GetBibIo();
					if( !g_bib.GetIoGrp( GetCurrentCpuNo(), fc, IOB, iopin, iogrp ) )
					{
						EPRINTF("%s error : fc:%d IOA io:%d get iogrp error!", __FUNCTION__, fc, iopin );
						continue;
					}

					if( !g_bib.GetSiToIndex( realscan, iogrp, dut ) )
					{
						EPRINTF("%s error : scan:%d iogrp:%d si_to_index error!", __FUNCTION__, realscan, iogrp );
						continue;
					}
					bFail = true;
					aFailDut[3] = dut+1;
//					SET_STEP_FAIL( BIB0, dut, true );
//					SET_CUM_FAIL ( BIB0, dut, true );
				}

				if ( bFail )
				{
					std::string strData = "";
					char szData[512]={0x0, };
					memset( szData, 0x0, sizeof(szData) );
					snprintf(szData, sizeof(szData), "FC%1d LATCH_DATA[%d] = %4X %4X (D:",
							 fc+1, seq, (lData.data[seq].ioab>>16)&0xffff, lData.data[seq].ioab&0xffff );
					strData += szData;
					for( int i=0; i<4; ++i )
					{
						if ( aFailDut[i] >0 )
						{
							snprintf(szData, sizeof(szData), "%3d ", aFailDut[i] );
							strData += szData;
						}
					}
					strData += ")" ;
					PRINTF( "%s", strData.c_str() );
					PRINTF(NULL);
				}
				else
				{
					PRINTF( "FC%1d LATCH_DATA[%d] = %4X %4X", fc+1, seq, (lData.data[seq].ioab>>16)&0xffff, lData.data[seq].ioab&0xffff );
					PRINTF(NULL);
				}
			}
		}
	}
}

void CApi::DISPLAY_FCAT()
{
	if ( (m_debug_mode&0x10)==0 ) return;

	PRINTF( "[%s]", __FUNCTION__ );
	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
	{
		if ( !IS_MYDUT(dut) )
			continue;
		else
		{
			int iFailCat = 0;
			for(uint i=0; i<m_nCatePerDut; ++i )
			{
				int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + (i>>3);	//dut * m_nCatePerDut + fail_cate / 8;
				int n = i & 0x7;										//fail_cate % 8;
				if ( (m_fail_cate[m]>>n)&1 )
				{
					iFailCat = i;
					break;
				}
			}
			PRINTF( "DUT%03d = %d", dut+1, iFailCat );
		}
	}
	PRINTF(NULL);
	memset( m_fail_cate, 0, sizeof(char) * m_nMaxDutPerCpu * m_nCatePerDut    );
}

//CATEGORY LOOP
//void CApi::DISPLAY_FCAT()
//{
//	if ( (m_debug_mode&0x10)==0 ) return;
//
//	PRINTF( "[%s]", __FUNCTION__ );
//	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
//	{
//		if ( !IS_MYDUT(dut) )
//			continue;
//		else
//		{
//			int iFailCat = 0;
////			for(uint i=0; i<m_nCatePerDut; ++i )
//			for(uint i=0; i<MAX_CATEGORY ; ++i )
//			{
//				int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + (i>>3);	//dut * m_nCatePerDut + fail_cate / 8;
//				int n = i & 0x7;										//fail_cate % 8;
//				if ( (m_fail_cate[m]>>n)&1 )
//				{
//					iFailCat = i;
//					break;
//				}
//			}
//			PRINTF( "DUT%03d = %d", dut+1, iFailCat );
//		}
//	}
//	PRINTF(NULL);
//	memset( m_fail_cate, 0, sizeof(char) * m_nMaxDutPerCpu * m_nCatePerDut    );
//}

//CATEGORY/8 LOOP
//void CApi::DISPLAY_FCAT()
//{
//	if ( (m_debug_mode&0x10)==0 ) return;
//
//	PRINTF( "[%s]", __FUNCTION__ );
//	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
//	{
//		if ( !IS_MYDUT(dut) )
//			continue;
//		else
//		{
//			int iFailCat = 0;
//			bool bFind = false;
//			for(uint i=0; i<m_nCatePerDut; ++i )
//			{
////				int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + (i>>3);	//dut * m_nCatePerDut + fail_cate / 8;
//				int m = (dut%m_nMaxDutPerCpu) * m_nCatePerDut + i;		//dut * m_nCatePerDut + fail_cate 8ea;
//				if ( m_fail_cate[m] )
//				{
//					for(int n=0; n<8; ++n)
//					{
//						if ( (m_fail_cate[m]>>n)&1 )
//						{
//							iFailCat = m*8 + n;
//							bFind = true;
//							break;
//						}
//					}
//				}
//
//				if ( bFind ) break;
//			}
//			PRINTF( "DUT%03d = %d", dut+1, iFailCat );
//		}
//	}
//	PRINTF(NULL);
//	memset( m_fail_cate, 0, sizeof(char) * m_nMaxDutPerCpu * m_nCatePerDut    );
//}

void CApi::DISPLAY_EXTFM_FCNT()
{
	if ( (m_debug_mode&0x20)==0 ) return;

	PRINTF(NULL);
	PRINTF( "[%s]", __FUNCTION__ );
	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
	{
		if ( !IS_MYDUT(dut) )
			continue;
		else
		{
			PRINTF( "DUT%03d = %d", dut+1, GET_EXTFM_FCNT(dut) );
		}
	}
	PRINTF("\n");
	PRINTF(NULL);
}

void CApi::DISPLAY_BLOCK_FAIL_ADDRESS()
{
	if ( (m_debug_mode&0x40)==0 ) return;

	PRINTF(NULL);
	PRINTF( "[%s]", __FUNCTION__ );
//	for(int i=0; i<100; ++i)
//	{
//		int x = m_now_step_fail_block[i];
//		fprintf( stdout, "m_now_step_fail_block[%d] = %2X\n", i, x);
//	}
	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
	{
		if ( !IS_MYDUT(dut) )
			continue;
		else
		{
			std::string strData = "";
			char szData[512] = {0x0, };
			memset( szData, 0x0, sizeof(szData) );
			int nFcnt = STEP_BLOCK_FAIL_COUNT(BIB0, dut, 0, 0, m_nCePerDut*m_nBlockPerCe -1 );

			snprintf( szData, sizeof(szData), "DUT%03d (%8d ea): ", dut+1,  nFcnt);
			strData += szData;

			if ( nFcnt < 100 )
			{
				for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
				{
					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
					{
						unsigned int minaddr = iCe*m_nBlockPerCe + iChip    *m_nBlockPerChip;
						unsigned int maxaddr = iCe*m_nBlockPerCe + (iChip+1)*m_nBlockPerChip - 1;

//						for(int i=0; i<10; ++i )
						while( true )
						{
							int badd = STEP_BLOCK_FAIL_ADDRESS(BIB0, dut, iCe, minaddr, maxaddr);
							if ( badd<0 ) break;
							snprintf( szData, sizeof(szData), "%x ", badd );
							strData += szData;
							minaddr = badd + 1;
						}
					}
				}
			}
			PRINTF( "%s\n", strData.c_str() );
			PRINTF(NULL);
		}
	}
	PRINTF( "\n" );
	PRINTF(NULL);
}

void CApi::DISPLAY_BLOCK_FAIL_ADDRESS_CE()
{
	if ( (m_debug_mode&0x40)==0 ) return;

	PRINTF( "[%s]", __FUNCTION__ );
	PRINTF(NULL);
//	for(int i=0; i<100; ++i)
//	{
//		int x = m_now_step_fail_block[i];
//		fprintf( stdout, "m_now_step_fail_block[%d] = %2X\n", i, x);
//	}
	for( uint dut=0;dut<BIB_X()*BIB_Y();dut++ )
	{
		if ( !IS_MYDUT(dut) )
			continue;
		else
		{
			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				std::string strData = "";
				char szData[512] = {0x0, };
				memset( szData, 0x0, sizeof(szData) );
				int nFcnt = STEP_BLOCK_FAIL_COUNT(BIB0, dut, 0, iCe*m_nBlockPerCe, iCe*m_nBlockPerCe+m_nBlockPerCe -1 );
				snprintf( szData, sizeof(szData), "DUT%03d CE%1d (%8d ea): ", dut+1,  iCe+1, nFcnt);
				strData += szData;

				if ( nFcnt < 100 )
				{
					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
					{
						unsigned int minaddr = iCe*m_nBlockPerCe + iChip    *m_nBlockPerChip;
						unsigned int maxaddr = iCe*m_nBlockPerCe + (iChip+1)*m_nBlockPerChip - 1;

//						for(int i=0; i<10; ++i )
						while( true )
						{
							int badd = STEP_BLOCK_FAIL_ADDRESS(BIB0, dut, iCe, minaddr, maxaddr);
							if ( badd<0 ) break;
							snprintf( szData, sizeof(szData), "%x ", badd );
							strData += szData;
							minaddr = badd + 1;
						}
					}
				}
				PRINTF( "%s", strData.c_str() );
			}
			PRINTF( "\n" );
			PRINTF(NULL);
		}
	}
	PRINTF( "\n" );
	PRINTF(NULL);
}

void CApi::DISPLAY_BLOCK_FAIL_ADDRESS_STACK()
{
	if ( ! m_bBlockFailAddr ) return;
	if ( (m_debug_mode&0x40)==0 ) return;

	PRINTF( "[%s]", __FUNCTION__ );
	PRINTF(NULL);

	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
			if ( !IS_MYDUT(dut) )
				continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
					std::string strData = "";
					char szData[512] = {0x0, };
					memset( szData, 0x0, sizeof(szData) );

					int nFcnt = STEP_BLOCK_FAIL_COUNT(BIB0, dut, 0, iCe*m_nBlockPerCe+iChip*m_nBlockPerChip, iCe*m_nBlockPerCe+iChip*m_nBlockPerChip+m_nBlockPerChip -1 );
//					if ( iChip==0 && nFcnt>0 )
//						fprintf( stdout, "========================================================================================\n");
//					fprintf( stdout, "DUT%03d CE%1d STACK%2d (%8d ea): ", dut+1,  iCe+1, iChip+1, nFcnt);

					//2023.05.12 - fatric(modify)
					if ( nFcnt==0 && IS_STEP_FAIL_CHIP( BIB0, dut, iCe*m_nChipPerCe+iChip ) && ! m_bProcExtFmBlock )
					{
						PRINTF( "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d (%8d EA): FAIL", dut+1, r+1, c+1, iCe+1, iChip+1, 1);
					}
					else
					{
						snprintf( szData, sizeof(szData), "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d (%8d EA): ", dut+1, r+1, c+1, iCe+1, iChip+1, nFcnt);
						strData += szData;
						if ( nFcnt < 100 )
						{
							unsigned int minaddr = iCe*m_nBlockPerCe + iChip    *m_nBlockPerChip;
							unsigned int maxaddr = iCe*m_nBlockPerCe + (iChip+1)*m_nBlockPerChip - 1;

	//						for(int i=0; i<10; ++i )
							while( true )
							{
								int badd = STEP_BLOCK_FAIL_ADDRESS(BIB0, dut, iCe, minaddr, maxaddr);
								if ( badd<0 ) break;
								snprintf( szData, sizeof(szData), "%x ", badd );
								strData += szData;
								minaddr = badd + 1;
							}
						}
						PRINTF( "%s", strData.c_str() );
					}
				}
			}
			PRINTF( "" );
		}
	}
	PRINTF( NULL );
}

void CApi::DISPLAY_STEP_FAIL_CHIP()
{
	PRINTF( "[%s]", __FUNCTION__ );
//	for(int i=0; i<100; ++i)
//	{
//		int x = m_now_step_fail_block[i];
//		fprintf( stdout, "m_now_step_fail_block[%d] = %2X\n", i, x);
//	}

	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
			if ( !IS_MYDUT(dut) )
				continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
					if ( IS_STEP_FAIL_CHIP( BIB0, dut, iCe*m_nChipPerCe+iChip ) )
						PRINTF( "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d : **FAIL", dut+1, r+1, c+1, iCe+1, iChip+1 );
					else
						PRINTF( "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d : PASS***", dut+1, r+1, c+1, iCe+1, iChip+1 );
				}
			}
		}
		PRINTF(NULL);
	}
	PRINTF(NULL);
}

void CApi::DISPLAY_CUM_BLOCK_FAIL_ADDRESS_STACK()
{
	if ( ! m_bBlockFailAddr ) return;
	if ( (m_debug_mode&0x40)==0 ) return;

	PRINTF(NULL);
	PRINTF( "[%s]", __FUNCTION__ );
//	for(int i=0; i<100; ++i)
//	{
//		int x = m_now_step_fail_block[i];
//		fprintf( stdout, "m_now_step_fail_block[%d] = %2X\n", i, x);
//	}


	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
			if ( !IS_MYDUT(dut) )
				continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
					std::string strData = "\n";
					char szData[512] = {0x0, };
					memset( szData, 0x0, sizeof(szData) );

					int nFcnt = CUM_BLOCK_FAIL_COUNT(BIB0, dut, 0, iCe*m_nBlockPerCe+iChip*m_nBlockPerChip, iCe*m_nBlockPerCe+iChip*m_nBlockPerChip+m_nBlockPerChip -1 );
//					if ( iChip==0 && nFcnt>0 )
//						fprintf( stdout, "========================================================================================\n");
//					fprintf( stdout, "DUT%03d CE%1d STACK%2d (%8d ea): ", dut+1,  iCe+1, iChip+1, nFcnt);
					//2023.05.12 - fatric(modify)
					if ( nFcnt==0 && IS_CUM_FAIL_CHIP( BIB0, dut, iCe*m_nChipPerCe+iChip ) && ! m_bProcExtFmBlock )
					{
						PRINTF( "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d (%8d EA): FAIL", dut+1, r+1, c+1, iCe+1, iChip+1, 1);
					}
					else
					{
						snprintf( szData, sizeof(szData), "DUT%03d R%02d, C%02d, CE%02d, CHIP%02d (%8d EA): ", dut+1, r+1, c+1, iCe+1, iChip+1, nFcnt);
						strData += szData;
						if ( nFcnt < 100 )
						{
							unsigned int minaddr = iCe*m_nBlockPerCe + iChip    *m_nBlockPerChip;
							unsigned int maxaddr = iCe*m_nBlockPerCe + (iChip+1)*m_nBlockPerChip - 1;

	//						for(int i=0; i<10; ++i )
							while( true )
							{
								int badd = CUM_BLOCK_FAIL_ADDRESS(BIB0, dut, iCe, minaddr, maxaddr);
								if ( badd<0 ) break;
								snprintf( szData, sizeof(szData), "%x ", badd );
								strData += szData;
								minaddr = badd + 1;
							}
						}
						PRINTF( "%s", strData.c_str() );
					}
				}
			}
			PRINTF(NULL);
		}
	}
	PRINTF(NULL);
}

void CApi::DISPLAY_CHIP_INFO()
{
//	if ( ! m_bBlockFailAddr ) return;
//	if ( (m_debug_mode&0x40)==0 ) return;

	PRINTF( "[%s]", __FUNCTION__ );
//	for(int i=0; i<100; ++i)
//	{
//		int x = m_now_step_fail_block[i];
//		PRINTF( "m_now_step_fail_block[%d] = %2X\n", i, x);
//	}


	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
			if ( !IS_MYDUT(dut) )
				continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
					PRINTF( "R%02d, C%02d, CE%02d, CHIP%02d     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
								r+1, c+1, iCe+1, iChip+1,
								m_pStChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
								m_pStChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
								m_pStChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
								m_pStChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
						  );
				}
			}
		}
	}
	PRINTF(NULL);
}

//2023.04.21 - fatric
void CApi::DISPLAY_CHUNKFAIL_CNT()
{
	uint iFc1IoA[6];
	uint iFc1IoB[6];
	uint iFc2IoA[6];
	uint iFc2IoB[6];

	for(int i=0; i<6; ++i )
	{
		iFc1IoA[i] = SysRead( 0x1000200 + i);
		iFc1IoB[i] = SysRead( 0x1000218 + i);
		iFc2IoA[i] = SysRead( 0x1000230 + i);
		iFc2IoB[i] = SysRead( 0x1000248 + i);
	}

	PRINTF("[%s]", __FUNCTION__);
	for(int i=0; i<6; ++i )
		PRINTF("FC1 IOA[%d]=%d", i, iFc1IoA[i] );
	PRINTF("");

	for(int i=0; i<6; ++i )
		PRINTF("FC1 IOB[%d]=%d", i, iFc1IoB[i] );
	PRINTF("");

	for(int i=0; i<6; ++i )
		PRINTF("FC2 IOA[%d]=%d", i, iFc2IoA[i] );
	PRINTF("");

	for(int i=0; i<6; ++i )
		PRINTF("FC2 IOB[%d]=%d", i, iFc2IoB[i] );
	PRINTF("");

}

void CApi::CYCLE_BEGIN()
{
	if( !GET_RUNMODE() ) return;		//2023.03.23 - KWMOON

	PRINTF("CYCLE_BEGIN");
	SetDevStatus( PROTOCOL::DEV_STATUS::DEV_CYCLE_IN   );
	//2023.03.23 - KWMOON
	if( g_sys.GetDevice() != DIAGCAL )
	{
		CreateCycleEnd( false );
		vecStepName.clear();
		ClearTestFailData();
		ClearChipData();
		ClearStepFailData();
		ClearStepFailBlockData();
		ClearStepFailChipData();
		ClearCumFailChipData();
		ClearStepTimeStamp();
		ClearProcGetExtFmBlock();		//2023.07.12 - kwmoon( add cj hynix)
	}
	memset( m_step_fail_block_cnt, 0, sizeof(uint) * m_nMaxDutPerCpu * m_nCePerDut );	//2023.07.12 - kwmoon( add cj hynix)

	PRINTF(NULL);
}

void CApi::SetInitStep(bool bInit/*=true*/)
{
	m_bInitStep = bInit;
	if( bInit )
	{
		memset( m_init_fail , 0, sizeof(char) * m_nMaxDutPerCpu );
	}
}

bool CApi::IsInitStep()
{
	return m_bInitStep;
}

void CApi::CYCLE_END()
{
	if( !GET_RUNMODE() ) return;		//2023.03.23 - KWMOON

	// TEST
	// 20240810 hsbae
	if( g_sys.GetDevice() != DIAGCAL )
	{
		sendSortBinData();
		CreateBinmapData();
	}

	SetDevStatus( PROTOCOL::DEV_STATUS::DEV_CYCLE_OUT   );
	//2023.03.23 - KWMOON
	if( g_sys.GetDevice() != DIAGCAL )
	{
		GET_BIBID();
		CreateCommonData();
		CreateCumFailBlockData();
		CreateCumFailChipData();
		CreateTestFailData();
		CreateStepName();
		CreateCumFailCate();	//2024.06.24 - KWMOON

		CreateSortBinData();
//		CreateBinmapData();		//2024.07.22 fatric
		CreateStepData();

		GenCumFailBlock();
		GenFls();
		if ( GenTot() )
		{
			CreateCycleEnd( false, 0 );
			CreateCycleEnd( false, 1 );
		}
		else
		{
			CreateCycleEnd( true );
		}
	}

	//2023.03.27 - KWMOON
	long long mil = std::chrono::duration_cast<std::chrono::milliseconds>(m_tpCycleEnd - m_tpCycleBegin).count();
	PRINTF("CYCLE_END : %lld sec %lld msec", mil/1000, mil%1000 );

	for(uint i=0; i<m_vStepData.size(); ++i )
		delete m_vStepData[i];
	m_vStepData.clear();

//	ClearTmpFiles();
}

bool CApi::ClearCommonData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "CommonData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearInitFailData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "InitFailData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearChipData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "ChipData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearStepName()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "StepName_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearTestFailData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "TestFailData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearStepFailData()
{
//	const char* pszFile = "StepFailData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailData_%d.dat", GetCurrentCpuNo() );
//	if ( unlink(pszFile) )
//	{
//		EPRINTF("%s Data file(%s) delete error", __FUNCTION__, pszFile );
//		return false;
//	}
	unlink(szFile);

	return true;
}

bool CApi::ClearStepFailBlockData()
{
//	const char* pszFile = "StepFailBlockData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailBlockData_%d.dat", GetCurrentCpuNo() );
//	if ( unlink(pszFile) )
//	{
//		EPRINTF("%s Data file(%s) delete error", __FUNCTION__, pszFile );
//		return false;
//	}
	unlink(szFile);

	return true;
}

bool CApi::ClearStepFailChipData()
{
//	const char* pszFile = "StepFailChipData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailChipData_%d.dat", GetCurrentCpuNo() );
//	if ( unlink(pszFile) )
//	{
//		EPRINTF("%s Data file(%s) delete error", __FUNCTION__, pszFile );
//		return false;
//	}
	unlink(szFile);

	return true;
}

bool CApi::ClearCumFailChipData()
{
//	const char* pszFile = "CumFailChipData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "CumFailChipData_%d.dat", GetCurrentCpuNo() );
//	if ( unlink(pszFile) )
//	{
//		EPRINTF("%s Data file(%s) delete error", __FUNCTION__, pszFile );
//		return false;
//	}
	unlink(szFile);

	return true;
}

bool CApi::ClearCumFailBlockData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "CumFailBlockData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearStepTimeStamp()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "StepTimeStamp_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearSortBinData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "SortBinData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearStepData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "StepData_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

//2023.05.12 - fatric
bool CApi::ClearProcGetExtFmBlock()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "ProcExtFmBlock_%d.dat", GetCurrentCpuNo() );
	unlink(szFile);

	return true;
}

bool CApi::ClearTmpFiles()
{
	ClearCommonData();
	ClearInitFailData();
	ClearChipData();
	ClearStepName();
	ClearTestFailData();
	ClearStepFailData();
	ClearStepFailBlockData();
	ClearStepFailChipData();
	ClearCumFailChipData();
	ClearCumFailBlockData();
	ClearStepTimeStamp();
	ClearSortBinData();
	ClearStepData();
	ClearProcGetExtFmBlock();	//2023.05.12 - fatric

	return true;
}

bool CApi::AppendStepFailData()
{
//	const char* pszFile = "StepFailData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu;	//* sizeof(char)
	nWrited = write( fd, m_step_fail, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step(%s) fail data write error", __FUNCTION__, m_sStepName.c_str() );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.24 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_DATA, (char*)&head, sizeof(stStepDataHead), m_step_fail, sizeof(char) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_DATA).toStdString().c_str() );
		}
	}
	return true;
}

bool CApi::AppendStepFailBlockData()

{
//	const char* pszFile = "StepFailBlockData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailBlockData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	char szStepNm[PATH_MAX] = { 0, };
	sprintf(szStepNm, "%s", m_sStepName.c_str() );
	nWrited = write( fd, szStepNm, PATH_MAX );
	if ( nWrited != PATH_MAX )
	{
		EPRINTF("%s Step(%s) fail block write error", __FUNCTION__, szStepNm );
		close( fd );
		return false;
	}
//	printf("%s:: %s %d bytes writed\n", __FUNCTION__, pszFile, (int)nWrited);

	nWrite  = m_nMaxDutPerCpu * m_nBlockPerDutBl;	//* sizeof(char)
	nWrited = write( fd, m_now_step_fail_block, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step(%s) fail block write error", __FUNCTION__, szStepNm );
		close( fd );
		return false;
	}
//	printf("%s:: %s %d bytes writed\n", __FUNCTION__, pszFile, (int)nWrited);
	close( fd );

	//2023.04.24 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );
		PRINTF("%s STEP_NAME:%s[%s]", __FUNCTION__, head.szStepName, m_sStepName.c_str() );
		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_BLOCK_DATA, (char*)&head, sizeof(stStepDataHead), m_now_step_fail_block, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_BLOCK_DATA).toStdString().c_str() );
		}
	}
	return true;
}

bool CApi::getStepFailData(int nStep, int nSize, char *pStepFail, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead = read( fd, pStepFail, nSize*nStep );
	if ( nRead != nSize*nStep )
	{
//		EPRINTF("nRead=%d   nSize=%d", nRead, nSize);
		EPRINTF("%s Step fail data read error(%d step)", __FUNCTION__);
		close(fd);
		return false;
	}
	close(fd);

	return true;
}

bool CApi::getStepFailDataT(int nStep, int nSize, char *pStepFail, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	for( int i=0; i<nStep; ++i )
	{
		ssize_t nRead = read( fd, &pStepFail[i*nSize*2+iCpu*nSize], nSize );
		if ( nRead != nSize )
		{
//			EPRINTF("nRead=%d   nSize=%d", nRead, nSize);
			EPRINTF("%s Step fail data read error(%d step)", __FUNCTION__, i);
			close(fd);
			return false;
		}
	}

	close(fd);
	return true;
}

bool CApi::getStepFailBlockData(int nStep, int nSize, char *pStepBlockFail, int iCpu/*=0*/)
{
//	for( int i=0; i<nStep; ++i)
//	{
//		for(int j=0; j<nSize; ++j)
//			pStepBlockFail[i*nSize+j] = i*nSize +j;
//	}
//
//	return true;
////////////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailBlockData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailBlockData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	for( int i=0; i<nStep; ++i)
	{
		if ( lseek( fd, PATH_MAX, SEEK_CUR) < 0 )	//STEP NAME skip
		{
			EPRINTF("%s seek error(%d step)", __FUNCTION__, szFile, i);
			close(fd);
			return false;
		}

		ssize_t nRead = read( fd, &pStepBlockFail[i*nSize], nSize );
		if ( nRead != nSize )
		{
			EPRINTF("%s fail block read error(%d step)", __FUNCTION__, i);
			close(fd);
			return false;
		}
	}

	close(fd);
	return true;
}

bool CApi::getStepFailBlockDataT(int nStep, int nSize, char *pStepBlockFail, int iCpu/*=0*/)
{
//	for( int i=0; i<nStep; ++i)
//	{
//		for(int j=0; j<nSize; ++j)
//			pStepBlockFail[i*nSize+j] = i*nSize +j;
//	}
//
//	return true;
////////////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailBlockData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailBlockData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	for( int i=0; i<nStep; ++i)
	{
		if ( lseek( fd, PATH_MAX, SEEK_CUR) < 0 )	//STEP NAME skip
		{
			EPRINTF("%s seek error(%d step)", __FUNCTION__, szFile, i);
			close(fd);
			return false;
		}

		ssize_t nRead = read( fd, &pStepBlockFail[i*nSize*2+iCpu*nSize], nSize );
		if ( nRead != nSize )
		{
			EPRINTF("%s fail block read error(%d step)", __FUNCTION__, i);
			close(fd);
			return false;
		}
	}

	close(fd);
	return true;
}

bool CApi::AppendStepFailChipData()
{
//	const char* pszFile = "StepFailChipData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepFailChipData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
//	char szStepNm[PATH_MAX] = { 0, };
//	sprintf(szStepNm, "%s", m_sStepName.c_str() );
//	nWrited = write( fd, szStepNm, PATH_MAX );
//	if ( nWrited != PATH_MAX )
//	{
//		EPRINTF("%s Step(%s) name write error", __FUNCTION__, szStepNm );
//		close( fd );
//		return false;
//	}

	nWrite  = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)
	nWrited = write( fd, m_step_fail_chip, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step fail chip write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.24 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_CHIP_DATA, (char*)&head, sizeof(stStepDataHead), m_step_fail_chip, sizeof(char) * m_nMaxDutPerCpu * m_nChipPerDutBl ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_CHIP_DATA).toStdString().c_str() );
		}
	}
	return true;
}

bool CApi::getStepFailChipData(int nStep, int nSize, char *pStepFailChip, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailChipData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailChipData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

//	for( int i=0; i<nStep; ++i)
	{
//		if ( lseek( fd, PATH_MAX, SEEK_CUR) < 0 )	//STEP NAME skip
//		{
//			EPRINTF("%s seek error(%d step)", __FUNCTION__, szFile, i);
//			close(fd);
//			return false;
//		}

		ssize_t nReadSize = nSize * nStep;
		ssize_t nRead = read( fd, pStepFailChip, nReadSize );
		if ( nRead != nReadSize )
		{
			EPRINTF("%s Step fail chip read error", __FUNCTION__);
			close(fd);
			return false;
		}
	}

	close(fd);
	return true;
}

bool CApi::AppendFCateData()
{
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_FCATE_DATA, (char*)&head, sizeof(stStepDataHead), (char *)m_step_fail_cate, sizeof(uint) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_FCATE_DATA).toStdString().c_str() );
			return false;
		}
	}
	return true;
}

bool CApi::AppendPCateData()
{
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_PCATE_DATA, (char*)&head, sizeof(stStepDataHead), (char *)m_step_pass_cate, sizeof(uint) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_PCATE_DATA).toStdString().c_str() );
			return false;
		}
	}
	return true;
}

bool CApi::AppendStepFailMode()
{
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_MODE, (char*)&head, sizeof(stStepDataHead), (char *)&m_fail_mode, sizeof(int) ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_MODE).toStdString().c_str() );
			return false;
		}
	}
	return true;
}

bool CApi::AppendStepUseMode()
{
	if( g_sys.GetAutoMode() )
	{
		stStepDataHead head;
		memset( (char*)&head, 0x0, sizeof(stStepDataHead) );
		head.nStepNo = m_nCurrentStepNo;
		head.nRfu1= 0x0;
		head.nRfu2= 0x0;
		snprintf( head.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_STEP_USE_MODE, (char*)&head, sizeof(stStepDataHead), (char *)&m_use_mode, sizeof(int) ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_STEP_USE_MODE).toStdString().c_str() );
			return false;
		}
	}
	return true;
}

bool CApi::getStepFailChipDataT(int nStep, int nSize, char *pStepFailChip, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepFailChipData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepFailChipData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	for( int i=0; i<nStep; ++i)
	{
		ssize_t nRead = read( fd, &pStepFailChip[i*nSize*2 + iCpu*nSize], nSize );
		if ( nRead != nSize )
		{
			EPRINTF("%s Step fail chip read error", __FUNCTION__);
			close(fd);
			return false;
		}
	}

	close(fd);
	return true;
}

bool CApi::AppendStepTimeStamp()
{
//	const char* pszFile = "StepTimeStamp.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepTimeStamp_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite = sizeof(time_t);
	nWrited = write( fd, &m_tStepBeginTime, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step begin time write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	nWrited = write( fd, &m_tStepEndTime, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step end time write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	close( fd );
	return true;
}

bool CApi::getStepTimeStampData(int nStep, std::vector<time_t> &vBegin, std::vector<time_t> &vEnd, int iCpu/*=0*/ )
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepTimeStamp_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepTimeStamp_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	for( int i=0; i<nStep; ++i)
	{
		time_t tBegin, tEnd;

		ssize_t nReadSize, nRead;
		nReadSize = sizeof(time_t);
		nRead     = read( fd, &tBegin, nReadSize );
		if ( nRead != nReadSize )
		{
			EPRINTF("%s Step begin time read error(%d step)", __FUNCTION__, i);
			close(fd);
			return false;
		}
		vBegin.push_back( tBegin );

		nRead     = read( fd, &tEnd, nReadSize );
		if ( nRead != nReadSize )
		{
			EPRINTF("%s Step end time read error(%d step)", __FUNCTION__, i);
			close(fd);
			return false;
		}
		vEnd.push_back( tEnd );
	}

	close(fd);
	return true;
}

bool CApi::CreateInitFailData()
{
//	const char* pszFile = "InitFailData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "InitFailData_%d.dat", GetCurrentCpuNo() );

	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu;	//* sizeof(char)
	nWrited = write( fd, m_init_fail, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Init fail data write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	close( fd );

	//2023.04.24 - KWMOON( moved from initTestEnd to here )
	if( g_sys.GetAutoMode() )
	{
		stInitTestHead inithead;
		memset( (char*)&inithead, 0x0, sizeof(stInitTestHead) );
		inithead.nStepNo    = m_nCurrentStepNo;
		inithead.tInitBegin = m_tStepBeginTime;
		inithead.tInitEnd   = m_tStepEndTime;
		snprintf( inithead.szStepName, MAX_TEXT_VALUE, "%s", m_sStepName.c_str() );

		if( !g_devsock.WriteTestData( PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_DATA, (char*)&inithead, sizeof(stInitTestHead), m_init_fail, sizeof(char) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_DATA).toStdString().c_str() );
		}
	}

	return true;
}

bool CApi::getInitFailData(char *pInitFail, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "InitFailData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "InitFailData_%d.dat", iCpu );

	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nReadSize = m_nMaxDutPerCpu;	//* sizeof(char)
	ssize_t nRead = read( fd, pInitFail, nReadSize );
	if ( nRead != nReadSize )
	{
		EPRINTF("%s Init fail data read error", __FUNCTION__);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool CApi::CreateTestFailData()
{
//	const char* pszFile = "TestFailData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "TestFailData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu;	//* sizeof(char)
	nWrited = write( fd, m_test_fail, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Test fail data write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.26 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_DATA, m_test_fail, sizeof(char) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_DATA).toStdString().c_str() );
		}
	}
	return true;
}

bool CApi::getTestFailData(char *pTestFail, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "TestFailData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "TestFailData_%d.dat", iCpu );

	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nReadSize = m_nMaxDutPerCpu;	//* sizeof(char)
	ssize_t nRead = read( fd, pTestFail, nReadSize );
	if ( nRead != nReadSize )
	{
		EPRINTF("%s Test fail data read error", __FUNCTION__);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool CApi::CreateCumFailChipData()
{
//	const char* pszFile = "CumFailChipData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "CumFailChipData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)
	nWrited = write( fd, m_cum_fail_chip, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Cum fail chip write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.26 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CHIP_DATA, m_cum_fail_chip, sizeof(char) * m_nMaxDutPerCpu * m_nChipPerDutBl ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CHIP_DATA).toStdString().c_str() );
		}
	}

	return true;
}

bool CApi::getCumFailChipData(int nSize, char *pCumFailChip, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "CumFailChipData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "CumFailChipData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead = read( fd, pCumFailChip, nSize );
	if ( nRead != nSize )
	{
		EPRINTF("%s Cum fail chip read error", __FUNCTION__);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool CApi::CreateCumFailBlockData()
{
//	const char* pszFile = "CumFailBlockData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "CumFailBlockData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu * m_nBlockPerDutBl;	//* sizeof(char)
	nWrited = write( fd, m_cum_fail_block, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Cum fail block data write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.26 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_BLOCK_DATA, m_cum_fail_block, sizeof(char) * m_nMaxDutPerCpu * m_nBlockPerDutBl ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_BLOCK_DATA).toStdString().c_str() );
		}
	}

	return true;
}

bool CApi::getCumFailBlockData(int nSize, char *pCumBlockFail, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "CumFailBlockData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "CumFailBlockData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead;
	nRead = read( fd, pCumBlockFail, nSize );
	if ( nRead != nSize )
	{
		EPRINTF("%s Cum fail block data write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	close( fd );
	return true;
}

bool CApi::CreateChipData()
{
//	const char* pszFile = "ChipData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "ChipData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

//	for ( uint r = 0;r<BIB_X();r++ )
//	{
//		for ( uint c = 0;c<BIB_Y();c++ )
//		{
//			int dut = c*BIB_X() + r;
//			if ( !IS_MYDUT(dut) )
//				continue;
//
//			for(uint iCe=0; iCe<m_addMaxCe; ++iCe)
//			{
//				for(uint iChip=0; iChip<m_addMaxChip; ++iChip)
//				{
//					sprintf(m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].szchipId, "%04d", r+1);
//					m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uWaferId = c +1;
//					m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uXPos    = iCe + 1;
//					m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uYPos	   = iChip+1;
//
//					printf( "CreateChipData R%s, C%02d, CE%02d, CHIP%02d\n",
//							m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].szchipId,
//							m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uWaferId,
//							m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uXPos   ,
//							m_pStChipId[dut*m_nChipPerDut + iCe*m_addMaxChip + iChip].uYPos
//							);
//				}
//			}
//		}
//	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu * m_nChipPerDut * sizeof(stChipId);	//m_stChipData.chip_count == m_nCePerDut * m_nChipPerCe
	nWrited = write( fd, m_pStChipId, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s-%s Chip data write error", __FUNCTION__, GET_STEP_NAME() );
		close( fd );
		return false;
	}
//	printf("%s:: %s %d bytes writed\n", __FUNCTION__, pszFile, (int)nWrited);
	close( fd );

	//2023.04.18 - KWMOON( Send ChipId info/data send )
	if( g_sys.GetAutoMode() )
	{
		g_devsock.WriteElement( PROTOCOL::CMD_CHIPID_INFO, (char*)&m_stChipData, sizeof(stChipData) );

		int nSize  = m_nMaxDutPerCpu * m_nChipPerDut * sizeof(stChipId);
		g_devsock.WriteElement( PROTOCOL::CMD_CHIPID_DATA, (char*)m_pStChipId, nSize );
	}

	return true;
}

bool CApi::getChipData(stChipId *pstChipId, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "ChipData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "ChipData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY);
	if ( fd == -1 )
	{
//		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead, nReadSize;
	nReadSize = m_nMaxDutPerCpu * m_nChipPerDut * sizeof(stChipId);	//nSize == m_stChipData.chip_count == m_nCePerDut * m_nChipPerCe
	nRead     = read( fd, pstChipId, nReadSize );
	if ( nRead != nReadSize )
	{
		EPRINTF("%s Chip data read error", __FUNCTION__ );
		close( fd );
		return false;
	}

	close( fd );
	return true;
}

bool CApi::CreateCommonData()
{
//	const char* pszFile = "CommonData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "CommonData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = sizeof(stCommonData);
	nWrited = write( fd, &m_stCommonData, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Common data write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	close( fd );
	return true;
}

bool CApi::getCommonData(stCommonData &_stCommonData, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "CommonData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "CommonData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead;
	nRead = read( fd, &_stCommonData, sizeof(stCommonData) );
	if ( nRead != sizeof(stCommonData) )
	{
		EPRINTF("%s Common data read error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	return true;
}

bool CApi::CreateStepName()
{
//	const char* pszFile = "StepName.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "StepName_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	int     nSize = vecStepName.size();
	ssize_t nWrite, nWrited;
	nWrite  = sizeof(int);
	nWrited = write( fd, &nSize, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step name size write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	char szStepNm[PATH_MAX];
	nWrite  = sizeof(szStepNm);	//* sizeof(char)
	for(int i=0; i<nSize; ++i )
	{
		sprintf(szStepNm, "%s", vecStepName[i].c_str() );
		nWrited = write( fd, &szStepNm, nWrite );
		if ( nWrited != nWrite )
		{
			EPRINTF("%s Step name(%s) write error", __FUNCTION__, szStepNm);
			close( fd );
			return false;
		}
	}
	close( fd );

	return true;
}

bool CApi::getStepName(std::vector<std::string> &vStepName, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepName_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepName_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	int     nVecSize;
	ssize_t nRead;
	nRead = read( fd, &nVecSize, sizeof(int) );
	if ( nRead != sizeof(int) )
	{
		EPRINTF("%s Step name size read error", __FUNCTION__ );
		close( fd );
		return false;
	}

	char szStepNm[PATH_MAX];
	for(int i=0; i<nVecSize; ++i )
	{
		nRead = read( fd, &szStepNm, sizeof(szStepNm) );
		if ( nRead != sizeof(szStepNm) )
		{
			EPRINTF("%s Step name(%s) write error", __FUNCTION__, szStepNm);
			close( fd );
			return false;
		}
		vStepName.push_back( szStepNm );
	}
	close( fd );

	return true;
}


//2024.06.24 - KWMOON
bool CApi::CreateCumFailCate()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "CumFailCate%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu * m_nCatePerDut * sizeof(char);
	nWrited = write( fd, m_fail_cate, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Init fail data write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	//2023.04.26 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CATEGORY, (char*)m_fail_cate, nWrite ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CATEGORY).toStdString().c_str() );
		}
	}

	return true;
}

void CApi::sendSortBinData()
{
	//2023.04.26 - KWMOON
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_SORTBIN_DATA, (char*)m_bin, sizeof(uint) * m_nMaxDutPerCpu ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_SORTBIN_DATA).toStdString().c_str() );
		}
	}
}

bool CApi::CreateSortBinData()
{
//	const char* pszFile = "SortBinData.dat";
	char szFile[PATH_MAX];
	sprintf(szFile, "SortBinData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = m_nMaxDutPerCpu * sizeof(uint);
	nWrited = write( fd, m_bin, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Init fail data write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

//	//2023.04.26 - KWMOON
//	if( g_sys.GetAutoMode() )
//	{
//		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_CUM_SORTBIN_DATA, (char*)m_bin, sizeof(uint) * m_nMaxDutPerCpu ) )
//		{
//			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_CUM_SORTBIN_DATA).toStdString().c_str() );
//		}
//	}

	return true;
}

bool CApi::CreateBinmapData()
{
	if( g_sys.GetAutoMode() )
	{
		if( !g_devsock.WriteElement( PROTOCOL::TCP_COMMAND::CMD_BINMAP_DATA, (char*)m_bin_map, sizeof(uint) * 16 ) )
		{
			EPRINT( "%s send error!", UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_BINMAP_DATA).toStdString().c_str() );
		}
	}

	return true;
}

bool CApi::getSortBinData(uint *pSortBin, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "SortBinData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "SortBinData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nReadSize = m_nMaxDutPerCpu * sizeof(uint);
	ssize_t nRead = read( fd, pSortBin, nReadSize );
	if ( nRead != nReadSize )
	{
		EPRINTF("%s Sort bin data read error", __FUNCTION__);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool CApi::CreateStepData()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "StepData_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	int     nSize = m_vStepData.size();
	ssize_t nWrite, nWrited;
	nWrite  = sizeof(int);
	nWrited = write( fd, &nSize, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Step data write error", __FUNCTION__ );
		close( fd );
		return false;
	}

	nWrite  = sizeof(CStepData);	//* sizeof(char)
	for(int i=0; i<nSize; ++i )
	{
		CStepData* pStepData = m_vStepData[i];
		nWrited = write( fd, pStepData, nWrite );
		if ( nWrited != nWrite )
		{
			EPRINTF("%s Step data write error", __FUNCTION__);
			close( fd );
			return false;
		}
	}
	close( fd );

	return true;
}

bool CApi::getStepData(std::vector<CStepData *> &vStepData, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "StepData_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "StepData_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	int     nVecSize;
	ssize_t nRead;
	nRead = read( fd, &nVecSize, sizeof(int) );
	if ( nRead != sizeof(int) )
	{
		EPRINTF("%s Step data size read error", __FUNCTION__ );
		close( fd );
		return false;
	}

	for(int i=0; i<nVecSize; ++i )
	{
		CStepData *pStepData = new (std::nothrow)CStepData;
		if ( ! pStepData )
		{
			EPRINTF("%s Step data memory alloc error", __FUNCTION__);
			close( fd );
			return false;
		}

		nRead = read( fd, pStepData, sizeof(CStepData) );
		if ( nRead != sizeof(CStepData) )
		{
			EPRINTF("%s Step data read error", __FUNCTION__);
			close( fd );
			return false;
		}
		vStepData.push_back( pStepData );
	}
	close( fd );

	return true;
}

bool CApi::CreateCycleEnd(bool bEnd, int iCpu/*=-1*/)
{
	char szFile[PATH_MAX];
	if ( iCpu < 0 ) iCpu = GetCurrentCpuNo();
	sprintf(szFile, "CycleEnd_%d.dat", iCpu );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = sizeof(bool);
	nWrited = write( fd, &bEnd, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s Cycle end write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	return true;
}

bool CApi::getCycleEnd(bool &bEnd, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
//	sprintf(szFile, "CycleEnd_%d.dat", GetCurrentCpuNo() );
	sprintf(szFile, "CycleEnd_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
//		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead;
	nRead = read( fd, &bEnd, sizeof(bool) );
	if ( nRead != sizeof(bool) )
	{
//		EPRINTF("%s Cycle end data read error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	return true;
}

//2023.05.12 - fatric
bool CApi::AppendProcExtFmBlock()
{
	char szFile[PATH_MAX];
	sprintf(szFile, "ProcExtFmBlock_%d.dat", GetCurrentCpuNo() );
	int fd = open(szFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nWrite, nWrited;
	nWrite  = sizeof(bool);
	nWrited = write( fd, &m_bProcExtFmBlock, nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s ProcExtFmBlock write error", __FUNCTION__ );
		close( fd );
		return false;
	}
	close( fd );

	return true;
}

//2023.05.12 - fatric
bool CApi::getProcExtFmBlock(int nSize, bool *pProcExtFmBlock, int iCpu/*=0*/)
{
	char szFile[PATH_MAX];
	sprintf(szFile, "ProcExtFmBlock_%d.dat", iCpu );
	int fd = open(szFile, O_RDONLY );
	if ( fd == -1 )
	{
		EPRINTF("%s Data file(%s) open error", __FUNCTION__, szFile );
		return false;
	}

	ssize_t nRead = read( fd, pProcExtFmBlock, nSize );
	if ( nRead != nSize )
	{
		EPRINTF("%s ProcExtFmBlock read error", __FUNCTION__);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool CApi::IsChipFail(char *pFailChip, int iDut, int iCe, int iChip, int iStep/*=0*/)
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)

	int chip_index = iCe * m_nChipPerCe + iChip;
	int m = nFailChipSizePerStep*iStep + (iDut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;											//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

bool CApi::IsChipFailT(char *pFailChip, int iDut, int iCe, int iChip, int iStep/*=0*/)
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl * 2;	//* sizeof(char)

	int chip_index = iCe * m_nChipPerCe + iChip;
	int m = nFailChipSizePerStep*iStep + iDut * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;														//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

bool CApi::IsChipFail(char *pFailChip, int iDut, int iStep/*=0*/)
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)

	bool bFail = false;
	for(uint iChip=0; iChip<m_nCePerDut*m_nChipPerCe; iChip+=8)
	{
		int chip_index = iChip;
		int m = nFailChipSizePerStep*iStep + (iDut%m_nMaxDutPerCpu) * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
//		int n = chip_index & 0x7;											//chip_index % 8;
//		bool bChilFail = (pFailChip[m] >> n) & 1;
		if ( pFailChip[m] )
		{
			bFail = true;
			break;
		}
	}

	return bFail;
}

bool CApi::IsChipFailT(char *pFailChip, int iDut, int iStep/*=0*/)
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl * 2;	//* sizeof(char)

	bool bFail = false;
	for(uint iChip=0; iChip<m_nCePerDut*m_nChipPerCe; iChip+=8)
	{
		int chip_index = iChip;
		int m = nFailChipSizePerStep*iStep + iDut * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
//		int n = chip_index & 0x7;											//chip_index % 8;
//		bool bChilFail = (pFailChip[m] >> n) & 1;
		if ( pFailChip[m] )
		{
			bFail = true;
			break;
		}
	}

	return bFail;
}

bool CApi::IsPreFail(char *pPreFail, int iDut)
{
	if ( pPreFail==nullptr ) return false;

	return static_cast<bool>( pPreFail[iDut%m_nMaxDutPerCpu] );
}

bool CApi::IsPreFailT(char *pPreFail, int iDut)
{
	if ( pPreFail==nullptr ) return false;

	return static_cast<bool>( pPreFail[iDut] );
}

bool CApi::IsTestFail(char *pTestFail, int iDut)
{
	if ( pTestFail==nullptr ) return false;

	return static_cast<bool>( pTestFail[iDut%m_nMaxDutPerCpu] );
}

bool CApi::IsTestFailT(char *pTestFail, int iDut)
{
	if ( pTestFail==nullptr ) return false;

	return static_cast<bool>( pTestFail[iDut] );
}

bool CApi::IsStepFail(char *pStepFail, int iDut, int iStep)
{
	if ( pStepFail==nullptr ) return false;

	ssize_t nStepFailSizePerStep = m_nMaxDutPerCpu;	//* sizeof(char)

	return static_cast<bool>( pStepFail[nStepFailSizePerStep*iStep + (iDut%m_nMaxDutPerCpu)] );
}

bool CApi::IsStepFailT(char *pStepFail, int iDut, int iStep)
{
	if ( pStepFail==nullptr ) return false;

	ssize_t nStepFailSizePerStep = m_nMaxDutPerCpu*2;	//* sizeof(char)

	return static_cast<bool>( pStepFail[nStepFailSizePerStep*iStep + iDut] );
}

bool CApi::IsAllStepPass(char *pStepFail, int iDut, int nStep)
{
	if ( pStepFail==nullptr ) return false;

	int nStepFailSizePerStep = m_nMaxDutPerCpu;	//* sizeof(char)
	int dut = iDut%m_nMaxDutPerCpu;
	for(int iStep=0; iStep<nStep; ++iStep)
	{
		if ( pStepFail[nStepFailSizePerStep*iStep + dut] )
			return false;
	}

	return true;
}

bool CApi::IsAllStepPassT(char *pStepFail, int iDut, int nStep)
{
	if ( pStepFail==nullptr ) return false;

	int nStepFailSizePerStep = m_nMaxDutPerCpu*2;	//* sizeof(char)
	int dut = iDut;
	for(int iStep=0; iStep<nStep; ++iStep)
	{
		if ( pStepFail[nStepFailSizePerStep*iStep + dut] )
			return false;
	}

	return true;
}

uint CApi::StepFailCnt(char *pStepFail, char *pPreFail, int iStep)
{
	if ( pStepFail==nullptr ) return 0;
	if ( pPreFail ==nullptr ) return 0;

	ssize_t nStepFailSizePerStep = m_nMaxDutPerCpu;	//* sizeof(char)
	uint    nOffset              = nStepFailSizePerStep*iStep;

	uint iFailCnt = 0;
	for(uint i=0; i<m_nMaxDutPerCpu; ++i)
	{
		if ( pPreFail [          i] ) continue;
		if ( pStepFail[nOffset + i] ) ++iFailCnt;
	}

	return iFailCnt;
}

uint CApi::StepFailCntT(char *pStepFail, char *pPreFail, int iStep)
{
	if ( pStepFail==nullptr ) return 0;
	if ( pPreFail ==nullptr ) return 0;

	ssize_t nStepFailSizePerStep = m_nMaxDutPerCpu*2;	//* sizeof(char)
	uint    nOffset              = nStepFailSizePerStep*iStep;

	uint iFailCnt = 0;
	for(uint i=0; i<m_nMaxDutPerCpu*2; ++i)
	{
		if ( pPreFail [          i] ) continue;
		if ( pStepFail[nOffset + i] ) ++iFailCnt;
	}

	return iFailCnt;
}

void CApi::getFailBlockInfo(std::vector<uint> &vBlkAddr, char *pFailBlock, int iDut, int iCe, int iChip, int iStep/*=0*/)
{
	ssize_t nFailBlockSizePerStep = m_nMaxDutPerCpu * m_nBlockPerDutBl;	//* sizeof(char)

	uint iOffset = nFailBlockSizePerStep*iStep + (iDut%m_nMaxDutPerCpu)*m_nBlockPerDutBl;
	uint minaddr = iCe * m_nBlockPerCe + iChip * m_nBlockPerChip;
	uint maxaddr = iCe * m_nBlockPerCe + iChip * m_nBlockPerChip + m_nBlockPerChip - 1;
	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
	{
		int m = iOffset + (iBlock>>3);
		if ( ! pFailBlock[m] )
		{
//			printf("Bf iBlock=%x     7-(iBlock%%8)=%d   ", iBlock, 7-(iBlock%8) );
			iBlock += 7 - (iBlock%8);
//			printf("Af iBlock=%x\n", iBlock );
			continue;
		}

		int n = iBlock&0x7;
		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
		if ( bFail )
		{
//			printf("fail=%x\n", iBlock );
			vBlkAddr.push_back( iBlock );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
}

void CApi::getFailBlockInfoT(std::vector<uint> &vBlkAddr, char *pFailBlock, int iDut, int iCe, int iChip, int iStep/*=0*/)
{
	ssize_t nFailBlockSizePerStep = m_nMaxDutPerCpu * m_nBlockPerDutBl * 2;	//* sizeof(char)

	uint iOffset = nFailBlockSizePerStep*iStep + iDut*m_nBlockPerDutBl;
	uint minaddr = iCe * m_nBlockPerCe + iChip * m_nBlockPerChip;
	uint maxaddr = iCe * m_nBlockPerCe + iChip * m_nBlockPerChip + m_nBlockPerChip - 1;
	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
	{
		int m = iOffset + (iBlock>>3);
		if ( ! pFailBlock[m] )
		{
//			printf("Bf iBlock=%x     7-(iBlock%%8)=%d   ", iBlock, 7-(iBlock%8) );
			iBlock += 7 - (iBlock%8);
//			printf("Af iBlock=%x\n", iBlock );
			continue;
		}

		int n = iBlock&0x7;
		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
		if ( bFail )
		{
//			printf("fail=%x\n", iBlock );
			vBlkAddr.push_back( iBlock );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
}

bool CApi::IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr)
{
	for(int i=0; i<(int)vCumBlkAddr.size(); ++i)
	{
		if ( vCumBlkAddr[i]==iBlkAddr ) return true;
	}

	return false;
}

void CApi::mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr)
{
	std::vector<uint> vNewBlkAddr;
	for(int i=0; i<(int)vBlkAddr.size(); ++i)
	{
		if ( ! IsIn(vCumBlkAddr, vBlkAddr[i]) )
			vNewBlkAddr.push_back( vBlkAddr[i] );
	}

	for(int i=0; i<(int)vNewBlkAddr.size(); ++i)
		vCumBlkAddr.push_back( vNewBlkAddr[i] );
}

bool CApi::GenCumFailBlock()
{
//	m_pStChipId = new stChipId[ m_nMaxDutPerCpu * m_nChipPerDut ];
//	CreateChipData();
//	delete [] m_pStChipId; m_pStChipId = NULL;
	int iCpu = GetCurrentCpuNo();
////////////////////////////////////////////////////
	std::vector<std::string> vStepName;
	if ( ! getStepName(vStepName, iCpu) )
	{
		EPRINTF("%s Step name read error", __FUNCTION__ );
		return false;
	}
	int nStep = vStepName.size();
//	for(size_t i=0; i<vStepName.size(); ++i )
//		printf("[%lu]{%s}\n", i, vStepName[i].c_str() );
////////////////////////////////////////////////////
	stCommonData tCommonData;
	if ( ! getCommonData(tCommonData, iCpu) )
	{
		EPRINTF("%s Common data read error", __FUNCTION__ );
		return false;
	}
//	PRINTF("Slot=%d   BibId=%03X%05d", tCommonData.iSlot, (tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff);
////////////////////////////////////////////////////
	bool bChipId = true;
	int nChipDataSize = m_nMaxDutPerCpu * m_nChipPerDut;
	stChipId *pstChipId = new (std::nothrow)stChipId[ nChipDataSize ];
	if ( pstChipId == nullptr )
	{
		EPRINTF("%s Memory(Chip id) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getChipData(pstChipId, iCpu) )
	{
//		EPRINTF("%s chip data read error", __FUNCTION__ );
		if ( pstChipId != nullptr ) { delete [] pstChipId; pstChipId = nullptr; }
		bChipId = false;
//		return false;
	}
////////////////////////////////////////////////////
	ssize_t nFailBlockSizePerDut = m_nMaxDutPerCpu * m_nBlockPerDutBl;	//* sizeof(char)
	char *pStepBlockFail = new (std::nothrow)char[nStep * nFailBlockSizePerDut];
	if ( pStepBlockFail == nullptr )
	{
		EPRINTF("%s Memory(step fail block data) alloc error", __FUNCTION__ );
		if ( pstChipId != nullptr ) { delete [] pstChipId; pstChipId = nullptr; }
		return false;
	}
	if ( ! getStepFailBlockData(nStep, nFailBlockSizePerDut, pStepBlockFail, iCpu) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		return false;
	}
////////////////////////////////////////////////////
	char *pCumBlockFail = new (std::nothrow)char[nFailBlockSizePerDut];
	if ( pCumBlockFail == nullptr )
	{
		EPRINTF("%s Memory(cum fail block data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		return false;
	}
	if ( ! getCumFailBlockData(nFailBlockSizePerDut, pCumBlockFail, iCpu) )
	{
		EPRINTF("%s cum fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)
	char *pStepFailChip = new (std::nothrow)char[nStep * nFailChipSizePerStep];
	if ( pStepFailChip == nullptr )
	{
		EPRINTF("%s Memory(step fail chip data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		return false;
	}
	if ( ! getStepFailChipData(nStep, nFailChipSizePerStep, pStepFailChip, iCpu) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	char *pCumFailChip = new (std::nothrow)char[nFailChipSizePerStep];
	if ( pCumFailChip == nullptr )
	{
		EPRINTF("%s Memory(step fail chip data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		return false;
	}
	if ( ! getCumFailChipData(nFailChipSizePerStep, pCumFailChip, iCpu) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		return false;
	}

////////////////////////////////////////////////////////////
	int nInitFailSize = m_nMaxDutPerCpu;
	char *pInitFail = new (std::nothrow)char[ nInitFailSize ];
	if ( pInitFail == nullptr )
	{
		EPRINTF("%s Memory(Init Fail) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		return false;
	}
	if ( ! getInitFailData(pInitFail, iCpu) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	int nTestFailSize = m_nMaxDutPerCpu;
	char *pTestFail = new (std::nothrow)char[ nTestFailSize ];
	if ( pTestFail == nullptr )
	{
		EPRINTF("%s Memory(Test Fail) alloc error", __FUNCTION__ );
		//2023.05.12 -  fatric
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		return false;
	}
	if ( ! getTestFailData(pTestFail, iCpu) )
	{
		EPRINTF("%s Test Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	std::vector<time_t> vBegin;
	std::vector<time_t> vEnd;
	if ( ! getStepTimeStampData(nStep, vBegin, vEnd, iCpu) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
//2023.05.12 - fatric
////////////////////////////////////////////////////////////
	bool *pProcExtFmBlock = new (std::nothrow)bool[ nStep ];
	if ( pProcExtFmBlock == nullptr )
	{
		EPRINTF("%s Memory(ProcExtFmBlock Fail) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
	if ( ! getProcExtFmBlock(nStep, pProcExtFmBlock, iCpu) )
	{
		EPRINTF("%s Test Fail data read error", __FUNCTION__ );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
	sprintf(szFile, "Sample_%d.cumFailBlock", iCpu);
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }
		return false;
	}

	string strCr = "\n";
	string strLine;
	string strData;
	char   szStepNm[33];
	char   szLine[1024];
	struct tm* t;
////////////////////////////////////////////////////////////
//	Head
	strData.clear();
	strLine = "REV            :  "; strData += strLine + strCr;
	strLine = "PART NAME      :  "; strData += strLine + strCr;
	strLine = "LOT NO         :  "; strData += strLine + strCr;
	strLine = "PGM NAME       :  "; strData += strLine + strCr;
	strLine = "FW_REV         :  "; strData += strLine + strCr;
	strLine = "CAT_REV        :  "; strData += strLine + strCr;
	strLine = "REMARK         :  "; strData += strLine + tCommonData.szRemark + strCr;
	strLine = "C_Vendor       :  "; strData += strLine + strCr;
	strLine = "SYSTEM_NAME    :  "; strData += strLine + strCr;
	strLine = "START TIME     :  "; strData += strLine;
	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "END TIME       :  "; strData += strLine;
	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "OPER           :  "; strData += strLine + strCr;
	strLine = "DEVICE         :  "; strData += strLine + strCr;
	strLine = "FACTORY        :  "; strData += strLine + strCr;
	strLine = "Fab            :  "; strData += strLine + strCr;
	strLine = "Owner          :  "; strData += strLine + strCr;
	strLine = "Grade          :  "; strData += strLine + strCr;

	ssize_t nWrite, nWrited;
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }	//2023.05.12 - fatric
		return false;
	}

////////////////////////////////////////////////////////////
//	if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
//	if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
//	if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
//	if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
//	if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
//	if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
//	if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
//	if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }	//2023.05.12 - fatric
//	close(fd);
//
//	return false;
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
			if ( !IS_MYDUT(dut)            ) continue;
			if ( IsPreFail(pInitFail, dut) ) continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
//					printf( "Slot%02d.(%03X%05d)R%02d, C%02d, CE%02d, CHIP%02d     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL\n",
//								tCommonData.iSlot,
//								(tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff,
//								r+1, c+1, iCe+1, iChip+1,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
//						  );

					strData.clear();
					sprintf( szLine, "Slot%02d.(%03X%05d)R%02d, C%02d, CE%02d, CHIP%02d",
								tCommonData.iSlot,
								(tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff,
								r+1, c+1, iCe+1, iChip+1
						   );
					strData += szLine;

					if ( bChipId )
					{
						sprintf( szLine, "     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
									pstChipId[(dut%m_nMaxDutPerCpu)*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
									pstChipId[(dut%m_nMaxDutPerCpu)*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
									pstChipId[(dut%m_nMaxDutPerCpu)*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
									pstChipId[(dut%m_nMaxDutPerCpu)*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
							   );
						strData += szLine;
					}
					strData += strCr;


//					sprintf( szLine, "Cum(%s/%s)                     ", (IsTestFail(pTestFail   , dut)?"FAIL":"PASS"), (IsChipFail(pCumFailChip, dut, iCe, iChip)?"FAIL":"PASS") );
					sprintf( szLine, "Cum(%s/%s)                     ", (IsChipFail(pCumFailChip, dut)?"FAIL":"PASS"), (IsChipFail(pCumFailChip, dut, iCe, iChip)?"FAIL":"PASS") );
					strData += szLine;
					std::vector<uint> vBlkAddr;
					getFailBlockInfo(vBlkAddr, pCumBlockFail, dut, iCe, iChip);
					sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
					strData += szLine;
					int nFailBlk = (int)vBlkAddr.size();
					if ( nFailBlk > 254 ) nFailBlk = 254;
					for(int i=0; i<nFailBlk; ++i)
					{
						sprintf( szLine, " %x ", vBlkAddr[i] );
						strData += szLine;
					}
					strData += strCr;

					//step loop
					std::vector<uint> vCumBlkAddr;
					vCumBlkAddr.clear();
					for(int iStep=0; iStep<nStep; ++iStep)
					{
						sprintf( szStepNm, "%02d.%s%s", iStep+1, vStepName[iStep].c_str(), "(N)" );
						sprintf( szLine, "%-35s", szStepNm );
						strData += szLine;

						vBlkAddr.clear();
						getFailBlockInfo(vBlkAddr, pStepBlockFail, dut, iCe, iChip, iStep);

						//2023.05.12 - fatric
						if ( vBlkAddr.size()<1 && IsChipFail(pStepFailChip, dut, iCe, iChip, iStep) && ! pProcExtFmBlock[iStep] )
						{
							sprintf( szLine, "%dEA FAIL ", 1 );
							strData += szLine + strCr;
						}
						else
						{
							sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
							strData += szLine;
							nFailBlk = (int)vBlkAddr.size();
							if ( nFailBlk > 253 ) nFailBlk = 253;
							for(int i=0; i<nFailBlk; ++i)
							{
								sprintf( szLine, " %x%c", vBlkAddr[i], !IsIn(vCumBlkAddr, vBlkAddr[i])?'!':' ' );
								strData += szLine;
							}
							if ( vBlkAddr.size() > 253 ) strData += " ...";
							strData += strCr;
						}
						mergeFailBlockInfo(vCumBlkAddr, vBlkAddr);
					}

					nWrite = strData.size();
					nWrited = write( fd, strData.c_str(), nWrite );
					if ( nWrited != nWrite )
					{
						EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
						close( fd );
						return false;
					}

				}//End of iChip
			}//End of iCe
		}//End of c
	}//End of r

	char szBegin[32];
	char szEnd  [32];
	char szTmp  [256];
	strData.clear();
	for(int iStep=0; iStep<nStep; ++iStep)
	{
		t = localtime( &vBegin[iStep] );
		sprintf( szBegin, "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
		t = localtime( &vEnd  [iStep] );
		sprintf( szEnd  , "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );

//		sprintf( szLine, "%02d.%-32s:%s - %s", iStep+1, vStepName[iStep].c_str(), szBegin, szEnd );

		sprintf( szTmp, "%02d.%-s"      , iStep+1, vStepName[iStep].c_str() );
		sprintf( szLine, "%-35s:%s - %s", szTmp, szBegin, szEnd );
		strData += szLine + strCr;
	}//End of iStep
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		return false;
	}
	close(fd);

////////////////////////////////////////////////////////////
	if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
	if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
	if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
	if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
	if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
	//2023.05.12 - fatric(add)
	if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
	if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
	if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }

	return true;
}

bool CApi::GenFls()
{
//	m_pStChipId = new stChipId[ m_nMaxDutPerCpu * m_nChipPerDut ];
//	CreateChipData();
//	delete [] m_pStChipId; m_pStChipId = NULL;
	int iCpu = GetCurrentCpuNo();
////////////////////////////////////////////////////
	std::vector<std::string> vStepName;
	if ( ! getStepName(vStepName, iCpu) )
	{
		EPRINTF("%s Step name read error", __FUNCTION__ );
		return false;
	}
	int nStep = vStepName.size();
//	for(size_t i=0; i<vStepName.size(); ++i )
//		printf("[%lu]{%s}\n", i, vStepName[i].c_str() );
////////////////////////////////////////////////////
	stCommonData tCommonData;
	if ( ! getCommonData(tCommonData, iCpu) )
	{
		EPRINTF("%s Common data read error", __FUNCTION__ );
		return false;
	}
//	PRINTF("Slot=%d   BibId=%03X%05d", tCommonData.iSlot, (tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff);
////////////////////////////////////////////////////
	int nInitFailSize = m_nMaxDutPerCpu;
	char *pInitFail = new (std::nothrow)char[ nInitFailSize ];
	if ( pInitFail == nullptr )
	{
		EPRINTF("%s Memory(Init Fail) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getInitFailData(pInitFail, iCpu) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		return false;
	}

	uint nTotal = m_nMaxDutPerCpu;
	for(uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut)
	{
		if ( pInitFail[iDut] )
			--nTotal;
	}

////////////////////////////////////////////////////
		int nStepFailSize = m_nMaxDutPerCpu;
		char *pStepFail = new (std::nothrow)char[ nStepFailSize*nStep ];
		if ( pStepFail == nullptr )
		{
			EPRINTF("%s Memory(Step fail) alloc error", __FUNCTION__ );
			return false;
		}
		if ( ! getStepFailData(nStep, nStepFailSize, pStepFail, iCpu) )
		{
			EPRINTF("%s Init Fail data read error", __FUNCTION__ );
			if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
			if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
			return false;
		}
////////////////////////////////////////////////////
		int nSortBinSize = m_nMaxDutPerCpu;
		uint *pSortBin = new (std::nothrow)uint[ nSortBinSize ];
		if ( pSortBin == nullptr )
		{
			EPRINTF("%s Memory(Step fail) alloc error", __FUNCTION__ );
			return false;
		}
		if ( ! getSortBinData(pSortBin, iCpu) )
		{
			EPRINTF("%s Sort Bin data read error", __FUNCTION__ );
			if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
			if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
			if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
			return false;
		}

		uint nSortBinCnt[17] = { 0, };
		for(uint iDut=0; iDut<m_nMaxDutPerCpu; ++iDut)
		{
			if (  IsPreFail(pInitFail, iDut) ) continue;;
			if ( pSortBin[iDut] > 16         ) continue;

			nSortBinCnt[ pSortBin[iDut] ]++;
//			switch( pSortBin[iDut] )
//			{
//				case  0: nSortBinCnt[ 0]++; break;
//				case  1: nSortBinCnt[ 1]++; break;
//				case  2: nSortBinCnt[ 2]++; break;
//				case  3: nSortBinCnt[ 3]++; break;
//				case  4: nSortBinCnt[ 4]++; break;
//				case  5: nSortBinCnt[ 5]++; break;
//				case  6: nSortBinCnt[ 6]++; break;
//				case  7: nSortBinCnt[ 7]++; break;
//				case  8: nSortBinCnt[ 8]++; break;
//				case  9: nSortBinCnt[ 9]++; break;
//				case 10: nSortBinCnt[10]++; break;
//				case 11: nSortBinCnt[11]++; break;
//				case 12: nSortBinCnt[12]++; break;
//				case 13: nSortBinCnt[13]++; break;
//				case 14: nSortBinCnt[14]++; break;
//				case 15: nSortBinCnt[15]++; break;
//				case 16: nSortBinCnt[16]++; break;
//			}
		}

////////////////////////////////////////////////////
	std::vector<time_t> vBegin;
	std::vector<time_t> vEnd;
	if ( ! getStepTimeStampData(nStep, vBegin, vEnd, iCpu) )
	{
		EPRINTF("%s step time stamp data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		return false;
	}

	std::vector<CStepData *> vStepData;
	if ( ! getStepData(vStepData, iCpu) )
	{
		EPRINTF("%s step data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

//////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
	sprintf(szFile, "Sample_%d.FLS", iCpu);
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

	string strCr = "\n";
	string strLine;
	string strData;
	char   szLine[1024];
	time_t tNow;
	struct tm* t;
////////////////////////////////////////////////////////////
//	Head
	strData.clear();
	sprintf(szLine, "%-100s", "<< SORTING DATA SUMMARY >>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	sprintf(szLine, "PARTS %-17s "     , " "                  );	strData += szLine;
	sprintf(szLine, "LOTNO %-10s "     , " "                  );	strData += szLine;
	sprintf(szLine, "SUBLOT %2d "      , 0                    );	strData += szLine;
	sprintf(szLine, "TP %-8s"          , " "                  );	strData += szLine + strCr;
	sprintf(szLine, "STATION %s%1d "   , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d "      , 1                    );	strData += szLine;
	sprintf(szLine, "SUBLOT-STATUS %s ", "CLOSE"              );	strData += szLine;
	sprintf(szLine, "REMARK "                                 );	strData += szLine; strData += tCommonData.szRemark;
	sprintf(szLine, " OPER %s"         , ""                   );	strData += szLine + strCr;
	sprintf(szLine, "OID EIS"                                 );	strData += szLine + strCr;

	sprintf(szLine, "%38s"             , "SORTING-LINE"       );	strData += szLine + strCr;
	strLine = "  BID      TOTAL    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16"; strData += strLine + strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%03X%05d ", (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff );	strData += szLine;
	sprintf(szLine, "%6d"      , nTotal );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4d", nSortBinCnt[iSortBin] );	strData += szLine;
	}
	strData += strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%-9s ", "TOTAL" );	strData += szLine;
	sprintf(szLine, "%6d"  , nTotal );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4d", nSortBinCnt[iSortBin] );	strData += szLine;
	}
	strData += strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%-16s", "RATE (%)" );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4.1f", nSortBinCnt[iSortBin]*100.0/nTotal );	strData += szLine;
	}
	strData += strCr + strCr;
/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<< SORTING DATA BOARD MAP >>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	sprintf(szLine, "PARTS %-17s "       , " "                  );	strData += szLine;
	sprintf(szLine, "LOTNO %-10s "       , " "                  );	strData += szLine;
	sprintf(szLine, "SUBLOT %2d "        , 0                    );	strData += szLine;
	sprintf(szLine, "TP %-8s"            , " "                  );	strData += szLine + strCr;
	sprintf(szLine, "STATION %s%1d "     , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d "        , 1                    );	strData += szLine;
	sprintf(szLine, "SUBLOT-STATUS %s "  , "CLOSE"              );	strData += szLine;
	sprintf(szLine, "REMARK "                                   );	strData += szLine; strData += tCommonData.szRemark;
	sprintf(szLine, " OPER %s"           , ""                   );	strData += szLine + strCr;
	sprintf(szLine, "OID EIS"                                   );	strData += szLine + strCr;
	sprintf(szLine, "BID %03X%05d "      , (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff );	strData += szLine;
	sprintf(szLine, "Z1S%02d IOC=0 BOC=0", tCommonData.iSlot    );	strData += szLine + strCr;
	sprintf(szLine, "Total %d"           , nTotal               );	strData += szLine + strCr;

	strData += "   ";
	for ( uint r=0; r<BIB_X(); r++ )
	{
		sprintf( szLine, " %2d", r+1 ); strData += szLine;
	}
	strData += strCr;

	for ( int c=BIB_Y()-1; c>=0; c-- )
	{
		sprintf( szLine, " %2d", c+1 ); strData += szLine;
		for ( uint r=0; r<BIB_X(); r++ )
		{
			int dut = c*BIB_X() + r;

				 if ( !IS_MYDUT(dut)             ) sprintf( szLine, "  -"                                 );
			else if (  IsPreFail(pInitFail, dut) ) sprintf( szLine, "  ."                                 );
			else                                   sprintf( szLine, " %2d", pSortBin[dut%m_nMaxDutPerCpu] );
			strData += szLine;
		}
		strData += strCr;
	}
	strData += strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<TEST HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
	strData += strLine + strCr;

	for(uint i=0; i<vStepData.size(); ++i )
	{
		t = localtime( &vStepData[i]->tStepEnd );
		sprintf(szLine, "%3d %02d/%02d %02d:%02d ", vStepData[i]->iTsNo, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min                         );	strData += szLine;
//		sprintf(szLine, "%s  %3d %6.1f %7.1f "   , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, vStepData[i]->dTemp, vStepData[i]->dBiTime/3600 );	strData += szLine;
		sprintf(szLine, "%s  %3d %6.1f %7.1f "   , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, 0.0                , 0.0                        );	strData += szLine;
		sprintf(szLine, "%-20s %-20s %5d %5d "  , vStepData[i]->szStepName, vStepData[i]->szPc, nTotal, nTotal                                  );	strData += szLine;
		uint   iStepFailCnt = 0;
		double dFailRate    = 0.0;
		if ( i>0 )
		{
			iStepFailCnt = StepFailCnt( pStepFail, pInitFail, i-1 );
			dFailRate    = iStepFailCnt * 100.0 / nTotal;
		}
		sprintf(szLine, "%5d %7.2f %4.1f", iStepFailCnt, dFailRate, vStepData[i]->dVcc );	strData += szLine + strCr;
	}
	strData += strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<TEST Block Count HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
	strData += strLine + strCr + strCr + strCr + strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<FAIL SOCKET HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, "BNO:%-3d BID %03X%05d Z%d-S%d", 1, (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff, 1, tCommonData.iSlot );
	strData += szLine + strCr;
	strLine = "                                                                                                         1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    5    5    5    5    5    5    5";	strData += strLine + strCr;
	strLine = "               1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3";	strData += strLine + strCr;
	strLine = "          5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0";	strData += strLine + strCr;
	strLine = "-----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----" ;	strData += strLine + strCr;

	for ( uint c=0; c<BIB_Y(); c++ )
	{
		for ( uint r=0; r<BIB_X(); r++ )
		{
			int dut = c*BIB_X() + r;

				 if ( !IS_MYDUT(dut)                        ) continue;
			else if (  IsPreFail(pInitFail, dut)            ) continue;
			else if (  IsAllStepPass(pStepFail, dut, nStep) ) continue;

			sprintf(szLine, "%02d-%02d|", c+1, r+1 );	strData += szLine;
			strLine.clear();
			for(uint i=0; i<vStepData.size(); ++i )
			{
				if ( i==0 ) { strLine += "m"; continue; }

				if ( IsStepFail(pStepFail, dut, i-1) ) strLine += "X";
				else                                   strLine += "o";

			}
			strData += strLine + strCr;
		}
	}

	ssize_t nWrite, nWrited;
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

	if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
	if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
	if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
	for( uint i=0; i<vStepData.size(); ++i )
		delete vStepData[i];
	vStepData.clear();
	close(fd);
	return true;
}

bool CApi::IsCycleEndOther(int iCpu)
{
	bool bEnd;
	int iOtherCpu;

	if ( iCpu==0 ) iOtherCpu = 1;
	else           iOtherCpu = 0;

	if ( ! getCycleEnd(bEnd, iOtherCpu) )
		return false;

	return bEnd;
}

bool CApi::GenTot()
{
	int iCpu = GetCurrentCpuNo();
	if ( ! IsCycleEndOther(iCpu) ) return false;

	GenCumFailBlockT();
	GenFlsT();

	return true;
}

bool CApi::GenCumFailBlockT()
{
//	m_pStChipId = new stChipId[ m_nMaxDutPerCpu * m_nChipPerDut ];
//	CreateChipData();
//	delete [] m_pStChipId; m_pStChipId = NULL;
	int iCpu = GetCurrentCpuNo();
	if ( ! IsCycleEndOther(iCpu) ) return false;
////////////////////////////////////////////////////
	std::vector<std::string> vStepName;
	if ( ! getStepName(vStepName, 0) )
	{
		EPRINTF("%s Step name read error", __FUNCTION__ );
		return false;
	}
	int nStep = vStepName.size();
//	for(size_t i=0; i<vStepName.size(); ++i )
//		printf("[%lu]{%s}\n", i, vStepName[i].c_str() );
////////////////////////////////////////////////////
	stCommonData tCommonData;
	if ( ! getCommonData(tCommonData, 0) )
	{
		EPRINTF("%s Common data read error", __FUNCTION__ );
		return false;
	}
//	PRINTF("Slot=%d   BibId=%03X%05d", tCommonData.iSlot, (tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff);
////////////////////////////////////////////////////
	bool bChipId = true;
	int nChipDataSize = m_nMaxDutPerCpu * m_nChipPerDut;
	stChipId *pstChipId = new (std::nothrow)stChipId[ nChipDataSize*2 ];
	if ( pstChipId == nullptr )
	{
		EPRINTF("%s Memory(Chip id) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getChipData(&pstChipId[0], 0) )
	{
//		EPRINTF("%s chip data read error", __FUNCTION__ );
		if ( pstChipId != nullptr ) { delete [] pstChipId; pstChipId = nullptr; }
		bChipId = false;
//		return false;
	}
	if ( ! getChipData(&pstChipId[nChipDataSize], 1) )
	{
//		EPRINTF("%s chip data read error", __FUNCTION__ );
		if ( pstChipId != nullptr ) { delete [] pstChipId; pstChipId = nullptr; }
		bChipId = false;
//		return false;
	}
////////////////////////////////////////////////////
	ssize_t nFailBlockSizePerDut = m_nMaxDutPerCpu * m_nBlockPerDutBl;	//* sizeof(char)
	char *pStepBlockFail = new (std::nothrow)char[nStep * nFailBlockSizePerDut*2];
	if ( pStepBlockFail == nullptr )
	{
		EPRINTF("%s Memory(step fail block data) alloc error", __FUNCTION__ );
		if ( pstChipId != nullptr ) { delete [] pstChipId; pstChipId = nullptr; }
		return false;
	}
	if ( ! getStepFailBlockDataT(nStep, nFailBlockSizePerDut, pStepBlockFail, 0) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		return false;
	}
	if ( ! getStepFailBlockDataT(nStep, nFailBlockSizePerDut, pStepBlockFail, 1) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		return false;
	}
////////////////////////////////////////////////////
	char *pCumBlockFail = new (std::nothrow)char[nFailBlockSizePerDut*2];
	if ( pCumBlockFail == nullptr )
	{
		EPRINTF("%s Memory(cum fail block data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		return false;
	}
	if ( ! getCumFailBlockData(nFailBlockSizePerDut, &pCumBlockFail[0], 0) )
	{
		EPRINTF("%s cum fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		return false;
	}
	if ( ! getCumFailBlockData(nFailBlockSizePerDut, &pCumBlockFail[nFailBlockSizePerDut], 1) )
	{
		EPRINTF("%s cum fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	ssize_t nFailChipSizePerStep = m_nMaxDutPerCpu * m_nChipPerDutBl;	//* sizeof(char)
	char *pStepFailChip = new (std::nothrow)char[nStep * nFailChipSizePerStep * 2];
	if ( pStepFailChip == nullptr )
	{
		EPRINTF("%s Memory(step fail chip data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		return false;
	}
	if ( ! getStepFailChipDataT(nStep, nFailChipSizePerStep, pStepFailChip, 0) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		return false;
	}
	if ( ! getStepFailChipDataT(nStep, nFailChipSizePerStep, pStepFailChip, 1) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	char *pCumFailChip = new (std::nothrow)char[nFailChipSizePerStep*2];
	if ( pCumFailChip == nullptr )
	{
		EPRINTF("%s Memory(step fail chip data) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		return false;
	}
	if ( ! getCumFailChipData(nFailChipSizePerStep, &pCumFailChip[0], 0) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		return false;
	}
	if ( ! getCumFailChipData(nFailChipSizePerStep, &pCumFailChip[nFailChipSizePerStep], 1) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	int nInitFailSize = m_nMaxDutPerCpu;
	char *pInitFail = new (std::nothrow)char[ nInitFailSize*2 ];
	if ( pInitFail == nullptr )
	{
		EPRINTF("%s Memory(Init Fail) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		return false;
	}
	if ( ! getInitFailData(&pInitFail[0], 0) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		return false;
	}
	if ( ! getInitFailData(&pInitFail[nInitFailSize], 1) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	int nTestFailSize = m_nMaxDutPerCpu;
	char *pTestFail = new (std::nothrow)char[ nTestFailSize*2 ];
	if ( pTestFail == nullptr )
	{
		EPRINTF("%s Memory(Test Fail) alloc error", __FUNCTION__ );
		//2023.05.12 - fatric
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		return false;
	}
	if ( ! getTestFailData(&pTestFail[0], 0) )
	{
		EPRINTF("%s Test Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
	if ( ! getTestFailData(&pTestFail[nTestFailSize], 1) )
	{
		EPRINTF("%s Test Fail data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	std::vector<time_t> vBegin;
	std::vector<time_t> vEnd;
	if ( ! getStepTimeStampData(nStep, vBegin, vEnd, 0) )
	{
		EPRINTF("%s step fail block data read error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
//2023.05.12 - fatric
////////////////////////////////////////////////////////////
	bool *pProcExtFmBlock = new (std::nothrow)bool[ nStep ];
	if ( pProcExtFmBlock == nullptr )
	{
		EPRINTF("%s Memory(ProcExtFmBlock Fail) alloc error", __FUNCTION__ );
		if ( pstChipId      != nullptr ) { delete [] pstChipId     ; pstChipId      = nullptr; }
		if ( pStepBlockFail != nullptr ) { delete [] pStepBlockFail; pStepBlockFail = nullptr; }
		if ( pCumBlockFail  != nullptr ) { delete [] pCumBlockFail ; pCumBlockFail  = nullptr; }
		if ( pStepFailChip  != nullptr ) { delete [] pStepFailChip ; pStepFailChip  = nullptr; }
		if ( pCumFailChip   != nullptr ) { delete [] pCumFailChip  ; pCumFailChip   = nullptr; }
		if ( pInitFail      != nullptr ) { delete [] pInitFail     ; pInitFail      = nullptr; }
		if ( pTestFail      != nullptr ) { delete [] pTestFail     ; pTestFail      = nullptr; }
		return false;
	}
	if ( ! getProcExtFmBlock(nStep, pProcExtFmBlock, 0) )
	{
		EPRINTF("%s Test Fail data read error", __FUNCTION__ );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }
		return false;
	}
////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
//	sprintf(szFile, "Sample_Tot.cumFailBlock");
	{
		struct tm* t;
		t = localtime( &tCommonData.tCycleEnd );
		sprintf(szFile, "Sample_Tot_%02d%02d%02d%02d.cumFailBlock", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	}
	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }
		return false;
	}

	string strCr = "\n";
	string strLine;
	string strData;
	char   szStepNm[33];
	char   szLine[1024];
	struct tm* t;
////////////////////////////////////////////////////////////
//	Head
	strData.clear();
	strLine = "REV            :  "; strData += strLine + strCr;
	strLine = "PART NAME      :  "; strData += strLine + strCr;
	strLine = "LOT NO         :  "; strData += strLine + strCr;
	strLine = "PGM NAME       :  "; strData += strLine + strCr;
	strLine = "FW_REV         :  "; strData += strLine + strCr;
	strLine = "CAT_REV        :  "; strData += strLine + strCr;
	strLine = "REMARK         :  "; strData += strLine + tCommonData.szRemark + strCr;
	strLine = "C_Vendor       :  "; strData += strLine + strCr;
	strLine = "SYSTEM_NAME    :  "; strData += strLine + strCr;
	strLine = "START TIME     :  "; strData += strLine;
	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "END TIME       :  "; strData += strLine;
	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "OPER           :  "; strData += strLine + strCr;
	strLine = "DEVICE         :  "; strData += strLine + strCr;
	strLine = "FACTORY        :  "; strData += strLine + strCr;
	strLine = "Fab            :  "; strData += strLine + strCr;
	strLine = "Owner          :  "; strData += strLine + strCr;
	strLine = "Grade          :  "; strData += strLine + strCr;

	ssize_t nWrite, nWrited;
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
		if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
		if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
		if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
		if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
		if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
		if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
		if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }	//2023.05.12 - fatric
		return false;
	}

////////////////////////////////////////////////////////////
//	if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
//	if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
//	if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
//	if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
//	if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
//	if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
//	if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
//	if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }	//2023.05.12 - fatric
//	close(fd);
//
//	return false;
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
	for ( uint r = 0;r<BIB_X();r++ )
	{
		for ( uint c = 0;c<BIB_Y();c++ )
		{
			int dut = c*BIB_X() + r;
//			if ( !IS_MYDUT(dut)             ) continue;
			if ( IsPreFailT(pInitFail, dut) ) continue;

			for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
			{
				for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
				{
//					printf( "Slot%02d.(%03X%05d)R%02d, C%02d, CE%02d, CHIP%02d     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL\n",
//								tCommonData.iSlot,
//								(tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff,
//								r+1, c+1, iCe+1, iChip+1,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
//								pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
//						  );

					strData.clear();
					sprintf( szLine, "Slot%02d.(%03X%05d)R%02d, C%02d, CE%02d, CHIP%02d",
								tCommonData.iSlot,
								(tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff,
								r+1, c+1, iCe+1, iChip+1
						   );
					strData += szLine;

					if ( bChipId )
					{
						sprintf( szLine, "     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
									pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
									pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
									pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
									pstChipId[dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
							   );
						strData += szLine;
					}
					strData += strCr;


//					sprintf( szLine, "Cum(%s/%s)                     ", (IsTestFail (pTestFail   , dut)?"FAIL":"PASS"), (IsChipFail (pCumFailChip, dut, iCe, iChip)?"FAIL":"PASS") );
					sprintf( szLine, "Cum(%s/%s)                     ", (IsChipFailT(pCumFailChip, dut)?"FAIL":"PASS"), (IsChipFailT(pCumFailChip, dut, iCe, iChip)?"FAIL":"PASS") );
					strData += szLine;
					std::vector<uint> vBlkAddr;
					getFailBlockInfoT(vBlkAddr, pCumBlockFail, dut, iCe, iChip);
					sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
					strData += szLine;
					int nFailBlk = (int)vBlkAddr.size();
					if ( nFailBlk > 254 ) nFailBlk = 254;
					for(int i=0; i<nFailBlk; ++i)
					{
						sprintf( szLine, " %x ", vBlkAddr[i] );
						strData += szLine;
					}
					strData += strCr;

					//step loop
					std::vector<uint> vCumBlkAddr;
					vCumBlkAddr.clear();
					for(int iStep=0; iStep<nStep; ++iStep)
					{
						sprintf( szStepNm, "%02d.%s%s", iStep+1, vStepName[iStep].c_str(), "(N)" );
						sprintf( szLine, "%-35s", szStepNm );
						strData += szLine;

						vBlkAddr.clear();
						getFailBlockInfoT(vBlkAddr, pStepBlockFail, dut, iCe, iChip, iStep);
						//2023.05.12 - fatric( modify )
						if ( vBlkAddr.size()<1 && IsChipFailT(pStepFailChip, dut, iCe, iChip, iStep) && ! pProcExtFmBlock[iStep] )
						{
							sprintf( szLine, "%dEA FAIL ", 1 );
							strData += szLine + strCr;
						}
						else
						{
							sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
							strData += szLine;
							nFailBlk = (int)vBlkAddr.size();
							if ( nFailBlk > 253 ) nFailBlk = 253;
							for(int i=0; i<nFailBlk; ++i)
							{
								sprintf( szLine, " %x%c", vBlkAddr[i], !IsIn(vCumBlkAddr, vBlkAddr[i])?'!':' ' );
								strData += szLine;
							}
							if ( vBlkAddr.size() > 253 ) strData += " ...";
							strData += strCr;
						}
						mergeFailBlockInfo(vCumBlkAddr, vBlkAddr);
					}

					nWrite = strData.size();
					nWrited = write( fd, strData.c_str(), nWrite );
					if ( nWrited != nWrite )
					{
						EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
						close( fd );
						return false;
					}

				}//End of iChip
			}//End of iCe
		}//End of c
	}//End of r

	char szBegin[32];
	char szEnd  [32];
	char szTmp  [256];
	strData.clear();
	for(int iStep=0; iStep<nStep; ++iStep)
	{
		t = localtime( &vBegin[iStep] );
		sprintf( szBegin, "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
		t = localtime( &vEnd  [iStep] );
		sprintf( szEnd  , "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );

//		sprintf( szLine, "%02d.%-32s:%s - %s", iStep+1, vStepName[iStep].c_str(), szBegin, szEnd );

		sprintf( szTmp, "%02d.%-s"      , iStep+1, vStepName[iStep].c_str() );
		sprintf( szLine, "%-35s:%s - %s", szTmp, szBegin, szEnd );
		strData += szLine + strCr;
	}//End of iStep
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		return false;
	}
	close(fd);

////////////////////////////////////////////////////////////
	if ( pstChipId       != nullptr ) { delete [] pstChipId      ; pstChipId       = nullptr; }
	if ( pStepBlockFail  != nullptr ) { delete [] pStepBlockFail ; pStepBlockFail  = nullptr; }
	if ( pCumBlockFail   != nullptr ) { delete [] pCumBlockFail  ; pCumBlockFail   = nullptr; }
	if ( pStepFailChip   != nullptr ) { delete [] pStepFailChip  ; pStepFailChip   = nullptr; }
	if ( pCumFailChip    != nullptr ) { delete [] pCumFailChip   ; pCumFailChip    = nullptr; }
	if ( pInitFail       != nullptr ) { delete [] pInitFail      ; pInitFail       = nullptr; }
	if ( pTestFail       != nullptr ) { delete [] pTestFail      ; pTestFail       = nullptr; }
	if ( pProcExtFmBlock != nullptr ) { delete [] pProcExtFmBlock; pProcExtFmBlock = nullptr; }

	return true;
}

bool CApi::GenFlsT()
{
//	m_pStChipId = new stChipId[ m_nMaxDutPerCpu * m_nChipPerDut ];
//	CreateChipData();
//	delete [] m_pStChipId; m_pStChipId = NULL;
	int iCpu = GetCurrentCpuNo();
	if ( ! IsCycleEndOther(iCpu) ) return false;
////////////////////////////////////////////////////
	std::vector<std::string> vStepName;
	if ( ! getStepName(vStepName, 0) )
	{
		EPRINTF("%s Step name read error", __FUNCTION__ );
		return false;
	}
	int nStep = vStepName.size();
//	for(size_t i=0; i<vStepName.size(); ++i )
//		printf("[%lu]{%s}\n", i, vStepName[i].c_str() );
////////////////////////////////////////////////////
	stCommonData tCommonData;
	if ( ! getCommonData(tCommonData, 0) )
	{
		EPRINTF("%s Common data read error", __FUNCTION__ );
		return false;
	}
//	PRINTF("Slot=%d   BibId=%03X%05d", tCommonData.iSlot, (tCommonData.nBibId>>16)&0xffff, tCommonData.nBibId&0xffff);
////////////////////////////////////////////////////
	int nInitFailSize = m_nMaxDutPerCpu;
	char *pInitFail = new (std::nothrow)char[ nInitFailSize*2 ];
	if ( pInitFail == nullptr )
	{
		EPRINTF("%s Memory(Init Fail) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getInitFailData(&pInitFail[0], 0) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		return false;
	}
	if ( ! getInitFailData(&pInitFail[nInitFailSize], 1) )
	{
		EPRINTF("%s Init Fail data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		return false;
	}

	uint nTotal = m_nMaxDutPerCpu*2;
	for(uint iDut=0; iDut<m_nMaxDutPerCpu*2; ++iDut)
	{
		if ( pInitFail[iDut] )
			--nTotal;
	}

////////////////////////////////////////////////////
	int nStepFailSize = m_nMaxDutPerCpu;
	char *pStepFail = new (std::nothrow)char[ nStepFailSize*nStep*2 ];
	if ( pStepFail == nullptr )
	{
		EPRINTF("%s Memory(Step fail) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getStepFailDataT(nStep, nStepFailSize, pStepFail, 0) )
	{
		EPRINTF("%s Step Fail data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		return false;
	}
	if ( ! getStepFailDataT(nStep, nStepFailSize, pStepFail, 1) )
	{
		EPRINTF("%s Step Fail data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		return false;
	}
////////////////////////////////////////////////////
	int nSortBinSize = m_nMaxDutPerCpu;
	uint *pSortBin = new (std::nothrow)uint[ nSortBinSize*2 ];
	if ( pSortBin == nullptr )
	{
		EPRINTF("%s Memory(Step fail) alloc error", __FUNCTION__ );
		return false;
	}
	if ( ! getSortBinData(&pSortBin[0], 0) )
	{
		EPRINTF("%s Sort Bin data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		return false;
	}
	if ( ! getSortBinData(&pSortBin[nSortBinSize], 1) )
	{
		EPRINTF("%s Sort Bin data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		return false;
	}

	uint nSortBinCnt[17] = { 0, };
	for(uint iDut=0; iDut<m_nMaxDutPerCpu*2; ++iDut)
	{
		if ( IsPreFailT(pInitFail, iDut) ) continue;;
		if ( pSortBin[iDut] > 16         ) continue;

		nSortBinCnt[ pSortBin[iDut] ]++;
//		switch( pSortBin[iDut] )
//		{
//			case  0: nSortBinCnt[ 0]++; break;
//			case  1: nSortBinCnt[ 1]++; break;
//			case  2: nSortBinCnt[ 2]++; break;
//			case  3: nSortBinCnt[ 3]++; break;
//			case  4: nSortBinCnt[ 4]++; break;
//			case  5: nSortBinCnt[ 5]++; break;
//			case  6: nSortBinCnt[ 6]++; break;
//			case  7: nSortBinCnt[ 7]++; break;
//			case  8: nSortBinCnt[ 8]++; break;
//			case  9: nSortBinCnt[ 9]++; break;
//			case 10: nSortBinCnt[10]++; break;
//			case 11: nSortBinCnt[11]++; break;
//			case 12: nSortBinCnt[12]++; break;
//			case 13: nSortBinCnt[13]++; break;
//			case 14: nSortBinCnt[14]++; break;
//			case 15: nSortBinCnt[15]++; break;
//			case 16: nSortBinCnt[16]++; break;
//		}
	}

////////////////////////////////////////////////////
	std::vector<time_t> vBegin;
	std::vector<time_t> vEnd;
	if ( ! getStepTimeStampData(nStep, vBegin, vEnd, 0) )
	{
		EPRINTF("%s step time stamp data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		return false;
	}

	std::vector<CStepData *> vStepData;
	if ( ! getStepData(vStepData, 0) )
	{
		EPRINTF("%s step data read error", __FUNCTION__ );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

//////////////////////////////////////////////////////////////
	char szFile[PATH_MAX];
//	sprintf(szFile, "Sample_Tot.FLS");
	{
		struct tm* t;
		t = localtime( &tCommonData.tCycleEnd );
		sprintf(szFile, "Sample_Tot_%02d%02d%02d%02d.FLS", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	}

	int fd = open(szFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		EPRINTF("%s file(%s) open error", __FUNCTION__, szFile );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

	string strCr = "\n";
	string strLine;
	string strData;
	char   szLine[1024];
	time_t tNow;
	struct tm* t;
////////////////////////////////////////////////////////////
//	Head
	strData.clear();
	sprintf(szLine, "%-100s", "<< SORTING DATA SUMMARY >>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	sprintf(szLine, "PARTS %-17s "     , " "                  );	strData += szLine;
	sprintf(szLine, "LOTNO %-10s "     , " "                  );	strData += szLine;
	sprintf(szLine, "SUBLOT %2d "      , 0                    );	strData += szLine;
	sprintf(szLine, "TP %-8s"          , " "                  );	strData += szLine + strCr;
	sprintf(szLine, "STATION %s%1d "   , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d "      , 1                    );	strData += szLine;
	sprintf(szLine, "SUBLOT-STATUS %s ", "CLOSE"              );	strData += szLine;
	sprintf(szLine, "REMARK "                                 );	strData += szLine; strData += tCommonData.szRemark;
	sprintf(szLine, " OPER %s"         , ""                   );	strData += szLine + strCr;
	sprintf(szLine, "OID EIS"                                 );	strData += szLine + strCr;

	sprintf(szLine, "%38s"             , "SORTING-LINE"       );	strData += szLine + strCr;
	strLine = "  BID      TOTAL    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16"; strData += strLine + strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%03X%05d ", (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff );	strData += szLine;
	sprintf(szLine, "%6d"      , nTotal );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4d", nSortBinCnt[iSortBin] );	strData += szLine;
	}
	strData += strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%-9s ", "TOTAL" );	strData += szLine;
	sprintf(szLine, "%6d"  , nTotal );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4d", nSortBinCnt[iSortBin] );	strData += szLine;
	}
	strData += strCr;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
	sprintf(szLine, "%-16s", "RATE (%)" );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4.1f", nSortBinCnt[iSortBin]*100.0/nTotal );	strData += szLine;
	}
	strData += strCr + strCr;
/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<< SORTING DATA BOARD MAP >>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	t = localtime( &tCommonData.tCycleBegin );
	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine;

	sprintf(szLine, "PARTS %-17s "       , " "                  );	strData += szLine;
	sprintf(szLine, "LOTNO %-10s "       , " "                  );	strData += szLine;
	sprintf(szLine, "SUBLOT %2d "        , 0                    );	strData += szLine;
	sprintf(szLine, "TP %-8s"            , " "                  );	strData += szLine + strCr;
	sprintf(szLine, "STATION %s%1d "     , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d "        , 1                    );	strData += szLine;
	sprintf(szLine, "SUBLOT-STATUS %s "  , "CLOSE"              );	strData += szLine;
	sprintf(szLine, "REMARK "                                   );	strData += szLine; strData += tCommonData.szRemark;
	sprintf(szLine, " OPER %s"           , ""                   );	strData += szLine + strCr;
	sprintf(szLine, "OID EIS"                                   );	strData += szLine + strCr;
	sprintf(szLine, "BID %03X%05d "      , (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff );	strData += szLine;
	sprintf(szLine, "Z1S%02d IOC=0 BOC=0", tCommonData.iSlot    );	strData += szLine + strCr;
	sprintf(szLine, "Total %d"           , nTotal               );	strData += szLine + strCr;

	strData += "   ";
	for ( uint r=0; r<BIB_X(); r++ )
	{
		sprintf( szLine, " %2d", r+1 ); strData += szLine;
	}
	strData += strCr;

	for ( int c=BIB_Y()-1; c>=0; c-- )
	{
		sprintf( szLine, " %2d", c+1 ); strData += szLine;
		for ( uint r=0; r<BIB_X(); r++ )
		{
			int dut = c*BIB_X() + r;

			if (  IsPreFailT(pInitFail, dut) ) sprintf( szLine, "  ."                 );
			else                               sprintf( szLine, " %2d", pSortBin[dut] );
//			else                               sprintf( szLine, " %2d", pSortBin[dut%m_nMaxDutPerCpu] );
			strData += szLine;
		}
		strData += strCr;
	}
	strData += strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<TEST HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
	strData += strLine + strCr;

	for(uint i=0; i<vStepData.size(); ++i )
	{
		t = localtime( &vStepData[i]->tStepEnd );
		sprintf(szLine, "%3d %02d/%02d %02d:%02d ", vStepData[i]->iTsNo, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min                         );	strData += szLine;
//		sprintf(szLine, "%s  %3d %6.1f %7.1f "   , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, vStepData[i]->dTemp, vStepData[i]->dBiTime/3600 );	strData += szLine;
		sprintf(szLine, "%s  %3d %6.1f %7.1f "   , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, 0.0                , 0.0                        );	strData += szLine;
		sprintf(szLine, "%-20s %-20s %5d %5d "  , vStepData[i]->szStepName, vStepData[i]->szPc, nTotal, nTotal                                  );	strData += szLine;
		uint   iStepFailCnt = 0;
		double dFailRate    = 0.0;
		if ( i>0 )
		{
			iStepFailCnt = StepFailCntT( pStepFail, pInitFail, i-1 );
			dFailRate    = iStepFailCnt * 100.0 / nTotal;
		}
		sprintf(szLine, "%5d %7.2f %4.1f", iStepFailCnt, dFailRate, vStepData[i]->dVcc );	strData += szLine + strCr;
	}
	strData += strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<TEST Block Count HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
	strData += strLine + strCr + strCr + strCr + strCr;

/////////////////////////////////////////////////////////////////////////////////////////////////
	sprintf(szLine, "%-100s", "<<FAIL SOCKET HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
	tNow = time(NULL); 	t = localtime( &tNow );
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, " PARTS %s   "     , ""                   );	strData += szLine;
	sprintf(szLine, "LOTNO %s   "      , ""                   );	strData += szLine;
	sprintf(szLine, "BATCH %04d    "   , 0                    );	strData += szLine;
	sprintf(szLine, "TP %s   "         , " "                  );	strData += szLine;
	sprintf(szLine, "STATION %s%1d   " , "AT6200_S", 1        );	strData += szLine;
	sprintf(szLine, "CHAMBER%1d    "   , 1                    );	strData += szLine;

	t = localtime( &tCommonData.tCycleEnd );
	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
	strData += szLine + strCr;

	sprintf(szLine, "BNO:%-3d BID %03X%05d Z%d-S%d", 1, (tCommonData.nBibId>>16)&0xfff, tCommonData.nBibId&0xffff, 1, tCommonData.iSlot );
	strData += szLine + strCr;
	strLine = "                                                                                                         1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    5    5    5    5    5    5    5";	strData += strLine + strCr;
	strLine = "               1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3";	strData += strLine + strCr;
	strLine = "          5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0";	strData += strLine + strCr;
	strLine = "-----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----" ;	strData += strLine + strCr;

	for ( uint c=0; c<BIB_Y(); c++ )
	{
		for ( uint r=0; r<BIB_X(); r++ )
		{
			int dut = c*BIB_X() + r;

				 if (  IsPreFailT(pInitFail, dut)            ) continue;
			else if (  IsAllStepPassT(pStepFail, dut, nStep) ) continue;

			sprintf(szLine, "%02d-%02d|", c+1, r+1 );	strData += szLine;
			strLine.clear();
			for(uint i=0; i<vStepData.size(); ++i )
			{
				if ( i==0 ) { strLine += "m"; continue; }

				if ( IsStepFailT(pStepFail, dut, i-1) ) strLine += "X";
				else                                    strLine += "o";

			}
			strData += strLine + strCr;
		}
	}

	ssize_t nWrite, nWrited;
	nWrite = strData.size();
	nWrited = write( fd, strData.c_str(), nWrite );
	if ( nWrited != nWrite )
	{
		EPRINTF("%s file(%s) write error", __FUNCTION__, szFile );
		close( fd );
		if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
		if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
		if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
		for( uint i=0; i<vStepData.size(); ++i )
			delete vStepData[i];
		vStepData.clear();
		return false;
	}

	if ( pInitFail != nullptr ) { delete [] pInitFail; pInitFail = nullptr; }
	if ( pStepFail != nullptr ) { delete [] pStepFail; pStepFail = nullptr; }
	if ( pSortBin  != nullptr ) { delete [] pSortBin ; pSortBin  = nullptr; }
	for( uint i=0; i<vStepData.size(); ++i )
		delete vStepData[i];
	vStepData.clear();
	close(fd);
	return true;
}

//2023.08.17 - KWMOON
void CApi::SET_MATCHCPEDLY( uint cycle )
{
	if( cycle > 7) cycle = 7;
	g_pg.SetMatchCpeDelay( cycle );
	if( 1 )
	{
		PRINTF( "MATCH CPEDLEAY (0x%08X,0x%08X)", BITCHART::GLOBAL_MODE_REG::_MATCH_CPE_DELAY, cycle );
	}
}
