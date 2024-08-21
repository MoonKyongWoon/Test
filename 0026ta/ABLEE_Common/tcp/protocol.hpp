#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include "chmbprotocol.hpp"

namespace PROTOCOL
{
	//2023.08.10 - KWMOON
	enum EN_KEEP_ALIVE
	{
		EN_COMM_PERIOD  =  5,
		EN_WARN_PERIOD  =  9,
		EN_FAULT_PERIOD = 15,
	};

	enum EN_PGM_RUNNER
	{
		EN_PGM_CPU0      = 0x0,
		EN_PGM_RUN0      = 0x0,
		EN_PGM_CPU1      = 0x1,
		EN_PGM_RUN1      = 0x1,
		EN_PGM_MAX_RUNNER     ,
	};

	enum EN_PGM_STATUS_CODE
	{
		EN_PGM_RUNNING      ,
		EN_PGM_TERMINATED   ,
		EN_PGM_UNKNOWN_STAT ,
	};

	enum EN_PGM_SOCKET_TYPE
	{
		EN_CTRL_SOCKET     ,
		EN_DATA_SOCKET     ,
		EN_LOG_SOCKET      ,
		EN_MAX_PGM_SOCKET  ,
	};

	enum SOURCE_TYPE
	{
		_ATMAIN_UI           ,
		_ATMGR               ,
		_ATLODER             ,
		_ATMAIN_UI_CHMB      ,
		_ATCHMB_UI           ,
		_ATCHMB_COMM         ,
		_ATPPSMON_UI         ,
		_ATPPSMON_COMM       ,
		_ATSYSMON_UI         ,
		_ATSYSMON_COMM       ,
		_ATTMS_UI            ,
		_ATSLOTS             ,		//CON SLOTS
		_ATSLOT01            ,
		_ATSLOT02            ,
		_ATSLOT03            ,
		_ATSLOT04            ,
		_ATSLOT05            ,
		_ATSLOT06            ,
		_ATSLOT07            ,
		_ATSLOT08            ,
		_ATSLOT09            ,
		_ATSLOT10            ,
		_ATSLOT11            ,
		_ATSLOT12            ,
		_ATSLOT13            ,
		_ATSLOT14            ,
		_ATSLOT15            ,
		_ATSLOT16            ,
		_ATCHMB_PLC          ,		//2023.05.15 - KWMOON(CHAMBER)
		_ATCHMB_CI           ,		//2023.05.18 - KWMOON(RFU)
		_APP_TYPE_COUNT      ,
		_ATPFMD				 ,		//2023.08.01 - hsbae
		_ATTMS_HSMS          ,		//2024.04.26 - kwmoon
		_ATCHMB_UI_COMPRESS  ,		//2024.08.19 - kwmoon
		_ATCHMB_UI_ALARM     ,		//2024.08.19 - kwmoon
		_ATCHMB_UI_PCW       ,		//2024.08.19 - kwmoon
	};

	enum PACKET_RESULT_CODE
	{
		NACK_ERROR       = -1,
		ACK_NORMAL           ,
	};

	enum PACKET_SIZE
	{
		MAX_IPC_BUFFER             = 8192,
		MAX_TCP_BUFFER             = 8192,
	};

