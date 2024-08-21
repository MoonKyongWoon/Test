#ifndef CLOCALCLIENT_HPP
#define CLOCALCLIENT_HPP

#include <QObject>
#include <QLocalSocket>

#include "commstruct.hpp"
#include "protocol.hpp"
#include "syslog.hpp"

class CLocalClient : public QObject
{
	Q_OBJECT
public:
	explicit CLocalClient(quintptr, QObject *parent = nullptr);
	~CLocalClient();
public:
	bool DisconnectServer();
signals:
	void SigIpcLogMessage(LOG_TYPE, QString);
	void SigIpcError( QLocalSocket::LocalSocketError, QString );
	void SigIpcConnectionStatus(bool);
	void SigIpcRecvPacket( stIpcElement );
public slots:
	void SlotIpcDisconnected();
	void SlotIpcReadyRead();
	void SlotIpcClientError(QLocalSocket::LocalSocketError socketError);
	void SlotSendPacket( char*, uint );
private:
	QLocalSocket *m_ipcClient;
	bool m_bConnected;
	bool m_bHeader;
	QString m_sIpcServer;
	QByteArray m_baBuffer;
	char* m_pBuffer;
private:
	int DATA_SIZE  ;
	int REMAIN_SIZE;
	int INSERT_POS ;
};
#endif
