#ifndef CDEVDATAMGR_HPP
#define CDEVDATAMGR_HPP

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QVector>
#include "common.hpp"
#include "logger.hpp"
#include "protocol.hpp"
#include "tcpserver.hpp"
#include "tcpclient.hpp"

class CDevDataMgr : public QObject
{
	Q_OBJECT
public:
	explicit CDevDataMgr(QObject *parent = nullptr);
	~CDevDataMgr();
public slots:
	void SlotRunService();
public slots:
	void SlotTcpConnected(quintptr);
	void SlotTcpStatusChanged(bool);
	void SlotTcpRecvPacket(stTcpElement);
	void SlotTcpLogMesage( LOG_TYPE, QString );
signals:
	void SigDevLogMessage( LOG_TYPE, QString );
	void SigDevStatusChanged( uint, uint, bool );
	void SigDevRecvPacket( stTcpElement );
signals:
	void SigDevSendPacket( char*, uint );
    void SigSaveSlotLog( uint, uint, LOG_TYPE, QString );
public slots:
	void SlotDevDataSendPacket( stTcpElement );
public:
	void SetLocalCommIp( QString );
	void SetSlotInfo( stSlotInfo slotinfo );
private:
	bool RunTcpDataServer();
private:
	bool m_bHasIp;
	QString m_sIp;
	stSlotInfo m_stSlotInfo;
	CTcpServer* m_pTcpSvr;
	QVector<CTcpClient*> m_vecTcpClient;
};

#endif // CDEVDATAMGR_HPP
