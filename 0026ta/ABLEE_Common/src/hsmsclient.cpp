#include "hsmsclient.hpp"

CHsmsClient::CHsmsClient(qintptr handle)
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
		//snprintf( m_szHostIp, sizeof(m_szHostIp), "%s", m_pSocket->peerAddress().toString().toLatin1().data() );
		//m_uHostPort = m_pSocket->peerPort();
		connect( m_pSocket, &QTcpSocket::readyRead   ,  this, &CHsmsClient::SlotReadyRead          );
		connect( m_pSocket, &QTcpSocket::disconnected,  this, &CHsmsClient::SlotSocketDisconnected );
		connect( m_pSocket, &QTcpSocket::stateChanged,  this, &CHsmsClient::SlotSocketStateChanged );
		m_bConn = true;
	}
}

CHsmsClient::~CHsmsClient()
{
	CloseSocket();
}

void CHsmsClient::SetUniqueId(uint id0, uint id1)
{
	m_nId0 = id0;
	m_nId1 = id1;
	m_bHaveId = true;
}

void CHsmsClient::GetUniqueId(uint& rId0, uint& rId1)
{
	rId0 = m_nId0;
	rId1 = m_nId1;
}

bool CHsmsClient::IsHaveId()
{
	return m_bHaveId;
}

void CHsmsClient::CloseSocket()
{
	if( m_pSocket != nullptr )
	{
		if( m_pSocket->state() == QAbstractSocket::ConnectedState )
		{
			m_pSocket->close();
		}		
		m_pSocket->deleteLater();
		m_bConn = false;
	}
}

void CHsmsClient::SlotSocketDisconnected()
{
	m_bConn = false;
	emit SigStatusChanged( false );
//	emit SigLogMessage( _NORMAL_LOG, tr("[HSMS_CLIENT]SlotSocketDisconnected CONN:%1 STATE(%2):NotConnected!")
//				.arg(m_bConn?"Y":"N").arg(m_pSocket->state()));
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
}

void CHsmsClient::SlotSocketStateChanged(QAbstractSocket::SocketState state)
{
	if( state != QAbstractSocket::ConnectedState )
	{
		emit SigLogMessage( _NORMAL_LOG, tr("[HSMS_CLIENT]SlotSocketStateChanged CONN:%1 STATE(%2):NotConnected!")
					.arg(m_bConn?"Y":"N").arg(state));
	}
}

QString CHsmsClient::GetHostIp()
{
	return m_pSocket->peerAddress().toString();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HEAD_MSG_LENGTH 4 BYTE + MSG_HEADER 10 BYTE + DATA BYTE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define HEAD_MSG_BYTES		(4)
#define HEAD_SIZE			sizeof(stHsmsHeader)
#define MESSAGE_HEADER_SIZE sizeof(stHsmsMsgHeader)
void CHsmsClient::SlotReadyRead()
{
	qint64 uByteAbailable  = m_pSocket->bytesAvailable();
	while( uByteAbailable > 0 )
	{
		if( m_bHeader )
		{
			if( static_cast<uint>(uByteAbailable) < HEAD_SIZE ) break;
			stHsmsHeader header;
			qint64 nRead = m_pSocket->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				emit SigRecvError();
				emit SigLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1 receive header error!").arg(m_szHostIp) );
				return;
			}

			int nHeadMsgLen = 0x0;
			nHeadMsgLen |= (header.szMsgLength[0] << 24 );
			nHeadMsgLen |= (header.szMsgLength[1] << 16 );
			nHeadMsgLen |= (header.szMsgLength[2] <<  8 );
			nHeadMsgLen |= (header.szMsgLength[3] <<  0 );

			int nDataLen = nHeadMsgLen - MESSAGE_HEADER_SIZE;
			m_pBuffer = new char [ nHeadMsgLen + HEAD_MSG_BYTES ];
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = nDataLen;
			REMAIN_SIZE     = nDataLen;
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
				emit SigRecvError();
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
			stHsmsElement element;
			memcpy( (char*)&element, m_pBuffer        , HEAD_SIZE  );
			element.pData = nullptr;
			if( DATA_SIZE > 0 )
			{
				element.pData = new char[DATA_SIZE];
				memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
			}
			emit SigReceivePacket( element );

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

bool CHsmsClient::SendPacket( char* buffer, uint size )
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
			emit SigSendError();
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

void CHsmsClient::SlotSendPacket( char* buf, uint size )
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
