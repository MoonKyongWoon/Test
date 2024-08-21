#ifndef CHAMBERVIEW_HPP
#define CHAMBERVIEW_HPP

#include <QAction>
#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <QPixmap>
#include <QSharedMemory>
#include <QTimer>
#include <QVector>

#include "common.hpp"
#include "dlgsv.hpp"
#include "localsocket.hpp"
#include "logger.hpp"
#include "syslog.hpp"
#include "sysini.hpp"
#include "passdlg.hpp"
#include "util.hpp"

#define LABEL_RUN_STYLE			"background-color:#00DC00;"
#define LABEL_STOP_STYLE		"background-color:#ff0000;"
#define LABEL_AMBIENT_STYLE		"background-color:#0080ff;"

#define LABEL_DISB_BG			"background-color:#ececec;"
#define LABEL_ENAB_BG			"background-color:#008080;"

#define PASS_TEXT_QCOLOR		QColor(  0,  0,  0)
#define FAIL_TEXT_QCOLOR		QColor(255, 77, 77)
#define FAULT_TEXT_QCOLOR		QColor(255,  0,  0)	//2023.03.18

#define WARN_ALARM_QCOLOR       QColor(255,195,  0)
#define HEAVY_ALARM_QCOLOR      QColor(255,  0,  0)

enum EN_TREE_HEADER
{
	HEADER_ALARM_DATE   = 0x0,
    HEADER_ALARM_UPDATE      ,
    HEADER_ALARM_LEVEL       ,
	HEADER_ALARM_NAME        ,
	HEADER_ALARM_INDEX       ,
	HEADER_ALARM_BIT         ,
};

enum EN_TIMER_VALUE
{
	TIMER_CHMB_COMM    = 5000,
	TIMER_CHMB_STAT    = 3000,
	TIMER_CHMB_COMP1_2 =10000,	//2024.08.19 - KWMOON
};

enum EN_CONNTION_PLC
{
	CONN_IPC,
	CONN_PLC,
	CONN_MAX,
};

enum EN_TAB_INDEX
{
    TAB_ALARM,
    TAB_SYSLOG,
    TAB_PLCLOG,
};

namespace Ui {
	class CChamberView;
}

typedef struct _stChmbStat
{
	bool bRun;
	bool bStop;
	bool bAmb;
	double dSv;
	double dPv;
}stChmbStat;

//2024.08.19 - KWMOON
typedef struct _stCompressure
{
	double dComp1PressHigh;
	double dComp1PressLow;
	double dComp1OutTemp;
	double dComp1InTemp;
	double dComp2PressHigh;
	double dComp2PressLow;
	double dComp2OutTemp;
	double dComp2InTemp;
}stCompressure;

class CChamberView : public QWidget
{
	Q_OBJECT

public:
	explicit CChamberView(QWidget *parent = nullptr);
	~CChamberView();
signals:
	void SigIpcChmbCommConnect();
	void SigIpcChmbCommSendPacket(char*,uint);
	void SigIpcChmbCommStatus(bool,bool);
private:
	void InitSharedMemory();
	bool IsSharedMemorySet();
	bool AppendTemperatureDat();
	void SetSharedMemoryStat(bool);
public slots:
	void SlotIpcChmbCommLogMessage (LOG_TYPE, QString);
	void SlotIpcChmbCommError ( QLocalSocket::LocalSocketError, QString );
	void SlotIpcChmbCommConnStatus(bool);
	void SlotIpcChmbCommRecvPacket( stIpcElement );
public slots:
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged();
public slots:
	void SlotStartChamberIpc();
	void SlotTimeoutChmbPlc();		//CHMB COMM <-> PLC CONN
    void SlotTimeoutChmbStat();
    void SlotTimeoutChmbWatchdog();
	void SlotTimeoutChmbCompressure();	//2024.08.19 - KWMOON
public slots:
	void SlotBtnRunClicked();
	void SlotBtnStopClicked();
	void SlotBtnAmbModeClicked();
	void SlotBtnAmbAutoClicked();
	void SlotBtnAlarmResetClicked();
	void SlotBtnMaskOnClicked();
	void SlotBtnMaskOffClicked();
	void SlotBtnWatchdogEnClicked();
    void SlotBtnWatchdogDisClicked();
	void SlotBtnDoorCloseClicked();
	void SlotBtnDoorOpenClicked();
	void SlotBtnRackInClicked();
	void SlotBtnRackOutClicked();
	void SlotBtnSetTempClicked();
	void SlotBtnSetAmbClicked();
	void SlotBtnRackOprTesterClicked();
	void SlotBtnRackOprChamberClicked();
	void SlotBtnBuzzerOffClicked();
	void SlotBtnBuzzerOnClicked();

	void SlotLampAllOffClicked();			//2023.05.24 - KWMOON
	void SlotLampLocalModeClicked();		//2023.05.24 - KWMOON
	void SlotLampRemoteModeClicked();		//2023.05.24 - KWMOON
	void SlotRedLampOnClicked();			//2023.05.24 - KWMOON
	void SlotYellowLampOnClicked();			//2023.05.24 - KWMOON
	void SlotGreenLampOnClicked();			//2023.05.24 - KWMOON
	void SlotRedLampBlinkClicked();			//2023.05.24 - KWMOON
	void SlotYellowLampBlinkClicked();		//2023.05.24 - KWMOON
	void SlotGreenLampBlinkClicked();		//2023.05.24 - KWMOON

