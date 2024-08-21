#include "yokogawaplc.hpp"

CYokogawaPlc::CYokogawaPlc(QObject *parent) : QObject(parent)
{
	m_bMsgLoop = true;
	m_bPlcCon  = false;
	CreateYkgCommandMap();
	CreateYkgExitErrorCodeMap();
	m_pSocket = new QTcpSocket(this);
	connect( m_pSocket, &QTcpSocket::disconnected, this, &CYokogawaPlc::SlotPlcDisconnected );
}

CYokogawaPlc::~CYokogawaPlc()
{
	if( m_pSocket->state() == QAbstractSocket::ConnectedState )
	{
		m_pSocket->disconnectFromHost();
		if ( m_pSocket->state() == QAbstractSocket::UnconnectedState || m_pSocket->waitForDisconnected(1000) )
		{
			qDebug() << __func__ << " Disconnected!!";
		}
	}
	m_pSocket->close();
	m_bPlcCon = false;
	if( m_pSocket ) delete m_pSocket;
}

void CYokogawaPlc::SetMsgLoopMode(bool mode)
{
	m_bMsgLoop = mode;
}

void CYokogawaPlc::SetChamberTempLimit(double l, double h)
{
	m_dTempLimitLow  = l;
	m_dTempLimitHigh = h;
}

void CYokogawaPlc::CreateYkgCommandMap()
{
	m_mapCmd.clear();
	m_mapCmd[CMD_BRD] = "BRD";  //"BIT READ"
	m_mapCmd[CMD_BWR] = "BWR";  //"BIT WRITE"
	m_mapCmd[CMD_BRR] = "BRR";  //"BIT RANDOM READ"
	m_mapCmd[CMD_BRW] = "BRW";  //"BIT RANDOM WRITE"
	m_mapCmd[CMD_WRD] = "WRD";  //"WORD READ"
	m_mapCmd[CMD_WWR] = "WWR";  //"WORD WRITE"
	m_mapCmd[CMD_WRR] = "WRR";  //"WORD RANDOM READ"
	m_mapCmd[CMD_WRW] = "WRW";  //"WORD RANDOM WRITE"
	m_mapCmd[CMD_SWR] = "SWR";  //"WORD SPECIAL READ"
	m_mapCmd[CMD_SWW] = "SWW";  //"WORD SPECIAL WRITE"
	m_mapCmd[CMD_PRI] = "PRI";  //"PROGRAM INFO READ"
}

void CYokogawaPlc::CreateYkgExitErrorCodeMap()
{
	m_mapExitErrCode["OK"]    = stExitErrorCode { EXIT_CODE_OK  , "NORMAL EXIT. "                     };
	m_mapExitErrCode["ER01"]  = stExitErrorCode { EXIT_CODE_ER01, "CPU NUMBER ERROR. "                };
	m_mapExitErrCode["ER02"]  = stExitErrorCode { EXIT_CODE_ER02, "COMMAND ERROR. "                   };
	m_mapExitErrCode["ER03"]  = stExitErrorCode { EXIT_CODE_ER02, "DEVICE SPECITICATION ERROR. "      };
	m_mapExitErrCode["ER04"]  = stExitErrorCode { EXIT_CODE_ER04, "VALUE OUTSIDE THE SETTING RANGE. " };
	m_mapExitErrCode["ER05"]  = stExitErrorCode { EXIT_CODE_ER05, "DATA COUNT OUT OF RANGE. "         };
	m_mapExitErrCode["ER06"]  = stExitErrorCode { EXIT_CODE_ER06, "MONITOR ERROR. "                   };
	m_mapExitErrCode["ER08"]  = stExitErrorCode { EXIT_CODE_ER08, "PARAMETER ERROR. "                 };
	m_mapExitErrCode["ER51"]  = stExitErrorCode { EXIT_CODE_ER51, "SEQUENCE CPU ERROR. "              };
	m_mapExitErrCode["ER52"]  = stExitErrorCode { EXIT_CODE_ER52, "SEQUENCE CPU PROCESSING ERROR. "   };

	m_mapExitErrCode["1"]     = stExitErrorCode { ERROR_CODE_ER1, "SELF-DIAGNOSTIC ERROR"             };
	m_mapExitErrCode["2"]     = stExitErrorCode { ERROR_CODE_ER2, "PROGRAM ERROR"                     };
	m_mapExitErrCode["3"]     = stExitErrorCode { ERROR_CODE_ER3, "INTER-CPU COMMUNICATION ERROR"     };
	m_mapExitErrCode["8"]     = stExitErrorCode { ERROR_CODE_ER8, "DEVICE ACCESS ERROR"               };
	m_mapExitErrCode["9"]     = stExitErrorCode { ERROR_CODE_ER9, "COMMAND ERROR"                     };
	m_mapExitErrCode["A"]     = stExitErrorCode { ERROR_CODE_ERA, "PARAMETER ERROR"                   };
	m_mapExitErrCode["B"]     = stExitErrorCode { ERROR_CODE_ERB, "OPERATION MODE ERROR"              };
	m_mapExitErrCode["C"]     = stExitErrorCode { ERROR_CODE_ERC, "PARAMETER ERROR"                   };
	m_mapExitErrCode["F"]     = stExitErrorCode { ERROR_CODE_ERF, "SYSTEM ERROR"                      };
}

