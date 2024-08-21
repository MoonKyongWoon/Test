#include "syspwr.hpp"
#include "ui_syspwr.h"

CSysPwr::CSysPwr(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSysPwr)
{
	ui->setupUi(this);
	InitVariables();
	MakePMBusCommandMap();
	InitUi();
	LoadIni();
	InitSigSlot();
    AppendLog( SIDE_ALL, _NORMAL_LOG, "System Power Monitoring Start!" );

	CreateThread();
	InitTimer();	
}

CSysPwr::~CSysPwr()
{
    if( m_tmr485con->isActive() ) m_tmr485con->stop();

	if( m_pThread[SIDE_A] )
	{
		m_pThread[SIDE_A]->quit();
		m_pThread[SIDE_A]->wait();
	}
	if( m_pThread[SIDE_B] )
	{
		m_pThread[SIDE_B]->quit();
		m_pThread[SIDE_B]->wait();
	}
	delete ui;
}

void CSysPwr::InitVariables()
{
	memset( m_LineMeas, 0x0, sizeof(m_LineMeas) );
	memset( m_ModlMeas, 0x0, sizeof(m_ModlMeas) );

	//LINE_A, LINE_B - V/A
	m_pLcdLine[SIDE_A][LINE1][LINE_V] = ui->lcdALine1V;
	m_pLcdLine[SIDE_A][LINE1][LINE_A] = ui->lcdALine1A;
	m_pLcdLine[SIDE_A][LINE2][LINE_V] = ui->lcdALine2V;
	m_pLcdLine[SIDE_A][LINE2][LINE_A] = ui->lcdALine2A;
	m_pLcdLine[SIDE_A][LINE3][LINE_V] = ui->lcdALine3V;
	m_pLcdLine[SIDE_A][LINE3][LINE_A] = ui->lcdALine3A;
	m_pLcdLine[SIDE_B][LINE1][LINE_V] = ui->lcdBLine1V;
	m_pLcdLine[SIDE_B][LINE1][LINE_A] = ui->lcdBLine1A;
	m_pLcdLine[SIDE_B][LINE2][LINE_V] = ui->lcdBLine2V;
	m_pLcdLine[SIDE_B][LINE2][LINE_A] = ui->lcdBLine2A;
	m_pLcdLine[SIDE_B][LINE3][LINE_V] = ui->lcdBLine3V;
	m_pLcdLine[SIDE_B][LINE3][LINE_A] = ui->lcdBLine3A;

	//SIDE_AB, MODULE1_8
	m_pLcdModl[SIDE_A][MODULE1][MODULE_VOUT] = ui->lcdAModule1Vout;
	m_pLcdModl[SIDE_A][MODULE1][MODULE_IOUT] = ui->lcdAModule1Iout;
	m_pLcdModl[SIDE_A][MODULE1][MODULE_POUT] = ui->lcdAModule1Pout;
	m_pLcdModl[SIDE_A][MODULE1][MODULE_TEMP] = ui->lcdAModule1Temp;
	m_pLcdModl[SIDE_A][MODULE2][MODULE_VOUT] = ui->lcdAModule2Vout;
	m_pLcdModl[SIDE_A][MODULE2][MODULE_IOUT] = ui->lcdAModule2Iout;
	m_pLcdModl[SIDE_A][MODULE2][MODULE_POUT] = ui->lcdAModule2Pout;
	m_pLcdModl[SIDE_A][MODULE2][MODULE_TEMP] = ui->lcdAModule2Temp;
	m_pLcdModl[SIDE_A][MODULE3][MODULE_VOUT] = ui->lcdAModule3Vout;
	m_pLcdModl[SIDE_A][MODULE3][MODULE_IOUT] = ui->lcdAModule3Iout;
	m_pLcdModl[SIDE_A][MODULE3][MODULE_POUT] = ui->lcdAModule3Pout;
	m_pLcdModl[SIDE_A][MODULE3][MODULE_TEMP] = ui->lcdAModule3Temp;
	m_pLcdModl[SIDE_A][MODULE4][MODULE_VOUT] = ui->lcdAModule4Vout;
	m_pLcdModl[SIDE_A][MODULE4][MODULE_IOUT] = ui->lcdAModule4Iout;
	m_pLcdModl[SIDE_A][MODULE4][MODULE_POUT] = ui->lcdAModule4Pout;
	m_pLcdModl[SIDE_A][MODULE4][MODULE_TEMP] = ui->lcdAModule4Temp;
	m_pLcdModl[SIDE_A][MODULE5][MODULE_VOUT] = ui->lcdAModule5Vout;
	m_pLcdModl[SIDE_A][MODULE5][MODULE_IOUT] = ui->lcdAModule5Iout;
	m_pLcdModl[SIDE_A][MODULE5][MODULE_POUT] = ui->lcdAModule5Pout;
	m_pLcdModl[SIDE_A][MODULE5][MODULE_TEMP] = ui->lcdAModule5Temp;
	m_pLcdModl[SIDE_A][MODULE6][MODULE_VOUT] = ui->lcdAModule6Vout;
	m_pLcdModl[SIDE_A][MODULE6][MODULE_IOUT] = ui->lcdAModule6Iout;
	m_pLcdModl[SIDE_A][MODULE6][MODULE_POUT] = ui->lcdAModule6Pout;
	m_pLcdModl[SIDE_A][MODULE6][MODULE_TEMP] = ui->lcdAModule6Temp;
	m_pLcdModl[SIDE_A][MODULE7][MODULE_VOUT] = ui->lcdAModule7Vout;
	m_pLcdModl[SIDE_A][MODULE7][MODULE_IOUT] = ui->lcdAModule7Iout;
	m_pLcdModl[SIDE_A][MODULE7][MODULE_POUT] = ui->lcdAModule7Pout;
	m_pLcdModl[SIDE_A][MODULE7][MODULE_TEMP] = ui->lcdAModule7Temp;
	m_pLcdModl[SIDE_A][MODULE8][MODULE_VOUT] = ui->lcdAModule8Vout;
	m_pLcdModl[SIDE_A][MODULE8][MODULE_IOUT] = ui->lcdAModule8Iout;
	m_pLcdModl[SIDE_A][MODULE8][MODULE_POUT] = ui->lcdAModule8Pout;
	m_pLcdModl[SIDE_A][MODULE8][MODULE_TEMP] = ui->lcdAModule8Temp;

	m_pLcdModl[SIDE_B][MODULE1][MODULE_VOUT] = ui->lcdBModule1Vout;
	m_pLcdModl[SIDE_B][MODULE1][MODULE_IOUT] = ui->lcdBModule1Iout;
	m_pLcdModl[SIDE_B][MODULE1][MODULE_POUT] = ui->lcdBModule1Pout;
	m_pLcdModl[SIDE_B][MODULE1][MODULE_TEMP] = ui->lcdBModule1Temp;
	m_pLcdModl[SIDE_B][MODULE2][MODULE_VOUT] = ui->lcdBModule2Vout;
	m_pLcdModl[SIDE_B][MODULE2][MODULE_IOUT] = ui->lcdBModule2Iout;
	m_pLcdModl[SIDE_B][MODULE2][MODULE_POUT] = ui->lcdBModule2Pout;
	m_pLcdModl[SIDE_B][MODULE2][MODULE_TEMP] = ui->lcdBModule2Temp;
	m_pLcdModl[SIDE_B][MODULE3][MODULE_VOUT] = ui->lcdBModule3Vout;
	m_pLcdModl[SIDE_B][MODULE3][MODULE_IOUT] = ui->lcdBModule3Iout;
	m_pLcdModl[SIDE_B][MODULE3][MODULE_POUT] = ui->lcdBModule3Pout;
	m_pLcdModl[SIDE_B][MODULE3][MODULE_TEMP] = ui->lcdBModule3Temp;
	m_pLcdModl[SIDE_B][MODULE4][MODULE_VOUT] = ui->lcdBModule4Vout;
	m_pLcdModl[SIDE_B][MODULE4][MODULE_IOUT] = ui->lcdBModule4Iout;
	m_pLcdModl[SIDE_B][MODULE4][MODULE_POUT] = ui->lcdBModule4Pout;
	m_pLcdModl[SIDE_B][MODULE4][MODULE_TEMP] = ui->lcdBModule4Temp;
	m_pLcdModl[SIDE_B][MODULE5][MODULE_VOUT] = ui->lcdBModule5Vout;
	m_pLcdModl[SIDE_B][MODULE5][MODULE_IOUT] = ui->lcdBModule5Iout;
	m_pLcdModl[SIDE_B][MODULE5][MODULE_POUT] = ui->lcdBModule5Pout;
	m_pLcdModl[SIDE_B][MODULE5][MODULE_TEMP] = ui->lcdBModule5Temp;
	m_pLcdModl[SIDE_B][MODULE6][MODULE_VOUT] = ui->lcdBModule6Vout;
	m_pLcdModl[SIDE_B][MODULE6][MODULE_IOUT] = ui->lcdBModule6Iout;
	m_pLcdModl[SIDE_B][MODULE6][MODULE_POUT] = ui->lcdBModule6Pout;
	m_pLcdModl[SIDE_B][MODULE6][MODULE_TEMP] = ui->lcdBModule6Temp;
	m_pLcdModl[SIDE_B][MODULE7][MODULE_VOUT] = ui->lcdBModule7Vout;
	m_pLcdModl[SIDE_B][MODULE7][MODULE_IOUT] = ui->lcdBModule7Iout;
	m_pLcdModl[SIDE_B][MODULE7][MODULE_POUT] = ui->lcdBModule7Pout;
	m_pLcdModl[SIDE_B][MODULE7][MODULE_TEMP] = ui->lcdBModule7Temp;
	m_pLcdModl[SIDE_B][MODULE8][MODULE_VOUT] = ui->lcdBModule8Vout;
	m_pLcdModl[SIDE_B][MODULE8][MODULE_IOUT] = ui->lcdBModule8Iout;
	m_pLcdModl[SIDE_B][MODULE8][MODULE_POUT] = ui->lcdBModule8Pout;
	m_pLcdModl[SIDE_B][MODULE8][MODULE_TEMP] = ui->lcdBModule8Temp;

	m_pStatModl[SIDE_A][MODULE1] = ui->lblAModule1Sta;
	m_pStatModl[SIDE_A][MODULE2] = ui->lblAModule2Sta;
	m_pStatModl[SIDE_A][MODULE3] = ui->lblAModule3Sta;
	m_pStatModl[SIDE_A][MODULE4] = ui->lblAModule4Sta;
	m_pStatModl[SIDE_A][MODULE5] = ui->lblAModule5Sta;
	m_pStatModl[SIDE_A][MODULE6] = ui->lblAModule6Sta;
    m_pStatModl[SIDE_A][MODULE7] = ui->lblAModule7Sta;
	m_pStatModl[SIDE_A][MODULE8] = ui->lblAModule8Sta;
	m_pStatModl[SIDE_B][MODULE1] = ui->lblBModule1Sta;
	m_pStatModl[SIDE_B][MODULE2] = ui->lblBModule2Sta;
	m_pStatModl[SIDE_B][MODULE3] = ui->lblBModule3Sta;
	m_pStatModl[SIDE_B][MODULE4] = ui->lblBModule4Sta;
	m_pStatModl[SIDE_B][MODULE5] = ui->lblBModule5Sta;
	m_pStatModl[SIDE_B][MODULE6] = ui->lblBModule6Sta;
	m_pStatModl[SIDE_B][MODULE7] = ui->lblBModule7Sta;
	m_pStatModl[SIDE_B][MODULE8] = ui->lblBModule8Sta;

    m_bOpen[SIDE_A] = false;
    m_bOpen[SIDE_B] = false;
}

