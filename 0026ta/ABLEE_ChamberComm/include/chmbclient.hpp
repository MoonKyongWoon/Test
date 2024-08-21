#ifndef CCHMBCLIENT_HPP
#define CCHMBCLIENT_HPP

#include <QObject>
#include <QLocalSocket>

#include "commstruct.hpp"
#include "protocol.hpp"
#include "syslog.hpp"

class CChmbClient : public QObject
{
	Q_OBJECT
public:
	explicit CChmbClient(uint id, quintptr, QObject *parent = nullptr);
	~CChmbClient();
public:
	bool DisconnectServer();
signals:
	void SigIpcLogMessage(uint, LOG_TYPE, QString);
	void SigIpcError( uint, QLocalSocket::LocalSocketError, QString );
	void SigIpcConnectionStatus(uint, PLC_ON_OFFLINE);
	void SigIpcRecvPacket( uint, stIpcElement );
public slots:
	void SlotIpcDisonnected();
	void SlotIpcReadyRead();
	void SlotIpcClientError(QLocalSocket::LocalSocketError socketError);
	void SlotSendPacket( uint, char*, uint );
private:
	uint m_nKeyId;
	QLocalSocket *m_ipcClient;
	QByteArray m_baBuffer;
	char* m_pBuffer;
	bool m_bConnected;
	bool m_bHeader;
private:
	int DATA_SIZE  ;
	int REMAIN_SIZE;
	int INSERT_POS ;
};
#endif
