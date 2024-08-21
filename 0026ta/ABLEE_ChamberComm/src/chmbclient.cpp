#include "chmbclient.hpp"


CChmbClient::CChmbClient(uint id, quintptr handle, QObject *parent) : QObject(parent), m_nKeyId(id)
{
	m_bHeader   = true;
	m_ipcClient = new QLocalSocket(this);
	if( !m_ipcClient->setSocketDescriptor( handle ) )
		return;
	connect( m_ipcClient, &QLocalSocket::readyRead    , this, &CChmbClient::SlotIpcReadyRead   );
	connect( m_ipcClient, &QLocalSocket::errorOccurred, this, &CChmbClient::SlotIpcClientError );
	connect( m_ipcClient, &QLocalSocket::disconnected , this, &CChmbClient::SlotIpcDisonnected );

	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
	m_bConnected = true;
	emit SigIpcConnectionStatus( id, PLC_ONLINE );
}

CChmbClient::~CChmbClient()
{
	DisconnectServer();
}

bool CChmbClient::DisconnectServer()
{
	if( !m_ipcClient  ) return true;
	if( !m_bConnected ) return true;
	if( m_ipcClient )
	{
		if( m_ipcClient->state() == QLocalSocket::LocalSocketState::ConnectedState )
		{
			m_ipcClient->disconnectFromServer();
		}
		delete m_ipcClient;
		m_ipcClient = nullptr;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
/// \brief CIPCClient::SlotIpcClientError
/// \param socketError
/// \details Local Socket error occurred
void CChmbClient::SlotIpcClientError(QLocalSocket::LocalSocketError socketError)
{
	switch (socketError)
	{
		case QLocalSocket::ConnectionRefusedError:
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The connection was refused by the peer."));
			break;

		case QLocalSocket::PeerClosedError       :
			break;

		case QLocalSocket::ServerNotFoundError   :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The host was not found."));
			break;

		case QLocalSocket::SocketAccessError     :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The socket operation failed because the application\n"
										"lacked the required privileges.") );
			break;

		case QLocalSocket::SocketResourceError   :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The local system ran out of resources ") );
			break;

		case QLocalSocket::SocketTimeoutError    :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The socket operation timed out.") );
			break;

		case QLocalSocket::DatagramTooLargeError :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The datagram was larger than the operating system's limit \n"
										"(which can be as low as 8192 bytes).") );
			break;

		case QLocalSocket::ConnectionError       :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("An error occurred with the connection") );
			break;

		case QLocalSocket::UnsupportedSocketOperationError:
			emit SigIpcError( m_nKeyId, socketError,
									 tr("The requested socket operation is not supported by \n"
										"the local operating system.") );
			break;

		case QLocalSocket::OperationError        :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("An operation was attempted while the socket was \n"
										"in a state that did not permit it.") );
			break;


		case QLocalSocket::UnknownSocketError    :
			emit SigIpcError( m_nKeyId, socketError,
									 tr("An unidentified error occurred.") );
			break;
	}
}

void CChmbClient::SlotIpcDisonnected()
{
	m_bConnected = false;
	//emit SigIpcLogMessage( m_nKeyId, LOG_TYPE::_ERROR_LOG, tr("[CONN] IPC Client disconnected!") );
	emit SigIpcConnectionStatus(m_nKeyId, PLC_OFFLINE);

	DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
}

#define HEAD_SIZE		sizeof(stIpcHeader)
void CChmbClient::SlotIpcReadyRead()
{
	//static int DATA_SIZE   = 0x0;
	//static int REMAIN_SIZE = 0x0;
	//static int INSERT_POS  = 0x0;
	qint64 uByteAbailable  = m_ipcClient->bytesAvailable();
	while( uByteAbailable > 0 )
	{
		//if (dwSizeRead > PROTOCOL::MAX_TCP_BUFFER) dwSizeRead = PROTOCOL::MAX_TCP_BUFFER;
		if( m_bHeader )
		{
			if( static_cast<uint>(uByteAbailable) < HEAD_SIZE ) break;
			stIpcHeader header;
			qint64 nRead = m_ipcClient->read( (char*)&header, HEAD_SIZE );
			if( nRead != HEAD_SIZE )
			{
				emit SigIpcLogMessage( m_nKeyId, _ERROR_LOG , tr("[RECV][ ERR] %1 receive header error!").arg(m_ipcClient->serverName()) );
				return;
			}
			m_pBuffer = new char [ HEAD_SIZE + header.datalen ];
			memset( m_pBuffer, 0x0, HEAD_SIZE + header.datalen );
			memcpy( m_pBuffer, (char*)&header, HEAD_SIZE );
			INSERT_POS      = HEAD_SIZE;
			uByteAbailable -= HEAD_SIZE;
			DATA_SIZE       = header.datalen;
			REMAIN_SIZE     = header.datalen;
			m_bHeader = false;
		}
		else
		{
			qint64 nRead = m_ipcClient->read( m_pBuffer+INSERT_POS, REMAIN_SIZE );
			if( nRead < 0 )
			{
				emit SigIpcLogMessage( m_nKeyId, _ERROR_LOG , tr("[RECV][ ERR] %1 receive data error!").arg(m_ipcClient->serverName()) );
				return;
			}
			uByteAbailable -= nRead;
			REMAIN_SIZE    -= nRead;
			INSERT_POS     += nRead;
		}

		if( !m_bHeader && (REMAIN_SIZE == 0) )
		{
			stIpcElement element;
			if( m_pBuffer )
			{
				memcpy( (char*)&element, m_pBuffer        , HEAD_SIZE  );
				element.pData = nullptr;
				if( DATA_SIZE > 0 )
				{
					element.pData = new char[DATA_SIZE];
					memcpy( element.pData, m_pBuffer+HEAD_SIZE, DATA_SIZE );
				}
				emit SigIpcRecvPacket( m_nKeyId, element );
				delete[] m_pBuffer;
				m_pBuffer = nullptr;
				m_bHeader = true;
				DATA_SIZE = REMAIN_SIZE = INSERT_POS = 0;
			}
		}
	}
}

void CChmbClient::SlotSendPacket( uint key, char* pData, uint size )
{
	if( key != m_nKeyId ) return;

	qint64 tosend = size;

	if( !pData ) return;
	char* buf = pData;
	while (tosend > 0)
	{
		qint64 nSendByte = m_ipcClient->write( buf, tosend );
		if ( nSendByte == QLocalSocket::UnknownSocketError )
		{
			emit SigIpcLogMessage( m_nKeyId, _ERROR_LOG , tr("[SEND][ ERR] Send to %1 error!").arg(m_ipcClient->serverName()) );
			delete[] buf;
			return;
		}
		tosend -= nSendByte;
		buf    += nSendByte;
	}
	m_ipcClient->flush();
	m_ipcClient->waitForBytesWritten(5);

	delete[] pData;
}
