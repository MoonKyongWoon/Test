//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIG INI & DATA FILE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SYSINI_HPP
#define SYSINI_HPP

#define CONFIG_DIR						"config"
#define SITE_INI						"atsite.ini"
#define SYSTEM_INI						"atsystem.ini"
#define FTP_INI							"atftp.ini"
#define SERVER_INI						"atserver.ini"
#define TMS_INI							"attms.ini"
#define CHAMBER_INI						"atchamber.ini"
#define PID_INI							"atpid.ini"					//2023.07.10 - kwmoon
#define PFM_SETVS_INI_1					"pfm_setvs_1.ini"			//2023.07.25 - kwmoon
#define PFM_SETVS_INI_2					"pfm_setvs_2.ini"			//2023.07.25 - kwmoon
#define PFM_FAULT_INI					"pfm_fault.ini"				//2023.07.25 - kwmoon

////////////////////////////////////////////////////////////////////////////
/// secsgem based ini files
////////////////////////////////////////////////////////////////////////////
#define VID_VALUE_INI					"vid_value.ini"				//2024.06.10 - hsbae
#define RCMD_INI						"rcmd.ini"					//2024.06.03 - hsbae for test

/////////////////////////////////////////////////////////////////
////atsite.ini
/////////////////////////////////////////////////////////////////
#define SECTION_SITE_COMMON				"COMMON"
#define SECTION_SITE_SLOT				"SLOT"

#define KEY_SITE_MAX_SLOT				"MAX_SLOT"
#define KEY_SITE_CPU_PER_SLOT			"CPU_PER_SLOT"
#define KEY_SITE_PGM_PER_SLOT			"PGM_PER_SLOT"
#define KEY_SITE_SLOT_MASK				"MASK"
#define KEY_SITE_CPU1_IP				"CPU1_IP"
#define KEY_SITE_CPU2_IP				"CPU2_IP"

#define DEFAULT_SITE_MAX_SLOT			(16)
#define DEFAULT_SITE_CPU_PER_SLOT		( 1)
#define DEFAULT_SITE_PGM_PER_SLOT		( 2)
#define KEY_SITE_CPU_IP_START			"192.168.62"

/////////////////////////////////////////////////////////////////
////atchamber.ini
/////////////////////////////////////////////////////////////////
#define SECTION_CHMB_COMMON				"COMMON"
#define KEY_CHMB_IP						"PLC_IP"
#define KEY_CHMB_PORT					"PLC_PORT"
#define KEY_CHMB_WATCHDOG_ENABLE		"WATCHDOG_ENABLE"
#define KEY_CHMB_WATCHDOG_PERIOD		"WATCHDOG_PERIOD"
#define KEY_CHMB_AMB_SV         		"AMB_SV"
#define KEY_CHMB_TEMP_LIMIT_LOW			"CHAMBER_TEMP_LIMIT_LOW"
#define KEY_CHMB_TEMP_LIMIT_HIGH		"CHAMBER_TEMP_LIMIT_HIGH"
#define KEY_CHMB_SOAK_TIME      		"SOAK_TIME"
#define KEY_CHMB_KEEP_TIME      		"KEEP_TIME"
#define KEY_CHMB_USE					"CHAMBER_USE"

#define DEFAULT_CHMB_IP					"192.168.63.100"
#define DEFAULT_CHMB_PORT				(12289)
#define DEFAULT_CHMB_WATCHDOG_PERIOD	( 8)
#define DEFAULT_CHMB_AMB_SV             ( 40.0)
//#define DEFAULT_CHMB_TEMP_LIMIT_LOW		( -40.0)
//#define DEFAULT_CHMB_TEMP_LIMIT_HIGH	( 140.0)
#define DEFAULT_CHMB_TEMP_UNIFORMITY	(   3.0)
#define DEFAULT_CHMB_SOAK_TIME      	(  5)
#define DEFAULT_CHMB_KEEP_TIME      	( 60)		//2023.07.09 - KWMOON
#define DEFAULT_CHMB_USE			   	( 1)

#define DEFAULT_CHMB_TEMP_MARGIN		( 0.5)

