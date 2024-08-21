#ifndef COMMON_HPP
#define COMMON_HPP

#include <QMetaType>
#include <iostream>
#include <QObject>
#include <QString>

#define SAFE_DELETE(p)          if( p ) { delete p; p=nullptr; }
#define MAX_LOG_LINE_NUM        (4096)
#define CR						"\r"
#define LF						"\n"
#define CRLF					"\n"
#define PROGRAM_NAME			"AT5008 YOKOGAWA PLC SIMULATOR"
#define PROGRAM_VERSION			"v1.0.0"

//////////////////////////////////////////////////////////////////////
///  DISPLAY COLOR DEFINE
//////////////////////////////////////////////////////////////////////
#define PASS_QCOLOR             QColor(  0,  0,  0)
#define FAIL_QCOLOR             QColor(255,  0,  0)
#define ODD_ROW_QCOLOR          QColor(245,245,245)
#define EVEN_ROW_QCOLOR         QColor(255,255,255)
#define TP_ODD_ROW_QCOLOR       QColor(236,251,252)
#define TP_EVEN_ROW_QCOLOR      QColor(255,235,217)

#define COMM_TITLE_STYLE        "background-color:#6080c0; font: 13pt;font-weight:bold;"
#define COMM_PLC_OFF_STYLE      "background-color:rgb(236,236,236); "
#define COMM_PLC_ON_STYLE       "background-color:rgb(0,255,0); "
#define COMM_ALARM_STYLE        "background-color: #DC0000; font-size: 8pt;"
#define COMM_WARNING_STYLE      "background-color: #F1C40F; font-size: 8pt;"
#define COMM_NORMAL_STYLE       "background-color: #00DC00; font-size: 8pt;"


//////////////////////////////////////////////////////////////////////
///  INI CONFIG
//////////////////////////////////////////////////////////////////////
#define INI_ERROR_VAL           "UNDEFINED"
#define CONFIG_DIR              "Config"
#define CONFIG_INI_NAME  		"PLC.ini"
#define CONFIG_INI_FILE         QString("%1/%2/%3").arg(QDir::current().currentPath()).arg(CONFIG_DIR).arg(CONFIG_INI_NAME)

#define INI_KEY_TYPE            "TYPE"
#define INI_KEY_RW              "RW"
#define INI_KEY_VALUE           "VALUE"
#define INI_KEY_DESC            "DESCRIPTION"
#define INI_KEY_ADDR            "ADDR"

#define INI_VAL_TYPE_BIT        "BIT"
#define INI_VAL_TYPE_WORD       "WORD"

enum EN_PLC_DEST
{
    EN_DEST_COMM       ,
	EN_DEST_ALARM      ,
};

enum EN_PLC
{
    EN_PLC_COMMON = 0,
    EN_PLC_CHANNEL1  ,  //CH 증가할 수 있어 1로 인덱스 잡음.
    EN_PLC_MAX       ,
};

enum EN_TCP_CONN
{
    EN_TCP_LISTEN_OK   = 0x0,
    EN_TCP_LISTEN_NG        ,
    EN_TCP_ACCEPT_NG        ,
    EN_TCP_LISTEN_CLOSE     ,
    EN_TCP_CONNECT_NG       ,
    EN_TCP_SEND_NG          ,
    EN_TCP_RECV_NG          ,
    EN_TCP_SOCKET_CLOSE     ,
};

