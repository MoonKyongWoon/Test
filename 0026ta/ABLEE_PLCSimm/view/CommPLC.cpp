#include "CommPLC.hpp"
#include "ui_CommPLC.h"

CCommPLC::CCommPLC(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CCommPLC)
{
    InitVariables();
    InitUi();
}

CCommPLC::~CCommPLC()
{
    delete ui;
}

void CCommPLC::InitVariables()
{
	m_stStatusD100.nMaskOffMode = 1;
	m_stStatusD100.nMaskOnMode  = 0;
	m_stStatusD100.nWatchdogDisableMode = 0;
	m_stStatusD100.nWatchdogEnableMode  = 1;
	m_stStatusD100.nDoorOpen = 1;
	m_stStatusD100.nDoorClose = 0;
	m_stStatusD100.nUpperRackDetect = 1;
	m_stStatusD100.nBottomRackDetect = 1;
	m_stStatusD100.nRackInComplete = 0;
	m_stStatusD100.nRackOutComplete = 1;
	m_stStatusD100.nEqReadyStatus = 0;
	m_stStatusD100.nEqAbortStatus = 0;
	m_stStatusD100.nAgvComplete   = 0;

	m_stStatusD101.nTempRun = 0;
	m_stStatusD101.nTempStop = 1;
	m_stStatusD101.nAmbient = 0;
	m_stStatusD101.nSystemDcPowerOn = 1;
	m_stStatusD101.nTowerLampLocal = 1;
	m_stStatusD101.nTowerLampRemote = 0;
	m_stStatusD101.nFingerUpRackIn = 0;
	m_stStatusD101.nDoorIgnoreRackInOut = 0;

	m_pTmrRack = new QTimer( this );
	m_pTmrDoor = new QTimer( this );
	connect(m_pTmrRack, &QTimer::timeout, this, &CCommPLC::SlotTimeoutRack );
	connect(m_pTmrDoor, &QTimer::timeout, this, &CCommPLC::SlotTimeoutDoor );

	m_bMaskOnOff = true;
	m_bUpperRackDetect = false;
	m_bBottomRackDetect = false;
	m_bAgvJobComplete = false;
	m_bEqReady = false;
	m_bEqAbort = false;
}