#define DEFAULT_CHMB_AMB_WAIT_TIME		( 60)
#define DEFAULT_CHMB_AMB_SOAK_TIME		( 5)

#define DEFAULT_CHMB_DRY_SV             ( 80.0)
#define DEFAULT_CHMB_DRY_WAIT_TIME		( 60)
#define DEFAULT_CHMB_DRY_SOAK_TIME		( 10)

/////////////////////////////////////////////////////////////////
////.config ui geometry & windowsState
/////////////////////////////////////////////////////////////////
#define CONFIG_INI_KEY				"at6200"

/////////////////////////////////////////////////////////////////
////atftp.ini
/////////////////////////////////////////////////////////////////
#define SECTION_EWS_FTP				"EWS_FTP"
#define KEY_EWS_FTP_IP				"IP"
#define KEY_EWS_FTP_PORT			"PORT"
#define KEY_EWS_FTP_USER			"USER"
#define KEY_EWS_FTP_PASSWD			"PASSWD"
#define KEY_EWS_FTP_HOME			"/home/ablee"

#define DEFAULT_EWS_FTP_IP			"192.168.62.100"
#define DEFAULT_EWS_FTP_PORT		(21)
#define DEFAULT_EWS_FTP_USER		"ablee"
#define DEFAULT_EWS_FTP_PASSWD		"ablee"

#define SECTION_SITE_FTP			"SITE_FTP"
#define KEY_SITE_FTP_IP				"IP"
#define KEY_SITE_FTP_PORT			"PORT"
#define KEY_SITE_FTP_USER			"USER"
#define KEY_SITE_FTP_PASSWD			"PASSWD"
#define KEY_SITE_FTP_HOME			"/home/ablee"

/////////////////////////////////////////////////////////////////
////atsystem.ini
/////////////////////////////////////////////////////////////////
#define SECTION_SMPS_COMM           "SMPS_COMM"
#define KEY_SMPS_PORT_NAME          "PORT_NAME"
#define KEY_SMPS_BAUD_RATE          "BAUD_RATE"

#define SECTION_DEV_PGM             "DEV_PGM"
#define KEY_FILE_EXTENSION          "EXTENSION"
#define DEFAULT_DEVEXT              ".dev"
//#define KEY_LOAD_EXTENSIONS			"LOAD_EXTENSIONS"

#define SECTION_OPEN_TIME           "OPEN_TIME"
#define KEY_OPEN_TIME_ENABLE        "OPEN_TIME_ENABLE"
#define KEY_FIRST                   "FIRST"
#define KEY_SECOND                  "SECOND"
#define KEY_FAST_OPEN_TIME          "FAST_OPEN_TIME"

#define SECTION_FAIL_RATE           "FAIL_RATE"
#define KEY_FAIL_RATE_ENABLE        "FAIL_RATE_ENABLE"
#define KEY_BIN_ENABLE              "ENABLE"
#define KEY_BIN_FAIL_RATE           "FAIL_RATE"

#define SECTION_SLOT_OPTION         "SLOT_OPTION"
#define KEY_EMPTY_BIB_TEST			"EMPTY_BIB_TEST"

#define SECTION_BIB_SIZE			"BIB_SIZE"
#define KEY_BIB_ROWS				"BIB_ROWS"
#define KEY_BIB_COLS				"BIB_COLS"

#define DEFAULT_LOT_YIELD_SKIP_QTY	(184)

/////////////////////////////////////////////////////////////////
////atpid.ini
/////////////////////////////////////////////////////////////////
#define SECTION_ATSYS_PID          "ATSYS_PID"

/////////////////////////////////////////////////////////////////
////lastsetup.ini -- testdata 2023/08/08
/////////////////////////////////////////////////////////////////
#define SECTION_SETUP	           "SETUP"
#define KEY_SETUP_MODE		       "UI_MODE"
#define KEY_SETUP_LOT		       "LOTNO"
#define KEY_SETUP_PART		       "PARTNO"
#define KEY_SETUP_OPRID		       "OPRID"

/////////////////////////////////////////////////////////////////
////atserver.ini
/////////////////////////////////////////////////////////////////

