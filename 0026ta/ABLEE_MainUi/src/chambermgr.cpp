#include "chambermgr.hpp"

CChamberMgr::CChamberMgr(QObject *parent) : QObject(parent)
{
	InitInstance();
}

CChamberMgr::~CChamberMgr()
{
	if( m_pIpcChmb != nullptr )
	{
		m_pIpcChmb->deleteLater();
	}
	if( m_tmrIpcCon->isActive() ) m_tmrIpcCon->stop();
	if( m_tmrStatusD100->isActive() ) m_tmrStatusD100->stop();
	if( m_tmrStatusD101->isActive() ) m_tmrStatusD101->stop();
	if( m_tmrTemperature->isActive() ) m_tmrTemperature->stop();		//2024.07.23 - KWMOON

	m_tmrIpcCon->deleteLater();
	m_tmrStatusD100->deleteLater();
	m_tmrStatusD101->deleteLater();
	m_tmrTemperature->deleteLater();		//2024.07.23 - KWMOON
}

void CChamberMgr::InitInstance()
{
	m_bAutoMode     = false;
	m_bConnIpc      = false;
	m_bConnPlc      = false;
	memset( &m_stChamberStatus, 0x0, sizeof(stChamberStatus) );

	m_pIpcChmb      = new CLocalSocket( ATMAIN_NAME );
	m_tmrIpcCon     = new QTimer( this );
	m_tmrStatusD100 = new QTimer( this );
	m_tmrStatusD101 = new QTimer( this );
	m_tmrTemperature= new QTimer( this );		//2024.07.23 - KWMOON

	m_pIpcChmb->SetHostInfo( IPC_INFO::IPC_CHMBCOMM_SOCK );

	connect( this          , &CChamberMgr::SigIpcChmbCommConnect    , m_pIpcChmb  , &CLocalSocket::SlotConnectToHost     );
	connect( this          , &CChamberMgr::SigIpcChmbCommSendPacket , m_pIpcChmb  , &CLocalSocket::SlotSendPacket        );

	connect( m_pIpcChmb    , &CLocalSocket::SigIpcConnectionStatus   , this  , &CChamberMgr::SlotIpcChmbCommConnStatus   );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcRecvPacket         , this  , &CChamberMgr::SlotIpcChmbCommRecvPacket   );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcError              , this  , &CChamberMgr::SlotIpcChmbCommError        );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcLogMessage         , this  , &CChamberMgr::SlotIpcChmbCommLogMessage   );

	connect(m_tmrIpcCon     , &QTimer::timeout, this, &CChamberMgr::SlotTimeoutIpcCon      );
	connect(m_tmrStatusD100 , &QTimer::timeout, this, &CChamberMgr::SlotTimeoutStatusD100  );
	connect(m_tmrStatusD101 , &QTimer::timeout, this, &CChamberMgr::SlotTimeoutStatusD101  );
	connect(m_tmrTemperature, &QTimer::timeout, this, &CChamberMgr::SlotTimeoutTemperature );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChamberMgr::SlotStartMgr
/// \param mode
/// \details signal from mainui - auto mode
void CChamberMgr::SlotStartMgr(bool mode)
{
	m_bAutoMode = mode;
	if( !mode )
	{
		if( m_tmrIpcCon->isActive() ) m_tmrIpcCon->stop();
		if( m_tmrStatusD100->isActive() ) m_tmrStatusD100->stop();
		if( m_tmrStatusD101->isActive() ) m_tmrStatusD101->stop();
		if( m_tmrTemperature->isActive()) m_tmrTemperature->stop();
	}
	else
	{
		emit SigIpcChmbCommConnect();
		m_tmrIpcCon->start( _TIME_CHMB_COMM );
	}
}

void CChamberMgr::SlotTimeoutIpcCon()
{
	if( !m_bAutoMode ) return;
	if( !m_pIpcChmb->IsConn() )
	{
		m_pIpcChmb->SetHostInfo( IPC_INFO::IPC_CHMBCOMM_SOCK );
		emit SigIpcChmbCommConnect();
	}
}

void CChamberMgr::SlotTimeoutStatusD100()
{
	GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1 );
}

void CChamberMgr::SlotTimeoutStatusD101()
{
	GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2 );
}

//2024.07.23 - KWMOON
void CChamberMgr::SlotTimeoutTemperature()
{
	GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_SV );
	GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_PV );
}

void CChamberMgr::SlotIpcChmbCommLogMessage (LOG_TYPE t, QString msg )
{
	emit SigControllerChmbMsg( t, msg );
}

void CChamberMgr::SlotIpcChmbCommError ( QLocalSocket::LocalSocketError err, QString msg )
{
	(void)err;
	emit SigControllerChmbMsg( _ERROR_LOG, msg );
}

