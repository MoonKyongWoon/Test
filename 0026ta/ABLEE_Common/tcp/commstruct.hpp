#ifndef COMM_STRUCT_HPP
#define COMM_STRUCT_HPP

#include <QDateTime>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "sysdef.hpp"
#include "swcontrol.h"
#include "protocol.hpp"

#define MAX_CHAR_VALUE		(64)
#define MAX_TEXT_VALUE		(96)
#define MAX_FILE_NAME		(64)
#define MAX_FILE_PATH		(128)
#define MAX_TEXT_MSG_256	(256)
#define MAX_TEXT_MSG		(1024)

#pragma pack(1)

enum EN_SLOT_STAT
{
	EN_SLOT_MASK   = -1,
	EN_SLOT_OFFLINE    ,
	EN_SLOT_ONLINE     ,
};

enum EN_BIBID_RESPONSE
{
	EN_BIBID_IO_ERR = -3 ,
	EN_BIBID_PARITY      ,
	EN_BIBID_EMPTY       ,
	EN_BIBIID_ACK        ,
};

enum EN_SLOT_SYNC
{
	EN_SLOT_LEAVE   = -1,
	EN_SLOT_ENTER   =  1,
};

typedef struct _stTcpHeader
{
	unsigned int cmd;
	unsigned int src;
	unsigned int dest;
	unsigned int slot;
	unsigned int runner;	//cpu1x/22 or pgm1/2
	int error;
	unsigned int datalen;
}stTcpHeader, stIpcHeader;

typedef struct _stTcpElement
{
	stTcpHeader tcpHeader;
	char* pData;
}stTcpElement;

typedef struct _stIpcElement
{
	stIpcHeader ipcHeader;
	char* pData;
}stIpcElement;


////////////////////////////////////////////////////////////////////////////
/// SLOT(SITE) INFO : MGR/MASK/IP/ SETUP
////////////////////////////////////////////////////////////////////////////

typedef struct _stSlotIp
{
	bool bMask;
	char szCpu1Ip[24];
	char szCpu2Ip[24];
}stSlotIp;

typedef struct _stSlotCpu
{
	int nMaxSlot;
	int nSlotNo;
	int nCpuPerSlot;	//cpu x1 or x2
	int nPgmPerSlot;	//pgm x1 or x2

}stSlotCpu;

//loader
typedef struct _stSlotInfo
{
	stSlotCpu SlotCpu;
	stSlotIp  SlotIp[MAX_SLOT];
}stSlotInfo;

//loader
typedef struct _stSlotLive
{
	char slot_status[MAX_SLOT][MAX_SYS_CPU];	//-1:mask, 0:offline, 1:online
}stSlotLive;

//dev pgm
typedef struct _stPgmLive
{
	char pgm_status[MAX_SLOT][PROTOCOL::EN_PGM_MAX_RUNNER][PROTOCOL::EN_MAX_PGM_SOCKET];
}stPgmLive;

typedef struct _stVersion
{
	char szVer[32];
}stVersion;

typedef struct _stBibId
{
	bool bBibConn;		//2023.08.04 - KWMOON
	bool bParityError;	//2023.08.04 - KWMOON
	int  nBibId;
	char szBibId[16];
}stBibId;

////////////////////////////////////////////////////////////////////////////
/// PROGRAM DOWNLOAD & DATA BETWEEN MAIN UI <-> LOADEER
////////////////////////////////////////////////////////////////////////////

typedef struct _stFtpInfo
{
	char szId    [32];
	char szPasswd[32];
	char szServer[32];
	unsigned int uPort;
}stFtpInfo;

typedef struct _stPgmDownload
{
	char    bCompress;
	char    szFilePath[MAX_FILE_PATH]; //file path & name	-> /home/ablee/atsystem/download/aaa.tgz or aaa.dev
	char    szFileName[MAX_FILE_NAME]; //file name
	int     nFileSize;
}stPgmDownload;

typedef struct _stPgmLoadStatus
{
	bool bSuccess;
	int nProgress;
	char szErrMsg[MAX_TEXT_MSG];
}stPgmLoadStatus;


//ui -> each cpu : init test
typedef struct _stUiTestInfo
{
	bool    is_set;
	bool    ui_chamberuse;
	bool    empty_bib_test;				//2023.07.09 - use until diag ui finished.
	unsigned int chmb_soak_time;		//2023.07.09 - kwmoon for tempcal only
	unsigned int chmb_keep_time;		//2023.07.09 - kwmoon for tempcal only

}stUiTestInfo;

//ui -> each cpu : init test
typedef struct _stUiInitInfo
{
	bool is_set;
	bool ui_automode;
	stBibId stBid;
}stUiInitInfo;

typedef struct _stTestItemSub
{
	bool bTestItem;
	int  nStepNo;
	char szItemCategory[MAX_TEXT_VALUE];
	char szItemName[MAX_TEXT_VALUE];
}stTestItemSub;

typedef struct _stTestItem
{
	bool bItemCategory;
	bool bChamberUse;
	unsigned int chmb_soak_time;		//2023.07.09 - kwmoon for tempcal only
	unsigned int chmb_keep_time;		//2023.07.09 - kwmoon for tempcal only
	uint nItemCount;
	uint nLoopCount;
	stTestItemSub ItemStep[MAX_STEP];
}stTestItem;

//ui -> cpu
typedef struct _stTestReqData
{
	bool chamber_use;
}stTestReqData;

typedef struct _stSetupInfo
{
	bool auto_mode;
	char operator_id[MAX_CHAR_VALUE];
	char part_number[MAX_CHAR_VALUE];
	char lot_number [MAX_CHAR_VALUE];
}stSetupInfo;

//UI Slot Test History & Setup Info
typedef struct _stSlotTestInfo
{
	bool bBibId;	//2024.06.19 - KWMOON
	bool bSetup;
	stSetupInfo setup_info;
	stBibId bib_id;
	char szTpName[MAX_TEXT_VALUE];
	char szSetupMsg[MAX_TEXT_VALUE];
	int  nTotalQty;
	int  nPassQty;
	int  nFailQty;
	int  nTotalTime;
	int  nBiTime;
	time_t tStartTime;
	time_t tEndTime;
}stSlotTestInfo;

//UI Setup
typedef struct _stUiSlotAck
{
	bool bSlotMask;
	bool bAck[MAX_SYS_CPU];
	bool bErr[MAX_SYS_CPU];
	bool bKilled[MAX_SYS_CPU];  //2023.06.02 - kwmoon
	std::string sMsg[MAX_SYS_CPU];
}stUiSlotAck;


////////////////////////////////////////////////////////////////////////////
/// TEST CYCLE/STEP DATA
////////////////////////////////////////////////////////////////////////////

//2023.04.26 - KWMOON
typedef struct _stCycleInfo
{
	bool bStart;
	bool bEnd;
	time_t tCycleBegin;
	time_t tCycleEnd;
}stCycleInfo;

//2023.04.27 - KWMOON
typedef struct _stInitTestHead
{
	uint    nStepNo;
	char   szStepName[MAX_TEXT_VALUE];
	time_t tInitBegin;
	time_t tInitEnd;
}stInitTestHead;

//2023.04.24 - KWMOON
typedef struct _stStepDataHead
{
	uint    nStepNo;
	uint    nRfu1;
	uint    nRfu2;
	char   szStepName[MAX_TEXT_VALUE];
}stStepDataHead;

//2023.04.24 - KWMOON
typedef struct _stStepInInfo
{
	uint    nStepNo;
	char   szStepName[MAX_TEXT_VALUE];
	time_t tStepBegin;
}stStepInInfo;

//2023.04.24 - KWMOON
typedef struct _stStepOutInfo
{
	uint    nStepNo;
	char   szStepName[MAX_TEXT_VALUE];
	time_t tStepEnd;
	int    nTsNo;
	char   szPc[MAX_TEXT_VALUE];
	double dVcc;
	double dBiTime;
	double dTemp;
	bool bProcExtFmBlock;	//2023.06.14 - KWMOON
}stStepOutInfo;

typedef struct _stCommonData
{
	char    szRemark[PATH_MAX];
	time_t	tCycleBegin;
	time_t	tCycleEnd;
	uint	iSlot;
	int		nBibId;
}stCommonData;


