#include "od.hpp"

namespace fs = std::filesystem;

T_SLOT::T_SLOT()
{
	Init();
}

T_SLOT::~T_SLOT()
{
	if ( sdut != nullptr ) { delete [] sdut; sdut = nullptr; }
	if ( ldut != nullptr ) { delete [] ldut; ldut = nullptr; }
	if ( mdut != nullptr ) { delete [] mdut; mdut = nullptr; }
}

void T_SLOT::Init()
{
	m_bValid    = false;
	BIB_ID  [0] = '\0';
	BIB_X       = 0;
	BIB_Y       = 0;
	MLB_CODE[0] = '\0';
	IOC     [0] = '\0';
	BOC     [0] = '\0';

	sdut       = nullptr;
	ldut       = nullptr;
	mdut       = nullptr;

	nInQty     = 0;
	memset(nSortBinCnt , 0, sizeof(nSortBinCnt ));
	memset(nSortBinHCnt, 0, sizeof(nSortBinHCnt));
}

void T_SLOT::SET_VALID(bool bValid)
{
	m_bValid = bValid;
}

bool T_SLOT::IS_VALID()
{
	return m_bValid;
}
/////////////////////////////////////////////////////////////////////////////////////////
T_LOT::T_LOT()
{
	Init();
}

T_LOT::T_LOT(string sLotNo)
{
	Init();
	sprintf( LOT_NO, "%s", sLotNo.c_str() );
}

void T_LOT::Init()
{
	LOT_NO     [0] = '\0';
	PART_NO    [0] = '\0';
	TEST_PGM_NO[0] = '\0';
	DEVICE_NO  [0] = '\0';
	OPER       [0] = '\0';
	FAB        [0] = '\0';
	GRADE      [0] = '\0';
	OWNER      [0] = '\0';
	FACTORY    [0] = '\0';
	RWK_COUNT      = 0;
	RWK_CODE   [0] = '\0';

	MPG_FLAG       = 'N';
	SNO            = 0;

	RS = nullptr;
	for(int i=0; i<32; ++i)
		S[i].Init();

	m_szReportDir[0] = '\0';

	m_uRS      = 0;					//Reference Slot No: 0 ~ 15
	m_uRC      = 0;					//Reference CPU  No: 0 ~  1

	m_tInitStart = 0; //for SaveMap
	m_tInitEnd   = 0; //for SaveMap
}
/////////////////////////////////////////////////////////////////////////////////////////
CFtpConf::CFtpConf()
{
	Init();
}

void CFtpConf::Init()
{
	IP  [0] = '\0';;
	USER[0] = '\0';;
	PASS[0] = '\0';;
	DIR [0] = '\0';;
}

bool CFtpConf::Get(const char* pszSvr)
{
	char*  pszEnv = getenv(ATOS_DIR_ENV);						// "/opt/at6200"
	char   szOsDir  [PATH_MAX];
	char   szIniFile[PATH_MAX];
//	char   szSection[] = "RESULT_UPLOAD";
	char   szSection[] = "SERVER";
	char   szKey[PATH_MAX];
	QString sValue;

	if( pszEnv != NULL ) sprintf(szOsDir, "%s", pszEnv     );	// "/opt/at6200"
	else                 sprintf(szOsDir, "%s", DIR_ATOS_PATH);	// "/opt/at6200"

	sprintf(szIniFile, "%s/%s/%s", szOsDir, DIR_CONFIG_PATH, SERVER_INI );	// "/opt/at6200/config/atserver.ini"

	sprintf(szKey, "%s/%s_SERVER", szSection, pszSvr);						//"RESULT_UPLOAD/${pszSvr}_SERVER"
	if ( ! UTIL::INI::readSettings(szIniFile, szKey, sValue) )
	{
		fprintf(stderr,"%s : %s read error(%s)!", __FUNCTION__, szIniFile, szKey);
		return false;
	}
	sprintf( IP, "%s", sValue.toStdString().c_str() );

	sprintf(szKey, "%s/%s_USER", szSection, pszSvr);
	if ( ! UTIL::INI::readSettings(szIniFile, szKey, sValue) )
	{
		fprintf(stderr,"%s : %s read error(%s)!", __FUNCTION__, szIniFile, szKey);
		return false;
	}
	sprintf( USER, "%s", sValue.toStdString().c_str());

	sprintf(szKey, "%s/%s_PASS", szSection, pszSvr);
	if ( ! UTIL::INI::readSettings(szIniFile, szKey, sValue) )
	{
		fprintf(stderr,"%s : %s read error(%s)!", __FUNCTION__, szIniFile, szKey);
		return false;
	}
	sprintf( PASS, "%s", sValue.toStdString().c_str());

	sprintf(szKey, "%s/%s_DIR", szSection, pszSvr);
	if ( ! UTIL::INI::readSettings(szIniFile, szKey, sValue) )
	{
		fprintf(stderr,"%s : %s read error(%s)!", __FUNCTION__, szIniFile, szKey);
		return false;
	}
	sprintf( DIR, "%s", sValue.toStdString().c_str());

	if ( false )
	{
		printf("  IP=%s\n", IP  );
		printf("USER=%s\n", USER);
		printf("PASS=%s\n", PASS);
		printf(" DIR=%s\n", DIR );
	}
//	return false;	//test code
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
COrganizedData::COrganizedData()
{
	m_bDebugMode = false;
	m_pDataMgr = nullptr;

	GetDataRootPath();
	m_pDataMgr = new CFileMap( m_sDataRoot.toStdString().c_str() );
	m_pDataMgr->SET_DEBUGMODE( true );
	m_pDataMgr->GET_UIAUTOMODE();
	GetReportRootPath();

	SET_DEBUGMODE( true );

///////////////////////////////////////////////////////////////////////
//Foundation Data
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		m_pSortBin [iSlot] = nullptr;
		m_pInitFail[iSlot] = nullptr;
		m_pStepFail[iSlot] = nullptr;

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			m_pStepInInfo [iSlot][iCpu] = nullptr;
			m_pStepOutInfo[iSlot][iCpu] = nullptr;
			m_vecStepData [iSlot][iCpu].clear();
		}

		m_pstChipId     [iSlot] = nullptr;
		m_pCumFailChip  [iSlot] = nullptr;
		m_pStepFailChip [iSlot] = nullptr;
		m_pCumFailBlock [iSlot] = nullptr;
		m_pStepFailBlock[iSlot] = nullptr;

		m_pStepFCate    [iSlot] = nullptr;
		m_pStepPCate    [iSlot] = nullptr;
		m_pStepFailMode [iSlot] = nullptr;
		m_pStepUseMode  [iSlot] = nullptr;

		m_szVersion     [iSlot][0] = '\0';
		m_szDevice      [iSlot][0] = '\0';
		m_nMaxCpu       [iSlot]    = 0;
		m_nMaxDut       [iSlot]    = 0;
		m_nMaxDutPerCpu [iSlot]    = 0;
		m_nBibX         [iSlot]    = 0;
		m_nBibY         [iSlot]    = 0;
		m_nMaxStep      [iSlot]    = 0;

		m_nCePerDut     [iSlot]    = 0;
		m_nChipPerCe    [iSlot]    = 0;
		m_nBlockPerCe   [iSlot]    = 0;
		m_nBlockPerChip [iSlot]    = 0;
		m_nChipPerDut   [iSlot]    = 0;
		m_nChipPerDutBl [iSlot]    = 0;
		m_nBlockPerDutBl[iSlot]    = 0;
	}
///////////////////////////////////////////////////////////////////////
}

COrganizedData::~COrganizedData()
{
	if( m_pDataMgr != nullptr ) { delete m_pDataMgr; m_pDataMgr = nullptr; }

	ClrLotList();

///////////////////////////////////////////////////////////////////////
//Foundation Data
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( m_pSortBin [iSlot] != nullptr ) { delete [] m_pSortBin [iSlot]; m_pSortBin [iSlot] = nullptr; }
		if ( m_pInitFail[iSlot] != nullptr ) { delete [] m_pInitFail[iSlot]; m_pInitFail[iSlot] = nullptr; }
		if ( m_pStepFail[iSlot] != nullptr ) { delete [] m_pStepFail[iSlot]; m_pStepFail[iSlot] = nullptr; }

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if ( m_pStepInInfo [iSlot][iCpu] != nullptr ) { delete [] m_pStepInInfo [iSlot][iCpu]; m_pStepInInfo [iSlot][iCpu] = nullptr; }
			if ( m_pStepOutInfo[iSlot][iCpu] != nullptr ) { delete [] m_pStepOutInfo[iSlot][iCpu]; m_pStepOutInfo[iSlot][iCpu] = nullptr; }

			for( uint i=0; i<m_vecStepData[iSlot][iCpu].size(); ++i )
				delete m_vecStepData[iSlot][iCpu][i];
			m_vecStepData[iSlot][iCpu].clear();
		}

		if ( m_pstChipId     [iSlot] != nullptr ) { delete [] m_pstChipId     [iSlot]; m_pstChipId     [iSlot] = nullptr; }
		if ( m_pCumFailChip  [iSlot] != nullptr ) { delete [] m_pCumFailChip  [iSlot]; m_pCumFailChip  [iSlot] = nullptr; }
		if ( m_pStepFailChip [iSlot] != nullptr ) { delete [] m_pStepFailChip [iSlot]; m_pStepFailChip [iSlot] = nullptr; }
		if ( m_pCumFailBlock [iSlot] != nullptr ) { delete [] m_pCumFailBlock [iSlot]; m_pCumFailBlock [iSlot] = nullptr; }
		if ( m_pStepFailBlock[iSlot] != nullptr ) { delete [] m_pStepFailBlock[iSlot]; m_pStepFailBlock[iSlot] = nullptr; }

		if ( m_pStepFCate    [iSlot] != nullptr ) { delete [] m_pStepFCate    [iSlot]; m_pStepFCate    [iSlot] = nullptr; }
		if ( m_pStepPCate    [iSlot] != nullptr ) { delete [] m_pStepPCate    [iSlot]; m_pStepPCate    [iSlot] = nullptr; }
		if ( m_pStepFailMode [iSlot] != nullptr ) { delete [] m_pStepFailMode [iSlot]; m_pStepFailMode [iSlot] = nullptr; }
		if ( m_pStepUseMode  [iSlot] != nullptr ) { delete [] m_pStepUseMode  [iSlot]; m_pStepUseMode  [iSlot] = nullptr; }
	}
