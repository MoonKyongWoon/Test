#include "PLCSimulator.hpp"
#include "ui_PLCSimulator.h"

CPLCSimulator::CPLCSimulator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CPLCSimulator)
{
    ui->setupUi(this);
    InitVariables();
    InitPlcAddressMap();
    InitUi();
    InitDirectory();
    InitSigSlots();
    InitTcpServer();
}

CPLCSimulator::~CPLCSimulator()
{
    delete ui;
}

void CPLCSimulator::InitVariables()
{
    m_sSvrIp   = "";
    m_uSvrPort = 12289;
    m_nClientConn = 0x0;
    m_mapId.clear();

    m_mapExitCode[PLC_EXIT_CODE_OK   ] = "OK";
    m_mapExitCode[PLC_EXIT_CODE_ER01 ] = "ER01";
    m_mapExitCode[PLC_EXIT_CODE_ER02 ] = "ER02";
    m_mapExitCode[PLC_EXIT_CODE_ER03 ] = "ER03";
    m_mapExitCode[PLC_EXIT_CODE_ER04 ] = "ER04";
    m_mapExitCode[PLC_EXIT_CODE_ER05 ] = "ER05";
    m_mapExitCode[PLC_EXIT_CODE_ER06 ] = "ER06";
    m_mapExitCode[PLC_EXIT_CODE_ER08 ] = "ER08";
    m_mapExitCode[PLC_EXIT_CODE_ER51 ] = "ER51";
    m_mapExitCode[PLC_EXIT_CODE_ER52 ] = "ER52";
    m_mapExitCode[PLC_ERROR_CODE_ER1 ] = "11";
    m_mapExitCode[PLC_ERROR_CODE_ER2 ] = "21";
    m_mapExitCode[PLC_ERROR_CODE_ER3 ] = "31";
    m_mapExitCode[PLC_ERROR_CODE_ER8 ] = "81";
    m_mapExitCode[PLC_ERROR_CODE_ER9 ] = "91";
    m_mapExitCode[PLC_ERROR_CODE_ERA ] = "A1";
    m_mapExitCode[PLC_ERROR_CODE_ERB ] = "B1";
    m_mapExitCode[PLC_ERROR_CODE_ERC ] = "C1";
    m_mapExitCode[PLC_ERROR_CODE_ERF ] = "F1";
    m_mapExitCode[PLC_ERROR_CODE_USER] = "U1";
}