PLC_ADDRESS_MAP CYokogawaPlc::GetPlcCmdToAddr(PROTOCOL::PLC_COMMAND cmd)
{
	PLC_ADDRESS_MAP address = ADDR_PLC_UNDEF;
	switch ( cmd )
	{
		case PROTOCOL::CMD_WB_TEMP_RUN_TRIGGER             :
			address = ADDR_PLC_W_TEMP_RUN;
			break;
		case PROTOCOL::CMD_WB_TEMP_STOP_TRIGGER            :
			address = ADDR_PLC_W_TEMP_STOP;
			break;
		case PROTOCOL::CMD_WB_AMB_MODE_TRIGGER             :
			address = ADDR_PLC_W_AMBIENT_RUN;
			break;
		case PROTOCOL::CMD_WB_AMB_AUTO_TRIGGER             :
			address = ADDR_PLC_W_AMBIENT_AUTO_RUN;
			break;
		case PROTOCOL::CMD_WB_ALARM_RESET                  :
			address = ADDR_PLC_W_ALARM_RESET;
			break;
		case PROTOCOL::CMD_WB_MASK_ONOFF_TRIGGER           :
			address = ADDR_PLC_W_MASK_ON_OFF;
			break;
		case PROTOCOL::CMD_WB_DOOR_OPEN_TRIGGER            :
			address = ADDR_PLC_W_DOOR_OPEN_RUN;
			break;
		case PROTOCOL::CMD_WB_DOOR_CLOSE_TRIGGER           :
			address = ADDR_PLC_W_DOOR_CLOSE_RUN;
			break;
		case PROTOCOL::CMD_WB_RACK_IN_TRIGGER              :
			address = ADDR_PLC_W_RACK_IN_RUN;
			break;
		case PROTOCOL::CMD_WB_RACK_OUT_TRIGGER             :
			address = ADDR_PLC_W_RACK_OUT_RUN;
			break;
		case PROTOCOL::CMD_WB_RACK_NOP_TRIGGER             :
			address = ADDR_PLC_W_RACK_NOP;
			break;

		case PROTOCOL::CMD_WW_WATCHDOG_TIME_SET            :
			address = ADDR_PLC_W_WATCHDOG_TIME_SET;
			break;
		case PROTOCOL::CMD_WW_WATCHDOG_TOGGLE_TRIGGER      :
			address = ADDR_PLC_W_WATCHDOG_TOGGLE;
			break;
		case PROTOCOL::CMD_WW_WATCHDOG_ENB_DSB             :
			address = ADDR_PLC_W_WATCHDOG_ENABLE;
			break;
		case PROTOCOL::CMD_WW_SV_WRITE                     :
			address = ADDR_PLC_W_SV_SET;
			break;
		case PROTOCOL::CMD_WW_AMB_SV_WRITE                 :
			address = ADDR_PLC_W_AMBIENT_SV_SET;
			break;
		case PROTOCOL::CMD_RB_RACK_IN                      :
			address = ADDR_PLC_R_RACK_IN_COMPLETE;
			break;
		case PROTOCOL::CMD_RB_RACK_OUT                     :
			address = ADDR_PLC_R_RACK_OUT_COMPLETE;
			break;
		case PROTOCOL::CMD_RB_DOOR_OPEN                    :
			address = ADDR_PLC_R_DOOR_OPEN;
			break;
		case PROTOCOL::CMD_RB_DOOR_CLOSE                   :
			address = ADDR_PLC_R_DOOR_CLOSE;
			break;
		case PROTOCOL::CMD_RB_TEMP_RUN                     :
			address = ADDR_PLC_R_TEMP_RUN;
			break;
		case PROTOCOL::CMD_RB_TEMP_STOP                    :
			address = ADDR_PLC_R_TEMP_STOP;
			break;
		case PROTOCOL::CMD_RB_AMBIENT                      :
			address = ADDR_PLC_R_AMBIENT;
			break;
		case PROTOCOL::CMD_RW_PLC_VER                      :
			address = ADDR_PLC_R_VERSION;
			break;
		case PROTOCOL::CMD_RW_PV                           :
			address = ADDR_PLC_R_PV;
			break;
		case PROTOCOL::CMD_RW_SV                           :
			address = ADDR_PLC_R_SV;
			break;
		case PROTOCOL::CMD_RW_PCW_TEMP                     :
			address = ADDR_PLC_R_PCW_TEMPERATURE;
			break;
		case PROTOCOL::CMD_RW_WPS_PRESSURE                 :
			address = ADDR_PLC_R_WPS_PRESSURE;
			break;
		case PROTOCOL::CMD_RW_PCW_FLOW                     :
			address = ADDR_PLC_R_PCW_FLOW;
			break;
		case PROTOCOL::CMD_RW_STATUS1                      :	//I00210 ~ I00224
			address = ADDR_PLC_R_WORD_STATUS1;
			break;
		case PROTOCOL::CMD_RW_STATUS2                      :	//I00226 ~ I00241
			address = ADDR_PLC_R_WORD_STATUS2;
			break;
		case PROTOCOL::CMD_RW_ALARM0                       :	//I1600 ~ I1615
			address = ADDR_PLC_R_ALARM_D150;
			break;
		case PROTOCOL::CMD_RW_ALARM1                       :
			address = ADDR_PLC_R_ALARM_D151;
			break;
		case PROTOCOL::CMD_RW_ALARM2                       :
			address = ADDR_PLC_R_ALARM_D152;
			break;
		case PROTOCOL::CMD_RW_ALARM3                       :
			address = ADDR_PLC_R_ALARM_D153;
			break;
		case PROTOCOL::CMD_RW_ALARM4                       :
			address = ADDR_PLC_R_ALARM_D154;
			break;
		case PROTOCOL::CMD_RW_ALARM5                       :
			address = ADDR_PLC_R_ALARM_D155;
			break;
		case PROTOCOL::CMD_RW_ALARM6                       :
			address = ADDR_PLC_R_ALARM_D156;
			break;
		case PROTOCOL::CMD_RW_ALARM7                       :
			address = ADDR_PLC_R_ALARM_D157;
			break;
		case PROTOCOL::CMD_RW_ALARM8                       :
			address = ADDR_PLC_R_ALARM_D158;
			break;
		case PROTOCOL::CMD_RW_ALARM9                       :
			address = ADDR_PLC_R_ALARM_D159;
			break;

		case PROTOCOL::CMD_RU_ALARM_READ                   :	//READ_USER(ALARM0 ~ ALARM9)
		case PROTOCOL::CMD_SYS_PLC_CONN                    :	//CHAMB UI <-> CHMB COMM <-> PLC
			address = ADDR_PLC_UNDEF;
			break;
		//2023.04.13 - KWMOON
		case PROTOCOL::CMD_WB_BUZZER_ONOFF                 :
			address = ADDR_PLC_W_BUZZER_ON_OFF;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_LAMP_MASK_ONOFF              :
			address = ADDR_PLC_W_LAMP_LOCAL_REMOTE;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_ON              :
			address = ADDR_PLC_W_RED_LAMP_ON;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_ON              :
			address = ADDR_PLC_W_YELLOW_LAMP_ON;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_ON              :
			address = ADDR_PLC_W_GREEN_LAMP_ON;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_BLINK              :
			address = ADDR_PLC_W_RED_LAMP_BLINK;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_BLINK              :
			address = ADDR_PLC_W_YELLOW_LAMP_BLINK;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_BLINK              :
			address = ADDR_PLC_W_GREEN_LAMP_BLINK;
			break;
		//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_ALL_LAMP_OFF              :
			address = ADDR_PLC_W_ALL_LAMP_OFF;
			break;
		//2023.09.07 - KWMOON
		case PROTOCOL::CMD_RB_RACK_NOP_TRIGGER      :
			address = ADDR_PLC_R_RACK_NOP;
			break;
		//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_FINGER_UP_STAT_RACKIN_ON      :
			address = ADDR_PLC_W_FINGER_RACKIN_ON;
			break;
		//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_FINGER_UP_STAT_RACKIN_OFF     :
			address = ADDR_PLC_W_FINGER_RACKIN_OFF;
			break;
		//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_DOOR_IGNORE_RACK_INOUT_ON     :
			address = ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_ON;
			break;
		//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF    :
			address = ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_OFF;
			break;
		//2023.09.19 - KWMOON
		case PROTOCOL::CMD_WB_SYSTEM_DC_ONOFF:
			address = ADDR_PLC_W_SYSTEM_DC_ONOFF;
			break;
		/////////////////////////////////////////////////////////////////////////////
		//2023.09.19 - KWMOON(COMP1, COMP2 PRESSURE, OUT, IN )
		/////////////////////////////////////////////////////////////////////////////
		case PROTOCOL::CMD_RW_COMP1_PRESSURE_HIGH_VALUE :
			address = ADDR_PLC_R_COMP1_PRESSURE_HIGH_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP1_PRESSURE_LOW_VALUE  :
			address = ADDR_PLC_R_COMP1_PRESSURE_LOW_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP2_PRESSURE_HIGH_VALUE :
			address = ADDR_PLC_R_COMP2_PRESSURE_HIGH_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP2_PRESSURE_LOW_VALUE  :
			address = ADDR_PLC_R_COMP2_PRESSURE_LOW_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP1_OUT_TEMP_VALUE      :
			address = ADDR_PLC_R_COMP1_OUT_TEMP_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP1_IN_TEMP_VALUE       :
			address = ADDR_PLC_R_COMP1_IN_TEMP_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP2_OUT_TEMP_VALUE      :
			address = ADDR_PLC_R_COMP2_OUT_TEMP_VALUE;
			break;
		case PROTOCOL::CMD_RW_COMP2_IN_TEMP_VALUE       :
			address = ADDR_PLC_R_COMP2_IN_TEMP_VALUE;
			break;
		default:
			address = ADDR_PLC_UNDEF;
			break;
	}
	return address;
}

