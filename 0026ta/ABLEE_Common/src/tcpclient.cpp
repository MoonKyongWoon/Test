#include "tcpclient.hpp"

CTcpClient::CTcpClient(qintptr handle)
{
	m_nId0 = 0;
	m_nId1 = 0;
	m_bHaveId = false;
	m_bHeader = true;
    m_pSocket = new QTcpSocket(this);

	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;

	if( m_pSocket->setSocketDescriptor(handle) )
    {
		m_pSocket->setSocketOption( QAbstractSocket::KeepAliveOption, 0x1 );

		int keepalive = 1;
		setsockopt(m_pSocket->socketDescriptor(), SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
		int maxIdle = 10;
		setsockopt(m_pSocket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
		int count = 3;
		setsockopt(m_pSocket->socketDescriptor(), SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));
		int interval = 2;
		setsockopt(m_pSocket->socketDescriptor(), SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

		//snprintf( m_szHostIp, sizeof(m_szHostIp), "%s", m_pSocket->peerAddress().toString().toLatin1().data() );
		//m_uHostPort = m_pSocket->peerPort();
		connect( m_pSocket, &QTcpSocket::readyRead   ,  this, &CTcpClient::SlotReadyRead          );
		connect( m_pSocket, &QTcpSocket::disconnected,  this, &CTcpClient::SlotSocketDisconnected );
		connect( m_pSocket, &QTcpSocket::stateChanged,  this, &CTcpClient::SlotSocketStateChanged );
	}
	m_bConn = true;
}

CTcpClient::~CTcpClient()
{
    CloseSocket();
}

void CTcpClient::SetUniqueId(uint id0, uint id1)
{
	m_nId0 = id0;
	m_nId1 = id1;
	m_bHaveId = true;
}

void CTcpClient::GetUniqueId(uint& rId0, uint& rId1)
{
	rId0 = m_nId0;
	rId1 = m_nId1;
}

bool CTcpClient::IsHaveId()
{
	return m_bHaveId;
}

void CTcpClient::CloseSocket()
{
	if( m_pSocket != nullptr )
	{
		if( m_pSocket->state() == QAbstractSocket::ConnectedState )
		{
			m_pSocket->close();
			m_bConn = false;
		}		
	}
}

void CTcpClient::SlotSocketDisconnected()
{
	m_bConn = false;
	emit SigStatusChanged( false );
	emit SigLogMessage( _NORMAL_LOG, tr("[CLIENT%1-%2]SlotSocketDisconnected CONN:%3 STATE(%4):NotConnected!")
				.arg(m_nId0+1).arg(m_nId1+1).arg(m_bConn?"Y":"N").arg(m_pSocket->state()));
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
}

void CTcpClient::SlotSocketStateChanged(QAbstractSocket::SocketState state)
{
	if( state != QAbstractSocket::ConnectedState )
	{
		emit SigLogMessage( _NORMAL_LOG, tr("[CLIENT%1-%2]SlotSocketStateChanged CONN:%3 STATE(%4):NotConnected!")
					.arg(m_nId0+1).arg(m_nId1+1).arg(m_bConn?"Y":"N").arg(state));
	}
}

QString CTcpClient::GetHostIp()
{
	return m_pSocket->peerAddress().toString();
}

#define HEAD_SIZE		sizeof(stTcpHeader)
void CTcpClient::SlotReadyRead()
{
	//static int DATA_SIZE   = 0x0;
	//static int REMAIN_SIZE = 0x0;
	//static int INSERT_POS  = 0x0;
	qint64 uByteAbailable  = m_pSocket->bytesAvailable();
	//qint64 uIndex          = 0;
	//qDebug() << tr("***%1*** Avail Bytes : %2").arg(__FUNCTION__).arg(uByteAbailable);

	while( uByteAbailable > 0 )
	{
		if( m_bHeader )
		{
			if( static_cast<uint>(uByteAbailable) < HEAD_SIZE ) break;
			stTcpHeader header;
			qint64 nRead = m_pSocket->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				qDebug() << "[TCP_CLIENT][HEAD] READ ERROR " << nRead;
				emit SigLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1 receive header error!").arg(m_szHostIp) );
				return;
			}
			//PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(header.cmd);
			m_pBuffer = new char [ HEAD_SIZE + header.datalen ];
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = header.datalen;
			REMAIN_SIZE     = header.datalen;
			m_bHeader = false;

			//if( (m_nId0 == 0) && (m_nId1 == 0) )
			//qDebug() << tr("[TCP_CLIENT%1-%2][HEAD] CMD:%3 HEADER:%4 / DATA:%5 READ SIZE : %6 ----- %7")
			//			.arg(m_nId0).arg(m_nId1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(HEAD_SIZE).arg(DATA_SIZE).arg(nRead).arg(uIndex++,5,10,QChar('0'));
		}
		else
		{
			qint64 nRead = m_pSocket->read( m_pBuffer+INSERT_POS, REMAIN_SIZE );
			if( nRead < 0 )
			{
				qDebug() << "[TCP_CLIENT][DATA] READ ERROR : " << nRead;
				emit SigLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1 receive data error!").arg(m_szHostIp) );
				return;
			}
			uByteAbailable -= nRead;
			REMAIN_SIZE    -= nRead;
			INSERT_POS     += nRead;

			//if( (m_nId0 == 0) && (m_nId1 == 0) )
			//qDebug() << tr("[TCP_CLIENT%1-%2][DATA] READ SIZE:%3 / REMAIN_SIZE:%4 ----- %5")
			//			.arg(m_nId0).arg(m_nId1).arg(nRead).arg(REMAIN_SIZE).arg(uIndex++,5,10,QChar('0'));
		}

		if( !m_bHeader && (REMAIN_SIZE == 0) )
		{
			stTcpElement element;
			memcpy( (char*)&element, m_pBuffer        , HEAD_SIZE  );
			element.pData = nullptr;
			if( DATA_SIZE > 0 )
			{
				element.pData = new char[DATA_SIZE];
				memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
			}
			emit SigReceivePacket( element );
			//qDebug() << tr("[TCP_CLIENT%1-%2][EMIT] DATA SIZE:%3 ----- %4")
			//			.arg(m_nId0).arg(m_nId1).arg(DATA_SIZE).arg(uIndex++,5,10,QChar('0'));

			if( m_pBuffer != nullptr )
			{
				delete[] m_pBuffer;
				m_pBuffer = nullptr;
			}
			m_bHeader = true;
			DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
		}
	}
}

bool CTcpClient::SendPacket( char* buffer, uint size )
{
	//QMutexLocker m_locker(&m_mutex);

	char* pBuffer = buffer;
	qint64 tosend = size;

	if( !m_bConn )
		return false;

	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( pBuffer, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			emit SigLogMessage( _ERROR_LOG , tr("[SEND][ ERR] Send to %1 error!").arg(m_szHostIp) );
			return false;
		}
		tosend  -= nSendByte;
		pBuffer += nSendByte;
	}
	m_pSocket->flush();
	//m_pSocket->waitForBytesWritten(1);
	//qDebug() << "LOADER->MGR " << "WRITE SIZE : " << size;

	return true;
}

void CTcpClient::SlotSendPacket( char* buf, uint size )
{
	if( !SendPacket( buf, size ) )
	{
		m_pSocket->close();
	}

	if( buf != nullptr )
	{
		delete[] buf;
		buf = nullptr;
	}
}