void CCommPLC::InitUi()
{
    ui->setupUi(this);
	InitStatusTitle();
	SetTempStopTrigger();
	SetTowerLampLocal();
    InitMask();
    InitWatchdog();

	ui->lblDcPowerOn->setStyleSheet(COMM_PLC_ON_STYLE);
	m_stStatusD100.nRackInComplete   = 0x0;
	m_stStatusD100.nRackOutComplete  = 0x1;
	m_stStatusD100.nBottomRackDetect = 0x0;
	m_stStatusD100.nUpperRackDetect  = 0x0;
	m_nRackNop = 0;
	ui->lblRackNotOperate->setText("RACK LOCAL");
	ui->lblRackNotOperate->setStyleSheet(COMM_PLC_OFF_STYLE);

	ui->lblRackInComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblRackOutComplete->setStyleSheet(COMM_PLC_ON_STYLE);
	ui->lblUpperRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblBottomRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);

	m_stStatusD100.nDoorOpen  = 0x1;
	m_stStatusD100.nDoorClose = 0x0;
	ui->lblDoorOpen->setStyleSheet(COMM_PLC_ON_STYLE);
	ui->lblDoorClose->setStyleSheet(COMM_PLC_OFF_STYLE);

	ui->lblAgvComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblEqReadyStat->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblEqAbortStat->setStyleSheet(COMM_PLC_OFF_STYLE);

	connect( ui->btnTempRun    , &QPushButton::clicked, this, &CCommPLC::SlotBtnRunClicked          );
	connect( ui->btnTempStop   , &QPushButton::clicked, this, &CCommPLC::SlotBtnStopClicked         );
	connect( ui->btnAmb        , &QPushButton::clicked, this, &CCommPLC::SlotBtnAmbClicked          );
	connect( ui->btnAmbAuto    , &QPushButton::clicked, this, &CCommPLC::SlotBtnAmbClicked          );
	connect( ui->btnAlarmReset , &QPushButton::clicked, this, &CCommPLC::SlotBtnAlarmResetClicked   );
	connect( ui->btnMaskOn     , &QPushButton::clicked, this, &CCommPLC::SlotBtnMaskOnClicked       );
	connect( ui->btnMaskOff    , &QPushButton::clicked, this, &CCommPLC::SlotBtnMaskOffClicked      );
	connect( ui->btnDoorOpen   , &QPushButton::clicked, this, &CCommPLC::SlotBtnDoorOpenClicked     );
	connect( ui->btnDoorClose  , &QPushButton::clicked, this, &CCommPLC::SlotBtnDoorCloseClicked    );
	connect( ui->btnRackIn     , &QPushButton::clicked, this, &CCommPLC::SlotBtnRackInClicked       );
	connect( ui->btnRackOut    , &QPushButton::clicked, this, &CCommPLC::SlotBtnRackOutClicked      );
	connect( ui->btnRackNotOpr , &QPushButton::clicked, this, &CCommPLC::SlotBtnRackNopClicked      );
	connect( ui->btnWatchdogEn , &QPushButton::clicked, this, &CCommPLC::SlotBtnWatchdogEnClicked   );
	connect( ui->btnWatchdogDis, &QPushButton::clicked, this, &CCommPLC::SlotBtnWatchdogDisClicked  );

	connect(ui->btnUpperRackDetect, &QPushButton::clicked, this, &CCommPLC::SlotBtnUpperRackDetectClicked);
	connect(ui->btnBottomRackDetect, &QPushButton::clicked, this, &CCommPLC::SlotBtnBottomRackDetectClicked);
	connect(ui->btnAgvJobComplete, &QPushButton::clicked, this, &CCommPLC::SlotBtnAgvJobCompleteClicked);
	connect(ui->btnEqReady, &QPushButton::clicked, this, &CCommPLC::SlotBtnEqReadyClicked);
	connect(ui->btnEqAbort, &QPushButton::clicked, this, &CCommPLC::SlotBtnEqAbortClicked);
}

void CCommPLC::InitStatusTitle()
{
	ui->titleControl->setText( "CONTROL CHAMBER" );
	ui->titleControl->setStyleSheet( COMM_TITLE_STYLE );

	ui->titleStatus->setText( "STATUS CHAMBER" );
	ui->titleStatus->setStyleSheet( COMM_TITLE_STYLE );
}

void CCommPLC::InitMask()
{
	if( m_stStatusD100.nMaskOffMode == 1 ) ui->lblMaskOff->setStyleSheet(COMM_PLC_ON_STYLE  );
	else                                   ui->lblMaskOff->setStyleSheet(COMM_PLC_OFF_STYLE );

	if( m_stStatusD100.nMaskOnMode == 1  ) ui->lblMaskOn->setStyleSheet(COMM_PLC_ON_STYLE  );
	else                                   ui->lblMaskOn->setStyleSheet(COMM_PLC_OFF_STYLE );
}

void CCommPLC::InitWatchdog()
{
	if( m_stStatusD100.nWatchdogEnableMode == 1  ) ui->lblWatchdogEn->setStyleSheet(COMM_PLC_ON_STYLE);
	else                                           ui->lblWatchdogEn->setStyleSheet(COMM_PLC_OFF_STYLE );

	if( m_stStatusD100.nWatchdogDisableMode == 1  ) ui->lblWatchdogDis->setStyleSheet(COMM_PLC_ON_STYLE);
	else                                            ui->lblWatchdogDis->setStyleSheet(COMM_PLC_OFF_STYLE );
}

void CCommPLC::SetAlarmReset()
{
	emit SigAlarmReset();
    emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] ALARM RESET"), true );
}

void CCommPLC::SetMaskOnOffStat(bool bOnOff)
{
	if( bOnOff )	//LOCAL
    {
		m_stStatusD100.nMaskOnMode  = 0x1;
		m_stStatusD100.nMaskOffMode = 0x0;
    }
    else
    {
		m_stStatusD100.nMaskOnMode  = 0x0;
		m_stStatusD100.nMaskOffMode = 0x1;
    }
    InitMask();
    emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] MASK %1").arg(bOnOff?"ON":"OFF"), true );
}

