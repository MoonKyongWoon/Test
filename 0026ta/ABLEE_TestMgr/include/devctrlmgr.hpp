#ifndef CDEVCTRLMGR_HPP
#define CDEVCTRLMGR_HPP

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QThread>
#include <QVector>
#include "common.hpp"
#include "protocol.hpp"
#include "tcpserver.hpp"
#include "tcpclient.hpp"

class CDevCtrlMgr : public QObject
{
	Q_OBJECT
public:
	explicit CDevCtrlMgr(QObject *parent = nullptr);
	~CDevCtrlMgr();
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
	void SigDevKeepAliveError( uint, uint );
signals:
	void SigDevSendPacket( char*, uint );
public slots:
	void SlotDevMgrSendPacket( stTcpElement );
	void SlotTimeoutKeepAlive();	//2023.08.10 - KWMOON
public:
	void SetLocalCommIp( QString );
	void SetSlotInfo( stSlotInfo slotinfo );
private:
	bool RunTcpServer();
	bool InitTimer();
	bool DisconnectSlotCpu( uint, uint );	//2023.08.10 - KWMOON
private:
	bool m_bHasIp;
	QString m_sIp;
	stSlotInfo m_stSlotInfo;
	CTcpServer* m_pTcpSvr;
	QVector<CTcpClient*> m_vecTcpClient;
private:
	QTimer* m_pTmrKeepAlive;								//2023.08.10 - KWMOON
	QDateTime m_dtKeepAlive[MAX_SLOT][MAX_SYS_CPU];		//2023.08.10 - KWMOON
	bool m_bConCpu[MAX_SLOT][MAX_SYS_CPU];				//2023.08.10 - KWMOON
};

#endif // CDEVCTRLMGR_HPP
