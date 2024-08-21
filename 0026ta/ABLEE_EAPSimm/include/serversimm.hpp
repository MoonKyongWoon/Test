#ifndef CSERVERSIMM_HPP
#define CSERVERSIMM_HPP


#include <QObject>
#include "tcpserver.hpp"
#include "hsmsclient.hpp"
#include "eaputil.hpp"
#include "secsgem.hpp"
class CServerSimm : public QObject
{
	Q_OBJECT
private:
	CTcpServer	*server;
	CHsmsClient	*client;
	HSMS_STATUS	hsms_status;
	uint		system_bytes;
	CSecsGem	*m_pSecs;
	bool		b_connect;
public:
	CServerSimm();
	~CServerSimm();

private:
	void initVariable();
	void createServer();
	void hsmsSelect(stHsmsElement element);
	void hsmsDeselect(stHsmsElement element);
	void hsmsLinkTest(stHsmsElement element);
	void setHsmsStatus(HSMS_STATUS status);

public:
	uint getSystemByte();
	bool getConnect();

signals:
	void sigSetTcpStatus(bool bConn);
	void sigSetHsmsStatus(bool bSelect);
	void sigAppendSystemLog(QString log);
	void sigAppendBinLog(QString log);
	void sigAppendHsmsInfo(QString log);
	void sigAppendSmlLog(QString log);
	void sigSendPacket(char* buffer, uint size);
    void sigRecvSystemBytes(uint);

public slots:
	void slotIncommingConnection(qintptr t);
	void slotStatusChanged(bool bConn);
	void slotReceivePacket(stHsmsElement element);
};

#endif // CSERVERSIMM_HPP