	enum TCP_COMMAND
	{
		CMD_SLOT_INFO_REQ          = 0x1000,	//UI -> MGR(SLOT MASK)
		CMD_SLOT_INFO_ACK          = 0x1001,	//UI <- MGR(SLOT MASK)
		CMD_LIVE_SLOT_REQ          = 0x1002,
		CMD_LIVE_SLOT_ACK          = 0x1003,
		CMD_LIVE_TMS_REQ           = 0x1004,
		CMD_LIVE_TMS_ACK           = 0x1005,
		CMD_LIVE_TMSSVR_REQ        = 0x1006,
		CMD_LIVE_TMSSVR_ACK        = 0x1007,
		CMD_LIVE_PGM_REQ           = 0x1008,
		CMD_LIVE_PGM_ACK           = 0x1009,
		CMD_SLOT_CPU_INFO_SEND     = 0x100a,	//MGR -> LOADER
		CMD_SLOT_CPU_INFO_ACK      = 0x100b,	//LOADER -> MGR
		CMD_EWS_FTP_INFO_SEND      = 0x100c,
		CMD_EWS_FTP_INFO_ACK       = 0x100d,
		CMD_LOADER_INFO_REQ        = 0x100e,
		CMD_LOADER_INFO_ACK        = 0x100f,
		//CMD_SETUP_BIBID_REQ        = 0x1010,	//2024.06.17 - kwmoon(NOT USE)
		//CMD_SETUP_BIBID_ACK        = 0x1011,	//2024.06.17 - kwmoon(NOT USE)
		CMD_PGM_LOAD_REQ           = 0x1012,
		CMD_PGM_LOAD_STATUS        = 0x1013,
		CMD_PGM_LOAD_ACK           = 0x1014,
		CMD_PGM_RELOAD_REQ         = 0x1015,
		CMD_PGM_RELOAD_ACK         = 0x1016,
		CMD_PGM_UNLOAD_REQ         = 0x1017,
		CMD_PGM_UNLOAD_ACK         = 0x1018,
		CMD_PGM_FINISHED           = 0x1019,
		CMD_VERSION_REQ            = 0x101a,
		CMD_VERSION_ACK            = 0x101b,
		//CMD_TEST_DATA_INIT_REQ     = 0x101c,	//2023.03.06 - kwmoon(NOT USE)
		//CMD_TEST_DATA_INIT_ACK     = 0x101d,	//2023.03.06 - kwmoon(NOT USE)
		CMD_UI_CHMBUSE_REQ         = 0x101e,	//2023.06.25 - kwmoon ui->test mgr chmb use
		CMD_UI_CHMBUSE_ACK         = 0x101f,	//2023.06.25 - kwmoon ui->test mgr chmb use

		CMD_READ_BIBID_REQ         = 0x1020,	//2024.06.17 - kwmoon
		CMD_READ_BIBID_ACK         = 0x1021,	//2024.06.17 - kwmoon
		CMD_ENGR_SETUP_REQ         = 0x1022,	//2024.06.17 - kwmoon
		CMD_ENGR_SETUP_ACK         = 0x1023,	//2024.06.17 - kwmoon
		CMD_AUTO_SETUP_REQ         = 0x1024,	//2024.06.17 - kwmoon
		CMD_AUTO_SETUP_ACK         = 0x1025,	//2024.06.17 - kwmoon
		CMD_UI_MODE_SEND           = 0x1026,	//2024.06.17 - kwmoon
		CMD_UI_MODE_ACK            = 0x1027,	//2024.06.17 - kwmoon

		CMD_PFM_SYSMONUI_SEND      = 0x1100,	//2023.08.01 - hsbae atpfmd->atsysmonui
		CMD_PFM_SYSMONUI_ACK       = 0x1101,	//2023.08.01 - hsbae atsysmonui->atpfmd

		CMD_LOT_ID_REQ             = 0x2000,	//UI <-> MGR <-> PGM
		CMD_LOT_ID_ACK             = 0x2001,
		CMD_PART_NAME_REQ          = 0x2002,
		CMD_PART_NAME_ACK          = 0x2003,
		CMD_INIT_TEST_START_REQ    = 0x2004,	//init test
		CMD_INIT_TEST_START_ACK    = 0x2005,
		CMD_TEST_START_REQ         = 0x2006,	//test
		CMD_TEST_START_ACK         = 0x2007,
		CMD_ITEM_TEST_START_REQ    = 0x2008,	//jump, manual test
		CMD_ITEM_TEST_START_ACK    = 0x2009,
		CMD_TEST_STOP_REQ          = 0x200a,	//stop test
		CMD_TEST_STOP_ACK          = 0x200b,
		CMD_UI_TESTINFO_REQ        = 0x200c,	//ui -> test info(bibid/ui mode/chmb use)
		CMD_UI_TESTINFO_ACK        = 0x200d,

