#include "../include/loadermgr.hpp"

CLoaderMgr::CLoaderMgr(QObject *parent) : QObject(parent)
{
	m_vClient.clear();
	m_nConnTime  = 2500;
	memset( &m_stSlotLive, 0x0, sizeof(stSlotLive) );
	m_pConnTimer = new QTimer(this);
}

CLoaderMgr::~CLoaderMgr()
{
	if( m_pConnTimer->isActive() ) m_pConnTimer->stop();

	for( int i=0;i<m_vClient.count();i++ )
	{
		CTcpSocket* tcpsocket = m_vClient.at(i);
		if( tcpsocket->IsConn() ) tcpsocket->CloseSocket();
		tcpsocket->deleteLater();
	}
	m_vClient.clear();
}

//EWS Ftp Info
void CLoaderMgr::LoadFtpIni()
{
	QString os_path = "";
	QString section_key = "";
	QString sValue = 0x0;
	bool bOk = true;

	memset( &m_stEwsFtpInfo, 0x0, sizeof(stFtpInfo) );
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(FTP_INI);

	//IP
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		snprintf( m_stEwsFtpInfo.szServer, sizeof(m_stEwsFtpInfo.szServer), "%s", DEFAULT_EWS_FTP_IP );
	}
	else
	{
		snprintf( m_stEwsFtpInfo.szServer, sizeof(m_stEwsFtpInfo.szServer), "%s", sValue.toStdString().c_str() );
	}

	//USER
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_USER);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		snprintf( m_stEwsFtpInfo.szId, sizeof(m_stEwsFtpInfo.szId), "%s", DEFAULT_EWS_FTP_USER );
	}
	else
	{
		snprintf( m_stEwsFtpInfo.szId, sizeof(m_stEwsFtpInfo.szId), "%s", sValue.toStdString().c_str() );
	}

	//PASSWD
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PASSWD);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		snprintf( m_stEwsFtpInfo.szPasswd, sizeof(m_stEwsFtpInfo.szPasswd), "%s", DEFAULT_EWS_FTP_PASSWD );
	}
	else
	{
		snprintf( m_stEwsFtpInfo.szPasswd, sizeof(m_stEwsFtpInfo.szPasswd), "%s", sValue.toStdString().c_str() );
	}

	//PORT
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PORT);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		m_stEwsFtpInfo.uPort = DEFAULT_EWS_FTP_PORT;

	}
	else
	{
		uint nPort = sValue.toUInt(&bOk, 10);
		if( !bOk ) nPort = DEFAULT_EWS_FTP_PORT;
		m_stEwsFtpInfo.uPort = nPort;
	}
	//qDebug() << "EWS FTP IP : " << m_stEwsFtpInfo.szServer;
	//qDebug() << "EWS FTP ID : " << m_stEwsFtpInfo.szId;
	//qDebug() << "EWS FTP PASSWD : " << m_stEwsFtpInfo.szPasswd;
	//qDebug() << "EWS FTP PORT   : " << m_stEwsFtpInfo.uPort;
}

void CLoaderMgr::SlotStop()
{
	if( m_pConnTimer )
	{
		if( m_pConnTimer->isActive() ) m_pConnTimer->stop();
	}
}

void CLoaderMgr::SlotRun()
{
	LoadFtpIni();
	for( int i=0;i<m_vClient.count();i++ )
	{
		int id0, id1;
		CTcpSocket* tcpsocket = m_vClient.at(i);
		tcpsocket->GetUniqueId( id0, id1 );
		connect( tcpsocket, &CTcpSocket::SigStatusChanged, this     , &CLoaderMgr::SlotTcpStatusChanged );
		connect( tcpsocket, &CTcpSocket::SigReceivePacket, this     , &CLoaderMgr::SlotTcpReceivePacket );
		connect( tcpsocket, &CTcpSocket::SigLogMessage   , this     , &CLoaderMgr::SlotTcpLogMessage    );
		connect( this     , &CLoaderMgr::SigTcpCommRun   , tcpsocket, &CTcpSocket::SlotConnectToHost    );
	}
	StartConnTimer();
	emit SigLogMessage( _NORMAL_LOG, tr("Loader manager start...") );
}

void CLoaderMgr::SlotMakeSockets()
{
	for( int i=0;i<m_vClient.count();i++ )
	{
		CTcpSocket* tcpsocket = m_vClient.at(i);
		if( tcpsocket->IsConn() ) tcpsocket->CloseSocket();
		tcpsocket->deleteLater();
	}
	m_vClient.clear();

	for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
	{
		if( m_stSlotInfo.SlotIp[s].bMask )
		{
			m_stSlotLive.slot_status[s][CPU00] = EN_SLOT_MASK;
			m_stSlotLive.slot_status[s][CPU01] = EN_SLOT_MASK;
			continue;
		}

		CTcpSocket* tcpsocket = new CTcpSocket(s, CPU00);
		tcpsocket->SetHostInfo( m_stSlotInfo.SlotIp[s].szCpu1Ip, TCP_PORT::CPU_LOADER_MGR_PORT );
		m_vClient.push_back( tcpsocket );

		if( m_stSlotInfo.SlotCpu.nCpuPerSlot  == 2 )
		{
			CTcpSocket* tcpsocket = new CTcpSocket(s, CPU01);
			tcpsocket->SetHostInfo( m_stSlotInfo.SlotIp[s].szCpu2Ip, TCP_PORT::CPU_LOADER_MGR_PORT );
			m_vClient.push_back( tcpsocket );
		}
	}
	emit SigLogMessage( _NORMAL_LOG, tr("Loader manager make tcp sockets...") );
}

