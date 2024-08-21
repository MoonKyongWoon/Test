#include "devctrlmgr.hpp"

CDevCtrlMgr::CDevCtrlMgr(QObject *parent) : QObject(parent), m_pTcpSvr(nullptr)
{
	m_bHasIp = false;
	m_vecTcpClient.clear();
	m_pTmrKeepAlive = nullptr;
	//2023.08.10 - KWMOON
	memset( m_bConCpu, 0x0, sizeof(m_bConCpu) );
}

CDevCtrlMgr::~CDevCtrlMgr()
{
	if( m_pTmrKeepAlive != nullptr )
	{
		if( m_pTmrKeepAlive->isActive() )
			m_pTmrKeepAlive->stop();
	}
	if( m_pTcpSvr->isListening() )
	{
		m_pTcpSvr->close();
	}
	delete m_pTcpSvr;
	m_pTcpSvr = nullptr;

	for( CTcpClient* clients : m_vecTcpClient )
	{
		clients->CloseSocket();
		clients->deleteLater();
	}
}

//rfu
void CDevCtrlMgr::SetLocalCommIp( QString sip )
{
	m_sIp = sip;
	m_bHasIp = true;
}

void CDevCtrlMgr::SetSlotInfo( stSlotInfo slotinfo )
{
	memcpy ( &m_stSlotInfo, &slotinfo, sizeof(stSlotInfo) );
}

bool CDevCtrlMgr::RunTcpServer()
{
	bool bListen = true;
	if( m_pTcpSvr == nullptr )
		m_pTcpSvr = new CTcpServer();

	if( m_pTcpSvr->isListening() ) return bListen;

	const int RETRY_COUNT = 10;
	int retry = 0;
	do
	{
		if( !m_pTcpSvr->listen( QHostAddress::AnyIPv4, TCP_PORT::EWS_TESTMGR_PGM_CTRL_PORT ) )
		{
			m_pTcpSvr->close();
			bListen = false;
		}
		else
		{
			bListen = true;
			connect( m_pTcpSvr, &CTcpServer::SigIncommingConnection, this     , &CDevCtrlMgr::SlotTcpConnected );
			break;
		}
		usleep( 1000 );
	}while( retry++ < RETRY_COUNT );

	if( retry >= RETRY_COUNT )
	{
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG , tr("PGM CTRL Server(%1) Listen NG!").arg(TCP_PORT::EWS_TESTMGR_PGM_CTRL_PORT) );
	}
	else
	{
		emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("PGM CTRL Server(%1) Listen OK!").arg(TCP_PORT::EWS_TESTMGR_PGM_CTRL_PORT) );
	}
	return bListen;
}

bool CDevCtrlMgr::InitTimer()
{
	if( m_pTmrKeepAlive == nullptr )
		m_pTmrKeepAlive = new QTimer(this);
	if( m_pTmrKeepAlive != nullptr )
	{
		connect( m_pTmrKeepAlive, &QTimer::timeout, this, &CDevCtrlMgr::SlotTimeoutKeepAlive );
		m_pTmrKeepAlive->start( PROTOCOL::EN_KEEP_ALIVE::EN_COMM_PERIOD * 1000 );
		emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("PGM CTRL KeepAlive Check Start : %1 sec").arg(PROTOCOL::EN_KEEP_ALIVE::EN_COMM_PERIOD) );
	}
	return true;
}

//2023.08.10 - KWMOON
void CDevCtrlMgr::SlotTimeoutKeepAlive()
{
	QDateTime dtCurrent = QDateTime::currentDateTime();
	for( int s=0;s<MAX_SLOT;s++ )
	{
		for( int r=0;r<MAX_SYS_CPU;r++ )
		{
			if( m_bConCpu[s][r] )
			{
				qint64 uGap = dtCurrent.secsTo( m_dtKeepAlive[s][r] );
				qint64 uGapAbs = std::abs( uGap );
				if( uGapAbs > PROTOCOL::EN_KEEP_ALIVE::EN_FAULT_PERIOD )
				{
					m_bConCpu[s][r] = false;
					//DisconnectSlotCpu( s, r );
					//emit SigDevKeepAliveError( s, r );
					emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
										   tr("PGM CTRL Slot%1-%2 keep alive period over than %3 secs")
										   .arg(s+1).arg(r+1).arg(PROTOCOL::EN_KEEP_ALIVE::EN_FAULT_PERIOD) );
				}
			}
		}
	}
}

void CDevCtrlMgr::SlotRunService()
{
	RunTcpServer();
	InitTimer();
}

void CDevCtrlMgr::SlotTcpConnected(quintptr handle)
{
	CTcpClient* pClient = new CTcpClient(handle);
	QString client_ip = pClient->GetHostIp().trimmed();

	bool bValidIp = false;
	for( int i=0;i<m_stSlotInfo.SlotCpu.nMaxSlot;i++ )
	{
		stSlotIp ip = m_stSlotInfo.SlotIp[i];
		QString ip1 = QString(ip.szCpu1Ip).trimmed();
		QString ip2 = QString(ip.szCpu2Ip).trimmed();
		if( ip1 == client_ip )
		{
			if( !ip.bMask )
			{
				bValidIp = true;
				break;
			}
			//error mask
		}
		if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
		{
			if( ip2 == client_ip )
			{
				if( !ip.bMask )
				{
					bValidIp = true;
				}
				break;
			}
		}
	}
	if( bValidIp )
	{
		connect( pClient, &CTcpClient::SigStatusChanged  , this   , &CDevCtrlMgr::SlotTcpStatusChanged  );
		connect( pClient, &CTcpClient::SigReceivePacket  , this   , &CDevCtrlMgr::SlotTcpRecvPacket     );
		connect( pClient, &CTcpClient::SigLogMessage     , this   , &CDevCtrlMgr::SlotTcpLogMesage      );
		//connect( this   , &CDevCtrlMgr::SigDevSendPacket , pClient, &CTcpClient::SlotSendPacket         );
		m_vecTcpClient.push_back( pClient );
		emit SigDevLogMessage( _NORMAL_LOG, tr("PGM CTGRL %1 connected...").arg(client_ip) );
	}
	else
	{
		pClient->CloseSocket();
		pClient->deleteLater();
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG,
						 tr("Undefined PGM CTRL %1 connected...").arg(client_ip) );
	}
}