QString CYokogawaPlc::GetPlcAddrToString(PLC_ADDRESS_MAP address)
{
	QString sAddress = "";
	switch ( address )
	{
		case ADDR_PLC_W_TEMP_RUN               :
			sAddress = "I00110";
			break;
		case ADDR_PLC_W_TEMP_STOP              :
			sAddress = "I00111";
			break;
		case ADDR_PLC_W_AMBIENT_RUN            :
			sAddress = "I00112";
			break;
		case ADDR_PLC_W_AMBIENT_AUTO_RUN       :
			sAddress = "I00113";
			break;
		case ADDR_PLC_W_ALARM_RESET            :
			sAddress = "I00117";
			break;
		case ADDR_PLC_W_MASK_ON_OFF             :
//			sAddress = "I01105";	//2023.08.29 - KWMOON
			sAddress = "I00118";
			break;
		case ADDR_PLC_W_DOOR_OPEN_RUN          :
			sAddress = "I00120";
			break;
		case ADDR_PLC_W_DOOR_CLOSE_RUN         :
			sAddress = "I00121";
			break;
		case ADDR_PLC_W_RACK_IN_RUN            :
			sAddress = "I00122";
			break;
		case ADDR_PLC_W_RACK_OUT_RUN           :
			sAddress = "I00123";
			break;
		case ADDR_PLC_W_RACK_NOP               :
			sAddress = "I00124";
			break;
		case ADDR_PLC_W_WATCHDOG_TIME_SET      :
			sAddress = "D00901";
			break;
		case ADDR_PLC_W_WATCHDOG_TOGGLE        :
			sAddress = "D00902";
			break;
		case ADDR_PLC_W_WATCHDOG_ENABLE        :
			sAddress = "D00903";
			break;
		case ADDR_PLC_W_SV_SET                 :
			sAddress = "D01103";
			break;
		case ADDR_PLC_W_AMBIENT_SV_SET         :
			sAddress = "D01113";
			break;
		case ADDR_PLC_R_WORD_STATUS1           :
			sAddress = "D100";
			break;
		case ADDR_PLC_R_MASK_OFF               :
			sAddress = "I00210";
			break;
		case ADDR_PLC_R_MASK_ON                :
			sAddress = "I00211";
			break;
		case ADDR_PLC_R_WATCHDOG_ENABLE        :
			sAddress = "I00212";
			break;
		case ADDR_PLC_R_WATCHDOG_DISABLE       :
			sAddress = "I00213";
			break;
		case ADDR_PLC_R_DOOR_OPEN              :
			sAddress = "I00214";
			break;
		case ADDR_PLC_R_DOOR_CLOSE             :
			sAddress = "I00215";
			break;
		case ADDR_PLC_R_UPPER_RACK_DETECT      :
			sAddress = "I00216";
			break;
		case ADDR_PLC_R_BOTTOM_RACK_DETECT     :
			sAddress = "I00217";
			break;
		case ADDR_PLC_R_RACK_IN_COMPLETE       :
			sAddress = "I00218";
			break;
		case ADDR_PLC_R_RACK_OUT_COMPLETE      :
			sAddress = "I00219";
			break;
		case ADDR_PLC_R_EQ_READY               :
			sAddress = "I00220";
			break;
		case ADDR_PLC_R_EQ_ABORT               :
			sAddress = "I00221";
			break;
		case ADDR_PLC_R_AVG_COMPLETE           :
			sAddress = "I00222";
			break;
		case ADDR_PLC_R_WORD_STATUS2           :
			sAddress = "D101";
			break;
		case ADDR_PLC_R_TEMP_RUN               :
			sAddress = "I00226";
			break;
		case ADDR_PLC_R_TEMP_STOP              :
			sAddress = "I00227";
			break;
		case ADDR_PLC_R_AMBIENT                :
			sAddress = "I00228";
			break;
		case ADDR_PLC_R_SYSTEM_DC_POWER_ON     :
			sAddress = "I00229";
			break;
		case ADDR_PLC_R_VERSION                :		//WORD
			sAddress = "D999";
			break;
		case ADDR_PLC_R_PV                     :
			sAddress = "D01100";
			break;
		case ADDR_PLC_R_SV                     :
			sAddress = "D01101";
			break;
		case ADDR_PLC_R_PCW_TEMPERATURE        :
			sAddress = "D01110";
			break;
		case ADDR_PLC_R_WPS_PRESSURE           :
			sAddress = "D01111";
			break;
		case ADDR_PLC_R_PCW_FLOW               :
			sAddress = "D01112";
			break;
		case ADDR_PLC_R_ALARM_D150             :
			sAddress = "D150";
			break;
		case ADDR_PLC_R_ALARM_D151             :
			sAddress = "D151";
			break;
		case ADDR_PLC_R_ALARM_D152             :
			sAddress = "D152";
			break;
		case ADDR_PLC_R_ALARM_D153             :
			sAddress = "D153";
			break;
		case ADDR_PLC_R_ALARM_D154             :
			sAddress = "D154";
			break;
		case ADDR_PLC_R_ALARM_D155             :
			sAddress = "D155";
			break;
		case ADDR_PLC_R_ALARM_D156             :
			sAddress = "D156";
			break;
		case ADDR_PLC_R_ALARM_D157             :
			sAddress = "D157";
			break;
		case ADDR_PLC_R_ALARM_D158             :
			sAddress = "D158";
			break;
		case ADDR_PLC_R_ALARM_D159             :
			sAddress = "D159";
			break;
		//2023.04.13 - KWMOON
		case ADDR_PLC_W_BUZZER_ON_OFF          :
			sAddress = "I00960";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_LAMP_LOCAL_REMOTE       :
			sAddress = "I00970";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_RED_LAMP_ON       :
			sAddress = "I00971";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_YELLOW_LAMP_ON       :
			sAddress = "I00972";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_GREEN_LAMP_ON       :
			sAddress = "I00973";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_RED_LAMP_BLINK       :
			sAddress = "I00974";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_YELLOW_LAMP_BLINK       :
			sAddress = "I00975";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_GREEN_LAMP_BLINK       :
			sAddress = "I00976";
			break;
		//2023.05.24 - KWMOON
		case ADDR_PLC_W_ALL_LAMP_OFF       :
			sAddress = "I00977";
			break;
		//2023.09.07 - KWMOON
		case ADDR_PLC_R_RACK_NOP       :
			sAddress = "I00124";
			break;
		//2023.09.07 - KWMOON
		case ADDR_PLC_W_FINGER_RACKIN_ON       :
			sAddress = "I00756";
			break;
		//2023.09.07 - KWMOON
		case ADDR_PLC_W_FINGER_RACKIN_OFF      :
			sAddress = "I00757";
			break;
		//2023.09.07 - KWMOON
		case ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_ON      :
			sAddress = "I00758";
			break;
		//2023.09.07 - KWMOON
		case ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_OFF     :
			sAddress = "I00759";
			break;
		//2023.09.19 - KWMOON
		case ADDR_PLC_W_SYSTEM_DC_ONOFF :
			sAddress = "Y00541";
			break;

		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP1_PRESSURE_HIGH_VALUE :
			sAddress = "D01130";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP1_PRESSURE_LOW_VALUE :
			sAddress = "D01131";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP2_PRESSURE_HIGH_VALUE :
			sAddress = "D01132";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP2_PRESSURE_LOW_VALUE :
			sAddress = "D01133";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP1_OUT_TEMP_VALUE :
			sAddress = "D01135";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP1_IN_TEMP_VALUE :
			sAddress = "D01136";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP2_OUT_TEMP_VALUE :
			sAddress = "D01137";
			break;
		//2024.08.19 - KWMOON
		case ADDR_PLC_R_COMP2_IN_TEMP_VALUE :
			sAddress = "D01138";
			break;

		case ADDR_PLC_UNDEF:
			sAddress = "D9999";
			break;

	}
	return sAddress;
}