class CStepData
{
public:
	uint	iTsNo;
	time_t	tStepBegin;
	time_t	tStepEnd;
	double	dTemp;
	double	dBiTime;
	char	szStepName[PATH_MAX];
	char	szPc      [PATH_MAX];
	double  dVcc;

public:
	CStepData()
	{
		iTsNo            = 0  ;
		tStepBegin       = 0  ;
		tStepEnd         = 0  ;
		dTemp            = 0.0;
		dBiTime          = 0.0;
		memset(szStepName, 0, sizeof(szStepName) );
		memset(szPc      , 0, sizeof(szPc      ) );
		dVcc             = 0.0;
	}
	CStepData( uint _iTsNo, time_t _tStepBegin,	time_t _tStepEnd, double _dTemp, double	_dBiTime, const char _szStepName[], const char _szPc[], double  _dVcc)
	{
		iTsNo            = _iTsNo      ;
		tStepBegin       = _tStepBegin ;
		tStepEnd         = _tStepEnd   ;
		dTemp            = _dTemp      ;
		dBiTime          = _dBiTime    ;
		strcpy(szStepName, _szStepName);
		strcpy(szPc      , _szPc      );
		dVcc             = _dVcc       ;
	}
};


//2023.03.06 - KWMOON( DEVICE PGM VARIABLES )
typedef struct _stDevPgmVariables
{
//	char szProgram[MAX_TEXT_VALUE];
	char szVersion[MAX_TEXT_VALUE];
	char szDevice [MAX_TEXT_VALUE];
	unsigned int nDeviceType;
	unsigned int nMaxDutPerCpu;
	unsigned int nBibX;
	unsigned int nBibY;
	unsigned int nCe;
	unsigned int nChip;
	unsigned int nBlockPerCe;
	unsigned int nBlockPerDut;
	unsigned int nBlockPerDutBl;
	unsigned int nChipPerDut;
	unsigned int nChipPerDutBl;
	unsigned int nDutPerFc;
	unsigned int nDutPerCpu;
	unsigned int nBlockShift;
	unsigned int nPageShift;
	unsigned int nColSize;
	unsigned int nblockLimit;
	unsigned int nblockLimitPerCe;
	unsigned int nAddrLimit;
	unsigned int nFailMode;
	unsigned int nScanInvisible;
	unsigned int nBibScan;
	unsigned int nIoSize;
	unsigned int nCpu2X;
	unsigned int nBiTime;
	unsigned int nAgingTime;
	unsigned int nMaxCategoryBit;		//2024.06.24 - KWMOON
}stDevPgmVariables;

///////////////////////////////////////////////////
/// chip id/wafer/x/y
/// 2022.11.28 - KWMOON
///////////////////////////////////////////////////
typedef struct _stChipData
{
	unsigned short chip_count;
	unsigned short data_length;
	unsigned short chip_size;
	char data_format[MAX_FILE_PATH];
	int chipid_shift;
}stChipData;

typedef struct _stChipId
{
	char szchipId[MAX_TEXT_VALUE];
	unsigned short uWaferId;
	unsigned short uXPos;
	unsigned short uYPos;
}stChipId;

//2023.03.20 - KWMOON
typedef struct _stPlcResponse
{
	PROTOCOL::PLC_COMMAND plc_cmd;
	YKG_CMD_LIST          ykg_Cmd;
	bool bPass;
	char szdata[16];
	int datalen;
}stPlcResponse;

///////////////////////////////////////////////////
/// Chamber
/// 2023.05.09 - KWMOON
///////////////////////////////////////////////////
typedef struct _stSetTemp
{
	int  nTemp;
	double dTemp;
}stSetTemp;

//Temp Read
typedef struct _stGetTemp
{
	int    nAck;
	double dSv;
	double dPv;
	double dPcw;
	char szMsg[MAX_TEXT_MSG_256];
}stGetTemp;

//Status, Alarm...
typedef struct _stGetChmbWord
{
	int nAck;
	int nStatus;
	char szMsg[MAX_TEXT_MSG_256];
}stGetChmbWord;

//Enter, Sync
typedef struct _stChmbCmdResponse
{
	int nAck;
	unsigned int nData;
	unsigned int nLen;
	char szMsg[MAX_TEXT_MSG_256];
}stChmbCmdResponse;

///////////////////////////////////////////////////
/// ATPFMD - SYSMONUI - MAINUI
/// 2023.08.09 - HSBAE
///////////////////////////////////////////////////

struct sm_dcdc_data {
	double	set_voltage;
	double	present_voltage;
	double	present_current;
	int		status_word;
	int		status_vout;
};

struct sm_cpu_data {
	sm_dcdc_data pps_data[8];
	sm_dcdc_data s1ps_data[8];
};

struct sm_slot_data {
	sm_cpu_data cpu_data[2];
};

typedef struct _stShmMainPfm {
	sm_slot_data slot_data[16];
}stShmMainPfm;

struct pfm_time {
	struct timeval	val;
	struct tm		t;
};

struct dcdc_data{
	double	set_voltage;
	int		depth[10];
	double	present_voltage[10];
	double	present_current[10];
	int		status_word[10];
	int		status_vout[10];
};

struct	pfm_data {
	pfm_time	time;
	int			is_fault;
	dcdc_data	pps[8];
	dcdc_data	s1ps[8];
	char		szStep[64];	// step name
	char		szPC[64];	// pattern name in step
	char		szFlag[8];	// init test or test entry
};

struct	dcdc_unit_data {
	double	set_voltage;
	int		depth;
	double	present_voltage;
	double	present_current;
	int		status_word;
	int		status_vout;
};

struct	pfm_unit_data {
	pfm_time		time;
	int				is_fault;
	dcdc_unit_data	pps[8];
	dcdc_unit_data	s1ps[8];
};

struct	pfm_update_data {
	bool			need_update[16][2];
	pfm_unit_data	data[16][2];

};

///////////////////////////////////////////////////
/// TEST MGR - CHMB MGR - TEMP SYNC
/// 2023.05.15 - KWMOON
///////////////////////////////////////////////////
typedef struct _stChmbMgrSv
{
	double dSv;
	bool bSvSet;
	bool bRecvRun;
	bool bEnter;
	bool bLeave;
}stChmbMgrSv;

///////////////////////////////////////////////////
/// MAIN UI <-> CHAMBER UI SHM STRUCT
/// 2023.06.19 - KWMOON
///////////////////////////////////////////////////
typedef struct _stShmMainChmb
{
	bool bUse;
	bool bTest;
}stShmMainChmb;


///////////////////////////////////////////////////
/// EAP <-> TMS UI TCP/IP STRUCT
/// 2024.03.28 - KWMOON
///////////////////////////////////////////////////
typedef struct _stHsmsMsgHeader
{
	union {
		unsigned short nDeviceId;			//Session Id 0-1
		struct {
			unsigned char cDevidMsb : 7;		//0
			unsigned char cRbit     : 1;
			unsigned char cDevidLsb : 8;		//1
		};
	};
	union {
		unsigned char cByte2;				//2
		struct {
			unsigned char cStream : 7;
			unsigned char cWbit   : 1;
		};
	};
	unsigned char cFunction;				//3
	unsigned char cPType;					//4
	unsigned char cSType;					//5
	unsigned char cSystemBytes[4];				//6-9
}stHsmsMsgHeader;

typedef struct _stHsmsHeader
{
	unsigned char  szMsgLength[4];
	stHsmsMsgHeader stMsgHeader;
}stHsmsHeader;

typedef struct _stHsmsElement
{
	stHsmsHeader hsmsHeader;
	char* pData;
}stHsmsElement;

///////////////////////////////////////////////////
/// SECS-II MESSAGE - RCMD
/// 2024.05.14 - HSBAE
///////////////////////////////////////////////////

typedef struct _stShmHsms
{
	uint system_byte;
	uint data_id;
}stShmHsms;

enum class _RCMD {
	AUTO_DIAG			 	 = 0,
	BI_CLOSE_LOT				,
	BI_INITIAL_END				,
	BI_INITIAL_START			,
	BI_OPEN_LOT					,
	BI_RECLOSE_LOT				,
	BI_TEST_TIME				,
	BOARD_MAINT_INFO			,
	BOARD_STATUS_INFO			,
	BOARD_TYPE					,
	CHECK_IN_BOARD				,
	CHECK_OUT_BOARD				,
	EQ_EFF_INFO					,
	HOLD_CHECK					,
	INTERLOCK_LIMIT				,
	READY_TO_LOAD				,
	LOT_CALL					,
	LOT_SET						,
	LOT_SET_INFO				,
	MANUAL_MASK_MAP				,
	PM_CHECK					,
	RACK_SET_INFO				,
	RECHECK_IN_BOARD			,
	AUTO_BYPASS					,
	REPLY_AUTO_BYPASS_EQP_STATE	,
	REQUEST_BOARD_MASK_SET		,
	REQUEST_RACK_SET			,
	RMVOU						,
	SLOT_MASK_INFO				,
	START_BI_CMD				,
	TGET						,
	TMVIN						,
	TMVOU						,
	VMVOU						,
	READY_TO_UNLOAD				,
	ERROR						,
	UNKNOWN
};