#define SECTION_SERVER				"SERVER"
#define KEY_FLS_SERVER				"FLS_SERVER"
#define KEY_FLS_USER				"FLS_USER"
#define KEY_FLS_PASSWD				"FLS_PASS"
#define KEY_FLS_DIR					"FLS_DIR"
#define KEY_TDB_SERVER				"TDB_SERVER"
#define KEY_TDB_USER				"TDB_USER"
#define KEY_TDB_PASSWD				"TDB_PASS"
#define KEY_TDB_DIR					"TDB_DIR"
#define KEY_TDBCUM_SERVER			"TDBCUM_SERVER"
#define KEY_TDBCUM_USER				"TDBCUM_USER"
#define KEY_TDBCUM_PASSWD			"TDBCUM_PASS"
#define KEY_TDBCUM_DIR				"TDBCUM_DIR"
#define KEY_ITEMCUM_SERVER			"ITEMCUM_SERVER"
#define KEY_ITEMCUM_USER			"ITEMCUM_USER"
#define KEY_ITEMCUM_PASSWD			"ITEMCUM_PASS"
#define KEY_ITEMCUM_DIR				"ITEMCUM_DIR"
#define KEY_MRD_SERVER				"MRD_SERVER"
#define KEY_MRD_USER				"MRD_USER"
#define KEY_MRD_PASSWD				"MRD_PASS"
#define KEY_MRD_DIR					"MRD_DIR"
#define KEY_EQPR_SERVER				"EQPR_SERVER"
#define KEY_EQPR_USER				"EQPR_USER"
#define KEY_EQPR_PASSWD				"EQPR_PASS"
#define KEY_EQPR_DIR				"EQPR_DIR"
#define KEY_CHDR_SERVER				"CHDR_SERVER"
#define KEY_CHDR_USER				"CHDR_USER"
#define KEY_CHDR_PASSWD				"CHDR_PASS"
#define KEY_CHDR_DIR				"CHDR_DIR"
#define KEY_DATA_SERVER				"DATA_SERVER"
#define KEY_DATA_USER				"DATA_USER"
#define KEY_DATA_PASSWD				"DATA_PASS"
#define KEY_DATA_DIR				"DATA_DIR"

#define DEFAULT_SERVER				"10.192.53.236"
#define DEFAULT_FLS_USER			"testeqp"
#define DEFAULT_FLS_PASSWD			"testeqp"
#define DEFAULT_FLS_DIR				"./TDB"

#define DEFAULT_TDB_USER			"chipusr"
#define DEFAULT_TDB_PASSWD			"chipusr0"
#define DEFAULT_TDB_DIR				"./TDB"

#define DEFAULT_TDBCUM_USER			"chipusr"
#define DEFAULT_TDBCUM_PASSWD		"chipusr0"
#define DEFAULT_TDBCUM_DIR			"./TDBCUM"

#define DEFAULT_ITEMCUM_USER		"chipusr"
#define DEFAULT_ITEMCUM_PASSWD		"chipusr0"
#define DEFAULT_ITEMCUM_DIR			"./TDB"

#define DEFAULT_MRD_USER			"prbftp"
#define DEFAULT_MRD_PASSWD			"filemanager!"
#define DEFAULT_MRD_DIR				"./mduetc/RAW/TDBI_TEMP"

#define DEFAULT_EQPR_USER			"chipusr"
#define DEFAULT_EQPR_PASSWD			"chipusr0"
#define DEFAULT_EQPR_DIR			"./TEST"

#define DEFAULT_CHDR_USER			"chipusr"
#define DEFAULT_CHDR_PASSWD			"chipusr0"
#define DEFAULT_CHDR_DIR			"./TEST"

#define DEFAULT_DATA_USER			"testeqp"
#define DEFAULT_DATA_PASSWD			"testeqp"
#define DEFAULT_DATA_DIR			"./TDB"

/////////////////////////////////////////////////////////////////
////attms.ini
/////////////////////////////////////////////////////////////////
#define SECTION_SYSTEM				"SYSTEM"
#define KEY_TMS_EQUIPID				"EQUIP_ID"

