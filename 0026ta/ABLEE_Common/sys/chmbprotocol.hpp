#ifndef CHMBPROTOCOL_HPP
#define CHMBPROTOCOL_HPP

#define WATCHDOG_DEFAULT_PERIOD				( 8)
#define CHMB_STATUS_READ_PERIOD				( 1)
#define TIMER_PLC_CON_PERIOD				( 3)
#define DEFAULT_AMBIENT_SV					(25)
#define DEFAULT_SV_GAP						(0.5)
#define DEFAULT_SOAK_TEMP_GAP				(10.0)
#define DEFAULT_SOAK_RETRY_COUNT			(30)

#define YKG_RECV_PACKET_TAIL_SIZE			( 2)
#define YKG_REQUEST_BYTE_VALUE				"0"
#define YKG_RESPONSE_BYTE_VALUE				"1"

#define MAX_RETRY_CONN_CNT					( 5)		//5
#define MAX_RETRY_COMM_CNT					(10)	//2022.03.11 - KWMOON( 5 -> 10 )
#define MAX_TEMP_PROFILE					(128)

#define PLC_RETRY_CONN_CNT					(10)
#define PLC_RETRY_COMM_CNT					( 5)

#define CR									"\r"
#define LF									"\n"

#define DEFAULT_ENTER_SYNC_TIME				(24*60)	//2023.08.14 - kwmoon( 60->120 min) --> //2024.07.12 - KWMOON(24HOUR WAIT SYNC)
#define DEFAULT_SETSV_SYNC_TIME				(30)	//2024.07.12 - KWMOON(30 MIN)
#define DEFAULT_RUN_SYNC_TIME				(30)	//2024.07.12 - KWMOON(30 MIN)

#define DEFAULT_CHMB_TEMP_LIMIT_LOW		( -40.0)
#define DEFAULT_CHMB_TEMP_LIMIT_HIGH	( 140.0)

enum PLC_ADDRESS_MAP
{
	//BIT
	ADDR_PLC_W_TEMP_RUN       =    0x0,
	ADDR_PLC_W_TEMP_STOP              ,
	ADDR_PLC_W_AMBIENT_RUN            ,
	ADDR_PLC_W_AMBIENT_AUTO_RUN       ,

	ADDR_PLC_W_ALARM_RESET            ,
	ADDR_PLC_W_MASK_ON_OFF            ,

	ADDR_PLC_W_DOOR_OPEN_RUN          ,
	ADDR_PLC_W_DOOR_CLOSE_RUN         ,
	ADDR_PLC_W_RACK_IN_RUN            ,
	ADDR_PLC_W_RACK_OUT_RUN           ,
	ADDR_PLC_W_RACK_NOP               ,

	ADDR_PLC_W_WATCHDOG_TIME_SET      ,
	ADDR_PLC_W_WATCHDOG_TOGGLE        ,
	ADDR_PLC_W_WATCHDOG_ENABLE        ,
	ADDR_PLC_W_SV_SET                 ,
	ADDR_PLC_W_AMBIENT_SV_SET         ,

	ADDR_PLC_R_WORD_STATUS1           ,
	ADDR_PLC_R_MASK_OFF               ,
	ADDR_PLC_R_MASK_ON                ,
	ADDR_PLC_R_WATCHDOG_ENABLE        ,
	ADDR_PLC_R_WATCHDOG_DISABLE       ,
	ADDR_PLC_R_DOOR_OPEN              ,
	ADDR_PLC_R_DOOR_CLOSE             ,
	ADDR_PLC_R_UPPER_RACK_DETECT      ,
	ADDR_PLC_R_BOTTOM_RACK_DETECT     ,
	ADDR_PLC_R_RACK_IN_COMPLETE       ,
	ADDR_PLC_R_RACK_OUT_COMPLETE      ,
	ADDR_PLC_R_EQ_READY               ,
	ADDR_PLC_R_EQ_ABORT               ,
	ADDR_PLC_R_AVG_COMPLETE           ,

	ADDR_PLC_R_WORD_STATUS2           ,
	ADDR_PLC_R_TEMP_RUN               ,
	ADDR_PLC_R_TEMP_STOP              ,
	ADDR_PLC_R_AMBIENT                ,
	ADDR_PLC_R_SYSTEM_DC_POWER_ON     ,

	//WORD
	ADDR_PLC_R_VERSION                ,
	ADDR_PLC_R_PV                     ,
	ADDR_PLC_R_SV                     ,
	ADDR_PLC_R_PCW_TEMPERATURE        ,
	ADDR_PLC_R_WPS_PRESSURE           ,
	ADDR_PLC_R_PCW_FLOW               ,

	//WORD
	ADDR_PLC_R_ALARM_D150             ,
	ADDR_PLC_R_ALARM_D151             ,
	ADDR_PLC_R_ALARM_D152             ,
	ADDR_PLC_R_ALARM_D153             ,
	ADDR_PLC_R_ALARM_D154             ,
	ADDR_PLC_R_ALARM_D155             ,
	ADDR_PLC_R_ALARM_D156             ,
	ADDR_PLC_R_ALARM_D157             ,
	ADDR_PLC_R_ALARM_D158             ,
	ADDR_PLC_R_ALARM_D159             ,

