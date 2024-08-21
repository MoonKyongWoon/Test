#ifndef CTESTDATAMGR_HPP
#define CTESTDATAMGR_HPP

//////////////////////////////////////////////////////////////////////////
/// TESTMGR & MAIN UI TEST DATA FILE I/F
//////////////////////////////////////////////////////////////////////////
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include "atstring.hpp"
#include "commstruct.hpp"
#include "protocol.hpp"
#include "sysdef.hpp"
#include "sysini.hpp"
#include "util.hpp"

#define MAX_DIR_FILENAME			(256)
#define MAX_LINE_BUFFER				(256)

////////////////////////////////////////////////////////////////////////////
/// TEST DATA STRUCT & FILE NAME
////////////////////////////////////////////////////////////////////////////
#define EXT_DAT							".dat"
#define EXT_TXT							".txt"
#define EXT_INI							".ini"
#define EXT_RCMD						".rcmd"
#define ENGR_DIR						"engr"
#define AUTO_DIR						"auto"
#define TGET_DIR						"TGET"
#define BOARD_MAINT_DIR					"BOARD_MAINT_INFO"
#define BI_OPEN_LOT_DIR					"BI_OPEN_LOT"

#define BIBID_TXT_NAME					"BIBID.ini"			//Text
#define DEVPGM_VAR_NAME					"DEVPGM_VAR"		//Bin
#define TESTITEM_NAME					"TESTITEM"			//Bin
#define INITFAIL_NAME					"INITFAIL"			//Bin
#define STEPIN_NAME						"STEPIN"			//Bin
#define STEPOUT_NAME					"STEPOUT"			//Bin
#define CYCLEINOUT_NAME					"CYCLEINOUT"		//Bin
#define CHIPDATA_NAME					"CHIPIDDATA"		//Bin
#define CHIPINFO_NAME					"CHIPIDINFO"		//Bin
#define STEPFAIL_NAME					"STEPFAIL"			//Bin
#define STEPFAILBLOCK_NAME				"STEPFAILBLOCK"		//Bin
#define STEPFAILCHIP_NAME				"STEPFAILCHIP"		//Bin
#define STEPSORTBIN_NAME				"STEPSORTBIN"		//Bin

#define STEPFCATE_NAME					"STEPFCATE"			//Bin	//2024.07.22 fatric
#define STEPPCATE_NAME					"STEPPCATE"			//Bin	//2024.07.22 fatric
#define STEPFAIL_MODE					"STEPFAILMODE"		//Bin	//2024.07.22 fatric
#define STEPUSE_MODE					"STEPUSEMODE"		//Bin	//2024.08.18 fatric

#define CUMFAILBLOCK_NAME				"CUMFAILBLOCK"		//Bin
#define CUMFAILCHIP_NAME				"CUMFAILCHIP"		//Bin
#define CUMTESTFAIL_NAME				"CUMTESTFAIL"		//Bin
#define CUMSORTBIN_NAME					"CUMSORTBIN"		//Bin
#define CUMFAILCATE_NAME				"CUMFAILCATE"		//Bin - 2024.04.24 - KWMOON

#define BINMAP_NAME						"BINMAP"			//Bin	//2024.07.22 fatric

class T_LOT;

