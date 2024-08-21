#include "PLCClient.hpp"

CPLCClient::CPLCClient(int id, qintptr handle, QObject *parent) : QObject(parent), m_nClientNo(id), m_handle(handle)
{
    m_pSocket = new QTcpSocket;
    if( m_pSocket->setSocketDescriptor( m_handle ) )
    {
        m_sClientIp = m_pSocket->peerAddress().toString();
        connect( m_pSocket, &QTcpSocket::readyRead   ,  this, &CPLCClient::SlotReadyRead    );
        connect( m_pSocket, &QTcpSocket::disconnected,  this, &CPLCClient::SlotDisconnected );
    }
}

CPLCClient::~CPLCClient()
{
    CloseSocket();
}

QString CPLCClient::GetIp()
{
    return  m_sClientIp;
}

void CPLCClient::DoEmit()
{
    if( IsValidSocket() )
        emit SigClientConnected( m_nClientNo, m_sClientIp );
    else
        emit SigClientDisconnected( m_nClientNo );
}

void CPLCClient::SlotNewConnection(qintptr handle)
{

}

void CPLCClient::CloseSocket()
{
    if( m_pSocket )
    {
        if( m_pSocket->state() == QAbstractSocket::ConnectedState )
        {
            m_pSocket->close();
            m_pSocket->deleteLater();
        }
    }
}

bool CPLCClient::IsValidSocket()
{
    if( !m_pSocket ) return false;
    return (m_pSocket->state() == QAbstractSocket::ConnectedState);
}

void CPLCClient::SlotReadyRead()
{
    if ( !m_pSocket->canReadLine() )
        m_pSocket->waitForReadyRead( 100 );

    while( m_pSocket->canReadLine() )
    {
        QByteArray ba = m_pSocket->readLine();
        QString sLog = QString(ba);
        sLog = sLog.replace("\r\n","");
		emit SigTcpMessageLog( QString("").asprintf("[ID%04d][RECV][INFO] %s", m_nClientNo, sLog.toLatin1().data()), true );
        emit SigRecvPacket(m_nClientNo, ba);
    }
}

void CPLCClient::SlotDisconnected()
{
    emit SigClientDisconnected( m_nClientNo );
}

void CPLCClient::SlotSendPacket(int id, QByteArray buffer, int length)
{
	if( id != m_nClientNo ) return;

	qint64 tosend = static_cast<qint64>(length);
	const char* buf = buffer.constData();
	while (tosend > 0)
	{
		qint64 nSendByte = m_pSocket->write( buf, tosend );
		if (nSendByte == QAbstractSocket::UnknownSocketError)
		{
			emit SigTcpMessageLog( QString("").asprintf("[ID%04d][SEND][ ERR] %s", m_nClientNo, buffer.constData()), true );
			return;
		}
		tosend -= nSendByte;
		buf    += nSendByte;
	}
	m_pSocket->waitForBytesWritten(5);
	m_pSocket->flush();

	QString sLog = QString(buffer);
	sLog = sLog.replace("\r\n","");
	emit SigTcpMessageLog( QString("").asprintf("[ID%04d][SEND][INFO] %s", m_nClientNo, sLog.toLatin1().data()), true );
}