	ADDR_PLC_W_BUZZER_ON_OFF          ,		//2023.04.13 - KWMOON
	ADDR_PLC_W_LAMP_LOCAL_REMOTE      ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_RED_LAMP_ON            ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_YELLOW_LAMP_ON         ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_GREEN_LAMP_ON          ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_RED_LAMP_BLINK         ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_YELLOW_LAMP_BLINK      ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_GREEN_LAMP_BLINK       ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_W_ALL_LAMP_OFF           ,		//2023.05.24 - KWMOON(TOWER RAMP)
	ADDR_PLC_R_RACK_NOP               ,		//2023.09.07 - KWMOON(RACK OPR LOCAL/REMOTE)
	ADDR_PLC_W_FINGER_RACKIN_ON       ,		//2023.09.07 - KWMOON(FINGER UP STAT RACKIN ON TRIG)
	ADDR_PLC_W_FINGER_RACKIN_OFF      ,		//2023.09.07 - KWMOON(FINGER UP STAT RACKIN OFF TRIG)
	ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_ON   ,		//2023.09.07 - KWMOON(DOOR IGNORE RACK IN/OUT ON TRIG)
	ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_OFF  ,		//2023.09.07 - KWMOON(DOOR IGNORE RACK IN/OUT OFF TRIG)
	ADDR_PLC_W_SYSTEM_DC_ONOFF            ,		//2023.09.18 - KWMOON
	ADDR_PLC_R_COMP1_PRESSURE_HIGH_VALUE    ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP1_PRESSURE_LOW_VALUE     ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP2_PRESSURE_HIGH_VALUE    ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP2_PRESSURE_LOW_VALUE     ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP1_OUT_TEMP_VALUE         ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP1_IN_TEMP_VALUE          ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP2_OUT_TEMP_VALUE         ,	//2024.08.19 - KWMOON
	ADDR_PLC_R_COMP2_IN_TEMP_VALUE          ,	//2024.08.19 - KWMOON
	ADDR_PLC_UNDEF     = 0x9999       ,
};

enum PLC_ON_OFFLINE
{
	PLC_OFFLINE   = -1  ,
	PLC_ONLINE    =  1  ,
};

enum PLC_COMM_RESULT_CODE
{
	PLC_COMM_SOCK_ERROR  = -1,
	PLC_COMM_CONN_ERROR      ,
	PLC_COMM_SEND_ERROR      ,
	PLC_COMM_RECV_ERROR      ,
	PLC_COMM_RESPONSE_ERROR  ,
	PLC_COMM_CMD_ERROR       ,
	PLC_COMM_SUCCESS         ,
};

enum PLC_RESPONSE_TYPE
{
	PLC_RESPONSE_DATA   = 0x10,		//DATA 전송
	PLC_RESPONSE_PF           ,		//RESULT 전송
};

enum PLC_CHAMBER_STATUS
{
	PLC_STATUS_RUN  = 0,
	PLC_STATUS_STOP    ,
	PLC_STATUS_AMBIENT ,
	PLC_STATUS_UNKNOWN ,
};

enum CHMB_ALARM_LEVEL
{
	CHMB_WARN_ALARM  = 0x01 ,
	CHMB_HEAVY_ALRM         ,
};

enum EN_MASK_ONOFF
{
	CHMB_MASK_ON     = 0x0,
	CHMB_MASK_OFF    = 0x1,
	CHMB_MASK_TOGGLE = 0x1,
};

enum EN_RACK_NOP_OPR
{
	RACK_NOP_CHAMBER = 0x0,
	RACK_NOP_TESTER       ,
};

enum EN_WATCHDOG_EN_DIS
{
	WATCHDOG_DISABLE  = 0x0,
	WATCHDOG_ENABLE        ,
};

enum EN_BUZZER_ONOFF
{
	CHMB_BUZZER_ON    = 0x0,
	CHMB_BUZZER_OFF        ,
};

enum EN_TOWERLAMP_ONOFF
{
	CHMB_TOWERLAMP_OFF    = 0x0,
	CHMB_TOWERLAMP_ON     = 0x1,
	CHMB_TOWERLAMP_BLINK  = 0x1,
};

enum EN_CHMB_RUN_STAT
{
	EN_CHMB_RUN,
	EN_CHMB_STOP,
	EN_CHMB_AMB,
	EN_CHMB_RUN_MAX,
};

enum EN_CHMB_ONOFF_TRIGGER
{
	EN_CHMB_OFF_TRIGGER  = 0x0,
	EN_CHMB_ON_TRIGGER   = 0x1,
};