///////////////////////////////////////////////////////////////////////
}

void COrganizedData::SET_DEBUGMODE( bool bDebugMode )
{
	m_bDebugMode = bDebugMode;
}

void COrganizedData::SET_ERRMSG( const char* str, ...)
{
	va_list	ap;
	char buf[MAX_TEXT_MSG] = {0x0,};
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	if( m_bDebugMode )
	{
		fprintf( stdout, "%s", buf ); fflush(stdout);
	}
}

void COrganizedData::ClrLotList()
{
	for (LOT_LIST::iterator it=m_Lot.begin(); it!=m_Lot.end(); ++it)
		delete *it;
	m_Lot.clear();
}

void COrganizedData::GetDataRootPath()
{
	char* env_data = getenv(ATDATA_DIR_ENV);							// "/data"
	QString data_path = "";

	if( env_data != NULL ) data_path = tr("%1").arg(env_data);			// "/data"
	else                   data_path = tr("%1").arg(DIR_ATDATA_PATH);	// "/nvme"

	m_sDataRoot    = tr("%1/%2").arg(data_path).arg(DIR_ATDATA_NAME);	// "/data/testdata" or "/nvme/testdata"
}

bool COrganizedData::GetReportRootPath()
{
	char* pszEnvRun  = getenv(ATRUN_DIR_ENV);	// /home/ablee/atsystem

	if( pszEnvRun != NULL )
	{
		// /home/ablee/atsystem/atdb/prod or engr
		snprintf( m_szReportRoot, PATH_MAX, "%s/%s/%s", pszEnvRun, DIR_REPORT_PATH, (m_pDataMgr->GET_UIAUTOMODE() ? DIR_PROD_DATA_PATH : DIR_ENGR_DATA_PATH) );
	}
	else
	{
		// /home/ablee/atsystem/atdb/prod or engr
		char* pszEnvHome = getenv("HOME");
		snprintf( m_szReportRoot, PATH_MAX, "%s/%s/%s/%s", pszEnvHome, "atsystem", DIR_REPORT_PATH, (m_pDataMgr->GET_UIAUTOMODE() ? DIR_PROD_DATA_PATH : DIR_ENGR_DATA_PATH) );
	}

	fs::path pr( m_szReportRoot );
	if( !fs::exists(pr) )
	{
		bool bOk = fs::create_directories(pr);
		if( !bOk )
		{
			SET_ERRMSG("%s : Make directory '%s' error!", __FUNCTION__, m_szReportRoot );
			return false;
		}
	}

	return true;
}

bool COrganizedData::GetRefSlotCpu()
{
	for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		pLot->m_uRS = 0;
		pLot->m_uRC = CPU00;

		bool bValid[32];
		for(int iSlot=0; iSlot<32; ++iSlot)
		{
			if ( pLot->S[iSlot].IS_VALID() ) bValid[iSlot] = true;
			else                             bValid[iSlot] = false;
		}

		if( !m_pDataMgr->GET_LAST_CYCLE_END_SLOT_CPU(pLot->m_uRS, pLot->m_uRC, bValid) )
		{
			SET_ERRMSG("[ ERR] %s : No slots were tested or not lot set\n", __FUNCTION__ );
			return false;
		}
		fprintf( stdout, "[INFO] %s Last Test Slot/Cpu : S#%02d-C#%02d\n", __FUNCTION__, pLot->m_uRS+1, pLot->m_uRC+1 );

		pLot->START_TIME = *(m_pDataMgr->GET_CYCLE_BEGIN_TIME(pLot->m_uRS, pLot->m_uRC));
		pLot->END_TIME   = *(m_pDataMgr->GET_CYCLE_END_TIME  (pLot->m_uRS, pLot->m_uRC));

		pLot->RS = &(pLot->S[pLot->m_uRS]);
	}

	return true;
}

bool COrganizedData::GetDevPgmVar()
{
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if( !m_bValidSlot[iSlot]                    ) continue;
		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(iSlot) ) continue;

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->ReadDevPgmVar(iSlot, iCpu, &m_stDevPgmVar[iSlot][iCpu]) )
				return false;
		}

		m_nMaxCpu[iSlot] = 1;
		bool bCpu2X  = m_stDevPgmVar[iSlot][CPU00].nCpu2X;
		if( bCpu2X ) m_nMaxCpu[iSlot] = 2;

//		uint nMaxStep0 = m_pDataMgr->GET_MAX_STEP( iSlot, CPU00 );
//		uint nMaxStep1 = m_pDataMgr->GET_MAX_STEP( iSlot, CPU01 );
////		printf("%s:%d   MAXSTEP:%u %d\n", __FUNCTION__, __LINE__, nMaxStep0, nMaxStep1);
//		if( nMaxStep0 != nMaxStep1 )
//		{
//			SET_ERRMSG("%s : S#%02d MaxStep[%d], CPU1 MaxStep[%d] is different\n",
//				   __FUNCTION__, iSlot, nMaxStep0, nMaxStep1 );
//			return false;
//		}
//		m_nMaxStep[iSlot] = nMaxStep0;
//
//		//printf( "   ->MAX STEP : %d\n", m_nMaxStep );
//		if( m_nMaxStep[iSlot] < 1 )
//		{
//			SET_ERRMSG("%s : S#%02d Step cout is less than 1\n", __FUNCTION__, iSlot );
//			return false;
//		}

//		std::vector<std::string> vStep;
//		vStep = m_pDataMgr->GET_STEPNAMES(iSlot, CPU00);
//		for(uint i=0; i<vStep.size(); ++i)
//		{
//			printf("%03d [%s]\n", i, vStep[i].c_str() );
//		}

		sprintf( m_szVersion[iSlot], "%s", m_stDevPgmVar[iSlot][CPU00].szVersion );
		sprintf( m_szDevice [iSlot], "%s", m_stDevPgmVar[iSlot][CPU00].szDevice  );
//		printf("%s:%d m_szDevice[%d]:[%s]\n", __FUNCTION__, __LINE__, iSlot, m_szDevice[iSlot]);

		m_nBibX         [iSlot] = m_stDevPgmVar[iSlot][CPU00].nBibX;
		m_nBibY         [iSlot] = m_stDevPgmVar[iSlot][CPU00].nBibY;
		m_nMaxDutPerCpu [iSlot] = m_stDevPgmVar[iSlot][CPU00].nMaxDutPerCpu;
		m_nMaxDut       [iSlot] = m_nBibX[iSlot] * m_nBibY[iSlot];

		m_nCePerDut     [iSlot] = m_stDevPgmVar[iSlot][CPU00].nCe;
		m_nChipPerCe    [iSlot] = m_stDevPgmVar[iSlot][CPU00].nChip;
		m_nBlockPerCe   [iSlot] = m_stDevPgmVar[iSlot][CPU00].nBlockPerCe;
		m_nBlockPerChip [iSlot] = m_nBlockPerCe[iSlot]/m_nChipPerCe[iSlot];
		m_nChipPerDut   [iSlot] = m_stDevPgmVar[iSlot][CPU00].nChipPerDut;
		m_nChipPerDutBl [iSlot] = m_stDevPgmVar[iSlot][CPU00].nChipPerDutBl;
		m_nBlockPerDutBl[iSlot] = m_stDevPgmVar[iSlot][CPU00].nBlockPerDutBl;

		if ( false )
		{
			printf("                   SLOT=%d\n", iSlot+1 );
			printf("m_nBibX         [iSlot]=%d\n", m_nBibX         [iSlot]);
			printf("m_nBibY         [iSlot]=%d\n", m_nBibY         [iSlot]);
			printf("m_nMaxDutPerCpu [iSlot]=%d\n", m_nMaxDutPerCpu [iSlot]);
			printf("m_nMaxDut       [iSlot]=%d\n", m_nMaxDut       [iSlot]);
			printf("m_nCePerDut     [iSlot]=%d\n", m_nCePerDut     [iSlot]);
			printf("m_nChipPerCe    [iSlot]=%d\n", m_nChipPerCe    [iSlot]);
			printf("m_nBlockPerCe   [iSlot]=%d\n", m_nBlockPerCe   [iSlot]);
			printf("m_nBlockPerChip [iSlot]=%d\n", m_nBlockPerChip [iSlot]);
			printf("m_nChipPerDut   [iSlot]=%d\n", m_nChipPerDut   [iSlot]);
			printf("m_nChipPerDutBl [iSlot]=%d\n", m_nChipPerDutBl [iSlot]);
			printf("m_nBlockPerDutBl[iSlot]=%d\n", m_nBlockPerDutBl[iSlot]);
		}
	}

	return true;
}