struct stRcmdBaseData {
	virtual ~stRcmdBaseData(){}
};

struct stRcmdData {
	_RCMD rcmd;
	stRcmdBaseData* data;
};

struct stRcmdAutoDiag : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char AUTO_DIAG_FLAG[16];
	char AUTO_DIAG_DESC[128];
//	~stRcmdAutoDiag()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (AUTO_DIAG_FLAG)			{ delete[] AUTO_DIAG_FLAG;			}
//		if (AUTO_DIAG_DESC)			{ delete[] AUTO_DIAG_DESC;			}
//	}
};

struct stRcmdBiCloseLot : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];		//2024.06.27 - KWMOON(20 -> 64*16)
//	~stRcmdBiCloseLot()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//	}
};

struct stRcmdBiInitialEnd : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdBiInitialEnd()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdBiInitialStart : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
//	~stRcmdBiInitialStart()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdBiOpenLot : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char LOT_ID[64];
	char SNO[32];
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];		//2024.06.27 - KWMOON(20 -> 64*16)
//	~stRcmdBiOpenLot()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (LOT_ID)					{ delete[] LOT_ID;					}
//		if (SNO)					{ delete[] SNO;						}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//	}
};

struct stRcmdBiRecloseLot : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdBiRecloseLot()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdBiTestTime : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdBiTestTime()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdBoardMaintInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char BIB_TYPE[64];
	char BIB_ID[64];
	char BIB_X_SIZE[32];
	char BIB_Y_SIZE[32];
	char P_CODE[32];
	char MLB_CODE[32];
	char MLB_LOT_LIST[64*48];
	char IOC[32];
	char BOC[32];
	char BURN_IN_COUNT[32];
	char PART_NO[64];				//2024.06.27 - KWMOON(30 -> 64)
	char LOT_NO[64];				//2024.06.27 - KWMOON(30 -> 64)
	char LOT_MAP[64*400];			//2024.06.27 - KWMOON(3000 -> 8192)
	char TEST_PGM_NO[64];			//2024.06.27 - KWMOON(20 -> 64)
	char INSERT_MOUNT_MAP[402];		//2024.06.27 - KWMOON(200->402)
	char INITIAL_MASK_MAP[402];		//2024.06.27 - KWMOON(200->402)
	char MANUAL_MASK_MAP [402];		//2024.06.27 - KWMOON(200->402)
	char FUNTION_MASK_MAP[402];		//2024.06.27 - KWMOON(200->402)
	char PRODUCT_NAME[256];
	char BIB_LAYOUT_FILE_NAME[256];
//	~stRcmdBoardMaintInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (BIB_X_SIZE)				{ delete[] BIB_X_SIZE;				}
//		if (BIB_Y_SIZE)				{ delete[] BIB_Y_SIZE;				}
//		if (P_CODE)					{ delete[] P_CODE;					}
//		if (MLB_CODE)				{ delete[] MLB_CODE;				}
//		if (MLB_LOT_LIST)			{ delete[] MLB_LOT_LIST;			}
//		if (IOC)					{ delete[] IOC;						}
//		if (BOC)					{ delete[] BOC;						}
//		if (BURN_IN_COUNT)			{ delete[] BURN_IN_COUNT;			}
//		if (PART_NO)				{ delete[] PART_NO;					}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (LOT_MAP)				{ delete[] LOT_MAP;					}
//		if (TEST_PGM_NO)			{ delete[] TEST_PGM_NO;				}
//		if (INSERT_MOUNT_MAP)		{ delete[] INSERT_MOUNT_MAP;		}
//		if (INITIAL_MASK_MAP)		{ delete[] INITIAL_MASK_MAP;		}
//		if (MANUAL_MASK_MAP)		{ delete[] MANUAL_MASK_MAP;			}
//		if (FUNTION_MASK_MAP)		{ delete[] FUNTION_MASK_MAP;		}
//		if (PRODUCT_NAME)			{ delete[] PRODUCT_NAME;			}
//		if (BIB_LAYOUT_FILE_NAME)	{ delete[] BIB_LAYOUT_FILE_NAME;	}
//	}
};

struct stRcmdBoardStatusInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdBoardStatusInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdBoardType : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char BIB_TYPE[64];
	char BIB_X_SIZE[32];
	char BIB_Y_SIZE[32];
	char BIB_DESCRIPTION[128];			//2024.06.27 - KWMOON(20 -> 128)
	char AUTO_START_CHECK[32];
	char INIT_RETRY[32];
	char SPECIAL_CODE_CHECK[32];
	char SUB_EXCLUDED[32];
	char IOC_CHECK[32];
	char LOT_INIT[32];
	char BIB_INIT[32];
	char MOUNT_RATE[32];
	char BIBQTY_CHK[32];
	char BIBQTY[32];
	char INCLUDED_SPECIAL_CODE[32];
	char INCLUDED_SUB_LOTID[32];
	char INCLUDED_LOT_INIT[32];
	char INCLUDED_BIB_INIT[32];
	char INCLUDED_MOUNT_RATE[32];
//	~stRcmdBoardType()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_X_SIZE)				{ delete[] BIB_X_SIZE;				}
//		if (BIB_Y_SIZE)				{ delete[] BIB_Y_SIZE;				}
//		if (BIB_DESCRIPTION)		{ delete[] BIB_DESCRIPTION;			}
//		if (AUTO_START_CHECK)		{ delete[] AUTO_START_CHECK;		}
//		if (INIT_RETRY)				{ delete[] INIT_RETRY;				}
//		if (SPECIAL_CODE_CHECK)		{ delete[] SPECIAL_CODE_CHECK;		}
//		if (SUB_EXCLUDED)			{ delete[] SUB_EXCLUDED;			}
//		if (IOC_CHECK)				{ delete[] IOC_CHECK;				}
//		if (LOT_INIT)				{ delete[] LOT_INIT;				}
//		if (BIB_INIT)				{ delete[] BIB_INIT;				}
//		if (MOUNT_RATE)				{ delete[] MOUNT_RATE;				}
//		if (BIBQTY_CHK)				{ delete[] BIBQTY_CHK;				}
//		if (BIBQTY)					{ delete[] BIBQTY;					}
//		if (INCLUDED_SPECIAL_CODE)	{ delete[] INCLUDED_SPECIAL_CODE;	}
//		if (INCLUDED_SUB_LOTID)		{ delete[] INCLUDED_SUB_LOTID;		}
//		if (INCLUDED_LOT_INIT)		{ delete[] INCLUDED_LOT_INIT;		}
//		if (INCLUDED_BIB_INIT)		{ delete[] INCLUDED_BIB_INIT;		}
//		if (INCLUDED_MOUNT_RATE)	{ delete[] INCLUDED_MOUNT_RATE;		}
//	}
};

struct stRcmdCheckInBoard : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char BIB_TYPE[64];
	char BIB_ID[64];
	char P_CODE[32];
	char MLB_CODE[32];
	char MLB_LOT_LIST[64*48];		//2024.06.27 - KWMOON(20 -> 64*16)
	char LOT_MAP[64*400];				//2024.06.27 - KWMOON(3000 -> 8192)
//	~stRcmdCheckInBoard()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (P_CODE)					{ delete[] P_CODE;					}
//		if (MLB_CODE)				{ delete[] MLB_CODE;				}
//		if (MLB_LOT_LIST)			{ delete[] MLB_LOT_LIST;			}
//		if (LOT_MAP)				{ delete[] LOT_MAP;					}
//	}
};

struct stRcmdCheckOutBoard : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char BIB_TYPE[64];
	char BIB_ID[64];
	char P_CODE[32];
	char MLB_CODE[32];
	char MLB_LOT_LIST[64*48];		//2024.06.27 - KWMOON(20 -> 64*16)
	char LOT_MAP[64*400];			//2024.06.27 - KWMOON(3000 -> 8192)
//	~stRcmdCheckOutBoard()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (P_CODE)					{ delete[] P_CODE;					}
//		if (MLB_CODE)				{ delete[] MLB_CODE;				}
//		if (MLB_LOT_LIST)			{ delete[] MLB_LOT_LIST;			}
//		if (LOT_MAP)				{ delete[] LOT_MAP;					}
//	}
};

struct stRcmdEqEffInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];			//2024.06.27 - KWMOON(20 -> 128)
	char JAM_GRADE[32];
	char SINGO_EVENT[32];
	char JAM_CONTENT[32];
