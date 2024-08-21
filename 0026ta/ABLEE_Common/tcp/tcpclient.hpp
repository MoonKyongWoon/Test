#ifndef CTCPCLIENT_HPP
#define CTCPCLIENT_HPP

#include <QObject>
#include <QHostAddress>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "commstruct.hpp"
#include "protocol.hpp"
#include "syslog.hpp"
#include "util.hpp"

class CTcpClient : public QObject
{
    Q_OBJECT
public:
    explicit CTcpClient( qintptr );
	~CTcpClient();
public:
	QString GetHostIp();
	void CloseSocket();
	void SetUniqueId(uint,uint);
	void GetUniqueId(uint&,uint&);
	bool IsHaveId();
	bool SendPacket( char*, uint );
public slots:
    void SlotReadyRead();
    void SlotSendPacket( char*, uint );
	void SlotSocketDisconnected();
	void SlotSocketStateChanged(QAbstractSocket::SocketState);
signals:
	void SigStatusChanged(bool);
	void SigReceivePacket( stTcpElement );
	void SigLogMessage( LOG_TYPE, QString );
private:
	bool m_bHaveId;
	uint  m_nId0;
	uint  m_nId1;
	int DATA_SIZE  ;
	int REMAIN_SIZE;
	int INSERT_POS ;
private:
    QTcpSocket* m_pSocket;
	char* m_pBuffer;
	bool m_bHeader;
	bool m_bConn;
	char    m_szHostIp[16];
	quint16 m_uHostPort;
};

#endif // CLIENTSOCKET_HPP