bool COrganizedData::GetMaxStep()
{
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if( !m_bValidSlot[iSlot] ) continue;

		uint nMaxStep0 = m_pDataMgr->GET_MAX_STEP( iSlot, CPU00 );
		uint nMaxStep1 = m_pDataMgr->GET_MAX_STEP( iSlot, CPU01 );
//		printf("%s:%d   MAXSTEP:%u %d\n", __FUNCTION__, __LINE__, nMaxStep0, nMaxStep1);
		if( nMaxStep0 != nMaxStep1 )
		{
			SET_ERRMSG("%s : S#%02d MaxStep[%d], CPU1 MaxStep[%d] is different\n",
				   __FUNCTION__, iSlot, nMaxStep0, nMaxStep1 );
			return false;
		}
		m_nMaxStep[iSlot] = nMaxStep0;

		//printf( "   ->MAX STEP : %d\n", m_nMaxStep );
		if( m_nMaxStep[iSlot] < 1 )
		{
			SET_ERRMSG("%s : S#%02d Step cout is less than 1\n", __FUNCTION__, iSlot );
			return false;
		}
	}

	return true;
}

bool COrganizedData::ParseRCmd(bool  bSavemap/*=false*/)
{
	bool bValidBib[32] = { false, };

//	if ( ! ParseStartBiCmd    (bValidBib          ) ) return false;
	if ( ! ParseEventIni      (bValidBib          ) ) return false;
	if ( ! GetDevPgmVar       (                   ) ) return false;
	if ( ! ParseBoardMaintInfo(bValidBib, bSavemap) ) return false;
	if ( ! ParseTgetBiOpenLot (           bSavemap) ) return false;
	if ( ! ParseVidIni        (           bSavemap) ) return false;

//	DispLOT();

	return true;
}

bool COrganizedData::ParseStartBiCmd(bool* pbValidBib)
{
///////////////////////////////////////////////////////////////////////////////////////////////
// START_BI_CMD
	QString sFile = tr("%1/%2/%3%4")
			.arg(m_sDataRoot)
			.arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
			.arg(RCMD_START_BI_CMD)
			.arg(EXT_RCMD);

	stRcmdStartBiCmd startbicmd;
	if( !m_pDataMgr->READ_FILE( sFile.toStdString().c_str(), (char*)&startbicmd, sizeof(stRcmdStartBiCmd) ) )
	{
		SET_ERRMSG( "%s : %s read error!", __FUNCTION__, sFile.toStdString().c_str() );
		return false;
	}
///////////////////////////////////////////////////////////////////////////////////////////////
	atstring sTotLotId = atstring( startbicmd.LOT_ID );
//	printf("startbicmd=%s\n", sFile.toStdString().c_str() );
//	printf("sTotLotId=%s\n", sTotLotId.c_str() );

	//TEST CODE
//	sTotLotId = "TEST_LOT_NO_1,TEST_LOT_NO_4";

	std::list<std::string> rLotList;
	sTotLotId.Split( rLotList, "," );

	ClrLotList();
//	printf("%s: rLotList size=%d\n", __FUNCTION__, rLotList.size() );
	for (std::list<std::string>::iterator it=rLotList.begin(); it != rLotList.end(); ++it)
	{
		T_LOT* pLot = new T_LOT( *it );
		m_Lot.push_back( pLot );
//		printf("%s: LOT_NO=%s\n", __FUNCTION__, pLot->LOT_NO );
	}
//	printf("m_Lot.size=%d\n", m_Lot.size() );

	for(int i=0; i<32; ++i)
	{
		pbValidBib[i] = false;
		if ( strlen(startbicmd.BOARD_ID[i]) > 0 )
			pbValidBib[i] = true;

//		if ( pbValidBib[i] ) printf("Valid Slot: %d\n", i);
	}

	return true;
}

bool COrganizedData::ParseEventIni(bool* pbValidBib)
{
	for(int iSlot=0; iSlot<32; ++iSlot)
		pbValidBib[iSlot] = false;

	QString	sDataPath;
	if ( m_pDataMgr->GET_UIAUTOMODE() ) sDataPath = DIR_AUTO_DATA_PATH;
	else                                sDataPath = DIR_ENGR_DATA_PATH;

	QString ini_file( tr("%1/%2/%3").arg(m_sDataRoot).arg(sDataPath).arg(EVENT_INI));
	QString sValue;

	//SLOT_WITH_BIB
	if ( ! UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, sValue) )
	{
		SET_ERRMSG( "%s : %s read(SLOT_WITH_BIB) error!", __FUNCTION__, ini_file.toStdString().c_str() );
		return false;
	}
	bool bOk = false;
	int iSlotWithBib = sValue.toInt(&bOk, 16);
	if ( ! bOk ) iSlotWithBib = 0;

	for(int iSlot=0; iSlot<32; ++iSlot)
	{
		if ( (iSlotWithBib>>iSlot)&1 )
			pbValidBib[iSlot] = true;
//		else
//			pbValidBib[iSlot] = false;
	}

//	printf("%s: iSlotWithBib: 0x%04X\n", __FUNCTION__, iSlotWithBib );
	for(int iSlot=0; iSlot<MAX_SLOT; ++iSlot)
		m_bValidSlot[iSlot] = pbValidBib[iSlot];

	return true;
}