//	~stRcmdEqEffInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (JAM_GRADE)				{ delete[] JAM_GRADE;				}
//		if (SINGO_EVENT)			{ delete[] SINGO_EVENT;				}
//		if (JAM_CONTENT)			{ delete[] JAM_CONTENT;				}
//	}
};

struct stRcmdHoldCheck : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];			//2024.06.27 - KWMOON(20 -> 128)
	char EQUIP_NO[64];
	char FIRST_QUARTER_PM_DATA[32];
	char SECOND_QUARTER_PM_DATA[32];
	char THIRD_QUARTER_PM_DATA[32];
	char FOURTH_QUARTER_PM_DATA[32];
	char FIRST_HALF_PM_DATA[32];
	char LATTER_HALF_PM_DATA[32];
	char HOLD_FLAG[16];
//	~stRcmdHoldCheck()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (EQUIP_NO)				{ delete[] EQUIP_NO;				}
//		if (FIRST_QUARTER_PM_DATA)	{ delete[] FIRST_QUARTER_PM_DATA;	}
//		if (SECOND_QUARTER_PM_DATA)	{ delete[] SECOND_QUARTER_PM_DATA;	}
//		if (THIRD_QUARTER_PM_DATA)	{ delete[] THIRD_QUARTER_PM_DATA;	}
//		if (FOURTH_QUARTER_PM_DATA)	{ delete[] FOURTH_QUARTER_PM_DATA;	}
//		if (FIRST_HALF_PM_DATA)		{ delete[] FIRST_HALF_PM_DATA;		}
//		if (LATTER_HALF_PM_DATA)	{ delete[] LATTER_HALF_PM_DATA;		}
//		if (HOLD_FLAG)				{ delete[] HOLD_FLAG;				}
//	}
};

struct stRcmdInterlockLimit : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char HOT_LIMIT_VALUE[32];
	char HOT_LIMIT_EVENT[32];
	char HOT_LIMIT_FLAG[16];
	char IOC_VALUE[32];
	char IOC_EVENT[32];
	char IOC_FLAG[16];
	char SLOT_MV_VALUE[32];
	char SLOT_MV_EVENT[32];
	char SLOT_MV_FLAG[16];
	char TEMP_DELAY_VALUE[32];
	char TEMP_DELAY_EVENT[32];
	char TEMP_DELAY_FLAG[16];
	char BOC_VALUE[32];
	char BOC_EVENT[32];
	char BOC_FLAG[16];
	char ALL_FAIL_VALUE[32];
	char ALL_FAIL_EVENT[32];
	char ALL_FAIL_FLAG[16];
	char BIN1_VALUE[32];
	char BIN1_EVENT[32];
	char BIN1_FLAG[16];
	char PDA_VALUE[32];
	char PDA_EVENT[32];
	char PDA_FLAG[16];
	char BIN9_VALUE[32];
	char BIN9_EVENT[32];
	char BIN9_FLAG[16];
	char BLOCK_FAIL_VALUE[32];
	char BLOCK_FAIL_EVENT[32];
	char BLOCK_FAIL_FLAG[16];
	char SLOT_PATE_VALUE[32];
	char SLOT_PATE_EVNET[32];
	char SLOT_PATE_FLAG[16];
	char END_DELAY_VALUE[32];
	char END_DELAY_EVNET[32];
	char END_DELAY_FLAG[16];
	char PFAIL_SKIP_FLAG[16];

//	~stRcmdInterlockLimit()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (HOT_LIMIT_VALUE)		{ delete[] HOT_LIMIT_VALUE;			}
//		if (HOT_LIMIT_EVENT)		{ delete[] HOT_LIMIT_EVENT;			}
//		if (HOT_LIMIT_FLAG)			{ delete[] HOT_LIMIT_FLAG;			}
//		if (IOC_VALUE)				{ delete[] IOC_VALUE;				}
//		if (IOC_EVENT)				{ delete[] IOC_EVENT;				}
//		if (IOC_FLAG)				{ delete[] IOC_FLAG;				}
//		if (SLOT_MV_VALUE)			{ delete[] SLOT_MV_VALUE;			}
//		if (SLOT_MV_EVENT)			{ delete[] SLOT_MV_EVENT;			}
//		if (SLOT_MV_FLAG)			{ delete[] SLOT_MV_FLAG;			}
//		if (TEMP_DELAY_VALUE)		{ delete[] TEMP_DELAY_VALUE;		}
//		if (TEMP_DELAY_EVENT)		{ delete[] TEMP_DELAY_EVENT;		}
//		if (TEMP_DELAY_FLAG)		{ delete[] TEMP_DELAY_FLAG;			}
//		if (BOC_VALUE)				{ delete[] BOC_VALUE;				}
//		if (BOC_EVENT)				{ delete[] BOC_EVENT;				}
//		if (BOC_FLAG)				{ delete[] BOC_FLAG;				}
//		if (ALL_FAIL_VALUE)			{ delete[] ALL_FAIL_VALUE;			}
//		if (ALL_FAIL_EVENT)			{ delete[] ALL_FAIL_EVENT;			}
//		if (ALL_FAIL_FLAG)			{ delete[] ALL_FAIL_FLAG;			}
//		if (BIN1_VALUE)				{ delete[] BIN1_VALUE;				}
//		if (BIN1_EVENT)				{ delete[] BIN1_EVENT;				}
//		if (BIN1_FLAG)				{ delete[] BIN1_FLAG;				}
//		if (PDA_VALUE)				{ delete[] PDA_VALUE;				}
//		if (PDA_EVENT)				{ delete[] PDA_EVENT;				}
//		if (PDA_FLAG)				{ delete[] PDA_FLAG;				}
//		if (BIN9_VALUE)				{ delete[] BIN9_VALUE;				}
//		if (BIN9_EVENT)				{ delete[] BIN9_EVENT;				}
//		if (BIN9_FLAG)				{ delete[] BIN9_FLAG;				}
//		if (BLOCK_FAIL_VALUE)		{ delete[] BLOCK_FAIL_VALUE;		}
//		if (BLOCK_FAIL_EVENT)		{ delete[] BLOCK_FAIL_EVENT;		}
//		if (BLOCK_FAIL_FLAG)		{ delete[] BLOCK_FAIL_FLAG;			}
//		if (SLOT_PATE_VALUE)		{ delete[] SLOT_PATE_VALUE;			}
//		if (SLOT_PATE_EVNET)		{ delete[] SLOT_PATE_EVNET;			}
//		if (SLOT_PATE_FLAG)			{ delete[] SLOT_PATE_FLAG;			}
//		if (END_DELAY_VALUE)		{ delete[] END_DELAY_VALUE;			}
//		if (END_DELAY_EVNET)		{ delete[] END_DELAY_EVNET;			}
//		if (END_DELAY_FLAG)			{ delete[] END_DELAY_FLAG;			}
//		if (PFAIL_SKIP_FLAG)		{ delete[] PFAIL_SKIP_FLAG;			}
//	}
};

struct stRcmdReadyToLoad : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char EQUIP_ID[64];					//2024.06.27 - KWMOON(20 -> 64)
	char ZONE[64];					//2024.06.27 - KWMOON(20 -> 64)
	char STATUS[32];
	char LOGIS_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
//	~stReadyToLoad()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (EQUIP_ID)				{ delete[] EQUIP_ID;				}
//		if (PORT_ID)				{ delete[] PORT_ID;					}
//		if (LOGIS_STATUS)			{ delete[] LOGIS_STATUS;			}
//		if (LOGIS_DESCRIPTION)		{ delete[] LOGIS_DESCRIPTION;		}
//	}
};

struct stRcmdLotCall : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];			//2024.06.27 - KWMOON(20 -> 128)
	char SORTER_REMOVE_EQUIP_ID[64];		//2024.06.27 - KWMOON(20 -> 64)
//	~stRcmdLotCall()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (SORTER_REMOVE_EQUIP_ID)	{ delete[] SORTER_REMOVE_EQUIP_ID;	}
//	}
};

struct stRcmdLotSet : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char BIB_TYPE[64];
	char BIB_ID[64];
	char BIB_X_SIZE[32];
	char BIB_Y_SIZE[32];
	char P_CODE[32];
	char MLB_CODE[32];
	char MLB_LOT_LIST[64*48];
	char LOT_MAP[64*400];
	char PART_NO[64];
	char LOT_NO[64];
	char TEST_PGM_NO[256];
	char INSERT_MOUNT_MAP[402];
	char ZONE_NO[32];
	char RACK_RFID[32];