void CChamberMgr::SlotIpcChmbCommConnStatus(bool conn)
{
	if( conn )
	{
		//emit SigToMainuiLogMessage( _NORMAL_LOG, "CHAMBERMGR : IPC CONNNECTED!" );
	}
	else
	{
		m_stChamberStatus.bChmbCon = false;
		emit SigToMainuiChmbStatus( m_stChamberStatus );
		//emit SigToMainuiLogMessage( _NORMAL_LOG, "CHAMBERMGR : IPC DISCONNNECTED!" );
	}
	m_bConnIpc = conn;
}

void CChamberMgr::SlotIpcChmbCommRecvPacket( stIpcElement emt )
{
	PROTOCOL::PLC_COMMAND cmd = static_cast<PROTOCOL::PLC_COMMAND>( emt.ipcHeader.cmd );
	unsigned char szBuf[32]={0x0,};
	uint datalen = emt.ipcHeader.datalen;
	int error    = emt.ipcHeader.error;

	memset( szBuf, 0x0, sizeof(szBuf) );
	memcpy( szBuf, emt.pData, datalen );

	switch ( cmd )
	{
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET:
		case PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TIME_SET:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB:
		case PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE:
		case PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE:
		{

		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1:
		{
			uint nD100 = 0x0;
			if( datalen == 2 )
			{
				nD100 |= (static_cast<int>(szBuf[0])<<8);
				nD100 |= (static_cast<int>(szBuf[1])<<0);
			}
			m_stChamberStatus.bMaskOff   = (( nD100 >> CHMB_STAT1_MASK_OFF) &0x1 );
			m_stChamberStatus.bMaskOn    = (( nD100 >> CHMB_STAT1_MASK_ON) &0x1 );
			m_stChamberStatus.bDoorOpen  = (( nD100 >> CHMB_STAT1_DOOR_OPEN ) &0x1 );
			m_stChamberStatus.bDoorClose = (( nD100 >> CHMB_STAT1_DOOR_CLOSE ) &0x1 );
			m_stChamberStatus.bUpperRackDetect  = (( nD100 >> CHMB_STAT1_UPPER_RACK_DETECT ) &0x1 );
			m_stChamberStatus.bBottomRackDetect = (( nD100 >> CHMB_STAT1_BOTTOM_RACK_DETECT ) &0x1 );
			m_stChamberStatus.bUpperRackDetect  = (( nD100 >> CHMB_STAT1_UPPER_RACK_DETECT ) &0x1 );
			m_stChamberStatus.bRackInComplete = (( nD100 >> CHMB_STAT1_RACK_IN_COMPLETE ) &0x1 );
			m_stChamberStatus.bRackOutComplete = (( nD100 >> CHMB_STAT1_RACK_OUT_COMPLETE ) &0x1 );
			m_stChamberStatus.bEqReadyStatus = (( nD100 >> CHMB_STAT1_EQ_READY_STATUS ) &0x1 );
			m_stChamberStatus.bEqAbortStatus = (( nD100 >> CHMB_STAT1_EQ_ABORT_STATUS ) &0x1 );
			m_stChamberStatus.bEqAgvComplete = (( nD100 >> CHMB_STAT1_AVG_JOB_COMPLETE ) &0x1 );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2:
		{
			uint nD101 = 0x0;
			if( datalen == 2 )
			{
				nD101 |= (static_cast<int>(szBuf[0])<<8);
				nD101 |= (static_cast<int>(szBuf[1])<<0);
			}
			m_stChamberStatus.bRun = (( nD101 >> CHMB_STAT2_RUN ) &0x1 );
			m_stChamberStatus.bStop = (( nD101 >> CHMB_STAT2_STOP ) &0x1 );
			m_stChamberStatus.bAmb = (( nD101 >> CHMB_STAT2_AMB ) &0x1 );
			if( 0 )
			{
				qDebug() << "PLC CONN : " << m_stChamberStatus.bChmbCon;
				qDebug() << "RUN : " << m_stChamberStatus.bRun;
				qDebug() << "STOP : " << m_stChamberStatus.bStop;
				qDebug() << "AMB : " << m_stChamberStatus.bAmb;
				qDebug() << "MASK OFF : " << m_stChamberStatus.bMaskOff;
				qDebug() << "MASK ON : " << m_stChamberStatus.bMaskOn;
				qDebug() << "DOOR OPEN : " << m_stChamberStatus.bDoorOpen;
				qDebug() << "DOOR CLOSE : " << m_stChamberStatus.bDoorClose;
				qDebug() << "UPPER RACK : " << m_stChamberStatus.bUpperRackDetect;
				qDebug() << "BOTTOM RACK : " << m_stChamberStatus.bBottomRackDetect;
				qDebug() << "RACK  IN: " << m_stChamberStatus.bRackInComplete;
				qDebug() << "RACK OUT: " << m_stChamberStatus.bRackOutComplete;
				qDebug() << "EQ READY : " << m_stChamberStatus.bEqReadyStatus;
				qDebug() << "EQ ABORT : " << m_stChamberStatus.bEqAbortStatus;
				qDebug() << "EQ COMPLETE : " << m_stChamberStatus.bEqAgvComplete;
			}
		}
		break;

		//2023.07.23 - KWMOON
		case PROTOCOL::PLC_COMMAND::CMD_RW_PV:
		{
			uint nPv = 0x0;
			short uPv = 0;
			double dPv = 0.0;
			if( datalen == 2 )
			{
				nPv  = (static_cast<int>(szBuf[0])<<8)&0xFF00;
				nPv |= (static_cast<int>(szBuf[1])<<0);
				uPv  = static_cast<short>(nPv&0xFFFF);
				dPv = static_cast<double>(uPv) / 10.0;
			}
			m_stChamberStatus.dPV = dPv;
			emit SigToMainuiChmbStatus( m_stChamberStatus );
		}
		break;

		//2023.07.23 - KWMOON
		case PROTOCOL::PLC_COMMAND::CMD_RW_SV:
		{
			uint nSv = 0x0;
			short uSv = 0;
			double dSv = 0.0;
			if( datalen == 2 )
			{
				nSv  = (static_cast<int>(szBuf[0])<<8);
				nSv |= (static_cast<int>(szBuf[1])<<0);
				uSv  = static_cast<short>(nSv&0xFFFF);
				dSv = static_cast<double>(uSv) / 10.0;
			}
			m_stChamberStatus.dSV = dSv;
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_SYS_PLC_CONN:
		{
			if( error == 0x1 )
				m_bConnPlc = false;
			else
				m_bConnPlc = true;

			if( m_bConnIpc && m_bConnPlc )
			{
				m_stChamberStatus.bChmbCon = true;
				if( !m_tmrStatusD100->isActive() ) m_tmrStatusD100->start( _TIME_CHMB_STAT_D100 );
				if( !m_tmrStatusD101->isActive() ) m_tmrStatusD101->start( _TIME_CHMB_STAT_D101 );
				if( !m_tmrTemperature->isActive() ) m_tmrTemperature->start( _TIME_CHMB_TEMPERATURE );
			}
			else
			{
				m_stChamberStatus.bChmbCon = false;
				m_tmrStatusD100->stop();
				m_tmrStatusD101->stop();
				m_tmrTemperature->stop();
				emit SigToMainuiLogMessage( _ERROR_LOG, "[ ERR] MainUI chamber connection error!" );
				emit SigToMainuiChmbStatus( m_stChamberStatus );
			}
		}
		break;

		default:
			break;
	}

	if( emt.ipcHeader.datalen > 0 )
	{
		if( emt.pData )
		{
			delete[] emt.pData;
			emt.pData = nullptr;
		}
	}
}

bool CChamberMgr::GetChmbStatus( PROTOCOL::PLC_COMMAND cmd )
{
	if( !m_bAutoMode ) return false;
	if( !m_bConnPlc )  return false;

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATMAIN_UI;
	head.datalen = 0;
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader);
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer, (char*)&head, size );
	emit SigIpcChmbCommSendPacket( pSendBuffer, size );
	return true;
}

bool CChamberMgr::SetChmbBitWrite ( PROTOCOL::PLC_COMMAND cmd, int bit )
{
	if( !m_bAutoMode ) return false;
	if( !m_bConnPlc )  return false;

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATMAIN_UI;
	head.datalen = 0x1; //bit -> 1byte
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader) + head.datalen;
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer                    , (char*)&head, sizeof(stIpcHeader) );
	memcpy( pSendBuffer+sizeof(stIpcHeader), (char*)&bit , head.datalen        );
	emit SigIpcChmbCommSendPacket( pSendBuffer, size );
	return true;
}

