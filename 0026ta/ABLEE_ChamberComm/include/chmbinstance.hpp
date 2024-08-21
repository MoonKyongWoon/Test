#ifndef CCHMBINSTANCE_HPP
#define CCHMBINSTANCE_HPP

#include <QObject>
#include "localclient.hpp"
#include "yokogawaplc.hpp"

class CChmbInstance : public QObject
{
	Q_OBJECT
public:
	explicit CChmbInstance(quintptr, QObject *parent = nullptr);
	~CChmbInstance();
public:
	void SetPlcTcpInfo(QString, uint);
signals:
	void SigIpcDisconnected(QString);
	void SigIpcSendPacket(char*,uint);
public slots:
	void SlotIpcStatusChanged(bool);
	void SlotIpcRecvPacket(stIpcElement);
	void SlotIpcLogMessage(LOG_TYPE, QString);
public slots:
	void SlotPlcLogMessage(LOG_TYPE, QString);
private:
	CLocalClient* m_pLocIpc;
	CYokogawaPlc* m_pYkgPlc;
	QString m_sIpcName;
};

#endif // CCHMBINSTANCE_HPP
