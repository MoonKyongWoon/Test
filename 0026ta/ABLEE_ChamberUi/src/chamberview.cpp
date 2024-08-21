#include "chamberview.hpp"
#include "ui_chamberview.h"

CChamberView::CChamberView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CChamberView)
{
	m_pShmMainUi = nullptr;
	memset( &m_stChmbStat, 0x0, sizeof(stChmbStat) );
	m_bConnIpcPlc[CONN_IPC] = false;
	m_bConnIpcPlc[CONN_PLC] = false;
	m_bAutoScrollSys  = true;
	m_bAutoScrollChmb = true;
	m_vecAlarmList.clear();
    LoadChmbIni();
	InitLogger();	//2024.08.19 - KWMOON
    InitUi();
	InitInstance();
	InitSharedMemory();
	InitAlarmList();
	InitTimer();
}

CChamberView::~CChamberView()
{
	SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB, WATCHDOG_DISABLE );

	KillTimer();
	for( stAlarmList* p : m_vecAlarmList )
	{
		delete p;
	}
	m_vecAlarmList.clear();

	//2023.06.20 - KWMOON
	if( m_pShmMainUi != nullptr )
	{
		if( m_pShmMainUi->isAttached() )
			m_pShmMainUi->detach();
	}
	//2024.08.19 - KWMOON(logger)
	if( m_pLoggerCompressure != nullptr )
		m_pLoggerCompressure->deleteLater();
	if( m_pLoggerAlarm != nullptr )
		m_pLoggerAlarm->deleteLater();
	if( m_pLoggerPcw != nullptr )
		m_pLoggerPcw->deleteLater();

	delete ui;
}

void CChamberView::LoadChmbIni()
{
    QString os_path = "";
    QString section_key = "";
    QString sValue = 0x0;

    char* env_os  = getenv(ATOS_DIR_ENV);
    if( env_os != NULL )
    {
        os_path = QString(env_os);
    }
    else
    {
		os_path = DIR_ATOS_PATH;
    }

    QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(CHAMBER_INI);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_AMB_SV);
    if ( !UTIL::INI::readSettings( ini, section_key, m_dAmbSv ) )
        m_dAmbSv = DEFAULT_CHMB_AMB_SV;

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_LOW);
    if ( !UTIL::INI::readSettings( ini, section_key, m_dTempLimitLow ) )
        m_dTempLimitLow = DEFAULT_CHMB_TEMP_LIMIT_LOW;

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_HIGH);
    if ( !UTIL::INI::readSettings( ini, section_key, m_dTempLimitHigh ) )
        m_dTempLimitHigh = DEFAULT_CHMB_TEMP_LIMIT_HIGH;

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_PERIOD);
    if ( !UTIL::INI::readSettings( ini, section_key, m_nWatchdogPeriod ) )
        m_nWatchdogPeriod = WATCHDOG_DEFAULT_PERIOD;
}

void CChamberView::InitUi()
{
	ui->setupUi(this);

	m_pLblChmbRun[EN_CHMB_RUN ] = ui->lblRun;
	m_pLblChmbRun[EN_CHMB_STOP] = ui->lblStop;
	m_pLblChmbRun[EN_CHMB_AMB ] = ui->lblAmbient;

	for( int i=0;i<MAX_WORD_STATUS1;i++ ) m_pLblChmbStat[i]  = nullptr;
	for( int i=0;i<MAX_WORD_STATUS2;i++ ) m_pLblChmbStat2[i] = nullptr;

	m_pLblChmbStat[CHMB_STAT1_MASK_OFF           ] = ui->statusMaskOff;
	m_pLblChmbStat[CHMB_STAT1_MASK_ON            ] = ui->statusMaskOn;
	m_pLblChmbStat[CHMB_STAT1_WATCHDOG_ENAB      ] = ui->statusWatchdogEn;
	m_pLblChmbStat[CHMB_STAT1_WATCHDOG_DISB      ] = ui->statusWatchdogDis;
	m_pLblChmbStat[CHMB_STAT1_DOOR_OPEN          ] = ui->statusDoorOpen;
	m_pLblChmbStat[CHMB_STAT1_DOOR_CLOSE         ] = ui->statusDoorClose;
	m_pLblChmbStat[CHMB_STAT1_UPPER_RACK_DETECT  ] = ui->statusUpperRackDetect;
	m_pLblChmbStat[CHMB_STAT1_BOTTOM_RACK_DETECT ] = ui->statusBottomRackDetect;
	m_pLblChmbStat[CHMB_STAT1_RACK_IN_COMPLETE   ] = ui->statusRackInComplete;
	m_pLblChmbStat[CHMB_STAT1_RACK_OUT_COMPLETE  ] = ui->statusRackOutComplete;
	m_pLblChmbStat[CHMB_STAT1_EQ_READY_STATUS    ] = ui->statusEqReadyStatus;
	m_pLblChmbStat[CHMB_STAT1_EQ_ABORT_STATUS    ] = ui->statusEqAbortStatus;
	m_pLblChmbStat[CHMB_STAT1_AVG_JOB_COMPLETE   ] = ui->statusAvgJob;

	m_pLblChmbStat2[CHMB_STAT2_DC_PWR  ] = ui->statusSysDcPowerOn;
	m_pLblChmbStat2[CHMB_STAT2_TOWERLAMP_LOCAL  ] = ui->statusTowerLampLocal;
	m_pLblChmbStat2[CHMB_STAT2_TOWERLAMP_REMOTE  ] = ui->statusTowerLampRemote;
	m_pLblChmbStat2[CHMB_STAT2_FINGER_UP_STAT   ] = ui->statusFingerUpStatRackIn;
	m_pLblChmbStat2[CHMB_STAT2_DOOR_IGNORE_RACK_STAT  ] = ui->statusDoorIgnoreRackInOut;

	m_pixmapOn.load("://icon/record.ico ");
	m_pixmapOff.load("://icon/pause.ico");

	InitLampMenu();
	InitButtons( false );
	InitChmbStatus();
	InitDisplay(0);

	InitSysLogView();
	InitPlcLogView();
	InitAlarmTree();

	connect( ui->btnRun          , &QPushButton::clicked , this, &CChamberView::SlotBtnRunClicked           );
	connect( ui->btnStop         , &QPushButton::clicked , this, &CChamberView::SlotBtnStopClicked          );
	connect( ui->btnAmbMode      , &QPushButton::clicked , this, &CChamberView::SlotBtnAmbModeClicked       );
	connect( ui->btnAmbAuto      , &QPushButton::clicked , this, &CChamberView::SlotBtnAmbAutoClicked       );
	connect( ui->btnAlarmReset   , &QPushButton::clicked , this, &CChamberView::SlotBtnAlarmResetClicked    );
	connect( ui->btnMaskOn       , &QPushButton::clicked , this, &CChamberView::SlotBtnMaskOnClicked        );
	connect( ui->btnMaskOff      , &QPushButton::clicked , this, &CChamberView::SlotBtnMaskOffClicked       );
	connect( ui->btnWatchdogEn   , &QPushButton::clicked , this, &CChamberView::SlotBtnWatchdogEnClicked    );
    connect( ui->btnWatchdogDis  , &QPushButton::clicked , this, &CChamberView::SlotBtnWatchdogDisClicked   );
	connect( ui->btnDoorClose    , &QPushButton::clicked , this, &CChamberView::SlotBtnDoorCloseClicked     );
	connect( ui->btnDoorOpen     , &QPushButton::clicked , this, &CChamberView::SlotBtnDoorOpenClicked      );
	connect( ui->btnRackIn       , &QPushButton::clicked , this, &CChamberView::SlotBtnRackInClicked        );
	connect( ui->btnRackOut      , &QPushButton::clicked , this, &CChamberView::SlotBtnRackOutClicked       );
	connect( ui->btnSetTemp      , &QPushButton::clicked , this, &CChamberView::SlotBtnSetTempClicked       );
	connect( ui->btnSetAmb       , &QPushButton::clicked , this, &CChamberView::SlotBtnSetAmbClicked        );
	connect( ui->btnRackOprPlc   , &QPushButton::clicked , this, &CChamberView::SlotBtnRackOprChamberClicked);
    connect( ui->btnRackOprLocal , &QPushButton::clicked , this, &CChamberView::SlotBtnRackOprTesterClicked );
	connect( ui->btnBuzzerOff    , &QPushButton::clicked , this, &CChamberView::SlotBtnBuzzerOffClicked     );
	connect( ui->btnBuzzerOn     , &QPushButton::clicked , this, &CChamberView::SlotBtnBuzzerOnClicked      );

	connect( ui->btnRackInFingerUpOn      , &QPushButton::clicked , this, &CChamberView::SlotbtnRackInFingerUpOnClicked       );	//2023.09.08 - KWMOON
	connect( ui->btnRackInFingerUpOff     , &QPushButton::clicked , this, &CChamberView::SlotbtnRackInFingerUpOffClicked      );	//2023.09.08 - KWMOON
	connect( ui->btnDoorIgnoreRackInOutOn , &QPushButton::clicked , this, &CChamberView::SlotbtnDoorIgnoreRackInOutOnClicked  );	//2023.09.08 - KWMOON
	connect( ui->btnDoorIgnoreRackInOutOff, &QPushButton::clicked , this, &CChamberView::SlotbtnDoorIgnoreRackInOutOffClicked );	//2023.09.08 - KWMOON
	//2023.09.19 - KWMOON
	connect( ui->btnDcPowerOn             , &QPushButton::clicked , this, &CChamberView::SlotBtnDcPowerOnClicked );	//2023.09.19 - KWMOON )
	connect( ui->btnDcPowerOff            , &QPushButton::clicked , this, &CChamberView::SlotBtnDcPowerOffClicked );	//2023.09.19 - KWMOON )
}

