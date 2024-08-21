#ifndef CTCPSOCKET_HPP
#define CTCPSOCKET_HPP

#include <QObject>
#include <QHostAddress>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "commstruct.hpp"
#include "syslog.hpp"
#include "protocol.hpp"
#include "util.hpp"

class CTcpSocket : public QObject
{
    Q_OBJECT
public:
	explicit CTcpSocket(int id=0, int cpu=0);
	~CTcpSocket();
public:
	void SetHostInfo( QString, quint16 );
	void CloseSocket();
	void GetUniqueId(int&,int&);
	bool IsConn();
	bool SendPacket( int,char*, uint );

public slots:
	void SlotTcpReadyRead();
	void SlotTcpSocketConnected();
	void SlotTcpSocketDisconnected();

public slots:
	void SlotConnectToHostA( int ,int );
	void SlotConnectToHost( int );
	void SlotSendPacketA( int, int, char*, uint );
	void SlotSendPacket( int, char*, uint );
signals:
	void SigStatusChanged(int,int,bool);
	void SigReceivePacket( int, int, stTcpElement );
	void SigLogMessage( int, int, LOG_TYPE, QString );
private:
	int  m_nId0;
	int  m_nId1;
	//QMutex m_mutex;
	int DATA_SIZE  ;
	int REMAIN_SIZE;
	int INSERT_POS ;
private:
	QTcpSocket* m_pSocket;
	QString m_sIp;
	quint16 m_uPort;
	bool m_bConn;
	bool m_bHost;
private:
	char* m_pBuffer;
	bool m_bHeader;
};

#endif // CLIENTSOCKET_HPP