class CFileMap
{
public:
	CFileMap( const char* pDataDir, bool mode );
	CFileMap( const char* pDataDir );
//setup
public:
	QString DATA_ROOTDIR();
	void LOAD_FILEMAP_INFO( const char* pDataDir);
	void LOAD_FILEMAP_INFO();
	void SET_DEBUGMODE(bool);
	bool SET_ENGR_SETUP(char* l, char* p, char* o);
	bool LOT_SET( const char*);
	bool GET_UIAUTOMODE();
	bool SET_UIAUTOMODE(bool);
	bool MAKE_ENGR_START_BICMD_FILE( char*, char*);
	bool MAKE_ENGR_TGET_FILE( char*, char*);
	bool MAKE_ENGR_BOARD_MAINT_INFO_FILE( char*, char*);
	bool MAKE_FILE( const char*, char*, uint );		//2024.06.24 - KWMOON(RCMD)
	bool READ_FILE( const char*, char*, uint );		//2024.06.24 - KWMOON(RCMD)
public:
	int GET_LOTLIST(std::list<std::string>&);
//data set
public:
	bool WRITE_BIBID(uint,char*);
	bool WRITE_BIBID(uint,uint);
	bool WRITE_BIBID(uint,stBibId);
	bool GET_BIBID(uint,char*);
	bool GET_BIBID(uint,stBibId&);
	const char* GET_BIBID(uint);
//ProgramStart, InitTest End
public:
	bool SET_DEVPGM_VAR(uint, uint, stDevPgmVariables );
	bool GET_DEVPGM_VAR(uint, uint, stDevPgmVariables* );
	bool SET_INITFAIL_DATA( uint, uint, char*,uint);
	bool SET_TESTITEM( uint, uint, stTestItem );
//Test Step End
public:
	bool SET_CYCLEINOUT_DATA( uint, uint, char*,uint);
	bool SET_STEPIN_DATA( uint, uint, char*,uint);
	bool SET_STEPOUT_DATA( uint, uint, char*,uint);
	bool SET_CHIPID_INFO( uint, uint, char*,uint);
	bool SET_CHIPID_DATA( uint, uint, char*,uint);
	bool SET_STEPFAIL_DATA( uint, uint, char*,uint);
	bool SET_STEPFAILBLOCK_DATA( uint, uint, char*,uint);
	bool SET_STEPFAILCHIP_DATA( uint, uint, char*,uint);
	bool SET_STEPSORTBIN_DATA( uint, uint, char*,uint);