bool CYokogawaPlc::MakePlcFormatCommand( YKG_CMD_LIST cmd, QString* sAddr, int* nData, int nLen )
{
	bool bCmdValid = true;
	QString sCmd   = m_mapCmd[cmd];
	QString sLen3  = QString("").asprintf("%.3d", nLen  );
	QString sLen2  = QString("").asprintf("%.2d", nLen  );
	QString sReq   = YKG_REQUEST_BYTE_VALUE;
	QString sCpu   = QString("").asprintf("%.1d", PLC_CPU);
	m_SendByteArray.clear();
	switch( cmd )
	{
		case CMD_BRD:	//BIT READ
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append((*sAddr).toUtf8());
			m_SendByteArray.append(',');
			m_SendByteArray.append(sLen3.toUtf8());
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_BWR:	//BIT WRITE
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append((*sAddr).toUtf8());
			m_SendByteArray.append(',');
			m_SendByteArray.append(sLen3.toUtf8());
			m_SendByteArray.append(',');
			for( int n=0;n<nLen;n++ )
			{
				if( nData[n] & 0x1 )
					m_SendByteArray.append('1');
				else
					m_SendByteArray.append('0');
			}
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_BRR:	//BIT RANDOM READ - not use
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append(sLen2.toUtf8());
			m_SendByteArray.append(',');
			for( int n=0;n<nLen;n++ )
			{
				m_SendByteArray.append(sAddr[n].toUtf8());
				if( n != (nLen-1) ) m_SendByteArray.append(',');
			}
			//AppendLog( tr("[BRD]%1").arg(m_baSendData.data()), true );
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_BRW:	//BIT RANDOM WRITE - not use
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append(sLen2.toUtf8());
			for( int n=0;n<nLen;n++ )
			{
				m_SendByteArray.append(sAddr[n].toUtf8());
				m_SendByteArray.append(',');
				if( nData[n] & 0x1 )
					m_SendByteArray.append('1');
				else
					m_SendByteArray.append('0');
				if( n != (nLen-1) )
					m_SendByteArray.append(',');
			}
			//AppendLog( tr("[BRW]%1").arg(m_baSendData.data()), true );
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_WRD:	//WORD READ
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append((*sAddr).toUtf8());
			m_SendByteArray.append(',');
			m_SendByteArray.append(sLen2.toUtf8());
			//AppendLog( tr("[WRD]%1").arg(m_SendByteArray.data()), true );
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
			//emit SigPlcSendRawData( stCmdType.cmd, m_baSendData, nLen );
		}
		break;

		case CMD_WWR:	//WORD WRITE
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append((*sAddr).toUtf8());
			m_SendByteArray.append(',');
			m_SendByteArray.append(sLen2.toUtf8());
			m_SendByteArray.append(',');
			for( int n=0;n<nLen;n++ )
			{
				QString sHex = QString("").asprintf("%04X", nData[n]&0xFFFF );
				m_SendByteArray.append(sHex.toUtf8());
			}
			//AppendLog( tr("[WWR]%1").arg(m_baSendData.data()), true );
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_WRR:	//WORD RANDOM READ
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append(sLen2.toUtf8());
			for( int n=0;n<nLen;n++ )
			{
				m_SendByteArray.append(sAddr[n].toUtf8());
				if( n != (nLen-1) ) m_SendByteArray.append(',');
			}
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_WRW:	//WORD RANDOM WRITE
		{
			m_SendByteArray.append(sReq.toUtf8());
			m_SendByteArray.append(sCpu.toUtf8());
			m_SendByteArray.append(sCmd.toUtf8());
			m_SendByteArray.append(sLen2.toUtf8());
			for( int n=0;n<nLen;n++ )
			{
				m_SendByteArray.append(sAddr[n].toUtf8());
				m_SendByteArray.append(',');
				QString sHex = QString("").asprintf("%04X", nData[n]&0xFFFF );
				m_SendByteArray.append(sHex.toUtf8());
				if( n != (nLen-1) )
					m_SendByteArray.append(',');
			}
			//AppendLog( tr("[WRW]%1").arg(m_baSendData.data()), true );
			m_SendByteArray.append(CR);
			m_SendByteArray.append(LF);
		}
		break;

		case CMD_SWR:
		case CMD_SWW:
		case CMD_PRI:
		{
			bCmdValid = false;
		}
		break;
	}
	return bCmdValid;
}

void CYokogawaPlc::SetPlcTcpInfo(QString ip, uint port)
{
	m_sIp = ip;
	m_uPort = static_cast<quint16>(port);
}

bool CYokogawaPlc::IsPlcConn()
{
	return m_bPlcCon;
}

bool CYokogawaPlc::CYokogawaPlc::PlcClose()
{
	if( !IsPlcConn() ) return true;

	if( m_pSocket->state() == QAbstractSocket::ConnectedState )
	{
		m_pSocket->disconnectFromHost();
		if (m_pSocket->state() == QAbstractSocket::UnconnectedState || m_pSocket->waitForDisconnected(1000))
		{
			//qDebug() << __func__ << " Disconnected!!";
		}
	}
	m_pSocket->close();
	if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, tr("[INFO] PLC Close connection!") );
	m_bPlcCon = false;
	return true;
}

void CYokogawaPlc::PlcSocketControl( PLC_ON_OFFLINE ctrl )
{
	int nControl = static_cast<int>(ctrl);
	bool bResult = false;
	switch( nControl )
	{
		case PLC_ONLINE:
			bResult = PlcConnect();
			emit SigPlcConnStatus ( bResult?PLC_ONLINE:PLC_OFFLINE );
			if( !bResult )
			{
				if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, tr("PLC Close connection!") );
			}
			break;

		case PLC_OFFLINE:
			bResult = PlcClose();
			emit SigPlcConnStatus( bResult?PLC_OFFLINE:PLC_ONLINE );
			break;
		default:
			break;
	}
}

