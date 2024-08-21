#ifndef CHSMSSOCKET_HPP
#define CHSMSSOCKET_HPP

#include <QObject>
#include <QHostAddress>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>

#include "commstruct.hpp"
#include "hsms.hpp"
#include "syslog.hpp"
#include "protocol.hpp"
#include "util.hpp"

class CHsmsSocket : public QObject
{
    Q_OBJECT
public:
	explicit CHsmsSocket(int id=0, int cpu=0);
	~CHsmsSocket();
public:
	void SetHostInfo( QString, quint16 );
	void CloseSocket();
	void GetUniqueId(int&,int&);
	bool IsConn();
	bool SendPacket( char*, uint );
	void Disconnect();

public slots:
	void SlotTcpReadyRead();
	void SlotTcpSocketConnected();
	void SlotTcpSocketDisconnected();

public slots:
	void SlotConnectToHost();
	void SlotDisconnectFromHost();
	void SlotSendPacket( char*, uint );
signals:
	void SigStatusChanged( bool );
	void SigReceivePacket( stHsmsElement );
	void SigLogMessage( LOG_TYPE, QString );
private:
	int  m_nId0;
	int  m_nId1;
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