#define SECTION_CEID_LIST			"CEID_LIST"
#define KEY_CEID_COUNT				"COUNT"

#define SECTION_COMM				"COMM"
#define KEY_TMS_PORT				"TCP_PORT"
#define KEY_TMS_DEVID				"DEVICE_ID"
#define KEY_T1_TIME					"T1"
#define KEY_T2_TIME					"T2"
#define KEY_T3_TIME					"T3"
#define KEY_T4_TIME					"T4"
#define KEY_T5_TIME					"T5"
#define KEY_T6_TIME					"T6"
#define KEY_T7_TIME					"T7"
#define KEY_T8_TIME					"T8"


#define KEY_TIMEOUT_MAX				(    5)
#define DEFAULT_TMS_EQUIPID			"CB4DA01-1"
#define DEFAULT_TMS_PORT			(62000)
#define DEFAULT_TMS_DEVID			(    0)
#define DEFAULT_TMS_T1_TIME			(  0.5)
#define DEFAULT_TMS_T2_TIME			(   10)
#define DEFAULT_TMS_T3_TIME			(   45)
#define DEFAULT_TMS_T4_TIME			(   45)
#define DEFAULT_TMS_T5_TIME			(   10)
#define DEFAULT_TMS_T6_TIME			(    5)
#define DEFAULT_TMS_T7_TIME			(   10)
#define DEFAULT_TMS_T8_TIME			(    5)

#define SECTION_VIDLIST				"VID_LIST"
#define SECTION_CEIDLIST			"CEID_LIST"
#define KEY_VAR_COUNT				"COUNT"

/////////////////////////////////////////////////////////////////
////UIMODE.ini
/////////////////////////////////////////////////////////////////
#define UIMODE_INI					"uimode.ini"			//2024.06.13 - kwmoon
#define SECTION_UIMODE				"UI_MODE"
#define KEY_UIMODE					"MODE"

/////////////////////////////////////////////////////////////////
////BIBID.ini
/////////////////////////////////////////////////////////////////
#define SECTION_BIBID				"BIBID"				//2024.06.17 - KWMOON
#define KEY_BIBID_ID				"ID"				//2024.06.17 - KWMOON
#define KEY_BIBID_CONN				"CONN"				//2024.06.17 - KWMOON
#define KEY_BIBID_PARITY			"PARITY_ERROR"		//2024.06.17 - KWMOON

/////////////////////////////////////////////////////////////////
////EVENT.ini
/////////////////////////////////////////////////////////////////
#define EVENT_INI					"event.ini"				//2024.06.28 - yseom
#define KEY_STATUS_ID				"STATUS_ID"
#define KEY_EVENT_ID				"EVENT_ID"				//2024.06.28 - yseom
#define KEY_LAST_CEID				"LAST_CEID"				//2024.06.28 - yseom
#define KEY_SLOT_WITH_BIB			"SLOT_WITH_BIB"			//2024.06.28 - yseom

/////////////////////////////////////////////////////////////////
////AUTO_DIAG
/////////////////////////////////////////////////////////////////
#define SECTION_AUTO_DIAG						"AUTO_DIAG"
#define SECTION_EQ_EFF_INFO						"EQ_EFF_INFO"
#define SECTION_TGET							"TGET"
#define SECTION_READY_TO_LOAD					"READY_TO_LOAD"
#define SECTION_READY_TO_UNLOAD					"READY_TO_UNLOAD"
#define SECTION_AUTO_BYPASS						"AUTO_BYPASS"
#define SECTION_REPLY_AUTO_BYPASS_EQP_STATE		"REPLY_AUTO_BYPASS_EQP_STATE"
#define SECTION_START_BI_CMD					"START_BI_CMD"
#define SECTION_HOLD_CHECK						"HOLD_CHECK"
#define SECTION_BOARD_TYPE						"BOARD_TYPE"
#define SECTION_BOARD_MAINT_INFO				"BOARD_MAINT_INFO"
#define SECTION_SLOT_MASK_INFO					"SLOT_MASK_INFO"
#define SECTION_BOARD_STATUS_INFO				"BOARD_STATUS_INFO"
#define SECTION_INTERLOCK_LIMIT					"INTERLOCK_LIMIT"
#define SECTION_BI_INITIAL_START				"BI_INITIAL_START"
#define SECTION_BI_INITIAL_END					"BI_INITIAL_END"
#define SECTION_BI_TEST_TIME					"BI_TEST_TIME"
#define SECTION_LOTCALL							"LOTCALL"
#define SECTION_TMVIN							"TMVIN"
#define SECTION_VMVOU							"VMVOU"
#define SECTION_TMVOU							"TMVOU"
#define SECTION_AUTO_DIAG						"AUTO_DIAG"
#define SECTION_BI_OPEN_LOT						"BI_OPEN_LOT"
#define SECTION_BI_CLOSE_LOT					"BI_CLOSE_LOT"
#define SECTION_CHECK_OUT_BOARD					"CHECK_OUT_BOARD"
#define SECTION_CHECK_IN_BOARD					"CHECK_IN_BOARD"
#define SECTION_PM_CHECK						"PM_CHECK"

