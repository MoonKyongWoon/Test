#include "filemaplib.hpp"
#include "syslog.hpp"
#include "sysini.hpp"

namespace fs = std::filesystem;

CFileMap::CFileMap( const char* pDirRoot )
{
	SetTestdataRootDir( pDirRoot );
	LOAD_FILEMAP_INFO( pDirRoot );
}

CFileMap::CFileMap( const char* pDirRoot, bool mode )
{
	SetTestdataRootDir( pDirRoot );
	SET_UIAUTOMODE( mode );
	LOAD_FILEMAP_INFO();
}

void CFileMap::LOAD_FILEMAP_INFO()
{
	InitTestdataDir( false );
	InitTestDataFiles();
}

void CFileMap::LOAD_FILEMAP_INFO( const char* pDirRoot )
{
	SetTestdataRootDir( pDirRoot );
	InitTestdataDir( false );
	InitTestDataFiles();
}

QString CFileMap::DATA_ROOTDIR()
{
	return QString(m_szDataRoot);
}

void CFileMap::SetTestdataRootDir( const char* pDataDir )
{
	m_bDebugMode = false;
	m_bIsDevVar  = false;
	if( pDataDir )
	{
		snprintf( m_szDataRoot, sizeof(m_szDataRoot), "%s", pDataDir );
	}
	else
	{
		ReadDataDirEnv();
	}

	fs::path p( m_szDataRoot );	//Data directory
	if( !fs::exists( p ) )
	{
		if( !fs::create_directory( m_szDataRoot ) )
		{
			SET_ERRMSG( "[%20s] %s is not exit, mkdir error!", __FUNCTION__, m_szDataDir  );
		}
	}
	snprintf( m_szUiModeFile, sizeof(m_szUiModeFile), "%s/%s", m_szDataRoot, UIMODE_INI );
}

bool CFileMap::ReadDataDirEnv()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	if( env_data != NULL )
	{
		snprintf( m_szDataRoot, sizeof(m_szDataRoot), "%s/%s", env_data, DIR_ATDATA_NAME );
	}
	else
	{
		snprintf( m_szDataRoot, sizeof(m_szDataRoot), "%s/%s", DIR_ATDATA_PATH, DIR_ATDATA_NAME );
	}
	return true;
}

//2024.06.13 - KWMOON
bool CFileMap::GET_UIAUTOMODE()
{
	std::string ui_mode = "";
	std::string section = std::string(SECTION_UIMODE) + std::string("/") + std::string(KEY_UIMODE);
	UTIL::INI::readSettings( m_szUiModeFile, section, ui_mode );
	if( ui_mode == "auto" ) return true;
	return false;
}

//2024.06.13 - KWMOON
bool CFileMap::SET_UIAUTOMODE(bool mode)
{
	std::string ui_mode = "engr";
	if( mode )  ui_mode = "auto";
	std::string section = std::string(SECTION_UIMODE) + std::string("/") + std::string(KEY_UIMODE);
	return UTIL::INI::writeSettings( m_szUiModeFile, section, ui_mode );
}

bool CFileMap::InitTestdataDir(bool bRemove)
{
	//namespace fs = std::filesystem;
	bool bAuto = GET_UIAUTOMODE();
	if( bAuto )  snprintf( m_szDataDir, sizeof(m_szDataDir), "%s/%s", m_szDataRoot, AUTO_DIR );
	else         snprintf( m_szDataDir, sizeof(m_szDataDir), "%s/%s", m_szDataRoot, ENGR_DIR );

	fs::path p( m_szDataDir );	//Data directory

	if ( bRemove )
	{
		if( fs::is_directory(p) )
		{
			for( const auto& entry : fs::directory_iterator(p) )
			{
				if( fs::is_directory( entry.path() ) )
				{
					fs::remove_all( entry.path() );
					if( m_bDebugMode )
					{
						//fprintf( stdout, "[%20s] %s directory remove!\n", __FUNCTION__, entry.path().c_str()  );
					}
				}
				else
				{
					fs::remove( entry.path() );
					if( m_bDebugMode )
					{
						//fprintf( stdout, "[%20s] %s file remove!\n", __FUNCTION__, entry.path().c_str()  );
					}
				}
			}
		}
	}
	else
	{
		if( !fs::exists( p ) )
		{
			if( !fs::create_directory( m_szDataDir ) )
			{
				SET_ERRMSG( "[%20s] %s is not exit, mkdir error!", __FUNCTION__, m_szDataDir  );
				return false;
			}
		}
	}
	InitTgetDir();
	InitBiOpenLotDir();
	InitBoardMaintInfoDir();
	InitSlotDir();
	return true;
}


bool CFileMap::SET_ERRMSG( const char* str, ...)
{
	va_list	ap;
	char buf[MAX_TEXT_MSG] = {0x0,};
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	snprintf( m_szErrMsg, MAX_TEXT_MSG, "%s", buf );
	if( m_bDebugMode )
	{
		fprintf( stdout, "%s\n", m_szErrMsg ); fflush(stdout);
	}
	return true;
}

bool CFileMap::SET_MSG( const char* str, ...)
{
	va_list	ap;
	char buf[MAX_TEXT_MSG] = {0x0,};
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	snprintf( m_szMsg, MAX_TEXT_MSG, "%s", buf );
	if( m_bDebugMode )
	{
		fprintf( stdout, "%s\n", m_szMsg ); fflush(stdout);
	}
	return true;
}

char* CFileMap::GET_ERRMSG()
{
	return m_szErrMsg;
}

void CFileMap::SET_DEBUGMODE(bool mode)
{
	m_bDebugMode = mode;
}

bool CFileMap::InitTestDataFiles()
{
	bool bOk = true;
	char szBuf[MAX_DIR_FILENAME]={0x0,};

	snprintf( m_szUiModeFile, sizeof(m_szUiModeFile), "%s/%s", m_szDataRoot, UIMODE_INI     );

	//BOARD MAINT DIR & FILE
	{
		snprintf(m_szBoardMaintdir, sizeof(m_szBoardMaintdir), "%s/%s", m_szDataDir, BOARD_MAINT_DIR );
		for( int s=0;s<MAX_SLOT;s++ )
		{
			//BOARD MAINT FILE
            snprintf( m_szBoardMaintFile[s], MAX_DIR_FILENAME, "%s/SLOT%d%s", m_szBoardMaintdir, s, EXT_RCMD );
		}
	}

	//TGET DIR
	{
		snprintf(m_szTgetDir, sizeof(m_szTgetDir), "%s/%s", m_szDataDir, TGET_DIR );
	}

	//BI_OPEN_LOT DIR
	{
		snprintf(m_szBiOpenLotDir, sizeof(m_szBiOpenLotDir), "%s/%s", m_szDataDir, BI_OPEN_LOT_DIR );
	}

	//SLOT FILES
	for( int s=0;s<MAX_SLOT;s++ )
	{
		memset( szBuf, 0x0, sizeof(szBuf) );
		snprintf(szBuf, sizeof(szBuf), "%s/SLOT%d", m_szDataDir, s );
		//BIBID
		snprintf( m_szBibIdFile[s], MAX_DIR_FILENAME, "%s/%s", szBuf, BIBID_TXT_NAME );

		//TEST ITEM
		snprintf( m_szTestItemFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, TESTITEM_NAME, CPU00, EXT_DAT );
		snprintf( m_szTestItemFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, TESTITEM_NAME, CPU01, EXT_DAT );

		//DEV_VAR
		snprintf( m_szDevVarFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, DEVPGM_VAR_NAME, CPU00, EXT_DAT );
		snprintf( m_szDevVarFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, DEVPGM_VAR_NAME, CPU01, EXT_DAT );

		//INITFAIL
		snprintf( m_szInitFailDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, INITFAIL_NAME, CPU00, EXT_DAT );
		snprintf( m_szInitFailDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, INITFAIL_NAME, CPU01, EXT_DAT );

		//STEPIN
		snprintf( m_szStepInFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPIN_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepInFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPIN_NAME, CPU01, EXT_DAT );

		//STEPOUT
		snprintf( m_szStepOutFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPOUT_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepOutFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPOUT_NAME, CPU01, EXT_DAT );

		//CYCLE INOUT
		snprintf( m_szCycleInOutFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CYCLEINOUT_NAME, CPU00, EXT_DAT );
		snprintf( m_szCycleInOutFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CYCLEINOUT_NAME, CPU01, EXT_DAT );

		//CHIPID INFO
		snprintf( m_szChipInfoFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CHIPINFO_NAME, CPU00, EXT_DAT );
		snprintf( m_szChipInfoFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CHIPINFO_NAME, CPU01, EXT_DAT );

		//CHIPID DATA
		snprintf( m_szChipDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CHIPDATA_NAME, CPU00, EXT_DAT );
		snprintf( m_szChipDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CHIPDATA_NAME, CPU01, EXT_DAT );

		//STEPFAIL
		snprintf( m_szStepFailDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAIL_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepFailDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAIL_NAME, CPU01, EXT_DAT );

		//STEPBLOCKFAIL
		snprintf( m_szStepFailBlockDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAILBLOCK_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepFailBlockDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAILBLOCK_NAME, CPU01, EXT_DAT );

		//STEPFAILCHIP
		snprintf( m_szStepFailChipDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAILCHIP_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepFailChipDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAILCHIP_NAME, CPU01, EXT_DAT );

		//STEP SORT BIN
		snprintf( m_szStepSortBinDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPSORTBIN_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepSortBinDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPSORTBIN_NAME, CPU01, EXT_DAT );

		//STEP FCATE 2024.07.22 fatric
		snprintf( m_szStepFCateDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFCATE_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepFCateDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFCATE_NAME, CPU01, EXT_DAT );

		//STEP PCATE 2024.07.22 fatric
		snprintf( m_szStepPCateDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPPCATE_NAME, CPU00, EXT_DAT );
		snprintf( m_szStepPCateDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPPCATE_NAME, CPU01, EXT_DAT );

		//STEP FAIL_MODE 2024.07.22 fatric
		snprintf( m_szStepFailModeFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAIL_MODE, CPU00, EXT_DAT );
		snprintf( m_szStepFailModeFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPFAIL_MODE, CPU01, EXT_DAT );

		//STEP USE_MODE 2024.07.22 fatric
		snprintf( m_szStepUseModeFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPUSE_MODE, CPU00, EXT_DAT );
		snprintf( m_szStepUseModeFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, STEPUSE_MODE, CPU01, EXT_DAT );

		//CUM FAILBLOCK
		snprintf( m_szCumFailBlockDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILBLOCK_NAME, CPU00, EXT_DAT );
		snprintf( m_szCumFailBlockDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILBLOCK_NAME, CPU01, EXT_DAT );

		//CUM FAILCHIP
		snprintf( m_szCumFailChipDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILCHIP_NAME, CPU00, EXT_DAT );
		snprintf( m_szCumFailChipDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILCHIP_NAME, CPU01, EXT_DAT );

		//CUM TESTFAIL
		snprintf( m_szCumTestFailDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMTESTFAIL_NAME, CPU00, EXT_DAT );
		snprintf( m_szCumTestFailDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMTESTFAIL_NAME, CPU01, EXT_DAT );

		//CUM SORT BIN
		snprintf( m_szCumSortBinDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMSORTBIN_NAME, CPU00, EXT_DAT );
		snprintf( m_szCumSortBinDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMSORTBIN_NAME, CPU01, EXT_DAT );

		//2024.06.24 - KWMOON CUM FAIL CATE
		snprintf( m_szCumFailCateDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILCATE_NAME, CPU00, EXT_DAT );
		snprintf( m_szCumFailCateDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, CUMFAILCATE_NAME, CPU01, EXT_DAT );

		//BINMAP 2024.07.22 fatric
		snprintf( m_szBinmapDataFile[s][CPU00], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, BINMAP_NAME, CPU00, EXT_DAT );
		snprintf( m_szBinmapDataFile[s][CPU01], MAX_DIR_FILENAME, "%s/%s%d%s", szBuf, BINMAP_NAME, CPU01, EXT_DAT );
	}
	if( m_bDebugMode ) fprintf( stdout, "[%20s] DONE\n", __FUNCTION__ );
	return bOk;
}

