#ifndef SYSLOG_HPP
#define SYSLOG_HPP

#define CR						"\r"
#define LF						"\n"

#ifdef _WIN32_
#define CRLF					"\r\n"
#else
#define CRLF					"\n"
#endif

#define ATOS_DIR_ENV            "ATOS_DIR"		//Env
#define ATRUN_DIR_ENV           "ATRUN_DIR"		//Env
#define ATDATA_DIR_ENV          "ATDATA_DIR"	//Env

#define LOG_SYSTEM_DIR			"system"
#define LOG_CHAMBER_DIR			"chamber"
#define LOG_SLOT_DIR			"slot"
#define LOG_FILE_EXTENSION      ".log"

#define DIR_ATOS_PATH			"/opt/at6200"				//ATOS_DIR ENV
#define DIR_ATRUN_PATH			"/home/ablee/atsystem"		//ATOS_DIR ENV
#define DIR_ATDATA_PATH			"/nvme"						//ATDATA_DIR
#define DIR_ATDATA_NAME			"testdata"					//ATDATA_DIR/testdata

#define DIR_BIN_PATH            "bin"
#define DIR_CONFIG_PATH         "config"
#define DIR_DATA_PATH           "data"
#define DIR_DOWN_PATH           "download"
#define DIR_PROGRAM_PATH        "program"
#define DIR_REPORT_PATH         "atdb"			//2023.05.01
#define DIR_INC_PATH            "include"
#define DIR_LIB_PATH            "lib"
#define DIR_LOG_PATH            "log"
#define DIR_RUN_PATH            "run"
#define DIR_SCRIPT_PATH         "script"
#define DIR_TOOL_PATH           "tool"
#define DIR_UDS_PATH            ".uds"
#define DIR_UTIL_PATH           "util"
#define DIR_CFG_PATH		    "cfg"
#define DIR_AUTO_DATA_PATH		"auto"			//2024.06.10 hsbae
#define DIR_ENGR_DATA_PATH		"engr"			//2024.06.10 hsbae
#define DIR_PROD_DATA_PATH		"prod"			//2024.08.10 fatric
#define DIR_SAVEMAP_PATH		"SaveMap"		//2024.08.10 fatric

/////////////////////////////////////////////////////////////////
////CHAMBER temperature.dat
/// 2023.06.15 - KWMOON
/////////////////////////////////////////////////////////////////
#define DIR_TEMPERATURE			"chamber"				//nvme/testdata/chamber/
#define DAT_TEMPERATURE			"temperature.dat"		//nvme/testdata/chamber/temperature.dat

/////////////////////////////////////////////////////////////////
////PFM Power Fault Monitoring Meas File
/// 2023.06.15 - KWMOON
/////////////////////////////////////////////////////////////////
#define DIR_PFM					"power"					//nvme/testdata/power/
#define DAT_PFM					"pfm.dat"				//nvme/testdata/power/pfm.dat

/////////////////////////////////////////////////////////////////
////SETUP START/END TIME
/// 2024.08.20 - HSBAE
/////////////////////////////////////////////////////////////////
#define DAT_SETUP_TIME			"setup_time.dat"


/////////////////////////////////////////////////////
/// CONSOLE LOG COLOR CODE
/////////////////////////////////////////////////////
#define COLOR_NC                "\033[0m"        //No Color"
#define COLOR_BLACK             "\033[0;30m"
#define COLOR_GRAY              "\033[1;30m"
#define COLOR_RED               "\033[0;31m"
#define COLOR_LIGHT_RED         "\033[1;31m"
#define COLOR_GREEN             "\033[0;32m"
#define COLOR_LIGHT_GREEN       "\033[1;32m"
#define COLOR_BROWN             "\033[0;33m"
#define COLOR_YELLOW            "\033[1;33m"
#define COLOR_BLUE              "\033[0;34m"
#define COLOR_LIGHT_BLUE        "\033[1;34m"
#define COLOR_PURPLE            "\033[0;35m"
#define COLOR_LIGHT_PURPLE      "\033[1;35m"
#define COLOR_CYAN              "\033[0;36m"
#define COLOR_LIGHT_CYAN        "\033[1;36m"
#define COLOR_LIGHT_GRAY        "\033[0;37m"
#define COLOR_WHITE             "\033[1;37m"


//tools
enum LOG_TYPE
{
	_NORMAL_LOG    = 0x0,
	_ERROR_LOG          ,
	_FAULT_LOG          ,
};

//lib
enum DEV_LOG_TYPE
{
	_NORMAL       = 0x0,
	_HW          ,
	_PPS         ,
	_PG          ,
	_DBM         ,
	_UBM         ,
	_DFM         ,
	_TG          ,
	_CYP         ,
	_MODULE      ,
	_BIB         ,
	_SCAN        ,
	_MASK        ,
	_TEMP        ,
	_USER        ,
};

#endif
