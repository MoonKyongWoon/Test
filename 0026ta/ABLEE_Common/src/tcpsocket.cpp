#include "tcpsocket.hpp"

CTcpSocket::CTcpSocket( int id0, int id1 ) : m_nId0(id0), m_nId1(id1)
{
	m_pSocket = new QTcpSocket(this);
	m_bConn = false;
	m_bHost =false;
	m_pBuffer = nullptr;
	m_bHeader = true;
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;

	m_pSocket->setSocketOption( QAbstractSocket::KeepAliveOption, 0x1 );

	connect( m_pSocket, &QTcpSocket::readyRead   ,  this, &CTcpSocket::SlotTcpReadyRead          );
	connect( m_pSocket, &QTcpSocket::connected   ,  this, &CTcpSocket::SlotTcpSocketConnected    );
	connect( m_pSocket, &QTcpSocket::disconnected,  this, &CTcpSocket::SlotTcpSocketDisconnected );
}

CTcpSocket::~CTcpSocket()
{
    CloseSocket();
}

void CTcpSocket::CloseSocket()
{
	if( m_pSocket )
	{
		if( m_pSocket->state() == QAbstractSocket::ConnectedState )
		{
			m_pSocket->disconnectFromHost();
		}
		m_pSocket->close();
		m_pSocket->deleteLater();
		DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
	}
}

void CTcpSocket::GetUniqueId( int& id0, int& id1 )
{
	id0 = m_nId0;
	id1 = m_nId1;
}

void CTcpSocket::SetHostInfo( QString ip, quint16 port)
{
	m_sIp = ip;
	m_uPort = port;	
	if( UTIL::VALIDATION::IsValidIpV4( ip.toStdString() ) )
		m_bHost = true;
	else
		m_bHost = false;
	//printf("LOADER TCP SOCKET ID0:%d ID1:%d IP:%s PORT:%d\n", m_nId0, m_nId1, m_sIp.toStdString().data(), m_uPort );
}

bool CTcpSocket::IsConn()
{
	return m_bConn;
}

void CTcpSocket::SlotConnectToHostA( int id0, int id1 )
{
	if( (id0 != m_nId0) || (id1 != m_nId1) ) return;

	if( !m_bHost )
	{
		emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[CONN][ ERR] invalid tcp server info %1:%2!").arg(m_sIp).arg(m_uPort) );
		return;
	}
	if( m_bConn ) return;

	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG , tr("tcp server info %2:%3!").arg(m_sIp).arg(m_uPort) );
	if( m_pSocket->state() != QAbstractSocket::UnconnectedState )
	{
		return;
	}

	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("connect to %1:%2").arg(m_sIp).arg(m_uPort) );
	m_pSocket->connectToHost( m_sIp, m_uPort );
}

void CTcpSocket::SlotConnectToHost( int id0 )
{
	if( m_bConn ) return;

	if( id0 != m_nId0 )
	{
		if( id0 != -1 ) return;		//-1 : all slot
	}

	if( !m_bHost )
	{
		emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[CONN][ ERR] invalid tcp server info %1:%2!").arg(m_sIp).arg(m_uPort) );
		return;
	}

	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG , tr("tcp server info %2:%3!").arg(m_sIp).arg(m_uPort) );
	if( m_pSocket->state() != QAbstractSocket::UnconnectedState )
	{
		return;
	}

	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("connect to %1:%2").arg(m_sIp).arg(m_uPort) );
	m_pSocket->connectToHost( m_sIp, m_uPort );
}

void CTcpSocket::SlotTcpSocketConnected()
{
	int keepalive = 1;
	setsockopt(m_pSocket->socketDescriptor(), SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
	int maxIdle = 10;
	setsockopt(m_pSocket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
	int count = 3;
	setsockopt(m_pSocket->socketDescriptor(), SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));
	int interval = 2;
	setsockopt(m_pSocket->socketDescriptor(), SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

	m_bConn = true;
	emit SigStatusChanged( m_nId0, m_nId1, m_bConn );
	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("Connected") );
}

void CTcpSocket::SlotTcpSocketDisconnected()
{
	m_bConn = false;
	emit SigStatusChanged( m_nId0, m_nId1, m_bConn );
	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("Disconnected") );
}