    void SlotRedLampOffClicked();			//2023.05.25 - KWMOON
    void SlotYellowLampOffClicked();		//2023.05.25 - KWMOON
    void SlotGreenLampOffClicked();			//2023.05.25 - KWMOON
    void SlotRedLampBlinkOffClicked();		//2023.05.25 - KWMOON
    void SlotYellowLampBlinkOffClicked();	//2023.05.25 - KWMOON
    void SlotGreenLampBlinkOffClicked();	//2023.05.25 - KWMOON

	void SlotbtnRackInFingerUpOnClicked();			//2023.09.08 - KWMOON
	void SlotbtnRackInFingerUpOffClicked();			//2023.09.08 - KWMOON
	void SlotbtnDoorIgnoreRackInOutOnClicked();		//2023.09.08 - KWMOON
	void SlotbtnDoorIgnoreRackInOutOffClicked();	//2023.09.08 - KWMOON

	void SlotBtnDcPowerOnClicked();		//2023.09.19 - KWMOON
	void SlotBtnDcPowerOffClicked();	//2023.09.19 - KWMOON
public slots:
	void SlotSvSet( EN_SV_MODE, double );
	void SlotPasswdDlgAccepted(PASSWD_MODE);
private:
	void InitUi();
    void LoadChmbIni();
	void InitAlarmTree();
	void InitSysLogView();
	void InitPlcLogView();
	void InitButtons(bool);
	void InitLampMenu();
	void InitChmbStatus();
	void InitDisplay(double);
	void SetChmbRunStatus(EN_CHMB_RUN_STAT);
	void SetChmbStatus(EN_WORD_STATUS1,bool);
	void SetChmbStatus2(EN_WORD_STATUS2,bool);
	void SetChmbStatusRackOpr(EN_RACK_OPR_STAT);
	void InitInstance();
	void InitAlarmList();
	void InitTimer();
	void KillTimer();
	void DisplayChmbWordStatus( PROTOCOL::PLC_COMMAND, int );
	void DisplaySv( double );
	void DisplayPv( double );
	void DisplayPcw( double );
	void DisplayWps( double );
	void DisplayPcwFlow( double );
	bool IsExistAlarm( int, int );
	bool UpdateAlarm( int, int );
	bool RemoveAlarm( int, int );
	void MakeWordAlarmList ( PROTOCOL::PLC_COMMAND, int, int );
	void DisplayAlarmList();
	void InitLogger();

private:
	bool GetChmbStatus   ( PROTOCOL::PLC_COMMAND );
    bool SetChmbBitWrite ( PROTOCOL::PLC_COMMAND, int );
    bool SetChmbWordWrite( PROTOCOL::PLC_COMMAND, int );
    bool SetChmbWatchdogPeriod();
    bool SetChmbWatchdogOnOff(bool);
    bool SetChmbWatchdog();
    bool SetChmbAmbSv();
    bool SetChmbAlarmReset();

private:
	void AppendSystemLog(LOG_TYPE, QString);
	void AppendPlcLog   (LOG_TYPE, QString);

signals:
	void sigCompressureLog(LOG_TYPE, QString);
	void sigAlarmLog(LOG_TYPE, QString);
	void sigPcwLog(LOG_TYPE, QString);
private:
	Ui::CChamberView *ui;
	CLocalSocket* m_pIpcChmb;
	QLabel* m_pLblChmbRun [EN_CHMB_RUN_MAX];
	QLabel* m_pLblChmbStat[MAX_WORD_STATUS1];
	QLabel* m_pLblChmbStat2[MAX_WORD_STATUS2];
	QTimer* m_timerChmbConn;
	QTimer* m_timerChmbStatus;
    QTimer* m_timerWatchdog;
	QTimer* m_timerCompressure1_2;	//2024.08.19 - KWMOON
    CDlgSv* m_pSv;
	QMutex m_mutex;
	QMenu* m_pMenuModeOff;
	QMenu* m_pMenuLampOn;
	CPassDlg* m_pPassDlg;
private:
	QSharedMemory* m_pShmMainUi;
	stChmbStat m_stChmbStat;
	QString m_sTempDatFile;
	QString m_sDataDir;
private:
	int m_nToggleIpc;
	int m_nTogglePlc;
	bool m_bAutoScrollSys;
	bool m_bAutoScrollChmb;
	bool m_bConnIpcPlc[2];
private:
	QVector<stAlarmData> m_vecAlarmData[MAX_ALARM_WORD_ADDR];
	QVector<stAlarmList*> m_vecAlarmList;
	CLogger* m_pLoggerAlarm;
	CLogger* m_pLoggerCompressure;
	CLogger* m_pLoggerPcw;
private:
    double m_dAmbSv;
    double m_dTempLimitLow;
    double m_dTempLimitHigh;
    int m_nWatchdogPeriod;
	QPixmap m_pixmapOn;
	QPixmap m_pixmapOff;
	stCompressure m_stCompressure;
};

#endif // CHAMBERVIEW_HPP
