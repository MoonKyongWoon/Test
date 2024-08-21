#include "localsocket.hpp"


CLocalSocket::CLocalSocket( QString name, QObject *parent) : QObject(parent), m_sName(name)
{
	m_bConnected = false;
	m_bHost = false;
	m_bHeader = true;
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
	m_pIpc  = new QLocalSocket(this);
	connect( m_pIpc, &QLocalSocket::errorOccurred, this, &CLocalSocket::SlotIpcClientError );
	connect( m_pIpc, &QLocalSocket::readyRead    , this, &CLocalSocket::SlotIpcReadyRead   );
	connect( m_pIpc, &QLocalSocket::connected    , this, &CLocalSocket::SlotIpcCnnected    );
	connect( m_pIpc, &QLocalSocket::disconnected , this, &CLocalSocket::SlotIpcDisonnected );
}

CLocalSocket::~CLocalSocket()
{
	CloseSocket();
}

void CLocalSocket::CloseSocket()
{
	if( m_pIpc )
	{
		if( m_pIpc->state() == QLocalSocket::LocalSocketState::ConnectedState )
		{
			m_pIpc->disconnectFromServer();
		}
		delete m_pIpc;
		m_pIpc = nullptr;
	}
	return;
}

QString CLocalSocket::GetName()
{
	return m_sName;
}

void CLocalSocket::SetHostInfo( QString uds )
{
	m_sIpcServer = uds;
	m_pIpc->setServerName( uds );
	m_bHost = true;
	//qInfo() << m_sName << " LOCAL SOCKET : " << m_pIpc->serverName();
}

bool CLocalSocket::IsConn()
{
	return m_bConnected;
}

void CLocalSocket::SlotConnectToHost()
{
	if( !m_bHost )
	{
		emit SigIpcLogMessage( _NORMAL_LOG, tr("[INFO][%1] server name is not set").arg(m_sName) );
		return;
	}

	if( m_bConnected ) return;

	emit SigIpcLogMessage( _NORMAL_LOG, tr("[INFO][%1] connect to %2").arg(m_sName).arg(m_pIpc->serverName()) );
	m_pIpc->connectToServer();
}

////////////////////////////////////////////////////////////////////////////////////////
/// \brief CIPCClient::SlotIpcClientError
/// \param socketError
/// \details Local Socket error occurred
void CLocalSocket::SlotIpcClientError(QLocalSocket::LocalSocketError socketError)
{
	switch (socketError)
	{
		case QLocalSocket::ConnectionRefusedError:
			emit SigIpcError( socketError,
									 tr("The connection was refused by the peer."));
			break;

		case QLocalSocket::PeerClosedError       :
			break;

		case QLocalSocket::ServerNotFoundError   :
			emit SigIpcError( socketError,
									 tr("The host(%1) was not found.").arg(m_sName));
			break;

		case QLocalSocket::SocketAccessError     :
			emit SigIpcError( socketError,
									 tr("The socket operation failed because the application\n"
										"lacked the required privileges.") );
			break;

		case QLocalSocket::SocketResourceError   :
			emit SigIpcError( socketError,
									 tr("The local system ran out of resources ") );
			break;

		case QLocalSocket::SocketTimeoutError    :
			emit SigIpcError( socketError,
									 tr("The socket operation timed out.") );
			break;

		case QLocalSocket::DatagramTooLargeError :
			emit SigIpcError( socketError,
									 tr("The datagram was larger than the operating system's limit \n"
										"(which can be as low as 8192 bytes).") );
			break;

		case QLocalSocket::ConnectionError       :
			emit SigIpcError( socketError,
									 tr("An error occurred with the connection") );
			break;

		case QLocalSocket::UnsupportedSocketOperationError:
			emit SigIpcError( socketError,
									 tr("The requested socket operation is not supported by \n"
										"the local operating system.") );
			break;

		case QLocalSocket::OperationError        :
			emit SigIpcError( socketError,
									 tr("An operation was attempted while the socket was \n"
										"in a state that did not permit it.") );
			break;


		case QLocalSocket::UnknownSocketError    :
			emit SigIpcError( socketError,
									 tr("An unidentified error occurred.") );
			break;
	}
}

void CLocalSocket::SlotIpcCnnected()
{
	m_bConnected = true;
	emit SigIpcLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][%1] %2 connected!").arg(m_sName).arg(m_sIpcServer) );
	emit SigIpcConnectionStatus(true);
}


void CLocalSocket::SlotIpcDisonnected()
{
	m_bConnected = false;
	emit SigIpcLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][%1] %2 disconnected!").arg(m_sName).arg(m_sIpcServer) );
	emit SigIpcConnectionStatus(false);
	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
}

#define HEAD_SIZE		sizeof(stIpcHeader)
void CLocalSocket::SlotIpcReadyRead()
{
	static int DATA_SIZE   = 0x0;
	static int REMAIN_SIZE = 0x0;
	static int INSERT_POS  = 0x0;
	qint64 uByteAbailable  = m_pIpc->bytesAvailable();
	while( uByteAbailable > 0 )
	{
		//if (dwSizeRead > PROTOCOL::MAX_TCP_BUFFER) dwSizeRead = PROTOCOL::MAX_IPC_BUFFER;
		if( m_bHeader )
		{
			if( static_cast<uint>(uByteAbailable) < HEAD_SIZE ) break;

			stTcpHeader header;
			qint64 nRead = m_pIpc->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				emit SigIpcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1 receive header error!").arg(m_pIpc->serverName()) );
				return;
			}
			m_pBuffer = new char [ HEAD_SIZE + header.datalen ];
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = header.datalen;
			REMAIN_SIZE     = header.datalen;
			m_bHeader = false;
			//qDebug() << "[IPC]HEAD " << "READ SIZE : " << HEAD_SIZE;
		}
		else
		{
			qint64 nRead = m_pIpc->read( m_pBuffer+INSERT_POS, REMAIN_SIZE );
			if( nRead < 0 )
			{
				emit SigIpcLogMessage( _ERROR_LOG , tr("[RECV][ ERR] %1 receive data error!").arg(m_pIpc->serverName()) );
				return;
			}
			uByteAbailable -= nRead;
			REMAIN_SIZE    -= nRead;
			INSERT_POS     += nRead;
			//qDebug() << "[IPC]BODY " << "READ SIZE : " << nRead;
		}

		if( !m_bHeader && (REMAIN_SIZE == 0) )
		{
			stIpcElement element;
			memcpy( (char*)&element, m_pBuffer        , HEAD_SIZE  );
			element.pData = nullptr;
			if( DATA_SIZE > 0 )
			{
				element.pData = new char[DATA_SIZE];
				memset( element.pData, 0x0, DATA_SIZE );
				memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
			}
			emit SigIpcRecvPacket( element );
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_bHeader = true;
			DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
		}
	}
}

void CLocalSocket::SlotSendPacket( char* pData, uint size )
{
	qint64 tosend = size;
	char* buf = pData;
	while (tosend > 0)
	{
		qint64 nSendByte = m_pIpc->write( buf, tosend );
		if ( nSendByte == QLocalSocket::UnknownSocketError )
		{
			emit SigIpcLogMessage( _ERROR_LOG , tr("[ ERR][%1] Send to %2 error!").arg(m_sName).arg(m_pIpc->serverName()) );
			//delete[] buf;
			return;
		}
		tosend -= nSendByte;
		buf    += nSendByte;
	}
	m_pIpc->flush();
	m_pIpc->waitForBytesWritten(5);
	if( pData )
	{
		delete[] pData;
		pData = nullptr;
	}
}