enum EN_PLC_EXIT_ERROR_CODE
{
	PLC_EXIT_CODE_OK      =0x0,		//EXIT CODE : NORMAL EXIT.
	PLC_EXIT_CODE_ER01        ,		//EXIT CODE : CPU NUMBER ERROR
	PLC_EXIT_CODE_ER02        ,		//EXIT CODE : COMMAND ERROR
	PLC_EXIT_CODE_ER03        ,		//EXIT CODE : DEVICE SPECITICATION ERROR
	PLC_EXIT_CODE_ER04        ,		//EXIT CODE : VALUE OUTSIDE THE SETTING RANGE
	PLC_EXIT_CODE_ER05        ,		//EXIT CODE : DATA COUNT OUT OF RANGE
	PLC_EXIT_CODE_ER06        ,		//EXIT CODE : MONITOR ERROR
	PLC_EXIT_CODE_ER08        ,		//EXIT CODE : PARAMETER ERROR
	PLC_EXIT_CODE_ER51        ,		//EXIT CODE : SEQUENCE CPU ERROR
	PLC_EXIT_CODE_ER52        ,		//EXIT CODE : SEQUENCE CPU PROCESSING ERROR
	PLC_ERROR_CODE_ER1        ,		//DETAIL ERROR CODE START ASCII '1' : SELF-DIAGNOSTIC ERROR
	PLC_ERROR_CODE_ER2        ,		//DETAIL ERROR CODE START ASCII '2' : PROGRAM ERROR
	PLC_ERROR_CODE_ER3        ,		//DETAIL ERROR CODE START ASCII '3' : INTER-CPU COMMUNICATION ERROR
	PLC_ERROR_CODE_ER8        ,		//DETAIL ERROR CODE START ASCII '8' : DEVICE ACCESS ERROR
	PLC_ERROR_CODE_ER9        ,		//DETAIL ERROR CODE START ASCII '9' : COMMAND ERROR
	PLC_ERROR_CODE_ERA        ,		//DETAIL ERROR CODE START ASCII 'A' : PARAMETER ERROR
	PLC_ERROR_CODE_ERB        ,		//DETAIL ERROR CODE START ASCII 'B' : OPERATION MODE ERROR
	PLC_ERROR_CODE_ERC        ,		//DETAIL ERROR CODE START ASCII 'C' : PARAMETER ERROR
	PLC_ERROR_CODE_ERF        ,		//DETAIL ERROR CODE START ASCII 'F' : SYSTEM ERROR
	PLC_ERROR_CODE_USER       ,		//USER ERROR EXIT CODE
};

enum EN_CHAMBER_STATUS
{
    EN_STATUS_RUN  = 0,
    EN_STATUS_STOP    ,
    EN_STATUS_AMBIENT ,
    EN_STATUS_DV      ,
    EN_STATUS_UNKNOWN ,
};

enum EN_ALARM_COUNT
{
    EN_COMMON_ALARM_COUNT   = 48,
    EN_CHANANEL_ALARM_COUNT = 64,
};

enum EN_ALARM_LEVEL
{
    EN_NO_WARN_ERROR    = 0x0,
    EN_WARNING_LEVEL         ,
    EN_HEAVY_LEVEL           ,
};

enum YOKOGAWA_PLC_CMD_TYPE
{
	YKG_TYPE_BIT_ACCESS   = 0x0,
	YKG_TYPE_WORD_ACCESS       ,
	YKG_TYPE_MODULE_ACCESS     ,
	YKG_TYPE_PROG_ACCESS       ,
};

enum EN_PLC_ADDRESS_MATCH_CMD
{
	EN_GET_COMMON_MASK_OFF       = 0x0,
	EN_GET_COMMON_MASK_ON             ,
	EN_GET_COMMON_WATCHDOG_EN         ,
	EN_GET_COMMON_WATCHDOG_DIS        ,
	EN_GET_COMMON_DOOR_OPEN           ,
	EN_GET_COMMON_DOOR_CLOSE          ,
	EN_GET_COMMON_UPPER_RACK_DETECT   ,
	EN_GET_COMMON_BOTTOM_RACK_DETECT  ,
	EN_GET_COMMON_RACKIN_COMPLETE     ,
	EN_GET_COMMON_RACKOUT_COMPLETE    ,
	EN_GET_COMMON_EQ_READY_STATUS     ,
	EN_GET_COMMON_EQ_ABORT_STATUS     ,
	EN_GET_COMMON_AGV_COMPLETE        ,
	EN_GET_COMMON_STATUS_D100         ,