bool CYokogawaPlc::PlcConnect()
{
	if( IsPlcConn() ) return true;

	m_bPlcCon = false;
	for( int nRetry=0;nRetry<MAX_RETRY_COMM_CNT;nRetry++ )
	{
		m_pSocket->connectToHost( m_sIp, m_uPort );
		if( !m_pSocket->waitForConnected( 100 ) )  //100 mseconds
		{
			continue;
		}
		else
		{
			m_bPlcCon = true;
			break;
		}
	}
	if( m_bMsgLoop ) emit SigPlcLogMessage( _NORMAL_LOG, tr("[INFO] %1:%2 try to connect...").arg(m_sIp).arg(m_uPort) );
	if( !m_bPlcCon) return false;

    emit SigPlcLogMessage( _NORMAL_LOG, tr("[INFO]%1:%2 Connected").arg(m_sIp).arg(m_uPort) );
	return true;
}

void CYokogawaPlc::SlotPlcDisconnected()
{
	m_bPlcCon = false;
	m_pSocket->close();
	if( m_bMsgLoop ) emit SigPlcConnStatus ( PLC_OFFLINE );
}

void CYokogawaPlc::SlotPlcSocketControl( PLC_ON_OFFLINE cmd )
{
	PlcSocketControl( cmd );
}

void CYokogawaPlc::SlotPlcControlByEmt( uint nId, stIpcElement IpcElement )
{
	QString STR_ADDR[PLC_MAX_ADDR_DATA] = {"",};
    int PLC_DATA[PLC_MAX_ADDR_DATA]  = {0x0,};
    char RAW_DATA[PLC_MAX_ADDR_DATA] = {0x0,};
	PLC_ADDRESS_MAP PLC_ADDR = ADDR_PLC_UNDEF;

	stIpcHeader header = IpcElement.ipcHeader;
    PROTOCOL::PLC_COMMAND cmd = static_cast<PROTOCOL::PLC_COMMAND>( header.cmd );
    uint size = header.datalen;
	int data = 0x0;
	if( size > 0 )
    {
		memcpy( (char*)&data, IpcElement.pData, size );
        memcpy( RAW_DATA    , IpcElement.pData, size );
    }

	bool bPass = true;
	switch ( cmd )
	{
		case PROTOCOL::CMD_WB_TEMP_RUN_TRIGGER          :
		case PROTOCOL::CMD_WB_TEMP_STOP_TRIGGER         :
		case PROTOCOL::CMD_WB_AMB_MODE_TRIGGER          :
		case PROTOCOL::CMD_WB_AMB_AUTO_TRIGGER          :
		case PROTOCOL::CMD_WB_ALARM_RESET               :
		case PROTOCOL::CMD_WB_MASK_ONOFF_TRIGGER        :
		case PROTOCOL::CMD_WB_DOOR_OPEN_TRIGGER         :
		case PROTOCOL::CMD_WB_DOOR_CLOSE_TRIGGER        :
		case PROTOCOL::CMD_WB_RACK_IN_TRIGGER           :
		case PROTOCOL::CMD_WB_RACK_OUT_TRIGGER          :
		case PROTOCOL::CMD_WB_RACK_NOP_TRIGGER          :
		case PROTOCOL::CMD_WB_BUZZER_ONOFF              :	//2023.04.13 - KWMOON
		case PROTOCOL::CMD_WB_LAMP_MASK_ONOFF           :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_ON               :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_ON            :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_ON             :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_BLINK            :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_BLINK         :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_BLINK          :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_ALL_LAMP_OFF              :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_FINGER_UP_STAT_RACKIN_ON  :	//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_FINGER_UP_STAT_RACKIN_OFF :	//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_DOOR_IGNORE_RACK_INOUT_ON :	//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF:	//2023.09.07 - KWMOON
		case PROTOCOL::CMD_WB_SYSTEM_DC_ONOFF           :
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			PLC_DATA[0] = data;
			if( !MakePlcFormatCommand( CMD_BWR, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveWriteAck( CMD_BWR );
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
					}
				}
			}
			DoChamberResponseEmit( nId, cmd, CMD_BWR, PLC_RESPONSE_PF, bPass );
		}
		break;

		case PROTOCOL::CMD_WW_WATCHDOG_TIME_SET            :
		case PROTOCOL::CMD_WW_WATCHDOG_TOGGLE_TRIGGER      :
		case PROTOCOL::CMD_WW_WATCHDOG_ENB_DSB             :
		case PROTOCOL::CMD_WW_SV_WRITE                     :
		case PROTOCOL::CMD_WW_AMB_SV_WRITE                 :
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			PLC_DATA[0] = data;
            if( !MakePlcFormatCommand( CMD_WWR, STR_ADDR, PLC_DATA, 0x1 ) ) //1 data
			{
				bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveWriteAck( CMD_WWR );
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
					}
				}
			}
			DoChamberResponseEmit( nId, cmd, CMD_WWR, PLC_RESPONSE_PF, bPass );
		}
		break;

		case PROTOCOL::CMD_RB_RACK_IN                      :
		case PROTOCOL::CMD_RB_RACK_OUT                     :
		case PROTOCOL::CMD_RB_DOOR_OPEN                    :
		case PROTOCOL::CMD_RB_DOOR_CLOSE                   :
		case PROTOCOL::CMD_RB_TEMP_RUN                     :
		case PROTOCOL::CMD_RB_TEMP_STOP                    :
		case PROTOCOL::CMD_RB_AMBIENT                      :
		case PROTOCOL::CMD_RB_RACK_NOP_TRIGGER             :	//2023.09.07 - KWMOON
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			if( !MakePlcFormatCommand( CMD_BRD, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveReadAck( CMD_BRD, PLC_DATA, 0x1 );
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
					}
				}
			}
			DoChamberResponseEmit( nId, cmd, CMD_BRD, PLC_RESPONSE_DATA, bPass, PLC_DATA, 0x1 );
		}
		break;

		case PROTOCOL::CMD_RW_PLC_VER                      :
		case PROTOCOL::CMD_RW_PV                           :
		case PROTOCOL::CMD_RW_SV                           :
		case PROTOCOL::CMD_RW_PCW_TEMP                     :
		case PROTOCOL::CMD_RW_WPS_PRESSURE                 :
		case PROTOCOL::CMD_RW_PCW_FLOW                     :
		case PROTOCOL::CMD_RW_STATUS1                      :	//I00210 ~ I00224
		case PROTOCOL::CMD_RW_STATUS2                      :	//I00226 ~ I00241
		case PROTOCOL::CMD_RW_ALARM0                       :	//I1600 ~ I1615
		case PROTOCOL::CMD_RW_ALARM1                       :
		case PROTOCOL::CMD_RW_ALARM2                       :
		case PROTOCOL::CMD_RW_ALARM3                       :
		case PROTOCOL::CMD_RW_ALARM4                       :
		case PROTOCOL::CMD_RW_ALARM5                       :
		case PROTOCOL::CMD_RW_ALARM6                       :
		case PROTOCOL::CMD_RW_ALARM7                       :
		case PROTOCOL::CMD_RW_ALARM8                       :
		case PROTOCOL::CMD_RW_ALARM9                       :
		case PROTOCOL::CMD_RW_COMP1_PRESSURE_HIGH_VALUE    :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP1_PRESSURE_LOW_VALUE     :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP2_PRESSURE_HIGH_VALUE    :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP2_PRESSURE_LOW_VALUE     :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP1_OUT_TEMP_VALUE         :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP1_IN_TEMP_VALUE          :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP2_OUT_TEMP_VALUE         :	//2024.08.19 - KWMOON
		case PROTOCOL::CMD_RW_COMP2_IN_TEMP_VALUE          :	//2024.08.19 - KWMOON
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
            if( !MakePlcFormatCommand( CMD_WRD, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
				}
				else
				{
                    PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveReadAck( CMD_WRD, PLC_DATA, 0x1 );      //1 ea
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, m_sErrMsg );
					}
				}
			}
            DoChamberResponseEmit( nId, cmd, CMD_WRD, PLC_RESPONSE_DATA, bPass, PLC_DATA, 0x1 );
		}
		break;

		case PROTOCOL::CMD_RU_ALARM_READ                   :	//READ_USER(ALARM0 ~ ALARM9) GROUP
			break;
		default:
			break;
	}
}