bool COrganizedData::ParseBoardMaintInfo(bool* pbValidBib, bool bSavemap/*=false*/)
{
	std::list<std::string> lstTotLot;

	lstTotLot.clear();
//	for (int iSlot=0; iSlot<32      ; ++iSlot)
	for (int iSlot=0; iSlot<MAX_SLOT; ++iSlot)
	{
//		if ( ! pbValidBib[iSlot] ) continue;
		if( !m_bValidSlot[iSlot] ) continue;

		string strBibIdEngr = "";
		if ( ! m_pDataMgr->GET_UIAUTOMODE() )
		{
			QString ini_file( tr("%1/%2/SLOT%3/%4").arg(m_sDataRoot).arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH).arg(iSlot).arg(BIBID_TXT_NAME));

			//BIBID
			char szSection[PATH_MAX];
			sprintf(szSection, "%s/%s", SECTION_BIBID, KEY_BIBID_ID);
			if ( ! UTIL::INI::readSettings(ini_file, szSection, strBibIdEngr) )
			{
				SET_ERRMSG( "%s : %s read(BIB_ID) error!", __FUNCTION__, ini_file.toStdString().c_str() );
				return false;
			}

			if ( false )
			{
				printf("M BIB_ID F:%s  ID:%s\n", ini_file.toStdString().c_str(), strBibIdEngr.c_str() );
			}
		}
///////////////////////////////////////////////////////////////////////////////////////////////
// BOARD_MAINT_INFO - Slot
		QString sFile = tr("%1/%2/%3/SLOT%4%5")
					.arg(m_sDataRoot)
					.arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
					.arg(BOARD_MAINT_DIR)
					.arg(iSlot)
					.arg(EXT_RCMD);

		stRcmdBoardMaintInfo BoardMaintInfo;
		if( !m_pDataMgr->READ_FILE( sFile.toStdString().c_str(), (char*)&BoardMaintInfo, sizeof(stRcmdBoardMaintInfo) ) )
		{
			SET_ERRMSG( "%s : %s read error!", __FUNCTION__, sFile.toStdString().c_str() );
			return false;
		}
///////////////////////////////////////////////////////////////////////////////////////////////
//		printf("BoardMaintInfo=%s\n", sFile.toStdString().c_str() );
//		printf("RETURN_FLAG=%s\n", BoardMaintInfo.RETURN_FLAG );
		if ( strcmp(BoardMaintInfo.RETURN_FLAG, "OK")!=0 )
		{
			SET_ERRMSG( "%s : %s RETURN_FLAG(%s) error!", __FUNCTION__, sFile.toStdString().c_str(), BoardMaintInfo.RETURN_FLAG );
			continue;
		}

		atstring sMlbLotId        = atstring( BoardMaintInfo.MLB_LOT_LIST     );
		atstring sLotMap          = atstring( BoardMaintInfo.LOT_MAP          );
		atstring sInsertMountMap  = atstring( BoardMaintInfo.INSERT_MOUNT_MAP );
		atstring sInitialMaskMap  = atstring( BoardMaintInfo.INITIAL_MASK_MAP );
		atstring sManualMaskMap   = atstring( BoardMaintInfo.MANUAL_MASK_MAP  );
		atstring sFunctionMaskMap = atstring( BoardMaintInfo.MANUAL_MASK_MAP  );

		if ( false && iSlot==0)
		{
			printf("          iSlot =%d\n", iSlot                           );
			printf("sMlbLotId       =%s\n", BoardMaintInfo.MLB_LOT_LIST     );
			printf("sLotMap         =%s\n", BoardMaintInfo.LOT_MAP          );
			printf("sInsertMountMap =%s\n", BoardMaintInfo.INSERT_MOUNT_MAP );
			printf("sInitialMaskMap =%s\n", BoardMaintInfo.INITIAL_MASK_MAP );
			printf("sManualMaskMap  =%s\n", BoardMaintInfo.MANUAL_MASK_MAP  );
			printf("sFunctionMaskMap=%s\n", BoardMaintInfo.MANUAL_MASK_MAP  );
		}

		std::list<std::string> rMlbLotList;
		std::list<std::string> rLotMapList;
		sMlbLotId.Split( rMlbLotList, "," );
//		sLotMap.Split  ( rLotMapList, "," );
////////////////////////////////////////////////////////////////////////////
//		if (iSlot==0 )
		{
			rLotMapList.clear();
			uint nMaxPos = sLotMap.size();
			uint iPos = 0;
//			uint iDut=0;
			while(true)
			{
				string::size_type iFind = sLotMap.find(',', iPos);
				if ( iFind == string::npos )
				{
					if ( iPos<=nMaxPos ) iFind = nMaxPos;
					else                 break;
				}

				uint nLen = iFind - iPos;
//				printf("A DUT%03d iPos = %d   max=%d   Len=%d   ", iDut+1, iPos, nMaxPos, nLen );
				string strLot = sLotMap.substr(iPos, nLen);
//				printf("[%s]\n", strLot.c_str() );
				rLotMapList.push_back(strLot);
//				++iDut;

				iPos = iFind+1;
				if ( iPos==nMaxPos )
				{
					strLot = "";
//					printf("B DUT%03d iPos = %d   max=%d   Len=%d   ", iDut+1, iPos, nMaxPos, nLen );
//					printf("[%s]\n", strLot.c_str() );
					rLotMapList.push_back(strLot);
//					++iDut;
				}
				if ( iPos>=nMaxPos )
					break;
			}
		}
//		else
//		{
//			rLotMapList.clear();
//			sLotMap.Split  ( rLotMapList, "," );
//		}

		if ( false && iSlot==0)
		{
			int iDut=0;
			printf("rLotMapList size=%d\n", rLotMapList.size() );
			for (std::list<std::string>::iterator itLotMap=rLotMapList.begin(); itLotMap != rLotMapList.end(); ++itLotMap, ++iDut)
			{
				string sLotMap = *itLotMap;
				printf("DUT%03d = [%s]\n", iDut+1, sLotMap.c_str());
			}
		}

////////////////////////////////////////////////////////////////////////////

		for (std::list<std::string>::iterator itMlbLot=rMlbLotList.begin(); itMlbLot != rMlbLotList.end(); ++itMlbLot)
		{
			string sMlbLot = *itMlbLot;

			if ( find(lstTotLot.begin(), lstTotLot.end(), sMlbLot) == lstTotLot.end() )
			{
				T_LOT* pLot = new T_LOT( sMlbLot );
				m_Lot.push_back( pLot );

				lstTotLot.push_back(sMlbLot);
			}

			for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
			{
				T_LOT* pLot = *itTotLot;
//				printf("sMlbLot=%s   LOT_NO=%s\n", sMlbLot.c_str(), pLot->LOT_NO);
				if ( sMlbLot.compare(pLot->LOT_NO)!=0 ) continue;
				if ( pLot->S[iSlot].IS_VALID()        ) continue;

						 pLot->S[iSlot].SET_VALID( true );

				if ( m_pDataMgr->GET_UIAUTOMODE() )
				{
					sprintf( pLot->S[iSlot].BIB_ID  , "%s", BoardMaintInfo.BIB_ID     );
							 pLot->S[iSlot].BIB_X   = atoi( BoardMaintInfo.BIB_X_SIZE );
							 pLot->S[iSlot].BIB_Y   = atoi( BoardMaintInfo.BIB_Y_SIZE );
					sprintf( pLot->S[iSlot].MLB_CODE, "%s", BoardMaintInfo.MLB_CODE   );
				}
				else
				{
				   sprintf( pLot->S[iSlot].BIB_ID  , "%s", strBibIdEngr.c_str()   );
							pLot->S[iSlot].BIB_X   = m_nBibX[iSlot];
							pLot->S[iSlot].BIB_Y   = m_nBibY[iSlot];
				   sprintf( pLot->S[iSlot].MLB_CODE, "%s", "SLB"   );
				}

//				sprintf( pLot->S[iSlot].IOC     , "%s", BoardMaintInfo.IOC        );
//				sprintf( pLot->S[iSlot].BOC     , "%s", BoardMaintInfo.BOC        );
				sprintf( pLot->S[iSlot].IOC     , "%s", "0"                       );
				sprintf( pLot->S[iSlot].BOC     , "%s", "0"                       );

				//if ( bSavemap )
				//{
				//	m_nMaxDut      [iSlot] = pLot->S[iSlot].BIB_X * pLot->S[iSlot].BIB_Y;
				//	m_nMaxDutPerCpu[iSlot] = m_nMaxDut[iSlot] / 2;
				//}

				if ( pLot->S[iSlot].sdut != nullptr ) { delete [] pLot->S[iSlot].sdut; pLot->S[iSlot].sdut = nullptr; }
				pLot->S[iSlot].sdut = new bool[pLot->S[iSlot].BIB_X * pLot->S[iSlot].BIB_Y];
				pLot->S[iSlot].ldut = new bool[pLot->S[iSlot].BIB_X * pLot->S[iSlot].BIB_Y];
				pLot->S[iSlot].mdut = new bool[pLot->S[iSlot].BIB_X * pLot->S[iSlot].BIB_Y];
				memset(pLot->S[iSlot].sdut, 0, sizeof(bool) * (pLot->S[iSlot].BIB_X) * (pLot->S[iSlot].BIB_Y) );
				memset(pLot->S[iSlot].ldut, 0, sizeof(bool) * (pLot->S[iSlot].BIB_X) * (pLot->S[iSlot].BIB_Y) );
				memset(pLot->S[iSlot].mdut, 0, sizeof(bool) * (pLot->S[iSlot].BIB_X) * (pLot->S[iSlot].BIB_Y) );

				pLot->S[iSlot].nInQty = 0;
				int iDut=0;
//				printf("rLotMapList size=%d\n", rLotMapList.size() );
				for (std::list<std::string>::iterator itLotMap=rLotMapList.begin(); itLotMap != rLotMapList.end(); ++itLotMap, ++iDut)
				{
					string sLotMap = *itLotMap;
					//if ( sLotMap.compare(pLot->LOT_NO) == 0 ) { pLot->S[iSlot].sdut[iDut] = true ; pLot->S[iSlot].nInQty++; }
					//else                                      { pLot->S[iSlot].sdut[iDut] = false; continue;                }
					//
					//if ( pLot->S[iSlot].sdut[iDut] && sInsertMountMap [iDut] != '1' ) pLot->S[iSlot].sdut[iDut] = false;
					//if ( pLot->S[iSlot].sdut[iDut] && sInitialMaskMap [iDut] != '1' ) pLot->S[iSlot].sdut[iDut] = false;
					//if ( pLot->S[iSlot].sdut[iDut] && sManualMaskMap  [iDut] != '1' ) pLot->S[iSlot].sdut[iDut] = false;
					//if ( pLot->S[iSlot].sdut[iDut] && sFunctionMaskMap[iDut] != '1' ) pLot->S[iSlot].sdut[iDut] = false;

					if ( sLotMap.compare(pLot->LOT_NO) == 0 &&
						 sInsertMountMap [iDut] == '1'      &&
						 sInitialMaskMap [iDut] == '1'      &&
						 sManualMaskMap  [iDut] == '1'      &&
						 sFunctionMaskMap[iDut] == '1'         )
					{
						pLot->S[iSlot].sdut[iDut] = true;
						pLot->S[iSlot].nInQty++;
					}
					else
					{
						pLot->S[iSlot].sdut[iDut] = false;
					}

					if ( sLotMap.compare(pLot->LOT_NO) == 0 ) pLot->S[iSlot].ldut[iDut] = true ;
					else                                      pLot->S[iSlot].ldut[iDut] = false;

					if ( pLot->S[iSlot].ldut[iDut] )
					{
//						if ( sInsertMountMap [iDut] == '1' ||
						if ( sInitialMaskMap [iDut] != '1' ||
							 sManualMaskMap  [iDut] != '1' ||
							 sFunctionMaskMap[iDut] != '1'   )
							pLot->S[iSlot].mdut[iDut] = true;
						else
							pLot->S[iSlot].mdut[iDut] = false;
					}
					else
						pLot->S[iSlot].mdut[iDut] = false;

//					if (strcmp(pLot->LOT_NO, "4QQLF17QAEN2")==0 )
//					{
//						printf("S%02d DUT%03d = ", iSlot+1, iDut+1);
//						for(int i=0; i<pLot->S[iSlot].BIB_X*pLot->S[iSlot].BIB_Y; ++i)
//							printf("%c", pLot->S[iSlot].sdut[i] ? '1' : '0');
//						printf("\n");
//					}
				}

				if ( false )
				{
					printf("SLOT=%d\n"    , iSlot+1 );
					printf("Valid=%s\n"   , pLot->S[iSlot].IS_VALID() ? "O" : "X");
					printf("LOT_NO=%s\n"  , pLot->LOT_NO );
					printf("BIB_ID=%s\n"  , pLot->S[iSlot].BIB_ID   );
					printf("BIB_X=%d\n"   , pLot->S[iSlot].BIB_X    );
					printf("BIB_Y=%d\n"   , pLot->S[iSlot].BIB_Y    );
					printf("MLB_CODE=%s\n", pLot->S[iSlot].MLB_CODE );
					printf("IOC=%s\n"     , pLot->S[iSlot].IOC      );
					printf("BOC=%s\n"     , pLot->S[iSlot].BOC      );
					printf("nInQty=%d\n"  , pLot->S[iSlot].nInQty   );

					printf("SDUT=" );
					for(int i=0; i<pLot->S[iSlot].BIB_X*pLot->S[iSlot].BIB_Y; ++i)
						printf("%c", pLot->S[iSlot].sdut[i] ? '1' : '0');
					printf("\n");
					printf("\n");
				}

//				if ( pLot->RS == nullptr ) pLot->RS = &(pLot->S[iSlot]);
			} //End of TotLot
		} //End of MlbLot(slot)
	} //End of iSlot
	lstTotLot.clear();

	return true;
}