void CChamberView::InitLampMenu()
{
	m_pMenuModeOff = new QMenu();
	m_pMenuModeOff->addAction( QIcon("://icon/ramp_off.ico") , "LAMP ALL OFF"    , this, &CChamberView::SlotLampAllOffClicked     );
	m_pMenuModeOff->addAction( QIcon("://icon/touch.ico")    , "LAMP LOCAL MODE" , this, &CChamberView::SlotLampLocalModeClicked  );
	m_pMenuModeOff->addAction( QIcon("://icon/remote.ico")   , "LAMP REMOTE MODE", this, &CChamberView::SlotLampRemoteModeClicked );
	ui->tbLampModeOff->setMenu( m_pMenuModeOff );

	m_pMenuLampOn = new QMenu();
    m_pMenuLampOn->addAction( QIcon("://icon/red_marble.ico")      , "RED ON"         , this, &CChamberView::SlotRedLampOnClicked      );
    m_pMenuLampOn->addAction( QIcon("://icon/yellow_rectangle.ico"), "YELLOW ON"      , this, &CChamberView::SlotYellowLampOnClicked   );
    m_pMenuLampOn->addAction( QIcon("://icon/green_rectangle.ico") , "GREEN ON"       , this, &CChamberView::SlotGreenLampOnClicked    );
    m_pMenuLampOn->addAction( QIcon("://icon/red_marble.ico")      , "RED BLINK ON"   , this, &CChamberView::SlotRedLampBlinkClicked   );
    m_pMenuLampOn->addAction( QIcon("://icon/yellow_rectangle.ico"), "YELLOW BLINK ON", this, &CChamberView::SlotYellowLampBlinkClicked);
    m_pMenuLampOn->addAction( QIcon("://icon/green_rectangle.ico") , "GREEN BLINK ON" , this, &CChamberView::SlotGreenLampBlinkClicked );

    m_pMenuLampOn->addAction( QIcon("://icon/red_marble.ico")      , "RED OFF"         , this, &CChamberView::SlotRedLampOffClicked        );
    m_pMenuLampOn->addAction( QIcon("://icon/yellow_rectangle.ico"), "YELLOW OFF"      , this, &CChamberView::SlotYellowLampOffClicked     );
    m_pMenuLampOn->addAction( QIcon("://icon/green_rectangle.ico") , "GREEN OFF"       , this, &CChamberView::SlotGreenLampOffClicked      );
    m_pMenuLampOn->addAction( QIcon("://icon/red_marble.ico")      , "RED BLINK OFF"   , this, &CChamberView::SlotRedLampBlinkOffClicked   );
    m_pMenuLampOn->addAction( QIcon("://icon/yellow_rectangle.ico"), "YELLOW BLINK OFF", this, &CChamberView::SlotYellowLampBlinkOffClicked);
    m_pMenuLampOn->addAction( QIcon("://icon/green_rectangle.ico") , "GREEN BLINK OFF" , this, &CChamberView::SlotGreenLampBlinkOffClicked );
	ui->tbLampOn->setMenu( m_pMenuLampOn );
}


void CChamberView::InitButtons(bool bCon)
{
	ui->btnRun->setEnabled( bCon );
	ui->btnStop->setEnabled( bCon );
	ui->btnAmbMode->setEnabled( bCon );
	ui->btnAmbAuto->setEnabled( bCon );
	ui->btnAlarmReset->setEnabled( bCon );
	ui->btnMaskOn->setEnabled( bCon );
	ui->btnMaskOff->setEnabled( bCon );
	ui->btnWatchdogEn->setEnabled( bCon );
    ui->btnWatchdogDis->setEnabled( bCon );
	ui->btnDoorClose->setEnabled( bCon );
	ui->btnDoorOpen->setEnabled( bCon );
	ui->btnRackIn->setEnabled( bCon );
	ui->btnRackOut->setEnabled( bCon );
	ui->btnSetTemp->setEnabled( bCon );
	ui->btnSetAmb->setEnabled( bCon );
	ui->btnRackOprPlc->setEnabled( bCon );
    ui->btnRackOprLocal->setEnabled( bCon );
	ui->btnBuzzerOff->setEnabled( bCon );
	ui->btnBuzzerOn->setEnabled( bCon );
	ui->tbLampModeOff->setEnabled( bCon );
	ui->tbLampOn->setEnabled( bCon );
	ui->btnDcPowerOn->setEnabled( bCon );	//2023.09.19 - KWMOON
	ui->btnDcPowerOff->setEnabled( bCon );	//2023.09.19 - KWMOON
	ui->btnRackInFingerUpOn->setEnabled( bCon );
	ui->btnRackInFingerUpOff->setEnabled( bCon );
	ui->btnDoorIgnoreRackInOutOn->setEnabled( bCon );
	ui->btnDoorIgnoreRackInOutOff->setEnabled( bCon );
}

void CChamberView::InitChmbStatus()
{
	SetChmbRunStatus( EN_CHMB_STOP );

	SetChmbStatus( CHMB_STAT1_MASK_OFF          , false );
	SetChmbStatus( CHMB_STAT1_MASK_ON           , false );
	SetChmbStatus( CHMB_STAT1_WATCHDOG_ENAB     , false );
	SetChmbStatus( CHMB_STAT1_WATCHDOG_DISB     , false );
	SetChmbStatus( CHMB_STAT1_DOOR_OPEN         , false );
	SetChmbStatus( CHMB_STAT1_DOOR_CLOSE        , false );
	SetChmbStatus( CHMB_STAT1_UPPER_RACK_DETECT , false );
	SetChmbStatus( CHMB_STAT1_BOTTOM_RACK_DETECT, false );
	SetChmbStatus( CHMB_STAT1_RACK_IN_COMPLETE  , false );
	SetChmbStatus( CHMB_STAT1_RACK_OUT_COMPLETE , false );
	SetChmbStatus( CHMB_STAT1_EQ_READY_STATUS   , false );
	SetChmbStatus( CHMB_STAT1_EQ_ABORT_STATUS   , false );
	SetChmbStatus( CHMB_STAT1_AVG_JOB_COMPLETE  , false );

	SetChmbStatus2( CHMB_STAT2_DC_PWR , false );
	SetChmbStatus2( CHMB_STAT2_TOWERLAMP_LOCAL , false );
	SetChmbStatus2( CHMB_STAT2_TOWERLAMP_REMOTE , false );
	SetChmbStatus2( CHMB_STAT2_FINGER_UP_STAT , false );			//2023.09.08 - KWMOON
	SetChmbStatus2( CHMB_STAT2_DOOR_IGNORE_RACK_STAT , false );		//2023.09.08 - KWMOON

	SetChmbStatusRackOpr( EN_RACK_OPR_UNKNOWN );	//2023.09.08 - KWMOON
}

void CChamberView::InitDisplay(double initVal)
{
	DisplaySv(initVal);
	DisplayPv(initVal);
	DisplayPcw(initVal);
	DisplayWps(initVal);
	DisplayPcwFlow(initVal);
}

void CChamberView::SetChmbRunStatus(EN_CHMB_RUN_STAT stat)
{
	switch ( stat )
	{
		case EN_CHMB_RUN:
			m_pLblChmbRun[EN_CHMB_RUN]->setStyleSheet( LABEL_RUN_STYLE );
			m_pLblChmbRun[EN_CHMB_STOP]->setStyleSheet( LABEL_DISB_BG );
			m_pLblChmbRun[EN_CHMB_AMB]->setStyleSheet( LABEL_DISB_BG );
			break;
		case EN_CHMB_STOP:
			m_pLblChmbRun[EN_CHMB_RUN]->setStyleSheet( LABEL_DISB_BG );
			m_pLblChmbRun[EN_CHMB_STOP]->setStyleSheet( LABEL_STOP_STYLE );
			m_pLblChmbRun[EN_CHMB_AMB]->setStyleSheet( LABEL_DISB_BG );
			break;
		case EN_CHMB_AMB:
			m_pLblChmbRun[EN_CHMB_RUN]->setStyleSheet( LABEL_DISB_BG );
			m_pLblChmbRun[EN_CHMB_STOP]->setStyleSheet( LABEL_DISB_BG );
			m_pLblChmbRun[EN_CHMB_AMB]->setStyleSheet( LABEL_AMBIENT_STYLE );
			break;
		default:
			break;
	}
}

void CChamberView::SetChmbStatus(EN_WORD_STATUS1 stat, bool onoff)
{
	if( m_pLblChmbStat[stat] == nullptr ) return;
	if( onoff )
		m_pLblChmbStat[stat]->setStyleSheet( LABEL_ENAB_BG );
	else
		m_pLblChmbStat[stat]->setStyleSheet( LABEL_DISB_BG );
}

