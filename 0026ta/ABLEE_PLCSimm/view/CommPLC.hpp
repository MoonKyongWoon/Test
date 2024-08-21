#ifndef COMMPLC_HPP
#define COMMPLC_HPP

#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include "Common.hpp"

namespace Ui {
class CCommPLC;
}

struct _stStatusD100
{
    int nMaskOffMode;
    int nMaskOnMode;
    int nWatchdogEnableMode;
    int nWatchdogDisableMode;
	int nDoorOpen;
	int nDoorClose;
	int nUpperRackDetect;
	int nBottomRackDetect;
	int nRackInComplete;
	int nRackOutComplete;
	int nEqReadyStatus;
	int nEqAbortStatus;
	int nAgvComplete;
};

struct _stStatusD101
{
	int nTempRun;
	int nTempStop;
	int nAmbient;
	int nSystemDcPowerOn;
	int nTowerLampLocal;
	int nTowerLampRemote;
	int nFingerUpRackIn;
	int nDoorIgnoreRackInOut;
};

class CCommPLC : public QWidget
{
    Q_OBJECT

public:
    explicit CCommPLC(QWidget *parent = nullptr);
    ~CCommPLC();
private:
    void InitVariables();
    void InitUi();
	void InitStatusTitle();
    void InitMask();
    void InitWatchdog();
    void SetAlarmReset();
	void SetMaskOnOffStat(bool);
    void SetWatchdogPeriod( int );
    void SetWatchdogTrigger();
    void SetWatchdogEnDisable(bool);
	void SetSv(int);
	void SetAmbSv(int);
	void SetDoorOpen();
	void SetDoorClose();
	void SetTempRunTrigger();
	void SetTempStopTrigger();
	void SetAmbTrigger();
	void SetAmbAutoTrigger();
	void SetRackInTrigger();
	void SetRackOutTrigger();
	void SetRackNopTrigger(bool);
	void SetTowerLampRemote();
	void SetTowerLampLocal();
	void SetUpperRackDetectToggle();
	void SetBottomRackDetectToggle();
	void SetAgvJobCompleteToggle();
	void SetEqReadyToggle();
	void SetEqAbortToggle();

public:
    bool DoCommonCmd( EN_PLC_ADDRESS_MATCH_CMD, YOKOGAWA_PLC_CMD_TYPE, int, int& );
public slots:
	void SlotTimeoutRack();
	void SlotTimeoutDoor();
public slots:
	void SlotBtnRunClicked();
	void SlotBtnStopClicked();
	void SlotBtnAmbClicked();
	void SlotBtnAlarmResetClicked();
	void SlotBtnMaskOnClicked();
	void SlotBtnMaskOffClicked();
	void SlotBtnDoorOpenClicked();
	void SlotBtnDoorCloseClicked();
	void SlotBtnRackInClicked();
	void SlotBtnRackOutClicked();
	void SlotBtnRackNopClicked();
	void SlotBtnWatchdogEnClicked();
	void SlotBtnWatchdogDisClicked();
	void SlotBtnUpperRackDetectClicked();
	void SlotBtnBottomRackDetectClicked();
	void SlotBtnAgvJobCompleteClicked();
	void SlotBtnEqReadyClicked();
	void SlotBtnEqAbortClicked();
signals:
    void SigPlcLogMsg( QString, bool );
	void SigAlarmReset();
private:
	EN_ALARM_LEVEL m_AlarmLevel[EN_COMMON_ALARM_COUNT];
	_stStatusD100 m_stStatusD100;
	_stStatusD101 m_stStatusD101;
	int m_nRackNop;
private:
	Ui::CCommPLC *ui;
	QTimer* m_pTmrRack;
	QTimer* m_pTmrDoor;
	bool m_bRackIn;
	bool m_bDoorOpen;
	bool m_bMaskOnOff;
	bool m_bUpperRackDetect;
	bool m_bBottomRackDetect;
	bool m_bAgvJobComplete;
	bool m_bEqReady;
	bool m_bEqAbort;
};

#endif // COMMPLC_HPP
