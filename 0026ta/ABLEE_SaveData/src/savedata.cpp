#include "savedata.hpp"

namespace fs = std::filesystem;

CSaveData::CSaveData()
{
	m_pDataMgr      = nullptr;
	m_pFls          = nullptr;
	m_pCumFailBlock = nullptr;
	m_pMrd          = nullptr;
	m_pChipId       = nullptr;
	m_pEqpr         = nullptr;
	m_pChdr         = nullptr;
	m_pItemCum      = nullptr;

	m_bUiMode     = false;
	m_szLotNo [0] = '\0';
	m_szPartNo[0] = '\0';
	m_szOprId [0] = '\0';
	m_iSublot     = 0;
}

CSaveData::~CSaveData()
{
	if( m_pDataMgr      != nullptr ) { delete m_pDataMgr     ; m_pDataMgr      = nullptr; }
	if( m_pFls          != nullptr ) { delete m_pFls         ; m_pFls          = nullptr; }
	if( m_pCumFailBlock != nullptr ) { delete m_pCumFailBlock; m_pCumFailBlock = nullptr; }
	if( m_pMrd          != nullptr ) { delete m_pMrd         ; m_pMrd          = nullptr; }
	if( m_pChipId       != nullptr ) { delete m_pChipId      ; m_pChipId       = nullptr; }
	if( m_pEqpr         != nullptr ) { delete m_pEqpr        ; m_pEqpr         = nullptr; }
	if( m_pChdr         != nullptr ) { delete m_pChdr        ; m_pChdr         = nullptr; }
	if( m_pItemCum      != nullptr ) { delete m_pItemCum     ; m_pItemCum      = nullptr; }
}

void CSaveData::ShowHelp()
{
	printf("execute program {arg1} {arg2}\n" );
	printf("    arg1      -----  lot no\n");
	printf("    arg2      ----- part no\n");
}

//2024.06.13 - kwmoon
bool CSaveData::LoadUiAutoMode()
{
	char szUiModeFile[MAX_FILE_PATH]={0x0,};
	memset( szUiModeFile, 0x0, sizeof(szUiModeFile) );
	snprintf( szUiModeFile, MAX_FILE_PATH, "%s/%s", m_szDataRoot, UIMODE_INI );

	//UI-MODE
	std::string ui_mode = "";
	std::string section = std::string(SECTION_UIMODE) + std::string("/") + std::string(KEY_UIMODE);
	UTIL::INI::readSettings( szUiModeFile, section, ui_mode );
	if( ui_mode == "auto" ) return true;
	return false;
}

bool CSaveData::InitDataDir()
{
	char* env_data = getenv(ATDATA_DIR_ENV);

	if( env_data != NULL )
	{
		snprintf( m_szDataRoot, MAX_FILE_PATH, "%s/%s", env_data, DIR_ATDATA_NAME );
	}
	else
	{
		snprintf( m_szDataRoot, MAX_FILE_PATH, "%s/%s", DIR_ATDATA_PATH, DIR_ATDATA_NAME );
	}

	return true;
}

bool CSaveData::ClearDirectory()
{
	char* env_run  = getenv(ATRUN_DIR_ENV);

	fs::path pd( m_szDataDir );
	if( !fs::exists(pd) )
	{
		fprintf( stdout, "[ ERR]DATA DIR : %s is not exist!\n", m_szDataDir  );
		return false;
	}

	if( env_run != NULL )
	{
		snprintf( m_szReportDir, MAX_FILE_PATH, "%s/atdb/%s", env_run, m_szPartNo );
	}
	else
	{
		std::string home_dir = getenv("HOME");
		snprintf( m_szReportDir, PATH_MAX, "%s/%s/%s/%s", home_dir.c_str(), "atsystem", DIR_REPORT_PATH, m_szLotNo );
	}
	bool bOk = true;
	fs::path pr( m_szReportDir );
	if( !fs::exists(pr) )
	{
		bOk = fs::create_directory(pr);
	}
	else
	{
		for( const auto& entry : fs::directory_iterator(pr) )
		{
			if( fs::is_directory( entry.path() ) )
				fs::remove_all( entry.path() );
			else
				fs::remove( entry.path() );
		}
	}
	//printf( "[%4s]REPORT DIR : %s[%s]\n", bOk?"INFO":" ERR", m_szReportDir, bOk?"OK":"NG" );

	return bOk;
}