//	~stRcmdLotSet()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (BIB_X_SIZE)				{ delete[] BIB_X_SIZE;				}
//		if (BIB_Y_SIZE)				{ delete[] BIB_Y_SIZE;				}
//		if (P_CODE)					{ delete[] P_CODE;					}
//		if (MLB_CODE)				{ delete[] MLB_CODE;				}
//		if (MLB_LOT_LIST)			{ delete[] MLB_LOT_LIST;			}
//		if (LOT_MAP)				{ delete[] LOT_MAP;					}
//		if (PART_NO)				{ delete[] PART_NO;					}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (TEST_PGM_NO)			{ delete[] TEST_PGM_NO;				}
//		if (INSERT_MOUNT_MAP)		{ delete[] INSERT_MOUNT_MAP;		}
//		if (ZONE_NO)				{ delete[] ZONE_NO;					}
//		if (RACK_RFID)				{ delete[] RACK_RFID;				}
//	}
};

struct stRcmdLotSetInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdLotSetInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdManualMaskMap : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdManualMaskMap()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdPmCheck : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char PM_NAME[64];
	char PM_FLAG[16];
//	~stRcmdPmCheck()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (PM_NAME)				{ delete[] PM_NAME;					}
//		if (PM_FLAG)				{ delete[] PM_FLAG;					}
//	}
};

struct stRcmdRackSetInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
//	~stRcmdRackSetInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//	}
};

struct stRcmdRecheckInBoard : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char BIB_TYPE[64];
	char BIB_ID[64];
	char P_CODE[32];
	char MLB_CODE[32];
	char MLB_LOT_LIST[64*48];
	char LOT_MAP[64*400];
//	~stRcmdRecheckInBoard()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (P_CODE)					{ delete[] P_CODE;					}
//		if (MLB_CODE)				{ delete[] MLB_CODE;				}
//		if (MLB_LOT_LIST)			{ delete[] MLB_LOT_LIST;			}
//		if (LOT_MAP)				{ delete[] LOT_MAP;					}
//	}
};

struct stRcmdAutoByPass : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char BYPASS_SELECT[16];
	char EQUID_ID[64];
//	~stRcmdAutoByPass()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BYPASS_SELECT)			{ delete[] BYPASS_SELECT;			}
//		if (EQUID_ID)				{ delete[] EQUID_ID;				}
//	}
};

struct stRcmdReplyAutoBypassEqpState : stRcmdBaseData {
	char BYPASSEQPSTATE[32];
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char EQUID_ID[64];
//	~stRcmdAutoByPass()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BYPASS_SELECT)			{ delete[] BYPASS_SELECT;			}
//		if (EQUID_ID)				{ delete[] EQUID_ID;				}
//	}
};


struct stRcmdRequestBoardMaskSet : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char BIB_TYPE[64];
	char BIB_ID[64];
	char BOARD_MASK_INFO[64];
//	~stRcmdRequestBoardMaskSet()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (BIB_ID)					{ delete[] BIB_ID;					}
//		if (BOARD_MASK_INFO)		{ delete[] BOARD_MASK_INFO;			}
//	}
};

struct stRcmdRequestRackSet : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char GROUP[32];
	char RACK_RFID[32];
	char BOARD_NO1[64];
	char BOARD_NO2[64];
	char BOARD_NO3[64];
	char BOARD_NO4[64];
	char BOARD_NO5[64];
	char BOARD_NO6[64];
	char BOARD_NO7[64];
	char BOARD_NO8[64];
	char LOT_ID1[64];
	char LOT_ID2[64];
	char LOT_ID3[64];
	char LOT_ID4[64];
	char LOT_ID5[64];
	char LOT_ID6[64];
	char LOT_ID7[64];
	char LOT_ID8[64];
//	~stRcmdRequestRackSet()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (GROUP)					{ delete[] GROUP;					}
//		if (RACK_RFID)				{ delete[] RACK_RFID;				}
//		if (BOARD_NO1)				{ delete[] BOARD_NO1;				}
//		if (BOARD_NO2)				{ delete[] BOARD_NO2;				}
//		if (BOARD_NO3)				{ delete[] BOARD_NO3;				}
//		if (BOARD_NO4)				{ delete[] BOARD_NO4;				}
//		if (BOARD_NO5)				{ delete[] BOARD_NO5;				}
//		if (BOARD_NO6)				{ delete[] BOARD_NO6;				}
//		if (BOARD_NO7)				{ delete[] BOARD_NO7;				}
//		if (BOARD_NO8)				{ delete[] BOARD_NO8;				}
//		if (LOT_ID1)				{ delete[] LOT_ID1;					}
//		if (LOT_ID2)				{ delete[] LOT_ID2;					}
//		if (LOT_ID3)				{ delete[] LOT_ID3;					}
//		if (LOT_ID4)				{ delete[] LOT_ID4;					}
//		if (LOT_ID5)				{ delete[] LOT_ID5;					}
//		if (LOT_ID6)				{ delete[] LOT_ID6;					}
//		if (LOT_ID7)				{ delete[] LOT_ID7;					}
//		if (LOT_ID8)				{ delete[] LOT_ID8;					}
//	}
};

struct stRcmdRMVOU : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char TRANS_TYPE[32];
	char LOT_ID[64];
	char RUN_NO[32];
	char WAFER_ID[64];
	char SYSTEM_OP_ID[64];
	char EQUIP_NO[64];
//	~stRcmdRMVOU()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (TRANS_TYPE)				{ delete[] TRANS_TYPE;				}
//		if (LOT_ID)					{ delete[] LOT_ID;					}
//		if (RUN_NO)					{ delete[] RUN_NO;					}
//		if (WAFER_ID)				{ delete[] WAFER_ID;				}
//		if (SYSTEM_OP_ID)			{ delete[] SYSTEM_OP_ID;			}
//		if (EQUIP_NO)				{ delete[] EQUIP_NO;				}
//	}
};

struct stRcmdSlotMaskInfo : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];
	char EQUIP_NO[64];
	char INTERLOCK_NAME[64];
	char EQUIP_MASK[64];
	char TOTAL_SLOT_MASK[64];
	char MASK_INFO[64];
	char SLOT_USE_CHECK[64];
	char SLOT_USE_LIMIT[64];
//	~stRcmdSlotMaskInfo()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (EQUIP_NO)				{ delete[] EQUIP_NO;				}
//		if (INTERLOCK_NAME)			{ delete[] INTERLOCK_NAME;			}
//		if (EQUIP_MASK)				{ delete[] EQUIP_MASK;				}
//		if (TOTAL_SLOT_MASK)		{ delete[] TOTAL_SLOT_MASK;			}
//		if (MASK_INFO)				{ delete[] MASK_INFO;				}
//		if (SLOT_USE_CHECK)			{ delete[] SLOT_USE_CHECK;			}
//		if (SLOT_USE_LIMIT)			{ delete[] SLOT_USE_LIMIT;			}
//	}
};