QString CYokogawaPlc::GetErrMsg()
{
	return m_sErrMsg;
}

//2023.05.13 - KWMOON
bool CYokogawaPlc::RequestCmd ( PROTOCOL::PLC_COMMAND cmd, stPlcResponse& rResponse, int data, uint len )
{
	if( !IsPlcConn() )
	{
		if( !PlcConnect() )
		{
			m_sErrMsg = QString("").asprintf("%s PLC is not connected!", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
			return false;
		}
	}

	QString STR_ADDR[PLC_MAX_ADDR_DATA] = {"",};
	int PLC_DATA[PLC_MAX_ADDR_DATA]  = {0x0,};
	char RAW_DATA[PLC_MAX_ADDR_DATA] = {0x0,};
	PLC_ADDRESS_MAP PLC_ADDR = ADDR_PLC_UNDEF;

	uint size = len;
	if( size > 0 )
		memcpy( RAW_DATA, (char*)&data, size );

	bool bPass = true;
	switch ( cmd )
	{
		case PROTOCOL::CMD_WB_TEMP_RUN_TRIGGER             :
		case PROTOCOL::CMD_WB_TEMP_STOP_TRIGGER            :
		case PROTOCOL::CMD_WB_AMB_MODE_TRIGGER             :
		case PROTOCOL::CMD_WB_AMB_AUTO_TRIGGER             :
		case PROTOCOL::CMD_WB_ALARM_RESET                  :
		case PROTOCOL::CMD_WB_MASK_ONOFF_TRIGGER           :
		case PROTOCOL::CMD_WB_DOOR_OPEN_TRIGGER            :
		case PROTOCOL::CMD_WB_DOOR_CLOSE_TRIGGER           :
		case PROTOCOL::CMD_WB_RACK_IN_TRIGGER              :
		case PROTOCOL::CMD_WB_RACK_OUT_TRIGGER             :
		case PROTOCOL::CMD_WB_RACK_NOP_TRIGGER             :
		case PROTOCOL::CMD_WB_BUZZER_ONOFF                 :	//2023.04.13 - KWMOON
		case PROTOCOL::CMD_WB_LAMP_MASK_ONOFF              :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_ON                  :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_ON               :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_ON                :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_RED_LAMP_BLINK               :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_YELLOW_LAMP_BLINK            :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_GREEN_LAMP_BLINK             :	//2023.05.24 - KWMOON
		case PROTOCOL::CMD_WB_ALL_LAMP_OFF                 :	//2023.05.24 - KWMOON
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			PLC_DATA[0] = data;
			if( !MakePlcFormatCommand( CMD_BWR, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				rResponse.bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				return false;
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					rResponse.bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					return false;
				}

				PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveWriteAck( CMD_BWR );
				if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
				{
					rResponse.bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					return false;
				}
			}
			rResponse = ResponseCmd( cmd, CMD_BWR, PLC_RESPONSE_PF, bPass );
		}
		break;

		case PROTOCOL::CMD_WW_WATCHDOG_TIME_SET            :
		case PROTOCOL::CMD_WW_WATCHDOG_TOGGLE_TRIGGER      :
		case PROTOCOL::CMD_WW_WATCHDOG_ENB_DSB             :
		case PROTOCOL::CMD_WW_SV_WRITE                     :
		case PROTOCOL::CMD_WW_AMB_SV_WRITE                 :
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			PLC_DATA[0] = data;
			if( !MakePlcFormatCommand( CMD_WWR, STR_ADDR, PLC_DATA, 0x1 ) ) //1 data
			{
				rResponse.bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				return false;
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					rResponse.bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					return false;
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveWriteAck( CMD_WWR );
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						rResponse.bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						return false;
					}
				}
			}
			rResponse = ResponseCmd( cmd, CMD_BWR, PLC_RESPONSE_PF, bPass );
		}
		break;

		case PROTOCOL::CMD_RB_RACK_IN                      :
		case PROTOCOL::CMD_RB_RACK_OUT                     :
		case PROTOCOL::CMD_RB_DOOR_OPEN                    :
		case PROTOCOL::CMD_RB_DOOR_CLOSE                   :
		case PROTOCOL::CMD_RB_TEMP_RUN                     :
		case PROTOCOL::CMD_RB_TEMP_STOP                    :
		case PROTOCOL::CMD_RB_AMBIENT                      :
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			if( !MakePlcFormatCommand( CMD_BRD, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				rResponse.bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				return false;
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					rResponse.bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					return false;
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveReadAck( CMD_BRD, PLC_DATA, 0x1 );
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						rResponse.bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						return false;
					}
				}
			}
			rResponse = ResponseCmd( cmd, CMD_BRD, PLC_RESPONSE_DATA, bPass, PLC_DATA, 0x1 );
		}
		break;

		case PROTOCOL::CMD_RW_PLC_VER                      :
		case PROTOCOL::CMD_RW_PV                           :
		case PROTOCOL::CMD_RW_SV                           :
		case PROTOCOL::CMD_RW_PCW_TEMP                     :
		case PROTOCOL::CMD_RW_WPS_PRESSURE                 :
		case PROTOCOL::CMD_RW_PCW_FLOW                     :
		case PROTOCOL::CMD_RW_STATUS1                      :	//I00210 ~ I00224
		case PROTOCOL::CMD_RW_STATUS2                      :	//I00226 ~ I00241
		case PROTOCOL::CMD_RW_ALARM0                       :	//I1600 ~ I1615
		case PROTOCOL::CMD_RW_ALARM1                       :
		case PROTOCOL::CMD_RW_ALARM2                       :
		case PROTOCOL::CMD_RW_ALARM3                       :
		case PROTOCOL::CMD_RW_ALARM4                       :
		case PROTOCOL::CMD_RW_ALARM5                       :
		case PROTOCOL::CMD_RW_ALARM6                       :
		case PROTOCOL::CMD_RW_ALARM7                       :
		case PROTOCOL::CMD_RW_ALARM8                       :
		case PROTOCOL::CMD_RW_ALARM9                       :
		{
			PLC_ADDR    = GetPlcCmdToAddr( cmd );
			STR_ADDR[0] = GetPlcAddrToString( PLC_ADDR );
			if( !MakePlcFormatCommand( CMD_WRD, STR_ADDR, PLC_DATA, 0x1 ) )
			{
				rResponse.bPass = false;
				m_sErrMsg = QString("").asprintf("%s PLC Command Geneartion Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
				return false;
			}
			else
			{
				PLC_COMM_RESULT_CODE result = DoSendLinePacket( m_SendByteArray );
				if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
				{
					rResponse.bPass = false;
					m_sErrMsg = QString("").asprintf("%s PLC Send Packet Error", UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
					return false;
				}
				else
				{
					PLC_EXIT_ERROR_CODE exitCode =  DoChamberReceiveReadAck( CMD_WRD, PLC_DATA, 0x1 );      //1 ea
					if( exitCode != PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
					{
						rResponse.bPass = false;
						m_sErrMsg = QString("").asprintf("%s PLC Received Packet Ack Error",  UTIL::CHAMBER::PLCCMD_STR(cmd).toLatin1().data() );
						return false;
					}
				}
			}
			rResponse = ResponseCmd( cmd, CMD_WRD, PLC_RESPONSE_DATA, bPass, PLC_DATA, 0x1 );
		}
		break;

		case PROTOCOL::CMD_RU_ALARM_READ                   :	//READ_USER(ALARM0 ~ ALARM9) GROUP
			break;
		default:
			bPass = false;
			break;
	}
	return bPass;
}

PLC_COMM_RESULT_CODE CYokogawaPlc::DoSendLinePacket(QByteArray ba)
{
	qint64 tosend = static_cast<qint64>(ba.size());
	const char* buf = ba.constData();
	if( !PlcConnect() )
	{
		if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG, tr("[SEND][ ERR] %1:%2 connection error!").arg(m_sIp).arg(m_uPort) );
		return PLC_COMM_RESULT_CODE::PLC_COMM_SOCK_ERROR;
	}
	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( buf, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			return PLC_COMM_RESULT_CODE::PLC_COMM_SOCK_ERROR;
		}
		tosend -= nSendByte;
		buf    += nSendByte;
	}
	QString sBa = "";
	sBa = QString(ba).replace(CR,"");
	sBa = QString(sBa).replace(LF,"");
	if( m_bMsgLoop ) emit SigPlcLogMessage( _NORMAL_LOG, tr("[SEND][LINE] %1").arg(sBa) );
	m_pSocket->waitForBytesWritten(5);
	m_pSocket->flush();
	return PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS;
}