void CCommPLC::SetWatchdogPeriod( int nPeriod )
{
	ui->spinWatchdog->setValue( nPeriod );
    emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] WATCHDOG PERIOD %1 sec").arg(nPeriod), true );
}

void CCommPLC::SetSv(int set)
{
	short sSv = static_cast<short>(set&0xFFFF);
	double dSv = static_cast<double>( sSv / 10.0 );
	ui->spinSV->setValue( dSv );
}

void CCommPLC::SetAmbSv(int set)
{
	short sSv = static_cast<short>(set&0xFFFF);
	double dSv = static_cast<double>( sSv / 10.0 );
	ui->spinSV->setValue( dSv );
}

void CCommPLC::SetWatchdogTrigger()
{
    emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] WATCHDOG TOGGLE TRIG"), true );
}

void CCommPLC::SetDoorOpen()
{
	if( m_pTmrDoor->isActive() ) return;
	m_bDoorOpen = true;
	m_stStatusD100.nDoorOpen  = 0x0;
	m_stStatusD100.nDoorClose = 0x0;
	ui->lblDoorOpen->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblDoorClose->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Door open timer 10 s"), true );
	m_pTmrDoor->start( 10000 );
}

void CCommPLC::SetDoorClose()
{
	if( m_pTmrDoor->isActive() ) return;
	m_bDoorOpen = false;
	m_stStatusD100.nDoorOpen  = 0x0;
	m_stStatusD100.nDoorClose = 0x0;
	ui->lblDoorOpen->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblDoorClose->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Door close timer 10 s"), true );
	m_pTmrDoor->start( 10000 );
}

void CCommPLC::SlotTimeoutDoor()
{
	m_pTmrDoor->stop();
	if( m_bDoorOpen )
	{
		m_stStatusD100.nDoorOpen  = 0x1;
		m_stStatusD100.nDoorClose = 0x0;
		ui->lblDoorOpen->setStyleSheet(COMM_PLC_ON_STYLE);
		ui->lblDoorClose->setStyleSheet(COMM_PLC_OFF_STYLE);
		emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Door open done"), true );
	}
	else
	{
		m_stStatusD100.nDoorOpen  = 0x0;
		m_stStatusD100.nDoorClose = 0x1;
		ui->lblDoorOpen->setStyleSheet(COMM_PLC_OFF_STYLE);
		ui->lblDoorClose->setStyleSheet(COMM_PLC_ON_STYLE);
		emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Door close done"), true );
	}
}


void CCommPLC::SetTempRunTrigger()
{
	m_stStatusD101.nTempRun  = 0x1;
	m_stStatusD101.nTempStop = 0x0;
	m_stStatusD101.nAmbient  = 0x0;
	ui->lblRun->setStyleSheet(COMM_PLC_ON_STYLE);
	ui->lblStop->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblAmb->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Temp Run"), true );
}

void CCommPLC::SetTempStopTrigger()
{
	m_stStatusD101.nTempRun  = 0x0;
	m_stStatusD101.nTempStop = 0x1;
	m_stStatusD101.nAmbient  = 0x0;
	ui->lblRun->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblStop->setStyleSheet(COMM_PLC_ON_STYLE);
	ui->lblAmb->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Temp Stop"), true );
}