	EN_GET_COMMON_TEMP_RUN_MODE       ,
	EN_GET_COMMON_TEMP_STOP_MODE      ,
	EN_GET_COMMON_TEMP_AMB_MODE       ,
	EN_GET_COMMON_SYSTEM_DC_POWER_ON  ,		//2022.04.05 - KWMOON
	EN_GET_COMMON_TOWER_LAMP_LOCAL    ,
	EN_GET_COMMON_TOWER_LAMP_REMOTE   ,
	EN_GET_COMMON_FINGERUP_RACK       ,
	EN_GET_COMMON_DOOR_IGNORE_RACK    ,
	EN_GET_COMMON_STATUS_D101         ,

	EN_GET_COMMON_PLC_VERSION         ,		//2022.04.05 - KWMOON
	EN_GET_COMMON_PV                  ,
	EN_GET_COMMON_SV                  ,
	EN_GET_COMMON_PCW_TEMPERATURE     ,
	EN_GET_WPS_PRESSURE               ,
	EN_GET_PCW_FLOW_READ              ,

	EN_GET_COMMON_ALARM_D150          ,
	EN_GET_COMMON_ALARM_D151          ,
	EN_GET_COMMON_ALARM_D152          ,
	EN_GET_COMMON_ALARM_D153          ,
	EN_GET_COMMON_ALARM_D154          ,
	EN_GET_COMMON_ALARM_D155          ,
	EN_GET_COMMON_ALARM_D156          ,
	EN_GET_COMMON_ALARM_D157          ,
	EN_GET_COMMON_ALARM_D158          ,
	EN_GET_COMMON_ALARM_D159          ,

	EN_SET_TEMP_RUN_TRIGGER           ,
	EN_SET_TEMP_STOP_TRIGGER          ,
	EN_SET_AMB_TRIGGER                ,
	EN_SET_AMB_AUTO_TRIGGER           ,
	EN_SET_ALARM_RESET                ,
	EN_SET_MASK_ON_OFF                ,
	EN_SET_DOOR_OPEN_TRIGGER          ,
	EN_SET_DOOR_CLOSE_TRIGGER         ,
	EN_SET_RACK_IN_TRIGGER            ,
	EN_SET_RACK_OUT_TRIGGER           ,
	EN_SET_RACK_NOP_TRIGGER           ,
	EN_GET_RACK_NOP_TRIGGER           ,
	EN_SET_BUZZER_OFF                 ,

	EN_SET_TOWER_LAMP_REMOTE_LOCAL    ,
	EN_SET_FINGER_UP_RACK_IN_ON       ,
	EN_SET_FINGER_UP_RACK_IN_OFF      ,
	EN_SET_DOOR_IGN_RACK_INOUT_ON     ,
	EN_SET_DOOR_IGN_RACK_INOUT_OFF    ,
	EN_SET_DC_POWER_ONOFF             ,

	EN_SET_WATCHDOG_PERIOD            ,
	EN_SET_WATCHDOG_TOGGLE            ,
	EN_SET_WATCHDOG_EN_DIS            ,
	EN_SET_SV                         ,
	EN_SET_AMB_SV                     ,

	//2024.08.20 - KWMOON
	EN_GET_COMP1_PRESSURE_HIGH        ,
	EN_GET_COMP1_PRESSURE_LOW         ,
	EN_GET_COMP1_OUT_TEMP             ,
	EN_GET_COMP1_IN_TEMP              ,
	EN_GET_COMP2_PRESSURE_HIGH        ,
	EN_GET_COMP2_PRESSURE_LOW         ,
	EN_GET_COMP2_OUT_TEMP             ,
	EN_GET_COMP2_IN_TEMP              ,


};

struct _stPlcAdress
{
    QString key;
    QString type;
    QString rw;
    QString value;
    QString description;
    int addr;
};

struct _stPlcAddrMap
{
    EN_PLC_DEST dest;
    YOKOGAWA_PLC_CMD_TYPE type;
    EN_PLC_ADDRESS_MATCH_CMD cmd;
    QString description;
};

#endif // COMMON_HPP