		CMD_INIT_TEST_DATA         = 0x2010,	//pgm -> testmgr : init Test result
		CMD_CHIPID_INFO            = 0x2011,	//pgm -> testmgr : chipid info(format/size..)
		CMD_TEST_STEP_BEGIN        = 0x2012,	//pgm -> testmgr : step begin
		CMD_TEST_STEP_END          = 0x2013,	//pgm -> testmgr : step end
		CMD_STEP_FAIL_DATA         = 0x2014,	//pgm -> testmgr : step fail data
		CMD_STEP_FAIL_BLOCK_DATA   = 0x2015,	//pgm -> testmgr : step fail block
		CMD_STEP_FAIL_CHIP_DATA    = 0x2016,	//pgm -> testmgr : step fail chip
		CMD_INIT_TEST_DONE         = 0x2017,	//pgm -> testmgr : init test done
		CMD_CUM_FAIL_BLOCK_DATA    = 0x2018,	//pgm -> testmgr : cum fail block
		CMD_CUM_FAIL_CHIP_DATA     = 0x2019,	//pgm -> testmgr : cum fail chip
		CMD_CUM_FAIL_DATA          = 0x201A,	//pgm -> testmgr : test fail data
		CMD_STEP_SORTBIN_DATA      = 0x201B,	//pgm -> testmgr : step bin data
		CMD_CUM_SORTBIN_DATA       = 0x201C,	//pgm -> testmgr : test bin(cum sort) data
		CMD_CHIPID_DATA            = 0x201D,	//pgm -> testmgr : chipid data
		CMD_STEP_IN_INFO           = 0x201E,	//pgm -> testmgr : step in		//2023.04.24
		CMD_STEP_OUT_INFO          = 0x201F,	//pgm -> testmgr : step out		//2023.04.24
		CMD_CYCLE_IN_INFO          = 0x2020,	//pgm -> testmgr : cycle in		//2023.04.26
		CMD_CYCLE_OUT_INFO         = 0x2021,	//pgm -> testmgr : cycle out	//2023.04.26
		CMD_GET_BLOCK_DATA         = 0x2022,	//pgm -> testmgr : BLOCK DATA	//2023.04.27
		CMD_CUM_FAIL_CATEGORY      = 0x2023,	//pgm -> testmgr : BLOCK DATA	//2023.06.24 - KWMOON

		CMD_STEP_FCATE_DATA        = 0x2024,	//pgm -> testmgr : step FCAT data	//2024.07.22 fatric
		CMD_STEP_PCATE_DATA        = 0x2025,	//pgm -> testmgr : step PCAT data	//2024.07.22 fatric
		CMD_BINMAP_DATA            = 0x2026,	//pgm -> testmgr : step binmap data	//2024.07.22 fatric
		CMD_STEP_FAIL_MODE         = 0x2027,	//pgm -> testmgr : step FAILMODE	//2024.07.22 fatric
		CMD_STEP_USE_MODE          = 0x2028,	//pgm -> testmgr : step USE MODE	//2024.08.18 fatric

		CMD_TEST_DONE              = 0x202F,	//pgm -> testmgr : test done

		CMD_SLOT_RUN_NO_SEND       = 0x2040,	//PGM -> MGR(EWS)
		CMD_PGM_LOAD_STATUS_SEND   = 0x2041,	//PGM -> MGR(EWS)
		CMD_PGM_ITEM_LISTUP        = 0x2042,	//PGM -> MGR(EWS)
		CMD_LOG_PRINT              = 0x2043,	//PGM -> MGR(EWS)
		CMD_PGM_RUNTIME_ERROR      = 0x2044,	//PGM -> MGR(EWS)

		CMD_DEV_PGM_VARIABLE       = 0x2050,	//PGM -> MGR(EWS)
		CMD_VAR_SET_BITIME	       = 0x2051,	//PGM -> MGR(EWS) : NOT USE

		CMD_CHMB_INIT_TEMP_REQ     = 0x2060,	//Slot Sync
		CMD_CHMB_INIT_TEMP_ACK     = 0x2061,	//Slot Sync
		CMD_CHMB_ENTER_SYNC_REQ    = 0x2062,	//Slot Sync
		CMD_CHMB_ENTER_SYNC_ACK    = 0x2063,	//Slot Sync
		CMD_CHMB_SETSV_VALID_REQ   = 0x2064,	//Slot Sync
		CMD_CHMB_SETSV_VALID_ACK   = 0x2065,	//Slot Sync
		CMD_CHMB_RUN_SYNC_REQ      = 0x2066,	//Slot Sync
		CMD_CHMB_RUN_SYNC_ACK      = 0x2067,	//Slot Sync
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// 2024.07.26 - KWMOON(CHAMBER SV SET , SV COMPLETE STATUS SEND TO MAIN UI FROM TESTMGR)
		////////////////////////////////////////////////////////////////////////////////////////////////////
		CMD_CHMB_TEMP_MON_SET    = 0x2068,	//SV START
		CMD_CHMB_TEMP_MON_END    = 0x2069,	//SV SET TIME SEND ACK FROM MAIN UI TO TETMGR