/*
struct _stPlcAddrMap
{
    EN_PLC_DEST dest;
    YOKOGAWA_PLC_CMD_TYPE type;
    QString description;
};
*/
void CPLCSimulator::InitPlcAddressMap()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//COMMON - STATUS 1/2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_mapPlcAddr["I00210"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_MASK_OFF          , "Get Mask Off Mode"           };
	m_mapPlcAddr["I00211"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_MASK_ON           , "Get Mask On Mode"            };
	m_mapPlcAddr["I00212"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_WATCHDOG_EN       , "Get Watch Dog Enable Mode"   };
	m_mapPlcAddr["I00213"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_WATCHDOG_DIS      , "Get Watch Dog Disable Mode"  };
	m_mapPlcAddr["I00214"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_DOOR_OPEN         , "Get Door Open Status"        };
	m_mapPlcAddr["I00215"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_DOOR_CLOSE        , "Get Door Close Status"       };
	m_mapPlcAddr["I00216"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_UPPER_RACK_DETECT , "Get Upper Rack detect"       };
	m_mapPlcAddr["I00217"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_BOTTOM_RACK_DETECT, "Get Bottom Rack Detect"      };
	m_mapPlcAddr["I00218"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_RACKIN_COMPLETE   , "Get RackIn Complete"         };
	m_mapPlcAddr["I00219"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_RACKOUT_COMPLETE  , "Get Rackout Complete"        };
	m_mapPlcAddr["I00220"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_EQ_READY_STATUS   , "Get Eq Ready Status"         };
	m_mapPlcAddr["D100"  ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_STATUS_D100       , "Get Status1"                 };

	m_mapPlcAddr["I00226"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_TEMP_RUN_MODE     , "Get Temp Run Mode"           };
	m_mapPlcAddr["I00227"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_TEMP_STOP_MODE    , "Get Temp Stop Mode"          };
	m_mapPlcAddr["I00228"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_TEMP_AMB_MODE     , "Get Temp Amb Mode"           };
	m_mapPlcAddr["I00229"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_SYSTEM_DC_POWER_ON, "Get System DC Power On"      };
	m_mapPlcAddr["I00230"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_TOWER_LAMP_LOCAL  , "Get Tower Lamp Local"        };
	m_mapPlcAddr["I00231"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_TOWER_LAMP_REMOTE , "Get Tower Lamp Remote"       };
	m_mapPlcAddr["I00233"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_FINGERUP_RACK     , "Get Finger Up RackIn"        };
	m_mapPlcAddr["I00234"] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS , EN_GET_COMMON_DOOR_IGNORE_RACK  , "Get Door Ignore Rack"        };
	m_mapPlcAddr["D101"  ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_STATUS_D101       , "Get Status2"                 };

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//COMMON - WORD STATUS
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_mapPlcAddr["D999"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_PLC_VERSION      , "Get PLC Version"             };
	m_mapPlcAddr["D01100" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_PV               , "PV READ"                     };
	m_mapPlcAddr["D01101" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_SV               , "SV READ"                     };
	m_mapPlcAddr["D01110" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_PCW_TEMPERATURE  , "PCW TEMPERATURE"             };
	m_mapPlcAddr["D01111" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_WPS_PRESSURE            , "WPS PRESSURE"                };
	m_mapPlcAddr["D01112" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_PCW_FLOW_READ           , "PCW FLOW READ"               };

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//COMMON - BIT WRITE
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_mapPlcAddr["I00110" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_TEMP_RUN_TRIGGER        , "Set Temp run trigger"        };
	m_mapPlcAddr["I00111" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_TEMP_STOP_TRIGGER       , "Set Temp stop trigger"       };
	m_mapPlcAddr["I00112" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_AMB_TRIGGER             , "Set Amb trigger"             };
	m_mapPlcAddr["I00113" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_AMB_AUTO_TRIGGER        , "Set Auto Amb trigger"        };
	m_mapPlcAddr["I00117" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_ALARM_RESET             , "Set Alarm Reset"             };
	m_mapPlcAddr["I00118" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_MASK_ON_OFF             , "Set Remote/Local"            };
	m_mapPlcAddr["I01105" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_MASK_ON_OFF             , "Set Remote/Local"            };
	m_mapPlcAddr["I00120" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_DOOR_OPEN_TRIGGER       , "Set Door Open Trigger"       };
	m_mapPlcAddr["I00121" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_DOOR_CLOSE_TRIGGER      , "Set Door Close Trigger"      };
	m_mapPlcAddr["I00122" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_RACK_IN_TRIGGER         , "Set Rack In Trigger"         };
	m_mapPlcAddr["I00123" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_RACK_OUT_TRIGGER        , "Set Rack Out Trigger"        };
	m_mapPlcAddr["I00124" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_RACK_NOP_TRIGGER        , "Set Rack Nop Trigger"        };
	m_mapPlcAddr["I00124" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_GET_RACK_NOP_TRIGGER        , "Set Rack Nop Trigger"        };
	m_mapPlcAddr["I00960" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_BUZZER_OFF              , "Set Buzzer Off"              };
	m_mapPlcAddr["I00970" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_TOWER_LAMP_REMOTE_LOCAL , "Set TowerLamp Remote/Local"  };
	m_mapPlcAddr["I00971" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_BUZZER_OFF              , "Set Buzzer Off"              };
	m_mapPlcAddr["I00756" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_FINGER_UP_RACK_IN_ON    , "FINGER UP RACKIN TRIGGER ON" };
	m_mapPlcAddr["I00757" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_FINGER_UP_RACK_IN_OFF   , "FINGER UP RACKIN TRIGGER OFF"};
	m_mapPlcAddr["I00758" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_DOOR_IGN_RACK_INOUT_ON  , "DOOR IGN RACK IN/OUT ON"     };
	m_mapPlcAddr["I00759" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_DOOR_IGN_RACK_INOUT_OFF , "DOOR IGN RACK IN/OUT OFF"    };
	m_mapPlcAddr["Y00541" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_BIT_ACCESS, EN_SET_DC_POWER_ONOFF          , "SYSTEM DC POWER ON/OFF"      };

	m_mapPlcAddr["D00901" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_SET_WATCHDOG_PERIOD         , "EN_SET_WATCHDOG_PERIOD"      };
	m_mapPlcAddr["D00902" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_SET_WATCHDOG_TOGGLE         , "EN_SET_WATCHDOG_TOGGLE"      };
	m_mapPlcAddr["D00903" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_SET_WATCHDOG_EN_DIS         , "EN_SET_WATCHDOG_EN_DIS"      };
	m_mapPlcAddr["D01103" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_SET_SV                      , "EN_SET_SV"                   };
	m_mapPlcAddr["D01113" ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_SET_AMB_SV                  , "EN_SET_AMB_SV"               };


	m_mapPlcAddr["D150"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D150     , "ALARM READ D150"             };
	m_mapPlcAddr["D151"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D151     , "ALARM READ D151"             };
	m_mapPlcAddr["D152"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D152     , "ALARM READ D152"             };
	m_mapPlcAddr["D153"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D153     , "ALARM READ D153"             };
	m_mapPlcAddr["D154"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D154     , "ALARM READ D154"             };
	m_mapPlcAddr["D155"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D155     , "ALARM READ D155"             };
	m_mapPlcAddr["D156"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D156     , "ALARM READ D156"             };
	m_mapPlcAddr["D157"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D157     , "ALARM READ D157"             };
	m_mapPlcAddr["D158"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D158     , "ALARM READ D158"             };
	m_mapPlcAddr["D159"   ] = _stPlcAddrMap{ EN_DEST_ALARM, YKG_TYPE_WORD_ACCESS, EN_GET_COMMON_ALARM_D159     , "ALARM READ D159"             };

	//2024.08.20 - KWMOON
	m_mapPlcAddr["D01130"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP1_PRESSURE_HIGH  , "COMP1 Pressure High value Read"   };
	m_mapPlcAddr["D01131"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP1_PRESSURE_LOW   , "COMP1 Pressure Low  value Read"   };
	m_mapPlcAddr["D01132"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP2_PRESSURE_HIGH  , "COMP2 Pressure High value Read"   };
	m_mapPlcAddr["D01133"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP2_PRESSURE_LOW   , "COMP2 Pressure Low  value Read"   };

	m_mapPlcAddr["D01135"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP1_OUT_TEMP       , "COMP1 Temp Out Read"   };
	m_mapPlcAddr["D01136"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP1_IN_TEMP        , "COMP1 Temp In  Read"   };
	m_mapPlcAddr["D01137"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP2_OUT_TEMP       , "COMP2 Temp Out Read"   };
	m_mapPlcAddr["D01138"   ] = _stPlcAddrMap{ EN_DEST_COMM, YKG_TYPE_WORD_ACCESS, EN_GET_COMP2_IN_TEMP        , "COMP2 Temp In  Read"   };
}