void CChamberView::SetChmbStatus2(EN_WORD_STATUS2 stat, bool onoff )
{
	if( m_pLblChmbStat2[stat] == nullptr ) return;

	if( onoff )
		m_pLblChmbStat2[stat]->setStyleSheet( LABEL_ENAB_BG );
	else
		m_pLblChmbStat2[stat]->setStyleSheet( LABEL_DISB_BG );
}

void CChamberView::SetChmbStatusRackOpr(EN_RACK_OPR_STAT stat)
{
	switch (stat) {
		case EN_RACK_OPR_UNKNOWN:
			ui->statusRackOprChmb->setStyleSheet( LABEL_DISB_BG );
			ui->statusRackOprRemote->setStyleSheet( LABEL_DISB_BG );
			break;
		case EN_RACK_OPR_LOCAL:
			ui->statusRackOprChmb->setStyleSheet( LABEL_ENAB_BG );
			ui->statusRackOprRemote->setStyleSheet( LABEL_DISB_BG );
			break;
		case EN_RACK_OPR_REMOTE:
			ui->statusRackOprChmb->setStyleSheet( LABEL_DISB_BG );
			ui->statusRackOprRemote->setStyleSheet( LABEL_ENAB_BG );
			break;
		default:
			break;
	}
}

void CChamberView::InitSysLogView()
{
	QFont font = ui->teSysLog->font();
	font.setPointSize(9);
	font.setFamily("DejaVu Sans Mono");
	ui->teSysLog->setFont(font);
	ui->teSysLog->setReadOnly(true);
	ui->teSysLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
	ui->teSysLog->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->teSysLog->setMaximumBlockCount( MAX_LOG_LINE_NUM );
	ui->teSysLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
	connect( ui->teSysLog, &QPlainTextEdit::customContextMenuRequested, this, &CChamberView::SlotCustomContextMenuRequested );
}

void CChamberView::InitPlcLogView()
{
	QFont font = ui->tePlcLog->font();
	font.setPointSize(9);
	font.setFamily("DejaVu Sans Mono");
	ui->tePlcLog->setFont(font);
	ui->tePlcLog->setReadOnly(true);
	ui->tePlcLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
	ui->tePlcLog->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->tePlcLog->setMaximumBlockCount( MAX_LOG_LINE_NUM );
	ui->tePlcLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
	connect( ui->tePlcLog, &QPlainTextEdit::customContextMenuRequested, this, &CChamberView::SlotCustomContextMenuRequested );
}

void CChamberView::InitAlarmList()
{
	for( int i=0;i<MAX_ALARM_WORD_ADDR;i++ )
	{
		m_vecAlarmData[i].clear();
	}
	//D150
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Chamber Main MCCB Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Reverse Phase Alarm"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Chamber Main MC Off Alarm"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "VSP Input Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "480->220 TR CP Trip Alarm"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "VSP Output Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Emergency Stop Alarm"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Opposite Emergency Stop Alarm"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "TR Input Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "TR Output Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "SMPS Input Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "SMPS Output Power Trip"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_WARNING_LEVEL, "System DC Off Alarm"} );		//2023.05.05 - KWMOON
	m_vecAlarmData[0].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[0].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );

	//D151
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Main Heater ELCB Trip"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Main Heater MC OFF Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_WARNING_LEVEL, "Main Heater SSR 60'C Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Main Heater SSR 80'C Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Main Heater EGO Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Main Heater Temp Bimetal Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Blower Motor#1 CP Trip"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Blower Motor#1 INV OFF Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Blower Motor#2 CP Trip"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Blower Motor#2 INV OFF Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Blower Motor INV Alarm"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[1].push_back( stAlarmData{ EN_HEAVY_LEVEL, "Smoke Sensor#4 Alarm"} );	//2023.05.05 - KWMOON

	//D152
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 MCCB Trip"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 MC Off Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 EOCR Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 DPS LOW PRE. Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 DPS HIGH PRE. Alarm"} );
    m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp WPS Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp Leak Sensor Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp Water Flow Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 MCCB Trip"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 MC OFF Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 EOCR Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 DPS LOW PRE. Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 DPS HIGH PRE. Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Air Pre. Alarm"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Servo Power Trip"} );
	m_vecAlarmData[2].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Servo Alarm"} );

	//D153
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Door Open Time Out Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Door Close Time Out Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Door Press Time Out Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Door Release Time Out Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_NO_WARN_ERROR, ""} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Protrusion detection during input"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack IN Limit Detect"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack OUT Limit Detect"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Light Curtain Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_WARNING_LEVEL, "Side Cover Open Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Smoke Sensor#1 Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Smoke Sensor#2 Alarm"} );
	m_vecAlarmData[3].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Smoke Sensor#3 Alarm"} );

	//D154
	m_vecAlarmData[4].push_back( stAlarmData{ EN_WARNING_LEVEL, "Upper Fan Power Trip"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Door Heater ELCB Trip"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Door Heater SSR80'C Alarm"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_WARNING_LEVEL, "Door Heater SSR60'C Alarm"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Gear Epoxy ELCB Trip"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Gear Epoxy SSR80'C Alarm"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_WARNING_LEVEL, "Gear Epoxy SSR60'C Alarm"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[4].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );

	//D155
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "System Main ELCB Trip"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "System Main MC Off Alarm"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "System SMPS#1 MCB Trip"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "System SMPS#2 MCB Trip"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "Area/Door Error During Input"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_WARNING_LEVEL, "Area/Door Error During Removing"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[5].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );

	//D156
	m_vecAlarmData[6].push_back( stAlarmData{ EN_WARNING_LEVEL, "PCW TEMP High/Low Alarm"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[6].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );

	//D157
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#01 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#02 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#03 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#04 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#05 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#06 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#07 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#08 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#09 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#10 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#11 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#12 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#13 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#14 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#15 Off Alarm"} );
	m_vecAlarmData[7].push_back( stAlarmData{ EN_WARNING_LEVEL, "Rack Cooling Fan#16 Off Alarm"} );

	//D158
	m_vecAlarmData[8].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp1 PRE. High Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp1 PRE. Low Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp2 PRE. High Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp2 PRE. Low Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp1 TEMP High Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp1 TEMP Low Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp2 TEMP High Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_WARNING_LEVEL, "Comp2 TEMP Low Warn Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 TEMP High Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp1 TEMP Low Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 TEMP High Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Comp2 TEMP Low Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Chamber High TEMP Alarm"} );
	m_vecAlarmData[8].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Chamber Low TEMP Alarm"} );

	//D159
	m_vecAlarmData[9].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Chamber Over TEMP Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Chamber Over TEMP Deviation Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Chamber Under TEMP Deviation Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_HEAVY_LEVEL  , "Ramp UP/DN Limit Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Watch Dog Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "TimeOut_PIO_Alarm"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Busy_Signal_Not_Turn_ON_Valid_Si"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Valid_Signal_Off_Before_Busy_Ala"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_WARNING_LEVEL, "Servo Home Nonactivation Alarm"} );	//2023.08.29 - KWMOON
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
	m_vecAlarmData[9].push_back( stAlarmData{ EN_NO_WARN_ERROR, "-"} );
}

void CChamberView::InitAlarmTree()
{
	QFont font;
	font.setPointSize(10);
	font.setFamily("DejaVu Sans Mono");
	ui->treeAlarmList->setFont(font);
	ui->treeAlarmList->clear();
	ui->treeAlarmList->header()->setStretchLastSection( true );
	ui->treeAlarmList->setColumnHidden( HEADER_ALARM_INDEX , true );
	ui->treeAlarmList->setColumnHidden( HEADER_ALARM_BIT   , true );
    ui->treeAlarmList->setColumnWidth ( HEADER_ALARM_DATE   , 200 );
    ui->treeAlarmList->setColumnWidth ( HEADER_ALARM_UPDATE , 200 );
    ui->treeAlarmList->setColumnWidth ( HEADER_ALARM_LEVEL  , 100 );
}

void CChamberView::DisplaySv( double val )
{
    QString str = QString("").asprintf("%6.1f", val);
	ui->lcdSV->display( str );
}

void CChamberView::DisplayPv( double val )
{
    QString str = QString("").asprintf("%6.1f", val);
	ui->lcdPV->display( str );
}

void CChamberView::DisplayPcw( double val )
{
	QString str = QString("").asprintf("%6.2f", val);
	ui->lcdPCW->display( str );
}

void CChamberView::DisplayWps( double val )
{
	QString str = QString("").asprintf("%6.2f", val);
	ui->lcdWPS->display( str );
}

void CChamberView::DisplayPcwFlow( double val )
{
	QString str = QString("").asprintf("%6.2f", val);
	ui->lcdPCWFlow->display( str );
}