////////////////////////////////////////////////////////////////////////
/// \brief CYokogawaPlc::RecvLine
/// \param baRecv
/// \return
/// \caption Read할 Size 신경쓰지 않고 Line 단위로 읽어볼 때 사용.
////////////////////////////////////////////////////////////////////////
PLC_COMM_RESULT_CODE CYokogawaPlc::DoRecvLinePacket(QByteArray& baRecv)
{
	int nRetry = 0;
	int nDelay = 500;   //500 MS
	baRecv.clear();
	if( !PlcConnect() )
	{
		if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1:%2 connection error!").arg(m_sIp).arg(m_uPort) );
		return PLC_COMM_RESULT_CODE::PLC_COMM_SOCK_ERROR;
	}

	while( nRetry < MAX_RETRY_COMM_CNT )
	{
		if ( m_pSocket->canReadLine() )
			break;
		m_pSocket->waitForReadyRead( nDelay );
		nRetry++;
	}

	if( nRetry >= MAX_RETRY_COMM_CNT )
	{
		if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] No response for %1 seconds, plc disconnect.").arg((nDelay*MAX_RETRY_COMM_CNT)/1000) );
		PlcSocketControl( PLC_OFFLINE  );
		QThread::msleep( 100 );

		PlcSocketControl( PLC_ONLINE );
		if( !PlcConnect() )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1:%2 connection error!").arg(m_sIp).arg(m_uPort) );
			return PLC_COMM_RESULT_CODE::PLC_COMM_SOCK_ERROR;
		}
		return PLC_COMM_RESULT_CODE::PLC_COMM_RECV_ERROR;
	}

	baRecv       = m_pSocket->readLine();
	QString sLog = QString(baRecv);
	sLog = sLog.replace("\r\n","");
	if( m_bMsgLoop ) emit SigPlcLogMessage( _NORMAL_LOG, tr("[RECV][LINE] %1").arg(sLog) );

	return PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
/// \brief CYokogawaPlc::DoYkgChamberReceiveWriteAck
/// \param PLC_CMD
/// \return
///
PLC_EXIT_ERROR_CODE CYokogawaPlc::DoChamberReceiveWriteAck( YKG_CMD_LIST PLC_CMD )
{
	QByteArray baReceive;

	PLC_COMM_RESULT_CODE sent = DoRecvLinePacket( baReceive );
	if( sent != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
	{
		return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
	}

	int nRecvSize      = baReceive.length();
	QString sReceive   = QString(baReceive);
	QString sWriteCmd  = m_mapCmd[PLC_CMD];
	QString sPlcCmd    = "";
	QString sExitCode  = "";
	QString sErrorCode = "";
	QString sErrorMsg  = "";

	_stExitErrorCode ExitErrorCode;
	sExitCode  = sReceive.at(2);
	sExitCode += sReceive.at(3);

	if( !m_mapExitErrCode.contains(sExitCode) )	//NOT OK
	{
		if( nRecvSize < 11)
		{
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
		}
		//ex)11ER52C1BRD
		sExitCode  = sReceive.at(2);
		sExitCode += sReceive.at(3);
		sExitCode += sReceive.at(4);
		sExitCode += sReceive.at(5);
		sErrorCode = sReceive.at(6);
		sErrorCode+= sReceive.at(7);
		sPlcCmd    = sReceive.at(8);
		sPlcCmd   += sReceive.at(9);
		sPlcCmd   += sReceive.at(10);
		if( sPlcCmd != sWriteCmd  )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] CMD SEND[%1] RECV[%2] MISMATCH").arg(sWriteCmd).arg(sPlcCmd) );
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_ER2;
		}

		//Exit Code
		if( !m_mapExitErrCode.contains(sExitCode) )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] UNDEF EXIT CODE : %1 [ERR CODE:%2]").arg(sExitCode).arg(sErrorCode) );
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
		}
		ExitErrorCode = m_mapExitErrCode[sExitCode];
		sErrorMsg = ExitErrorCode.description;

		//Detail Error Code
		if( !m_mapExitErrCode.contains(sErrorCode.at(0)) )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] UNDEF ERR CODE:%1]").arg(sErrorCode) );
			return ExitErrorCode.code;
		}
		ExitErrorCode = m_mapExitErrCode[sErrorCode.at(0)];
		sErrorMsg += ExitErrorCode.description;
		if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] MSG:%1[EXIT:%2,ERR:%3]").arg(sErrorMsg).arg(sExitCode).arg(sErrorCode) );
		return ExitErrorCode.code;
	}
	else	//OK
	{
		ExitErrorCode = m_mapExitErrCode[sExitCode];
		if( ExitErrorCode.code == PLC_EXIT_ERROR_CODE::EXIT_CODE_OK )
		{
			return PLC_EXIT_ERROR_CODE::EXIT_CODE_OK;
		}
		return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
	}
}