bool CFileMap::SET_ENGR_SETUP(char* lotno, char* partno, char* oprid)
{
	bool bOk = true;
	for( int i=0;i<MAX_SLOT;i++ )
	{
		REMOVE_INITTEST_DATA(i);
		REMOVE_STEPTEST_DATA(i);
	}
	REMOVE_RCMD_FILES();

	bOk &= MAKE_ENGR_START_BICMD_FILE( lotno, partno );
	bOk &= MAKE_ENGR_TGET_FILE( lotno, partno );
	bOk &= MAKE_ENGR_BOARD_MAINT_INFO_FILE( lotno, partno );
	return bOk;
}

//2024.06.27 - KWMOON(INI ->RCMD) --> 2024.08.05 -KWMOON(BOARD_ID / COUNT / ...  Add )
bool CFileMap::MAKE_ENGR_START_BICMD_FILE( char* lot,  char* part )
{
	char szSBI[MAX_DIR_FILENAME]={0x0,};
	snprintf( szSBI, sizeof(szSBI), "%s/%s%s", m_szDataDir, RCMD_START_BI_CMD, EXT_RCMD );
	(void)part;

	stRcmdStartBiCmd startbicmd;
	memset( (char*)&startbicmd, 0x0, sizeof(stRcmdStartBiCmd) );
	snprintf( startbicmd.LOT_ID, sizeof(startbicmd.LOT_ID), "%s", lot );

	uint bib_count = 0;
	stBibId bibid;
	for( uint s=0;s<MAX_SLOT;s++ )
	{
		if( !GET_BIBID( s, bibid ) ) continue;
		snprintf( startbicmd.BOARD_ID[s], sizeof(startbicmd.BOARD_ID[s]), "%s", bibid.szBibId );
		bib_count++;
	}
	snprintf( startbicmd.RACK_COUNT, sizeof(startbicmd.RACK_COUNT), "%d", 2 );
	snprintf( startbicmd.BID_COUNT , sizeof(startbicmd.BID_COUNT ), "%d", bib_count );

	return MAKE_FILE( szSBI, (char*)&startbicmd, sizeof(stRcmdStartBiCmd) );
}

//2024.06.27 - KWMOON(INI ->RCMD)
bool CFileMap::MAKE_ENGR_TGET_FILE( char* lot,  char* part )
{
	char szLotFile[MAX_DIR_FILENAME]={0x0,};
	snprintf( szLotFile, sizeof(szLotFile), "%s/%s%s", m_szTgetDir, lot, EXT_RCMD );

	fs::path tgetdir( m_szTgetDir );	//Data directory
	if( !fs::exists( tgetdir ) )
	{
		if( !fs::create_directory( tgetdir ) )
		{
			return false;
		}
	}
	fprintf( stdout, "m_szTgetDir : %s\n", m_szTgetDir );
	fprintf( stdout, "MAKE TGET FILE : %s\n", szLotFile );

	stRcmdTGET tget;
	memset( (char*)&tget, 0x0, sizeof(stRcmdTGET) );
	//RETURN FLAG
	snprintf( tget.RETURN_FLAG, sizeof(tget.RETURN_FLAG), "%s", "OK" );

	//LOT NO
	snprintf( tget.LOT_NO, sizeof(tget.LOT_NO), "%s", lot );

	//PART NO
	snprintf( tget.PART_NO, sizeof(tget.PART_NO), "%s", part );

	//MLB_FLAG
	snprintf( tget.MLB_FLAG, sizeof(tget.MLB_FLAG), "%s", "Y" );

	//MLB_PGM_LOT_LIST
	snprintf( tget.MLB_PGM_LOT_LIST, sizeof(tget.MLB_PGM_LOT_LIST), "%s", lot );

	//RWK_COUNT
	snprintf( tget.RWK_COUNT, sizeof(tget.RWK_COUNT), "%s", "0" );

	return MAKE_FILE( szLotFile, (char*)&tget, sizeof(stRcmdTGET) );
}

//2024.06.27 - KWMOON(INI ->RCMD)
bool CFileMap::MAKE_ENGR_BOARD_MAINT_INFO_FILE( char* lot,  char* part )
{
	const int MAX_BIB_DUT = 192;
	char szBMI[MAX_DIR_FILENAME]={0x0,};
	for( int s=0;s<MAX_SLOT;s++ )
	{
		snprintf( szBMI, sizeof(szBMI), "%s", m_szBoardMaintFile[s] );
		if( !fs::exists( m_szBibIdFile[s] ) )
		{
			if( fs::exists( szBMI ) ) fs::remove( szBMI );
			continue;
		}
		stRcmdBoardMaintInfo bmi;
		memset( (char*)&bmi, 0x0, sizeof(stRcmdBoardMaintInfo) );

		//RETURN FLAG
		snprintf( bmi.RETURN_FLAG, sizeof(bmi.RETURN_FLAG), "%s", "OK" );

		//ERROR DESC
		snprintf( bmi.ERROR_DESCRIPTION, sizeof(bmi.ERROR_DESCRIPTION), "%s", "OK" );

		//MLB CODE
		snprintf( bmi.MLB_CODE, sizeof(bmi.MLB_CODE), "%s", "SLB" );

		//LOT NO
		snprintf( bmi.LOT_NO, sizeof(bmi.LOT_NO), "%s", lot );

		//MLB LOT LIST
		snprintf( bmi.MLB_LOT_LIST, sizeof(bmi.MLB_LOT_LIST), "%s", lot );

		//PART NO
		snprintf( bmi.PART_NO, sizeof(bmi.PART_NO), "%s", part );

		//LOT MAP
		QString lot_map = "";
		for( int i=0;i<MAX_BIB_DUT;i++ )
		{
			lot_map +=QString(lot);
			if( i < (MAX_BIB_DUT-1) ) lot_map += ",";
		}
		snprintf( bmi.LOT_MAP, sizeof(bmi.LOT_MAP), "%s", lot_map.toStdString().c_str() );

		QString mount_mask_map = "";
		for( int i=0;i<MAX_BIB_DUT;i++ )
			mount_mask_map += "1";

		//KEY_BMI_INSERT_MOUNT_MAP
		snprintf( bmi.INSERT_MOUNT_MAP, sizeof(bmi.INSERT_MOUNT_MAP), "%s", mount_mask_map.toStdString().c_str() );

		//KEY_BMI_INITIAL_MASK_MAP
		snprintf( bmi.INITIAL_MASK_MAP, sizeof(bmi.INITIAL_MASK_MAP), "%s", mount_mask_map.toStdString().c_str() );

		//KEY_BMI_MANUAL_MASK_MAP
		snprintf( bmi.MANUAL_MASK_MAP, sizeof(bmi.MANUAL_MASK_MAP), "%s", mount_mask_map.toStdString().c_str() );

		//KEY_BMI_FUNCTION_MASK_MAP
		snprintf( bmi.FUNTION_MASK_MAP, sizeof(bmi.FUNTION_MASK_MAP), "%s", mount_mask_map.toStdString().c_str() );

		MAKE_FILE( szBMI, (char*)&bmi, sizeof(stRcmdBoardMaintInfo) );
	}
	return true;
}

int CFileMap::GET_LOTLIST(std::list<std::string>& rLotList )
{
	atstring lotno = "";
	char szSBI[MAX_DIR_FILENAME]={0x0,};
	snprintf( szSBI, sizeof(szSBI), "%s/%s%s", m_szDataDir, RCMD_START_BI_CMD, EXT_RCMD );

	rLotList.clear();
	stRcmdStartBiCmd startbicmd;
	if( !READ_FILE( szSBI, (char*)&startbicmd, sizeof(stRcmdStartBiCmd) ) )
	{
		SET_ERRMSG( "%s : %s read error!", __FUNCTION__, szSBI );
		return -1;
	}
	lotno = atstring(startbicmd.LOT_ID);
	return lotno.Split( rLotList, "," );
}

bool CFileMap::LOT_SET( const char* lotno)
{
	if( !m_bUiMode )	//engr
	{
		//GET_SETUP_LOTNO();
	}
	else
	{

	}
	snprintf( m_szLotNo  , sizeof(m_szLotNo)  , "%s", lotno  );
	bool bFound = false;

	if( bFound )
	{
		//InitTestDataFileName();
		//m_bIsSetup = true;
		return true;
	}
	else
	{
		SET_ERRMSG( "%s LOT Dir/Data is not exist!", lotno );
	}
	return false;
}

bool CFileMap::InitSlotDir()
{
	bool bInit = true;
	fs::path p( m_szDataDir );	//Data directory
	if( fs::is_directory(p) )
	{
		for( int s=0;s<MAX_SLOT;s++ )
		{
			char szBuf[MAX_DIR_FILENAME*2]={0x0,};
			snprintf(szBuf, sizeof(szBuf), "%s/SLOT%d", m_szDataDir, s );
			fs::path slotdir( szBuf );	//Data directory
			if( !fs::exists(slotdir) )
			{
				if( !fs::create_directory( szBuf ) )
				{
					bInit = false;
					SET_ERRMSG( "[%20s] %s mkdir error", __FUNCTION__, szBuf );
				}
			}
		}
	}
	else
	{
		bInit = false;
		SET_ERRMSG( "[%20s] %s not exist", __FUNCTION__, m_szDataDir );
	}
	return bInit;
}