#define HEAD_SIZE		sizeof(stTcpHeader)
void CTcpSocket::SlotTcpReadyRead()
{
	//static int DATA_SIZE   = 0x0;
	//static int REMAIN_SIZE = 0x0;
	//static int INSERT_POS  = 0x0;
	qint64 uByteAbailable  = m_pSocket->bytesAvailable();
	while( uByteAbailable > 0 )
	{
		//if (dwSizeRead > PROTOCOL::MAX_TCP_BUFFER) dwSizeRead = PROTOCOL::MAX_TCP_BUFFER;
		if( m_bHeader )
		{
			if( static_cast<uint>(uByteAbailable) < HEAD_SIZE ) break;

			stTcpHeader header;
			qint64 nRead = m_pSocket->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[RECV][ ERR] %1:%2 receive header error!").arg(m_sIp).arg(m_uPort) );
				return;
			}
			m_pBuffer = new char [ HEAD_SIZE + header.datalen ];
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = header.datalen;
			REMAIN_SIZE     = header.datalen;
			m_bHeader = false;
		}
		else
		{
			qint64 nRead = m_pSocket->read( m_pBuffer+INSERT_POS, REMAIN_SIZE );
			if( nRead < 0 )
			{
				emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[RECV][ ERR] %1:%2 receive data error!").arg(m_sIp).arg(m_uPort) );
				return;
			}
			uByteAbailable -= nRead;
			REMAIN_SIZE    -= nRead;
			INSERT_POS     += nRead;
		}

		if( !m_bHeader && (REMAIN_SIZE == 0) )
		{
			stTcpElement element;
			element.pData = nullptr;
			memcpy( (char*)&element.tcpHeader, m_pBuffer        , HEAD_SIZE  );
			if( DATA_SIZE > 0 )
			{
				element.pData = new char[DATA_SIZE];
				memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
			}
			//qDebug() << tr("[TCP SOCKET][EMIT] CMD : %1[%2] Available Bytes:%3")
			//			.arg(element.tcpHeader.cmd).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(element.tcpHeader.cmd))).arg(uByteAbailable);
			emit SigReceivePacket( m_nId0, m_nId1, element );
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_bHeader = true;
			DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
		}		
		//uByteAbailable  = m_pSocket->bytesAvailable();
		//qDebug() << tr("[TCP SOCKET][LOOP] Available Bytes : %1").arg(uByteAbailable);
	}
}

//no use
void CTcpSocket::SlotSendPacketA( int id0, int id1, char* buf, uint size )
{
	if( (id0 != m_nId0) || (id1 != m_nId1) ) return;

	if( !m_bConn )
	{
		emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[SEND][ ERR] SLOT%1-CPU:%2 is not connected!").arg(id0+1).arg(id1+1) );
		return;
	}

	char* pBuffer = buf;
	qint64 tosend = size;
	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( buf, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[SEND][ ERR] SLOT%1-CPU:%2 error[%3]").arg(id0+1).arg(id1).arg(m_pSocket->errorString()) );
			delete[] pBuffer;
			return;
		}
		tosend -= nSendByte;
		buf    += nSendByte;
	}
	m_pSocket->flush();
	m_pSocket->waitForBytesWritten(5);
	delete[] pBuffer;
}

bool CTcpSocket::SendPacket( int id0, char* buf, uint size )
{
	if( id0 != m_nId0 ) return false;

	//QMutexLocker m_locker(&m_mutex);
	if( !m_bConn )
	{
		emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[SEND][ ERR] SLOT%1-CPU:%2 is not connected!").arg(m_nId0+1).arg(m_nId1+1) );
		return false;
	}

	char* pBuffer = buf;
	qint64 tosend = size;
	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( pBuffer, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			emit SigLogMessage( m_nId0, m_nId1, _ERROR_LOG , tr("[SEND][ ERR] SLOT%1-CPU:%2 error[%3]").arg(m_nId0+1).arg(m_nId1+1).arg(m_pSocket->errorString()) );
			return false;
		}
		tosend  -= nSendByte;
		pBuffer += nSendByte;
	}
	m_pSocket->flush();
	m_pSocket->waitForBytesWritten(1);
	return true;
}

void CTcpSocket::SlotSendPacket( int id0, char* buf, uint size )
{
	if( !SendPacket( id0, buf, size ) )
	{
		m_pSocket->close();
	}
	delete[] buf;
}