bool COrganizedData::ParseTgetBiOpenLot(bool  bSavemap/*=false*/)
{
	for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

///////////////////////////////////////////////////////////////////////////////////////////////
// TGET - LOT
		QString sFile = tr("%1/%2/%3/%4%5")
					.arg(m_sDataRoot)
					.arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
					.arg(TGET_DIR)
					.arg(pLot->LOT_NO)
					.arg(EXT_RCMD);

		stRcmdTGET TGet;
		if( !m_pDataMgr->READ_FILE( sFile.toStdString().c_str(), (char*)&TGet, sizeof(stRcmdTGET) ) )
		{
			SET_ERRMSG( "%s : %s read error!", __FUNCTION__, sFile.toStdString().c_str() );
			return false;
		}
///////////////////////////////////////////////////////////////////////////////////////////////
		sprintf( pLot->PART_NO    , "%s", TGet.PART_NO     );
		sprintf( pLot->TEST_PGM_NO, "%s", TGet.TEST_PGM_NO );
		sprintf( pLot->DEVICE_NO  , "%s", TGet.DEVICE_NO    );
		sprintf( pLot->OPER       , "%s", TGet.OPER        );
		sprintf( pLot->FAB        , "%s", TGet.FAB         );
		sprintf( pLot->GRADE      , "%s", TGet.GRADE       );
		sprintf( pLot->OWNER      , "%s", TGet.OWNER       );
		sprintf( pLot->FACTORY    , "%s", TGet.FACTORY     );
				 pLot->RWK_COUNT  = atoi( TGet.RWK_COUNT   );
		sprintf( pLot->RWK_CODE   , "%s", TGet.RWK_CODE    );

	if ( ! m_pDataMgr->GET_UIAUTOMODE() )
	{
		int iRefS = -1;
		for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( pLot->S[iSlot].IS_VALID()  )
			{
				iRefS = iSlot;
				break;
			}
		}
		if ( iRefS < 0 )
		{
			SET_ERRMSG( "%s : LOT(%s) reference slot(%d) not found!", __FUNCTION__, pLot->LOT_NO, iRefS );
			return false;
		}

		sprintf( pLot->TEST_PGM_NO, "%s", m_szDevice[iRefS] );
	}

//	printf("%s:%d TEST_PGM_NO:[%s]\n", __FUNCTION__, __LINE__, pLot->TEST_PGM_NO);
///////////////////////////////////////////////////////////////////////////////////////////////
// BI_OPEN_LOT - LOT
		if ( ! bSavemap )
		{
			if ( m_pDataMgr->GET_UIAUTOMODE() )
			{
				sFile = tr("%1/%2/%3/%4%5")
							.arg(m_sDataRoot)
							.arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
							.arg(BI_OPEN_LOT_DIR)
							.arg(pLot->LOT_NO)
							.arg(EXT_RCMD);

				stRcmdBiOpenLot BiOpenLot;
				if( !m_pDataMgr->READ_FILE( sFile.toStdString().c_str(), (char*)&BiOpenLot, sizeof(stRcmdBiOpenLot) ) )
				{
					SET_ERRMSG( "%s : %s read error!", __FUNCTION__, sFile.toStdString().c_str() );
					return false;
				}
				pLot->SNO = atoi( BiOpenLot.SNO );
			}
			else
				pLot->SNO = 0;
		}
///////////////////////////////////////////////////////////////////////////////////////////////

		if ( false )
		{
			printf("%s: LOT_NO      = %s\n", __FUNCTION__, pLot->LOT_NO      );
			printf("%s: SNO         = %d\n", __FUNCTION__, pLot->SNO         );
			printf("%s: PART_NO     = %s\n", __FUNCTION__, pLot->PART_NO     );
			printf("%s: TEST_PGM_NO = %s\n", __FUNCTION__, pLot->TEST_PGM_NO );
			printf("%s: DEVICE_NO   = %s\n", __FUNCTION__, pLot->DEVICE_NO   );
			printf("%s: OPER        = %s\n", __FUNCTION__, pLot->OPER        );
			printf("%s: FAB         = %s\n", __FUNCTION__, pLot->FAB         );
			printf("%s: GRADE       = %s\n", __FUNCTION__, pLot->GRADE       );
			printf("%s: OWNER       = %s\n", __FUNCTION__, pLot->OWNER       );
			printf("%s: FACTORY     = %s\n", __FUNCTION__, pLot->FACTORY     );
			printf("%s: RWK_COUNT   = %d\n", __FUNCTION__, pLot->RWK_COUNT   );
			printf("%s: RWK_CODE    = %s\n", __FUNCTION__, pLot->RWK_CODE    );
			printf("\n");
		}
	}

	return true;
}

bool COrganizedData::ParseVidIni(bool  bSavemap/*=false*/)
{
	if ( bSavemap ) return true;

	char cMpgFlag;
	char szOP_ID[32];

	if ( m_pDataMgr->GET_UIAUTOMODE() )
	{
		QString ini_file( tr("%1/%2/%3").arg(m_sDataRoot).arg(DIR_AUTO_DATA_PATH).arg(VID_VALUE_INI));
		atstring sValue("");

		//MPG_FLAG
		if ( ! UTIL::INI::readSettings(ini_file, QString::number(7001), sValue) )
		{
			SET_ERRMSG( "%s : %s read(MPG_FLAG) error!", __FUNCTION__, ini_file.toStdString().c_str() );
			return false;
		}
		cMpgFlag = sValue[0];

		//OP_ID
		if ( ! UTIL::INI::readSettings(ini_file, QString::number(1120), sValue) )
		{
			SET_ERRMSG( "%s : %s read(OP_ID) error!", __FUNCTION__, ini_file.toStdString().c_str() );
			return false;
		}
		sprintf( szOP_ID, "%s", sValue.c_str() );
	}
	else
	{
		cMpgFlag = 'N';
		sprintf( szOP_ID, "%s", "EIS" );
	}

	for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

				 pLot->MPG_FLAG   = cMpgFlag;
		sprintf( pLot->OP_ID, "%s", szOP_ID );

		if ( false )
		{
			printf("%s: LOT_NO      = %s\n", __FUNCTION__, pLot->LOT_NO      );
			printf("%s: SNO         = %d\n", __FUNCTION__, pLot->SNO         );
			printf("%s: PART_NO     = %s\n", __FUNCTION__, pLot->PART_NO     );
			printf("%s: TEST_PGM_NO = %s\n", __FUNCTION__, pLot->TEST_PGM_NO );
			printf("%s: DEVICE_NO   = %s\n", __FUNCTION__, pLot->DEVICE_NO   );
			printf("%s: OPER        = %s\n", __FUNCTION__, pLot->OPER        );
			printf("%s: FAB         = %s\n", __FUNCTION__, pLot->FAB         );
			printf("%s: GRADE       = %s\n", __FUNCTION__, pLot->GRADE       );
			printf("%s: OWNER       = %s\n", __FUNCTION__, pLot->OWNER       );
			printf("%s: FACTORY     = %s\n", __FUNCTION__, pLot->FACTORY     );
			printf("%s: RWK_COUNT   = %d\n", __FUNCTION__, pLot->RWK_COUNT   );
			printf("%s: RWK_CODE    = %s\n", __FUNCTION__, pLot->RWK_CODE    );
			printf("%s: MPG_FLAG    = %s\n", __FUNCTION__, pLot->MPG_FLAG    );
			printf("%s: OP_ID       = %s\n", __FUNCTION__, pLot->OP_ID       );
			printf("\n");
		}
	}

	return true;
}