bool CFileMap::InitTgetDir()
{
	bool bInit = true;
	fs::path p( m_szDataDir );	//Data directory
	if( fs::is_directory(p) )
	{
		char szBuf[MAX_DIR_FILENAME*2]={0x0,};
		snprintf(szBuf, sizeof(szBuf), "%s/%s", m_szDataDir, TGET_DIR );

		fs::path tget( szBuf );	//Data directory
		if( !fs::exists(tget) )
		{
			if( !fs::create_directory( szBuf ) )
			{
				bInit = false;
				SET_ERRMSG( "[%20s] %s mkdir error", __FUNCTION__, szBuf );
			}
		}
	}
	else
	{
		bInit = false;
		SET_ERRMSG( "[%20s] %s not exist", __FUNCTION__, m_szDataDir );
	}
	return bInit;
}

bool CFileMap::InitBiOpenLotDir()
{
	bool bInit = true;
	fs::path p( m_szDataDir );	//Data directory
	if( fs::is_directory(p) )
	{
		char szBuf[MAX_DIR_FILENAME*2]={0x0,};
		snprintf(szBuf, sizeof(szBuf), "%s/%s", m_szDataDir, BI_OPEN_LOT_DIR );

		fs::path tget( szBuf );	//Data directory
		if( !fs::exists(tget) )
		{
			if( !fs::create_directory( szBuf ) )
			{
				bInit = false;
				SET_ERRMSG( "[%20s] %s mkdir error", __FUNCTION__, szBuf );
			}
		}
	}
	else
	{
		bInit = false;
		SET_ERRMSG( "[%20s] %s not exist", __FUNCTION__, m_szDataDir );
	}
	return bInit;
}

bool CFileMap::InitBoardMaintInfoDir()
{
	bool bInit = true;
	fs::path p( m_szDataDir );	//Data directory
	if( fs::is_directory(p) )
	{
		char szBuf[MAX_DIR_FILENAME*2]={0x0,};
		snprintf(szBuf, sizeof(szBuf), "%s/%s", m_szDataDir, BOARD_MAINT_DIR );

		fs::path bmd( szBuf );	//Data directory
		if( !fs::exists(bmd) )
		{
			if( !fs::create_directory( szBuf ) )
			{
				bInit = false;
				SET_ERRMSG( "[%20s] %s mkdir error", __FUNCTION__, szBuf );
			}
		}
	}
	else
	{
		bInit = false;
		SET_ERRMSG( "[%20s] %s not exist", __FUNCTION__, m_szDataDir );
	}
	return bInit;
}

bool CFileMap::REMOVE_BIBID_FILE(uint slot)
{
	if( fs::exists( m_szBibIdFile[slot] ) )
		fs::remove( m_szBibIdFile[slot] );
	return true;
}

//2024.06.24 - KWMOON
bool CFileMap::REMOVE_RCMD_FILES()
{
	fs::path p( m_szDataDir );	//Data directory

	if( fs::is_directory(p) )
	{
		for( const auto& entry : fs::recursive_directory_iterator(p) )
		{
			if( fs::is_directory( entry.path() ) )
			{
			}
			else
			{
				std::string file = entry.path().generic_string();
				if (file.find(EXT_RCMD) != string::npos)
				{
					fs::remove( entry.path() );
				}
			}
		}
	}
	return true;
}

//2024.06.24 - KWMOON
bool CFileMap::MAKE_FILE( const char* filename, char* pData, uint size )
{
	FILE* fp = NULL;
	fp = fopen( filename, "wb" );
	if( !fp )
	{
		SET_ERRMSG( "%s open error!", filename );
		return false;
	}
	char* pBuf = pData;
	uint towrite = size;
	uint written = 0;
	uint retry   = 0;
	while( towrite > 0 )
	{
		size_t writebyte = fwrite( pBuf, 1, towrite, fp );
		if( writebyte < 1 ) retry++;
		towrite -= writebyte;
		pBuf    += writebyte;
		written += writebyte;
		if( retry >= 5 )
		{
			SET_ERRMSG( "%s write try count >= 5,  written size : %d, file size :%d\n", filename, written, size );
			break;
		}
	}
	fflush(fp);
	fclose(fp);

	if( size != written )
	{
		SET_ERRMSG( "%s expect size[%d] write size[%d] error!", filename, size, written );
		return false;
	}
	return true;
}

//2024.06.24 - KWMOON
bool CFileMap::READ_FILE( const char* filename, char* ptr, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( filename, "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!", __FUNCTION__, filename );

		return false;
	}
	fseek(fp, 0, SEEK_SET);
	uint expect_size = size;
	uint read = fread( ptr, 1, expect_size, fp );
	fclose(fp);
	if( expect_size != read )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",	__FUNCTION__, filename, expect_size, read );
	}
	return true;
}


bool CFileMap::REMOVE_INITTEST_DATA(uint slot )
{
	//if( !m_bIsSetup ) return false;
	if( slot > MAX_SLOT ) return false;

	if( fs::exists( m_szInitFailDataFile[slot][CPU00] ) ) fs::remove( m_szInitFailDataFile[slot][CPU00] );
	if( fs::exists( m_szInitFailDataFile[slot][CPU01] ) ) fs::remove( m_szInitFailDataFile[slot][CPU01] );
	return true;
}

bool CFileMap::REMOVE_STEPTEST_DATA(uint slot)
{
	//if( !m_bIsSetup ) return false;
	if( slot > MAX_SLOT ) return false;

	//STEPIN
	if( fs::exists( m_szStepInFile[slot][CPU00] ) ) fs::remove( m_szStepInFile[slot][CPU00] );
	if( fs::exists( m_szStepInFile[slot][CPU01] ) ) fs::remove( m_szStepInFile[slot][CPU01] );

	//STEPOUT
	if( fs::exists( m_szStepOutFile[slot][CPU00] ) ) fs::remove( m_szStepOutFile[slot][CPU00] );
	if( fs::exists( m_szStepOutFile[slot][CPU01] ) ) fs::remove( m_szStepOutFile[slot][CPU01] );

	//CHIPID INFO
	if( fs::exists( m_szChipInfoFile[slot][CPU00] ) ) fs::remove( m_szChipInfoFile[slot][CPU00] );
	if( fs::exists( m_szChipInfoFile[slot][CPU01] ) ) fs::remove( m_szChipInfoFile[slot][CPU01] );

	//CHIPID DATA
	if( fs::exists( m_szChipDataFile[slot][CPU00] ) ) fs::remove( m_szChipDataFile[slot][CPU00] );
	if( fs::exists( m_szChipDataFile[slot][CPU01] ) ) fs::remove( m_szChipDataFile[slot][CPU01] );

	//STEPFAIL
	if( fs::exists( m_szStepFailDataFile[slot][CPU00] ) ) fs::remove( m_szStepFailDataFile[slot][CPU00] );
	if( fs::exists( m_szStepFailDataFile[slot][CPU01] ) ) fs::remove( m_szStepFailDataFile[slot][CPU01] );

	//STEPBLOCKFAIL
	if( fs::exists( m_szStepFailBlockDataFile[slot][CPU00] ) ) fs::remove( m_szStepFailBlockDataFile[slot][CPU00] );
	if( fs::exists( m_szStepFailBlockDataFile[slot][CPU01] ) ) fs::remove( m_szStepFailBlockDataFile[slot][CPU01] );

	//STEPFAILCHIP
	if( fs::exists( m_szStepFailChipDataFile[slot][CPU00] ) ) fs::remove( m_szStepFailChipDataFile[slot][CPU00] );
	if( fs::exists( m_szStepFailChipDataFile[slot][CPU01] ) ) fs::remove( m_szStepFailChipDataFile[slot][CPU01] );

	//STEPSORTBIN
	if( fs::exists( m_szStepSortBinDataFile[slot][CPU00] ) ) fs::remove( m_szStepSortBinDataFile[slot][CPU00] );
	if( fs::exists( m_szStepSortBinDataFile[slot][CPU01] ) ) fs::remove( m_szStepSortBinDataFile[slot][CPU01] );

	//STEPFCATE 2024.07.22 fatric
	if( fs::exists( m_szStepFCateDataFile[slot][CPU00] ) ) fs::remove( m_szStepFCateDataFile[slot][CPU00] );
	if( fs::exists( m_szStepFCateDataFile[slot][CPU01] ) ) fs::remove( m_szStepFCateDataFile[slot][CPU01] );

	//STEPPCATE 2024.07.22 fatric
	if( fs::exists( m_szStepPCateDataFile[slot][CPU00] ) ) fs::remove( m_szStepPCateDataFile[slot][CPU00] );
	if( fs::exists( m_szStepPCateDataFile[slot][CPU01] ) ) fs::remove( m_szStepPCateDataFile[slot][CPU01] );

	//STEPFAILMODE 2024.07.22 fatric
	if( fs::exists( m_szStepFailModeFile[slot][CPU00] ) ) fs::remove( m_szStepFailModeFile[slot][CPU00] );
	if( fs::exists( m_szStepFailModeFile[slot][CPU01] ) ) fs::remove( m_szStepFailModeFile[slot][CPU01] );

	//STEUSEMODE 2024.07.22 fatric
	if( fs::exists( m_szStepUseModeFile[slot][CPU00] ) ) fs::remove( m_szStepUseModeFile[slot][CPU00] );
	if( fs::exists( m_szStepUseModeFile[slot][CPU01] ) ) fs::remove( m_szStepUseModeFile[slot][CPU01] );

	//CUM BLOCK FAIL
	if( fs::exists( m_szCumFailBlockDataFile[slot][CPU00] ) ) fs::remove( m_szCumFailBlockDataFile[slot][CPU00] );
	if( fs::exists( m_szCumFailBlockDataFile[slot][CPU01] ) ) fs::remove( m_szCumFailBlockDataFile[slot][CPU01] );

	//CUM FAIL CHIP
	if( fs::exists( m_szCumFailChipDataFile[slot][CPU00] ) ) fs::remove( m_szCumFailChipDataFile[slot][CPU00] );
	if( fs::exists( m_szCumFailChipDataFile[slot][CPU01] ) ) fs::remove( m_szCumFailChipDataFile[slot][CPU01] );

	//TEST FAIL
	if( fs::exists( m_szCumTestFailDataFile[slot][CPU00] ) ) fs::remove( m_szCumTestFailDataFile[slot][CPU00] );
	if( fs::exists( m_szCumTestFailDataFile[slot][CPU01] ) ) fs::remove( m_szCumTestFailDataFile[slot][CPU01] );

	//SORT BIN
	if( fs::exists( m_szCumSortBinDataFile[slot][CPU00] ) ) fs::remove( m_szCumSortBinDataFile[slot][CPU00] );
	if( fs::exists( m_szCumSortBinDataFile[slot][CPU01] ) ) fs::remove( m_szCumSortBinDataFile[slot][CPU01] );

	//2024.06.24 - KWMOON CUM FAIL CATE
	if( fs::exists( m_szCumFailCateDataFile[slot][CPU00] ) ) fs::remove( m_szCumFailCateDataFile[slot][CPU00] );
	if( fs::exists( m_szCumFailCateDataFile[slot][CPU01] ) ) fs::remove( m_szCumFailCateDataFile[slot][CPU01] );
	return true;
}

