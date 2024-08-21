#include "devdatamgr.hpp"

CDevDataMgr::CDevDataMgr(QObject *parent) : QObject(parent), m_pTcpSvr(nullptr)
{
	m_bHasIp = false;
	m_vecTcpClient.clear();
}

CDevDataMgr::~CDevDataMgr()
{
    //2023.08.10 - KWMOON
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
void CDevDataMgr::SetLocalCommIp( QString sip )
{
	m_sIp = sip;
	m_bHasIp = true;
}

void CDevDataMgr::SetSlotInfo( stSlotInfo slotinfo )
{
	memcpy ( &m_stSlotInfo, &slotinfo, sizeof(stSlotInfo) );
}

bool CDevDataMgr::RunTcpDataServer()
{
	bool bListen = true;
	if( m_pTcpSvr == nullptr )
		m_pTcpSvr = new CTcpServer();

	if( m_pTcpSvr->isListening() ) return bListen;

	const int RETRY_COUNT = 10;
	int retry = 0;
	do
	{
		if( !m_pTcpSvr->listen( QHostAddress::AnyIPv4, TCP_PORT::EWS_TESTMGR_PGM_DATA_PORT ) )
		{
			m_pTcpSvr->close();
			bListen = false;
		}
		else
		{
			bListen = true;
			connect( m_pTcpSvr, &CTcpServer::SigIncommingConnection, this     , &CDevDataMgr::SlotTcpConnected );
			break;
		}
		usleep( 1000 );
	}while( retry++ < RETRY_COUNT );

	if( retry >= RETRY_COUNT )
	{
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG , tr("PGM DATA Server(%1) Listen NG!").arg(TCP_PORT::EWS_TESTMGR_PGM_DATA_PORT) );
	}
	else
	{
		emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG, tr("PGM DATA Server(%1) Listen OK!").arg(TCP_PORT::EWS_TESTMGR_PGM_DATA_PORT) );
	}
	return bListen;
}

void CDevDataMgr::SlotRunService()
{
	RunTcpDataServer();
}

void CDevDataMgr::SlotTcpConnected(quintptr handle)
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
		connect( pClient, &CTcpClient::SigStatusChanged  , this   , &CDevDataMgr::SlotTcpStatusChanged  );
		connect( pClient, &CTcpClient::SigReceivePacket  , this   , &CDevDataMgr::SlotTcpRecvPacket     );
		connect( pClient, &CTcpClient::SigLogMessage     , this   , &CDevDataMgr::SlotTcpLogMesage      );
		//connect( this   , &CDevCtrlMgr::SigDevSendPacket , pClient, &CTcpClient::SlotSendPacket         );
		m_vecTcpClient.push_back( pClient );
		emit SigDevLogMessage( _NORMAL_LOG, tr("PGM DATA %1 connected...").arg(client_ip) );
	}
	else
	{
		pClient->CloseSocket();
		pClient->deleteLater();
		emit SigDevLogMessage( LOG_TYPE::_ERROR_LOG,
						 tr("Undefined PGM DATA %1 connected...").arg(client_ip) );
	}
}

void CDevDataMgr::SlotTcpStatusChanged(bool bConn)
{
	if( !bConn )
	{
		CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
		if( pClient )
		{
			uint slot = 0, runner = 0;
			QString client_ip = pClient->GetHostIp().trimmed();
			pClient->GetUniqueId( slot, runner );

			emit SigDevLogMessage( _NORMAL_LOG, tr("PGM DATA %1 disconnected...").arg(client_ip) );
			emit SigDevStatusChanged( slot, runner, false );
			m_vecTcpClient.removeOne( pClient );
			pClient->deleteLater();
		}
	}
}