PLC_EXIT_ERROR_CODE CYokogawaPlc::DoChamberReceiveReadAck( YKG_CMD_LIST PLC_CMD, int* pData, int nLen )
{
	QByteArray baReceive;

	PLC_COMM_RESULT_CODE result = DoRecvLinePacket( baReceive );
	if( result != PLC_COMM_RESULT_CODE::PLC_COMM_SUCCESS )
	{
		return ERROR_CODE_USER;
	}

	int nRecvSize        = baReceive.length();
	QString sReceive     = QString(baReceive);
	QString sWriteCmd    = m_mapCmd[PLC_CMD];
	QString sPlcCmd      = "";
	QString sExitCode    = "";
	QString sErrorCode   = "";
	QString sMsgExitError= "";
	_stExitErrorCode ExitErrorCode;
	sExitCode  = sReceive.at(2);
	sExitCode += sReceive.at(3);

	if( !m_mapExitErrCode.contains(sExitCode) )
	{
		if( nRecvSize < 11)
		{
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
		}
		//ex)12ER5281BRD / 11ER52C1BRD
		sExitCode  = sReceive.at(2);
		sExitCode += sReceive.at(3);
		sExitCode += sReceive.at(4);
		sExitCode += sReceive.at(5);
		sErrorCode = sReceive.at(6);
		sErrorCode+= sReceive.at(7);
		sPlcCmd    = sReceive.at(8);
		sPlcCmd   += sReceive.at(9);
		sPlcCmd   += sReceive.at(10);
		if( sPlcCmd != sWriteCmd  )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] CMD SEND[%1] RECV[%2] MISMATCH").arg(sWriteCmd).arg(sPlcCmd) );
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_ER2;
		}

		//Exit Code
		if( !m_mapExitErrCode.contains(sExitCode) )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] UNDEF EXIT CODE : %1 [ERR CODE:%2]").arg(sExitCode).arg(sErrorCode) );
			return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
		}
		ExitErrorCode = m_mapExitErrCode[sExitCode];
		sMsgExitError = ExitErrorCode.description;

		//Detail Error Code
		if( !m_mapExitErrCode.contains(sErrorCode.at(0)) )
		{
			if( m_bMsgLoop ) emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] UNDEF ERR CODE:%1]").arg(sErrorCode) );
			return ExitErrorCode.code;
		}
		ExitErrorCode = m_mapExitErrCode[sErrorCode.at(0)];	//첫 글자
		sMsgExitError+= ExitErrorCode.description;
		emit SigPlcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] MSG:%1[EXIT:%2,ERR:%3]").arg(sMsgExitError).arg(sExitCode).arg(sErrorCode) );
		return ExitErrorCode.code;
	}
	else	//OK
	{
		ExitErrorCode = m_mapExitErrCode[sExitCode];
		if( ExitErrorCode.code == EXIT_CODE_OK )
		{
			switch( PLC_CMD )
			{
			case YKG_CMD_LIST::CMD_BRD:
			case YKG_CMD_LIST::CMD_BRR:
				for( int n=0;n<nLen;n++ )
				{
					if( sReceive.at(4+n) == '1' ) pData[0] = (1<<n);	//bit read max 12 이상 읽을 경우는 없다. 대부분 1개.
					else                          pData[0] = (0<<n);	//bit read max 12 이상 읽을 경우는 없다. 대부분 1개.
				}
				break;
			case YKG_CMD_LIST::CMD_WRD:
			case YKG_CMD_LIST::CMD_WRR:
				for( int n=0;n<nLen;n++ )
				{
					QString sData = "";
					int nOffset = n*4+4;		//4,8,C
					for( int w=0;w<4;w++ )
					{
						sData += sReceive.at(nOffset+w);
					}
					bool bOk = false;
					pData[n] = sData.toInt(&bOk,16);
                    //emit SigPlcLogMessage( _NORMAL_LOG, tr("[CHMB][WRD]SDATA:%1 RAWD:%2").arg(sData).arg(pData[n], 4, 16, QChar('0')) );
				}
				break;
			default:
				return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
			}
			return PLC_EXIT_ERROR_CODE::EXIT_CODE_OK;
		}
		return PLC_EXIT_ERROR_CODE::ERROR_CODE_USER;
	}
}

void CYokogawaPlc::DoChamberResponseEmit( uint id, PROTOCOL::PLC_COMMAND plc_cmd, YKG_CMD_LIST ykg_cmd, PLC_RESPONSE_TYPE bData, bool bPass, int* pData, int nDataLen )
{
	stPlcResponse response;

	response.bPass = bPass;
	response.plc_cmd = plc_cmd;
	response.ykg_Cmd = ykg_cmd;
	response.datalen = 0;

	if( bData == PLC_RESPONSE_TYPE::PLC_RESPONSE_DATA )
	{
		switch( ykg_cmd)
		{
		case YKG_CMD_LIST::CMD_BRD:
		case YKG_CMD_LIST::CMD_BRR:
			for( int n=0;n<nDataLen;n++ )
			{
                response.szdata[n] = static_cast<char>(pData[n]);
			}
            response.datalen = 1;
			break;
		case YKG_CMD_LIST::CMD_WRD:
		case YKG_CMD_LIST::CMD_WRR:
			for( int n=0;n<nDataLen;n++ )
			{
                response.szdata[n*2+0] = (pData[n]>>8)&0xFF;
                response.szdata[n*2+1] = (pData[n]>>0)&0xFF;
                //QString log = QString("").asprintf( "[CHMB][WRD] RDATA:0x%04X SDATA:0x%02X%02X", pData[n], response.szdata[n*2+0]&0xFF, response.szdata[n*2+1]&0xFF );
                //emit SigPlcLogMessage( _NORMAL_LOG, log );
			}
            response.datalen = 2;
			break;
		default:
			break;
		}
	}
	emit SigPlcChmbResponse ( id, response );
}

stPlcResponse CYokogawaPlc::ResponseCmd( PROTOCOL::PLC_COMMAND plc_cmd, YKG_CMD_LIST ykg_cmd, PLC_RESPONSE_TYPE bData, bool bPass, int* pData, int nDataLen )
{
	stPlcResponse response;

	response.bPass = bPass;
	response.plc_cmd = plc_cmd;
	response.ykg_Cmd = ykg_cmd;
	response.datalen = 0;

	if( bData == PLC_RESPONSE_TYPE::PLC_RESPONSE_DATA )
	{
		switch( ykg_cmd)
		{
		case YKG_CMD_LIST::CMD_BRD:
		case YKG_CMD_LIST::CMD_BRR:
			for( int n=0;n<nDataLen;n++ )
			{
				response.szdata[n] = static_cast<char>(pData[n]);
			}
			response.datalen = 1;
			break;
		case YKG_CMD_LIST::CMD_WRD:
		case YKG_CMD_LIST::CMD_WRR:
			for( int n=0;n<nDataLen;n++ )
			{
				response.szdata[n*2+0] = (pData[n]>>8)&0xFF;
				response.szdata[n*2+1] = (pData[n]>>0)&0xFF;
			}
			response.datalen = 2;
			break;
		default:
			break;
		}
	}
	return response;
}