#define KEY_RETURN_FLAG							"RETURN_FLAG"
#define KEY_ERROR_DESCRIPTION					"ERROR_DESCRIPTION"
#define KEY_JAM_GRADE							"JAM_GRADE"
#define KEY_SINGO_EVENT							"SINGO_EVENT"
#define KEY_JAM_CONTENT							"JAM_CONTENT"
#define KEY_AUTO_DIAG_FLAG						"AUTO_DIAG_FLAG"
#define KEY_AUTO_DIAG_DESC						"AUTO_DIAG_DESC"

/////////////////////////////////////////////////////////////////
////START_BI_CMD //2024.06.12
/////////////////////////////////////////////////////////////////
#define RCMD_START_BI_CMD			"START_BI_CMD"
#define KEY_SBC_BATCH_ID			"BATCH_ID"
#define KEY_SBC_LOT_ID				"LOT_ID"
#define KEY_SBC_GROUP				"GROUP"
#define KEY_SBC_BID_COUNT			"BID_COUNT"
#define KEY_SBC_BACK_COUNT			"RACK_COUNT"
#define KEY_SBC_ZONE_NO1			"ZONE_NO1"
#define KEY_SBC_ZONE_NO2			"ZONE_NO2"
#define KEY_SBC_ZONE_NO3			"ZONE_NO3"
#define KEY_SBC_ZONE_NO4			"ZONE_NO4"
#define KEY_SBC_BOARD_ID1			"BOARD_ID1"
#define KEY_SBC_BOARD_ID2			"BOARD_ID2"
#define KEY_SBC_BOARD_ID3			"BOARD_ID3"
#define KEY_SBC_BOARD_ID4			"BOARD_ID4"
#define KEY_SBC_BOARD_ID5			"BOARD_ID5"
#define KEY_SBC_BOARD_ID6			"BOARD_ID6"
#define KEY_SBC_BOARD_ID7			"BOARD_ID7"
#define KEY_SBC_BOARD_ID8			"BOARD_ID8"
#define KEY_SBC_BOARD_ID9			"BOARD_ID9"
#define KEY_SBC_BOARD_ID10			"BOARD_ID10"
#define KEY_SBC_BOARD_ID11			"BOARD_ID11"
#define KEY_SBC_BOARD_ID12			"BOARD_ID12"
#define KEY_SBC_BOARD_ID13			"BOARD_ID13"
#define KEY_SBC_BOARD_ID14			"BOARD_ID14"
#define KEY_SBC_BOARD_ID15			"BOARD_ID15"
#define KEY_SBC_BOARD_ID16			"BOARD_ID16"
#define KEY_SBC_BOARD_ID17			"BOARD_ID17"
#define KEY_SBC_BOARD_ID18			"BOARD_ID18"
#define KEY_SBC_BOARD_ID19			"BOARD_ID19"
#define KEY_SBC_BOARD_ID20			"BOARD_ID20"
#define KEY_SBC_BOARD_ID21			"BOARD_ID21"
#define KEY_SBC_BOARD_ID22			"BOARD_ID22"
#define KEY_SBC_BOARD_ID23			"BOARD_ID23"
#define KEY_SBC_BOARD_ID24			"BOARD_ID24"
#define KEY_SBC_BOARD_ID25			"BOARD_ID25"
#define KEY_SBC_BOARD_ID26			"BOARD_ID26"
#define KEY_SBC_BOARD_ID27			"BOARD_ID27"
#define KEY_SBC_BOARD_ID28			"BOARD_ID28"
#define KEY_SBC_BOARD_ID29			"BOARD_ID29"
#define KEY_SBC_BOARD_ID30			"BOARD_ID30"
#define KEY_SBC_BOARD_ID31			"BOARD_ID31"
#define KEY_SBC_BOARD_ID32			"BOARD_ID32"