//Text
bool CFileMap::WRITE_BIBID(uint slot, char* szBibid)
{
	//if( !m_bIsSetup ) return false;

	FILE* fp = NULL;
	fp = fopen( m_szBibIdFile[slot], "w" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s open error!", __FUNCTION__, m_szBibIdFile[slot]  );
		return false;
	}
	fprintf( fp, "%s\n", szBibid );
	fflush(fp);
	fclose(fp);
	if( m_bDebugMode )
	{
		fprintf( stdout, "[%20s] BIBID : %s", __FUNCTION__, szBibid );
	}
	return true;
}

//Text
bool CFileMap::WRITE_BIBID(uint slot, uint nbibid)
{
	//if( !m_bIsSetup ) return false;

	FILE* fp = NULL;
	fp = fopen( m_szBibIdFile[slot], "w" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s open error!", __FUNCTION__, m_szBibIdFile[slot]  );
		return false;
	}
	fprintf( fp, "%03X%05d\n", (nbibid>>16)&0xfff, nbibid&0xffff );
	fflush(fp);
	fclose(fp);
	if( m_bDebugMode )
	{
		fprintf( stdout, "[%20s] BIBID : %03X%05d\n", __FUNCTION__, (nbibid>>16)&0xfff, nbibid&0xffff );
	}
	return true;
}

//2024.06.17 - KWMOON
bool CFileMap::WRITE_BIBID(uint slot, stBibId bibid)
{
	//if( !m_bIsSetup ) return false;

	bool bWritten = true;
	std::string section = "";
	char szBibId[16] = { 0x0, };
	memset( szBibId, 0x0, sizeof(szBibId) );
	snprintf( szBibId, sizeof(szBibId), "%03X%05d", (bibid.nBibId>>16)&0xfff, bibid.nBibId&0xffff );

	//BIBID
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_ID);
	bWritten &= UTIL::INI::writeSettings( m_szBibIdFile[slot], section, szBibId );
	//CONN
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_CONN);
	bWritten &= UTIL::INI::writeSettings( m_szBibIdFile[slot], section, bibid.bBibConn?1:0 );
	//PARITY
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_PARITY);
	bWritten &= UTIL::INI::writeSettings( m_szBibIdFile[slot], section, bibid.bParityError?1:0 );
	return bWritten;
}

const char* CFileMap::GET_BIBID(uint slot)
{
//	if( !m_bIsSetup ) return "";

	std::string sBibId = "";
	//BIBID
	std::string section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_ID);
	UTIL::INI::readSettings(  m_szBibIdFile[slot], section, sBibId );

    //20240725 from hy
    snprintf( m_szBidId, sBibId.length() + 1 , "%s", sBibId.c_str() );

	return m_szBidId;

	FILE* fp = NULL;
	fp = fopen( m_szBibIdFile[slot], "r" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] Slot%02d %s open error!", __FUNCTION__, slot+1, m_szBibIdFile[slot]  );
		return "";
	}

	char szLine[64] = { 0x0, };
	memset( m_szBidId, 0x0, sizeof(m_szBidId) );
	char* rtn = fgets( szLine, 64, fp);
	if( rtn != NULL )
	{
		snprintf( m_szBidId, strlen(szLine), "%s", szLine );
	}
	fclose( fp );

	if( m_bDebugMode )
	{
		fprintf( stdout, "[%20s] Slot%02d BIBID : %s\n", __FUNCTION__, slot+1, m_szBidId );
	}
	return m_szBidId;
}

bool CFileMap::GET_BIBID(uint slot, char* pBibId)
{
	bool bRead = true;
	//if( !m_bIsSetup ) return false;
	std::string sBibId = "";
	std::string section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_ID);
	bRead = UTIL::INI::readSettings(  m_szBibIdFile[slot], section, sBibId );
	snprintf( pBibId, sBibId.length() , "%s", sBibId.c_str() );
	return bRead;
/*
	FILE* fp = NULL;
	fp = fopen( m_szBibIdFile[slot], "r" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s open error!", __FUNCTION__, m_szBibIdFile[slot]  );
		return false;
	}

	bool bFound = false;
	char szLine[MAX_LINE_BUFFER] = { 0x0, };
	while( fgets( szLine, MAX_LINE_BUFFER, fp) )
	{
		snprintf( pBibId, strlen(szLine), "%s", szLine );
		bFound = true;
		break;
	}
	fclose( fp );
	if( m_bDebugMode )
	{
		fprintf( stdout, "[%20s] BIBID : %s\n", __FUNCTION__, pBibId );
	}
	return bFound;
*/
}

bool CFileMap::GET_BIBID(uint slot, stBibId& rBibId)
{
	bool bRead = true;
	//if( !m_bIsSetup ) return false;
	int conn = 0;
	int parity_err = 0;
	std::string sBibId  = "";
	std::string section = "";

	//2024.08.05 - KWMOON
	if( !QFile::exists( m_szBibIdFile[slot] ) ) return false;

	//BIB ID
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_ID);
	bRead &= UTIL::INI::readSettings(  m_szBibIdFile[slot], section, sBibId );
	sprintf( rBibId.szBibId, "%s", sBibId.c_str() );

	//CONN
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_CONN);
	bRead &= UTIL::INI::readSettings(  m_szBibIdFile[slot], section, conn );
	rBibId.bBibConn = (conn>0)?true:false;

	//PARITY
	section = std::string(SECTION_BIBID) + std::string("/") + std::string(KEY_BIBID_PARITY);
	bRead &= UTIL::INI::readSettings(  m_szBibIdFile[slot], section, parity_err );
	rBibId.bParityError = (parity_err>0)?true:false;
	return bRead;
}

bool CFileMap::SET_DEVPGM_VAR(uint slot, uint cpu, stDevPgmVariables variables )
{
	//if( !m_bIsSetup ) return false;

	bool bWriten = WriteAppendFile( m_szDevVarFile[slot][cpu], (char*)&variables, sizeof(stDevPgmVariables), false );
	m_bIsDevVar = bWriten;
	return bWriten;
}

bool CFileMap::GET_DEVPGM_VAR(uint slot, uint cpu, stDevPgmVariables* pVar )
{
	//if( !m_bIsSetup ) return false;
	if( !ReadDevPgmVar( slot, cpu ) )
	{
		return false;
	}
	memcpy( pVar, &m_stDevPgmVar, sizeof(stDevPgmVariables) );
	return true;
}

bool CFileMap::SET_INITFAIL_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;

	bool bValidCheck = false;
	//no use
	if( bValidCheck )
	{
		uint expect_size = GET_MAXDUTPERCPU(slot, cpu) * sizeof(char) + sizeof(stStepDataHead);
		if( size != expect_size )
		{
			SET_ERRMSG( "[%20s] %s recv data error, expect size[%d] / written size[%d]!",
						__FUNCTION__, m_szInitFailDataFile[slot][cpu], expect_size, size );
			return false;
		}
	}
	return WriteAppendFile( m_szInitFailDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::SET_TESTITEM( uint slot, uint cpu, stTestItem  items )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szTestItemFile[slot][cpu], (char*)&items, sizeof(stTestItem), false );
}

//2023.04.25 - kwmoon
bool CFileMap::SET_STEPIN_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepInFile[slot][cpu], pData, size );
}

//2023.04.25 - kwmoon
bool CFileMap::SET_STEPOUT_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepOutFile[slot][cpu], pData, size );
}

//2023.04.26 - kwmoon
bool CFileMap::SET_CYCLEINOUT_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCycleInOutFile[slot][cpu], pData, size, false );
}

//2023.04.19 - kwmoon
bool CFileMap::SET_CHIPID_INFO( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szChipInfoFile[slot][cpu], pData, size, false );
}

//2023.04.19 - kwmoon
bool CFileMap::SET_CHIPID_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szChipDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::SET_STEPFAIL_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepFailDataFile[slot][cpu], pData, size, true );
}

bool CFileMap::SET_STEPFAILBLOCK_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepFailBlockDataFile[slot][cpu], pData, size, true );
}

bool CFileMap::SET_STEPFAILCHIP_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepFailChipDataFile[slot][cpu], pData, size, true );
}

bool CFileMap::SET_STEPSORTBIN_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szStepSortBinDataFile[slot][cpu], pData, size, true );
}

//2024.07.22 fatric
bool CFileMap::SET_STEPFCATE_DATA( uint slot, uint cpu, char* pData, uint size )
{
//	if( !m_bIsLotSet ) return false;
	return WriteAppendFile( m_szStepFCateDataFile[slot][cpu], pData, size, true );
}

//2024.07.22 fatric
bool CFileMap::SET_STEPPCATE_DATA( uint slot, uint cpu, char* pData, uint size )
{
//	if( !m_bIsLotSet ) return false;
	return WriteAppendFile( m_szStepPCateDataFile[slot][cpu], pData, size, true );
}

//2024.07.22 fatric
bool CFileMap::SET_STEPFAIL_MODE( uint slot, uint cpu, char* pData, uint size )
{
//	if( !m_bIsLotSet ) return false;
	return WriteAppendFile( m_szStepFailModeFile[slot][cpu], pData, size, true );
}

//2024.08.18 fatric
bool CFileMap::SET_STEPUSE_MODE( uint slot, uint cpu, char* pData, uint size )
{
//	if( !m_bIsLotSet ) return false;
	return WriteAppendFile( m_szStepUseModeFile[slot][cpu], pData, size, true );
}

bool CFileMap::SET_CUMFAILBLOCK_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCumFailBlockDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::SET_CUMFAILCHIP_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCumFailChipDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::SET_CUMFAIL_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCumTestFailDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::SET_CUMSORTBIN_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCumSortBinDataFile[slot][cpu], pData, size, false );
}

//2024.06.24 - KWMOON
bool CFileMap::SET_CUMFAILCATE_DATA( uint slot, uint cpu, char* pData, uint size )
{
	//if( !m_bIsSetup ) return false;
	return WriteAppendFile( m_szCumFailCateDataFile[slot][cpu], pData, size, false );
}