//receive from multiple cpu programs
void CDevDataMgr::SlotTcpRecvPacket(stTcpElement tcpelement)
{
	//QString sFrom = "<-DEVPGM";
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( tcpelement.tcpHeader.cmd );
    uint slot = tcpelement.tcpHeader.slot;
    uint runs = tcpelement.tcpHeader.runner;
	//uint datalen = tcpelement.tcpHeader.datalen;
	switch ( cmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_SLOT_RUN_NO_SEND:
		{
			CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
			if( pClient )
			{
				QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMDATA", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
				pClient->SetUniqueId( slot, runs );
				emit SigDevStatusChanged( slot, runs, true );
				emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
									   tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
			}
		}
		break;

		//2023.08.10 - KWMOON -->2023.08.13 - KWMOON(move to logmgr)
		//case PROTOCOL::TCP_COMMAND::CMD_LOG_PRINT:
		//{
		//    char* pData = NULL;
		//    QString log = "";
		//    if( datalen > 0 )
		//    {
		//        pData = new char[datalen+1];
		//        memset( pData, 0x0, datalen+1 );
		//        memcpy( pData, tcpelement.pData, datalen );
		//        pData[datalen]='\0';
		//        log = QString(pData);
		//        emit SigSaveSlotLog( slot, runs, _NORMAL_LOG, tr("%1").arg(log) );
		//    }
		//}
		//break;

		//case PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_RESULT:
		//case PROTOCOL::TCP_COMMAND::CMD_TEST_STEP_RESULT:
		//case PROTOCOL::TCP_COMMAND::CMD_TEST_CUM_RESULT:
		//case PROTOCOL::TCP_COMMAND::CMD_TEST_END_RESULT:
		//case PROTOCOL::TCP_COMMAND::CMD_TEMP_SET_REQ:
		//case PROTOCOL::TCP_COMMAND::CMD_PGM_ITEM_LISTUP:
		//case PROTOCOL::TCP_COMMAND::CMD_LOG_PRINT:
		//case PROTOCOL::TCP_COMMAND::CMD_VAR_SET_DEVICE:
		//{
		//	uint slot = tcpelement.tcpHeader.slot;
		//	uint runs = tcpelement.tcpHeader.runner;
		//	QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMDATA", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
		//	emit SigDevRecvPacket( tcpelement );
		//	emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
		//						   tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
		//}
		//break;

		default:
		{
			uint slot = tcpelement.tcpHeader.slot;
			uint runs = tcpelement.tcpHeader.runner;
			QString title = tr("[%1][S#%2-R#%3]").arg("<-PGMDATA", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runs+1);
			emit SigDevLogMessage( LOG_TYPE::_NORMAL_LOG,
								   tr("%1 %2 [DataLen:%3]").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(tcpelement.tcpHeader.datalen) );
			emit SigDevRecvPacket( tcpelement );
		}
		break;
	}
}

void CDevDataMgr::SlotTcpLogMesage( LOG_TYPE type, QString log )
{
	QString sFrom = "<-PGM";
	QString msg = tr("[%1] MSG : %2").arg(sFrom, 7, QChar(' ') ).arg(log);
	emit SigDevLogMessage( type, msg );
}

//from mgr/ui to device pgm data/log socket
void CDevDataMgr::SlotDevDataSendPacket( stTcpElement element )
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(element.tcpHeader.cmd);
	uint nSlot   = element.tcpHeader.slot;
	uint nRunner = element.tcpHeader.runner;
	uint nSize   = sizeof(stTcpHeader) + element.tcpHeader.datalen;
	char* pData  = new char[nSize];
	memcpy( pData, &element.tcpHeader, sizeof(stTcpHeader) );
	if( element.tcpHeader.datalen > 0 )
	{
		memcpy( pData+sizeof(stTcpHeader), element.pData, element.tcpHeader.datalen );
		delete[] element.pData;
	}

	for( CTcpClient* client : m_vecTcpClient )
	{
		uint slot = 0, runner = 0;
		if( !client->IsHaveId() ) continue;

		client->GetUniqueId( slot, runner );
		if( (nSlot == slot) && (nRunner == runner) )
		{
			if( !client->SendPacket( pData, nSize ) )
			{
				client->CloseSocket();
				QString title = tr("[%1][S#%2-R#%3]").arg("->PGM", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0'));
				emit SigDevLogMessage( _ERROR_LOG, tr("%1 %2 error!").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				continue;
			}
			QString title = tr("[%1][S#%2-R#%3]").arg("->PGM", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0'));
			emit SigDevLogMessage( _NORMAL_LOG, tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
			break;
		}
	}
	delete[] pData;
}