/////////////////////////////////////////////////////////////////
////TGET //2024.06.19
/////////////////////////////////////////////////////////////////
#define RCMD_TGET							"TGET"
#define KEY_TGET_RETURN_FLAG				"RETURN_FLAG"
#define KEY_TGET_ERROR_DESC					"ERROR_DESCRIPTION"
#define KEY_TGET_SORT						"SORT"
#define KEY_TGET_TGET						"TGET"
#define KEY_TGET_LOT_NO						"LOT_NO"
#define KEY_TGET_MLB_FLAG					"MLB_FLAG"
#define KEY_TGET_MLB_PGM_LL					"MLB_PGM_LOT_LIST"
#define KEY_TGET_PART_NO					"PART_NO"
#define KEY_TGET_TEST_PGM_NO				"TEST_PGM_NO"
#define KEY_TGET_EQUIP_NO					"EQUIP_NO"
#define KEY_TGET_BIB_TYPE					"BIB_TYPE"
#define KEY_TGET_SYS_OP_ID					"SYSTEM_OP_ID"
#define KEY_TGET_DEVICE_NO					"DEVICE_NO"
#define KEY_TGET_OPER						"OPER"
#define KEY_TGET_FAB						"FAB"
#define KEY_TGET_GRADE						"GRADE"
#define KEY_TGET_DEVICE_QTY					"DEVICE_QTY"
#define KEY_TGET_SPECIAL_CODE				"SPECIAL_CODE"
#define KEY_TGET_PGM_DIR					"PGM_DIR"
#define KEY_TGET_DEVICE_DENSITY				"DEVICE_DENSITY"
#define KEY_TGET_DEVICE_TECH				"DEVICE_TECH"
#define KEY_TGET_BIB_QTY					"BIB_QTY"
#define KEY_TGET_TAMS_IP					"TAMS_IP"
#define KEY_TGET_TAMS_ID					"TAMS_ID"
#define KEY_TGET_TAMS_PWD					"TAMS_PWD"
#define KEY_TGET_POWER_UP_RATE_LIMIT		"POWER_UP_RATE_LIMIT"
#define KEY_TGET_POWER_UP_YIELD_LIMIT		"POWER_UP_YIELD_LIMIT"
#define KEY_TGET_PGM_TEST_TIME				"PGM_TEST_TIME"
#define KEY_TGET_POWER_UP_PDA_LIMIT			"POWER_UP_PDA_LIMIT"
#define KEY_TGET_PGM_EQ_MODEL				"PGM_EQ_MODEL"
#define KEY_TGET_OS_VERSION					"OS_VERSION"
#define KEY_TGET_FAMILY						"FAMILY"
#define KEY_TGET_OWNER						"OWNER"
#define KEY_TGET_FACTORY					"FACTORY"
#define KEY_TGET_RWK_COUNT					"RWK_COUNT"
#define KEY_TGET_RWK_CODE					"RWK_CODE"
#define KEY_TGET_1RML_GRP_ID				"1RML_GRP_ID"
#define KEY_TGET_MPG						"MPG"
#define KEY_TGET_TEMP1						"Temp1"
#define KEY_TGET_TEMP2						"Temp2"
#define KEY_TGET_TEMP3						"Temp3"
#define KEY_TGET_EQUIP_OWNER				"EQUIP_OWNER"
#define KEY_TGET_RML_GRP_ID_BIB_QTY			"RML_GRP_ID_BIB_QTY"