void CSysPwr::LoadIni()
{
	//m_sComPort[SIDE_A] = "/dev/ttyS3";
	m_stty[SIDE_A] = "/dev/ttyS2";
	m_stty[SIDE_B] = "/dev/ttyS3";
}

void CSysPwr::MakePMBusCommandMap()
{
	m_mapPbCmdIsoComm.clear();
	m_mapPbCmdModule.clear();
	m_mapPbCmdIsoComm["W_OPERATION"        ] = stPmubusCommand{ CMD_OPERATION        , "W_OPERATION"     , PMBUS_WRIT, FORMAT_BITMAPPED,   1, 0    , 0 };
    m_mapPbCmdIsoComm["W_CLEAR_FAULTS"     ] = stPmubusCommand{ CMD_CLEAR_FAULTS     , "W_CLEAR_FAULTS"  , PMBUS_WRIT, FORMAT_NA       ,   0, 0    , 0 };
	m_mapPbCmdIsoComm["W_WRITE_PROTECT"    ] = stPmubusCommand{ CMD_WRITE_PROTECT    , "W_WRITE_PROTECT" , PMBUS_WRIT, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdIsoComm["R_OPERATION"        ] = stPmubusCommand{ CMD_OPERATION        , "R_OPERATION"     , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdIsoComm["R_WRITE_PROTECT"    ] = stPmubusCommand{ CMD_WRITE_PROTECT    , "R_WRITE_PROTECT" , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdIsoComm["R_STATUS_BYTE"      ] = stPmubusCommand{ CMD_STATUS_BYTE      , "R_STATUS_BYTE"   , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdIsoComm["R_STATUS_WORD"      ] = stPmubusCommand{ CMD_STATUS_WORD      , "R_STATUS_WORD"   , PMBUS_READ, FORMAT_BITMAPPED,   2, 0    , 0 };
    m_mapPbCmdIsoComm["R_STATUS_CML"       ] = stPmubusCommand{ CMD_STATUS_CML       , "R_STATUS_CML"    , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };   //2023.05.19 - kwmoon
    m_mapPbCmdIsoComm["R_FAN1_SPEED"       ] = stPmubusCommand{ CMD_READ_FAN1_SPEED  , "R_FAN1_SPEED"    , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdIsoComm["R_FAN2_SPEED"       ] = stPmubusCommand{ CMD_READ_FAN2_SPEED  , "R_FAN2_SPEED"    , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdIsoComm["R_FAN3_SPEED"       ] = stPmubusCommand{ CMD_READ_FAN3_SPEED  , "R_FAN3_SPEED"    , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdIsoComm["R_FAN4_SPEED"       ] = stPmubusCommand{ CMD_READ_FAN4_SPEED  , "R_FAN4_SPEED"    , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdIsoComm["R_MFR_ID"           ] = stPmubusCommand{ CMD_MFR_ID           , "R_MFR_ID"        , PMBUS_READ, FORMAT_ASCII    ,   7, 0    , 0 };
	m_mapPbCmdIsoComm["R_MFR_MODEL"        ] = stPmubusCommand{ CMD_MFR_MODEL        , "R_MFR_MODEL"     , PMBUS_READ, FORMAT_ASCII    ,  12, 0    , 0 };
	m_mapPbCmdIsoComm["R_READ_VIN1"        ] = stPmubusCommand{ CMD_READ_VIN1        , "R_READ_VIN1"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 10   , 0 };
	m_mapPbCmdIsoComm["R_READ_VIN2"        ] = stPmubusCommand{ CMD_READ_VIN2        , "R_READ_VIN2"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 10   , 0 };
	m_mapPbCmdIsoComm["R_READ_VIN3"        ] = stPmubusCommand{ CMD_READ_VIN3        , "R_READ_VIN3"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 10   , 0 };
	m_mapPbCmdIsoComm["R_READ_IIN1"        ] = stPmubusCommand{ CMD_READ_IIN1        , "R_READ_IIN1"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 100  , 0 };
	m_mapPbCmdIsoComm["R_READ_IIN2"        ] = stPmubusCommand{ CMD_READ_IIN2        , "R_READ_IIN2"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 100  , 0 };
	m_mapPbCmdIsoComm["R_READ_IIN3"        ] = stPmubusCommand{ CMD_READ_IIN3        , "R_READ_IIN3"     , PMBUS_READ, FORMAT_DIRECT   ,   2, 100  , 0 };

	m_mapPbCmdModule["W_OPERATION"         ] = stPmubusCommand{ CMD_OPERATION        , "W_OPERATION"      , PMBUS_WRIT, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["W_VREF"              ] = stPmubusCommand{ CMD_VREF             , "W_VREF"           , PMBUS_WRIT, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["W_PROTECTION"        ] = stPmubusCommand{ CMD_WRITE_PROTECT    , "W_PROTECTION"     , PMBUS_WRIT, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["R_OPERATION"         ] = stPmubusCommand{ CMD_OPERATION        , "R_OPERATION"      , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["R_VOUT_MAX"          ] = stPmubusCommand{ CMD_VOUT_MAX         , "R_VOUT_MAX"       , PMBUS_READ, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["R_POUT_MAX"          ] = stPmubusCommand{ CMD_POUT_MAX         , "R_POUT_MAX"       , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdModule["R_STATUS_BYTE"       ] = stPmubusCommand{ CMD_STATUS_BYTE      , "R_STATUS_BYTE"    , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["R_STATUS_WORD"       ] = stPmubusCommand{ CMD_STATUS_WORD      , "R_STATUS_WORD"    , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["R_STATUS_IOUT"       ] = stPmubusCommand{ CMD_STATUS_IOUT      , "R_STATUS_IOUT"    , PMBUS_READ, FORMAT_BITMAPPED,   1, 0    , 0 };
	m_mapPbCmdModule["R_READ_VOUT"         ] = stPmubusCommand{ CMD_READ_VOUT        , "R_READ_VOUT"      , PMBUS_READ, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["R_READ_IOUT"         ] = stPmubusCommand{ CMD_READ_IOUT        , "R_READ_IOUT"      , PMBUS_READ, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["R_TEMPERATUR_1"      ] = stPmubusCommand{ CMD_READ_TEMPERATUR_1, "R_TEMPERATUR_1"   , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
    m_mapPbCmdModule["R_TEMPERATUR_2"      ] = stPmubusCommand{ CMD_READ_TEMPERATUR_2, "R_TEMPERATUR_2"   , PMBUS_READ, FORMAT_LINEAR   ,   2, 0    , 0 };
	m_mapPbCmdModule["R_MFR_VOUT_MAX"      ] = stPmubusCommand{ CMD_MFR_VOUT_MAX     , "R_MFR_VOUT_MAX"   , PMBUS_READ, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["R_MFR_IOUT_MAX"      ] = stPmubusCommand{ CMD_MFR_IOUT_MAX     , "R_MFR_IOUT_MAX"   , PMBUS_READ, FORMAT_DIRECT   ,   3, 10000, 1 };
	m_mapPbCmdModule["R_MFR_POUT_MAX"      ] = stPmubusCommand{ CMD_MFR_POUT_MAX     , "R_MFR_POUT_MAX"   , PMBUS_READ, FORMAT_LINEAR   ,   3, 10000, 1 };
}

void CSysPwr::InitTimer()
{
    m_tmr485con = new QTimer(this);
    connect(m_tmr485con  , &QTimer::timeout, this, &CSysPwr::SlotTimeout485Con   );
	m_tmr485con->start( 10000 );	//2024.08.20 - KWMOON( 5000 -> 10000 )
}

void CSysPwr::SlotTimeout485Con()
{
    if( !m_bOpen[SIDE_A] || !m_bOpen[SIDE_B] )
    {
        emit SigOpenDevice();
    }
    else
    {
        AppendLog( SIDE_ALL, _NORMAL_LOG, tr("Line Monitoring Request") );
        if( m_bOpen[SIDE_A] ) emit SigStartMon(SIDE_A);
        if( m_bOpen[SIDE_B] ) emit SigStartMon(SIDE_B);
    }
}

void CSysPwr::SlotOpenDeviceResult(SIDE_AB ab, bool bopen )
{
    m_bOpen[ab] = bopen;
}


void CSysPwr::InitUi()
{
	InitUiComm();
	InitUiLineVA();
	InitUiModules();
	InitUiModuleStat();
	InitUiLog();
}

void CSysPwr::InitUiComm()
{
    //ui->leAComport->setText("");
}

void CSysPwr::InitUiLineVA()
{
	for( int ab=0;ab<MAX_SIDE;ab++ )
	{
		for( int line=0;line<MAX_LINE;line++ )
		{
			for( int i=0;i<MAX_LINE_ITEM;i++ )
			{
				m_pLcdLine[ab][line][i]->display(0.0);
			}
		}
	}
}

void CSysPwr::InitUiModules()
{
	for( int ab=0;ab<MAX_SIDE;ab++ )
	{
		for( int mdl=0;mdl<MAX_MODULE;mdl++ )
		{
			for( int i=0;i<MAX_MODULE_ITEM-1;i++ )
			{
				m_pLcdModl[ab][mdl][i]->display(0.0);
			}
		}
	}
}

void CSysPwr::InitUiModuleStat()
{
	for( int ab=0;ab<MAX_SIDE;ab++ )
	{
		for( int mdl=0;mdl<MAX_MODULE;mdl++ )
		{
			m_pStatModl[ab][mdl]->setScaledContents(true);
			m_pStatModl[ab][mdl]->setPixmap( PIXMAP_PASS );
		}
	}
}

void CSysPwr::InitUiLog()
{
	ui->teLog->setReadOnly(true);
	ui->teLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
	ui->teLog->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->teLog->setMaximumBlockCount( 32768 );
}

void CSysPwr::SlotCustomContextMenuRequested( QPoint p )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;

	pAction = new QAction( tr("Auto Scroll On"), this );
	pAction->setCheckable( true );
	if( m_bAutoScroll ) pAction->setChecked(true);
	else                pAction->setChecked(false);
	connect( pAction, &QAction::triggered, this, &CSysPwr::SlotScrollStatusChanged );
	pMenu->addAction( pAction );

	// Clear.
	// Because QPlainTextEdit::clear() is a slot method, I can connect directly to it.
	pAction = new QAction( "Clear", this );
	connect( pAction, SIGNAL( triggered() ), pSender, SLOT( clear() ) );
	pMenu->addAction( pAction );

	// Show the menu.
	QPoint q = pSender->mapToGlobal( p );
	pMenu->exec( q );

	// The menu's ownership is transferred to me, so I must destroy it.
	delete pMenu;
}

void CSysPwr::SlotScrollStatusChanged()
{
	if( m_bAutoScroll ) m_bAutoScroll = false;
	else                m_bAutoScroll = true;
}

void CSysPwr::CreateThread()
{
    m_pPwrTh[SIDE_A]  = new CSysPwrTh(SIDE_A, m_mapPbCmdIsoComm, m_mapPbCmdModule);
    m_pPwrTh[SIDE_B]  = new CSysPwrTh(SIDE_B, m_mapPbCmdIsoComm, m_mapPbCmdModule);
	m_pThread[SIDE_A] = new QThread();
	m_pThread[SIDE_B] = new QThread();

	m_pPwrTh[SIDE_A]->moveToThread( m_pThread[SIDE_A] );
	m_pPwrTh[SIDE_B]->moveToThread( m_pThread[SIDE_B] );

    connect( m_pThread[SIDE_A], &QThread::finished              , m_pPwrTh[SIDE_A], &QObject::deleteLater          );
    connect( m_pThread[SIDE_B], &QThread::finished              , m_pPwrTh[SIDE_B], &QObject::deleteLater          );
    connect( m_pPwrTh[SIDE_A] , &CSysPwrTh::SigLogMessage       , this            , &CSysPwr::SlotLogMessage       );
    connect( m_pPwrTh[SIDE_B] , &CSysPwrTh::SigLogMessage       , this            , &CSysPwr::SlotLogMessage       );
    connect( m_pPwrTh[SIDE_A] , &CSysPwrTh::SigIsoComm          , this            , &CSysPwr::SlotLineMeasData     );
    connect( m_pPwrTh[SIDE_A] , &CSysPwrTh::SigModules          , this            , &CSysPwr::SlotModuleMeasData   );
    connect( m_pPwrTh[SIDE_B] , &CSysPwrTh::SigIsoComm          , this            , &CSysPwr::SlotLineMeasData     );
    connect( m_pPwrTh[SIDE_B] , &CSysPwrTh::SigModules          , this            , &CSysPwr::SlotModuleMeasData   );
    connect( m_pPwrTh[SIDE_A] , &CSysPwrTh::SigOpenDeviceResult , this            , &CSysPwr::SlotOpenDeviceResult );
    connect( m_pPwrTh[SIDE_B] , &CSysPwrTh::SigOpenDeviceResult , this            , &CSysPwr::SlotOpenDeviceResult );

    connect( this             , &CSysPwr::SigManualTest         , m_pPwrTh[SIDE_A], &CSysPwrTh::SlotManualTest   );
    connect( this             , &CSysPwr::SigManualTest         , m_pPwrTh[SIDE_B], &CSysPwrTh::SlotManualTest   );
    connect( this             , &CSysPwr::SigOpenDevice         , m_pPwrTh[SIDE_A], &CSysPwrTh::SlotOpenDevice   );
    connect( this             , &CSysPwr::SigOpenDevice         , m_pPwrTh[SIDE_B], &CSysPwrTh::SlotOpenDevice   );
    connect( this             , &CSysPwr::SigTextTest           , m_pPwrTh[SIDE_A], &CSysPwrTh::SlotTextTest     );
    connect( this             , &CSysPwr::SigTextTest           , m_pPwrTh[SIDE_B], &CSysPwrTh::SlotTextTest     );
    connect( this             , &CSysPwr::SigStartMon           , m_pPwrTh[SIDE_A], &CSysPwrTh::SlotMonitoring   );
    connect( this             , &CSysPwr::SigStartMon           , m_pPwrTh[SIDE_B], &CSysPwrTh::SlotMonitoring   );

	m_pPwrTh[SIDE_A]->SetSerialInfo( m_stty[SIDE_A] );
	m_pPwrTh[SIDE_B]->SetSerialInfo( m_stty[SIDE_B] );

    m_pThread[SIDE_A]->start();
    m_pThread[SIDE_B]->start();
}

void CSysPwr::InitSigSlot()
{
	connect( ui->cboAddress   , QOverload<int>::of(&QComboBox::currentIndexChanged), this  , &CSysPwr::SlotCboAddressChanged    );
	connect( ui->cboCommand   , QOverload<int>::of(&QComboBox::currentIndexChanged), this  , &CSysPwr::SlotCboCommandChanged    );
	connect( ui->btnManualTest, &QPushButton::clicked                              , this  , &CSysPwr::SlotBtnManualTestClicked );
	connect( ui->teLog, &QPlainTextEdit::customContextMenuRequested, this, &CSysPwr::SlotCustomContextMenuRequested );
	ui->cboAddress->setCurrentIndex(1);
	ui->cboAddress->setCurrentIndex(0);
}

void CSysPwr::SlotCboAddressChanged(int idx)
{
	if( idx == 0 ) //ISOCOMM
	{
		ui->cboModule->setEnabled( false );
		ui->cboCommand->clear();
		for(auto iter = m_mapPbCmdIsoComm.begin(); iter != m_mapPbCmdIsoComm.end(); ++iter)
		{
			ui->cboCommand->addItem( iter->second.sCommand.c_str() );
		}
		ui->cboCommand->setCurrentIndex(0);
        ui->cboCommand->setEnabled( true );
	}
	else if( idx == 1 ) //MODULE
	{
		ui->cboModule->setEnabled( true );
		ui->cboCommand->clear();
		for(auto iter = m_mapPbCmdModule.begin(); iter != m_mapPbCmdModule.end(); ++iter)
		{
			ui->cboCommand->addItem( iter->second.sCommand.c_str() );
		}
		ui->cboCommand->setCurrentIndex(0);
        ui->cboCommand->setEnabled( true );
	}
	else	//no use
	{
		ui->cboModule->setEnabled( false );
		ui->cboCommand->setEnabled( false );
		ui->leManualData->setFocus();
	}
}

void CSysPwr::SlotCboCommandChanged(int idx)
{
    (void)idx;
}

void CSysPwr::SlotBtnManualTestClicked()
{
	stUiRequest request;
	int nSide = ui->cboSide->currentIndex();

	int nCboAddress = ui->cboAddress->currentIndex();
	int nCboModule  = ui->cboModule->currentIndex();
    QString sCmd = ui->cboCommand->currentText().trimmed();
    QString sText = ui->leManualData->text().trimmed();
	std::map<QString, stPmubusCommand>* pMap = nullptr;

	if( nSide == 0 ) request.uSide = SIDE_A;
	else	         request.uSide = SIDE_B;

	if( nCboAddress == 0 )
	{
		request.uDevice = DEVICE_ADDRESS::DEV_ISOCOMM;
		pMap = &m_mapPbCmdIsoComm;
	}
	else if( nCboAddress == 1 )
	{
		request.uDevice = DEVICE_ADDRESS::DEV_MODULE1 + nCboModule;
		pMap = &m_mapPbCmdModule;
	}
	else
	{
		SIDE_AB side = SIDE_A;
		if( nSide == 1 )  side = SIDE_B;
        emit SigTextTest(side, sText);
        AppendLog( SIDE_ALL, _NORMAL_LOG, tr("%1 debug test!").arg(sText) );
		return;
	}

	std::map<QString, stPmubusCommand>::iterator it = pMap->find( sCmd );
	if( it == pMap->end() )
	{
		AppendLog( SIDE_ALL, _ERROR_LOG, tr("%1 is not found command map!").arg(sCmd) );
		return;
	}
	request.stPmbusCmd = it->second;
	if( request.stPmbusCmd.enCmdType == PMBUS_OPENRATION::PMBUS_WRIT )
	{
		QString sData = ui->leManualData->text().trimmed();
		if( sData.isEmpty() )
		{
            if( request.stPmbusCmd.uNumOfByte > 0 )
            {
                AppendLog( SIDE_ALL, _ERROR_LOG, tr("%1 is write command, but data is empty!").arg(sCmd) );
                ui->leManualData->setFocus();
                return;
            }
		}

		bool bOk = false;
		if (request.stPmbusCmd.enDataFormat == PMBUS_DATA_FORMAT::FORMAT_BITMAPPED )
		{
			if( sData.left(2).toUpper() == "0X" )
				request.uData = sData.toInt( &bOk, 16 );
			else
				request.uData = sData.toInt( &bOk, 10 );
		}
		else if (request.stPmbusCmd.enDataFormat == PMBUS_DATA_FORMAT::FORMAT_DIRECT )
		{
			if( sData.left(2).toUpper() == "0X" )
			{
				AppendLog( SIDE_ALL, _ERROR_LOG, tr("%1 is direct command, need float type data").arg(sCmd) );
				ui->leManualData->setText("");
				ui->leManualData->setFocus();
				return;
			}
			float fData = sData.toFloat(&bOk);
			fData = fData * request.stPmbusCmd.uMultiplier;
			request.uData = static_cast<int>(fData);
		}
        else if (request.stPmbusCmd.enDataFormat == PMBUS_DATA_FORMAT::FORMAT_LINEAR )
		{
			if( sData.left(2).toUpper() == "0X" )
				request.uData = sData.toInt( &bOk, 16 );
			else
				request.uData = sData.toInt( &bOk, 10 );
		}
        else if (request.stPmbusCmd.enDataFormat == PMBUS_DATA_FORMAT::FORMAT_NA )
        {
            request.uData = 0x0;
        }
		else
		{
			AppendLog( SIDE_ALL, _ERROR_LOG, tr("%1 is Unsupport Command Data Format!").arg(sCmd) );
			return;
		}
	}
	else
	{
		request.uData = 0x0;
	}
	emit SigManualTest(request);
}

void CSysPwr::SlotLineMeasData( SIDE_AB ch, stLineMeas meas)
{
    AppendLog( ch, _NORMAL_LOG, tr("Line Monitoring Response") );
    char szFmt[64]={0x0,};
    if( ch == SIDE_A )
    {
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[0] );
        ui->lcdALine1V->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[1] );
        ui->lcdALine2V->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[2] );
        ui->lcdALine3V->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[0] );
        ui->lcdALine1A->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[1] );
        ui->lcdALine2A->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[2] );
        ui->lcdALine3A->display( tr("%1").arg(szFmt) );
    }
    else
    {
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[0] );
        ui->lcdBLine1V->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[1] );
        ui->lcdBLine2V->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[2] );
        ui->lcdBLine3V->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[0] );
        ui->lcdBLine1A->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[1] );
        ui->lcdBLine2A->display( tr("%1").arg(szFmt) );
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[2] );
        ui->lcdBLine3A->display( tr("%1").arg(szFmt) );
    }
}

void CSysPwr::SlotModuleMeasData( SIDE_AB ch, stModulelMeas meas )
{
    AppendLog( ch, _NORMAL_LOG, tr("Module Monitoring Response") );
    char szFmt[64]={0x0,};

    for( int module=0;module<MAX_MODULE;module++ )
    {
        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Voltage[module] );
        m_pLcdModl[ch][module][MODULE_VOUT]->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Current[module] );
        m_pLcdModl[ch][module][MODULE_IOUT]->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Watt[module] );
        m_pLcdModl[ch][module][MODULE_POUT]->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Temp[module] );
        m_pLcdModl[ch][module][MODULE_TEMP]->display( tr("%1").arg(szFmt) );

        snprintf( szFmt, sizeof(szFmt), "%6.2f", meas.Temp[module] );
        m_pLcdModl[ch][module][MODULE_TEMP]->display( tr("%1").arg(szFmt) );

        if( meas.Status[module] == 0x0 )
            m_pStatModl[ch][module]->setPixmap( PIXMAP_PASS );
        else
            m_pStatModl[ch][module]->setPixmap( PIXMAP_FAIL );
    }
}

void CSysPwr::SlotLogMessage( SIDE_AB ch, LOG_TYPE t, QString msg )
{
	AppendLog( ch, t, msg );
}

void CSysPwr::AppendLog( SIDE_AB ch, LOG_TYPE t, QString msg )
{
	if( ui->teLog->document()->blockCount()> MAX_LOG_LINE_NUM )
		ui->teLog->clear();

	QTextCharFormat tf;
	tf = ui->teLog->currentCharFormat();
	tf.setForeground( FAIL_QCOLOR );

	QString sLog = UTIL::DATE::yyyyMMDDHHmmss(true) + msg;
	if( t == LOG_TYPE::_NORMAL_LOG )
	{
		tf.setForeground( PASS_QCOLOR );
		ui->teLog->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAIL_QCOLOR );
		ui->teLog->setCurrentCharFormat( tf );
	}
	ui->teLog->appendPlainText( sLog );

	if( m_bAutoScroll )
	{
		QTextCursor c = ui->teLog->textCursor();
		c.movePosition(QTextCursor::End);
		ui->teLog->setTextCursor(c);
	}
}