		////////////////////////////////////////////////////////////////////////////////////////////////////
		CMD_CHMB_READ_TEMP_REQ     = 0x2070,	//Chamber Ctrl
		CMD_CHMB_READ_TEMP_ACK     = 0x2071,	//Chamber Ctrl
		CMD_CHMB_READ_ALARM_REQ    = 0x2072,	//Chamber Ctrl
		CMD_CHMB_READ_ALARM_ACK    = 0x2073,	//Chamber Ctrl
		CMD_CHMB_READ_STAT1_REQ    = 0x2074,	//Chamber Ctrl
		CMD_CHMB_READ_STAT1_ACK    = 0x2075,	//Chamber Ctrl
		CMD_CHMB_READ_STAT2_REQ    = 0x2076,	//Chamber Ctrl
		CMD_CHMB_READ_STAT2_ACK    = 0x2077,	//Chamber Ctrl
		CMD_CHMB_DOOR_CLOSE_REQ    = 0x2078,	//Chamber Ctrl
		CMD_CHMB_DOOR_CLOSE_ACK    = 0x2079,	//Chamber Ctrl
		CMD_CHMB_DOOR_OPEN_REQ     = 0x207A,	//Chamber Ctrl
		CMD_CHMB_DOOR_OPEN_ACK     = 0x207B,	//Chamber Ctrl
		CMD_CHMB_RACKIN_REQ        = 0x207C,	//Chamber Ctrl
		CMD_CHMB_RACKIN_ACK        = 0x207D,	//Chamber Ctrl
		CMD_CHMB_RACKOUT_REQ       = 0x207E,	//Chamber Ctrl
		CMD_CHMB_RACKOUT_ACK       = 0x207F,	//Chamber Ctrl
		CMD_CHMB_SET_SV_REQ        = 0x2080,	//Chamber Ctrl
		CMD_CHMB_SET_SV_ACK        = 0x2081,	//Chamber Ctrl
		CMD_CHMB_SET_AMB_REQ       = 0x2082,	//Chamber Ctrl
		CMD_CHMB_SET_AMB_ACK       = 0x2083,	//Chamber Ctrl
		CMD_CHMB_RESET_ALARM_REQ   = 0x2084,	//Chamber Ctrl
		CMD_CHMB_RESET_ALARM_ACK   = 0x2085,	//Chamber Ctrl
		CMD_CHMB_RUN_REQ           = 0x2086,	//Chamber Ctrl
		CMD_CHMB_RUN_ACK           = 0x2087,	//Chamber Ctrl
		CMD_CHMB_STOP_REQ          = 0x2088,	//Chamber Ctrl
		CMD_CHMB_STOP_ACK          = 0x2089,	//Chamber Ctrl
		CMD_CHMB_MASK_ONOFF_REQ    = 0x208A,	//Chamber Ctrl
		CMD_CHMB_MASK_ONOFF_ACK    = 0x208B,	//Chamber Ctrl
		CMD_CHMB_RACK_REMOTE_REQ   = 0x208C,	//2023.09.13 - KWMOON(mainui -> testmgr)
		CMD_CHMB_RACK_REMOTE_ACK   = 0x208D,	//2023.09.13 - KWMOON(mainui -> testmgr)
		CMD_CHMB_CONN_REQ          = 0x208E,	//2023.09.13 - KWMOON(mainui -> testmgr)
		CMD_CHMB_CONN_ACK          = 0x208F,	//2023.09.13 - KWMOON(mainui -> testmgr)
		CMD_KEEP_ALIVE             = 0x2222,	//2023.09.13 - KWMOON(socket alive check 0x9999->0x2222)

		CMD_TMSUI_WITH_MAIN_MSG	   = 0x3000,    //2024.06.13 - YSEOM -> KWMOON change name
		CMD_TMSUI_EAP_CONN_STAT	   = 0x3001,    //2024.06.13 - YSEOM -> KWMOON change name
		CMD_TMS_SET_ONLINE_MODE	   = 0x3002,    //2024.06.13 - YSEOM
	};