void COrganizedData::DispLOT()
{
	for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		printf("%s: LOT_NO      = %s\n", __FUNCTION__, pLot->LOT_NO      );
		printf("%s: SNO         = %d\n", __FUNCTION__, pLot->SNO         );
		printf("%s: PART_NO     = %s\n", __FUNCTION__, pLot->PART_NO     );
		printf("%s: TEST_PGM_NO = %s\n", __FUNCTION__, pLot->TEST_PGM_NO );
		printf("%s: DEVICE_NO   = %s\n", __FUNCTION__, pLot->DEVICE_NO   );
		printf("%s: OPER        = %s\n", __FUNCTION__, pLot->OPER        );
		printf("%s: FAB         = %s\n", __FUNCTION__, pLot->FAB         );
		printf("%s: GRADE       = %s\n", __FUNCTION__, pLot->GRADE       );
		printf("%s: OWNER       = %s\n", __FUNCTION__, pLot->OWNER       );
		printf("%s: FACTORY     = %s\n", __FUNCTION__, pLot->FACTORY     );
		printf("%s: RWK_COUNT   = %d\n", __FUNCTION__, pLot->RWK_COUNT   );
		printf("%s: RWK_CODE    = %s\n", __FUNCTION__, pLot->RWK_CODE    );
		printf("%s: MPG_FLAG    = %s\n", __FUNCTION__, pLot->RWK_CODE    );
		printf("%s: RWK_CODE    = %s\n", __FUNCTION__, pLot->RWK_CODE    );

		for(int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

			printf("\n");
			printf("%s: SLOT=%d\n"    , __FUNCTION__,         iSlot+1         );
			printf("%s: BIB_ID=%s\n"  , __FUNCTION__, pLot->S[iSlot].BIB_ID   );
			printf("%s: BIB_X=%d\n"   , __FUNCTION__, pLot->S[iSlot].BIB_X    );
			printf("%s: BIB_Y=%d\n"   , __FUNCTION__, pLot->S[iSlot].BIB_Y    );
			printf("%s: MLB_CODE=%s\n", __FUNCTION__, pLot->S[iSlot].MLB_CODE );
			printf("%s: IOC=%s\n"     , __FUNCTION__, pLot->S[iSlot].IOC      );
			printf("%s: BOC=%s\n"     , __FUNCTION__, pLot->S[iSlot].BOC      );
			printf("%s: nInQty=%d\n"  , __FUNCTION__, pLot->S[iSlot].nInQty   );

			printf("%s: SDUT="        , __FUNCTION__ );
			for(int i=0; i<pLot->S[iSlot].BIB_X*pLot->S[iSlot].BIB_Y; ++i)
			{
				printf("%c", pLot->S[iSlot].sdut[i] ? '1' : '0');
				if ( i==95 ) printf("\n              ");
			}
			printf("\n");
		}
		printf("\n");

		for(int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

			printf("                   SLOT=%d\n", iSlot+1 );
			printf("m_nBibX         [iSlot]=%d\n", m_nBibX         [iSlot]);
			printf("m_nBibY         [iSlot]=%d\n", m_nBibY         [iSlot]);
			printf("m_nMaxDutPerCpu [iSlot]=%d\n", m_nMaxDutPerCpu [iSlot]);
			printf("m_nMaxDut       [iSlot]=%d\n", m_nMaxDut       [iSlot]);
			printf("m_nCePerDut     [iSlot]=%d\n", m_nCePerDut     [iSlot]);
			printf("m_nChipPerCe    [iSlot]=%d\n", m_nChipPerCe    [iSlot]);
			printf("m_nBlockPerCe   [iSlot]=%d\n", m_nBlockPerCe   [iSlot]);
			printf("m_nBlockPerChip [iSlot]=%d\n", m_nBlockPerChip [iSlot]);
			printf("m_nChipPerDut   [iSlot]=%d\n", m_nChipPerDut   [iSlot]);
			printf("m_nChipPerDutBl [iSlot]=%d\n", m_nChipPerDutBl [iSlot]);
			printf("m_nBlockPerDutBl[iSlot]=%d\n", m_nBlockPerDutBl[iSlot]);
		}
		printf("\n");
		printf("#################################################################\n");
	}
}

bool COrganizedData::MkReportDir(bool  bSavemap/*=false*/)
{
	fs::path pr( m_szReportRoot );
	if( !fs::exists(pr) )
	{
		SET_ERRMSG("%s : Report directory '%s' does not exist!", __FUNCTION__, m_szReportRoot );
		return false;
	}

	for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

///////////////////////////////////////////////////////////////////////////////////////////////////
// PART_NO
		snprintf( pLot->m_szReportDir, PATH_MAX, "%s/%s", m_szReportRoot, pLot->PART_NO );
		fs::path pr( pLot->m_szReportDir );
		if( !fs::exists(pr) )
		{
			bool bOk = fs::create_directories(pr);
			if( !bOk )
			{
				SET_ERRMSG("%s : Make directory '%s' error!", __FUNCTION__, pLot->m_szReportDir );
				return false;
			}
		}
///////////////////////////////////////////////////////////////////////////////////////////////////
// LOT_NO
		string sReportDir = pLot->m_szReportDir;
		snprintf( pLot->m_szReportDir, PATH_MAX, "%s/%s", sReportDir.c_str(), pLot->LOT_NO );
		pr = pLot->m_szReportDir;
		if( !fs::exists(pr) )
		{
			bool bOk = fs::create_directories(pr);
			if( !bOk )
			{
				SET_ERRMSG("%s : Make directory '%s' error!", __FUNCTION__, pLot->m_szReportDir );
				return false;
			}
		}
///////////////////////////////////////////////////////////////////////////////////////////////////
		if ( bSavemap )
		{
// SaveMap
			sReportDir = pLot->m_szReportDir;
			snprintf( pLot->m_szReportDir, PATH_MAX, "%s/%s", sReportDir.c_str(), DIR_SAVEMAP_PATH );
			pr = pLot->m_szReportDir;
			if( !fs::exists(pr) )
			{
				bool bOk = fs::create_directories(pr);
				if( !bOk )
				{
					SET_ERRMSG("%s : Make directory '%s' error!", __FUNCTION__, pLot->m_szReportDir );
					return false;
				}
			}
//			printf("%s: LOT=%s   m_szReportDir=%s\n", __FUNCTION__, pLot->LOT_NO, pLot->m_szReportDir );
		}
		else
		{
// SUB_LOT
			sReportDir = pLot->m_szReportDir;
			snprintf( pLot->m_szReportDir, PATH_MAX, "%s/%d", sReportDir.c_str(), pLot->SNO );
			pr = pLot->m_szReportDir;
			if( fs::exists(pr) )
			{
				//fs::remove_all( pLot->m_szReportDir + string("/*") );
				char szCmd[PATH_MAX];
				sprintf(szCmd, "rm -f %s/*", pLot->m_szReportDir );
				system( szCmd );
//				printf("%s: %s\n", __FUNCTION__, szCmd);
			}
			else
			{
				bool bOk = fs::create_directories(pr);
				if( !bOk )
				{
					SET_ERRMSG("%s : Make directory '%s' error!", __FUNCTION__, pLot->m_szReportDir );
					return false;
				}
			}
//			printf("%s: LOT=%s   m_szReportDir=%s\n", __FUNCTION__, pLot->LOT_NO, pLot->m_szReportDir );
		}
///////////////////////////////////////////////////////////////////////////////////////////////////
	}

	return true;
}