bool CSaveData::StbyReportDir()
{
	char szDbDir[MAX_FILE_PATH]={0x0,};		//home/atsystem/atdb
	char* env_run  = getenv(ATRUN_DIR_ENV);
	if( env_run != NULL )
	{
		snprintf( m_szReportDir, MAX_FILE_PATH, "%s/%s/%s/%s/%s/0", env_run, DIR_REPORT_PATH, m_bUiMode?MASS_DIR:ENGR_DIR, m_szPartNo, m_szLotNo );
	}
	else
	{
		std::string home_dir = getenv("HOME");
		snprintf( m_szReportDir, PATH_MAX, "%s/%s/%s/%s/%s/%s/0", home_dir.c_str(), "atsystem", DIR_REPORT_PATH, m_bUiMode?MASS_DIR:ENGR_DIR, m_szPartNo, m_szLotNo );
	}
	bool bOk = true;
	fs::path pr( m_szReportDir );
	if( !fs::exists(pr) )
	{
		bOk = fs::create_directories(pr);
		if( !bOk )
		{
			fprintf( stdout, "[ ERR] Make directory '%s' error!", m_szReportDir );
			return false;
		}
	}
	fprintf( stdout, "[INFO] Save Path : %s\n", m_szReportDir );
	return true;
}

int CSaveData::parseArgument( int argc, char* argv[] )
{
	if ( ! m_OD.ParseRCmd()            ) return 0;
	if ( ! m_OD.MkReportDir()          ) return 0;
	if ( ! m_OD.GatherFoundationData() ) return 0;

	m_pFls          = new CFls         (&m_OD);
	m_pCumFailBlock = new CCumFailBlock(&m_OD);
	m_pMrd          = new CMrd         (&m_OD);
	m_pChipId       = new CChipId      (&m_OD);
	m_pEqpr         = new CEqpr	       (&m_OD);
	m_pChdr         = new CChdr        (&m_OD);
	m_pItemCum      = new CItemCum     (&m_OD);
	return SaveData();


//	InitDataDir();
//	//m_bUiMode = LoadUiAutoMode();
//	//snprintf( m_szDataDir, MAX_FILE_PATH, "%s/%s", m_szDataRoot, m_bUiMode?AUTO_DIR:ENGR_DIR );
//	//if( !m_bUiMode )
//	//{
//	//
//	//}
//	//else
//	//{
//	//	//START_BI_CMD Parse
//	//	//TGET & BOARD_MAINT_INFO Parse
//	//}
//	if( !StbyReportDir() ) return -1;
//
//	m_pDataMgr = new CFileMap( m_szDataDir );
//	if( !m_pDataMgr->LOT_SET( m_szLotNo ) )
//	{
//		printf( "%s\n", m_pDataMgr->GET_ERRMSG() );
//		delete m_pDataMgr;
//		m_pDataMgr = nullptr;
//		return -1;
//	}
//
//	m_pFls          = new CFls         (m_pDataMgr);
//	m_pCumFailBlock = new CCumFailBlock(m_pDataMgr);
//	m_pMrd          = new CMrd         (m_pDataMgr);
//	m_pChipId       = new CChipId      (m_pDataMgr);
//	m_pEqpr         = new CEqpr	       (m_pDataMgr);
//	m_pChdr         = new CChdr        (m_pDataMgr);
//	m_pItemCum      = new CItemCum     (m_pDataMgr);
//	return SaveData();
}

bool CSaveData::SaveData()
{
	bool bSaved = true;
	printf("PRG:0\n");
	bSaved &= GenFls();
	if( bSaved ) printf("PRG:25\n");
	else return false;

	bSaved &= GenCumFailBlock();
	if( bSaved ) printf("PRG:50\n");
	else return false;

	bSaved &= GenMrd();
	if( bSaved ) printf("PRG:60\n");
	else return false;

	bSaved &= GenChipId();
	if( bSaved ) printf("PRG:70\n");
	else return false;

	bSaved &= GenEqpr();
	if( bSaved ) printf("PRG:80\n");
	else return false;

	bSaved &= GenChdr();
	if( bSaved ) printf("PRG:90\n");
	else return false;

	bSaved &= GenItemCum();
	if( bSaved ) printf("PRG:100\n");
	else return false;

	return bSaved;
}

bool CSaveData::GenFls()
{
	if( m_pFls == nullptr ) return false;
	return m_pFls->GenFls();
}

bool CSaveData::GenCumFailBlock()
{
	if( m_pCumFailBlock == nullptr ) return false;
	return m_pCumFailBlock->GenCumFailBlock();
	return true;
}

bool CSaveData::GenMrd()
{
	if( m_pMrd == nullptr ) return false;
	return m_pMrd->GenMrd();
	return true;
}

bool CSaveData::GenChipId()
{
	if( m_pChipId == nullptr ) return false;
	return m_pChipId->GenChipId();
	return true;
}

bool CSaveData::GenEqpr()
{
	if( m_pEqpr == nullptr ) return false;
	return m_pEqpr->GenEqpr();
	return true;
}

bool CSaveData::GenChdr()
{
	if( m_pChdr == nullptr ) return false;
	return m_pChdr->GenChdr();
	return true;
}

bool CSaveData::GenItemCum()
{
	if( m_pItemCum == nullptr ) return false;
	return m_pItemCum->GenItemCum();
	return true;
}

bool CSaveData::WriteFile(int fd, const char* data, uint size )
{
	ssize_t written = write( fd, data, size );
	if( size != written ) return false;
	return true;
}