void CLoaderMgr::SlotTimeOutConn()
{
	if( m_vClient.count() > 0 ) emit SigTcpCommRun();
}

void CLoaderMgr::StartConnTimer()
{
	connect( m_pConnTimer, &QTimer::timeout, this, &CLoaderMgr::SlotTimeOutConn );
	m_pConnTimer->start( m_nConnTime );
}

//atsite.ini - slot info.
void CLoaderMgr::SetSlotInfo( stSlotInfo slotinfo )
{
	memcpy ( &m_stSlotInfo, &slotinfo, sizeof(stSlotInfo) );
}

void CLoaderMgr::SlotTcpReceivePacket( int id0, int id1, stTcpElement element )
{
	//element.tcpHeader.slot   = id0;
	//element.tcpHeader.runner = id1;
	emit SigLoaderRecvPacket( id0, id1, element );
}

void CLoaderMgr::SlotTcpStatusChanged( int id0, int id1, bool bConn )
{
	QString title = tr("SLOT%1-CPU%2").arg(id0+1, 2, 10, QChar('0')).arg(id1+1, 2, 10, QChar('0'));
	if( !bConn )
	{
		emit SigLogMessage( _NORMAL_LOG, tr("%1 not connected....").arg(title) );
		m_stSlotLive.slot_status[id0][id1] = EN_SLOT_OFFLINE;
	}
	else
	{
		emit SigLogMessage( _NORMAL_LOG, tr("%1 connected....").arg(title) );
		m_stSlotLive.slot_status[id0][id1] = EN_SLOT_ONLINE;
		SendSystemInfoToSlot( id0, id1 );
		SendFtpInfoToSlot   ( id0, id1 );
		//2023.06.07 - thread run for none blocking
		std::thread thsync( &CLoaderMgr::SshPassSlotTimeSync, this, id0, id1 );
		thsync.detach();
	}
	//qDebug() << title << m_stSlotLive.slot_status[id0][id1];
	emit SigLoaderAckConnSlots( m_stSlotLive );
}

void CLoaderMgr::SlotTcpLogMessage( int id0, int id1, LOG_TYPE t, QString s )
{
	QString title = tr("SLOT%1-RUN%2").arg(id0+1,2,10,QChar('0')).arg(id1+1,2,10,QChar('0'));
	QString msg   = tr("%1 %2").arg(title).arg(s);
	emit SigLogMessage( t, msg );
}

bool CLoaderMgr::IsSlotCpuConn( int slot, int cpu)
{
	for( CTcpSocket* tcpsocket : m_vClient )
	{
		int rSlot = 0x0, rCpu = 0x0;
		tcpsocket->GetUniqueId( rSlot, rCpu );
		if( (slot == rSlot) && (cpu == rCpu) )
		{
			if( tcpsocket->IsConn() ) return true;
		}
	}
	return false;
}

//2023.06.03 - KWMOON
void CLoaderMgr::SshPassSlotTimeSync( int s, int c )
{
	QString ip = QString( m_stSlotInfo.SlotIp[s].szCpu1Ip );
	QString title = tr("[%1][S#%2]").arg("->CPU", 10, QChar(' ')).arg(s+1,2,10,QChar('0'));
	QString scmd = tr("sshpass -p %1 ssh %2@%3 -T -o StrictHostKeyChecking=no 'echo %4 | sudo -S systemctl restart systemd-timesyncd.service'").arg(SSH_PASSWD).arg(SSH_ID).arg(ip).arg(SSH_PASSWD);
	QString msg = title + scmd;
	emit SigLogMessage( _NORMAL_LOG, msg );

	std::system ( scmd.toStdString().c_str() );
	//UTIL::PROCESS::DetachRun( prg.toStdString(), param );
/*
	if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
	{
		param.clear();
		ip = QString( m_stSlotInfo.SlotIp[s].szCpu2Ip );
		param << "-p" << SSH_PASSWD << "ssh" << tr("%1@%2").arg(SSH_ID).arg(ip) << tr("'echo %1 | sudo -S systemctl restart systemd-timesyncd.service'").arg(SSH_PASSWD);
		UTIL::PROCESS::DetachRun( prg.toStdString(), param );
	}
*/
}