void CCommPLC::SetAmbTrigger()
{
	m_stStatusD101.nTempRun  = 0x0;
	m_stStatusD101.nTempStop = 0x0;
	m_stStatusD101.nAmbient  = 0x1;
	ui->lblRun->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblStop->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblAmb->setStyleSheet(COMM_PLC_ON_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Temp Amb"), true );
}

void CCommPLC::SetAmbAutoTrigger()
{
	SetAmbTrigger();
}

void CCommPLC::SetRackInTrigger()
{
	if( m_pTmrRack->isActive() ) return;
	m_bRackIn = true;
	m_stStatusD100.nRackInComplete  = 0x0;
	m_stStatusD100.nRackOutComplete = 0x0;
	ui->lblRackInComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblRackOutComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RackIn timer 10 s"), true );
	m_pTmrRack->start( 10000 );
}

void CCommPLC::SetRackOutTrigger()
{
	if( m_pTmrRack->isActive() ) return;
	m_bRackIn = false;
	m_stStatusD100.nRackInComplete  = 0x0;
	m_stStatusD100.nRackOutComplete = 0x0;
	ui->lblRackInComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblRackOutComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RackOut timer 10 s"), true );
	m_pTmrRack->start( 10000 );

	//m_stStatusD100.nRackInComplete   = 0x0;
	//m_stStatusD100.nRackOutComplete  = 0x1;
	//m_stStatusD100.nBottomRackDetect = 0x0;
	//m_stStatusD100.nUpperRackDetect  = 0x0;
	//ui->lblRackInComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	//ui->lblRackOutComplete->setStyleSheet(COMM_PLC_ON_STYLE);
	//ui->lblUpperRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);
	//ui->lblBottomRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);
	//emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RackOut"), true );
}

void CCommPLC::SlotTimeoutRack()
{
	m_pTmrRack->stop();
	if( m_bRackIn )
	{
		m_stStatusD100.nRackInComplete   = 0x1;
		m_stStatusD100.nRackOutComplete  = 0x0;
		ui->lblRackInComplete->setStyleSheet(COMM_PLC_ON_STYLE);
		ui->lblRackOutComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
		emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RackIn Done"), true );
	}
	else
	{
		m_stStatusD100.nRackInComplete   = 0x0;
		m_stStatusD100.nRackOutComplete  = 0x1;
		ui->lblRackInComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
		ui->lblRackOutComplete->setStyleSheet(COMM_PLC_ON_STYLE);
		emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RackOut Done"), true );
	}
}

void CCommPLC::SetRackNopTrigger(bool remote)
{
	if( remote )
	{
		m_nRackNop = 1;
		ui->lblRackNotOperate->setStyleSheet(COMM_PLC_ON_STYLE);
		ui->lblRackNotOperate->setText("RACK REMOTE");
	}
	else
	{
		m_nRackNop = 0;
		ui->lblRackNotOperate->setStyleSheet(COMM_PLC_OFF_STYLE);
		ui->lblRackNotOperate->setText("RACK LOCAL");
	}
	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] RACK OPR %1").arg(remote?"REMOTE":"LOCAL"), true );
}

void CCommPLC::SetTowerLampRemote()
{
	m_stStatusD101.nTowerLampRemote = true;
	m_stStatusD101.nTowerLampLocal  = false;
	ui->lblTowerLampRemote->setStyleSheet(COMM_PLC_ON_STYLE);
	ui->lblTowerLampLocal->setStyleSheet(COMM_PLC_OFF_STYLE);
}

void CCommPLC::SetTowerLampLocal()
{
	m_stStatusD101.nTowerLampRemote = false;
	m_stStatusD101.nTowerLampLocal  = true;
	ui->lblTowerLampRemote->setStyleSheet(COMM_PLC_OFF_STYLE);
	ui->lblTowerLampLocal->setStyleSheet(COMM_PLC_ON_STYLE);
}

void CCommPLC::SetWatchdogEnDisable( bool bEnable )
{
    if( bEnable )
    {
		m_stStatusD100.nWatchdogEnableMode  = 0x1;
		m_stStatusD100.nWatchdogDisableMode = 0x0;
    }
    else
    {
		m_stStatusD100.nWatchdogEnableMode  = 0x0;
		m_stStatusD100.nWatchdogDisableMode = 0x1;
    }
    InitWatchdog();
    emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] WATCHDOG %1").arg(bEnable?"ENABLE":"DISABLE"), true );
}

void CCommPLC::SetUpperRackDetectToggle()
{
	m_bUpperRackDetect = !m_bUpperRackDetect;

	if (m_bUpperRackDetect)
	{
		m_stStatusD100.nUpperRackDetect  = 0x1;
		ui->lblUpperRackDetect->setStyleSheet(COMM_PLC_ON_STYLE);
	}
	else
	{
		m_stStatusD100.nUpperRackDetect  = 0x0;
		ui->lblUpperRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);
	}

	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Upper Rack %1").arg(m_bUpperRackDetect?"Detected":"Undetected"), true );
}

