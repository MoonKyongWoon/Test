#ifndef CCHMBMANAGER_HPP
#define CCHMBMANAGER_HPP

#include <QMap>
#include <QObject>
#include <QTimer>
#include <QThread>

#include "yokogawaplc.hpp"
#include "chmbclient.hpp"
#include "logger.hpp"
#include "localserver.hpp"
#include "localclient.hpp"
#include "systype.hpp"

#define MY_UNIQUE_ID        (0x1)

class CChmbManager : public QObject
{
	Q_OBJECT
public:
	explicit CChmbManager(QObject *parent = nullptr);
	~CChmbManager();
public:
	int run(int argc, char* argv[]);
	void DeleteInstance();
private:
	void Init();
	void InitTimer();
	void LoadChmbIni();
	void StandByDirectory();
	void RegisterMetaType();
	bool RunLocalServer();
	uint GenerateUniqueId();
	bool SendPlcStatusToIpcs(uint, PLC_ON_OFFLINE);
    void InitChamber();
signals:
	void SigSaveLog( LOG_TYPE, QString );
	void SigIpcSendPacket (uint, char*, uint);
signals:
	void SigPlcSocketControl( PLC_ON_OFFLINE );
public slots:
	void SlotPlcChmbResponse( uint, stPlcResponse );
public slots:
	void SlotIpcLogSave( uint, LOG_TYPE, QString );
	void SlotIpcConnected(quintptr);
	void SlotIpcConnectionStatus(uint, PLC_ON_OFFLINE);

public:
	void SlotPlcConnStatus( PLC_ON_OFFLINE );
	void SlotTimeoutPlcCon();

private:
	QString m_sChamberIp;
	quint16 m_uChamberPort;
	double  m_dTempLimitLow;
	double  m_dTempLimitHigh;
	double  m_dWatchdogPeriod;
private:
	bool m_bDebugPrint;
    uint m_nMyKey;
	QString m_sCurrentDir;	
private:
	CLogger* m_pLogger;
	CLocalServer* m_pIpcSvr;
	CYokogawaPlc* m_pYkgPlc;
	QThread* m_pYkgPlcTh;
	QMap<uint, CChmbClient*> m_mapIpc;
	QTimer* m_timerPlcCon;
};

#endif // CCHMBMANAGER_HPP