//PLC ADDRESS STATUS1 - D100
enum EN_WORD_STATUS1
{
	CHMB_STAT1_MASK_OFF           ,
	CHMB_STAT1_MASK_ON            ,
	CHMB_STAT1_WATCHDOG_ENAB      ,
	CHMB_STAT1_WATCHDOG_DISB      ,
	CHMB_STAT1_DOOR_OPEN          ,
	CHMB_STAT1_DOOR_CLOSE         ,
	CHMB_STAT1_UPPER_RACK_DETECT  ,
	CHMB_STAT1_BOTTOM_RACK_DETECT ,
	CHMB_STAT1_RACK_IN_COMPLETE   ,
	CHMB_STAT1_RACK_OUT_COMPLETE  ,
	CHMB_STAT1_EQ_READY_STATUS    ,
	CHMB_STAT1_EQ_ABORT_STATUS    ,
	CHMB_STAT1_AVG_JOB_COMPLETE   ,	
	MAX_WORD_STATUS1              ,
};


//PLC ADDRESS STATUS2 - D101
enum EN_WORD_STATUS2
{
	CHMB_STAT2_RUN                  ,	//BTN_RUN_STAT,
	CHMB_STAT2_STOP                 ,	//BTN_STOP_STAT,
	CHMB_STAT2_AMB                  ,	//BTN_AMB_STAT,
	CHMB_STAT2_DC_PWR               ,	//STAT2_SYSTEM_DC_POWERON,
	CHMB_STAT2_TOWERLAMP_LOCAL      ,
	CHMB_STAT2_TOWERLAMP_REMOTE     ,
	CHMB_STAT2_DUMMY                ,	//2023.09.07 - KWMOON
	CHMB_STAT2_FINGER_UP_STAT       ,	//2023.09.07 - KWMOON
	CHMB_STAT2_DOOR_IGNORE_RACK_STAT,	//2023.09.07 - KWMOON
	MAX_WORD_STATUS2 ,
};

//2023.09.08 - KWMOON
enum EN_RACK_OPR_STAT
{
	EN_RACK_OPR_UNKNOWN             ,
	EN_RACK_OPR_LOCAL               ,
	EN_RACK_OPR_REMOTE              ,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// YOKOGAWA PLC DEFINES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum YKG_CMD_LIST
{
	CMD_BRD	 = 0x0,
	CMD_BWR	      ,
	CMD_BRR	      ,
	CMD_BRW	      ,
	CMD_WRD	      ,
	CMD_WWR	      ,
	CMD_WRR	      ,
	CMD_WRW	      ,
	CMD_SWR	      ,
	CMD_SWW	      ,
	CMD_PRI	      ,
};

enum PLC_CMD_TYPE
{
	TYPE_BIT_ACCESS   = 0x0,
	TYPE_WORD_ACCESS       ,
	TYPE_MODULE_ACCESS     ,
	TYPE_PROG_ACCESS       ,
};

//PLC EXIT & ERROR CODE
enum PLC_EXIT_ERROR_CODE
{
	EXIT_CODE_OK      =0x0,		//EXIT CODE : NORMAL EXIT.
	EXIT_CODE_ER01        ,		//EXIT CODE : CPU NUMBER ERROR
	EXIT_CODE_ER02        ,		//EXIT CODE : COMMAND ERROR
	EXIT_CODE_ER03        ,		//EXIT CODE : DEVICE SPECITICATION ERROR
	EXIT_CODE_ER04        ,		//EXIT CODE : VALUE OUTSIDE THE SETTING RANGE
	EXIT_CODE_ER05        ,		//EXIT CODE : DATA COUNT OUT OF RANGE
	EXIT_CODE_ER06        ,		//EXIT CODE : MONITOR ERROR
	EXIT_CODE_ER08        ,		//EXIT CODE : PARAMETER ERROR
	EXIT_CODE_ER51        ,		//EXIT CODE : SEQUENCE CPU ERROR
	EXIT_CODE_ER52        ,		//EXIT CODE : SEQUENCE CPU PROCESSING ERROR
	ERROR_CODE_ER1        ,		//DETAIL ERROR CODE START ASCII '1' : SELF-DIAGNOSTIC ERROR
	ERROR_CODE_ER2        ,		//DETAIL ERROR CODE START ASCII '2' : PROGRAM ERROR
	ERROR_CODE_ER3        ,		//DETAIL ERROR CODE START ASCII '3' : INTER-CPU COMMUNICATION ERROR
	ERROR_CODE_ER8        ,		//DETAIL ERROR CODE START ASCII '8' : DEVICE ACCESS ERROR
	ERROR_CODE_ER9        ,		//DETAIL ERROR CODE START ASCII '9' : COMMAND ERROR
	ERROR_CODE_ERA        ,		//DETAIL ERROR CODE START ASCII 'A' : PARAMETER ERROR
	ERROR_CODE_ERB        ,		//DETAIL ERROR CODE START ASCII 'B' : OPERATION MODE ERROR
	ERROR_CODE_ERC        ,		//DETAIL ERROR CODE START ASCII 'C' : PARAMETER ERROR
	ERROR_CODE_ERF        ,		//DETAIL ERROR CODE START ASCII 'F' : SYSTEM ERROR
	ERROR_CODE_USER       ,		//USER ERROR EXIT CODE
};


#endif // CHMBPROTOCOL_HPP