void CCommPLC::SetBottomRackDetectToggle()
{
	m_bBottomRackDetect = !m_bBottomRackDetect;

	if (m_bBottomRackDetect)
	{
		m_stStatusD100.nBottomRackDetect  = 0x1;
		ui->lblBottomRackDetect->setStyleSheet(COMM_PLC_ON_STYLE);
	}
	else
	{
		m_stStatusD100.nBottomRackDetect  = 0x0;
		ui->lblBottomRackDetect->setStyleSheet(COMM_PLC_OFF_STYLE);
	}

	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Bottom Rack %1").arg(m_bBottomRackDetect?"Detected":"Undetected"), true );
}

void CCommPLC::SetAgvJobCompleteToggle()
{
	m_bAgvJobComplete = !m_bAgvJobComplete;

	if (m_bAgvJobComplete)
	{
		m_stStatusD100.nAgvComplete  = 0x1;
		ui->lblAgvComplete->setStyleSheet(COMM_PLC_ON_STYLE);
	}
	else
	{
		m_stStatusD100.nAgvComplete  = 0x0;
		ui->lblAgvComplete->setStyleSheet(COMM_PLC_OFF_STYLE);
	}

	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] AGV Job %1").arg(m_bAgvJobComplete?"Complete":"Incomplete"), true );
}

void CCommPLC::SetEqReadyToggle()
{
	m_bEqReady = !m_bEqReady;

	if (m_bEqReady)
	{
		m_stStatusD100.nEqReadyStatus  = 0x1;
		ui->lblEqReadyStat->setStyleSheet(COMM_PLC_ON_STYLE);
	}
	else
	{
		m_stStatusD100.nEqReadyStatus  = 0x0;
		ui->lblEqReadyStat->setStyleSheet(COMM_PLC_OFF_STYLE);
	}

	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Equipment %1").arg(m_bEqReady?"Ready":"Not Ready"), true );
}

void CCommPLC::SetEqAbortToggle()
{
	m_bEqAbort = !m_bEqAbort;

	if (m_bEqAbort)
	{
		m_stStatusD100.nEqAbortStatus  = 0x1;
		ui->lblEqAbortStat->setStyleSheet(COMM_PLC_ON_STYLE);
	}
	else
	{
		m_stStatusD100.nEqAbortStatus  = 0x0;
		ui->lblEqAbortStat->setStyleSheet(COMM_PLC_OFF_STYLE);
	}

	emit SigPlcLogMsg( tr("[COMMON][ CMD][INFO] Equipment %1").arg(m_bEqAbort?"Abort":"Keep"), true );
}

