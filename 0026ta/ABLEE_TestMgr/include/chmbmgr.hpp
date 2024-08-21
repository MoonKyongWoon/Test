/*************************************************************************
 * DEVICE PGM CHAMBER CONTROL
 * MAIN UI CHAMBER CONTROL
*************************************************************************/
#ifndef CCHMBMGR_HPP
#define CCHMBMGR_HPP

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include "math.h"
#include "yokogawaplc.hpp"

enum EN_CHMB_TEST_STAT
{
    EN_CHMB_STANDBY ,
    EN_CHMB_TESTING ,
    EN_CHMB_TESTDONE,
	EN_CHMB_HEATING_AMB,	//2023.08.17 - KWMOON
};

class CChmbMgr : public QObject
{
	Q_OBJECT
public:
	explicit CChmbMgr(QObject *parent = nullptr);
	~CChmbMgr();
private:
	void Init();
	void LoadChmbIni();
    void InitTimer();
private:
	void InitSlotSync(uint,uint);
	void SetTestStat(uint,uint,EN_CHMB_TEST_STAT);
	void DoDevInitTemp( PROTOCOL::TCP_COMMAND, stIpcElement );
	void DoDevEnter( PROTOCOL::TCP_COMMAND, stIpcElement );
	void DoDevSetTempnValid( PROTOCOL::TCP_COMMAND, stIpcElement );
	void DoDevChamberRun( PROTOCOL::TCP_COMMAND, stIpcElement );
	void DoDevControlChamber( PROTOCOL::TCP_COMMAND, stIpcElement );
private:
	bool SetChamberUse(bool);
	bool DoChmbSvSet(double);
	bool DoChmbAlarmReset();
	bool DoChmbRun();
	bool DoChmbStop();
	bool DoChmbAmb();
    bool DoChmbMaskOnOff(bool onoff);
	bool DoChmbRackOprTester(bool);
    bool DoChmbTestDone();
    bool DoWatchdogTimeSet(uint);
    bool DoWatchdogOnoff(bool);
    bool DoWatchdog();
	bool DoChmbDoorOpen();
	bool DoChmbDoorClose();
	bool DoChmbRackIn();
	bool DoChmbRackOut();
	bool DoChmbDoorCloseRead(bool&);
	bool DoChmbRackInRead(bool&);
	bool DoChmbTempRead(double& sv, double& pv);		//2024.08.14 - KWMOON
private:
	bool DoChmbBitWrite ( PROTOCOL::PLC_COMMAND, uint  );
	bool DoChmbBitRead  ( PROTOCOL::PLC_COMMAND, uint& );
	bool DoChmbWordWrite( PROTOCOL::PLC_COMMAND, uint  );
	bool DoChmbWordRead ( PROTOCOL::PLC_COMMAND, uint& );
	void SetTestSlots(uint);	//2023.08.18 - KWMOON
public:
	void InitChmbSync(unsigned int bitslot=0xFFFF);
	void SetSlotInfo( stSlotInfo slotinfo );
public slots:
	void SlotChmbMgrStart();
	void SlotSetLivePgmList(stPgmLive);
	void SlotTestDone();
public slots:
	void SlotChamberUse(bool);
	void SlotRecvChamberCmd(char*, uint);
    void SlotRecvTestCmd(char*, uint);
public slots:
    void SlotTimeoutChmb();
signals:
	void SigChmbConnStatus(bool);
	void SigChmbMgrAck( stTcpElement );
	void SigChmbMgrLogMessage( LOG_TYPE, QString );
	void SigChmbTempMonSet( stGetTemp );
	void SigChmbTempMonEnd( stGetTemp );

private:
	stSlotInfo m_stSlotInfo;
    stPgmLive  m_PgmLive;
private:
	uint m_nId;
	CYokogawaPlc* m_pYkgPlc;
	stChmbMgrSv m_stChmbSv[MAX_SLOT][MAX_SYS_CPU];
    EN_CHMB_TEST_STAT m_bTest[MAX_SLOT][MAX_SYS_CPU];
private:
	QString m_sChamberIp;
	uint m_nChamberPort;
	double m_dAmbSV;
	double m_dTempLimitHigh;
	double m_dTempLimitLow;
	bool   m_bChamberUse;
	bool   m_nWatchdogPeriod;
private:
    QTimer* m_timerChmb;
    QMutex m_mutexChmb;
private:
	bool   m_bChamberRun;
	bool   m_dCurrentSV;
	uint   m_uTestSlots;		//2023.08.18 - KWMOON
	bool   m_bChmbMonSet;
	bool   m_bChmbMonEnd;
};

#endif // CCHMBMGR_HPP