/////////////////////////////////////////////////////////////////
////BOARD MAINT INFO //2024.06.20
/////////////////////////////////////////////////////////////////
#define RCMD_BOARD_MAINT_INFO				"BOARD_MAINT_INFO"
#define KEY_BMI_RETURN_FLAG					"RETURN_FLAG"
#define KEY_BMI_ERROR_DESC					"ERROR_DESCRIPTION"
#define KEY_BMI_BIB_TYPE					"BIB_TYPE"
#define KEY_BMI_BIB_ID						"BIB_ID"
#define KEY_BMI_BIB_X_SIZE					"BIB_X_SIZE"
#define KEY_BMI_BIB_Y_SIZE					"BIB_Y_SIZE"
#define KEY_BMI_P_CODE						"P_CODE"
#define KEY_BMI_MLB_CODE					"MLB_CODE"
#define KEY_BMI_MLB_LOT_LIST				"MLB_LOT_LIST"
#define KEY_BMI_IOC							"IOC"
#define KEY_BMI_BOC							"BOC"
#define KEY_BMI_BURN_IN_COUNT				"BURN_IN_COUNT"
#define KEY_BMI_PART_NO						"PART_NO"
#define KEY_BMI_LOT_NO						"LOT_NO"
#define KEY_BMI_LOT_MAP						"LOT_MAP"
#define KEY_BMI_TEST_PGM_NO					"TEST_PGM_NO"
#define KEY_BMI_INSERT_MOUNT_MAP			"INSERT_MOUNT_MAP"
#define KEY_BMI_INITIAL_MASK_MAP			"INITIAL_MASK_MAP"
#define KEY_BMI_MANUAL_MASK_MAP				"MANUAL_MASK_MAP"
#define KEY_BMI_FUNCTION_MASK_MAP			"FUNCTION_MASK_MAP"
#define KEY_BMI_PRODUCT_NAME				"PRODUCT_NAME"
#define KEY_BMI_BIB_LAYOUT_FILE_NAME		"BIB_LAYOUT_FILE_NAME"

/////////////////////////////////////////////////////////////////
////RCMD FILE //2024.07.24
/////////////////////////////////////////////////////////////////
#define RCMD_BOARD_TYPE						"BOARD_TYPE"
#define RCMD_BI_OPEN_LOT                    "BI_OPEN_LOT"

/////////////////////////////////////////////////////////////////
////VID //2024.07.24
/////////////////////////////////////////////////////////////////
#define VID_DEFAULT_SYSTEM_OP_ID_VALUE			"EIS"