bool CCommPLC::DoCommonCmd( EN_PLC_ADDRESS_MATCH_CMD cmd, YOKOGAWA_PLC_CMD_TYPE type, int set, int& get)
{
    int bit = 0;
    switch( cmd )
    {
    case EN_GET_COMMON_MASK_OFF       :
		get = m_stStatusD100.nMaskOffMode & 0x1;
        break;
    case EN_GET_COMMON_MASK_ON        :
		get = m_stStatusD100.nMaskOnMode  & 0x1;
        break;
    case EN_GET_COMMON_WATCHDOG_EN    :
		get = m_stStatusD100.nWatchdogEnableMode & 0x1;
        break;
    case EN_GET_COMMON_WATCHDOG_DIS   :
		get = m_stStatusD100.nWatchdogDisableMode & 0x1;
        break;
	case EN_GET_COMMON_DOOR_OPEN   :
		get = m_stStatusD100.nDoorOpen & 0x1;
		break;
	case EN_GET_COMMON_DOOR_CLOSE   :
		get = m_stStatusD100.nDoorClose & 0x1;
		break;
	case EN_GET_COMMON_UPPER_RACK_DETECT   :
		get = m_stStatusD100.nUpperRackDetect & 0x1;
		break;
	case EN_GET_COMMON_BOTTOM_RACK_DETECT   :
		get = m_stStatusD100.nBottomRackDetect & 0x1;
		break;
	case EN_GET_COMMON_RACKIN_COMPLETE   :
		get = m_stStatusD100.nRackInComplete & 0x1;
		break;
	case EN_GET_COMMON_RACKOUT_COMPLETE   :
		get = m_stStatusD100.nRackOutComplete & 0x1;
		break;
	case EN_GET_COMMON_EQ_READY_STATUS   :
		get = m_stStatusD100.nEqReadyStatus & 0x1;
		break;
	case EN_GET_COMMON_EQ_ABORT_STATUS   :
		get = m_stStatusD100.nEqAbortStatus & 0x1;
		break;
	case EN_GET_COMMON_AGV_COMPLETE   :
		get = m_stStatusD100.nAgvComplete & 0x1;
		break;
	case EN_GET_COMMON_STATUS_D100         :
		get  = (m_stStatusD100.nMaskOffMode         & 0x1) << 0;
		get |= (m_stStatusD100.nMaskOnMode          & 0x1) << 1;
		get |= (m_stStatusD100.nWatchdogEnableMode  & 0x1) << 2;
		get |= (m_stStatusD100.nWatchdogDisableMode & 0x1) << 3;
		get |= (m_stStatusD100.nDoorOpen            & 0x1) << 4;
		get |= (m_stStatusD100.nDoorClose           & 0x1) << 5;
		get |= (m_stStatusD100.nUpperRackDetect     & 0x1) << 6;
		get |= (m_stStatusD100.nBottomRackDetect    & 0x1) << 7;
		get |= (m_stStatusD100.nRackInComplete      & 0x1) << 8;
		get |= (m_stStatusD100.nRackOutComplete     & 0x1) << 9;
		get |= (m_stStatusD100.nEqReadyStatus       & 0x1) << 10;
		get |= (m_stStatusD100.nEqAbortStatus       & 0x1) << 11;
		get |= (m_stStatusD100.nAgvComplete         & 0x1) << 12;
		break;
	case EN_GET_COMMON_STATUS_D101         :
		get   = (m_stStatusD101.nTempRun             & 0x1 ) << 0;
		get  |= (m_stStatusD101.nTempStop            & 0x1 ) << 1;
		get  |= (m_stStatusD101.nAmbient             & 0x1 ) << 2;
		get  |= (m_stStatusD101.nSystemDcPowerOn     & 0x1 ) << 3;
		get  |= (m_stStatusD101.nTowerLampLocal      & 0x1 ) << 4;
		get  |= (m_stStatusD101.nTowerLampRemote     & 0x1 ) << 5;
		get  |= (m_stStatusD101.nFingerUpRackIn      & 0x1 ) << 6;
		get  |= (m_stStatusD101.nDoorIgnoreRackInOut & 0x1 ) << 7;
		break;
	case EN_SET_TEMP_RUN_TRIGGER:
		SetTempRunTrigger();
		break;
	case EN_SET_TEMP_STOP_TRIGGER:
		SetTempStopTrigger();
		break;
	case EN_SET_AMB_TRIGGER:
	case EN_SET_AMB_AUTO_TRIGGER:
		SetAmbTrigger();
		break;
	case EN_SET_ALARM_RESET    :
        SetAlarmReset();
        break;
	case EN_SET_MASK_ON_OFF        :
		SetMaskOnOffStat( set );
        break;
	case EN_SET_DOOR_OPEN_TRIGGER        :
		SetDoorOpen();
		break;
	case EN_SET_DOOR_CLOSE_TRIGGER        :
		SetDoorClose();
		break;
	case EN_SET_RACK_IN_TRIGGER:
		SetRackInTrigger();
		break;
	case EN_SET_RACK_OUT_TRIGGER:
		SetRackOutTrigger();
		break;
	case EN_SET_RACK_NOP_TRIGGER:
		SetRackNopTrigger(set);
		break;
	case EN_GET_RACK_NOP_TRIGGER:
		get = m_nRackNop;
		break;
	case EN_SET_WATCHDOG_PERIOD:
        SetWatchdogPeriod( set );
        break;
	case EN_SET_WATCHDOG_TOGGLE:
        SetWatchdogTrigger();
        break;
	case EN_SET_WATCHDOG_EN_DIS:
        SetWatchdogEnDisable( set );
        break;
	case EN_SET_SV:
		SetSv( set );
		break;
	case EN_SET_AMB_SV:
		SetAmbSv( set );
		break;
	case EN_GET_COMMON_PLC_VERSION:
		get = ui->spinVer->value()*10;
        break;
	case EN_GET_COMMON_PV:
		get = ui->spinPV->value()*10;
		break;
	case EN_GET_COMMON_SV:
		get = ui->spinSV->value()*10;
		break;
	case EN_GET_COMMON_PCW_TEMPERATURE:
		get = ui->spinPcwTemp->value()*100;
		break;
	case EN_GET_WPS_PRESSURE:
		get = ui->spinWpsPress->value()*100;
		break;
	case EN_GET_PCW_FLOW_READ:
		get = ui->spinWpsPress->value()*10;
		break;
	//2024.08.20 - KWMOON
	case EN_GET_COMP1_PRESSURE_HIGH:
		get = static_cast<int>(ui->spinComp1PressureHigh->value()*10);
		break;
	case EN_GET_COMP1_PRESSURE_LOW:
		get = static_cast<int>(ui->spinComp1PressureLow->value()*10);
		break;
	case EN_GET_COMP2_PRESSURE_HIGH:
		get = static_cast<int>(ui->spinComp2PressureHigh->value()*10);
		break;
	case EN_GET_COMP2_PRESSURE_LOW:
		get = static_cast<int>(ui->spinComp2PressureLow->value()*10);
		break;
	case EN_GET_COMP1_OUT_TEMP:
		get = static_cast<int>(ui->spinComp1OutTemp->value()*10);
		break;
	case EN_GET_COMP1_IN_TEMP:
		get = static_cast<int>(ui->spinComp1InTemp->value()*10);
		break;
	case EN_GET_COMP2_OUT_TEMP:
		get = static_cast<int>(ui->spinComp2OutTemp->value()*10);
		break;
	case EN_GET_COMP2_IN_TEMP:
		get = static_cast<int>(ui->spinComp2InTemp->value()*10);
		break;
	default:
		emit SigPlcLogMsg( tr("[COMMON][ CMD][ ERR] CMD:%1").arg(cmd), false );
        break;
    }
    return true;
}