bool COrganizedData::GatherFoundationData()
{
	if ( ! GetRefSlotCpu() ) return false;
//	if ( ! GetDevPgmVar()  ) return false;
	if ( ! GetMaxStep  ()  ) return false;

	for( int iSlot=0; iSlot<MAX_SLOT; iSlot++ )
	{
		if( !m_bValidSlot[iSlot]                    ) continue;
		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(iSlot) ) continue;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// INIT FAIL, InitTestHead
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		stInitTestHead initHead[MAX_SYS_CPU];
		m_pInitFail[iSlot] = new char[ m_nMaxDut[iSlot] ];
		if ( m_pInitFail[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d FLS Memory(Init Fail) alloc error\n", __FUNCTION__, iSlot+1 );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_INITTEST_DATA( iSlot, iCpu, initHead[iCpu], m_pInitFail[iSlot]+iCpu*m_nMaxDutPerCpu[iSlot], sizeof(char)*m_nMaxDutPerCpu[iSlot]) )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot+1, iCpu+1, m_pDataMgr->GET_ERRMSG() );
				return false;
			}

			m_initTestHead[iSlot][iCpu].nStepNo = initHead[iCpu].nStepNo;
			sprintf( m_initTestHead[iSlot][iCpu].szStepName, "%s", initHead[iCpu].szStepName );

			m_initTestHead[iSlot][iCpu].tInitBegin = initHead[iCpu].tInitBegin;
			m_initTestHead[iSlot][iCpu].tInitEnd   = initHead[iCpu].tInitEnd  ;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP FAIL
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//printf( " ->PRE_FLS STEP FAIL DATA\n" );
		uint nEachFailSize = sizeof(char) * m_nMaxDutPerCpu[iSlot];
		m_pStepFail[iSlot] = new char[ m_nMaxDut[iSlot]*m_nMaxStep[iSlot] ];
		if ( m_pStepFail[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Fail) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPFAIL_DATA(iSlot, iCpu, m_pStepFail[iSlot], m_nMaxStep[iSlot], nEachFailSize) )
			{
				SET_ERRMSG("%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// SORT BIN
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//printf( " ->PRE_FLS SORT BIN DATA\n" );
		m_pSortBin[iSlot]  = new uint[ m_nMaxDut[iSlot] ];
		if ( m_pSortBin[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Sort Bin) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			uint iOffset = iCpu*m_nMaxDutPerCpu[iSlot];
			if( !m_pDataMgr->GET_SORTBIN_DATA(iSlot, iCpu, m_pSortBin[iSlot] + iOffset, sizeof(int)*m_nMaxDutPerCpu[iSlot]) )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// BINMAP DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if( !m_pDataMgr->GET_BINMAP_DATA(iSlot, CPU00, m_Binmap[iSlot], sizeof(uint)*16) )
		{
			SET_ERRMSG( "%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot, CPU00, m_pDataMgr->GET_ERRMSG() );
			return false;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// SortBinCnt, SortBinHCnt
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for ( LOT_LIST::iterator itTotLot = m_Lot.begin(); itTotLot != m_Lot.end(); ++ itTotLot )
		{
			T_LOT* pLot = *itTotLot;

			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

			memset( pLot->S[iSlot].nSortBinCnt , 0x0, sizeof(uint)*17 );
			memset( pLot->S[iSlot].nSortBinHCnt, 0x0, sizeof(uint)*17 );

			for( uint iDut=0; iDut<m_nMaxDut[iSlot]; ++iDut )
			{
				if( ! pLot->S[iSlot].sdut[iDut]     ) continue;
				if(   m_pInitFail[iSlot][iDut]      ) continue;
				if(   m_pSortBin [iSlot][iDut] > 16 ) continue;

				pLot->S[iSlot].nSortBinCnt [ m_Binmap[iSlot][m_pSortBin[iSlot][iDut]] ]++;	//fls
				pLot->S[iSlot].nSortBinHCnt[                 m_pSortBin[iSlot][iDut]  ]++;	//flsh
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP INFO(COUNT/BEGIN/END/TEMP/BITIME...)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//printf( " ->PRE_FLS STEP IN/OUT DATA\n" );
		//printf( "   ->MAX STEP : %d\n", m_nMaxStep[iSlot] );
		if( m_nMaxStep[iSlot] < 1 )
		{
			SET_ERRMSG("%s : S#%02d MaxStep is less than 1\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			m_pStepInInfo [iSlot][iCpu] = new stStepInInfo [m_nMaxStep[iSlot]];
			if ( m_pStepInInfo[iSlot][iCpu] == nullptr )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d Memory(Step In Info) alloc error\n", __FUNCTION__, iSlot, iCpu );
				return false;
			}

			m_pStepOutInfo[iSlot][iCpu] = new stStepOutInfo[m_nMaxStep[iSlot]];
			if ( m_pStepOutInfo[iSlot][iCpu] == nullptr )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d Memory(Step In Out) alloc error\n", __FUNCTION__, iSlot, iCpu );
				return false;
			}

			if( !m_pDataMgr->GET_STEPIN_INFO(iSlot, iCpu, m_pStepInInfo[iSlot][iCpu], m_nMaxStep[iSlot] ) )
			{
				SET_ERRMSG("%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}

			if( !m_pDataMgr->GET_STEPOUT_INFO(iSlot, iCpu, m_pStepOutInfo[iSlot][iCpu], m_nMaxStep[iSlot] ) )
			{
				SET_ERRMSG("%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			for(uint i=0; i<m_vecStepData[iSlot][iCpu].size(); ++i )
				delete m_vecStepData[iSlot][iCpu][i];
			m_vecStepData[iSlot][iCpu].clear();
		}

		uint iTestStep = 1;
		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			CStepData* pStepInit = new CStepData;
			if ( pStepInit == nullptr )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d Memory(Init Step Data) alloc error\n", __FUNCTION__, iSlot, iCpu );
				return false;
			}

					 pStepInit->iTsNo           = iTestStep;
					 pStepInit->tStepBegin      = m_initTestHead[iSlot][iCpu].tInitBegin  ;
					 pStepInit->tStepEnd        = m_initTestHead[iSlot][iCpu].tInitEnd    ;
			sprintf( pStepInit->szStepName, "%s" ,m_initTestHead[iSlot][iCpu].szStepName );
			sprintf( pStepInit->szPc      , "%s" ,m_initTestHead[iSlot][iCpu].szStepName );

			m_vecStepData[iSlot][iCpu].push_back( pStepInit );
		}
		iTestStep++;

		for( uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep )
		{
			for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
			{
				CStepData* pStepData = new CStepData;
				if ( pStepData == nullptr )
				{
					SET_ERRMSG( "%s : S#%02d-C#%02d Memory(Step Data) alloc error\n", __FUNCTION__, iSlot, iCpu );
					return false;
				}

						 pStepData->iTsNo           = iTestStep;
						 pStepData->tStepBegin      = m_pStepInInfo [iSlot][iCpu][iStep].tStepBegin  ;
						 pStepData->tStepEnd        = m_pStepOutInfo[iSlot][iCpu][iStep].tStepEnd    ;
						 pStepData->dTemp           = m_pStepOutInfo[iSlot][iCpu][iStep].dTemp       ;
						 pStepData->dBiTime         = m_pStepOutInfo[iSlot][iCpu][iStep].dBiTime     ;
						 pStepData->dVcc            = m_pStepOutInfo[iSlot][iCpu][iStep].dVcc        ;
				sprintf( pStepData->szStepName, "%s", m_pStepOutInfo[iSlot][iCpu][iStep].szStepName );
				sprintf( pStepData->szPc      , "%s", m_pStepOutInfo[iSlot][iCpu][iStep].szPc       );

				if( 0 )
				{
					struct tm* tBegin;
					struct tm* tEnd  ;
					tBegin = localtime( &pStepData->tStepBegin );
					tEnd   = localtime( &pStepData->tStepEnd   );
					printf( "   ->STEP.%03d : %s\n", pStepData->iTsNo, pStepData->szStepName );
					printf( "   ->STEP.%03d BEGIN: %04d/%02d/%02d %02d:%02d:%02d\n",
							pStepData->iTsNo, tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec );
					printf( "   ->STEP.%03d END  : %04d/%02d/%02d %02d:%02d:%02d\n",
							pStepData->iTsNo, tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
				}
//printf("m_vecStepData[%d][%d]  %s\n", iSlot, iCpu, pStepData->szStepName);
				m_vecStepData[iSlot][iCpu].push_back( pStepData );
			}
			iTestStep++;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// CHIP ID DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//printf("S%02d    m_nMaxDut=%d   m_nChipPerDut=%d\n", iSlot, m_nMaxDut[iSlot], m_nChipPerDut[iSlot] );
		uint nChipDataSize  = m_nMaxDut      [iSlot] * m_nChipPerDut[iSlot];
		uint nDutPerCpuSize = m_nMaxDutPerCpu[iSlot] * m_nChipPerDut[iSlot];
		uint nChipIdSize    = sizeof(stChipId);
		m_pstChipId[iSlot] = new stChipId[ nChipDataSize ];
//		printf("S%02d  m_pstChipId[iSlot]=0x%x\n", iSlot, m_pstChipId[iSlot]);
		if ( m_pstChipId[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Chip ID data) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_CHIPID_DATA(iSlot, iCpu, (char*)(m_pstChipId[iSlot]+ iCpu*nDutPerCpuSize), nDutPerCpuSize, nChipIdSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		if ( false )
		{
			for(int iDut=0; iDut<192; ++iDut )
			{
				if ( m_pInitFail[iSlot][iDut] ) continue;

				for(uint iCe=0; iCe<m_nCePerDut[iSlot]; ++iCe)
				{
					for(uint iChip=0; iChip<m_nChipPerCe[iSlot]; ++iChip)
					{
						char szLine[1024];
						sprintf( szLine, "     %4s       %02x       %02d       %02d",
								m_pstChipId[iSlot][iDut*m_nChipPerDut[iSlot] + iCe*m_nChipPerCe[iSlot] + iChip].szchipId,
								m_pstChipId[iSlot][iDut*m_nChipPerDut[iSlot] + iCe*m_nChipPerCe[iSlot] + iChip].uWaferId,
								m_pstChipId[iSlot][iDut*m_nChipPerDut[iSlot] + iCe*m_nChipPerCe[iSlot] + iChip].uXPos   ,
								m_pstChipId[iSlot][iDut*m_nChipPerDut[iSlot] + iCe*m_nChipPerCe[iSlot] + iChip].uYPos
								);
						printf("S%02d DUT%03d CE%02d CHIP%01d   %s\n", iSlot, iDut, iCe, iChip, szLine);
					}
				}
			}

		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// CUM FAIL CHIP DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		nChipDataSize  = m_nMaxDut      [iSlot] * m_nChipPerDutBl[iSlot];
		nDutPerCpuSize = m_nMaxDutPerCpu[iSlot] * m_nChipPerDutBl[iSlot];
		m_pCumFailChip[iSlot] = new char[ nChipDataSize ];
		if ( m_pCumFailChip[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Cum Fail Chip) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_CUMFAIL_CHIP_DATA(iSlot, iCpu, m_pCumFailChip[iSlot]+ iCpu*nDutPerCpuSize, nDutPerCpuSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP FAIL CHIP DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nStepChipDataSize  = m_nMaxDut      [iSlot] * m_nChipPerDutBl[iSlot] * m_nMaxStep[iSlot];
		nChipDataSize           = m_nMaxDutPerCpu[iSlot] * m_nChipPerDutBl[iSlot];
		m_pStepFailChip[iSlot] = new char[ nStepChipDataSize ];
		if ( m_pStepFailChip[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Fail Chip) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPFAIL_CHIP_DATA(iSlot, iCpu, m_pStepFailChip[iSlot], m_nMaxStep[iSlot], nChipDataSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// CUM FAIL BLOCK DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nCumBlockSize       = m_nMaxDut      [iSlot] * m_nBlockPerDutBl[iSlot];
		uint nCumBlockSizePerCpu = m_nMaxDutPerCpu[iSlot] * m_nBlockPerDutBl[iSlot];
		m_pCumFailBlock[iSlot] = new char[ nCumBlockSize ];
		if ( m_pCumFailBlock[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Cum Fail Block) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_CUMFAIL_BLOCK_DATA(iSlot, iCpu, m_pCumFailBlock[iSlot]+ iCpu*nCumBlockSizePerCpu, nCumBlockSizePerCpu ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP FAIL BLOCK DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nMaxStepBlockSize = m_nMaxDut      [iSlot] * m_nBlockPerDutBl[iSlot] * m_nMaxStep[iSlot];
		nCumBlockSize          = m_nMaxDutPerCpu[iSlot] * m_nBlockPerDutBl[iSlot];
		m_pStepFailBlock[iSlot] = new char[ nMaxStepBlockSize ];
		if ( m_pStepFailBlock[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Fail Block) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPFAIL_BLOCK_DATA(iSlot, iCpu, m_pStepFailBlock[iSlot], m_nMaxStep[iSlot], nCumBlockSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP FAIL CATE DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nStepCateSize  = m_nMaxDut[iSlot] * m_nMaxStep     [iSlot];
		uint nCateSize      = sizeof(uint)     * m_nMaxDutPerCpu[iSlot];
		m_pStepFCate[iSlot] = new uint[ nStepCateSize ];
		if ( m_pStepFCate[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Fail Cate) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPFCATE_DATA(iSlot, iCpu, m_pStepFCate[iSlot], m_nMaxStep[iSlot], nCateSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		if ( false )
		{
			for(uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep)
			{
				for(int iDut=0; iDut<192; ++iDut)
				{
					if ( m_pStepFCate[iSlot][iStep*192 + iDut] != 0 )
						printf("Step%d dut%03d = %d\n", iStep, iDut, m_pStepFCate[iSlot][iStep*192 + iDut] );
				}
				printf("\n");
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP PASS CATE DATA
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_pStepPCate[iSlot] = new uint[ nStepCateSize ];
		if ( m_pStepPCate[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Pass Cate) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPPCATE_DATA(iSlot, iCpu, m_pStepPCate[iSlot], m_nMaxStep[iSlot], nCateSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		if ( false )
		{
			for(uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep)
			{
				for(int iDut=0; iDut<192; ++iDut)
				{
					if ( m_pStepPCate[iSlot][iStep*192 + iDut] != 0 )
						printf("Step%d dut%03d = %d\n", iStep, iDut, m_pStepPCate[iSlot][iStep*192 + iDut] );
				}
				printf("\n");
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP FAIL_MODE
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nStepFailModeSize = m_nMaxStep[iSlot];
		uint nFailModeSize     = sizeof(int);
		m_pStepFailMode[iSlot] = new int[ nStepFailModeSize ];
		if ( m_pStepFailMode[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Fail Mode) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		if( !m_pDataMgr->GET_STEPFAIL_MODE(iSlot, CPU00, m_pStepFailMode[iSlot], m_nMaxStep[iSlot], nFailModeSize ) )
		{
			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, CPU00, m_pDataMgr->GET_ERRMSG() );
			return false;
		}

		if ( false )
		{
			for(uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep)
			{
				printf("Step%d = %d\n", iStep, m_pStepFailMode[iSlot][iStep] );
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// STEP USE_MODE
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint nStepUseModeSize = m_nMaxStep[iSlot];
		uint nUseModeSize     = sizeof(int);
		m_pStepUseMode[iSlot] = new int[ nStepUseModeSize ];
		if ( m_pStepUseMode[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Use Mode) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

//		if( !m_pDataMgr->GET_STEPUSE_MODE(iSlot, CPU00, m_pStepUseMode[iSlot], m_nMaxStep[iSlot], nUseModeSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}

		int *pStepUseMode[MAX_SYS_CPU] = { nullptr,nullptr };
		pStepUseMode[CPU00] = new int[ nStepUseModeSize ];
		if ( pStepUseMode[CPU00] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Use Mode) alloc error\n", __FUNCTION__, iSlot );
			return false;
		}

		pStepUseMode[CPU01] = new int[ nStepUseModeSize ];
		if ( pStepUseMode[CPU01] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d Memory(Step Use Mode) alloc error\n", __FUNCTION__, iSlot );
			if ( pStepUseMode[CPU00] != nullptr ) { delete [] pStepUseMode[CPU00]; pStepUseMode[CPU00] = nullptr; }
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_STEPUSE_MODE(iSlot, iCpu, pStepUseMode[iCpu], m_nMaxStep[iSlot], nUseModeSize ) )
			{
				fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", iSlot, iCpu, m_pDataMgr->GET_ERRMSG() );
				return false;
			}
		}

		for(uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep)
		{
			m_pStepUseMode[iSlot][iStep] = pStepUseMode[CPU00][iStep] | pStepUseMode[CPU01][iStep];
//			m_pStepUseMode[iSlot][iStep] = 2;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if ( pStepUseMode[iCpu] != nullptr )
			{
				delete [] pStepUseMode[iCpu];
				pStepUseMode[iCpu] = nullptr;
			}
		}

		if ( false )
		{
			printf("[STEP USE_MODE]\n");
			for(uint iStep=0; iStep<m_nMaxStep[iSlot]; ++iStep)
			{
				printf("Step%03d = %d\n", iStep+1, m_pStepUseMode[iSlot][iStep] );
			}
		}

//		printf("%s: %d: -----------------------\n", __FUNCTION__, __LINE__ );
////////////////////////////////
	}

//	printf("%s\n", __FUNCTION__ );

	return true;
}

bool COrganizedData::GatherInitFailData(uint iFileType) //for SaveMap only, iFileType==0: set map, iFileType==1: pre map
{
	if ( iFileType==0 ) return true; //set map

	//pre map
	for( int iSlot=0; iSlot<MAX_SLOT; iSlot++ )
	{
		if( !m_bValidSlot[iSlot]                    ) continue;
		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(iSlot) ) continue;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// INIT FAIL, InitTestHead
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		stInitTestHead initHead[MAX_SYS_CPU];
		m_pInitFail[iSlot] = new char[ m_nMaxDut[iSlot] ];
		if ( m_pInitFail[iSlot] == nullptr )
		{
			SET_ERRMSG( "%s : S#%02d FLS Memory(Init Fail) alloc error\n", __FUNCTION__, iSlot+1 );
			return false;
		}

		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			if( !m_pDataMgr->GET_INITTEST_DATA( iSlot, iCpu, initHead[iCpu], m_pInitFail[iSlot]+iCpu*m_nMaxDutPerCpu[iSlot], sizeof(char)*m_nMaxDutPerCpu[iSlot]) )
			{
				SET_ERRMSG( "%s : S#%02d-C#%02d : %s\n", __FUNCTION__, iSlot+1, iCpu+1, m_pDataMgr->GET_ERRMSG() );
				return false;
			}

			m_initTestHead[iSlot][iCpu].nStepNo = initHead[iCpu].nStepNo;
			sprintf( m_initTestHead[iSlot][iCpu].szStepName, "%s", initHead[iCpu].szStepName );

			m_initTestHead[iSlot][iCpu].tInitBegin = initHead[iCpu].tInitBegin;
			m_initTestHead[iSlot][iCpu].tInitEnd   = initHead[iCpu].tInitEnd  ;
		}
	}

	for ( LOT_LIST::iterator itLot = m_Lot.begin(); itLot != m_Lot.end(); ++ itLot )
	{
		T_LOT* pLot = *itLot;

		pLot->m_tInitStart = 0;
		pLot->m_tInitEnd   = 0;
		for(int iSlot=0; iSlot<32; ++iSlot)
		{
			if( !pLot->S[iSlot].IS_VALID()              ) continue;
			if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(iSlot) ) continue;

			for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
			{
				if ( pLot->m_tInitStart==0 ) pLot->m_tInitStart = m_initTestHead[iSlot][iCpu].tInitBegin;
				if ( pLot->m_tInitEnd  ==0 ) pLot->m_tInitEnd   = m_initTestHead[iSlot][iCpu].tInitEnd  ;

				if ( pLot->m_tInitStart < m_initTestHead[iSlot][iCpu].tInitBegin )
					pLot->m_tInitStart = m_initTestHead[iSlot][iCpu].tInitBegin;

				if ( pLot->m_tInitEnd   > m_initTestHead[iSlot][iCpu].tInitEnd   )
					pLot->m_tInitEnd   = m_initTestHead[iSlot][iCpu].tInitEnd  ;

			} //End of Cpu
		} //End of Slot
	} //End of Lot

	return true;
}

bool COrganizedData::ReadSetupTime(stSetupTimePair &tSetup)
{
	QString sFile = tr("%1/%2/%3")
			.arg(m_sDataRoot)
			.arg(m_pDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
			.arg(DAT_SETUP_TIME);

	QFile file(sFile);
	if (file.open(QIODevice::ReadOnly))
	{
		stSetupTimePair read_setup_time;
		size_t nRead = file.read((char *)&read_setup_time, sizeof(stSetupTimePair));
		if( sizeof(stSetupTimePair) != nRead )
		{
			file.close();
			return false;
		}
		file.close();
		tSetup = read_setup_time;
	}
	else
		return false;

	return true;
}