void CChamberView::InitInstance()
{
	m_pIpcChmb  = new CLocalSocket( ATCHMBUI_NAME );
	m_pIpcChmb->SetHostInfo( IPC_INFO::IPC_CHMBCOMM_SOCK );
	m_pSv = new CDlgSv( this );
	m_pSv->setTempLimit( DEFAULT_CHMB_TEMP_LIMIT_LOW, DEFAULT_CHMB_TEMP_LIMIT_HIGH );

	m_pPassDlg = new CPassDlg( this );

	connect( this          , &CChamberView::SigIpcChmbCommConnect    , m_pIpcChmb  , &CLocalSocket::SlotConnectToHost     );
	connect( this          , &CChamberView::SigIpcChmbCommSendPacket , m_pIpcChmb  , &CLocalSocket::SlotSendPacket        );

	connect( m_pIpcChmb    , &CLocalSocket::SigIpcConnectionStatus   , this  , &CChamberView::SlotIpcChmbCommConnStatus   );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcRecvPacket         , this  , &CChamberView::SlotIpcChmbCommRecvPacket   );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcError              , this  , &CChamberView::SlotIpcChmbCommError        );
	connect( m_pIpcChmb    , &CLocalSocket::SigIpcLogMessage         , this  , &CChamberView::SlotIpcChmbCommLogMessage   );
	connect( m_pSv         , &CDlgSv::SigSvSet                       , this  , &CChamberView::SlotSvSet                   );
	connect( m_pPassDlg    , &CPassDlg::SigAccepted                  , this  , &CChamberView::SlotPasswdDlgAccepted       );
}

void CChamberView::SetSharedMemoryStat(bool on)
{
	if( on ) { ui->lblSaveStatus->setPixmap( m_pixmapOn  ); ui->lblSaveStatus->setToolTip( tr("save %1").arg(m_sTempDatFile) ); }
	else     { ui->lblSaveStatus->setPixmap( m_pixmapOff ); ui->lblSaveStatus->setToolTip("save stopped..");}
}

//2023.06.19 - KWMOON
void CChamberView::InitSharedMemory()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	if( env_data != NULL )
	{
		m_sDataDir = tr("%1/%2").arg(env_data).arg(DIR_TEMPERATURE);
	}
	else
	{
		m_sDataDir = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_TEMPERATURE);
	}
	QDir dir(m_sDataDir);
	if( !dir.exists( m_sDataDir)  )	dir.mkdir( m_sDataDir );

	SetSharedMemoryStat( false );

	m_sTempDatFile= tr("%1/%2").arg(m_sDataDir).arg(DAT_TEMPERATURE);
	m_pShmMainUi = new QSharedMemory( SHM_KEY::SHM_MAIN_CHMB_KEY, this );
	m_pShmMainUi->create( sizeof(stShmMainChmb), QSharedMemory::ReadWrite );
	if( ! m_pShmMainUi->attach( QSharedMemory::ReadOnly ) )
	{
		AppendSystemLog( _ERROR_LOG , tr("%1 attach error").arg(SHM_KEY::SHM_MAIN_CHMB_KEY) );
		return;
	}
	m_pShmMainUi->lock();
	stShmMainChmb ShmData;
	stShmMainChmb* from = (stShmMainChmb*)m_pShmMainUi->constData();
	memcpy( &ShmData, from, sizeof(stShmMainChmb) );
	m_pShmMainUi->unlock();
	m_pShmMainUi->detach();

	AppendSystemLog( _NORMAL_LOG , tr("%1 Init Use[%2] Test[%3]").arg(SHM_KEY::SHM_MAIN_CHMB_KEY).arg(ShmData.bUse).arg(ShmData.bTest) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChamberView::IsSharedMemorySet
/// \return
/// \details main ui set true, start monitoring chamber
bool CChamberView::IsSharedMemorySet()
{
	if( !m_pShmMainUi->isAttached() )
	{
		if( ! m_pShmMainUi->attach( QSharedMemory::ReadOnly ) )
		{
			//ui->lblSaveStatus->setPixmap( QPixmap(""));
			AppendSystemLog( _ERROR_LOG , tr("%1 attach error").arg(SHM_KEY::SHM_MAIN_CHMB_KEY) );
			SetSharedMemoryStat( false );
			return false;
		}
	}

	m_pShmMainUi->lock();
	stShmMainChmb ShmData;
	memset( &ShmData, 0x0, sizeof(stShmMainChmb) );
	stShmMainChmb* from = (stShmMainChmb*)m_pShmMainUi->constData();
	if( from != NULL )
	{
		memcpy( &ShmData, from, sizeof(stShmMainChmb) );
	}
	m_pShmMainUi->unlock();
	//m_pShmMainUi->detach();
	bool bMon = (ShmData.bUse && ShmData.bTest);
	SetSharedMemoryStat( bMon );

	//AppendSystemLog( _NORMAL_LOG , tr("%1 [%2/%3]").arg(__FUNCTION__).arg(ShmData.bUse).arg(ShmData.bTest) );
	return bMon;
}

//for mainui temperature graph
bool CChamberView::AppendTemperatureDat()
{
	QString dat = "";
	int nStat   = 0;
		 if( m_stChmbStat.bStop ) nStat = 0;
	else if( m_stChmbStat.bRun  ) nStat = 1;
	else if( m_stChmbStat.bAmb  ) nStat = 2;
	else                          nStat = 0;

	if( IsSharedMemorySet() )
	{
		dat = tr("%1,%2,%3,%4").arg(UTIL::DATE::yyyyMMDDHHmmss()).arg(nStat).arg(m_stChmbStat.dPv).arg(m_stChmbStat.dSv);
		UTIL::FILE::AppendFile( m_sTempDatFile, dat );
		//AppendSystemLog( _NORMAL_LOG , tr("%1 : %2").arg(m_sTempDatFile).arg(dat) );
		return true;
	}
	return false;
}

void CChamberView::InitTimer()
{
	m_timerChmbConn = new QTimer( this );
	m_timerChmbStatus = new QTimer( this );
    m_timerWatchdog = new QTimer( this );
	m_timerCompressure1_2 = new QTimer( this );	//2024.08.19 - KWMOON
	connect(m_timerChmbConn      , &QTimer::timeout, this, &CChamberView::SlotTimeoutChmbPlc         );
	connect(m_timerChmbStatus    , &QTimer::timeout, this, &CChamberView::SlotTimeoutChmbStat        );
	connect(m_timerWatchdog      , &QTimer::timeout, this, &CChamberView::SlotTimeoutChmbWatchdog    );
	connect(m_timerCompressure1_2, &QTimer::timeout, this, &CChamberView::SlotTimeoutChmbCompressure );	//2024.08.19 - KWMOON
}

void CChamberView::KillTimer()
{
	if( m_timerChmbConn->isActive() ) m_timerChmbConn->stop();
	if( m_timerChmbStatus->isActive() ) m_timerChmbStatus->stop();
    if( m_timerWatchdog->isActive() ) m_timerWatchdog->stop();
	if( m_timerCompressure1_2->isActive() ) m_timerCompressure1_2->stop();
}

void CChamberView::SlotTimeoutChmbWatchdog()
{
	SetChmbWatchdog();
}

void CChamberView::SlotTimeoutChmbPlc()
{
	if( !m_pIpcChmb->IsConn() )
	{
		m_pIpcChmb->SetHostInfo( IPC_INFO::IPC_CHMBCOMM_SOCK );
		emit SigIpcChmbCommConnect();
	}
}

void CChamberView::SlotTimeoutChmbStat()
{
	if( !m_bConnIpcPlc[CONN_IPC] ) return;
	if( !m_bConnIpcPlc[CONN_PLC] ) return;

	bool bSent = true;
	//STATUS READ COMMAND
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_PV      );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_SV      );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_PCW_TEMP );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_WPS_PRESSURE );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_PCW_FLOW );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1 );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2 );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM0  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM1  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM2  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM3  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM4  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM5  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM6  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM7  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM8  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM9  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RB_RACK_NOP_TRIGGER  );	//2023.09.07 - KWMOON

	if( !bSent )
		AppendSystemLog( _ERROR_LOG , tr("[SEND] STATUS READ NG") );

    if( m_vecAlarmList.size() > 0 )
    {
        const QIcon icon = QIcon("://icon/ng.ico");
        ui->tabLog->setTabIcon( TAB_ALARM, icon );
    }
    else
    {
        const QIcon icon = QIcon("://icon/ok.ico");
        ui->tabLog->setTabIcon( TAB_ALARM, icon );
    }

	//2023.06.20 - KWMOON
	AppendTemperatureDat();
}

void CChamberView::SlotStartChamberIpc()
{
	emit SigIpcChmbCommConnect();
	m_timerChmbConn->start ( TIMER_CHMB_COMM  );
	m_timerChmbStatus->start( TIMER_CHMB_STAT );
	m_timerCompressure1_2->start( TIMER_CHMB_COMP1_2 );	//2024.08.19 - KWMOON

}

void CChamberView::SlotIpcChmbCommLogMessage (LOG_TYPE t, QString s)
{
	AppendSystemLog( t, s );
}

void CChamberView::SlotIpcChmbCommError ( QLocalSocket::LocalSocketError err, QString msg )
{
	AppendSystemLog( _ERROR_LOG, msg );
}

void CChamberView::SlotIpcChmbCommConnStatus(bool conn)
{
	if( conn )
	{
		AppendSystemLog( _NORMAL_LOG, tr("%1 connected!").arg(IPC_INFO::IPC_CHMBCOMM_SOCK) );
	}
	else
	{
		if( !m_pIpcChmb->IsConn() )
		AppendSystemLog( _ERROR_LOG , tr("%1 disconnected!").arg(IPC_INFO::IPC_CHMBCOMM_SOCK) );
    }
	m_bConnIpcPlc[CONN_IPC] = conn;

	if( m_bConnIpcPlc[CONN_IPC] && m_bConnIpcPlc[CONN_PLC] )
	{
		InitButtons( true );
	}
    else
    {

    }
    //InitButtons( true );		//for test
	emit SigIpcChmbCommStatus( m_bConnIpcPlc[CONN_IPC] , m_bConnIpcPlc[CONN_PLC] );
}

