#ifndef CCLIENTSIMM_HPP
#define CCLIENTSIMM_HPP

#include <QObject>
#include <QTimer>
#include <QList>
#include "hsmssocket.hpp"
#include "eaputil.hpp"

class CClientSimm : public QObject
{
	Q_OBJECT
private:
	CHsmsSocket					*socket;
	HSMS_STATUS					hsms_status;
	uint						system_bytes;
	QList<stControlListItem*>	*control_message_list;
	QTimer						*timer;

public:
	CClientSimm();
	~CClientSimm();

private:
	void initVariable();
	void initTimer();
	void createSocket();
	void createList();
	void clearMessageList();
	void addItemToList(TIME_OUT_TYPE type, int system_byte);
	void createTimer();
	void startTimer();

	void setHsmsStatus(HSMS_STATUS status);

	void hsmsSelect(stHsmsElement element);
	void hsmsDeselect(stHsmsElement element);

	void makeSelectRequestMessage(stHsmsElement &element);
	void makeDeselectRequestMessage(stHsmsElement &element);
	void makeSeperateRequestMessage(stHsmsElement &element);
	void makeLinkTestRequestMessage(stHsmsElement &element);
	void makeRejectRequestMessage(stHsmsElement &element);

public:
	uint getSystemByte();

signals:
	void sigConnectToHost();
	void sigDisconnectFromHost();
	void sigSetTcpStatus(bool bConn);
	void sigSetHsmsStatus(bool bSelect);
	void sigAppendSystemLog(QString log);
	void sigAppendBinLog(QString log);
	void sigAppendSmlLog(QString log);
	void sigSendPacket(char* buffer, uint size);

public slots:
	void slotStatusChanged(bool bConn);
	void slotConnectToHost();
	void slotDisconnectFromHost();
	void slotSendControlMessage(SType s_type);
//	void slotSendDataMessage(QByteArray data);
	void slotRecvPacket(stHsmsElement element);
	void slotCheckList();
};

#endif // CCLIENTSIMM_HPP
