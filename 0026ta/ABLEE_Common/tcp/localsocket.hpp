#ifndef CLOCALSOCKET_HPP
#define CLOCALSOCKET_HPP

#include <QObject>
#include <QLocalSocket>

#include "commstruct.hpp"
#include "protocol.hpp"
#include "syslog.hpp"

class CLocalSocket : public QObject
{
	Q_OBJECT
public:
	explicit CLocalSocket( QString, QObject *parent = nullptr );
	~CLocalSocket();
public:
	void SetHostInfo( QString );
	void CloseSocket();
	bool IsConn();
	QString GetName();
signals:
	void SigIpcLogMessage(LOG_TYPE, QString);
	void SigIpcError( QLocalSocket::LocalSocketError, QString );
	void SigIpcConnectionStatus(bool);
	void SigIpcRecvPacket( stIpcElement );
public slots:
	void SlotConnectToHost();
	void SlotIpcCnnected();
	void SlotIpcDisonnected();
	void SlotIpcReadyRead();
	void SlotIpcClientError(QLocalSocket::LocalSocketError socketError);
	void SlotSendPacket( char*, uint );
private:
	QString m_sName;
	QLocalSocket *m_pIpc;
	bool m_bConnected;
	bool m_bHeader;
	bool m_bHost;
	char* m_pBuffer;
	QString m_sIpcServer;
	QByteArray m_baBuffer;
private:
	int DATA_SIZE  ;
	int REMAIN_SIZE;
	int INSERT_POS ;
};
#endif