	//2023.03.18 - KWMOON
	enum PLC_COMMAND
	{
		CMD_WB_TEMP_RUN_TRIGGER    = 0x3000 ,
		CMD_WB_TEMP_STOP_TRIGGER            ,
		CMD_WB_AMB_MODE_TRIGGER             ,
		CMD_WB_AMB_AUTO_TRIGGER             ,
		CMD_WB_ALARM_RESET                  ,
		CMD_WB_MASK_ONOFF_TRIGGER           ,
		CMD_WB_DOOR_OPEN_TRIGGER            ,
		CMD_WB_DOOR_CLOSE_TRIGGER           ,
		CMD_WB_RACK_IN_TRIGGER              ,
		CMD_WB_RACK_OUT_TRIGGER             ,
		CMD_WB_RACK_NOP_TRIGGER             ,
		CMD_WW_WATCHDOG_TIME_SET            ,
		CMD_WW_WATCHDOG_TOGGLE_TRIGGER      ,
		CMD_WW_WATCHDOG_ENB_DSB             ,
		CMD_WW_SV_WRITE                     ,
		CMD_WW_AMB_SV_WRITE                 ,

		CMD_RB_RACK_IN                      ,
		CMD_RB_RACK_OUT                     ,

		CMD_RB_DOOR_OPEN                    ,
		CMD_RB_DOOR_CLOSE                   ,

		CMD_RB_TEMP_RUN                     ,
		CMD_RB_TEMP_STOP                    ,
		CMD_RB_AMBIENT                      ,

		CMD_RW_PLC_VER                      ,
		CMD_RW_PV                           ,
		CMD_RW_SV                           ,
		CMD_RW_PCW_TEMP                     ,
		CMD_RW_WPS_PRESSURE                 ,
		CMD_RW_PCW_FLOW                     ,

		CMD_RW_STATUS1                      ,	//I00210 ~ I00224
		CMD_RW_STATUS2                      ,	//I00226 ~ I00241
		CMD_RW_ALARM0                       ,	//I1600 ~ I1615
		CMD_RW_ALARM1                       ,
		CMD_RW_ALARM2                       ,
		CMD_RW_ALARM3                       ,
		CMD_RW_ALARM4                       ,
		CMD_RW_ALARM5                       ,
		CMD_RW_ALARM6                       ,
		CMD_RW_ALARM7                       ,
		CMD_RW_ALARM8                       ,
		CMD_RW_ALARM9                       ,
		CMD_RU_ALARM_READ                   ,	//READ_USER(ALARM0 ~ ALARM9)
		CMD_SYS_PLC_CONN                    ,	//CHAMB UI <-> CHMB COMM <-> PLC
		CMD_WB_BUZZER_ONOFF                 ,	//2023.04.13 - KWMOON
		CMD_WB_LAMP_MASK_ONOFF              ,	//2023.05.24 - KWMOON
		CMD_WB_RED_LAMP_ON                  ,	//2023.05.24 - KWMOON
		CMD_WB_YELLOW_LAMP_ON               ,	//2023.05.24 - KWMOON
		CMD_WB_GREEN_LAMP_ON                ,	//2023.05.24 - KWMOON
		CMD_WB_RED_LAMP_BLINK               ,	//2023.05.24 - KWMOON
		CMD_WB_YELLOW_LAMP_BLINK            ,	//2023.05.24 - KWMOON
		CMD_WB_GREEN_LAMP_BLINK             ,	//2023.05.24 - KWMOON
		CMD_WB_ALL_LAMP_OFF                 ,	//2023.05.24 - KWMOON

		CMD_RB_RACK_NOP_TRIGGER             ,	//2023.09.07 - KWMOON
		CMD_RB_FINGER_UP_STAT_RACKIN        ,	//2023.09.07 - KWMOON
		CMD_RB_DOOR_IGNORE_RACKINOUT        ,	//2023.09.07 - KWMOON
		CMD_WB_FINGER_UP_STAT_RACKIN_ON     ,	//2023.09.07 - KWMOON
		CMD_WB_FINGER_UP_STAT_RACKIN_OFF    ,	//2023.09.07 - KWMOON
		CMD_WB_DOOR_IGNORE_RACK_INOUT_ON    ,	//2023.09.07 - KWMOON
		CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF   ,	//2023.09.07 - KWMOON