void CCommPLC::SlotBtnRunClicked()
{
	SetTempRunTrigger();
}

void CCommPLC::SlotBtnStopClicked()
{
	SetTempStopTrigger();
}

void CCommPLC::SlotBtnAmbClicked()
{
	SetAmbTrigger();
}

void CCommPLC::SlotBtnAlarmResetClicked()
{
	SetAlarmReset();
}

void CCommPLC::SlotBtnMaskOnClicked()
{
	m_bMaskOnOff = true;
	SetMaskOnOffStat( m_bMaskOnOff );
}

void CCommPLC::SlotBtnMaskOffClicked()
{
	m_bMaskOnOff = false;
	SetMaskOnOffStat( m_bMaskOnOff );
}

void CCommPLC::SlotBtnDoorOpenClicked()
{
	SetDoorOpen();
}

void CCommPLC::SlotBtnDoorCloseClicked()
{
	SetDoorClose();
}

void CCommPLC::SlotBtnRackInClicked()
{
	SetRackInTrigger();
}

void CCommPLC::SlotBtnRackOutClicked()
{
	SetRackOutTrigger();
}

void CCommPLC::SlotBtnRackNopClicked()
{
	static int nToggle = 0x1;
	SetRackNopTrigger( static_cast<bool>(nToggle) );
	nToggle = (~nToggle)&0x1;
}

void CCommPLC::SlotBtnWatchdogEnClicked()
{
	SetWatchdogEnDisable( 1 );
}

void CCommPLC::SlotBtnWatchdogDisClicked()
{
	SetWatchdogEnDisable( 0 );
}

void CCommPLC::SlotBtnUpperRackDetectClicked()
{
	SetUpperRackDetectToggle();
}

void CCommPLC::SlotBtnBottomRackDetectClicked()
{
	SetBottomRackDetectToggle();
}

void CCommPLC::SlotBtnAgvJobCompleteClicked()
{
	SetAgvJobCompleteToggle();
}

void CCommPLC::SlotBtnEqReadyClicked()
{
	SetEqReadyToggle();
}

void CCommPLC::SlotBtnEqAbortClicked()
{
	SetEqAbortToggle();
}