	bool SET_STEPFCATE_DATA( uint, uint, char*,uint);		//2024.07.22 fatric
	bool SET_STEPPCATE_DATA( uint, uint, char*,uint);		//2024.07.22 fatric
	bool SET_STEPFAIL_MODE ( uint, uint, char*,uint);		//2024.07.22 fatric
	bool SET_STEPUSE_MODE  ( uint, uint, char*,uint);		//2024.08.18 fatric
public:
	bool SET_CUMFAILBLOCK_DATA( uint, uint, char*,uint);
	bool SET_CUMFAILCHIP_DATA( uint, uint, char*,uint);
	bool SET_CUMFAIL_DATA( uint, uint, char*,uint);
	bool SET_CUMSORTBIN_DATA( uint, uint, char*,uint);
	bool SET_CUMFAILCATE_DATA(uint,uint,char*,uint);		//2024.06.24 - kwmoon
	bool SET_BINMAP_DATA(uint, uint, char*,uint);			//2024.07.02 fatric
public:
	char* GET_ERRMSG();
	bool IS_EXIST_DEVPGM_VAR(uint);
	bool IS_INITTEST_DONE(uint);
	bool IS_EXIST_TESTITEM(uint);
	bool GET_INITTEST_HEAD(uint,uint, stInitTestHead&);
	bool GET_INITTEST_DATA(uint,uint, stInitTestHead&, char*, uint);
	bool GET_INITTEST_FAIL(uint,uint, char*, uint);
	bool GET_TESTITEM(uint,uint,stTestItem&);
	bool GET_SORTBIN_DATA(uint,uint,uint*,uint);
	bool GET_BINMAP_DATA(uint,uint,uint*,uint);				//2024.07.22 fatric
	bool GET_STEPFAIL_DATA(uint,uint,char*,uint,uint);
	bool GET_STEPIN_INFO(uint,uint,stStepInInfo*,uint);
	bool GET_STEPOUT_INFO(uint,uint,stStepOutInfo*,uint);
	bool GET_CHIPID_DATA(uint,uint,char*,uint,uint);
	bool GET_CUMFAIL_CHIP_DATA(uint,uint,char*,uint);
	bool GET_STEPFAIL_CHIP_DATA(uint,uint,char*,uint,uint);
	bool GET_CUMFAIL_BLOCK_DATA(uint,uint,char*,uint);
	bool GET_STEPFAIL_BLOCK_DATA(uint,uint,char*,uint,uint);
	bool GET_CUMFAILCATE_DATA(uint,uint,char*,uint);		//2024.06.24 - kwmoon
	bool GET_STEPFCATE_DATA(uint,uint,uint*,uint,uint);		//2024.07.22 fatric
	bool GET_STEPPCATE_DATA(uint,uint,uint*,uint,uint);		//2024.07.22 fatric
	bool GET_STEPFAIL_MODE (uint,uint,int*,uint,uint);		//2024.07.22 fatric
	bool GET_STEPUSE_MODE  (uint,uint,int*,uint,uint);		//2024.08.18 fatric
public:
	bool GET_STEPOUT_LIST(uint,uint, std::list<stStepOutInfo>*);
public:
	bool REMOVE_BIBID_FILE(uint);
	bool REMOVE_INITTEST_DATA(uint);
	bool REMOVE_STEPTEST_DATA(uint);
	bool REMOVE_RCMD_FILES();
public:
	bool GET_VERSION(uint,uint,char*);
	const char* GET_REMARK(uint,uint);
	const char* GET_LOTNO();
	const char* GET_PARTNO();
	const char* GET_OPRID();
	uint GET_MAX_STEP(uint,uint);
	time_t* GET_CYCLE_BEGIN_TIME(uint slot, uint cpu);
	time_t* GET_CYCLE_END_TIME  (uint slot, uint cpu);
	time_t* GET_CYCLE_BEGIN_TIME(uint slot);
	time_t* GET_CYCLE_END_TIME  (uint slot);
	time_t* GET_STEP_BEGIN_TIME(uint slot, uint);
	time_t* GET_STEP_END_TIME  (uint slot, uint);
	std::vector<std::string> GET_STEPNAMES(uint,uint);	//stepout
	bool GET_LAST_CYCLE_END_SLOT_CPU(uint &slot, uint &cpu);		//2023.08.18 - KWMOON
	bool GET_LAST_CYCLE_END_SLOT_CPU(uint &slot, uint &cpu, bool* pbValid);	//2024.07.31 fatric
	bool GET_LAST_CYCLE_END_SLOT(uint &slot);		//2023.08.18 - KWMOON
public:
	time_t* GET_CYCLE_BEGIN_TIME();
	time_t* GET_CYCLE_END_TIME  ();
	const char* GET_STATION();
	int GET_ZONE();
public:
	bool GET_DEVICE (uint,uint,char*);
	const char* GET_DEVICE (uint,uint);
	int GET_DEVICE_TYPE(uint,uint);
	uint GET_MAXDUTPERCPU(uint,uint);
	int GET_CE_COUNT(uint,uint);
	int GET_CHIP_COUNT(uint,uint);
	int GET_BLOCKPERCE(uint,uint);
	int GET_BLOCKPERCHIP(uint,uint);
	int GET_BLOCKPERDUTBL(uint,uint);
	int GET_CHIPPERDUT(uint,uint);
	int GET_CHIPPERDUTBL(uint,uint);
	int GET_DUTPERFC(uint,uint);
	int GET_DUTPERCPU(uint,uint);
	int GET_BLOCKSHIFT(uint,uint);
	int GET_PAGESHIFT(uint,uint);
	int GET_COLSIZE(uint,uint);
	int GET_BLOCKLIMIT(uint,uint);
	int GET_BLOCKLIMITPERCE(uint,uint);
	int GET_ADDRLIMIT(uint,uint);
	int GET_FAILMODE(uint,uint);
	int GET_SCANINVISIBLE(uint,uint);
	int GET_BIBSCAN(uint,uint);
	int GET_IOSIZE(uint,uint);
	int GET_BLOCKPERDUT(uint,uint);
	int GET_BIBX(uint,uint);
	int GET_BIBY(uint,uint);
	int GET_CPU2X(uint,uint);
	int GET_BITIME(uint,uint);
	int GET_AGINGTIME(uint,uint);
	int GET_MAX_CATEGORY_BIT(uint,uint);	//2024.06.24 - KWMOON

private:
	void SetTestdataRootDir(const char*);
	bool InitTestdataDir(bool bRemove = false);
	bool InitTgetDir();
	bool InitBiOpenLotDir();
	bool InitBoardMaintInfoDir();
	bool InitSlotDir();
	bool ReadDataDirEnv();
	bool ReadDevPgmVar(uint,uint);
	bool InitTestDataFiles();
	bool WriteAppendFile( char* fname, char* data, uint size, bool bAppend=true );

public:
	bool ReadDevPgmVar(uint,uint, stDevPgmVariables*);	//2024.07.31 fatric

private:
	bool SET_ERRMSG( const char*,...);
	bool SET_MSG( const char*,...);
private:
	bool m_bDebugMode;
	bool m_bIsDevVar;
	char m_szLotNo  [64];
	char m_szPartNo [64];
	char m_szOprId  [64];
	char m_szBidId  [64];
	char m_szDataRoot[MAX_DIR_FILENAME];
	char m_szDataDir [MAX_DIR_FILENAME];
	char m_szSlotDir[MAX_SLOT][MAX_DIR_FILENAME];
	char m_szTgetDir[MAX_DIR_FILENAME];			//2024.06.17 - KWMOON
	char m_szBoardMaintdir[MAX_DIR_FILENAME];	//2024.06.17 - KWMOON
	char m_szBiOpenLotDir[MAX_DIR_FILENAME];	//2024.08.19 - HSBAE
	char m_szErrMsg [MAX_TEXT_MSG];
	char m_szMsg    [MAX_TEXT_MSG];
	stDevPgmVariables m_stDevPgmVar;
	time_t m_time_t;
	bool m_bUiMode;	//2024.06.17 - KWMOON
private:
	char m_szUiModeFile[MAX_DIR_FILENAME];						//2024.06.13 - common mode
private:
	char m_szBibIdFile[MAX_SLOT][MAX_DIR_FILENAME];						//filename
	char m_szDevVarFile[MAX_SLOT][2][MAX_DIR_FILENAME];					//filename
	char m_szTestItemFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename
	char m_szStepInFile[MAX_SLOT][2][MAX_DIR_FILENAME];					//filename
	char m_szStepOutFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename
	char m_szCycleInOutFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename
	char m_szInitFailDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename
	char m_szChipInfoFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename
	char m_szChipDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename
	char m_szStepFailDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename
	char m_szStepFailBlockDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//filename
	char m_szStepFailChipDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//filename
	char m_szStepSortBinDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//filename
	char m_szStepFCateDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename	//2024.07.22 fatric
	char m_szStepPCateDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename	//2024.07.22 fatric
	char m_szStepFailModeFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename	//2024.07.22 fatric
	char m_szStepUseModeFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename	//2024.08.18 fatric
	//char m_szLastSetup[MAX_DIR_FILENAME];								//2023.08.08
private:
	char m_szBoardMaintFile[MAX_SLOT][MAX_DIR_FILENAME];				//2024.06.17 - kwmoon

private:
	char m_szCumFailBlockDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];   	//filename
	char m_szCumFailChipDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//filename
	char m_szCumTestFailDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//filename
	char m_szCumSortBinDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];			//filename
	char m_szCumFailCateDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];		//2024.06.24 - KWMOON
	char m_szBinmapDataFile[MAX_SLOT][2][MAX_DIR_FILENAME];				//filename	//2024.07.22 fatric
};

#endif // CTESTDATAMGR_HPP