void CLoaderMgr::SendSystemInfoToSlot( int s, int c )
{
	stTcpHeader header;
	header.cmd     = PROTOCOL::CMD_SLOT_CPU_INFO_SEND;
	header.src     = PROTOCOL::_ATMGR;
	header.slot    = s;
	header.dest    = (PROTOCOL::_ATSLOT01 + s );
	header.datalen = sizeof(stSlotCpu);

	stSlotCpu SlotCpu;
	SlotCpu.nSlotNo     = s+1;
	SlotCpu.nMaxSlot    = m_stSlotInfo.SlotCpu.nMaxSlot;
	SlotCpu.nCpuPerSlot = m_stSlotInfo.SlotCpu.nCpuPerSlot;
	SlotCpu.nPgmPerSlot = m_stSlotInfo.SlotCpu.nPgmPerSlot;

	uint nSize = sizeof(stTcpHeader) + header.datalen;
	char* pBuffer = new char[nSize];
	memcpy( pBuffer                    , &header   , sizeof(stTcpHeader) );
	memcpy( pBuffer+sizeof(stTcpHeader), &SlotCpu , header.datalen       );

	for( CTcpSocket* tcpsocket : m_vClient )
	{
		int rSlot = 0x0, rCpu = 0x0;
		tcpsocket->GetUniqueId( rSlot, rCpu );
		if( (rSlot==s) && (rCpu==c) )
		{
			tcpsocket->SendPacket( rSlot, pBuffer, nSize );
		}
	}
	if( nSize )
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}
}

void CLoaderMgr::SendFtpInfoToSlot( int s, int c )
{
	stTcpHeader header;
	header.cmd     = PROTOCOL::CMD_EWS_FTP_INFO_SEND;
	header.src     = PROTOCOL::_ATMGR;
	header.dest    = (PROTOCOL::_ATSLOT01 + s );
	header.slot    = s;
	header.datalen = sizeof(stFtpInfo);

	uint nSize = sizeof(stTcpHeader) + header.datalen;
	char* pBuffer = new char[nSize];
	memcpy( pBuffer                    , &header         , sizeof(stTcpHeader) );
	memcpy( pBuffer+sizeof(stTcpHeader), &m_stEwsFtpInfo , header.datalen      );

	for( CTcpSocket* tcpsocket : m_vClient )
	{
		int rSlot = 0x0, rCpu = 0x0;
		tcpsocket->GetUniqueId( rSlot, rCpu );
		if( (rSlot==s) && (rCpu==c) )
		{
			QString title = tr("[%1][S#%2]").arg("->CPU", 10, QChar(' ')).arg(rSlot+1,2,10,QChar('0'));
			if( !tcpsocket->SendPacket( rSlot, pBuffer, nSize ) )
			{
				tcpsocket->CloseSocket();
				emit SigLogMessage( _ERROR_LOG, tr("%1 %2 error!").arg(title).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_EWS_FTP_INFO_SEND)) );
			}
			else
			{
				emit SigLogMessage( _NORMAL_LOG, tr("%1 %2!").arg(title).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_EWS_FTP_INFO_SEND)) );
			}
		}
	}
	if( nSize )
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}
}

void CLoaderMgr::SlotSendPacketToLoader( stTcpElement emt )
{
	uint nSize = sizeof(stTcpHeader) + emt.tcpHeader.datalen;
	uint nSlot = emt.tcpHeader.slot;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(emt.tcpHeader.cmd);

	char* pBuffer = new char[nSize];
	memcpy( pBuffer, &emt.tcpHeader, sizeof(stTcpHeader) );
	if( emt.tcpHeader.datalen > 0 )
	{
		if( emt.pData )
		{
			memcpy( pBuffer+sizeof(stTcpHeader), emt.pData, emt.tcpHeader.datalen );
			delete[] emt.pData;
			emt.pData = nullptr;
		}
	}

	for( CTcpSocket* tcpsocket : m_vClient )
	{
		int rSlot = 0x0, rCpu = 0x0;
		tcpsocket->GetUniqueId( rSlot, rCpu );
		if( ( nSlot & (1<<rSlot) ) )
		{
			if( rCpu==CPU00 )
			{
				QString title = tr("[%1][S#%2]").arg("->CPU", 10, QChar(' ')).arg(rSlot+1,2,10,QChar('0'));
				if( !tcpsocket->SendPacket( rSlot, pBuffer, nSize ) )
				{
					emit SigLogMessage( _ERROR_LOG, tr("%1 %2 error!").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
					tcpsocket->CloseSocket();
				}
				else
				{
					emit SigLogMessage( _NORMAL_LOG, tr("%1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				}
			}
		}
		if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
		{
			if( ( nSlot & (1<<rSlot) ) )
			{
				if( rCpu== CPU01 )
				{
					QString title = tr("[%1][S#%2]").arg("->CPU", 10, QChar(' ')).arg(rSlot+1,2,10,QChar('0'));
					if( !tcpsocket->SendPacket( rSlot, pBuffer, nSize ) )
					{
						emit SigLogMessage( _ERROR_LOG, tr("%1 %2 error!").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
						tcpsocket->CloseSocket();
					}
					else
					{
						emit SigLogMessage( _NORMAL_LOG, tr("[SEND] %1 %2").arg(title).arg(UTIL::TCPIP::CMD_STR(cmd)) );
					}
				}
			}
		}
	}
	if( nSize )
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}

}
