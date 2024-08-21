#include "devlogmgr.hpp"

CDevLogMgr::CDevLogMgr(QObject *parent) : QObject(parent)
{
	m_pTcpSvr = nullptr;
	m_bHasIp = false;
	m_vecTcpClient.clear();
	InitSlotLogger();
}

CDevLogMgr::~CDevLogMgr()
{
	//2023.08.10 - KWMOON
	for( int i=0;i<MAX_SLOT;i++ )
	{
		for(int r=0;r<MAX_SYS_CPU;r++ )
		{
			if( m_pLoggerSlot[i][r] )
			{
				delete m_pLoggerSlot[i][r];
				m_pLoggerSlot[i][r] = nullptr;
			}
		}
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
void CDevLogMgr::SetLocalCommIp( QString sip )
{
	m_sIp = sip;
	m_bHasIp = true;
}

void CDevLogMgr::SetSlotInfo( stSlotInfo slotinfo )
{
	memcpy ( &m_stSlotInfo, &slotinfo, sizeof(stSlotInfo) );
}

//2023.08.10 - KWMOON
void CDevLogMgr::InitSlotLogger()
{
	for( int i=0;i<MAX_SLOT;i++ )
	{
		for(int r=0;r<MAX_SYS_CPU;r++ )
		{
			int type = PROTOCOL::_ATSLOT01 + i;
			m_pLoggerSlot[i][r] = new CLogger( static_cast<PROTOCOL::SOURCE_TYPE>(type), r );
			connect( this, &CDevLogMgr::SigSaveSlotLog, m_pLoggerSlot[i][r], &CLogger::SlotLogSlotMessage );
		}
	}
}

bool CDevLogMgr::RunTcpLogServer()
{
	bool bListen = true;
	if( m_pTcpSvr == nullptr )
		m_pTcpSvr = new CTcpServer();

	if( m_pTcpSvr->isListening() ) return bListen;

	const int RETRY_COUNT = 10;
	int retry = 0;
	do
	{
		if( !m_pTcpSvr->listen( QHostAddress::AnyIPv4, TCP_PORT::EWS_TESTMGR_PGM_LOG_PORT ) )
		{
			m_pTcpSvr->close();
			bListen = false;
		}
		else
		{
			bListen = true;
			connect( m_pTcpSvr, &CTcpServer::SigIncommingConnection, this, &CDevLogMgr::SlotTcpConnected );
			break;
		}
		usleep( 1000 );
	}while( retry++ < RETRY_COUNT );

	if( retry >= RETRY_COUNT )
	{
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG , tr("PGM LOG Server(%1) Listen NG!").arg(TCP_PORT::EWS_TESTMGR_PGM_LOG_PORT) );
	}
	else
	{
		emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("PGM LOG Server(%1) Listen OK!").arg(TCP_PORT::EWS_TESTMGR_PGM_LOG_PORT) );
	}
	return bListen;
}

void CDevLogMgr::SlotRunService()
{
	RunTcpLogServer();
}

void CDevLogMgr::SlotTcpConnected(quintptr handle)
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
		connect( pClient, &CTcpClient::SigStatusChanged  , this   , &CDevLogMgr::SlotTcpStatusChanged  );
		connect( pClient, &CTcpClient::SigReceivePacket  , this   , &CDevLogMgr::SlotTcpRecvPacket     );
		connect( pClient, &CTcpClient::SigLogMessage     , this   , &CDevLogMgr::SlotTcpLogMesage      );
		m_vecTcpClient.push_back( pClient );
		emit SigDevLogMessage( _NORMAL_LOG, tr("PGM LOG %1 connected...").arg(client_ip) );
	}
	else
	{
		pClient->CloseSocket();
		pClient->deleteLater();
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG,
						 tr("Undefined PGM LOG %1 connected...").arg(client_ip) );
	}
}

void CDevLogMgr::SlotTcpStatusChanged(bool bConn)
{
	if( !bConn )
	{
		CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
		if( pClient )
		{
			uint slot = 0, runner = 0;
			QString client_ip = pClient->GetHostIp().trimmed();
			pClient->GetUniqueId( slot, runner );

			emit SigDevLogMessage( _NORMAL_LOG, tr("PGM LOG %1 disconnected...").arg(client_ip) );
			emit SigDevStatusChanged( slot, runner, false );
			m_vecTcpClient.removeOne( pClient );
			pClient->deleteLater();
		}
	}
}

//receive from multiple cpu programs
void CDevLogMgr::SlotTcpRecvPacket(stTcpElement tcpelement)
{
	QString sFrom = "<-DEVPGM";
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( tcpelement.tcpHeader.cmd );
	uint slot = tcpelement.tcpHeader.slot;
	uint runs = tcpelement.tcpHeader.runner;
	uint datalen = tcpelement.tcpHeader.datalen;
	switch ( cmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_SLOT_RUN_NO_SEND:
		{
			CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
			if( pClient )
			{
				QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMLOG", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
				pClient->SetUniqueId( slot, runs );
				emit SigDevStatusChanged( slot, runs, true );
				emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
									   tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
			}
		}
		break;

		//2023.08.10 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_LOG_PRINT:
		{
			char* pData = NULL;
			QString log = "";
			if( datalen > 0 )
			{
				pData = new char[datalen+1];
				memset( pData, 0x0, datalen+1 );
				memcpy( pData, tcpelement.pData, datalen );
				pData[datalen]='\0';
				log = QString(pData);				
				emit SigSaveSlotLog( slot, runs, _NORMAL_LOG, tr("%1").arg(log) );

				//fprintf( stdout, "***** [LOG MGR Size:%d] ***** \n%s", datalen, log.toStdString().c_str() ); fflush(stdout);
			}
		}
		break;

		default:
		{
			uint slot = tcpelement.tcpHeader.slot;
			uint runs = tcpelement.tcpHeader.runner;
			QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMLOG", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
			emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
								   tr("%1 %2 is invalid log socket cmd [DataLen:%3]").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(tcpelement.tcpHeader.datalen) );
		}
		break;
	}
}

void CDevLogMgr::SlotTcpLogMesage( LOG_TYPE type, QString log )
{
	QString sFrom = "<-PGM";
	QString msg = tr("[%1] MSG : %2").arg(sFrom, 7, QChar(' ') ).arg(log);
	emit SigDevLogMessage( type, msg );
}