//2023.08.10 - KWMOON
bool CDevCtrlMgr::DisconnectSlotCpu( uint slot, uint runs )
{
	for( CTcpClient* pClient : m_vecTcpClient )
	{
		uint s = 0;
		uint r = 0;
		pClient->GetUniqueId( s, r );
		if( (slot == s) && (runs == r) )
		{
			pClient->CloseSocket();
			break;
		}
	}
	return true;
}

void CDevCtrlMgr::SlotTcpStatusChanged(bool bConn)
{
	if( !bConn )
	{
		CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
		if( pClient )
		{
			uint slot = 0, runner = 0;
			QString client_ip = pClient->GetHostIp().trimmed();
			pClient->GetUniqueId( slot, runner );
			QString title = tr("[%1][S#%2-R#%3]").arg("PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1);
			emit SigDevLogMessage( _NORMAL_LOG, tr("PGM CTRL %2 disconnected...").arg(client_ip) );
			emit SigDevStatusChanged( slot, runner, false );			
			m_vecTcpClient.removeOne( pClient );
			pClient->deleteLater();
			m_bConCpu[slot][runner] = false;		//2023.08.10 - KWMOON
		}
	}
}

//receive from multiple cpu programs
void CDevCtrlMgr::SlotTcpRecvPacket(stTcpElement tcpelement)
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( tcpelement.tcpHeader.cmd );

	uint slot = tcpelement.tcpHeader.slot;
	uint runs = tcpelement.tcpHeader.runner;

	switch ( cmd )
	{
		//case PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_START_ACK:
		//case PROTOCOL::TCP_COMMAND::CMD_TEST_START_ACK:
		//case PROTOCOL::TCP_COMMAND::CMD_ITEM_TEST_START_ACK:
		//case PROTOCOL::TCP_COMMAND::CMD_TEST_STOP_ACK:
		//{
		//	uint slot = tcpelement.tcpHeader.slot;
		//	uint runs = tcpelement.tcpHeader.runner;
		//	QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
		//	emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
		//	emit SigDevRecvPacket( tcpelement );
		//}
		//break;

		case PROTOCOL::TCP_COMMAND::CMD_SLOT_RUN_NO_SEND:
		{
			CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
			if( pClient )
			{
				pClient->SetUniqueId( slot, runs );
				QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
				emit SigDevStatusChanged( slot, runs, true );
				emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,  tr("%1 %2 ").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				m_bConCpu[slot][runs] = true;		//2023.08.10 - KWMOON
			}
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_KEEP_ALIVE:
		{
			CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
			if( pClient )
			{
				pClient->SetUniqueId( slot, runs );
                //QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
                //emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,  tr("%1 %2 ").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				//m_bConCpu[slot][runs] = true;								//2023.08.10 - KWMOON
				m_dtKeepAlive[slot][runs] = QDateTime::currentDateTime();	//2023.08.10 - KWMOON
			}
		}
		break;

		default:
		{
			uint slot = tcpelement.tcpHeader.slot;
			uint runs = tcpelement.tcpHeader.runner;
			QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
			emit SigDevRecvPacket( tcpelement );
			emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
		}
		break;
	}
}

void CDevCtrlMgr::SlotTcpLogMesage( LOG_TYPE type, QString log )
{
	QString sFrom = "<-PGM";
	QString msg = tr("[%1] MSG : %2").arg(sFrom, 7, QChar(' ') ).arg(log);
	emit SigDevLogMessage( type, msg );
}

//from ui or testmgr to device programs ctrl socket ....
void CDevCtrlMgr::SlotDevMgrSendPacket( stTcpElement element )
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(element.tcpHeader.cmd);
	uint nslots = element.tcpHeader.slot;
	uint nSize  = sizeof(stTcpHeader) + element.tcpHeader.datalen;
	char* pData = new char[nSize];
	memcpy( pData, &element.tcpHeader, sizeof(stTcpHeader) );
	if( element.tcpHeader.datalen > 0 )
	{
		memcpy( pData+sizeof(stTcpHeader), element.pData, element.tcpHeader.datalen );
		delete[] element.pData;
	}
	QString title = tr("[%1] %2[0x%3]").arg("<-TESTMGR", 10, QChar(' ')).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(nslots,2,16,QChar('0'));
	emit SigDevLogMessage( _NORMAL_LOG,
						   tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	for( CTcpClient* client : m_vecTcpClient )
	{
		uint slot = 0, runner = 0;
		if( !client->IsHaveId() ) continue;
		client->GetUniqueId( slot, runner );
		if( (1<<slot) & nslots )
		{
			if( !client->SendPacket( pData, nSize ) )
			{
				client->CloseSocket();
				QString title = tr("[%1][S#%2-R#%3]").arg("->PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1);
				emit SigDevLogMessage( _NORMAL_LOG,
									   tr("%1 %2 error!").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				continue;
			}
			QString title = tr("[%1][S#%2-R#%3]").arg("->PGMCTRL", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1);
			emit SigDevLogMessage( _NORMAL_LOG,
								   tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
		}
	}
	if( pData != nullptr )
	{
		delete[] pData;
		pData = nullptr;
	}
}