//2024.07.22 fatric
bool CFileMap::SET_BINMAP_DATA( uint slot, uint cpu, char* pData, uint size )
{
//	if( !m_bIsLotSet ) return false;
	return WriteAppendFile( m_szBinmapDataFile[slot][cpu], pData, size, false );
}

bool CFileMap::WriteAppendFile( char* fname, char* pData, uint size, bool bAppend )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = NULL;
	if( bAppend ) { fp = fopen( fname, "ab" ); }
	else          { fp = fopen( fname, "wb" ); }
	if( !fp )
	{
		SET_ERRMSG( "%s open error!", fname );
		return false;
	}
	char* pBuf = pData;
	uint towrite = size;
	uint written = 0;
	uint retry   = 0;
	while( towrite > 0 )
	{
		size_t writebyte = fwrite( pBuf, 1, towrite, fp );
		if( writebyte < 1 ) retry++;
		towrite -= writebyte;
		pBuf    += writebyte;
		written += writebyte;
		if( retry >= 5 )
		{
			SET_ERRMSG( "%s write try count >= 5,  written size : %d, file size :%d\n", fname, written, size );
			break;
		}
	}
	fflush(fp);
	fclose(fp);

	if( size != written )
	{
		SET_ERRMSG( "%s expect size[%d] write size[%d] error!", fname, size, written );
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GET INIT/STEP TEST DONE CHECK
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFileMap::IS_EXIST_DEVPGM_VAR(uint slot)
{
	//if( !m_bIsSetup ) return false;

	bool bDone = false;
	if( fs::exists(m_szDevVarFile[slot][CPU00] ) && fs::exists(m_szDevVarFile[slot][CPU01] ) )
		bDone = true;
	return bDone;
}

bool CFileMap::IS_EXIST_TESTITEM(uint slot)
{
	//if( !m_bIsSetup ) return false;

	bool bDoneCpu0 = false;
	bool bDoneCpu1 = false;
	if( fs::exists(m_szTestItemFile[slot][CPU00] ) )
			bDoneCpu0 = true;
	if( fs::exists(m_szTestItemFile[slot][CPU01] ) )
			bDoneCpu1 = true;
	if( !(bDoneCpu0 && bDoneCpu1) )
	{
		SET_ERRMSG( "[%20s][ ERR] SLOT#%d CPU0 TEST_ITEM:%d / CPU1 TEST_ITEM:%d",
					__FUNCTION__, slot+1, bDoneCpu0, bDoneCpu1 );
	}
	return (bDoneCpu0 && bDoneCpu1);
}

bool CFileMap::GET_TESTITEM(uint slot, uint cpu, stTestItem& rTestItem )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szTestItemFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szTestItemFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	uint expect_size = sizeof(stTestItem);
	uint read = fread( (char*)&rTestItem, 1, expect_size, fp );
	fclose(fp);
	if( expect_size != read )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szTestItemFile[slot][cpu], expect_size, read );
	}
	return true;
}

bool CFileMap::IS_INITTEST_DONE(uint slot)
{
	//if( !m_bIsSetup ) return false;

	bool bDoneCpu0 = false;
	bool bDoneCpu1 = false;
	if( fs::exists(m_szInitFailDataFile[slot][CPU00] ) )
			bDoneCpu0 = true;
	if( fs::exists(m_szInitFailDataFile[slot][CPU01] ) )
			bDoneCpu1 = true;
	if( !(bDoneCpu0 && bDoneCpu1) )
	{
		SET_ERRMSG( "[%20s] SLOT#%d CPU0_INIT DONE:%d / CPU1_INIT_DONE:%d",
					__FUNCTION__, slot+1, bDoneCpu0, bDoneCpu1 );
	}
	return (bDoneCpu0 && bDoneCpu1);
}

bool CFileMap::GET_INITTEST_HEAD(uint slot, uint cpu, stInitTestHead& rHead )
{
	//if( !m_bIsSetup ) return false;

	FILE* fp = fopen( m_szInitFailDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);	//SEEK_END
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}

	int head_size = sizeof(stInitTestHead);
	if( (file_size-head_size) < 1 )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size, head_size );
		delete[] pBuffer;
		return false;
	}
	memcpy( (char*)&rHead, pBuffer          , head_size );
	delete[] pBuffer;
	//if( m_bDebugMode )
	if( 1 )
	{
		struct tm* tBegin = localtime( &rHead.tInitBegin );
		struct tm* tEnd   = localtime( &rHead.tInitEnd   );
		SET_MSG( "%s STEP_NO.%03d STEP_NAME.%s  BEGIN[%04d%02d%02d %02d:%02d:%02d]  END[%04d%02d%02d %02d:%02d:%02d]",
				 __FUNCTION__, rHead.nStepNo, rHead.szStepName,
				 tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec,
				 tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
	}
	return true;
}

bool CFileMap::GET_INITTEST_DATA(uint slot, uint cpu, stInitTestHead& rHead, char* pInitFail, uint size )
{
	//if( !m_bIsSetup ) return false;

	qDebug() << "FILE OPEN[" << slot << "][" << cpu << "] : " << m_szInitFailDataFile[slot][cpu];

	FILE* fp = fopen( m_szInitFailDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu] );
		return false;
	}
	fseek(fp, 0, SEEK_END);	//SEEK_END
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}

	int head_size = sizeof(stInitTestHead);
	if( (file_size-head_size) != size )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size-head_size, size );
		delete[] pBuffer;
		return false;
	}
	memcpy( (char*)&rHead, pBuffer          , head_size );
	memcpy( pInitFail    , pBuffer+head_size, size      );
	delete[] pBuffer;
	//if( m_bDebugMode )
	if( 1 )
	{
		struct tm* tBegin = localtime( &rHead.tInitBegin );
		struct tm* tEnd   = localtime( &rHead.tInitEnd   );
		SET_MSG( "%s STEP_NO.%03d STEP_NAME.%s  BEGIN[%04d%02d%02d %02d:%02d:%02d]  END[%04d%02d%02d %02d:%02d:%02d]",
				 __FUNCTION__, rHead.nStepNo, rHead.szStepName,
				 tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec,
				 tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
	}
	return true;

}

bool CFileMap::GET_INITTEST_FAIL(uint slot, uint cpu, char* pInitFail, uint size)
{
	//if( !m_bIsSetup ) return false;

	FILE* fp = fopen( m_szInitFailDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);	//SEEK_END
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}

	int head_size = sizeof(stInitTestHead);
	if( (file_size-head_size) != size )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szInitFailDataFile[slot][cpu], file_size-head_size, size );
		delete[] pBuffer;
		return false;
	}
	stInitTestHead inithead;
	memcpy( (char*)&inithead, pBuffer          , head_size );
	memcpy( pInitFail       , pBuffer+head_size, size      );
	delete[] pBuffer;
	//if( m_bDebugMode )
	if( 0 )
	{
		struct tm* tBegin = localtime( &inithead.tInitBegin );
		struct tm* tEnd   = localtime( &inithead.tInitEnd   );
		SET_MSG( "%s STEP_NO.%03d STEP_NAME.%s  BEGIN[%04d%02d%02d %02d:%02d:%02d]  END[%04d%02d%02d %02d:%02d:%02d]",
				 __FUNCTION__, inithead.nStepNo, inithead.szStepName,
				 tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec,
				 tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
	}
	return true;

}

bool CFileMap::GET_SORTBIN_DATA(uint slot, uint cpu, uint* pSortBin, uint size)
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szCumSortBinDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szCumSortBinDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);	//SEEK_END
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumSortBinDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	if( file_size < size )
	{
		SET_ERRMSG( "[%20s] %s size for read[%d] > file size[%d]",
					__FUNCTION__, m_szCumSortBinDataFile[slot][cpu], size, file_size );
		size = file_size;
	}
	memcpy( pSortBin , pBuffer, size );
	delete[] pBuffer;
	return true;
}

//2024.07.22 fatric
bool CFileMap::GET_BINMAP_DATA(uint slot, uint cpu, uint* pBinmap, uint size)
{
//	if( !m_bIsLotSet ) return false;
	FILE* fp = fopen( m_szBinmapDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szBinmapDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);	//SEEK_END
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);	//SEEK_END
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szBinmapDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	if( file_size < size )
	{
		SET_ERRMSG( "[%20s] %s size for read[%d] > file size[%d]",
					__FUNCTION__, m_szBinmapDataFile[slot][cpu], size, file_size );
		size = file_size;
	}
	memcpy( pBinmap , pBuffer, size );
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_STEPFAIL_DATA(uint slot, uint cpu, char* pStepFail, uint step_count, uint size)
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szStepFailDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepFailDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( step_count > saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s read step count error, request[%d] > saved[%d]",
					__FUNCTION__, m_szStepFailDataFile[slot][cpu], step_count, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
	char* pSrc  = pBuffer + offset;
	char* pDest = pStepFail+(cpu*size);
	for( uint nStep=0;nStep<step_count;nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size);
		pDest += (size*2);
	}
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_STEPIN_INFO(uint slot, uint cpu, stStepInInfo* pStepIn, uint count)
{
	int fd = open( m_szStepInFile[slot][cpu], O_RDONLY );
	if( fd == -1 )
	{
		SET_ERRMSG( "[%20s] %s read error!",
					__FUNCTION__, m_szStepInFile[slot][cpu] );
		return false;
	}
	uint expect_size = sizeof(stStepInInfo)*count;
	ssize_t read_size = read( fd, (char*)pStepIn, expect_size );
	if( expect_size != read_size )
	{
		SET_ERRMSG( "[%20s] %s read size error, expect[%d] / read[%d]",
					__FUNCTION__, m_szStepInFile[slot][cpu], expect_size, read_size );
		close(fd);
		return false;
	}
	return true;
}

bool CFileMap::GET_STEPOUT_INFO(uint slot, uint cpu, stStepOutInfo* pStepOut, uint count)
{
	int fd = open( m_szStepOutFile[slot][cpu], O_RDONLY );
	if( fd == -1 )
	{
		SET_ERRMSG( "[%20s] %s read error!",
					__FUNCTION__, m_szStepOutFile[slot][cpu] );
		return false;
	}
	uint expect_size = sizeof(stStepOutInfo)*count;
	ssize_t read_size = read( fd, (char*)pStepOut, expect_size );
	if( expect_size != read_size )
	{
		SET_ERRMSG( "[%20s] %s read size error, expect[%d] / read[%d]",
					__FUNCTION__, m_szStepOutFile[slot][cpu], expect_size, read_size );
		close(fd);
		return false;
	}
	return true;
}