#define VID_EQ_ID								"100"
#define VID_INSERT_EQP_NO						"110"
#define VID_BURN_IN_COUNT                       "170"
#define VID_BIB_ID								"200"
#define VID_BIB_TYPE							"210"
#define VID_BIB_LIST							"205"
#define VID_PG_FW_INFO							"250"
#define VID_DRV_FW_INFO							"260"
#define VID_OS_VER  							"270"
#define VID_LOT_MAP								"300"
#define VID_RE_LOT_MAP							"309"
//#define VID_SYSTEM_BIN_MAP                      "325"			SAME NAME AS 330
#define VID_SYSTEM_BIN_MAP                      "330"
#define VID_INITIAL_BIN_VALUE					"370"
#define VID_FIRST_INITIAL_BIN_VALUE             "380"
#define VID_BIN_CODE                            "400"
#define VID_BIN_QTY                             "401"
#define VID_BIB_QTY                             "500"
#define VID_DEVICE_QTY                          "520"
#define VID_IN_QTY                              "521"
#define VID_OUT_QTY                             "522"
#define VID_INITIAL_TEST_DUT_QTY				"530"
#define VID_INITIAL_TEST_PASS_QTY				"531"
#define VID_INITIAL_TEST_FAIL_QTY				"532"
#define VID_INITIAL_SOCKET_MASK_QTY				"535"
#define VID_SYSTEM_INSERT_DEVICE_QTY			"550"
#define VID_TEST_TOTAL_QTY          			"560"
#define VID_TEST_PASS_QTY           			"561"
#define VID_TEST_FAIL_QTY           			"562"
#define VID_INITIAL_TEST_TOTAL_COUNT			"730"
#define VID_TEMP_INIT_FLAG						"740"
#define VID_MLB_FLAG							"770"
#define VID_MLB_EQ_FLAG							"771"
#define VID_P_CODE								"800"
#define VID_PROCESS								"810"
#define VID_SPECIAL_CODE                        "830"
#define VID_MLB_CODE							"880"
#define VID_EVENT_ID							"900"
#define VID_COMMENT								"910"
#define VID_ZONE1_FLAG							"911"
#define VID_ZONE2_FLAG							"912"
#define VID_ZONE3_FLAG							"913"
#define VID_ZONE4_FLAG							"914"
#define VID_ZONE_NO								"915"
#define VID_RACK_COUNT							"916"
#define VID_EMPTY_ZONE							"917"
#define VID_MULU_PORT							"918"
#define VID_DOOR_OPEN_INFO						"950"
#define VID_STATUS_ID							"990"
#define VID_LOT_NO								"1000"
#define VID_SNO 								"1005"
#define VID_PART_NO								"1010"
#define VID_FAMILY								"1020"
#define VID_OS_VERSION							"1090"
#define VID_SYSTEM_OP_ID                        "1120"
#define VID_SLOT_ID								"1125"
#define VID_BIB_X_SIZE                          "1200"
#define VID_BIB_Y_SIZE                          "1201"
#define VID_BIB_TYPE_DENSITY_INFO               "1300"
#define VID_MLB_PGM_LOT_LIST					"1770"
#define VID_MLB_LOT_LIST						"1880"
#define VID_PGM_NO								"2000"
#define VID_TRANS_TYPE							"2100"
#define VID_RACK_RFID							"3103"
#define VID_SITE_ID     						"4200"
#define VID_SITE_FIRMWARE						"4210"
#define VID_SITE_CPU_OS_VERSION     			"4220"
#define VID_CAL_FLAG                         	"4300"
#define VID_CAL_DATE                  			"4310"
#define VID_AUTO_MODE							"4600"
#define VID_FIRST_INITIAL_BIN                   "4610"
#define VID_BIN0A                               "4630"
#define VID_1RML_GRP_ID                         "7000"
#define VID_MPG_FLAG                            "7001"
#define VID_BOC									"7600"
#define VID_IOC									"7620"
#define VID_INITIAL_START_TIME					"9040"
#define VID_INITIAL_END_TIME					"9041"
#define VID_TEST_START_TIME 					"9070"
#define VID_CHECK_IN_BOARD_TIME                 "9120"
#define VID_END_TIME                            "9160"
#define VID_POWER_FAULT_COUNT                   "9600"
#define VID_EMPTY_DATE_TIME						"9910"
#define VID_STATUS_TIME							"9930"
#define VID_START_DATE_TIME                     "9080"
#define VID_SYSTEM_START_TIME 					"9090"
#define VID_SYSTEM_END_TIME 					"9091"
#define VID_BEFORE_BI_TEST_START_TIME           "9180"
#define VID_BEFORE_BI_TEST_END_TIME         	"9181"
#define VID_BI_TEST_START_TIME                  "9190"
#define VID_BI_TEST_END_TIME                	"9191"
#define VID_AFTER_BI_TEST_START_TIME            "9200"
#define VID_AFTER_BI_TEST_END_TIME           	"9201"
#define VID_TEMP_UP_START_TIME_1                "9210"
#define VID_TEMP_UP_END_TIME_1                  "9211"
#define VID_TEMP_UP_START_TIME_2                "9220"
#define VID_TEMP_UP_END_TIME_2                  "9221"
#define VID_TEMP_DOWN_START_TIME_1              "9230"
#define VID_TEMP_DOWN_END_TIME_1              	"9231"
#define VID_TEMP_DOWN_START_TIME_2              "9240"
#define VID_TEMP_DOWN_END_TIME_2                "9241"
#define VID_OTHERS_START_TIME                   "9250"
#define VID_OTHERS_END_TIME                     "9251"

#endif