bool CChamberView::GetChmbStatus ( PROTOCOL::PLC_COMMAND cmd )
{
    if( !m_bConnIpcPlc[CONN_IPC] && !m_bConnIpcPlc[CONN_PLC] )
	{
        //AppendSystemLog( _ERROR_LOG, tr("[SEND] %1 error!").arg(UTIL::CHAMBER::PLCCMD_STR(cmd)) );
		return false;
	}
    QMutexLocker locker(&m_mutex);

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATCHMB_UI;
	head.datalen = 0;
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader);
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer, (char*)&head, size );
	emit SigIpcChmbCommSendPacket( pSendBuffer, size );
	return true;
}

bool CChamberView::SetChmbBitWrite ( PROTOCOL::PLC_COMMAND cmd, int bit)
{
    if( !m_bConnIpcPlc[CONN_IPC] && !m_bConnIpcPlc[CONN_PLC] )
	{
        //AppendSystemLog( _ERROR_LOG, tr("[SEND] %1 error!").arg(UTIL::CHAMBER::PLCCMD_STR(cmd)) );
		return false;
	}
	QMutexLocker locker(&m_mutex);

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATCHMB_UI;
    head.datalen = 0x1; //bit -> 1byte
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader) + head.datalen;
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer                    , (char*)&head, sizeof(stIpcHeader) );
	memcpy( pSendBuffer+sizeof(stIpcHeader), (char*)&bit , head.datalen        );

	emit SigIpcChmbCommSendPacket( pSendBuffer, size );
	QString log = QString("").asprintf("[INFO] UI %s [SET:0x%01X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), bit&0x1 );
	AppendSystemLog( _NORMAL_LOG, log );
	return true;
}

