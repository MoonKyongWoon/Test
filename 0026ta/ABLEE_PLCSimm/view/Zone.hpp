#ifndef ZONE_HPP
#define ZONE_HPP

#include <QWidget>
#include <QPushButton>
#include "Common.hpp"

#define TITLE_CH_STYLE              "background-color:#6886c5; font: 13pt;font-weight:bold;"
#define STATUS_NORM_STYLE           "background-color:rgb(236,236,236); "
#define STATUS_RUN_STYLE            "background-color:rgb(0,255,0); "
#define STATUS_STOP_STYLE           "background-color:rgb(255,0,0); "
#define STATUS_AMBIENT_STYLE        "background-color:rgb(0,128,128); "
#define STATUS_DV_STYLE             "background-color:rgb(0,100,255); "
#define ALARM_ALARM_STYLE           "background-color: #DC0000; font-size: 8pt;"
#define ALARM_WARNING_STYLE         "background-color: #F1C40F; font-size: 8pt;"
#define ALARM_NORMAL_STYLE          "background-color: #00DC00; font-size: 8pt;"
#define MASK_REMOTE_ON              "background-color:#008080; font: 9pt; font-weight: bold;"
#define MASK_REMOTE_OFF             "background-color:#D4D4D4; font: 9pt;"

namespace Ui {
class CZone;
}

struct _stZoneStatus
{
    EN_CHAMBER_STATUS nTempRunMode;
    int nDoorCloseComplete;
    int nSlot1PowerOnMode;
    int nSlot2PowerOnMode;
    int nSystemPowerOnMode;
    int nMaskOffMode;
    int nMaskOnMode;
    double dSv;
    double dAmbSv;
    double dDv;
    bool   bDvMode;
};

class CZone : public QWidget
{
    Q_OBJECT

public:
    explicit CZone(int,QWidget *parent = nullptr);
    ~CZone();

private:
    Ui::CZone *ui;
    int m_nChannelNo;
    QPushButton* m_pAlarmBtns[EN_CHANANEL_ALARM_COUNT];
    EN_ALARM_LEVEL m_AlarmLevel[EN_CHANANEL_ALARM_COUNT];
    _stZoneStatus m_stZoneStatus;
private:
    void InitVariables();
    void InitUi();
    void InitChannelTitle();
    void InitChamberStatus();
    void InitDoorAndMask();
    void InitSlotPower();
    void InitSystemPower();
    void InitSvPv();
    void InitAlarmButtons();
    void ClearAlarm();
    void LogMessage( QString, bool );
    void InitTesterAlarm(bool);
public:
    void SetChamberStatus(EN_CHAMBER_STATUS);
    EN_CHAMBER_STATUS GetStatus();
    void SetMaskRemote(bool);
    bool GetMaskRemote();
    void SetSv(double);
    void SetDv(double);
    void SetAmbientSv(double);
    void GetTemp(double& sv,double& pv, double& dv);
    bool DoChannelCmd( EN_PLC_ADDRESS_MATCH_CMD, YOKOGAWA_PLC_CMD_TYPE, int, int& );
signals:
    void SigPlcLogMsg( QString, bool );
public slots:
    void SlotAlarmClicked( bool );
    void SlotSysPwrClicked(bool );
};

#endif // ZONE_HPP
