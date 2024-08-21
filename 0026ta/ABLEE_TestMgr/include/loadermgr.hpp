#ifndef CLOADERMGR_HPP
#define CLOADERMGR_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <thread>
#include <QThread>
#include <QVector>
#include "common.hpp"
#include "protocol.hpp"
#include "tcpsocket.hpp"

#define SSH_ID			"ablee"
#define SSH_PASSWD		"ablee"

class CLoaderMgr : public QObject
{
	Q_OBJECT
public:
	explicit CLoaderMgr(QObject *parent = nullptr);
	~CLoaderMgr();
private:
	void LoadFtpIni();
public:
	void SetSlotInfo( stSlotInfo );

signals:
	void SigLogMessage( LOG_TYPE, QString );
	void SigLoaderRecvPacket( int, int, stTcpElement );
	void SigLoaderAckConnSlots( stSlotLive );
	void SigTcpCommRun(int id0=-1);
public slots:
	void SlotTcpReceivePacket( int, int, stTcpElement );
	void SlotTcpStatusChanged(int, int, bool);
	void SlotTcpLogMessage(int, int, LOG_TYPE, QString );

public slots:
	void SlotMakeSockets();
	void SlotRun();
	void SlotStop();
	void SlotTimeOutConn();
	void SlotSendPacketToLoader( stTcpElement );
private:
	void StartConnTimer();
	bool IsSlotCpuConn( int, int);
	void SendSystemInfoToSlot(int,int);
	void SendFtpInfoToSlot(int,int);
	void SshPassSlotTimeSync(int,int);
private:
	QVector<CTcpSocket*> m_vClient;
	stSlotInfo m_stSlotInfo;
	stSlotLive m_stSlotLive;
	stFtpInfo m_stEwsFtpInfo;
	QTimer* m_pConnTimer;
	int m_nConnTime;
	int m_nCountOfClient;

};

#endif // CLOADERMGR_HPP