		CMD_WB_SYSTEM_DC_ONOFF              ,	//2023.09.19 - KWMOON

		CMD_RW_COMP1_PRESSURE_HIGH_VALUE    ,	//2024.08.19 - KWMOON
		CMD_RW_COMP1_PRESSURE_LOW_VALUE     ,	//2024.08.19 - KWMOON
		CMD_RW_COMP2_PRESSURE_HIGH_VALUE    ,	//2024.08.19 - KWMOON
		CMD_RW_COMP2_PRESSURE_LOW_VALUE     ,	//2024.08.19 - KWMOON
		CMD_RW_COMP1_OUT_TEMP_VALUE         ,	//2024.08.19 - KWMOON
		CMD_RW_COMP1_IN_TEMP_VALUE          ,	//2024.08.19 - KWMOON
		CMD_RW_COMP2_OUT_TEMP_VALUE         ,	//2024.08.19 - KWMOON
		CMD_RW_COMP2_IN_TEMP_VALUE          ,	//2024.08.19 - KWMOON
	};

	enum DEV_STATUS
	{
		DEV_PROGRAM_START_IN  ,
		DEV_PROGRAM_START_OUT ,
		DEV_INIT_TEST_IN      ,
		DEV_INIT_TEST_OUT     ,
		DEV_CYCLE_IN          ,
		DEV_STEP_IN           ,
		DEV_STEP_OUT          ,
		DEV_CYCLE_OUT         ,
		DEV_TEST_STOP         ,
	};

	enum PGM_CHMB_REQ_ACK
	{
		PGM_CHMB_ACK    = 0x01,
		PGM_CHMB_TRUE   = 0x02,
		PGM_CHMB_NACK   = 0x11,
		PGM_CHMB_FALSE  = 0x12,
        PGM_CHMB_NOUSE  = 0x13,
		PGM_CHMB_FAULT  = 0x80,
	};
}

namespace IPC_INFO
{
	const char* const IPC_TESTMGR_SOCK  = "TESTMGR.sock";
	const char* const IPC_CHMBCOMM_SOCK = "CHMBCOMM.sock";
	const char* const IPC_SMONCOMM_SOCK = "SMONCOMM.sock";
	const char* const IPC_PPSCOMM_SOCK  = "PPSCOMM.sock";
	const char* const IPC_LOADER_SOCK   = "LOADER.sock";
	const char* const IPC_CHMBUI_SOCK   = "CHMBUI.sock";
	const char* const IPC_TMS_SOCK      = "TMS.sock";
}

namespace SHM_KEY
{
	const char* const SHM_MAIN_CHMB_KEY = "MAIN_CHMB.key";
	const char* const SHM_CPUMON0       = "CPUMON0.key";
	const char* const SHM_CPUMON1       = "CPUMON1.key";
	const char* const SHM_PFM_MON_KEY   = "PFM_MON.key";
	const char* const SHM_HSMS_KEY      = "HSMS.key";
}

namespace TCP_PORT
{
	enum SYSTEM
	{
		CPU_LOADER_MGR_PORT      = 6200 ,	//atloader - mgr
		CPU_LOADER_TOOL_PORT            ,	//atloader - diag/cal rfu
		EWS_TESTMGR_PGM_CTRL_PORT       ,   //atmgr - start/stop
		EWS_TESTMGR_PGM_DATA_PORT       ,   //atmgr - pgm data
		EWS_MAINUI_DIAG_CTRL_PORT       ,	//atdiag/cal - ctrl port
		EWS_MAINUI_DIAG_DATA_PORT       ,	//atdiag/cal - data port
		EWS_TESTMGR_PGM_LOG_PORT        ,   //2023.08.13 - KWMOON
		EWS_MAINUI_DIAG_LOG_PORT        ,	//2023.08.13 - KWMOON
		CPU_SYSMON_PORT                 ,
		PFM_PORT						,	//atpfmd - atsysmonui
	};

	enum PLC
	{
		PORT_PLC_CHAMBER     = 12289,   //CHAMBER: PLC SERVER
	};
};

#endif // DEVICEIF_HPP