void CPLCSimulator::InitUi()
{
    InitUiControls();
    InitUiIpListPort();
    InitUiConnectionTable();
    InitPlcView();

    QFont font;
    font.setPointSize(9);
    font.setFamily("Courier New");
    ui->teTcpLog->setFont(font);
    ui->tePlcLog->setFont(font);

	setWindowTitle( tr("%1 - %2[%3]").arg(ATPLCSIMMUI_NAME).arg(ATPLCSIMMUI_VER).arg(ATPLCSIMMUI_DATE));
}

void CPLCSimulator::InitUiControls()
{
    ui->btnListen->setEnabled( true );
    ui->btnClose->setEnabled( false );
}

void CPLCSimulator::InitUiIpListPort()
{
    ui->cboIp->clear();
    ui->cboIp->addItem("IP SELECT");
    foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
    {
        if ( interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {
            foreach (QNetworkAddressEntry entry, interface.addressEntries())
            {
                if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".") )
                {
                    QString ip = entry.ip().toString(); //interface.humanReadableName()
                    ui->cboIp->addItem( ip );
                }
            }
        }
    }
    ui->cboIp->setCurrentIndex(0);
    ui->lePort->setText( tr("%1").arg(m_uSvrPort));
}

void CPLCSimulator::InitUiConnectionTable()
{
    QTableWidget* pTable = ui->tblConnList;
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //pTable->setStyleSheet("QTableWidget::item:selected { border: 1px solid #ff847c;} QTableWidget {selection-background-color:transparent;}");
    pTable->verticalHeader()->setVisible(false);
    pTable->resizeColumnsToContents();
    pTable->resizeRowsToContents();
    pTable->horizontalHeader()->setStretchLastSection(true);
    pTable->verticalHeader()->setStretchLastSection(true);
    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pTable->clearContents();
    pTable->setColumnWidth(0, 50);
    pTable->setColumnWidth(1,100);
}

void CPLCSimulator::InitLogViewContextMenuRequest()
{
    ui->tePlcLog->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->teTcpLog->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( ui->tePlcLog   , &QPlainTextEdit::customContextMenuRequested, this, &CPLCSimulator::SlotCustomContextMenuRequested );
    connect( ui->teTcpLog   , &QPlainTextEdit::customContextMenuRequested, this, &CPLCSimulator::SlotCustomContextMenuRequested );
}