struct stRcmdStartBiCmd : stRcmdBaseData {
	char BATCH_ID[64];
	char LOT_ID[64];
	char GROUP[32];
	char BID_COUNT[64];
	char RACK_COUNT[64];
	char ZONE_NO1[32];
	char ZONE_NO2[32];
	char ZONE_NO3[32];
	char ZONE_NO4[32];
	char BOARD_ID[64][64];	//2024.08.05 - KWMOON( BOARD_ID1 ~ BOARD_ID16 -> ARRAY)
//	~stRcmdStartBiCmd()
//	{
//		if (BATCH_ID)				{ delete[] BATCH_ID;				}
//		if (LOT_ID)					{ delete[] LOT_ID;					}
//		if (GROUP)					{ delete[] GROUP;					}
//		if (BID_COUNT)				{ delete[] BID_COUNT;				}
//		if (RACK_COUNT)				{ delete[] RACK_COUNT;				}
//		if (ZONE_NO1)				{ delete[] ZONE_NO1;				}
//		if (ZONE_NO2)				{ delete[] ZONE_NO2;				}
//		if (ZONE_NO3)				{ delete[] ZONE_NO3;				}
//		if (ZONE_NO4)				{ delete[] ZONE_NO4;				}
//		if (BOARD_ID1)				{ delete[] BOARD_ID1;				}
//		if (BOARD_ID2)				{ delete[] BOARD_ID2;				}
//		if (BOARD_ID3)				{ delete[] BOARD_ID3;				}
//		if (BOARD_ID4)				{ delete[] BOARD_ID4;				}
//		if (BOARD_ID5)				{ delete[] BOARD_ID5;				}
//		if (BOARD_ID6)				{ delete[] BOARD_ID6;				}
//		if (BOARD_ID7)				{ delete[] BOARD_ID7;				}
//		if (BOARD_ID8)				{ delete[] BOARD_ID8;				}
//		if (BOARD_ID9)				{ delete[] BOARD_ID9;				}
//		if (BOARD_ID10)				{ delete[] BOARD_ID10;				}
//		if (BOARD_ID11)				{ delete[] BOARD_ID11;				}
//		if (BOARD_ID12)				{ delete[] BOARD_ID12;				}
//		if (BOARD_ID13)				{ delete[] BOARD_ID13;				}
//		if (BOARD_ID14)				{ delete[] BOARD_ID14;				}
//		if (BOARD_ID15)				{ delete[] BOARD_ID15;				}
//		if (BOARD_ID16)				{ delete[] BOARD_ID16;				}
//		if (BOARD_ID17)				{ delete[] BOARD_ID17;				}
//		if (BOARD_ID18)				{ delete[] BOARD_ID18;				}
//		if (BOARD_ID19)				{ delete[] BOARD_ID19;				}
//		if (BOARD_ID20)				{ delete[] BOARD_ID20;				}
//		if (BOARD_ID21)				{ delete[] BOARD_ID21;				}
//		if (BOARD_ID22)				{ delete[] BOARD_ID22;				}
//		if (BOARD_ID23)				{ delete[] BOARD_ID23;				}
//		if (BOARD_ID24)				{ delete[] BOARD_ID24;				}
//		if (BOARD_ID25)				{ delete[] BOARD_ID25;				}
//		if (BOARD_ID26)				{ delete[] BOARD_ID26;				}
//		if (BOARD_ID27)				{ delete[] BOARD_ID27;				}
//		if (BOARD_ID28)				{ delete[] BOARD_ID28;				}
//		if (BOARD_ID29)				{ delete[] BOARD_ID29;				}
//		if (BOARD_ID30)				{ delete[] BOARD_ID30;				}
//		if (BOARD_ID31)				{ delete[] BOARD_ID31;				}
//		if (BOARD_ID32)				{ delete[] BOARD_ID32;				}
//	}
};

struct stRcmdTGET : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char SORT[32];
	char TGET[32];
	char LOT_NO[64];				//2024.06.27 - KWMOON(20 -> 64)
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];	//2024.06.27 - KWMOON(20 -> 64*16)
	char PART_NO[64];				//2024.06.27 - KWMOON(20 -> 64)
	char TEST_PGM_NO[64];			//2024.06.27 - KWMOON(20 -> 64)
	char EQUIP_NO[64];
	char BIB_TYPE[64];
	char SYSTEM_OP_ID[64];
	char DEVICE_NO[64];
	char OPER[32];
	char FAB[32];
	char GRADE[32];
	char DEVICE_QTY[32];
	char SPECIAL_CODE[32];
	char PGM_DIR[256];
	char DEVICE_DENSITY[32];
	char DEVICE_TECH[32];
	char BIB_QTY[32];
	char TAMS_IP[32];
	char TAMS_ID[64];
	char TAMS_PWD[64];
	char POWER_UP_RATE_LIMIT[32];
	char POWER_UP_YIELD_LIMIT[32];
	char PGM_TEST_TIME[32];
	char POWER_UP_PDA_LIMIT[32];
	char PGM_EQ_MODEL[64];
	char OS_VERSION[2048];
	char FAMILY[32];
	char OWNER[32];
	char FACTORY[32];
	char RWK_COUNT[32];
	char RWK_CODE[32];
	char RML_GRP_ID[32];
	char MPG[32];
	char Temp1[32];
	char Temp2[32];
	char Temp3[32];
	char EQUIP_OWNER[32];
	char RML_GRP_ID_BIB_QTY[32];

//	~stRcmdTGET()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (SORT)					{ delete[] SORT;					}
//		if (TGET)					{ delete[] TGET;					}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//		if (PART_NO)				{ delete[] PART_NO;					}
//		if (TEST_PGM_NO)			{ delete[] TEST_PGM_NO;				}
//		if (EQUIP_NO)				{ delete[] EQUIP_NO;				}
//		if (BIB_TYPE)				{ delete[] BIB_TYPE;				}
//		if (SYSTEM_OP_ID)			{ delete[] SYSTEM_OP_ID;			}
//		if (DEVICE_NO)				{ delete[] DEVICE_NO;				}
//		if (OPER)					{ delete[] OPER;					}
//		if (FAB)					{ delete[] FAB;						}
//		if (GRADE)					{ delete[] GRADE;					}
//		if (DEVICE_QTY)				{ delete[] DEVICE_QTY;				}
//		if (SPECIAL_CODE)			{ delete[] SPECIAL_CODE;			}
//		if (PGM_DIR)				{ delete[] PGM_DIR;					}
//		if (DEVICE_DENSITY)			{ delete[] DEVICE_DENSITY;			}
//		if (DEVICE_TECH)			{ delete[] DEVICE_TECH;				}
//		if (BIB_QTY)				{ delete[] BIB_QTY;					}
//		if (TAMS_IP)				{ delete[] TAMS_IP;					}
//		if (TAMS_ID)				{ delete[] TAMS_ID;					}
//		if (TAMS_PWD)				{ delete[] TAMS_PWD;				}
//		if (POWER_UP_RATE_LIMIT)	{ delete[] POWER_UP_RATE_LIMIT;		}
//		if (POWER_UP_YIELD_LIMIT)	{ delete[] POWER_UP_YIELD_LIMIT;	}
//		if (PGM_TEST_TIME)			{ delete[] PGM_TEST_TIME;			}
//		if (POWER_UP_PDA_LIMIT)		{ delete[] POWER_UP_PDA_LIMIT;		}
//		if (PGM_EQ_MODEL)			{ delete[] PGM_EQ_MODEL;			}
//		if (OS_VERSION)				{ delete[] OS_VERSION;				}
//		if (FAMILY)					{ delete[] FAMILY;					}
//		if (OWNER)					{ delete[] OWNER;					}
//		if (FACTORY)				{ delete[] FACTORY;					}
//		if (RWK_COUNT)				{ delete[] RWK_COUNT;				}
//		if (RWK_CODE)				{ delete[] RWK_CODE;				}
//		if (RML_GRP_ID)				{ delete[] RML_GRP_ID;				}
//		if (MPG)					{ delete[] MPG;						}
//		if (Temp1)					{ delete[] Temp1;					}
//		if (Temp2)					{ delete[] Temp2;					}
//		if (Temp3)					{ delete[] Temp3;					}
//		if (EQUIP_OWNER)			{ delete[] EQUIP_OWNER;				}
//		if (RML_GRP_ID_BIB_QTY)		{ delete[] RML_GRP_ID_BIB_QTY;		}
//	}
};

struct stRcmdTMVIN : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char SORT[64];
	char TRANS_TYPE[32];
	char LOT_NO[64];
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];	//2024.06.27 - KWMOON(20 -> 64*16)
	char SYSTEM_OP_ID[64];
	char EQP_NO[64];
	char TEST_PGM_NO[256];
	char DEVICE_FAB[32];
	char DEVICE_GRADE[32];
	char DEVICE_TECH[32];
	char DEVICE_DENSITY[32];
//	~stRcmdTMVIN()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (SORT)					{ delete[] SORT;					}
//		if (TRANS_TYPE)				{ delete[] TRANS_TYPE;				}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//		if (SYSTEM_OP_ID)			{ delete[] SYSTEM_OP_ID;			}
//		if (EQP_NO)					{ delete[] EQP_NO;					}
//		if (TEST_PGM_NO)			{ delete[] TEST_PGM_NO;				}
//		if (DEVICE_FAB)				{ delete[] DEVICE_FAB;				}
//		if (DEVICE_GRADE)			{ delete[] DEVICE_GRADE;			}
//		if (DEVICE_TECH)			{ delete[] DEVICE_TECH;				}
//		if (DEVICE_DENSITY)			{ delete[] DEVICE_DENSITY;			}
//	}
};

struct stRcmdTMVOU : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char TRANS_TYPE[32];
	char LOT_NO[64];
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];		//2024.06.27 - KWMOON(20 -> 64*16)
	char SYSTEM_OP_ID[64];
	char EQP_NO[64];
//	~stRcmdTMVOU()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (TRANS_TYPE)				{ delete[] TRANS_TYPE;				}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//		if (SYSTEM_OP_ID)			{ delete[] SYSTEM_OP_ID;			}
//		if (EQP_NO)					{ delete[] EQP_NO;					}
//	}
};

struct stRcmdVMVOU : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];	//2024.06.27 - KWMOON(20 -> 128)
	char TRANS_TYPE[32];
	char LOT_NO[64];
	char MLB_FLAG[16];
	char MLB_PGM_LOT_LIST[64*48];	//2024.06.27 - KWMOON(20 -> BIB_QTY[32];)
	char SYSTEM_OP_ID[64];
	char EQP_NO[64];
