#include "hsmssocket.hpp"

CHsmsSocket::CHsmsSocket( int id0, int id1 ) : m_nId0(id0), m_nId1(id1)
{
	m_pSocket = new QTcpSocket(this);
	m_bConn = false;
	m_bHost =false;
	m_pBuffer = nullptr;
	m_bHeader = true;
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;

	m_pSocket->setSocketOption( QAbstractSocket::KeepAliveOption, 0x1 );
	connect( m_pSocket, &QTcpSocket::readyRead   ,  this, &CHsmsSocket::SlotTcpReadyRead          );
	connect( m_pSocket, &QTcpSocket::connected   ,  this, &CHsmsSocket::SlotTcpSocketConnected    );
	connect( m_pSocket, &QTcpSocket::disconnected,  this, &CHsmsSocket::SlotTcpSocketDisconnected );
}

CHsmsSocket::~CHsmsSocket()
{
    CloseSocket();
}

void CHsmsSocket::CloseSocket()
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

void CHsmsSocket::GetUniqueId( int& id0, int& id1 )
{
	id0 = m_nId0;
	id1 = m_nId1;
}

void CHsmsSocket::SetHostInfo( QString ip, quint16 port)
{
	m_sIp = ip;
	m_uPort = port;	
	if( UTIL::VALIDATION::IsValidIpV4( ip.toStdString() ) )
		m_bHost = true;
	else
		m_bHost = false;
	//printf("LOADER TCP SOCKET ID0:%d ID1:%d IP:%s PORT:%d\n", m_nId0, m_nId1, m_sIp.toStdString().data(), m_uPort );
}

bool CHsmsSocket::IsConn()
{
	return m_bConn;
}

void CHsmsSocket::Disconnect()
{
	this->m_pSocket->disconnectFromHost();
}

void CHsmsSocket::SlotDisconnectFromHost()
{
	this->Disconnect();
}

void CHsmsSocket::SlotConnectToHost()
{
	if( m_bConn ) return;

	if( !m_bHost )
	{
		emit SigLogMessage( _ERROR_LOG , tr("[CONN][ ERR] invalid tcp server info %1:%2!").arg(m_sIp).arg(m_uPort) );
		return;
	}

	if( m_pSocket->state() != QAbstractSocket::UnconnectedState )
	{
		return;
	}

	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("connect to %1:%2").arg(m_sIp).arg(m_uPort) );
	m_pSocket->connectToHost( m_sIp, m_uPort );
}

void CHsmsSocket::SlotTcpSocketConnected()
{
	m_bConn = true;
	emit SigStatusChanged( m_bConn );
	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("Connected") );
}

void CHsmsSocket::SlotTcpSocketDisconnected()
{
	m_bConn = false;
	emit SigStatusChanged( m_bConn );
	//emit SigLogMessage( m_nId0, m_nId1, _NORMAL_LOG, tr("Disconnected") );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HEAD_MSG_LENGTH 4 BYTE + MSG_HEADER 10 BYTE + DATA BYTES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define HEAD_MSG_BYTES		(4)
#define HEAD_SIZE			sizeof(stHsmsHeader)
#define MESSAGE_HEADER_SIZE sizeof(stHsmsMsgHeader)
void CHsmsSocket::SlotTcpReadyRead()
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

			stHsmsHeader header;
			qint64 nRead = m_pSocket->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				emit SigLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1:%2 receive header error!").arg(m_sIp).arg(m_uPort) );
				return;
			}
			int nHeadMsgLen = 0x0;
			nHeadMsgLen |= (header.szMsgLength[0] << 24 );
			nHeadMsgLen |= (header.szMsgLength[1] << 16 );
			nHeadMsgLen |= (header.szMsgLength[2] <<  8 );
			nHeadMsgLen |= (header.szMsgLength[3] <<  0 );

			int nDataLen = nHeadMsgLen - MESSAGE_HEADER_SIZE;
			m_pBuffer = new char [ nHeadMsgLen + HEAD_MSG_BYTES];
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = nDataLen;
			REMAIN_SIZE     = nDataLen;
			m_bHeader = false;
		}
		else
		{
			qint64 nRead = m_pSocket->read( m_pBuffer+INSERT_POS, REMAIN_SIZE );
			if( nRead < 0 )
			{
				emit SigLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1:%2 receive data error!").arg(m_sIp).arg(m_uPort) );
				return;
			}
			uByteAbailable -= nRead;
			REMAIN_SIZE    -= nRead;
			INSERT_POS     += nRead;
		}

		if( !m_bHeader && (REMAIN_SIZE == 0) )
		{
			stHsmsElement element;
			element.pData = nullptr;
			memcpy( (char*)&element.hsmsHeader, m_pBuffer, HEAD_SIZE  );
			if( DATA_SIZE > 0 )
			{
				element.pData = new char[DATA_SIZE];
				memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
			}
			emit SigReceivePacket( element );
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_bHeader = true;
			DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
		}		
	}
}

bool CHsmsSocket::SendPacket( char* buf, uint size )
{
	//QMutexLocker m_locker(&m_mutex);
	if( !m_bConn )
	{
		emit SigLogMessage( _ERROR_LOG , tr("[SEND][ ERR] not connected!") );
		return false;
	}

	char* pBuffer = buf;
	qint64 tosend = size;
	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( pBuffer, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			emit SigLogMessage( _ERROR_LOG , tr("[SEND][ ERR] error[%1]").arg(m_pSocket->errorString()) );
			return false;
		}
		tosend  -= nSendByte;
		pBuffer += nSendByte;
	}
	m_pSocket->flush();
	m_pSocket->waitForBytesWritten(1);
	return true;
}

void CHsmsSocket::SlotSendPacket( char* buf, uint size )
{
	if( !SendPacket( buf, size ) )
	{
		m_pSocket->close();
	}
	delete[] buf;
}