bool CChamberView::SetChmbWordWrite( PROTOCOL::PLC_COMMAND cmd, int word)
{
	if( !m_bConnIpcPlc[CONN_IPC] && !m_bConnIpcPlc[CONN_PLC] )
	{
		//AppendSystemLog( _ERROR_LOG, tr("[SEND] %1 error!").arg(UTIL::CHAMBER::PLCCMD_STR(cmd)) );
		return false;
	}

	QMutexLocker locker(&m_mutex);

	stIpcHeader head;
	head.cmd     = cmd;
	head.dest    = PROTOCOL::_ATCHMB_COMM;
	head.src     = PROTOCOL::_ATCHMB_UI;
    head.datalen = 0x2; //word -> 2byte
	head.slot    = 0x0;
	head.runner  = 0x0;

	uint size = sizeof(stIpcHeader) + head.datalen;
	char* pSendBuffer = new char[ size ];
	memcpy( pSendBuffer                    , (char*)&head, sizeof(stIpcHeader) );
	memcpy( pSendBuffer+sizeof(stIpcHeader), (char*)&word, head.datalen        );
	emit SigIpcChmbCommSendPacket( pSendBuffer, size );

	if( (cmd == PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE ) || (cmd == PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE) )
	{
        double dSv = static_cast<double>(word/10.0);
		QString log = QString("").asprintf("[INFO] UI %s [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), dSv, word&0xffff );
		AppendSystemLog( _NORMAL_LOG, log );
	}
	else
	{
		if( cmd == PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER ) return true;

		QString log = QString("").asprintf("[INFO] UI %s [0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), word&0xffff );
		AppendSystemLog( _NORMAL_LOG, log );
	}
	return true;
}

void CChamberView::SlotIpcChmbCommRecvPacket( stIpcElement emt)
{
	PROTOCOL::PLC_COMMAND cmd = static_cast<PROTOCOL::PLC_COMMAND>( emt.ipcHeader.cmd );
    unsigned char szBuf[32]={0x0,};
	uint datalen = emt.ipcHeader.datalen;
	int error    = emt.ipcHeader.error;

    memset( szBuf, 0x0, sizeof(szBuf) );
    memcpy( szBuf, emt.pData, datalen );

    //QString log = QString("").asprintf( "pData=[0]0x%2X [1]0x%02X", szBuf[0]&0xFF, szBuf[1]&0xFF );
    //AppendPlcLog( _ERROR_LOG, log );

	switch ( cmd )
	{
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET:
		case PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TIME_SET:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB:
		case PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE:
		case PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE:
		{
			QString log = QString("").asprintf( "[RECV] %s  %s", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK" );
			if( error ) AppendPlcLog( _ERROR_LOG , log );
			//else        AppendPlcLog( _NORMAL_LOG, log );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_PLC_VER:
		{
            int nVer = 0x0;
            short uVer;
            double dVer = 0.0;
            if( datalen == 2 )
			{

                nVer |= (static_cast<int>(szBuf[0])<<8);
                nVer |= (static_cast<int>(szBuf[1])<<0);
                nVer &= 0xFFFF;
                uVer  = static_cast<short>(nVer&0xFFFF);
                dVer = static_cast<double>(uVer) / 10.0;
			}
            QString log = QString("").asprintf( "[RECV] %s  %s  [VER:%3.1f]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", dVer );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			else        { AppendPlcLog( _NORMAL_LOG , log ); }

            ui->lblPlcVer->setText( QString("").asprintf("PLC VER : %3.1f", dVer) );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_PV:
		{
            uint nPv = 0x0;
            short uPv = 0;
			double dPv = 0.0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nPv  = (static_cast<int>(szBuf[0])<<8)&0xFF00;
                nPv |= (static_cast<int>(szBuf[1])<<0);
                uPv  = static_cast<short>(nPv&0xFFFF);
                dPv = static_cast<double>(uPv) / 10.0;
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", dPv, uPv&0xFFFF );

			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
			DisplayPv( dPv );
			m_stChmbStat.dPv = dPv;	//2023.06.19 - KWMOON
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_SV:
		{
            uint nSv = 0x0;
            short uSv = 0;
			double dSv = 0.0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nSv  = (static_cast<int>(szBuf[0])<<8);
                nSv |= (static_cast<int>(szBuf[1])<<0);
                uSv  = static_cast<short>(nSv&0xFFFF);
                dSv = static_cast<double>(uSv) / 10.0;
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", dSv, uSv&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
            DisplaySv( dSv );
			m_stChmbStat.dSv = dSv;	//2023.06.19 - KWMOON
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_PCW_TEMP:
		{
            uint nPcw = 0x0;
            short uPcw = 0;
			double dPcw = 0.0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nPcw  = (static_cast<int>(szBuf[0])<<8);
                nPcw |= (static_cast<int>(szBuf[1])<<0);
                uPcw = static_cast<short>(nPcw);
                dPcw = static_cast<double>(uPcw) / 10.0;
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK",  dPcw, uPcw&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
			DisplayPcw( dPcw );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_WPS_PRESSURE:
		{
            uint nWps = 0x0;
            short uWps = 0;
			double dWps = 0.0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nWps  = (static_cast<int>(szBuf[0])<<8);
                nWps |= (static_cast<int>(szBuf[1])<<0);
                uWps = static_cast<short>(nWps);
                dWps = static_cast<double>(nWps) / 100.0;
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK",  dWps, uWps&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
			DisplayWps( dWps );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_PCW_FLOW:
		{
            uint nFlow = 0x0;
            short uFlow = 0;
			double dFlow = 0.0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nFlow |= (static_cast<int>(szBuf[0])<<8);
                nFlow |= (static_cast<int>(szBuf[1])<<0);
                uFlow = static_cast<short>(nFlow);
                dFlow = static_cast<double>(nFlow) / 10.0;
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [%6.1f:0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK",  dFlow, uFlow&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
			DisplayPcwFlow( dFlow );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1:
		{
            uint nD100 = 0x0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nD100 |= (static_cast<int>(szBuf[0])<<8);
                nD100 |= (static_cast<int>(szBuf[1])<<0);
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", nD100&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG, log ); }
			DisplayChmbWordStatus( cmd, nD100 );
			//ui->lcdWPS->display( sFlow );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2:
		{
            uint nD101 = 0x0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nD101 |= (static_cast<int>(szBuf[0])<<8);
                nD101 |= (static_cast<int>(szBuf[1])<<0);
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", nD101&0xFFFF );
			if( error ) AppendPlcLog( _ERROR_LOG , log );
			//else        AppendPlcLog( _NORMAL_LOG, log );
			DisplayChmbWordStatus( cmd, nD101 );
			//ui->lcdWPS->display( sFlow );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM0:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM1:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM2:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM3:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM4:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM5:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM6:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM7:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM8:
		case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM9:
		{
			int index = cmd - PROTOCOL::PLC_COMMAND::CMD_RW_ALARM0;
			int nD150_159 = 0x0;
			if( datalen == 2 )
			{
                //memcpy( szBuf, emt.pData, datalen );
                nD150_159 |= (static_cast<int>(szBuf[0])<<8);
                nD150_159 |= (static_cast<int>(szBuf[1])<<0);
			}
			QString log = QString("").asprintf( "[RECV] %s  %s  [0x%04X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", nD150_159&0xFFFF );
			if( error ) { AppendPlcLog( _ERROR_LOG  , log ); }
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
            MakeWordAlarmList( cmd, index, nD150_159 );
			//ui->lcdWPS->display( sFlow );
		}
		break;

		case PROTOCOL::PLC_COMMAND::CMD_SYS_PLC_CONN:
		{
			if( error == 0x1 )
				m_bConnIpcPlc[CONN_PLC] = false;
			else
				m_bConnIpcPlc[CONN_PLC] = true;

            if( m_bConnIpcPlc[CONN_IPC] && m_bConnIpcPlc[CONN_PLC] )
			{
                InitButtons( true );

                if( !m_timerWatchdog->isActive() )
                {
                    GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_PLC_VER );     //2023.09.20 - kwmoon(once run)
                    SetChmbWatchdogPeriod();
                    SetChmbWatchdogOnOff(true);
                    SetChmbAmbSv();
                    SetChmbWatchdog();              //2023.07.14 - kwmoon
                    SetChmbAlarmReset();            //2023.07.14 - kwmoon
					int nPeriod = m_nWatchdogPeriod-2;
					if( nPeriod < 1 ) nPeriod = 1;
                    m_timerWatchdog->start( nPeriod * 1000 );
                    AppendSystemLog( _NORMAL_LOG, tr("Start Watchdog : %1 seconds").arg(nPeriod) );
                }
			}
            else
			{
                InitButtons( false );
				InitChmbStatus();
				InitDisplay(0);

				for (int i = 0; i < MAX_WORD_STATUS1; i++)
				{
					m_pLblChmbStat[i]->setStyleSheet( LABEL_DISB_BG );
				}

                if( m_timerWatchdog->isActive() )
                {
                    m_timerWatchdog->stop();
                    AppendSystemLog( _NORMAL_LOG, tr("Stop Watchdog") );
                }
			}
            emit SigIpcChmbCommStatus( m_bConnIpcPlc[CONN_IPC] , m_bConnIpcPlc[CONN_PLC] );
		}
		break;

		//2023.09.08 - KWMOON
		case PROTOCOL::PLC_COMMAND::CMD_RB_RACK_NOP_TRIGGER:
		{
			int nBit  = 0x0;
			if( datalen != 1 )
				return;
			nBit = static_cast<int>(szBuf[0]);
			QString log = QString("").asprintf( "[RECV] %s  %s  [0x%02X]", UTIL::CHAMBER::PLCCMD_STR(cmd).toStdString().c_str(), error?"NG":"OK", nBit&0x1 );
			if( error )
			{
				AppendPlcLog( _ERROR_LOG  , log );
				return;
			}
			//else        { AppendPlcLog( _NORMAL_LOG , log ); }
			if( nBit == 0 )
				SetChmbStatusRackOpr( EN_RACK_OPR_LOCAL );
			else
				SetChmbStatusRackOpr( EN_RACK_OPR_REMOTE );
		}
		break;

		//2024.08.19 - KWMOON
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_PRESSURE_HIGH_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_PRESSURE_LOW_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_PRESSURE_HIGH_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_PRESSURE_LOW_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_OUT_TEMP_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_IN_TEMP_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_OUT_TEMP_VALUE:
		case PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_IN_TEMP_VALUE:
		{
			int nValue = 0;
			short uValue = 0;
			double dValue = 0.0;
			if( datalen == 2 )
			{
				int index = cmd - PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_PRESSURE_HIGH_VALUE;
				//memcpy( szBuf, emt.pData, datalen );
				nValue |= (static_cast<int>(szBuf[0])<<8);
				nValue |= (static_cast<int>(szBuf[1])<<0);
				uValue = static_cast<short>(nValue);
				dValue = static_cast<double>(nValue) / 10.0;
					 if( index == 0 ) m_stCompressure.dComp1PressHigh = dValue;
				else if( index == 1 ) m_stCompressure.dComp1PressLow  = dValue;
				else if( index == 2 ) m_stCompressure.dComp2PressHigh = dValue;
				else if( index == 3 ) m_stCompressure.dComp2PressLow  = dValue;
				else if( index == 4 ) m_stCompressure.dComp1OutTemp   = dValue;
				else if( index == 5 ) m_stCompressure.dComp1InTemp    = dValue;
				else if( index == 6 ) m_stCompressure.dComp2OutTemp   = dValue;
				else if( index == 7 )
				{
					m_stCompressure.dComp2InTemp    = dValue;
					QString sLogComp1 = QString("").asprintf("COMP1 PRESSURE HIGH[%5.1f] LOW[%5.1f]   OUT_TEMP[%5.1f] IN_TEMP[%5.1f]",
															m_stCompressure.dComp1PressHigh, m_stCompressure.dComp1PressLow, m_stCompressure.dComp1OutTemp, m_stCompressure.dComp1InTemp );
					QString sLogComp2 = QString("").asprintf("COMP2 PRESSURE HIGH[%5.1f] LOW[%5.1f]   OUT_TEMP[%5.1f] IN_TEMP[%5.1f]",
															m_stCompressure.dComp2PressHigh, m_stCompressure.dComp2PressLow, m_stCompressure.dComp2OutTemp, m_stCompressure.dComp2InTemp );
					emit sigCompressureLog( LOG_TYPE::_NORMAL_LOG, sLogComp1 );
					emit sigCompressureLog( LOG_TYPE::_NORMAL_LOG, sLogComp2 );
				}
			}
		}
		break;

		default:
			break;
	}

	if( emt.ipcHeader.datalen > 0 )
	{
		if( emt.pData )
		{
			delete[] emt.pData;
			emt.pData = nullptr;
		}
	}
}

void CChamberView::DisplayChmbWordStatus( PROTOCOL::PLC_COMMAND cmd, int data)
{
	if( cmd == PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1 )
	{
		for( int i=0;i<MAX_WORD_STATUS1;i++ )
		{
			if( data &(1<<i) )
			{
				SetChmbStatus( static_cast<EN_WORD_STATUS1>(i), true );
			}
			else
			{
				SetChmbStatus( static_cast<EN_WORD_STATUS1>(i), false );
			}
		}
	}
    else if( cmd == PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2 )
	{
		if( data & (1<<CHMB_STAT2_RUN) )
		{
			SetChmbRunStatus( EN_CHMB_RUN_STAT::EN_CHMB_RUN );
			m_stChmbStat.bRun  = true;	//2023.06.19 - KWMOON
			m_stChmbStat.bStop = false;	//2023.06.19 - KWMOON
			m_stChmbStat.bAmb  = false;	//2023.06.19 - KWMOON
		}
		if( data & (1<<CHMB_STAT2_STOP) )
		{
			SetChmbRunStatus( EN_CHMB_RUN_STAT::EN_CHMB_STOP );
			m_stChmbStat.bRun  = false;	//2023.06.19 - KWMOON
			m_stChmbStat.bStop = true;	//2023.06.19 - KWMOON
			m_stChmbStat.bAmb  = false;	//2023.06.19 - KWMOON
		}
		if( data & (1<<CHMB_STAT2_AMB) )
		{
            SetChmbRunStatus( EN_CHMB_RUN_STAT::EN_CHMB_AMB );
			m_stChmbStat.bRun  = false;	//2023.06.19 - KWMOON
			m_stChmbStat.bStop = false;	//2023.06.19 - KWMOON
			m_stChmbStat.bAmb  = true;	//2023.06.19 - KWMOON
		}

		if( data & (1<<CHMB_STAT2_DC_PWR) )
			SetChmbStatus2( CHMB_STAT2_DC_PWR, true );
        else
			SetChmbStatus2( CHMB_STAT2_DC_PWR, false );

		if( data & (1<<CHMB_STAT2_TOWERLAMP_LOCAL) )
			SetChmbStatus2( CHMB_STAT2_TOWERLAMP_LOCAL, true );
		else
			SetChmbStatus2( CHMB_STAT2_TOWERLAMP_LOCAL, false );

		if( data & (1<<CHMB_STAT2_TOWERLAMP_REMOTE) )
			SetChmbStatus2( CHMB_STAT2_TOWERLAMP_REMOTE, true );
		else
			SetChmbStatus2( CHMB_STAT2_TOWERLAMP_REMOTE, false );
		/////////////////////////////////////////////////////////////////////////////////////
		/// 2023.09.07 - KWMOON
		/////////////////////////////////////////////////////////////////////////////////////
		if( data & (1<<CHMB_STAT2_FINGER_UP_STAT) )
			SetChmbStatus2( CHMB_STAT2_FINGER_UP_STAT, true );
		else
			SetChmbStatus2( CHMB_STAT2_FINGER_UP_STAT, false );

		if( data & (1<<CHMB_STAT2_DOOR_IGNORE_RACK_STAT) )
			SetChmbStatus2( CHMB_STAT2_DOOR_IGNORE_RACK_STAT, true );
		else
			SetChmbStatus2( CHMB_STAT2_DOOR_IGNORE_RACK_STAT, false );
	}
}

bool CChamberView::IsExistAlarm( int index, int bit )
{
	for( stAlarmList* p : m_vecAlarmList )
	{
		if( (p->index == index) && (p->bit == bit) ) return true;
	}
	return false;
}

bool CChamberView::UpdateAlarm( int index, int bit )
{
	bool bUpdate = false;
	for( stAlarmList* p : m_vecAlarmList )
	{
		if( (p->index == index) && (p->bit == bit) )
		{
            p->sUpdateTime = QString(UTIL::DATE::yyMMDDHHmmss( false ));
			bUpdate = true;
			break;
		}
	}
	return bUpdate;
}

bool CChamberView::RemoveAlarm( int index, int bit )
{
	bool bRemove = false;
	for( stAlarmList* p : m_vecAlarmList )
	{
		if( (p->index == index) && (p->bit == bit) )
		{
			m_vecAlarmList.removeOne( p );
			delete p;
			p = nullptr;
			bRemove = true;
			break;
		}
	}
	return bRemove;
}

void CChamberView::DisplayAlarmList()
{
	ui->treeAlarmList->clear();
	for( stAlarmList* p : m_vecAlarmList )
	{
		QString sLevel = "HEAVY";
		QTreeWidgetItem* top_item = new QTreeWidgetItem();
        if( p->level == EN_ALARM_LEVEL::EN_WARNING_LEVEL )
        {
            sLevel = "WARNING";
            top_item->setBackground( HEADER_ALARM_DATE   , WARN_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_UPDATE , WARN_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_LEVEL  , WARN_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_NAME   , WARN_ALARM_QCOLOR );

        }
        else
        {
            top_item->setBackground( HEADER_ALARM_DATE   , HEAVY_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_UPDATE , HEAVY_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_LEVEL  , HEAVY_ALARM_QCOLOR );
            top_item->setBackground( HEADER_ALARM_NAME   , HEAVY_ALARM_QCOLOR );
        }
        top_item->setText( HEADER_ALARM_DATE  , p->sDateTime   );
        top_item->setText( HEADER_ALARM_UPDATE, p->sUpdateTime );
        top_item->setText( HEADER_ALARM_LEVEL , sLevel );
        top_item->setText( HEADER_ALARM_NAME  , p->sAlarmName );

		ui->treeAlarmList->addTopLevelItem( top_item );
	}
}

void CChamberView::MakeWordAlarmList( PROTOCOL::PLC_COMMAND cmd, int index, int data)
{
	(void)cmd;
    int nAlarm = data & 0xFFFF; //( ((data&0xFF)<<8)&0xFF00) | ((data>>8)&0xFF) ;
	for( int a=0;a<16;a++ )
	{
		stAlarmData alarm_data = m_vecAlarmData[index].at(a);
		if( alarm_data.level == EN_ALARM_LEVEL::EN_NO_WARN_ERROR ) continue;
		if( !(nAlarm &(1<<a)) )
		{
			if( IsExistAlarm( index, a) )
			{
				RemoveAlarm( index, a );
			}
		}
		else
		{
			if( IsExistAlarm( index, a) )
			{
				UpdateAlarm( index, a );
			}
			else
			{
				stAlarmList* p = new stAlarmList;
				p->index = index;
				p->bit   = a;
                p->sDateTime   = UTIL::DATE::yyMMDDHHmmss( false );
                p->sUpdateTime = UTIL::DATE::yyMMDDHHmmss( false );
                p->level       = alarm_data.level;
                p->sAlarmName  = QString(alarm_data.sAlarmName);
				m_vecAlarmList.push_back( p );
			}
			if( alarm_data.level == EN_WARNING_LEVEL )
			{
				emit sigAlarmLog( LOG_TYPE::_NORMAL_LOG, tr("[ WARN] %1").arg(alarm_data.sAlarmName));
			}
			else if( alarm_data.level == EN_HEAVY_LEVEL )
			{
				emit sigAlarmLog( LOG_TYPE::_NORMAL_LOG, tr("[HEAVY] %1").arg(alarm_data.sAlarmName));
			}
			else {}

		}
	}
	DisplayAlarmList();
}

void CChamberView::SlotCustomContextMenuRequested( QPoint point )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;
	bool bSender = false;

	if( pSender == ui->teSysLog )
	{
		bSender = true;
		pAction = new QAction( tr("Auto Scroll On"), pSender );
		pAction->setCheckable( true );
		if( m_bAutoScrollSys ) pAction->setChecked(true);
		else                   pAction->setChecked(false);
		connect( pAction, &QAction::triggered, this, &CChamberView::SlotScrollStatusChanged );
		pMenu->addAction( pAction );

	}
	else if( pSender == ui->tePlcLog )
	{
		bSender = true;
		pAction = new QAction( tr("Auto Scroll On"), pSender );
		pAction->setCheckable( true );
		if( m_bAutoScrollChmb ) pAction->setChecked(true);
		else                    pAction->setChecked(false);
		connect( pAction, &QAction::triggered, this, &CChamberView::SlotScrollStatusChanged );
		pMenu->addAction( pAction );
	}
	else
	{

	}
	if( !bSender )
	{
		delete pMenu;
		return;
	}

	// Clear.
	// Because QPlainTextEdit::clear() is a slot method, I can connect directly to it.
	pAction = new QAction( "Clear", pSender );
	connect( pAction, SIGNAL( triggered() ), pSender, SLOT( clear() ) );
	pMenu->addAction( pAction );

	// Show the menu.
	QPoint q = pSender->mapToGlobal( point );
	pMenu->exec( q );

	// The menu's ownership is transferred to me, so I must destroy it.
	delete pMenu;
}

void CChamberView::SlotScrollStatusChanged()
{
    QAction* pSender = static_cast<QAction*>(QObject::sender());
    if( pSender->parent() == ui->teSysLog )
	{
		m_bAutoScrollSys = !m_bAutoScrollSys;
	}
    else if( pSender->parent() == ui->tePlcLog )
	{
		m_bAutoScrollChmb = !m_bAutoScrollChmb;
	}
	else
	{

	}
}

void CChamberView::AppendSystemLog(LOG_TYPE t, QString s)
{
	QPlainTextEdit* pEditor = ui->teSysLog;
	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = UTIL::DATE::yyMMDDHHmmss(true) + s;
	if( t == _NORMAL_LOG )
	{
		tf.setForeground( PASS_TEXT_QCOLOR );
		 pEditor->setCurrentCharFormat( tf );
	}
	else if( t == _ERROR_LOG )
	{
		tf.setForeground( FAIL_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAULT_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	pEditor->appendPlainText( sLog );

	if( m_bAutoScrollSys )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}
}

void CChamberView::AppendPlcLog(LOG_TYPE t, QString s)
{
    QPlainTextEdit* pEditor = ui->tePlcLog;
	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = UTIL::DATE::yyMMDDHHmmss(true) + s;
	if( t == _NORMAL_LOG )
	{
		tf.setForeground( PASS_TEXT_QCOLOR );
		 pEditor->setCurrentCharFormat( tf );
	}
	else if( t == _ERROR_LOG )
	{
		tf.setForeground( FAIL_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAULT_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	pEditor->appendPlainText( sLog );

    if( m_bAutoScrollChmb )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}
}

void CChamberView::SlotSvSet( EN_SV_MODE mode, double sv )
{
    short sSv = static_cast<short>( sv * 10 );
    int nSv = static_cast<int>(sSv&0xFFFF);
	if( mode == SV_TEMP )
		SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE    , nSv );
	else
		SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE, nSv );
}

//2023.09.19 - KWMOON
void CChamberView::SlotPasswdDlgAccepted( PASSWD_MODE mode)
{
	if( mode == PASSWD_DC_ON )
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, PROGRAM_NAME, tr("do you want to system dc power on?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_SYSTEM_DC_ONOFF, 0x1 );
		}
	}
	else if( mode == PASSWD_DC_OFF )
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, PROGRAM_NAME, tr("do you want to system dc power off?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_SYSTEM_DC_ONOFF, 0x0 );
		}
	}
	else
	{

	}
}

void CChamberView::SlotBtnRunClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to run the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnStopClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to stop the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnAmbModeClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to amb_mode the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnAmbAutoClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to amb_auto the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnAlarmResetClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to reset alarm the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET, 0x1 );
		//MakeWordAlarmList( PROTOCOL::PLC_COMMAND::CMD_RW_ALARM0, 0, 0xF );
	}
}

void CChamberView::SlotBtnMaskOnClicked()
{
    int onoff = 0x0;
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to remote control chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, onoff );
	}
}

//2023.08.29 - KWMOON(MASK ON/OFF - separation )
void CChamberView::SlotBtnMaskOffClicked()
{
    int onoff = 0x1;
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to local(touch) control chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, onoff );
	}
}

void CChamberView::SlotBtnWatchdogEnClicked()
{
	QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to enable the watchdog of the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB, WATCHDOG_ENABLE );
	}
}