//	~stRcmdVMVOU()
//	{
//		if (RETURN_FLAG)			{ delete[] RETURN_FLAG;				}
//		if (ERROR_DESCRIPTION)		{ delete[] ERROR_DESCRIPTION;		}
//		if (TRANS_TYPE)				{ delete[] TRANS_TYPE;				}
//		if (LOT_NO)					{ delete[] LOT_NO;					}
//		if (MLB_FLAG)				{ delete[] MLB_FLAG;				}
//		if (MLB_PGM_LOT_LIST)		{ delete[] MLB_PGM_LOT_LIST;		}
//		if (SYSTEM_OP_ID)			{ delete[] SYSTEM_OP_ID;			}
//		if (EQP_NO)					{ delete[] EQP_NO;					}
//	}
};

struct stRcmdReadyToUnload : stRcmdBaseData {
	char RETURN_FLAG[16];
	char ERROR_DESCRIPTION[128];		//2024.06.27 - KWMOON(20 -> 128)
	char EQUIP_ID[32];
	char ZONE[32];
	char STATUS[32];
	char LOGIS_DESCRIPTION[128];			//2024.06.27 - KWMOON(20 -> 128)
};

typedef struct _stS6F11_1014 {
	// undefined
} stS6F11_1014;

typedef struct _stS6F11_1015 {
	// undefined
} stS6F11_1015;

typedef struct _stS6F11_2006 {
	char* pEvenTime;
	char* pAlarmId;
	char* pAlarmText;
} stS6F11_2006;

typedef struct _stS6F11_2007 {
	char* pEvenTime;
	char* pAlarmId;
} stS6F11_2007;

typedef struct _stS6F11_3300 {
	char* pEqId;
	char* pBibType;
} stS6F11_3300;

typedef struct _stS6F11_3400 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pSlotId;
	char* pMlbEqpFlag;
} stS6F11_3400;

typedef struct _stS6F11_3410 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pPCode;
} stS6F11_3410;

typedef struct _stS6F11_3420 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pPartNo;
	char* pLotNo;
	char* pOpId;
	char* pComment;
} stS6F11_3420;

typedef struct _stS6F11_3500 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pPCode;
	char* pSystemStartTime;
	unsigned int uInsertDevQty;
	char* pPartNo;
	char* pOpId;
	char* pSlotId;
	char* pLotNo;
	char* pMlbCode;
	char* pMlbEqpFlag;
} stS6F11_3500;

typedef struct _stS6F11_3600 {
	char* pBibType;
	char* pBibId;
	char* pBibXSize;
	char* pBibYSize;
	char* pCheckinBoardTime;
	char* pPCode;
	char* pIoc;
	char* pBoc;
	char* pBurnInCount;
	char* pPartNo;
	char* pLotNo;
	unsigned int uSubLotNo;
	char* pTestPgmName;
	char* pEqId;
	char* pSystemStartTime;
	char* pSystemEndTime;
	unsigned int uInsertDevQty;
	char* pOpId;
	char* pSlotId;
	char* pInsertMap;
	char* pInitBinValue;
	unsigned int uTestTotalQty;
	unsigned int uTestTotalPassQty;
	unsigned int uTestTotalFailQty;
	char* pMlbCode;
	char* pMlbLotList;
	char* pLotMap;
	char* pFirstInitialBinValue;
	char* pMlbEqpFlag;
} stS6F11_3600;

typedef struct _stS6F11_3699 {
	char* pReBibType;
	char* pReBibId;
	char* pReBibXSize;
	char* pReBibYSize;
	char* pReCheckinBoardTime;
	char* pRePCode;
	char* pReIoc;
	char* pReBoc;
	char* pReBurnInCount;
	char* pRePartNo;
	char* pReLotNo;
	unsigned int uReSubLotNo;
	char* pReTestPgmName;
	char* pReEqId;
	char* pReSystemStartTime;
	char* pReSystemEndTime;
	unsigned int uReInsertDevQty;
	char* pReOpId;
	char* pReEpSlotId;
	char* pReSystemBinMap;
	char* pReInitBinValue;
	unsigned int uReTestTotalQty;
	unsigned int uReTestTotalPassQty;
	unsigned int uReTestTotalFailQty;
	char* pReMlbCode;
	char* pReMlbLotList;
	char* pReLotMap;
	char* pReFirstInitialBinValue;
} stS6F11_3699;

typedef struct _stS6F11_3900 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
} stS6F11_3900;

typedef struct _stS6F11_3901 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pPartNo;
	char* pLotNo;
	char* pOpId;
	char* pPCode;
	char* pTestPgmName;
	char* pSystemBinMap;
	unsigned int uZoneNo;
	char* pRackRfid;
	char* pMlbCode;
	char* pMlbLotList;
	char* pLotMap;
} stS6F11_3901;

typedef struct _stS6F11_3902 {
	char* pEqId;
	char* pRackRfid;
} stS6F11_3902;

typedef struct _stS6F11_3903 {
	char* pEqId;
	char* pGroup;
	unsigned int uZoneNo;
	char* pRackRfid;
	char* pOpId;
	char* pBoardNo1;
	char* pBoardNo2;
	char* pBoardNo3;
	char* pBoardNo4;
	char* pBoardNo5;
	char* pBoardNo6;
	char* pBoardNo7;
	char* pBoardNo8;
	char* pLotId1;
	char* pLotId2;
	char* pLotId3;
	char* pLotId4;
	char* pLotId5;
	char* pLotId6;
	char* pLotId7;
	char* pLotId8;
} stS6F11_3903;

typedef struct _stS6F11_3910 {
	char* pEqId;
	char* pBibType;
	char* pBibId;
	char* pOpId;
	char* pManualMaskMap;
} stS6F11_3910;

typedef struct _stS6F11_4200 {
	// undefined
} stS6F11_4200;

typedef struct _stS6F11_4400 {
	char* pLotNo;
	char* pOpId;
	char* pBibType;
	char* pEqId;
	char* pOsVersion;
	char* pMlbEqpFlag;
} stS6F11_4400;

typedef struct _stS6F11_4500 {
	char* pProcess;
	char* pPartNo;
	char* pLotNo;
	unsigned int uDeviceQty;
	char* pEqId;
	char* pTestPgmName;
	char* pBibType;
	char* pOpId;
	char* pTestDataTime;
	char* pFamily;
	char* pMpgFlag;
	char* pMlbFlag;
	char* pMlbPgmLotList;
	char* pMlbEqpFlag;
} stS6F11_4500;

typedef struct _stS6F11_4600 {
	char* pPCode;
	char* pPartNo;
	char* pLotNo;
	unsigned int uSubLotNo;
	unsigned int uDeviceQty;
	char* pEqId;
	char* pTestPgmName;
	char* pBibType;
	char* pOpId;
	char* pTestEndTime;
	char* pAutoMode;
	char* pFamily;
	char* pPowerFaultCount;
	char* pFirstInitialBin1;
	char* pInitTotalCount;
	char* pBin0Count;
	char* pSpecialCode;
	char* pMlbFlag;
	char* pMlbPgmLotList;
} stS6F11_4600;

typedef struct _stS6F11_4699 {
	char* pReProcess;
	char* pRePartNo;
	char* pReLotNo;
	unsigned int uReSubLotNo;
	unsigned int uReDeviceQty;
	char* pReEqId;
	char* pReTestPgmName;
	char* pReBibType;
	char* pReOpId;
	char* pReTestEndTime;
	char* pReAutoMode;
	char* pReFamily;
	char* pRePowerFaultCount;
	char* pReMlbFlag;
	char* pReMlbPgmLotList;
} stS6F11_4699;

typedef struct _stS6F11_5110 {
	char* pEqId;
	char* pZone;
	char* pEmptyDateTime;
	char* pMuluPort;
} stS6F11_5110;

typedef struct _stS6F11_5120 {
	char* pEqId;
	unsigned int uZoneNo;
	char* pDepositDateTime;
} stS6F11_5120;

typedef struct _stS6F11_5130 {
	char* pEqId;
	char* pZone;
	char* pEmptyDateTime;
	char* pMuluPort;
} stS6F11_5130;

typedef struct _stS6F11_5140 {
	char* pEqId;
	unsigned int uZoneNo;
	char* pDepositDateTime;
} stS6F11_5140;

typedef struct _stS6F11_5150 {
	char* pEqId;
	char* pZone;
	char* pEmptyDateTime;
	char* pMuluPort;
} stS6F11_5150;