bool CFileMap::GET_STEPOUT_LIST(uint slot, uint cpu, std::list<stStepOutInfo>* list )
{
	FILE* fp = fopen( m_szStepOutFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepOutFile[slot][cpu] );
		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);
	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepOutFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint count = file_size / sizeof(stStepOutInfo);
	uint offset = sizeof(stStepOutInfo);
	for( uint i=0;i<count;i++ )
	{
		stStepOutInfo stepout;
		memcpy( &stepout, pBuffer+(i*offset), offset );
		list->push_back( stepout );
	}
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_CHIPID_DATA(uint slot, uint cpu, char* pChipId, uint dutsize, uint chipidsize)
{
	//if( !m_bIsSetup ) return false;

	FILE* fp = fopen( m_szChipDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szChipDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szChipDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint for_read_chip_size = dutsize * chipidsize;	//dutsize=(96*ce*mcp) * sizeof(stchipid)
	if( file_size != for_read_chip_size )
	{
		SET_ERRMSG( "[%20s] %s chipid file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szChipDataFile[slot][cpu], for_read_chip_size, file_size );
		delete[] pBuffer;
		return false;
	}
	memcpy( pChipId, pBuffer, file_size );
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_CUMFAIL_CHIP_DATA(uint slot, uint cpu, char* pCumFailChip, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szCumFailChipDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szCumFailChipDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailChipDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint for_read_failchip_size = size;	//dutsize=(96*ce*mcp) * sizeof(stchipid)
	if( file_size != for_read_failchip_size )
	{
		SET_ERRMSG( "[%20s] %s chipid file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailChipDataFile[slot][cpu], for_read_failchip_size, file_size );
		delete[] pBuffer;
		return false;
	}
	memcpy( pCumFailChip, pBuffer, file_size );
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_STEPFAIL_CHIP_DATA(uint slot, uint cpu, char* pStepFailChip, uint maxstep, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szStepFailChipDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepFailChipDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailChipDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( maxstep != saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s stepfail data step count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailChipDataFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
	char* pSrc  = pBuffer + offset;
	char* pDest = pStepFailChip+(cpu*size);
	for( uint nStep=0;nStep<maxstep;nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size);
		pDest += (size*2);
	}
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_CUMFAIL_BLOCK_DATA(uint slot, uint cpu, char* pCumFailBlock, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szCumFailBlockDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szCumFailBlockDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailBlockDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint for_read_failchip_size = size;	//dutsize=(96*ce*mcp) * sizeof(stchipid)
	if( file_size != for_read_failchip_size )
	{
		SET_ERRMSG( "[%20s] %s cum failblock file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailBlockDataFile[slot][cpu], for_read_failchip_size, file_size );
		delete[] pBuffer;
		return false;
	}
	memcpy( pCumFailBlock, pBuffer, file_size );
	delete[] pBuffer;
	return true;
}

bool CFileMap::GET_STEPFAIL_BLOCK_DATA(uint slot, uint cpu, char* pStepFailBlock, uint maxstep, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szStepFailBlockDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepFailBlockDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailBlockDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}

	uint head_size = sizeof(stStepDataHead);
	uint saved_step_count =  file_size / ( head_size+size );
	//if( maxstep != saved_step_count )	//2023.08.31 - KWMOON(COMMENT)
	if( maxstep > saved_step_count )	//2023.08.31 - KWMOON(need step is bigger than save step, then error)
	{
		SET_ERRMSG( "[%20s] %s stepfail count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailBlockDataFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
	char* pSrc  = pBuffer + offset;
	char* pDest = pStepFailBlock+(cpu*size);
	for( uint nStep=0;nStep<maxstep;nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size);
		pDest += (size*2);
	}
	delete[] pBuffer;
	return true;
}

//2024.06.24 - kwmoon
bool CFileMap::GET_CUMFAILCATE_DATA(uint slot, uint cpu, char* pCumFailCate, uint size )
{
	//if( !m_bIsSetup ) return false;
	FILE* fp = fopen( m_szCumFailCateDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szCumFailCateDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* pBuffer = new char[file_size];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailCateDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint for_read_failchip_size = size;	//dutsize=(96*ce*mcp) * sizeof(stchipid)
	if( file_size != for_read_failchip_size )
	{
		SET_ERRMSG( "[%20s] %s cum failblock file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szCumFailCateDataFile[slot][cpu], for_read_failchip_size, file_size );
		delete[] pBuffer;
		return false;
	}
	memcpy( pCumFailCate, pBuffer, file_size );
	delete[] pBuffer;
	return true;
}

//2024.07.22 fatric
bool CFileMap::GET_STEPFCATE_DATA(uint slot, uint cpu, uint* pStepFailCate, uint maxstep, uint size )
{
//	if( !m_bIsLotSet ) return false;
	FILE* fp = fopen( m_szStepFCateDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepFCateDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	uint* pBuffer = new uint[file_size/sizeof(uint) + 1 ];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFCateDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( maxstep != saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s stepfail data step count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFCateDataFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
	uint* pSrc  = pBuffer + offset/sizeof(uint);
	uint* pDest = pStepFailCate + (cpu*size)/sizeof(uint);
	for( uint nStep=0; nStep<maxstep; nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size)/sizeof(uint);
		pDest += (size  *2   )/sizeof(uint);
	}
	delete[] pBuffer;
	return true;
}

//2024.07.22 fatric
bool CFileMap::GET_STEPPCATE_DATA(uint slot, uint cpu, uint* pStepPassCate, uint maxstep, uint size )
{
//	if( !m_bIsLotSet ) return false;
	FILE* fp = fopen( m_szStepPCateDataFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepPCateDataFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	uint* pBuffer = new uint[file_size/sizeof(uint) + 1 ];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepPCateDataFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( maxstep != saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s stepfail data step count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepPCateDataFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
	uint* pSrc  = pBuffer + offset/sizeof(uint);
	uint* pDest = pStepPassCate + (cpu*size)/sizeof(uint);
	for( uint nStep=0; nStep<maxstep; nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size)/sizeof(uint);
		pDest += (size  *2   )/sizeof(uint);
	}
	delete[] pBuffer;
	return true;
}

//2024.07.22 fatric
bool CFileMap::GET_STEPFAIL_MODE(uint slot, uint cpu, int* pStepFailMode, uint maxstep, uint size )
{
//	if( !m_bIsLotSet ) return false;
	FILE* fp = fopen( m_szStepFailModeFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepFailModeFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int* pBuffer = new int[file_size/sizeof(int)+1];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailModeFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( maxstep != saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s stepfail data step count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepFailModeFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
//	printf("sizeof(stStepDataHead)=%d\n", sizeof(stStepDataHead));	//108
	int* pSrc  = pBuffer + offset/sizeof(int);
	int* pDest = pStepFailMode;
	for( uint nStep=0; nStep<maxstep; nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size)/sizeof(int);
		pDest += (size       )/sizeof(int);
	}
	delete[] pBuffer;
	return true;
}

//2024.08.18 fatric
bool CFileMap::GET_STEPUSE_MODE(uint slot, uint cpu, int* pStepFailMode, uint maxstep, uint size )
{
//	if( !m_bIsLotSet ) return false;
	FILE* fp = fopen( m_szStepUseModeFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s data read error!",
					__FUNCTION__, m_szStepUseModeFile[slot][cpu] );

		return false;
	}
	fseek(fp, 0, SEEK_END);
	uint file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int* pBuffer = new int[file_size/sizeof(int)+1];
	uint read = fread( pBuffer, 1, file_size, fp );
	fclose(fp);

	if( file_size != read )
	{
		SET_ERRMSG( "[%20s] %s read file size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepUseModeFile[slot][cpu], file_size, read );
		delete[] pBuffer;
		return false;
	}
	uint saved_step_count = file_size / ( sizeof(stStepDataHead)+size );
	if( maxstep != saved_step_count )
	{
		SET_ERRMSG( "[%20s] %s stepfail data step count error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szStepUseModeFile[slot][cpu], maxstep, saved_step_count );
		delete[] pBuffer;
		return false;
	}
	uint offset = sizeof(stStepDataHead);
//	printf("sizeof(stStepDataHead)=%d\n", sizeof(stStepDataHead));	//108
	int* pSrc  = pBuffer + offset/sizeof(int);
	int* pDest = pStepFailMode;
	for( uint nStep=0; nStep<maxstep; nStep++ )
	{
		if( !pSrc  ) break;
		if( !pDest ) break;
		memcpy( pDest, pSrc, size );
		pSrc  += (offset+size)/sizeof(int);
		pDest += (size       )/sizeof(int);
	}
	delete[] pBuffer;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GET DEVICE PROGRAM VARIABLEs
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFileMap::ReadDevPgmVar(uint slot, uint cpu)
{
	//if( !m_bIsSetup ) return false;
	//if( !m_bIsDevVar ) return false;

	FILE* fp = NULL;
	fp = fopen( m_szDevVarFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s file read error!",__FUNCTION__, m_szDevVarFile[slot][cpu] );
		return false;
	}
	uint read = fread( (char*)&m_stDevPgmVar, 1, sizeof(stDevPgmVariables), fp );
	fclose(fp);
	if( read != sizeof(stDevPgmVariables) )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szDevVarFile[slot][cpu], sizeof(stDevPgmVariables), read );
		return false;
	}
	return true;
}

bool CFileMap::ReadDevPgmVar(uint slot, uint cpu, stDevPgmVariables* pstDevPgmVar)
{
	//if( !m_bIsSetup ) return false;
	//if( !m_bIsDevVar ) return false;

	FILE* fp = NULL;
	fp = fopen( m_szDevVarFile[slot][cpu], "rb" );
	if( !fp )
	{
		SET_ERRMSG( "[%20s] %s file read error!",__FUNCTION__, m_szDevVarFile[slot][cpu] );
		return false;
	}
	uint read = fread( (char*)pstDevPgmVar, 1, sizeof(stDevPgmVariables), fp );
	fclose(fp);
	if( read != sizeof(stDevPgmVariables) )
	{
		SET_ERRMSG( "[%20s] %s read data size error, expect[%d]/read[%d]!",
					__FUNCTION__, m_szDevVarFile[slot][cpu], sizeof(stDevPgmVariables), read );
		return false;
	}
	return true;
}

bool CFileMap::GET_VERSION(uint slot, uint cpu, char* pBuf)
{
	if( !ReadDevPgmVar(slot,cpu) ) return false;
	snprintf( pBuf , sizeof(m_stDevPgmVar.szVersion) , "%s", m_stDevPgmVar.szVersion );
	return true;
}

const char* CFileMap::GET_REMARK(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return "";
	return m_stDevPgmVar.szVersion;
}

bool CFileMap::GET_DEVICE (uint slot, uint cpu, char* pBuf)
{
	if( !ReadDevPgmVar(slot,cpu) ) return false;
	snprintf( pBuf , sizeof(m_stDevPgmVar.szDevice) , "%s", m_stDevPgmVar.szDevice );
	return true;
}

const char* CFileMap::GET_DEVICE (uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return "";
	return m_stDevPgmVar.szDevice;
}

const char* CFileMap::GET_LOTNO()
{
//	if( !m_bIsSetup ) return "";
	return m_szLotNo;
}

const char* CFileMap::GET_PARTNO()
{
//	if( !m_bIsSetup ) return "";
	/*
	FILE* fp = fopen( m_szPartNoFile, "r" );
	if( !fp ) return "";
	char szLine[MAX_LINE_BUFFER] = { 0x0, };
	if( fgets( szLine, MAX_LINE_BUFFER, fp) )
	{
		snprintf( m_szPartNo, strlen(szLine), "%s", szLine );
	}
	*/
	return m_szPartNo;
}

const char* CFileMap::GET_OPRID()
{
//	if( !m_bIsSetup ) return "";
	/*
	FILE* fp = fopen( m_szOprIdFile, "r" );
	if( !fp ) return "";
	char szLine[MAX_LINE_BUFFER] = { 0x0, };
	if( fgets( szLine, MAX_LINE_BUFFER, fp) )
	{
		snprintf( m_szOprId, strlen(szLine), "%s", szLine );
	}
	*/
	return m_szOprId;
}

time_t* CFileMap::GET_CYCLE_BEGIN_TIME(uint slot, uint cpu)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
	if( fd == -1 )
	{
		return &m_time_t;
	}
	stCycleInfo cycleinfo;
	if( read( fd, (char*)&cycleinfo, sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
	{
		return &m_time_t;
	}
	close(fd);
	m_time_t = cycleinfo.tCycleBegin;
	return &m_time_t;
}

time_t* CFileMap::GET_CYCLE_END_TIME  (uint slot, uint cpu)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
	if( fd == -1 )
	{
		return &m_time_t;
	}
	stCycleInfo cycleinfo;
	if( read( fd, (char*)&cycleinfo, sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
	{
		return &m_time_t;
	}
	close(fd);
	m_time_t = cycleinfo.tCycleEnd;
	return &m_time_t;
}

time_t* CFileMap::GET_CYCLE_BEGIN_TIME(uint slot)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	stCycleInfo cycleinfo[2];
	int nMaxCpu = 1;
	bool bCpu2x = GET_CPU2X(slot,CPU00);
	if( bCpu2x ) nMaxCpu = 2;
	for( int cpu=0;cpu<nMaxCpu;cpu++ )
	{
		int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
		if( fd == -1 )
		{
			cycleinfo[cpu].tCycleBegin = time(NULL);
			continue;
		}
		if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
		{
			cycleinfo[cpu].tCycleBegin = time(NULL);
			close(fd);
		}
	}
	if( bCpu2x )
	{
		if( difftime( cycleinfo[CPU00].tCycleBegin, cycleinfo[CPU01].tCycleBegin ) < 0 )
			m_time_t = cycleinfo[CPU00].tCycleBegin;
		else
			m_time_t = cycleinfo[CPU01].tCycleBegin;
	}
	else
	{
		m_time_t = cycleinfo[CPU00].tCycleBegin;
	}
	return &m_time_t;
}

time_t* CFileMap::GET_CYCLE_END_TIME  (uint slot)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	stCycleInfo cycleinfo[2];
	int nMaxCpu = 1;
	bool bCpu2x = GET_CPU2X(slot,CPU00);
	if( bCpu2x ) nMaxCpu = 2;
	for( int cpu=0;cpu<nMaxCpu;cpu++ )
	{
		int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
		if( fd == -1 )
		{
			cycleinfo[cpu].tCycleEnd = time(NULL);
			continue;
		}
		if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
		{
			cycleinfo[cpu].tCycleEnd = time(NULL);
			close(fd);
		}
	}
	if( bCpu2x )
	{
		if( difftime( cycleinfo[CPU00].tCycleEnd, cycleinfo[CPU01].tCycleEnd ) < 0 )
			m_time_t = cycleinfo[CPU01].tCycleEnd;
		else
			m_time_t = cycleinfo[CPU00].tCycleEnd;
	}
	else
	{
		m_time_t = cycleinfo[CPU00].tCycleEnd;
	}
	return &m_time_t;
}

time_t* CFileMap::GET_CYCLE_BEGIN_TIME()
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	time_t t_begin_tme = time(NULL);
	for( int slot=0;slot<MAX_SLOT;slot++ )
	{
		if( !IS_EXIST_DEVPGM_VAR(slot) )
			continue;

		stCycleInfo cycleinfo[2];
		int nMaxCpu = 1;
		bool bCpu2x = GET_CPU2X(slot,CPU00);
		if( bCpu2x ) nMaxCpu = 2;
		for( int cpu=0;cpu<nMaxCpu;cpu++ )
		{
			int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
			if( fd == -1 )
			{
				cycleinfo[cpu].tCycleBegin = time(NULL);
				continue;
			}
			if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
			{
				cycleinfo[cpu].tCycleBegin = time(NULL);
			}
			close(fd);
		}
		if( bCpu2x )
		{
			if( difftime( cycleinfo[CPU00].tCycleBegin, cycleinfo[CPU01].tCycleBegin ) <= 0 )
				t_begin_tme = cycleinfo[CPU00].tCycleBegin;
			else
				t_begin_tme = cycleinfo[CPU01].tCycleBegin;
		}
		else
		{
			t_begin_tme = cycleinfo[CPU00].tCycleBegin;
		}
		if( difftime( m_time_t, t_begin_tme) > 0 )
		{
			m_time_t = t_begin_tme;
		}
	}
	return &m_time_t;
}

time_t* CFileMap::GET_CYCLE_END_TIME  ()
{
	m_time_t = 0;	//time(NULL);	//2023.08.18 - kwmoon( 0 -> time(NULL) )
//	if( !m_bIsSetup ) return &m_time_t;

	time_t t_end_time = time(NULL);
	for( int slot=0;slot<MAX_SLOT;slot++ )
	{
		if( !IS_EXIST_DEVPGM_VAR(slot) )
			continue;

		stCycleInfo cycleinfo[2];
		int nMaxCpu = 1;
		bool bCpu2x = GET_CPU2X(slot,CPU00);
		if( bCpu2x ) nMaxCpu = 2;
		for( int cpu=0;cpu<nMaxCpu;cpu++ )
		{
			int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
			if( fd == -1 )
			{
				cycleinfo[cpu].tCycleEnd = time(NULL);
				continue;
			}
			if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
			{
				cycleinfo[cpu].tCycleEnd = time(NULL);
				close(fd);
			}
		}
		if( bCpu2x )
		{
			if( difftime( cycleinfo[CPU00].tCycleEnd, cycleinfo[CPU01].tCycleEnd ) <= 0 )
				t_end_time = cycleinfo[CPU01].tCycleEnd;
			else
				t_end_time = cycleinfo[CPU00].tCycleEnd;
		}
		else
		{
			t_end_time = cycleinfo[CPU00].tCycleEnd;
		}
		if( difftime( m_time_t, t_end_time) < 0 )
		{
			m_time_t = t_end_time;
		}
	}
	return &m_time_t;
}

bool CFileMap::GET_LAST_CYCLE_END_SLOT( uint& rslot )
{
	rslot = -1;
	//if( !m_bIsSetup ) return false;

	m_time_t = 0;
	time_t t_end_time = time(NULL);
	bool bOk = false;
	for( int slot=0;slot<MAX_SLOT;slot++ )
	{
		if( !IS_EXIST_DEVPGM_VAR(slot) )
			continue;

		stCycleInfo cycleinfo[2];
		int nMaxCpu = 1;
		bool bCpu2x = GET_CPU2X(slot,CPU00);
		if( bCpu2x ) nMaxCpu = 2;
		for( int cpu=0;cpu<nMaxCpu;cpu++ )
		{
			int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
			if( fd == -1 )
			{
				cycleinfo[cpu].tCycleEnd = 0;
				continue;
			}
			if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			//2023.08.22 - KWMOON
			if( !cycleinfo[cpu].bEnd )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			close(fd);
		}
		if( bCpu2x )
		{
			if( difftime( cycleinfo[CPU00].tCycleEnd, cycleinfo[CPU01].tCycleEnd ) <= 0 )
			{
				t_end_time = cycleinfo[CPU01].tCycleEnd;
			}
			else
			{
				t_end_time = cycleinfo[CPU00].tCycleEnd;
			}
		}
		else
		{
			t_end_time = cycleinfo[CPU00].tCycleEnd;
		}

		if( difftime( m_time_t, t_end_time) < 0 )
		{
			m_time_t = t_end_time;
			rslot = slot;
			bOk = true;
		}
	}
	return bOk;
}

bool CFileMap::GET_LAST_CYCLE_END_SLOT_CPU( uint& rslot, uint& rcpu )
{
	rslot = -1;
	rcpu  = -1;
	//if( !m_bIsSetup ) return false;

	m_time_t = 0;
	time_t t_end_time = time(NULL);
	bool bOk = false;
	for( int slot=0;slot<MAX_SLOT;slot++ )
	{
		if( !IS_EXIST_DEVPGM_VAR(slot) )
			continue;

		stCycleInfo cycleinfo[2];
		int nMaxCpu = 1;
		bool bCpu2x = GET_CPU2X(slot,CPU00);
		if( bCpu2x ) nMaxCpu = 2;
		for( int cpu=0;cpu<nMaxCpu;cpu++ )
		{
			int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
			if( fd == -1 )
			{
				cycleinfo[cpu].tCycleEnd = 0;
				continue;
			}
			if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			//2023.08.22 - KWMOON
			if( !cycleinfo[cpu].bEnd )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			close(fd);
		}
		uint cpu = CPU00;
		if( bCpu2x )
		{
			if( difftime( cycleinfo[CPU00].tCycleEnd, cycleinfo[CPU01].tCycleEnd ) <= 0 )
			{
				t_end_time = cycleinfo[CPU01].tCycleEnd;
				cpu = CPU01;
			}
			else
			{
				t_end_time = cycleinfo[CPU00].tCycleEnd;
				cpu = CPU00;
			}
		}
		else
		{
			t_end_time = cycleinfo[CPU00].tCycleEnd;
			cpu = CPU00;
		}

        if( difftime( m_time_t, t_end_time) <= 0 )
		{
			m_time_t = t_end_time;
			rslot = slot;
			rcpu  = cpu;
			bOk = true;
		}
	}
	return bOk;
}

bool CFileMap::GET_LAST_CYCLE_END_SLOT_CPU( uint& rslot, uint& rcpu, bool* pbValid )
{
	rslot = -1;
	rcpu  = -1;
	//if( !m_bIsSetup ) return false;

	m_time_t = 0;
	time_t t_end_time = time(NULL);
	bool bOk = false;
	for( int slot=0;slot<MAX_SLOT;slot++ )
	{
		if( !pbValid[slot]             ) continue;
		if( !IS_EXIST_DEVPGM_VAR(slot) ) continue;

		stCycleInfo cycleinfo[2];
		int nMaxCpu = 1;
		bool bCpu2x = GET_CPU2X(slot,CPU00);
		if( bCpu2x ) nMaxCpu = 2;
		for( int cpu=0;cpu<nMaxCpu;cpu++ )
		{
			int fd = open( m_szCycleInOutFile[slot][cpu], O_RDONLY );
			if( fd == -1 )
			{
				cycleinfo[cpu].tCycleEnd = 0;
				continue;
			}
			if( read( fd, (char*)&cycleinfo[cpu], sizeof(stCycleInfo) ) != sizeof(stCycleInfo) )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			//2023.08.22 - KWMOON
			if( !cycleinfo[cpu].bEnd )
			{
				cycleinfo[cpu].tCycleEnd = 0;
			}
			close(fd);
		}
		uint cpu = CPU00;
		if( bCpu2x )
		{
			if( difftime( cycleinfo[CPU00].tCycleEnd, cycleinfo[CPU01].tCycleEnd ) <= 0 )
			{
				t_end_time = cycleinfo[CPU01].tCycleEnd;
				cpu = CPU01;
			}
			else
			{
				t_end_time = cycleinfo[CPU00].tCycleEnd;
				cpu = CPU00;
			}
		}
		else
		{
			t_end_time = cycleinfo[CPU00].tCycleEnd;
			cpu = CPU00;
		}

		if( difftime( m_time_t, t_end_time) <= 0 )
		{
			m_time_t = t_end_time;
			rslot = slot;
			rcpu  = cpu;
			bOk = true;
		}
	}
	return bOk;
}

const char* CFileMap::GET_STATION()
{
	static std::string hostname = "";//UTIL::TCPIP::GetHostName();
	hostname = UTIL::TCPIP::GetHostName();
	return hostname.c_str();
}

int CFileMap::GET_ZONE()
{
	int zone = 0;
	atstring shost = UTIL::TCPIP::GetHostName();
	atstring szone = shost.Right(1);
	bool isnum = true;
	for( char const &c: szone )
	{
		if( std::isdigit(c) == 0 )
		{
			isnum = false;
			break;
		}
	}
	if( isnum )
		zone = std::stoi( szone );
	else
		zone = 1;
	return zone;
}

time_t* CFileMap::GET_STEP_BEGIN_TIME(uint slot, uint stepno)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	stStepInInfo stepin[2];
	int nMaxCpu = 1;
	bool bCpu2x = GET_CPU2X(slot,CPU00);
	if( bCpu2x ) nMaxCpu = 2;
	for( int cpu=0;cpu<nMaxCpu;cpu++ )
	{
		int fd = open( m_szStepInFile[slot][cpu], O_RDONLY );
		if( fd == -1 )
		{
			stepin[cpu].tStepBegin = time(NULL);
			continue;
		}

		ssize_t read_size = read( fd, (char*)&stepin[cpu], sizeof(stStepInInfo) );
		do
		{
			if( read_size != sizeof(stStepInInfo) ) break;
			if( stepin[cpu].nStepNo == stepno )
			{
				break;
			}
			read_size = read( fd, (char*)&stepin[cpu], sizeof(stStepInInfo) );

		}while( read_size == sizeof(stStepInInfo) );
		close(fd);
	}
	if( bCpu2x )
	{
		if( difftime( stepin[CPU00].tStepBegin, stepin[CPU01].tStepBegin ) < 0 )
			m_time_t = stepin[CPU00].tStepBegin;
		else
			m_time_t = stepin[CPU01].tStepBegin;
	}
	else
	{
		m_time_t = stepin[CPU00].tStepBegin;
	}
	return &m_time_t;
}

time_t* CFileMap::GET_STEP_END_TIME  (uint slot, uint stepno)
{
	m_time_t = time(NULL);
//	if( !m_bIsSetup ) return &m_time_t;

	stStepOutInfo stepout[2];
	int nMaxCpu = 1;
	bool bCpu2x = GET_CPU2X(slot,CPU00);
	if( bCpu2x ) nMaxCpu = 2;
	for( int cpu=0;cpu<nMaxCpu;cpu++ )
	{
		int fd = open( m_szStepOutFile[slot][cpu], O_RDONLY );
		if( fd == -1 )
		{
			stepout[cpu].tStepEnd = time(NULL);
			continue;
		}

		ssize_t read_size = read( fd, (char*)&stepout[cpu], sizeof(stStepOutInfo) );
		do
		{
			if( read_size != sizeof(stStepOutInfo) ) break;
			if( stepout[cpu].nStepNo == stepno )
			{
				break;
			}
			read_size = read( fd, (char*)&stepout[cpu], sizeof(stStepOutInfo) );

		}while( read_size == sizeof(stStepOutInfo) );
		close(fd);
	}
	if( bCpu2x )
	{
		if( difftime( stepout[CPU00].tStepEnd, stepout[CPU01].tStepEnd ) < 0 )
			m_time_t = stepout[CPU01].tStepEnd;
		else
			m_time_t = stepout[CPU00].tStepEnd;
	}
	else
	{
		m_time_t = stepout[CPU00].tStepEnd;
	}
	return &m_time_t;
}

//2023.05.03 - KWMOON
uint CFileMap::GET_MAX_STEP(uint slot, uint cpu)
{
	uint max_out_step = 0;
	uint step_out_size = sizeof(stStepOutInfo);
	uint step_in_size  = sizeof(stStepInInfo);
	(void)step_in_size;
	int fd = open( m_szStepOutFile[slot][cpu], O_RDONLY );
	if( fd == -1 )
		return 0;

	off_t fsize;
	if( (fsize = lseek(fd, 0, SEEK_END)) < 0 )
	{
		fprintf(stderr, "%s lseek error\n", m_szStepOutFile[slot][cpu] );
		return 0;
	}
	close(fd);

	max_out_step = fsize / step_out_size;
//	printf("%s:%d  fsize=%d   step_out_size=%d   max_out_step=%d\n", __FUNCTION__, __LINE__, fsize, step_out_size, max_out_step);

	return max_out_step;
}


int CFileMap::GET_DEVICE_TYPE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nDeviceType;
}

uint CFileMap::GET_MAXDUTPERCPU(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nMaxDutPerCpu;
}

int CFileMap::GET_CE_COUNT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nCe;
}

int CFileMap::GET_CHIP_COUNT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nChip;
}

int CFileMap::GET_BLOCKPERCE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBlockPerCe;
}

int CFileMap::GET_BLOCKPERCHIP(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBlockPerCe/m_stDevPgmVar.nChip;
}

int CFileMap::GET_BLOCKPERDUTBL(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBlockPerDutBl;
}
int CFileMap::GET_CHIPPERDUT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nChipPerDut;
}

int CFileMap::GET_CHIPPERDUTBL(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nChipPerDutBl;
}

int CFileMap::GET_DUTPERFC(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nDutPerFc;
}

int CFileMap::GET_DUTPERCPU(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nMaxDutPerCpu;
}

int CFileMap::GET_BLOCKSHIFT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBlockShift;
}

int CFileMap::GET_PAGESHIFT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nPageShift;
}

int CFileMap::GET_COLSIZE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nColSize;
}

int CFileMap::GET_BLOCKLIMIT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nblockLimit;
}

int CFileMap::GET_BLOCKLIMITPERCE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nblockLimitPerCe;
}