void CPLCSimulator::SlotCustomContextMenuRequested( QPoint p )
{
    // Start with the standard menu.
    QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
    QMenu * pMenu = pSender->createStandardContextMenu();
    QAction * pAction;

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

void CPLCSimulator::InitPlcView()
{
    ui->tabChamberInfo->clear();
    m_pCommPlcView = new CCommPLC();
	m_pAlarmView   = new CAlarmView();
	ui->tabChamberInfo->addTab( m_pCommPlcView, "STATUS" );
	ui->tabChamberInfo->addTab( m_pAlarmView  , "ALARM" );
	connect( m_pCommPlcView, &CCommPLC::SigPlcLogMsg  , this, &CPLCSimulator::AppendPlcLog );
	connect( m_pCommPlcView, &CCommPLC::SigAlarmReset , m_pAlarmView, &CAlarmView::SlotAlarmReset );
	//connect( m_pChPlcView[ch], &CZone::SigPlcLogMsg, this, &CPLCSimulator::AppendPlcLog );
}

void CPLCSimulator::InitTcpServer()
{
    m_pPlcServer = new CPLCServer(this);
    connect( m_pPlcServer, &QTcpServer::acceptError            , this, &CPLCSimulator::SlotAcceptError   );
    connect( m_pPlcServer, &CPLCServer::SigIncommingConnection , this, &CPLCSimulator::SlotNewConnection );
}

bool CPLCSimulator::ListenTcpServer( QString ip, quint16 port )
{
    if( m_pPlcServer->isListening() )
    {
        CloseTcpServer();
    }
    if( !m_pPlcServer->listen( QHostAddress(ip), port ) )
    {
        AppendTcpLog( tr("PGM Server listen error![%1]").arg(m_pPlcServer->errorString()), false );
        return false;
    }
    AppendTcpLog( tr("PGM Server listen [%1:%2]").arg(ip).arg(port), true );
    ui->btnListen->setEnabled( false );
    ui->btnClose->setEnabled( true );
    return true;
}

bool CPLCSimulator::CloseTcpServer()
{
    m_pPlcServer->close();
    AppendTcpLog( tr("PGM Server Close"), true );
    ui->btnListen->setEnabled( true );
    ui->btnClose->setEnabled( false );
    return true;
}

void CPLCSimulator::InitSigSlots()
{
    InitLogViewContextMenuRequest();
    connect( ui->btnListen    , &QPushButton::clicked                 , this         , &CPLCSimulator::SlotBtnOpenClicked       );
    connect( ui->btnClose     , &QPushButton::clicked                 , this         , &CPLCSimulator::SlotBtnCloseClicked      );
}

void CPLCSimulator::InitDirectory()
{
    QString sDirRoot   = QDir::current().currentPath();
    QString sDirConfig = tr("%1/%2").arg(sDirRoot).arg(CONFIG_DIR);

    //Config
    QDir dirConfig( sDirConfig );
    if( !dirConfig.exists())
        dirConfig.mkdir( sDirConfig );
}

void CPLCSimulator::AppendTcpLog ( QString log, bool pass)
{
    QPlainTextEdit* te = ui->teTcpLog;
    if( te->document()->blockCount()> MAX_LOG_LINE_NUM )
        te->clear();

    QTextCharFormat tf;
    tf = te->currentCharFormat();
    tf.setForeground( FAIL_QCOLOR );

    QString sLog = Ablee::CUtil::getCurrentDateTime(true) + log;
    if( pass )
    {
        tf.setForeground( PASS_QCOLOR );
        te->setCurrentCharFormat( tf );
        //m_pLog[0]->setTextColor( PASS_QCOLOR );
    }
    else
    {
        tf.setForeground( FAIL_QCOLOR );
        te->setCurrentCharFormat( tf );
    }
    te->appendPlainText( sLog );

    QTextCursor c =  te->textCursor();
    c.movePosition(QTextCursor::End);
    te->setTextCursor(c);
}

void CPLCSimulator::AppendPlcLog( QString log, bool pass)
{
    QPlainTextEdit* te = ui->tePlcLog;
    if( te->document()->blockCount()> MAX_LOG_LINE_NUM )
        te->clear();

    QTextCharFormat tf;
    tf = te->currentCharFormat();
    tf.setForeground( FAIL_QCOLOR );

    QString sLog = Ablee::CUtil::getCurrentDateTime(true) + log;
    if( pass )
    {
        tf.setForeground( PASS_QCOLOR );
        te->setCurrentCharFormat( tf );
        //m_pLog[0]->setTextColor( PASS_QCOLOR );
    }
    else
    {
        tf.setForeground( FAIL_QCOLOR );
        te->setCurrentCharFormat( tf );
    }
    te->appendPlainText( sLog );

    QTextCursor c =  te->textCursor();
    c.movePosition(QTextCursor::End);
     te->setTextCursor(c);
}


///////////////////////////////////////////////////////////////////////////////
///Slots
///////////////////////////////////////////////////////////////////////////////

void CPLCSimulator::SlotBtnOpenClicked()
{
    bool bOk = false;
    QString ip = ui->cboIp->currentText().trimmed();
    quint16 port = ui->lePort->text().toUShort(&bOk,10);

    if( !Ablee::CUtil::IsValidIpV4Format(ip.toStdString()) )
    {
        QMessageBox::critical( this, "IP Address", tr("IP를 선택하세요."), QMessageBox::Ok );
        ui->cboIp->setFocus();
        return;
    }
    if( (port<1) || (port>65534) )
    {
        QMessageBox::critical( this, "Port", tr("Port를 0 - 65534 값으로 입력하세요."), QMessageBox::Ok );
        ui->lePort->setFocus();
        return;
    }
    ListenTcpServer(ip, port);
}

void CPLCSimulator::SlotBtnCloseClicked()
{
    CloseTcpServer();
}

void CPLCSimulator::SlotAcceptError(QAbstractSocket::SocketError err)
{
    QString sErrDesc = Ablee::CSockErr::GetSocketErrDescription(err);
    AppendTcpLog( tr("PLC Server accept error [%1]").arg(sErrDesc), false );
}

int CPLCSimulator::GetGeneratedId()
{
    if( m_nClientConn > 65535 )
        m_nClientConn = 0x0;
    return m_nClientConn++;
}

void CPLCSimulator::SlotNewConnection(qintptr h)
{
    //QThread* pThread = new QThread;
    int id = GetGeneratedId();
    CPLCClient* pClient = new CPLCClient( id, h );
    //pClient->moveToThread(pThread);
    connect( pClient , &CPLCClient::SigClientConnected     , this   , &CPLCSimulator::SlotClientConnected     );
    connect( pClient , &CPLCClient::SigClientDisconnected  , this   , &CPLCSimulator::SlotClientDisconnected  );
    connect( pClient , &CPLCClient::SigRecvPacket          , this   , &CPLCSimulator::SlotClientRecvPacket    );
    connect( pClient , &CPLCClient::SigTcpConfigLog        , [=]( QString msg, bool pass) { AppendTcpLog (msg,pass); } );
    connect( pClient , &CPLCClient::SigTcpMessageLog       , [=]( QString msg, bool pass) { AppendPlcLog(msg,pass);  } );
    connect( this    , &CPLCSimulator::SigClientSendPacket , pClient, &CPLCClient::SlotSendPacket             );
    pClient->DoEmit();
    //connect( this    , &CPLCSimulator::SigTcpDescriptor    , pClient, &CPLCClient::SlotNewConnection          );
    //pThread->start();
    //emit SigTcpDescriptor(h);
    //disconnect( this    , &CPLCSimulator::SigTcpDescriptor    , pClient, &CPLCClient::SlotNewConnection          );
}

void CPLCSimulator::SlotClientConnected(int id, QString ip)
{
    CPLCClient* pClient = qobject_cast<CPLCClient*>(QObject::sender());
	if( pClient != nullptr )
	{
        m_mapId[id]= pClient;
	}
	AppendTcpLog( QString("").asprintf("ID:%05X IP:%s Connected!", id, ip.toLatin1().data()), true );
    UpdateConnectionTable();
}

void CPLCSimulator::UpdateConnectionTable()
{
    QTableWidget* pTable = ui->tblConnList;
    pTable->clearContents();

    int row=0;
    std::map<int,CPLCClient*>::iterator it;
    for( it = m_mapId.begin(); it != m_mapId.end(); it++)
    {
        QTableWidgetItem* pItemNo = new QTableWidgetItem;
        QTableWidgetItem* pItemId = new QTableWidgetItem;
        QTableWidgetItem* pItemIp = new QTableWidgetItem;

        pItemNo->setFlags(pItemNo->flags()^Qt::ItemIsEditable);
        pItemNo->setTextAlignment(Qt::AlignCenter);
        pItemNo->setBackground( (row%2)?ODD_ROW_QCOLOR:EVEN_ROW_QCOLOR );

        pItemId->setFlags(pItemNo->flags()^Qt::ItemIsEditable);
        pItemId->setTextAlignment(Qt::AlignCenter);
        pItemId->setBackground( (row%2)?ODD_ROW_QCOLOR:EVEN_ROW_QCOLOR );

        pItemIp->setFlags(pItemNo->flags()^Qt::ItemIsEditable);
        pItemIp->setTextAlignment(Qt::AlignCenter);
        pItemIp->setBackground( (row%2)?ODD_ROW_QCOLOR:EVEN_ROW_QCOLOR );

		pItemNo->setText( QString("").asprintf("%03d",row) );
		pItemId->setText( QString("").asprintf("%05X",it->first) );
        pItemIp->setText( static_cast<CPLCClient*>(it->second)->GetIp() );

        pTable->setItem(row, 0, pItemNo );
        pTable->setItem(row, 1, pItemId );
        pTable->setItem(row, 2, pItemIp );
        row++;
    }
}

void CPLCSimulator::SlotClientDisconnected(int id)
{
    CPLCClient* pClient = qobject_cast<CPLCClient*>(QObject::sender());
    if( pClient != nullptr )
    {
        auto exist = m_mapId.find( id );
        if( exist != m_mapId.end() )
            m_mapId.erase( id );
    }
	AppendTcpLog( QString("").asprintf("ID:%05X Disconnected!", id), true );
    UpdateConnectionTable();
}

void CPLCSimulator::SlotReloadConfigIni()
{
    //LoadConfigIni();
}

//Receive Packet From Client
void CPLCSimulator::SlotClientRecvPacket(int id, QByteArray ba)
{
    QString sPacket = QString(ba).left(ba.size()-2);    // CRLF Remove
    QString ack    = "1";
    QString cpu    = sPacket.mid(1,1);
    QString cmd    = sPacket.mid(2,3);
    QString addr   = "";
    QString len    = "";
    QString detail = sPacket.mid(5);

    if( cmd == "BRD" )
    {
        DoBRD( id, cpu, cmd, detail );
    }
    else if( cmd == "BRR" )
    {
        DoBRR( id, cpu, cmd, detail );
    }
    else if( cmd == "BWR" )
    {
        DoBWR( id, cpu, cmd, detail );
    }
    else if( cmd == "BRW" )
    {
        DoBRW( id, cpu, cmd, detail );
    }
    else if( cmd == "WRD" )
    {
        DoWRD( id, cpu, cmd, detail );
    }
    else if( cmd == "WRR" )
    {
        DoWRR( id, cpu, cmd, detail );
    }
    else if( cmd == "WWR" )
    {
        DoWWR( id, cpu, cmd, detail );
    }
    else if( cmd == "WRW" )
    {
        DoWRW( id, cpu, cmd, detail );
    }
}

QString CPLCSimulator::GetExitCode(EN_PLC_EXIT_ERROR_CODE code)
{
    QString sCode = "ERXX";
    auto exist = m_mapExitCode.find( code );
    if( exist != m_mapExitCode.end() )
        sCode = exist->second;
    return sCode.trimmed();
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoBRD
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption Bit Read ex) 01BRDI09300,001CRLF
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoBRD( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    QByteArray baPacket;
    QByteArray baData;
	QStringList listWord = data.split( QRegExp(",| "), Qt::KeepEmptyParts );
    if( listWord.count() < 2 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_EXIT_CODE_ER52).toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ERA).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }

    QString sAddress = listWord.at(0);
    QString sLength  = listWord.at(1);
    QString sKeyAddr = sAddress.left(1);
    int nSet         = 0x0;
    int nAddr        = sAddress.mid(1).toInt(&bOk);
    int nBitLength   = sLength.toInt(&bOk);

    if( nBitLength <   1 ) nBitLength = 1;
    if( nBitLength > 255 ) nBitLength = 255;

    baData.clear();
    for( int n=0;n<nBitLength;n++ )
    {
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            _stPlcAddrMap addr = search->second;
            if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_BIT_ACCESS )
            {
                int nGet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
                    if( m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                        baData.append( static_cast<char>(nGet&0x1)?"1":"0" );
                    else
                        baData.append("0");
					break;
				case EN_PLC_DEST::EN_DEST_ALARM:
					break;
                }
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
        nAddr++;
		sAddress = QString("").asprintf("%s%05d", sKeyAddr.toLatin1().data(), nAddr );
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
    baPacket.append("OK");
    baPacket.append(baData);
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoBRR
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption Bit Random Read
/// \caption 01BRR02,I09300,I09302
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoBRR( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    QByteArray baPacket;
    QByteArray baData;
    QString sData    = data.replace("\r\n","");
    QString sLength  = sData.mid(0,2);
    int nLength      = sLength.toInt(&bOk);
    if( nLength <  1 ) nLength = 1;
    if( nLength > 32 ) nLength = 32;

	QStringList listWord = sData.split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() < 2 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ERA).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
        //Error
    }
    else if( nLength != (listWord.count()-1) )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_EXIT_CODE_ER52).toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ERA).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }

    int nSet = 0x0;
    baData.clear();
    for( int n=1;n<listWord.count();n++ )
    {
        QString sAddress = listWord.at(n);
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            _stPlcAddrMap addr = search->second;
            if( addr.type != YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_BIT_ACCESS )
            {
                int nGet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
                    if( m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                        baData.append( static_cast<char>(nGet&0x1) );
                    else
                        baData.append("0");
                    break;
				case EN_PLC_DEST::EN_DEST_ALARM:
					break;
				}
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER3).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
	baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
	baPacket.append(baData);
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoBWR
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \caption I00030,002,01 CRLF ( Bit Write )
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoBWR( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    bool bSuccess = false;
    QByteArray baPacket;
    QByteArray baData;
    QString sData        = data.replace("\r\n","");
	QStringList listWord = sData.split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() != 3 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER2).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }
    else
    {
        QString sAddress = listWord.at(0);
        QString sLen     = listWord.at(1);
        QString sValue   = listWord.at(2);
        QString sKeyAddr = sAddress.left(1);
        QString sDevice  = "";
        int nGet         = 0x0;
        int nAddr        = sAddress.mid(1).toInt(&bOk);
        int nBitLength   = sLen.toInt(&bOk);
        if( nBitLength <   1 ) nBitLength =   1;
        if( nBitLength > 256 ) nBitLength = 256;
        for( int d=0;d<nBitLength;d++ )
        {
            auto search = m_mapPlcAddr.find(sAddress);
            if( search != m_mapPlcAddr.end() )
            {
                _stPlcAddrMap addr = search->second;
                if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_BIT_ACCESS )
                {
                    int nSet = (sValue.at(d)=='1')?1:0;
                    switch( addr.dest )
                    {
                    case EN_PLC_DEST::EN_DEST_COMM:
						if( sAddress == "I00124")
						{
							if( m_pCommPlcView->DoCommonCmd( EN_SET_RACK_NOP_TRIGGER, addr.type, nSet, nGet ) )
								bSuccess = true;
						}
						else if( m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
						{
                            bSuccess = true;
						}
                        else
                        {
                            baPacket.append("1");
							baPacket.append(cpu.toUtf8());
							baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
							baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
							baPacket.append(cmd.toUtf8());
                            baPacket.append(0xd);
                            baPacket.append(0xa);
                            emit SigClientSendPacket(id, baPacket, baPacket.size() );
                            return false;
                        }
                        break;
					case EN_PLC_DEST::EN_DEST_ALARM:
						break;
                    }
                }
            }
            else
            {
                baPacket.append("1");
				baPacket.append(cpu.toUtf8());
				baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
				baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
				baPacket.append(cmd.toUtf8());
                baPacket.append(0xd);
                baPacket.append(0xa);
                emit SigClientSendPacket(id, baPacket, baPacket.size() );
                return false;
            }
            nAddr++;
			sAddress = QString("").asprintf("%s%05d", sKeyAddr.toLatin1().data(), nAddr );
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
	baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoBRW
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption 02I00015,1,I00031,1CRLF ( Bit Random Write )
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoBRW( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    bool bSuccess = false;
    QByteArray baPacket;
    QByteArray baData;
    QString sData    = data.replace("\r\n","");
    QString sLen     = sData.left(2);
    int nLength      = sLen.toInt(&bOk);
    if( nLength <   1 ) nLength =   1;
    if( nLength > 256 ) nLength = 256;
	QStringList listWord = sData.mid(2).split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() < 2 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER2).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }
    else
    {
        for( int d=0;d<nLength;d++ )
        {
            QString sAddress = listWord.at(d*2+0);
            QString sValue   = listWord.at(d*2+1);
            auto search = m_mapPlcAddr.find(sAddress);
            if( search != m_mapPlcAddr.end() )
            {
                _stPlcAddrMap addr = search->second;
                if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_BIT_ACCESS )
                {
                    int nGet = 0x0;
                    int nSet = (sValue == "1")?1:0;
                    switch( addr.dest )
                    {
                    case EN_PLC_DEST::EN_DEST_COMM:
                        if( !m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                        {
                            baPacket.append("1");
							baPacket.append(cpu.toUtf8());
							baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
							baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
							baPacket.append(cmd.toUtf8());
                            baPacket.append(0xd);
                            baPacket.append(0xa);
                            emit SigClientSendPacket(id, baPacket, baPacket.size() );
                            return false;
                        }
                        break;
					case EN_PLC_DEST::EN_DEST_ALARM:
						break;
                    }
                }
            }
            else
            {
                baPacket.append("1");
				baPacket.append(cpu.toUtf8());
				baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
				baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
				baPacket.append(cmd.toUtf8());
                baPacket.append(0xd);
                baPacket.append(0xa);
                emit SigClientSendPacket(id, baPacket, baPacket.size() );
                return false;
            }
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
	baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoWRD
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption D01100,01CRLF
bool CPLCSimulator::DoWRD( int id, QString cpu, QString cmd, QString data )
{
    QByteArray baPacket;
    QByteArray baData;
	QStringList listWord = data.split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() < 2 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER2).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }

    bool bOk = false;
    QString sAddress = listWord.at(0);
    QString sLength  = listWord.at(1);
    QString sKeyAddr = sAddress.left(1);
    int nAddr        = sAddress.mid(1).toInt(&bOk);
    int nWordLength  = sLength.toInt(&bOk);
    if( nWordLength <  1 ) nWordLength = 1;
    if( nWordLength > 64 ) nWordLength = 64;

    baData.clear();
    for( int n=0;n<nWordLength;n++ )
    {
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            _stPlcAddrMap addr = search->second;
            if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_WORD_ACCESS )
            {
                int nGet = 0x0;
                int nSet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
                    if( m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                    {
						QString format = QString("").asprintf("%04X", nGet&0xFFFF);
						baData.append( format.toUtf8() );
                    }
                    else
                    {
                        baData.append("0000");
                    }
                    break;
				case EN_PLC_DEST::EN_DEST_ALARM:
						if( m_pAlarmView->DoReadAlarm( addr.cmd, nGet) )
						{
							QString format = QString("").asprintf("%04X", nGet&0xFFFF);
							baData.append( format.toUtf8() );
						}
						else
						{
							baData.append("0000");
						}
					break;
				}
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
        nAddr++;
		sAddress = QString("").asprintf("%s%05d", sKeyAddr.toLatin1().data(), nAddr );
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
	baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
    baPacket.append(baData);
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoWRR
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption 02D01100, D01101CRLF
bool CPLCSimulator::DoWRR( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    QByteArray baPacket;
    QByteArray baData;
    QString sLength  = data.left(2);
    int nWordLength      = sLength.toInt(&bOk);
    if( nWordLength <  1 ) nWordLength = 1;
    if( nWordLength > 32 ) nWordLength = 32;

	QStringList listWord = data.mid(2).split( QRegExp(",| "), Qt::KeepEmptyParts);

    baData.clear();
    for( int n=0;n<nWordLength;n++ )
    {
        QString sAddress = listWord.at(n);
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            _stPlcAddrMap addr = search->second;
            if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_WORD_ACCESS )
            {
                int nGet = 0x0;
                int nSet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
                    if( m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                    {
						QString format = QString("").asprintf("%04X", nGet&0xFFFF);
						baData.append( format.toUtf8() );
                    }
                    else
                    {
                        baData.append("0000");
                    }
                    break;
				case EN_PLC_DEST::EN_DEST_ALARM:
					break;
                }
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
	baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
    baPacket.append(baData);
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoWWR
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return
/// \caption D01103,01,04E2CRLF ( WORD WRITE )
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoWWR( int id, QString cpu, QString cmd, QString data )
{
    bool bOk      = false;
    bool bSuccess = true;
    QByteArray baPacket;
    QByteArray baData;
	QStringList listWord = data.split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() < 3 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER3).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
    }
    QString sAddress = listWord.at(0);
    QString sKeyAddr = sAddress.left(1);
    QString sLength  = listWord.at(1);
    QString sValue   = listWord.at(2);
    int nWordLength  = sLength.toInt(&bOk);
    if( nWordLength <  1 ) nWordLength = 1;
    if( nWordLength > 64 ) nWordLength = 64;

    for( int n=0;n<1;n++ )
    {
        QString sDevice = sAddress;
        QString sAddress = listWord.at(n);
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            QString sWord = "";
            _stPlcAddrMap addr = search->second;
            if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_WORD_ACCESS )
            {
                int nGet = 0x0;
                int nSet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
                    sWord = sValue.mid(n*4,4);
                    nSet = sWord.toInt(&bOk,16);
                    if( !m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                    {
                        bSuccess = false;
                    }
					break;
				case EN_PLC_DEST::EN_DEST_ALARM:
					break;
                }
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
    if( bSuccess )
    {
		baPacket.append(GetExitCode(PLC_EXIT_CODE_OK).toUtf8());
    }
    else
    {
		baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
		baPacket.append(cmd.toUtf8());
    }
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief CPLCSimulator::DoWRW
/// \param id
/// \param cpu
/// \param cmd
/// \param data
/// \return ) 01WRW02D01102,00FA,D01103,04E2CRLF
/////////////////////////////////////////////////////////////////////////////////
bool CPLCSimulator::DoWRW( int id, QString cpu, QString cmd, QString data )
{
    bool bOk = false;
    bool bSuccess = true;
    QByteArray baPacket;
    QByteArray baData;
    QString sLength  = data.left(2);
    int nWordLength      = sLength.toInt(&bOk);
    if( nWordLength <  1 ) nWordLength = 1;
    if( nWordLength > 32 ) nWordLength = 32;

	QStringList listWord = data.mid(2).split( QRegExp(",| "), Qt::KeepEmptyParts);
    if( listWord.count() < 2 )
    {
        baPacket.append("1");
		baPacket.append(cpu.toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER3).toUtf8());
		baPacket.append(cmd.toUtf8());
        baPacket.append(0xd);
        baPacket.append(0xa);
        emit SigClientSendPacket(id, baPacket, baPacket.size() );
        return false;
        //Error
    }
    for( int n=0;n<nWordLength;n++ )
    {
        QString sAddress = listWord.at(n*2+0);
        QString sDevice  = sAddress;
        QString sValue   = listWord.at(n*2+1);
        auto search = m_mapPlcAddr.find(sAddress);
        if( search != m_mapPlcAddr.end() )
        {
            _stPlcAddrMap addr = search->second;
            if( addr.type == YOKOGAWA_PLC_CMD_TYPE::YKG_TYPE_WORD_ACCESS )
            {
                int nGet = 0x0;
                int nSet = 0x0;
                switch( addr.dest )
                {
                case EN_PLC_DEST::EN_DEST_COMM:
				{
                    QString sWord = sValue;
                    nSet = sWord.toInt(&bOk,16);
                    if( !m_pCommPlcView->DoCommonCmd( addr.cmd, addr.type, nSet, nGet ) )
                    {
                        bSuccess = false;
                    }
				}
				break;

				case EN_PLC_DEST::EN_DEST_ALARM:
					break;
                }
            }
        }
        else
        {
            baPacket.append("1");
			baPacket.append(cpu.toUtf8());
			baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
			baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
			baPacket.append(cmd.toUtf8());
            baPacket.append(0xd);
            baPacket.append(0xa);
            emit SigClientSendPacket(id, baPacket, baPacket.size() );
            return false;
        }
    }
    baPacket.append("1");
	baPacket.append(cpu.toUtf8());
    if( bSuccess )
    {
        baPacket.append("OK");
    }
    else
    {
		baPacket.append(GetExitCode(PLC_EXIT_CODE_ER08).toUtf8());
		baPacket.append(GetExitCode(PLC_ERROR_CODE_ER9).toUtf8());
		baPacket.append(cmd.toUtf8());
    }
    baPacket.append(0xd);
    baPacket.append(0xa);
    emit SigClientSendPacket(id, baPacket, baPacket.size() );
    return true;
}