typedef struct _stS6F11_5200 {
	char* pEqId;
	unsigned int uRackCount;
	char* pZone1Flag;
	char* pZone2Flag;
	char* pZone3Flag;
	char* pZone4Flag;
} stS6F11_5200;

typedef struct _stS6F11_5300 {
	char* pTransType;
	char* pLotNo;
	char* pOpId;
	char* pEqId;
	unsigned int uOutQty;
	char* pTestPgmName;
	unsigned int uBibQty;
	char* pBibType;
	char* pEstimatedTime;
	char* pTestStartTime;
	char* pFamily;
	char* pMlbFlag;
	char* pMlbPgmLotList;
} stS6F11_5300;

typedef struct _stS6F11_5400 {
	char* pTransType;
	char* pLotNo;
	char* pOpId;
	char* pEqId;
	unsigned int uInQty;
	unsigned int uOutQty;
	unsigned int uBibQty;
	char* pBinCode;
	char* pBinQty;
	char* pBibIdList;
	char* pTpgFwInfo;
	char* pDrvFwInfo;
	char* pOsVersion;
	char* pFamily;
	char* pArmlGrpId;
	char* pMpgFlag;
	char* pMlbFlag;
	char* pMlbPgmLotList;
} stS6F11_5400;

typedef struct _stS6F11_5500 {
	char* pEqId;
	char* pLotNo;
} stS6F11_5500;

typedef struct _stS6F11_5600 {
	char* pTransType;
	char* pLotNo;
	char* pRunNo;
	char* pWaferNo;
	char* pOpId;
	char* pEqId;
	unsigned int uInQty;
	char* pBinNo;
	char* pPdaLimit;
	char* pPdaBin1;
	char* pPdaRef1;
	char* pPdaBin2;
	char* pPdaRef2;
	char* pPdaBin3;
	char* pPdaRef3;
	unsigned int uBibQty;
	char* pBibIdList;
} stS6F11_5600;

typedef struct _stS6F11_6100 {
	char* pEqId;
	char* pLotNo;
	char* pPartNo;
	char* pHotTempFlag;
	char* pOpId;
	char* pInitStartTime;
	char* pFamily;
	char* pMlbFlag;
	char* pMlbPgmLotList;
} stS6F11_6100;

typedef struct _stS6F11_6200 {
	char* pEqId;
	char* INSERTpEqId;
	char* pLotNo;
	char* pPartNo;
	char* pBibType;
	char* pBibId;
	char* pOpId;
	char* pSlotId;
	char* pTestPgmName;
	char* pIoc;
	char* pHotTempFlag;
	char* pInitTotalCount;
	unsigned int uInitTotalQty;
	unsigned int uInitPassQty;
	unsigned int uInitFailQty;
	unsigned int uInitMaskQty;
	char* pInitStartTime;
	char* pInitEndTime;
	char* pInitBinValue;
	char* pMlbCode;
	char* pMlbLotList;
	char* pLotMap;
} stS6F11_6200;

typedef struct _stS6F11_6300 {
	char* pPartNo;
	char* pLotNo;
	char* pStepStartTime;
	char* pStepName;
	char* pStepNo;
	char* pEqId;
	char* pBibId;
	char* pBibType;
	char* pSetTemp;
	char* pCurrentTemp;
} stS6F11_6300;

typedef struct _stS6F11_6400 {
	char* pPartNo;
	char* pLotNo;
	char* pStepStartTime;
	char* pStepEndTime;
	char* pStepName;
	char* pStepNo;
	char* pStepResultRate;
	char* pEqId;
	char* pBibId;
	char* pBibType;
	char* pSetTemp;
	char* pCurrentTemp;
	char* pVoltPs1;
	char* pVoltPs2;
	char* pVoltPs3;
	char* pSlotId;
	unsigned int uStepTotalQty;
	unsigned int uStepPassQty;
	unsigned int uStepFailQty;
} stS6F11_6400;

typedef struct _stS6F11_6600 {
	char* pTransType;
	char* pLotNo;
	char* pOpId;
	char* pEqId;
	unsigned int uInQty;
	unsigned int uOutQty;
	unsigned int uBibQty;
	char* pBinCode;
	char* pBinQty;
	char* pBibIdList;
	char* pTpgFwInfo;
	char* pDrvFwInfo;
	char* pOsVersion;
	char* pFamily;
	char* pArmlGrpId;
	char* pMpgFlag;
	char* pMlbFlag;
	char* pMlbPgmLotList;
} stS6F11_6600;

typedef struct _stS6F11_7000 {
	char* pEqId;
	char* MONITORING;
	char* pComment;
	char* pLotNo;
	char* pOpId;
	char* pParaOrg;
	char* pParaNew;
	char* pLoadPara;
	char* pLoadPkg;
	char* pLoadBib;
	char* pUnloadPara;
	char* pUnloadPkg;
	char* pUnloadBib;
	char* pDc1;
	char* pDc2;
	char* pBibType;
	char* pCallStartTime;
	char* pZone1Flag;
	char* pZone2Flag;
	char* pZone3Flag;
	char* pZone4Flag;
} stS6F11_7000;

typedef struct _stS6F11_7007 {
	char* pEqId;
	char* pBypassSelect;
	char* pBypassDetail;
	unsigned int uSeqCountNo;
} stS6F11_7007;

typedef struct _stS6F11_7010 {
	char* pEqId;
	char* pStatusId;
	char* pStatusTime;
} stS6F11_7010;

typedef struct _stS6F11_7100 {
	char* pEqId;
} stS6F11_7100;

typedef struct _stS6F11_7300 {
	char* pEqId;
	char* pBibType;
} stS6F11_7300;

typedef struct _stS6F11_7320 {
	char* pEqId;
} stS6F11_7320;

typedef struct _stS6F11_7340 {
	char* pEqId;
	char* pBibType;
} stS6F11_7340;

typedef struct _stS6F11_7360 {
	char* pEqId;
	char* pBibType;
} stS6F11_7360;

typedef struct _stS6F11_7400 {
	char* pEqId;
	char* pBibType;
} stS6F11_7400;

typedef struct _stS6F11_8000 {
// undefined
} stS6F11_8000;

typedef struct _stS6F11_8001 {
	char* pEqId;
	char* pBypassSelect;
	char* pBypassDetail;
} stS6F11_8001;

typedef struct _stS6F11_8100 {
	char* pEqId;
	char* pPartNo;
	char* pLotNo;
	char* MODE;
	char* pTestPgmName;
	char* pSystemStartTime;
	char* pItemStartTime;
	char* pItemEndTime;
	char* pTimeRequired;
	char* pCurrentTemp;
	char* pSetTemp;
	char* pBibType;
	char* pStepNo;
} stS6F11_8100;

typedef struct _stS6F11_8300 {
	char* pEqId;
	char* pLotNo;
	char* pPCode;
	char* pTestPgmName;
	char* pSpecialCode;
	char* pInitStartTime;
	char* pInitEndTime;
	char* pBeforeBiTestStartTime;
	char* pBeforeBiTestEndTime;
	char* pBiTestStartTime;
	char* pBiTestEndTime;
	char* pAfterBiTestStartTime;
	char* pAfterBiTestEndTime;
	char* pTempUpStartTime1;
	char* pTempUpEndTime1;
	char* pTempUpStartTime2;
	char* pTempUpEndTime2;
	char* pTempDownStartTime1;
	char* pTempDownEndTime1;
	char* pTempDownStartTime2;
	char* pTempDownEndTime2;
	char* pOtherStartTime;
	char* pOtherEndTime;
	char* pBibDensityInfo;
	char* pTpgFwInfo;
	char* pDrvFwInfo;
	char* pOsVersion;
} stS6F11_8300;

///////////////////////////////////////////////////
/// MAIN UI  CHAMBER MGR <-> HSMS MANAGER
/// 2024.06.10 - KWMOON
///////////////////////////////////////////////////
typedef struct _stChamberStatus
{
	bool bChmbCon;		//2024.06.12 - KWMOON
	bool bMaskOff;
	bool bMaskOn;
	bool bDoorOpen;
	bool bDoorClose;
	bool bUpperRackDetect;
	bool bBottomRackDetect;
	bool bRackInComplete;
	bool bRackOutComplete;
	bool bEqReadyStatus;
	bool bEqAbortStatus;
	bool bEqAgvComplete;
	bool bRun;
	bool bStop;
	bool bAmb;
	double dPV;		//2024.07.23 - KWMOON
	double dSV;		//2024.07.23 - KWMOON
}stChamberStatus;

struct stSetupTimePair {
	 time_t start_time;
	 time_t end_time;
};

#pragma pack()

#endif