bool CChamberMgr::SetChmbWordWrite( PROTOCOL::PLC_COMMAND cmd, int word )
{
	if( !m_bAutoMode ) return false;
	if( !m_bConnPlc )  return false;

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATCHMB_UI;
	head.datalen = 0x2; //word -> 2byte
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader) + head.datalen;
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer                    , (char*)&head, sizeof(stIpcHeader) );
	memcpy( pSendBuffer+sizeof(stIpcHeader), (char*)&word, head.datalen        );
	emit SigIpcChmbCommSendPacket( pSendBuffer, size );
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChamberMgr::SlotMainuiChmbControl
/// \param cmd
/// \param data
/// \details from mainui(hsms manager) chamber control command
void CChamberMgr::SlotFromMainuiChmbControl( PROTOCOL::PLC_COMMAND cmd, int data )
{
	bool bWritten = false;
	switch( cmd )
	{
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET:
		case PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_BUZZER_ONOFF:
		case PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF:		//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_ON:			//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_ON:		//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_ON:		//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_BLINK:		//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_BLINK:	//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_BLINK:	//2024.08.19 - KWMOON(TOWER LAMP)
		case PROTOCOL::PLC_COMMAND::CMD_WB_ALL_LAMP_OFF:		//2024.08.19 - KWMOON(TOWER LAMP)
			bWritten = SetChmbBitWrite( cmd, data );
			break;
		case PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE:
		case PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE:
			bWritten = SetChmbWordWrite( cmd, data );
			break;
		default:
			bWritten = false;
			break;
	}
	emit SigToMainuiChmbControlRes( cmd, bWritten );
}