void CChamberView::SlotBtnWatchdogDisClicked()
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to disable the watchdog of the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB, WATCHDOG_DISABLE );
    }
}

void CChamberView::SlotBtnDoorCloseClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to door close the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnDoorOpenClicked()
{
	QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to door open the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnRackInClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to rack in the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnRackOutClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to rack out the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER, 0x1 );
	}
}

void CChamberView::SlotBtnSetTempClicked()
{
	QPoint mPos( mapToGlobal( this->geometry().topLeft() ) );
	QPoint pos( mapToGlobal( ui->btnSetTemp->geometry().bottomLeft() ) );
	QPoint nnn( pos );
	nnn.setY( nnn.y() + ui->btnSetTemp->height() );

	m_pSv->setModal( true );
	m_pSv->setMode(SV_TEMP);
	m_pSv->move( nnn );
	m_pSv->show();
}

void CChamberView::SlotBtnSetAmbClicked()
{
	QPoint mPos( mapToGlobal( this->geometry().topLeft() ) );
	QPoint pos( mapToGlobal( ui->btnSetAmb->geometry().bottomLeft() ) );
	QPoint nnn( pos );
	nnn.setY( nnn.y() + ui->btnSetAmb->height() );

	//pos.setY( pos.y() + ui->btnTempSet->height() );
	//AppendSysLog(EN_NORMAL_LOG, tr("BTN  : X:%1, Y:%2, HEIGHT:%3").arg(pos.x()).arg(pos.y()).arg(ui->btnTempSet->height()) );
	//AppendSysLog(EN_NORMAL_LOG, tr("PV   : X:%1, Y:%2").arg(nnn.x()).arg(nnn.y()) );

	m_pSv->setModal( true );
	m_pSv->setMode(SV_AMB);
	m_pSv->move( nnn );
	m_pSv->show();
}

void CChamberView::SlotBtnRackOprTesterClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to remote rack operation the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER, RACK_NOP_TESTER );
	}
}

void CChamberView::SlotBtnRackOprChamberClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to local rack operation the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER, RACK_NOP_CHAMBER );
	}
}

//2023.04.13 - KWMOON
void CChamberView::SlotBtnBuzzerOffClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to buzz off the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_BUZZER_ONOFF, CHMB_BUZZER_OFF );
	}
}

//2023.04.13 - KWMOON
void CChamberView::SlotBtnBuzzerOnClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to buzz on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_BUZZER_ONOFF, CHMB_BUZZER_ON );
	}
}

void CChamberView::SlotLampAllOffClicked()			//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to all lamp off the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_ALL_LAMP_OFF, CHMB_TOWERLAMP_ON );
	}
}

void CChamberView::SlotLampLocalModeClicked()		//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to tower lamp chamber mode the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF, CHMB_MASK_OFF );
	}
}

void CChamberView::SlotLampRemoteModeClicked()		//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to tower lamp remote mode on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF, CHMB_MASK_ON );
	}
}

void CChamberView::SlotRedLampOnClicked()			//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to red lamp on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_ON, CHMB_TOWERLAMP_ON );
	}
}

void CChamberView::SlotRedLampOffClicked()			//2023.05.25 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to red lamp off the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_ON, CHMB_TOWERLAMP_OFF );
    }
}

void CChamberView::SlotYellowLampOnClicked()			//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to yellow lamp on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_ON, CHMB_TOWERLAMP_ON );
	}
}

void CChamberView::SlotYellowLampOffClicked()			//2023.05.25 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to yellow lamp off the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_ON, CHMB_TOWERLAMP_OFF );
    }
}

void CChamberView::SlotGreenLampOnClicked()			//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to green lamp on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_ON, CHMB_TOWERLAMP_ON );
	}
}

void CChamberView::SlotGreenLampOffClicked()			//2023.05.24 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to green lamp off the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_ON, CHMB_TOWERLAMP_OFF );
    }
}

void CChamberView::SlotRedLampBlinkClicked()			//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to red lamp blink on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_BLINK, CHMB_TOWERLAMP_BLINK );
	}
}

void CChamberView::SlotRedLampBlinkOffClicked()			//2023.05.24 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to red lamp blink on the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_BLINK, CHMB_TOWERLAMP_OFF );
    }
}

void CChamberView::SlotYellowLampBlinkClicked()		//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to yellow lamp blink on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_BLINK, CHMB_TOWERLAMP_BLINK );
	}
}

void CChamberView::SlotYellowLampBlinkOffClicked()		//2023.05.24 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to yellow lamp blink off the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_BLINK, CHMB_TOWERLAMP_OFF );
    }
}

void CChamberView::SlotGreenLampBlinkClicked()		//2023.05.24 - KWMOON
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to green lamp blink on the chamber?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_BLINK, CHMB_TOWERLAMP_BLINK );
	}
}

void CChamberView::SlotGreenLampBlinkOffClicked()		//2023.05.24 - KWMOON
{
    QMessageBox::StandardButton resBtn =
    QMessageBox::question( this, PROGRAM_NAME, tr("do you want to green lamp blink off the chamber?"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_BLINK, CHMB_TOWERLAMP_OFF );
    }
}

//2023.09.08 - KWMOON
void CChamberView::SlotbtnRackInFingerUpOnClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to enable Finger up when rack-in?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_FINGER_UP_STAT_RACKIN_ON, EN_CHMB_ON_TRIGGER );
	}
}

//2023.09.08 - KWMOON
void CChamberView::SlotbtnRackInFingerUpOffClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to disable Finger up when rack-in?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_FINGER_UP_STAT_RACKIN_OFF, EN_CHMB_ON_TRIGGER );
	}
}

//2023.09.08 - KWMOON
void CChamberView::SlotbtnDoorIgnoreRackInOutOnClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to enable Rack In-out ignore door close?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_IGNORE_RACK_INOUT_ON, EN_CHMB_ON_TRIGGER );
	}
}

//2023.09.08 - KWMOON
void CChamberView::SlotbtnDoorIgnoreRackInOutOffClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, PROGRAM_NAME, tr("do you want to disable Rack In-out ignore door close?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		SetChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF, EN_CHMB_ON_TRIGGER );
	}
}

//2023.09.19 - KWMOON
void CChamberView::SlotBtnDcPowerOnClicked()
{
	//QPoint mPos( mapToGlobal( this->geometry().topLeft() ) );
	//QPoint pos( mapToGlobal( ui->btnSetTemp->geometry().bottomLeft() ) );
	//QPoint nnn( pos );
	//nnn.setY( nnn.y() + ui->btnSetTemp->height() );

	m_pPassDlg->setModal( true );
	m_pPassDlg->SetMode(PASSWD_DC_ON);
	//m_pSv->move( nnn );
	m_pPassDlg->show();
}

//2023.09.19 - KWMOON
void CChamberView::SlotBtnDcPowerOffClicked()
{
	m_pPassDlg->setModal( true );
	m_pPassDlg->SetMode(PASSWD_DC_OFF);
	//m_pSv->move( nnn );
	m_pPassDlg->show();
}

bool CChamberView::SetChmbWatchdogPeriod()
{
    SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TIME_SET, m_nWatchdogPeriod );
    return true;
}

bool CChamberView::SetChmbWatchdogOnOff(bool onoff)
{
    int nOnOff = (onoff)?0x1:0x0;
    SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB, nOnOff );
    return true;
}

bool CChamberView::SetChmbWatchdog()
{
    static int nToggle = 0x0;
    SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER, nToggle&0x1 );
    nToggle = (~nToggle&0x1);
    return true;
}

bool CChamberView::SetChmbAmbSv()
{
    short sSv = static_cast<short>( m_dAmbSv * 10 );
    int nSv = static_cast<int>(sSv&0xFFFF);
    SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE, nSv );
    return true;
}

bool CChamberView::SetChmbAlarmReset()
{
    SetChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET, 0x1 );
    return true;
}

//2024.08.19 - KWMOON
void CChamberView::InitLogger()
{
	m_pLoggerCompressure = new CLogger( PROTOCOL::_ATCHMB_UI_COMPRESS );
	m_pLoggerAlarm       = new CLogger( PROTOCOL::_ATCHMB_UI_ALARM    );
	m_pLoggerPcw         = new CLogger( PROTOCOL::_ATCHMB_UI_PCW      );

	connect( this, &CChamberView::sigCompressureLog , m_pLoggerCompressure, &CLogger::SlotLogMessage );
	connect( this, &CChamberView::sigAlarmLog , m_pLoggerAlarm, &CLogger::SlotLogMessage );
	connect( this, &CChamberView::sigPcwLog , m_pLoggerPcw, &CLogger::SlotLogMessage );
}

//2024.08.19 - KWMOON
void CChamberView::SlotTimeoutChmbCompressure()
{
	if( !m_bConnIpcPlc[CONN_IPC] ) return;
	if( !m_bConnIpcPlc[CONN_PLC] ) return;

	bool bSent = true;
	//COMPRESSURE1/2
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_PRESSURE_HIGH_VALUE );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_PRESSURE_LOW_VALUE  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_PRESSURE_HIGH_VALUE );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_PRESSURE_LOW_VALUE  );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_OUT_TEMP_VALUE      );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP1_IN_TEMP_VALUE       );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_OUT_TEMP_VALUE      );
	bSent &= GetChmbStatus( PROTOCOL::PLC_COMMAND::CMD_RW_COMP2_IN_TEMP_VALUE       );

	if( !bSent )
		AppendSystemLog( _ERROR_LOG , tr("[SEND] COMPRESSURE READ NG") );

}