int CFileMap::GET_ADDRLIMIT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nAddrLimit;
}

int CFileMap::GET_FAILMODE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nFailMode;
}

int CFileMap::GET_SCANINVISIBLE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nScanInvisible;
}

int CFileMap::GET_BIBSCAN(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBibScan;
}

int CFileMap::GET_IOSIZE(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nIoSize;
}

int CFileMap::GET_BLOCKPERDUT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBlockPerDut;
}

int CFileMap::GET_BIBX(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBibX;
}

int CFileMap::GET_BIBY(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return -1;
	return m_stDevPgmVar.nBibY;
}

int CFileMap::GET_CPU2X(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return 0;
	return m_stDevPgmVar.nCpu2X;
}

int CFileMap::GET_BITIME(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return 0;
	return m_stDevPgmVar.nBiTime;
}

int CFileMap::GET_AGINGTIME(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return 0;
	return m_stDevPgmVar.nAgingTime;
}

//2024.06.24 - KWMOON
int CFileMap::GET_MAX_CATEGORY_BIT(uint slot, uint cpu)
{
	if( !ReadDevPgmVar(slot,cpu) ) return 0;
	return m_stDevPgmVar.nMaxCategoryBit;
}

std::vector<std::string> CFileMap::GET_STEPNAMES(uint slot, uint cpu)
{
	const char* pszFile = m_szStepOutFile[slot][cpu];
	std::vector<std::string> vStep;
	vStep.clear();

	off_t fsize;
	int fd = open( pszFile, O_RDONLY );
	if( fd < 0 )
	{
		return vStep;
	}
	if( (fsize = lseek(fd, 0, SEEK_END)) < 0 )
	{
		fprintf(stderr, "%s lseek error\n", pszFile );
		return vStep;
	}
	lseek(fd, 0, SEEK_SET );

	uint nSize = sizeof(stStepOutInfo);
	uint nStep = fsize / sizeof(stStepOutInfo);
	for( uint i=0;i<nStep;i++ )
	{
		stStepOutInfo stepout;
		int nRead = read( fd, (char*)&stepout, sizeof(stStepOutInfo) );
		if( nRead < 1 ) break;
		if( nRead != sizeof(stStepOutInfo) )
		{
			fprintf(stderr, "%s read size error, expect[%d] read[%d]", pszFile, nSize, nRead);
			break;
		}
		vStep.push_back( std::string(stepout.szStepName) );
//		printf("%s:%d  %3d:%s\n", __FUNCTION__, __LINE__, stepout.nStepNo, stepout.szStepName );
		//if ( lseek( fd, sizeof(stStepOutInfo), SEEK_CUR) < 0 )
		//{
		//	fprintf(stderr, "%s seek error (%d step)", pszFile, i);
		//	break;
		//}
	}
	close(fd);

	return vStep;
}
