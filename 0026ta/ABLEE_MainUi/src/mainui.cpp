#include "mainui.hpp"
#include "ui_mainui.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CMainWindow), m_sApplicationName(qApp->applicationName())
{
	InitVariable();
	RegisterMetaType();
	InitDirectory();
	InitInstance();
	ConnectRcmdSigSlot();
	InitUi();
}

MainWindow::~MainWindow()
{
	KillTimer();

	if( m_pMgrIpc != nullptr )
	{
		if( m_pMgrIpc->IsConn() )
			m_pMgrIpc->CloseSocket();
		delete m_pMgrIpc;
		m_pMgrIpc = nullptr;
	}
	if( m_pTmsIpc != nullptr )
	{
		if( m_pTmsIpc->IsConn() )
			m_pTmsIpc->CloseSocket();
		delete m_pTmsIpc;
		m_pTmsIpc = nullptr;
	}
	if( m_pTestDataMgr != nullptr )
	{
		delete m_pTestDataMgr;
		m_pTestDataMgr = nullptr;
	}

	if( m_pShmChmb != nullptr )
	{
		SetSharedMemory( false, false );	//2023.06.19 - KWMOON
		if( m_pShmChmb->isAttached() )
			m_pShmChmb->detach();
	}

	if ( m_pHsmsMgrTh != nullptr )
	{
		if( m_pHsmsMgrTh->isRunning() )
		{
			m_pHsmsMgrTh->quit();
			m_pHsmsMgrTh->wait();
		}
	}

	if( m_pChmbMgrTh != nullptr )
	{
		m_pChmbMgrTh->quit();
		m_pChmbMgrTh->wait();
	}

	if ( m_pPgmDownloaderTh != nullptr)		//2024.06.11 - yseom
	{
		m_pPgmDownloaderTh->quit();
		m_pPgmDownloaderTh->wait();
	}
	//TEMP - MASK

	if ( m_bQuitAtTestMgr )
	{
		UTIL::PROCESS::Kill( ATMGR_NAME );
	}

	delete ui;
}

void MainWindow::SetWindowTitle(QString title, bool bVersion)
{
	if( bVersion )
	{
		QString text = QString("%1 %2 %3").arg(m_sApplicationName).arg(m_bAutoMode?"-auto":"-man").arg(ATMAIN_VER "[" ATMAIN_DATE "]") ;
		setWindowTitle( text );
	}
	else
	{
		setWindowTitle( title );
	}
}

void MainWindow::RegisterMetaType()
{
	qRegisterMetaType<EN_SLOT_STAT>("EN_SLOT_STAT");
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<EN_UI_STATUS>("EN_UI_STATUS");
	qRegisterMetaType<stTcpElement>("stTcpElement");
	qRegisterMetaType<stIpcElement>("stIpcElement");
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	qRegisterMetaType<stSlotInfo>("stSlotInfo");
	qRegisterMetaType<stSlotLive>("stSlotLive");
	qRegisterMetaType<stSetupInfo>("stSetupInfo");
	qRegisterMetaType<stSlotIp>("stSlotIp");
	qRegisterMetaType<stSlotCpu>("stSlotCpu");
	qRegisterMetaType<stVersion>("stVersion");
	qRegisterMetaType<stBibId>("stBibId");
	qRegisterMetaType<stFtpInfo>("stFtpInfo");
	qRegisterMetaType<stPgmDownload>("stPgmDownload");
	qRegisterMetaType<stPgmLive>("stPgmLive");
	qRegisterMetaType<stTestItem>("stTestItem");
	qRegisterMetaType<EN_STATUS_LABEL>("EN_STATUS_LABEL");
	qRegisterMetaType<stChamberStatus>("stChamberStatus");
	qRegisterMetaType<PROTOCOL::PLC_COMMAND>("PROTOCOL::PLC_COMMAND");
	qRegisterMetaType<CSecsMsg>("CSecsMsg");
	qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
	qRegisterMetaType<stGetTemp>("stGetTemp");

}

void MainWindow::InitDirectory()
{
	char* env_run  = getenv(ATRUN_DIR_ENV);
	char* env_data = getenv(ATDATA_DIR_ENV);
	char* env_os   = getenv(ATOS_DIR_ENV);

	QString os_path   = "";
	QString run_path  = "";
	QString data_path = "";

	if( env_os != NULL )
	{
		m_sOsDir = QString(env_os);
	}
	else
	{
		m_sOsDir = QString(DIR_ATOS_PATH);
	}

	if( env_run != NULL )
	{
		run_path = QString(env_run);
	}
	else
	{
		run_path = tr("%1/%2").arg(QDir::home().absolutePath()).arg("atsystem");
	}

	if( env_data != NULL )
	{
		data_path = tr("%1").arg(env_data);
	}
	else
	{
		data_path = tr("%1").arg(DIR_ATDATA_PATH);
	}

	QDir::setCurrent( run_path );
	QDir rdir(run_path);
	if( !rdir.exists( run_path)  )	rdir.mkdir( run_path  );
	if( !rdir.exists( data_path) )	rdir.mkdir( data_path );

	m_sCurrentDir  = run_path;
	m_sDownloadDir = tr("%1/%2").arg(run_path).arg(DIR_DOWN_PATH);
	m_sProgramDir  = tr("%1/%2").arg(run_path).arg(DIR_PROGRAM_PATH);
	m_sCfgDir      = tr("%1/%2").arg(run_path).arg(DIR_CFG_PATH);
	m_sDataRoot    = tr("%1/%2").arg(data_path).arg(DIR_ATDATA_NAME);
	m_sTempDatDir  = tr("%1/%2").arg(data_path).arg(DIR_TEMPERATURE);

	QDir dir(m_sDownloadDir);
	if( !dir.exists( m_sDownloadDir)  )	dir.mkdir( m_sDownloadDir );
	if( !dir.exists( m_sProgramDir )  )	dir.mkdir( m_sProgramDir  );
	if( !dir.exists( m_sDataRoot   )  )	dir.mkdir( m_sDataRoot    );
	if( !dir.exists( m_sTempDatDir )  )	dir.mkdir( m_sTempDatDir  );
	if( !dir.exists( m_sCfgDir  )  )	dir.mkdir( m_sCfgDir      );

	//2023.07.25 - KWMOON
	//QString sCpuDir1, sCpuDir2;
	//sCpuDir1  = tr("%1/%2").arg(m_sCfgDir).arg("cpu1");
	//sCpuDir2  = tr("%1/%2").arg(m_sCfgDir).arg("cpu2");
	//if( !dir.exists( sCpuDir1)  )	dir.mkdir( sCpuDir1 );
	//if( !dir.exists( sCpuDir2)  )	dir.mkdir( sCpuDir2 );

//	//2024.06.24 - hsbae
//	QString engr_data_path(tr("%1/%2").arg(m_sDataRoot).arg("engr"));
//	QString auto_data_path(tr("%1/%2").arg(m_sDataRoot).arg("auto"));
//	QString tget_data_path(tr("%1/%2").arg(auto_data_path).arg("TGET"));
//	QString board_maint_data_path(tr("%1/%2").arg(auto_data_path).arg("BOARD_MAINT_INFO"));
//	QString bi_open_lot_data_path(tr("%1/%2").arg(auto_data_path).arg("BI_OPEN_LOT"));
//	if( !dir.exists( engr_data_path			)  )	dir.mkdir( engr_data_path		);
//	if( !dir.exists( auto_data_path			)  )	dir.mkdir( auto_data_path		);
//	if( !dir.exists( tget_data_path			)  )	dir.mkdir( tget_data_path		);
//	if( !dir.exists( board_maint_data_path	)  )	dir.mkdir( board_maint_data_path);
//	if( !dir.exists( bi_open_lot_data_path	)  )	dir.mkdir( bi_open_lot_data_path);

	m_sCurrentLoadDir = QDir::home().absolutePath();
}

void MainWindow::SetAutoMode(bool mode)
{
	m_bAutoMode = mode;
	m_stSetupInfo.auto_mode  = mode;			//2023.03.09 - KWMOON
	m_uiInitInfo.ui_automode = mode;			//2023.05.22 - KWMOON
	m_pHsmsManager->SET_UIAUTOMODE( mode );		//2024.07.23 - KWMOON

	m_pTestDataMgr->SET_DEBUGMODE( true );
	if( m_bAutoMode )
	{
		ui->tabAutoEngr->setCurrentIndex( RUN_TAB );
		m_uiTestInfo.ui_chamberuse = true;
		m_pTestDataMgr->SET_UIAUTOMODE( true );			//Only MainUI write uimode.ini
		m_pVerInfo->SetUiAutoMode( true );				//2024.08.16 - KWMOON
	}
	else
	{
		ui->tabAutoEngr->setCurrentIndex( ENGR_TAB );
		m_uiTestInfo.ui_chamberuse = false;
		m_pTestDataMgr->SET_UIAUTOMODE( false );		//Only MainUI write uimode.ini
		m_pVerInfo->SetUiAutoMode( false );				//2024.08.16 - KWMOON
	}

	m_pMassView->SetUiTestInfo( m_uiTestInfo );
	m_pEngrView->SetUiTestInfo( m_uiTestInfo );

	m_pTestDataMgr->LOAD_FILEMAP_INFO( m_sDataRoot.toStdString().c_str() );
	m_pEngrView->SetTestDataMgrPtr( m_pTestDataMgr );
	m_pMassView->SetTestDataMgrPtr( m_pTestDataMgr );
	m_pJumpDlg->SetTestDataMgrPtr ( m_pTestDataMgr );
	m_pSlotView->SetTestDataMgrPtr( m_pTestDataMgr );

	//2024.06.03 - KWMOON
	emit SigChmbMrStart(mode);
	emit SigLoadExistData();	//2024.06.20 - KWMOON
	SetWindowTitle("", true);
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_SYS_NAME, tr("%1").arg(m_bAutoMode?"AUTO MODE":"ENGR MODE"), false );
}

void MainWindow::InitVariable()
{
	m_bQuitAtTestMgr = false;
	m_nCurrentSlot = 0;
	m_pStatusBar = nullptr;
	m_pMassView = nullptr;
	m_pEngrView = nullptr;
	m_pSetupDlg = nullptr;
	m_pPrgDlg = nullptr;
	m_pFailRate = nullptr;
	m_pJumpDlg = nullptr;
	m_pOtDelay = nullptr;
	m_pMgrIpc = nullptr;
	m_pTmsIpc = nullptr;
	m_timerSlotCon = nullptr;
	m_timerMgrCon = nullptr;
	m_timerChmbCon = nullptr;
	m_timerPsMonCon = nullptr;
	m_timerPgmCon = nullptr;
	m_pShmChmb = nullptr;
	m_pErrPopup = nullptr;  //2023.03.22 - kwmoon
	m_bInitialized = false;
	m_bMgrConn = false;
	m_bTmsConn = false;
	m_pSaveDataDlg = nullptr;
	m_pChmbMgr = nullptr;
	m_pChmbMgrTh = nullptr;
	m_pHsmsManager = nullptr;
	m_pHsmsMgrTh = nullptr;
	m_pPgmDownloader = nullptr;
	m_pPgmDownloaderTh = nullptr;
	m_pLogView = nullptr;
	m_pSlotConnView = nullptr;
	m_sCurrentLotId = "";				//2024.07.02 - yseom
	m_uCorrespondingSlotBits = 0x00;	//2024.07.02 - yseom
	m_uValidSlotBit = 0x00;				//2024.07.24 - yseom

	memset( &m_stSetupInfo   , 0x0, sizeof(stSetupInfo)             );
	memset( &m_stSlotTestInfo, 0x0, sizeof(stSlotTestInfo)*MAX_SLOT );
	memset( &m_uiInitInfo    , 0x0, sizeof(stUiInitInfo)            );
	memset( &m_uiTestInfo    , 0x0, sizeof(stUiTestInfo)            );

	memset(this->m_bTestStart, false, sizeof(this->m_bTestStart));
	memset(this->m_bTestDone,  false, sizeof(this->m_bTestDone));

	ClearSlotActionDoneAck( ACTION_BIBID_READ );
	ClearSlotActionDoneAck( ACTION_LOAD );
	ClearSlotActionDoneAck( ACTION_UNLOAD );
	ClearSlotActionDoneAck( ACTION_INITTEST );
	ClearSlotActionDoneAck( ACTION_TEST );
	ClearSlotActionDoneAck( ACTION_STOP );
}

void MainWindow::InitUi()
{
	ui->setupUi(this);
	CreateToolbar();
	CreateStatusBar();
	CreateMassEngrTab();
	CreateDockWidget();
	setContextMenuPolicy ( Qt::NoContextMenu );
	//setCentralWidget( m_pTabWidget );
	SetToolBarStatus( UI_INIT );
	SetTestStatus( UI_INIT );
	LoadSysChmbIni();
}

void MainWindow::InitInstance()
{
	m_pMassView    = new CMassTab;
	m_pEngrView    = new CEngrTab;
	m_pSetupDlg    = new CSetup(this);
	m_pMgrIpc      = new CLocalSocket( "   MGRIPC" );
	m_pTmsIpc	   = new CLocalSocket( "   TMSIPC" );
	m_pStatusBar   = new CStatusBar( this );
	m_pPrgDlg      = new CProgressDlg( this );
	m_pFailRate    = new CFailRate( this );
	m_pJumpDlg     = new CJumpStepDlg( this );
	m_pOtDelay     = new COpenTimeDelay( this );
    m_pErrPopup    = new CErrorPopup( this );
	m_pHsmsManager = new CHsmsManager;
	m_pTestDataMgr = new CFileMap( m_sDataRoot.toStdString().c_str() );
	m_pTestDataMgr->SET_DEBUGMODE( true );
	m_pMgrIpc->SetHostInfo( IPC_INFO::IPC_TESTMGR_SOCK );
	m_pTmsIpc->SetHostInfo( IPC_INFO::IPC_TMS_SOCK );

	m_pSaveDataDlg = new CSaveDataDlg( this );
	m_pChmbMgr     = new CChamberMgr;
	m_pChmbMgrTh   = new QThread;
	m_pChmbMgr->moveToThread( m_pChmbMgrTh );
	m_pPgmDownloader     = new CDownloader;					//2024.06.11 - yseom
	m_pPgmDownloaderTh   = new QThread;						//2024.06.11 - yseom
	m_pPgmDownloader->moveToThread( m_pPgmDownloaderTh );	//2024.06.11 - yseom
	m_pHsmsMgrTh = new QThread;
	m_pHsmsManager->moveToThread(m_pHsmsMgrTh);

	m_pLogView = new CLogView();
	m_pSlotConnView = new CSlotConnView();
	m_pSlotView = new CSlotView();


	connect( m_pSetupDlg   , &CSetup::SigSetupDlgResult             , this       , &MainWindow::SlotSetupDlgResult          );
	connect( this          , &MainWindow::SigIpcMgrConnect          , m_pMgrIpc  , &CLocalSocket::SlotConnectToHost         );
	connect( this          , &MainWindow::SigIpcMgrSendPacket       , m_pMgrIpc  , &CLocalSocket::SlotSendPacket            );
	connect( m_pMgrIpc     , &CLocalSocket::SigIpcLogMessage        , this       , &MainWindow::SlotIpcMgrIpcLogMessage     );
	connect( m_pMgrIpc     , &CLocalSocket::SigIpcError             , this       , &MainWindow::SlotIpcMgrIpcError          );
	connect( m_pMgrIpc     , &CLocalSocket::SigIpcConnectionStatus  , this       , &MainWindow::SlotIpcMgrConnectionStatus  );
	connect( m_pMgrIpc     , &CLocalSocket::SigIpcRecvPacket        , this       , &MainWindow::SlotIpcMgrRecvPacket        );
	connect( this          , &MainWindow::SigIpcTmsConnect          , m_pTmsIpc  , &CLocalSocket::SlotConnectToHost         );
	connect( this          , &MainWindow::SigIpcTmsSendPacket       , m_pTmsIpc  , &CLocalSocket::SlotSendPacket            );
	connect( m_pTmsIpc     , &CLocalSocket::SigIpcLogMessage        , this       , &MainWindow::SlotIpcTmsIpcLogMessage     );
	connect( m_pTmsIpc     , &CLocalSocket::SigIpcError             , this       , &MainWindow::SlotIpcTmsIpcError          );
	connect( m_pTmsIpc     , &CLocalSocket::SigIpcConnectionStatus  , this       , &MainWindow::SlotIpcTmsConnectionStatus  );
	connect( this          , &MainWindow::SigSysLogMessage          , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.06.14 - KWMOON
	connect( this          , &MainWindow::SigChmbLogMessage         , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.06.14 - KWMOON
	connect( this          , &MainWindow::SigPgmLogMessage          , m_pEngrView , &CEngrTab::SlotPgmLogMessage            );
	connect( this          , &MainWindow::SigPgmLoad                , m_pEngrView , &CEngrTab::SlotPgmLoad                  );
	connect( this          , &MainWindow::SigPgmLoadDone            , m_pEngrView , &CEngrTab::SlotPgmLoadDone              );
	connect( this          , &MainWindow::SigMgrTmsStatusChanged    , m_pStatusBar, &CStatusBar::SlotStatusChanged          );
	connect( this          , &MainWindow::SigSlotSetupResult        , m_pMassView , &CMassTab::SlotSlotSetupResult          );
	connect( this          , &MainWindow::SigSlotSetupResult        , m_pEngrView , &CEngrTab::SlotSlotSetupResult          );
	connect( this          , &MainWindow::SigProgressDlgTitle       , m_pPrgDlg   , &CProgressDlg::SlotProgressDlgTitle     );
	connect( this          , &MainWindow::SigProgressDlgInit        , m_pPrgDlg   , &CProgressDlg::SlotProgressDlgInit      );
	connect( this          , &MainWindow::SigProgressDlgValue       , m_pPrgDlg   , &CProgressDlg::SlotProgressDlgValue     );
	connect( this          , &MainWindow::SigProgressDlgText        , m_pPrgDlg   , &CProgressDlg::SlotProgressDlgText      );
	connect( this          , &MainWindow::SigProgressDlgHide        , m_pPrgDlg   , &CProgressDlg::SlotProgressDlgHide      );
	connect( this          , &MainWindow::SigInitTestDone           , m_pEngrView , &CEngrTab::SlotInitTestDone             );
	connect( this          , &MainWindow::SigTestItemListup         , m_pEngrView , &CEngrTab::SlotTestItemListup           );
	connect( this          , &MainWindow::SigTestItemListup         , m_pJumpDlg  , &CJumpStepDlg::SlotTestItemListup       );
	connect( m_pJumpDlg    , &CJumpStepDlg::SigJumpLogMessage       , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.06.14 - KWMOON
	connect( m_pJumpDlg    , &CJumpStepDlg::SigJumpTest             , this        , &MainWindow::SlotJumpTest               );
	connect( m_pJumpDlg    , &CJumpStepDlg::SigJumpChamberUse       , this        , &MainWindow::SlotJumpChamberUse         );	//2023.05.22 - kwmoon

	connect( this          , &MainWindow::SigAppendErrorPopup       , m_pErrPopup , &CErrorPopup::SlotAppendErrorPopup      );
	connect( this          , &MainWindow::SigTestStartFlag          , m_pMassView , &CMassTab::SlotTestStartFlag            );
	connect( this          , &MainWindow::SigTestStartFlag          , m_pEngrView , &CEngrTab::SlotTestStartFlag            );
	connect( this          , &MainWindow::SigTestPgmVariableFlag    , m_pMassView , &CMassTab::SlotTestPgmVariableFlag      );
	connect( this          , &MainWindow::SigTestPgmVariableFlag    , m_pEngrView , &CEngrTab::SlotTestPgmVariableFlag      );
	connect( this          , &MainWindow::SigTestEndFlag            , m_pMassView , &CMassTab::SlotTestEndFlag              );
	connect( this          , &MainWindow::SigTestEndFlag            , m_pEngrView , &CEngrTab::SlotTestEndFlag              );
	connect( this          , &MainWindow::SigStepIn                 , m_pEngrView , &CEngrTab::SlotStepIn                   );
	connect( this          , &MainWindow::SigStepOut                , m_pMassView , &CMassTab::SlotStepOut                  );
	connect( this          , &MainWindow::SigStepOut                , m_pEngrView , &CEngrTab::SlotStepOut                  );

	connect( this          , &MainWindow::SigStartChmbPlot          , m_pMassView , &CMassTab::SlotStartChmbPlot            );   //2023.06.20 - kwmoon
	connect( this          , &MainWindow::SigStopChmbPlot           , m_pMassView , &CMassTab::SlotStopChmbPlot             );   //2023.06.20 - kwmoon
	connect( this          , &MainWindow::SigResetChmbPlot          , m_pMassView , &CMassTab::SlotResetChmbPlot            );   //2023.06.20 - kwmoon
	connect( m_pMassView   , &CMassTab::SigSysLogMessage            , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.06.14 - KWMOON

	connect( this          , &MainWindow::SigMakeReport             , m_pSaveDataDlg , &CSaveDataDlg::SlotMakeReport        );   //2023.07.23 - kwmoon
	connect( m_pSaveDataDlg, &CSaveDataDlg::SigMakeReportDone       , this           , &MainWindow::SlotMakeReportDone      );   //2023.07.23 - kwmoon
	connect( m_pSaveDataDlg, &CSaveDataDlg::SigSysLogMessage        , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.08.14 - KWMOON
	connect( this          , &MainWindow::SigInitTestDone           , m_pMassView    , &CMassTab::SlotInitTestStartFlag     );   //2023.09.21 - hsbae for start pfm when init test

	connect( m_pChmbMgrTh  , &QThread::finished                     , m_pChmbMgr    , &QObject::deleteLater                   );   //2024.06.03 - kwmoon
	connect( this          , &MainWindow::SigChmbMrStart            , m_pChmbMgr    , &CChamberMgr::SlotStartMgr              );   //2024.06.03 - kwmoon
	connect( m_pChmbMgr    , &CChamberMgr::SigToMainuiLogMessage    , m_pLogView  , &CLogView::SlotAppendSysLog               );	//2024.06.14 - KWMOON
	connect( m_pHsmsManager, &CHsmsManager::sigChmbCtrolToChmbMgr   , m_pChmbMgr    , &CChamberMgr::SlotFromMainuiChmbControl );   //2024.06.10 - kwmoon
	connect( m_pChmbMgr    , &CChamberMgr::SigToMainuiChmbControlRes, m_pHsmsManager, &CHsmsManager::slotFromChmbMgrResponse  );   //2024.06.10 - kwmoon
	connect( m_pChmbMgr    , &CChamberMgr::SigToMainuiChmbStatus    , m_pHsmsManager, &CHsmsManager::slotFromChmbMgrStatus    );   //2024.06.10 - kwmoon
    connect( this          , &MainWindow::sigChmbCtrolToChmbMgr     , m_pChmbMgr    , &CChamberMgr::SlotFromMainuiChmbControl );   //2024.07.19 - yseom

	connect( m_pPgmDownloaderTh, &QThread::finished                 , m_pPgmDownloader, &QObject::deleteLater                 );	//2024.06.11 - yseom
	connect( m_pPgmDownloader  , &CDownloader::SigDownloadProgress	, this            , &MainWindow::SlotDownloadProgress     );	//2024.06.11 - yseom
	connect( m_pPgmDownloader  , &CDownloader::SigDownloadErrorReport, this           , &MainWindow::SlotDownloadFtpError     );	//2024.06.13 - yseom
	connect( m_pPgmDownloader  , &CDownloader::SigDownloadResult	, this            , &MainWindow::SlotDownloadFtpResult    );	//2024.06.13 - yseom
	connect( this              , &MainWindow::SigDownloadPgmResult	, m_pHsmsManager  , &CHsmsManager::slotDownloadPgmResult  );	//2024.06.12 - yseom
	connect( this              , &MainWindow::SigDownloadPgm        , m_pPgmDownloader, &CDownloader::SlotDownloadFile        );	//2024.06.11 - yseom

	connect( m_pHsmsMgrTh  , &QThread::finished						, m_pHsmsManager, &QObject::deleteLater					);
	connect( m_pTmsIpc     , &CLocalSocket::SigIpcRecvPacket        , m_pHsmsManager, &CHsmsManager::slotRecvPacketFromTms	);
	connect( this		   , &MainWindow::SigSyncSlotTimeResult		, m_pHsmsManager, &CHsmsManager::slotSyncSlotTimeResult	);
	connect( m_pHsmsManager, &CHsmsManager::sigSyncSlotTime			, this			, &MainWindow::SlotSyncSlotTime			);
	connect( m_pHsmsManager, &CHsmsManager::sigSendToTmsUi			, m_pTmsIpc		, &CLocalSocket::SlotSendPacket			);
	connect( m_pHsmsManager, &CHsmsManager::sigSendRcmd				, m_pMassView	, &CMassTab::SlotRecvRcmd				);
	connect( m_pHsmsManager, &CHsmsManager::sigEapConnectStatus		, this			, &MainWindow::SlotRecvEapConnectStatus	);

	connect( m_pHsmsManager, &CHsmsManager::sigStartInitTest		, this			, &MainWindow::SlotActInitTest			);
	connect( m_pHsmsManager, &CHsmsManager::sigSysLogMessage        , m_pLogView  , &CLogView::SlotAppendSysLog             );	//2024.07.22 - KWMOON

	connect (this		   , &MainWindow::SigAutoInitTestStart		, m_pHsmsManager, &CHsmsManager::slotAutoInitTestStart);
	connect (this		   , &MainWindow::SigAutoInitTestDone		, m_pHsmsManager, &CHsmsManager::slotAutoInitTestDone);
    connect (this		   , &MainWindow::SigAutoStartTest  		, m_pHsmsManager, &CHsmsManager::slotAutoTestStart);
    connect (this		   , &MainWindow::SigAutoStartTestDone		, m_pHsmsManager, &CHsmsManager::slotAutoTestStartDone);
	//TEMP
//	connect( m_pHsmsManager, &CHsmsManager::sigPgmDownloadInAutoMode, this			, &MainWindow::SlotPgmDownloadAutoMode);

	connect( this          , &MainWindow::SigToSlotViewUpdateBibId  , m_pSlotView     , &CSlotView::SlotFromMainUiUpdateBibId     );  //2024.06.19 - kwmoon
	connect( this		   , &MainWindow::SigToSlotViewUpdateSetup	, m_pSlotView	  , &CSlotView::SlotFromMainUiUpdateSetup	  );  //2024.07.02 - hsbae
	connect( m_pSlotView   , &CSlotView::SigToMainUiCurrentSlot     , this            , &MainWindow::SlotFromSlotViewCurrentSlot  );  //2024.06.20 - kwmoon
	connect( m_pSlotView   , &CSlotView::SigToMainUiCurrentSlot     , m_pMassView     , &CMassTab::SlotFromSlotViewCurrentSlot    );  //2024.06.20 - kwmoon
	connect( m_pSlotView   , &CSlotView::SigToMainUiCurrentSlot     , m_pEngrView     , &CEngrTab::SlotFromSlotViewCurrentSlot    );  //2024.06.20 - kwmoon
	connect( this          , &MainWindow::SigSlotLoaderStatusChanged, m_pSlotConnView , &CSlotConnView::SlotLoaderStatusChanged   );  //2024.06.20 - KWMOON
	connect( this          , &MainWindow::SigSlotPgmStatusChanged   , m_pSlotConnView , &CSlotConnView::SlotPgmStatusChanged      );  //2024.06.20 - KWMOON
	connect( this          , &MainWindow::SigLoadExistData          , m_pSlotView     , &CSlotView::SlotFromMainUiLoadExistData   );  //2024.06.20 - kwmoon
	connect( this          , &MainWindow::SigLoadExistData          , m_pEngrView     , &CEngrTab::SlotFromMainUiLoadExistData    );  //2024.06.20 - kwmoon
	connect( this          , &MainWindow::SigLoadExistData          , m_pMassView     , &CMassTab::SlotFromMainUiLoadExistData    );  //2024.06.20 - kwmoon

	connect (this		   , &MainWindow::SigChmbTempMonSet		    , m_pHsmsManager  , &CHsmsManager::slotChamberTempMonSet      );
	connect (this		   , &MainWindow::SigChmbTempMonEnd		    , m_pHsmsManager  , &CHsmsManager::slotChamberTempMonEnd      );
	connect (this		   , &MainWindow::SigInitSlotData			, m_pSlotView     , &CSlotView::SlotInitSlotData		      );
	connect( m_pSlotView   , &CSlotView::SigSysLogMessage           , m_pLogView      , &CLogView::SlotAppendSysLog               );	//2024.08.19 - KWMOON
	connect (this		   , &MainWindow::SigEngrInitTestDone       , m_pSlotView     , &CSlotView::SlotFromMainUiInitTestDone    );	//2024.08.19 - KWMOON
	InitSharedMemory();

	emit SigSysLogMessage( _NORMAL_LOG, tr("%1").arg(ATMAIN_VERSION) );
	emit SigSysLogMessage( _NORMAL_LOG, tr("%1:%2").arg(ATDATA_DIR_ENV).arg(m_sDataDir) );

	m_pChmbMgrTh->start();			//2024.06.03 - kwmoon
	m_pPgmDownloaderTh->start();	//2024.06.11 - yseom
	m_pHsmsMgrTh->start();
}

//2023.06.19 - KWMOON
bool MainWindow::InitSharedMemory()
{
	m_pShmChmb = new QSharedMemory( SHM_KEY::SHM_MAIN_CHMB_KEY, this );
	//m_pShmChmb->detach();
	if( !m_pShmChmb->create( sizeof(stShmMainChmb), QSharedMemory::ReadWrite ) )
	{
		//emit SigSysLogMessage( _NORMAL_LOG,
		//					   tr("%1 create error : %2!").arg(SHM_KEY::SHM_MAIN_CHMB_KEY).arg(m_pShmChmb->errorString()) );
		//return false;
	}
	//emit SigSysLogMessage( _NORMAL_LOG, tr("%1 create %2!").arg(SHM_KEY::SHM_MAIN_CHMB_KEY).arg(sizeof(stShmMainChmb)) );
	SetSharedMemory( false, false );
	return true;
}

void MainWindow::ConnectRcmdSigSlot()
{
	connect( this,				&MainWindow::SigRcmdReply,					m_pHsmsManager,	&CHsmsManager::slotRcmdReply				);
	connect( m_pHsmsManager,	&CHsmsManager::sigAutoDiag,					this,			&MainWindow::SlotFromHsmsMgrAutoDiag					);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiCloseLot,				this,			&MainWindow::SlotFromHsmsMgrBiCloseLot					);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiInitialEnd,				this,			&MainWindow::SlotFromHsmsMgrBiInitialEnd				);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiInitialStart,			this,			&MainWindow::SlotFromHsmsMgrBiInitialStart				);
    connect( m_pHsmsManager,	&CHsmsManager::sigStartTest,                this,			&MainWindow::SlotActTestStart				);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiOpenLot,				this,			&MainWindow::SlotFromHsmsMgrBiOpenLot					);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiRecloseLot,				this,			&MainWindow::SlotFromHsmsMgrBiRecloseLot				);
	connect( m_pHsmsManager,	&CHsmsManager::sigBiTestTime,				this,			&MainWindow::SlotFromHsmsMgrBiTestTime					);
	connect( m_pHsmsManager,	&CHsmsManager::sigBoardMaintInfo,		    this,			&MainWindow::SlotFromHsmsMgrBoardMaintInfo       	    );
	connect( m_pHsmsManager,	&CHsmsManager::sigBoardStatusInfo,			this,			&MainWindow::SlotFromHsmsMgrBoardStatusInfo			);
	connect( m_pHsmsManager,	&CHsmsManager::sigBoardType,				this,			&MainWindow::SlotFromHsmsMgrBoardType					);
	connect( m_pHsmsManager,	&CHsmsManager::sigCheckInBoard,				this,			&MainWindow::SlotFromHsmsMgrCheckInBoard				);
	connect( m_pHsmsManager,	&CHsmsManager::sigCheckOutBoard,			this,			&MainWindow::SlotFromHsmsMgrCheckOutBoard				);
	connect( m_pHsmsManager,	&CHsmsManager::sigEqEffInfo,				this,			&MainWindow::SlotFromHsmsMgrEqEffInfo					);
	connect( m_pHsmsManager,	&CHsmsManager::sigHoldCheck,				this,			&MainWindow::SlotFromHsmsMgrHoldCheck					);
	connect( m_pHsmsManager,	&CHsmsManager::sigInterLockLimit,			this,			&MainWindow::SlotFromHsmsMgrInterLockLimit				);
	connect( m_pHsmsManager,	&CHsmsManager::sigReadyToLoad,				this,			&MainWindow::SlotFromHsmsMgrReadyToLoad				);
	connect( m_pHsmsManager,	&CHsmsManager::sigLotCall,					this,			&MainWindow::SlotFromHsmsMgrLotCall					);
	connect( m_pHsmsManager,	&CHsmsManager::sigLotSet,					this,			&MainWindow::SlotFromHsmsMgrLotSet						);
	connect( m_pHsmsManager,	&CHsmsManager::sigLotSetInfo,				this,			&MainWindow::SlotFromHsmsMgrLotSetInfo					);
	connect( m_pHsmsManager,	&CHsmsManager::sigManualMaskMap,			this,			&MainWindow::SlotFromHsmsMgrManualMaskMap				);
	connect( m_pHsmsManager,	&CHsmsManager::sigPmCheck,					this,			&MainWindow::SlotFromHsmsMgrPmCheck					);
	connect( m_pHsmsManager,	&CHsmsManager::sigRackSetInfo,				this,			&MainWindow::SlotFromHsmsMgrRackSetInfo				);
	connect( m_pHsmsManager,	&CHsmsManager::sigRecheckInBoard,			this,			&MainWindow::SlotFromHsmsMgrRecheckInBoard				);
	connect( m_pHsmsManager,	&CHsmsManager::sigAutoBypass,				this,			&MainWindow::SlotFromHsmsMgrAutoBypass					);
	connect( m_pHsmsManager,	&CHsmsManager::sigReplyAutoBypassEqpState,	this,			&MainWindow::SlotFromHsmsMgrReplyAutoBypassEqpState	);
	connect( m_pHsmsManager,	&CHsmsManager::sigRequestBoardMaskSet,		this,			&MainWindow::SlotFromHsmsMgrRequestBoardMaskSet		);
	connect( m_pHsmsManager,	&CHsmsManager::sigRequestRackSet,			this,			&MainWindow::SlotFromHsmsMgrRequestRackSet				);
	connect( m_pHsmsManager,	&CHsmsManager::sigRMVOU,					this,			&MainWindow::SlotFromHsmsMgrRMVOU						);
//	connect( m_pHsmsManager,	&CHsmsManager::sigSlotMaskInfo1,			this,			&MainWindow::SlotFromHsmsMgrSlotMaskInfo1				);
	connect( m_pHsmsManager,	&CHsmsManager::sigSlotMaskInfo1,			this,			&MainWindow::SlotAutoSetupReq				);
	connect( m_pHsmsManager,	&CHsmsManager::sigSlotMaskInfo2,			this,			&MainWindow::SlotFromHsmsMgrSlotMaskInfo2				);
	connect( m_pHsmsManager,	&CHsmsManager::sigStartBiCmd,				this,			&MainWindow::SlotFromHsmsMgrStartBiCmd					);
	connect( m_pHsmsManager,	&CHsmsManager::sigTGET,						this,			&MainWindow::SlotFromHsmsMgrTGET						);
//	connect( this,	&MainWindow::sigTGET,						this,			&MainWindow::SlotFromHsmsMgrTGET						);	//2024.07.02 - yseom for Test
	connect( m_pHsmsManager,	&CHsmsManager::sigTMVIN,					this,			&MainWindow::SlotFromHsmsMgrTMVIN						);
	connect( m_pHsmsManager,	&CHsmsManager::sigTMVOU,					this,			&MainWindow::SlotFromHsmsMgrTMVOU						);
	connect( m_pHsmsManager,	&CHsmsManager::sigVMVOU,					this,			&MainWindow::SlotFromHsmsMgrVMVOU						);
	connect( m_pHsmsManager,	&CHsmsManager::sigReadyToUnload,			this,			&MainWindow::SlotFromHsmsMgrReadyToUnload				);
	connect( m_pHsmsManager,	&CHsmsManager::sigError,					this,			&MainWindow::SlotFromHsmsMgrError						);
	connect( m_pHsmsManager,	&CHsmsManager::sigBibIdRead ,			    this,			&MainWindow::SlotFromHsmsMgrBibIdRead				    );

	connect(this, &MainWindow::SigSetRackIgnore          ,		m_pHsmsManager, &CHsmsManager::slotSetRackIgnore );
	connect(this, &MainWindow::SigReadyToLoad            ,		m_pHsmsManager, &CHsmsManager::slotReadyToLoad   );
	connect(this, &MainWindow::SigLoadComplete           ,		m_pHsmsManager, &CHsmsManager::slotLoadComplete  );
	connect(this, &MainWindow::SigToHsmsMgrBibIdReadDone ,		m_pHsmsManager, &CHsmsManager::slotBibIdReadDone );
	connect(this, &MainWindow::sigQueueMonitoring		 ,		m_pHsmsManager, &CHsmsManager::slotQueueMonitoring);
	connect(this, &MainWindow::sigInformSlotLiveToHsmsMgr,		m_pHsmsManager, &CHsmsManager::slotRecvSlotLiveStatus);
	connect(this, &MainWindow::SigToHsmsMgrAutoManualSetupDone,	m_pHsmsManager, &CHsmsManager::slotAutoManualSetupDone);
	connect(this, &MainWindow::SigMakeReportDone,				m_pHsmsManager, &CHsmsManager::slotMakeReportDone);
    //test
//    connect(this, &MainWindow::sigSetChamberDoorClose, m_pHsmsManager, &CHsmsManager::slotChamberDoorClose);
	connect(this, &MainWindow::sigTestAmb, m_pHsmsManager, &CHsmsManager::slotSetAmbAfterTest);
	connect( m_pHsmsManager,	&CHsmsManager::sigUpdateAutoInitResult,		m_pSlotView,	&CSlotView::SlotFromUiUpdateAutoInitResult				);  //2024.07.02 - hsbae
	connect( m_pHsmsManager,	&CHsmsManager::sigInitSlotData,				m_pSlotView,	&CSlotView::SlotInitSlotData							);
	connect( m_pHsmsManager,	&CHsmsManager::sigAppenErrorPopup,			m_pErrPopup,	&CErrorPopup::SlotAppendErrorPopup						);
	connect( m_pHsmsManager,	&CHsmsManager::sigToSlotViewUpdateSetup,	m_pSlotView	  , &CSlotView::SlotFromMainUiUpdateSetup	  );  //2024.07.02 - hsbae

}

//2023.06.19 - KWMOON
void MainWindow::SetSharedMemory(bool bUse, bool bTest)
{
	if( !m_pShmChmb->isAttached() )
	{
		if( !m_pShmChmb->attach( QSharedMemory::ReadWrite ) )
		{
			//emit SigSysLogMessage( _NORMAL_LOG, tr("%1 attach error!").arg(SHM_KEY::SHM_MAIN_CHMB_KEY) );
			return;
		}
	}

	m_pShmChmb->lock();
	stShmMainChmb ShmData;
	if( m_pShmChmb->size() > 0 )
	{
		stShmMainChmb* to = (stShmMainChmb*)m_pShmChmb->data();
		memcpy( &ShmData, to, sizeof(stShmMainChmb) );

		ShmData.bUse  = bUse;
		ShmData.bTest = bTest;
		memcpy( to, &ShmData, sizeof(stShmMainChmb) );
	}
	m_pShmChmb->unlock();
	//emit SigSysLogMessage( _NORMAL_LOG, tr("Chamber Use[%1] Test[%2]").arg(bUse).arg(bTest) );

	if( bUse && bTest )
	{
		emit SigStartChmbPlot();
	}
	else
	{
		emit SigStopChmbPlot();
	}
}

bool MainWindow::RemoveTempDatFile()
{
	//2023.06.20 - KWMOON
	SetSharedMemory( false, false );
	QThread::usleep( 500 );

	QString dat = tr("%1/%2").arg(m_sTempDatDir).arg(DAT_TEMPERATURE);
	QFile::remove( dat );
	if( 0 )
	{
		emit SigSysLogMessage( _NORMAL_LOG , tr("%1 remove!").arg(dat) );
	}
	return true;
}

void MainWindow::CreateMassEngrTab()
{
	ui->tabAutoEngr->clear();
	ui->tabAutoEngr->addTab( m_pMassView, QIcon(":/icon/icon/auto.ico")  , " RUN" );
	ui->tabAutoEngr->addTab( m_pEngrView, QIcon(":/icon/icon/manual.ico"), "ENGR" );
	ui->tabAutoEngr->setCurrentIndex( 0 );
	connect( ui->tabAutoEngr, &QTabWidget::currentChanged, this, &MainWindow::SlotRunManualTabChanged );
}

void MainWindow::CreateDockWidget()
{
	//Slot View
	QDockWidget *dockSlotView = new QDockWidget(tr("SLOT VIEW"), this);
	dockSlotView->setObjectName("dockSlotView");
	dockSlotView->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dockSlotView->setFloating( false );
	dockSlotView->setFeatures( QDockWidget::DockWidgetFeature::DockWidgetMovable  );
	dockSlotView->setWidget(m_pSlotView);
	dockSlotView->setMinimumWidth(150);
	addDockWidget(Qt::LeftDockWidgetArea, dockSlotView);

	//System Log
	QDockWidget *dockLogView = new QDockWidget(tr("LOG VIEW"), this);
	dockLogView->setObjectName("dockLogView");
	dockLogView->setAllowedAreas(Qt::BottomDockWidgetArea);
	dockLogView->setFloating( false );
	dockLogView->setFeatures( QDockWidget::DockWidgetFeature::DockWidgetMovable  );
	dockLogView->setWidget(m_pLogView);
	dockLogView->setMinimumHeight(50);
	addDockWidget(Qt::BottomDockWidgetArea, dockLogView);

	//Monitor Log
	QDockWidget *dockConnView = new QDockWidget(tr("SLOT CONNECTION"), this);
	dockConnView->setObjectName("dockConnView");
	dockConnView->setAllowedAreas(Qt::BottomDockWidgetArea);
	dockConnView->setFloating( false );
	dockConnView->setFeatures( QDockWidget::DockWidgetFeature::DockWidgetMovable  );
	dockConnView->setWidget(m_pSlotConnView);
	dockConnView->setMinimumHeight(50);
	addDockWidget(Qt::BottomDockWidgetArea, dockConnView);
}

void MainWindow::SlotRunManualTabChanged(int index)
{
	if( index == RUN_TAB )
	{
		m_pMassView->ResizeView();
	}
	else
	{
		m_pEngrView->ResizeView();
	}
}

void MainWindow::CreateToolbar()
{
	QToolBar *fileToolBar = addToolBar(tr("Toolbar"));
	fileToolBar->setFloatable(false);
	fileToolBar->setMovable(false);
	fileToolBar->setHidden(false);
	fileToolBar->setIconSize(QSize(24,24));
	fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	fileToolBar->setObjectName("ToolBar");
	//fileToolBar->setFont( QFont(fileToolBar->font().family(), 7) );

	//Setup
	const QIcon icoSetup = QIcon::fromTheme( tr("setup"), QIcon(":/icon/icon/settings.ico"));
	m_pActSetup = new QAction(icoSetup, tr("SETUP"), this);
	//m_pActSetup->setStatusTip(tr("Test Setup"));
	m_pActSetup->setToolTip(tr("Test Setup"));
	fileToolBar->addAction(m_pActSetup);

	//Load
	const QIcon icoLoad = QIcon::fromTheme( tr("load"), QIcon(":/icon/icon/load_balancer.ico"));
	m_pActLoad = new QAction(icoLoad, tr("LOAD"), this);
	//m_pActLoad->setStatusTip(tr("Load test program to site"));
	m_pActLoad->setToolTip(tr("Load test program"));
	fileToolBar->addAction(m_pActLoad);

	m_pLoadMenu = new QMenu;
	m_pLoadBtn  = new QToolButton;
	//Reload
	const QIcon icoReload = QIcon::fromTheme( tr("reload"), QIcon(":/icon/icon/reload0.ico"));
	m_pActReload = new QAction(icoReload, tr("RELOAD"), this);
	//m_pActReload->setStatusTip(tr("Reload test program to site"));
	m_pActReload->setToolTip(tr("Reload test program"));
	m_pLoadMenu->addAction(m_pActReload);

	//Reload
	const QIcon icoUnload = QIcon::fromTheme( tr("reload"), QIcon(":/icon/icon/unload.ico"));
	m_pActUnload = new QAction(icoUnload, tr("UNLOAD"), this);
	//m_pActReload->setStatusTip(tr("Reload test program to site"));
	m_pActUnload->setToolTip(tr("Reload test program"));
	m_pLoadMenu->addAction(m_pActUnload);

	m_pLoadBtn->setMenu( m_pLoadMenu );
	m_pLoadBtn->setDefaultAction( m_pActReload );
	m_pLoadBtn->setPopupMode( QToolButton::InstantPopup );
	m_pLoadBtn->setText("RELOAD");
	m_pLoadBtn->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextUnderIcon );
	fileToolBar->addWidget( m_pLoadBtn );

	//LotSet 2024.08.14 - KWMOON
	const QIcon icoLotset = QIcon::fromTheme( tr("reload"), QIcon(":/icon/icon/lotset.ico"));
	m_pActLotSet = new QAction(icoLotset, tr("LOT SET"), this);
	//m_pActReload->setStatusTip(tr("Lot set"));
	m_pActLotSet->setToolTip(tr("Lot set"));
	fileToolBar->addAction(m_pActLotSet);
	fileToolBar->addSeparator();


	//Init Teset
	const QIcon icoInitTest = QIcon::fromTheme( tr("PreTest"), QIcon(":/icon/icon/manual_run0.ico"));
	m_pActInitTest = new QAction(icoInitTest, tr("PRE TEST"), this);
	//m_pActInitTest->setStatusTip(tr("Bib contact test"));
	m_pActInitTest->setToolTip(tr("Bib contact test"));
	fileToolBar->addAction(m_pActInitTest);

	//Test
	const QIcon icoTest = QIcon::fromTheme( tr("manual-test"), QIcon(":/icon/icon/run.ico"));
	m_pActTest = new QAction(icoTest, tr("TEST"), this);
	//m_pActTest->setStatusTip(tr("All item test"));
	m_pActTest->setToolTip(tr("All item test"));
	fileToolBar->addAction(m_pActTest);

	//Stop
	const QIcon icoStop = QIcon::fromTheme( tr("stop"), QIcon(":/icon/icon/stop_circle.ico"));
	m_pActStop = new QAction(icoStop, tr("STOP"), this);
	//m_pActStop->setStatusTip(tr("Stop test"));
	m_pActStop->setToolTip(tr("Stop test"));
	fileToolBar->addAction(m_pActStop);

	//Jump
	const QIcon icoJump = QIcon::fromTheme( tr("Jump"), QIcon(":/icon/icon/recursive.ico"));
	m_pActJump = new QAction(icoJump, tr("JUMP"), this);
	//m_pActSJump->setStatusTip(tr("Stop test"));
	m_pActJump->setToolTip(tr("Jump test"));
	fileToolBar->addAction(m_pActJump);
	fileToolBar->addSeparator();

	//Fail Rate
	const QIcon icoFailRate = QIcon::fromTheme( tr("FailRate"), QIcon(":/icon/icon/graph.ico"));
	m_pActFailRate = new QAction(icoFailRate, tr("FAIL RATE"), this);
	m_pActFailRate->setToolTip(tr("Fail Rate"));
	fileToolBar->addAction(m_pActFailRate);

	//OT Delay
	const QIcon icoOTDelay = QIcon::fromTheme( tr("otdelay"), QIcon(":/icon/icon/clock.ico"));
	m_pActOtDelay = new QAction(icoOTDelay, tr("O/T DELAY"), this);
	//m_pActOtDelay->setStatusTip(tr("Overtime Delay"));
	m_pActOtDelay->setToolTip(tr("Overtime Delay"));
	fileToolBar->addAction(m_pActOtDelay);

	//Judge
	const QIcon icoJudge = QIcon::fromTheme( tr("judge"), QIcon(":/icon/icon/judge0.ico"));
	m_pActJudge = new QAction(icoJudge, tr("JUDGE"), this);
	//m_pActJudge->setStatusTip(tr("Judge pass/fail result"));
	m_pActJudge->setToolTip(tr("Judge pass/fail result"));
	fileToolBar->addAction(m_pActJudge);
	fileToolBar->addSeparator();

	//Alarm
	const QIcon icoAlarm = QIcon::fromTheme( tr("alarm"), QIcon(":/icon/icon/alarm.ico"));
	m_pActAlarm = new QAction(icoAlarm, tr("ALARM"), this);
	//m_pActAlarm->setStatusTip(tr("System alarm list"));
	m_pActAlarm->setToolTip(tr("System alarm list"));
	fileToolBar->addAction(m_pActAlarm);

	//MAINT CALL
	const QIcon icoMCall = QIcon::fromTheme( tr("maint call"), QIcon(":/icon/icon/maintenance.png"));
	m_pActMCall = new QAction(icoMCall, tr("MCall"), this);
	//m_pActMCall->setStatusTip(tr("Maint call"));
	m_pActMCall->setToolTip(tr("Maint call"));
	fileToolBar->addAction(m_pActMCall);
	fileToolBar->addSeparator();

	//SAVE DATA
	const QIcon icoSaveData = QIcon::fromTheme( tr("save cdata"), QIcon(":/icon/icon/file_info.ico"));
	m_pActSaveData = new QAction(icoSaveData, tr("REPORT"), this);
	m_pActSaveData->setToolTip(tr("Save Data"));
	fileToolBar->addAction(m_pActSaveData);
	fileToolBar->addSeparator();

	//LOG VIEW
	const QIcon icoLogView = QIcon::fromTheme( tr("log view"), QIcon(":/icon/icon/log.ico"));
	m_pActLogView = new QAction(icoLogView, tr("LOG VIEW"), this);
	m_pActLogView->setToolTip(tr("Log View"));
	fileToolBar->addAction(m_pActLogView);
	fileToolBar->addSeparator();

	//yseom
//	const QIcon icoTemp = QIcon::fromTheme( tr("temporary"), QIcon(":/icon/icon/file_info.ico"));
//	m_pActTemp = new QAction(icoTemp, tr("TEMPORARY"), this);
//	m_pActTemp->setToolTip(tr("Temporary"));
//	fileToolBar->addAction(m_pActTemp);
//	fileToolBar->addSeparator();

    QCheckBox* rack_ignore_check_box = new QCheckBox("RACK IGNORE");
    fileToolBar->addWidget(rack_ignore_check_box);

    const QIcon icoReadyToLoad = QIcon::fromTheme( tr("READY_TO_LOAD"), QIcon(":/icon/icon/file_info.ico"));
    m_pActReadyToLoad = new QAction(icoReadyToLoad, tr("READY_TO_LOAD"), this);
    m_pActReadyToLoad->setToolTip(tr("READY_TO_LOAD"));
    fileToolBar->addAction(m_pActReadyToLoad);
    fileToolBar->addSeparator();

    const QIcon icoLoadComplete = QIcon::fromTheme( tr("LOAD_COMPLETE"), QIcon(":/icon/icon/file_info.ico"));
    m_pActLoadComplete = new QAction(icoLoadComplete, tr("LOAD_COMPLETE"), this);
    m_pActLoadComplete->setToolTip(tr("LOAD_COMPLETE"));
    fileToolBar->addAction(m_pActLoadComplete);
    fileToolBar->addSeparator();

	//empty
	QWidget* empty = new QWidget();
	empty->setStyleSheet("background-color: #2b579a;");
	empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	fileToolBar->addWidget(empty);

	//Hostname & Version
	m_pVerInfo = new CVersionInfo( this );
	fileToolBar->addWidget(m_pVerInfo);

	connect( m_pActSetup    , &QAction::triggered, this , &MainWindow::SlotActManualSetup  );
	connect( m_pActLoad     , &QAction::triggered, this , &MainWindow::SlotActLoadPgm      );
	connect( m_pActReload   , &QAction::triggered, this , &MainWindow::SlotActReloadPgm    );
	connect( m_pActUnload   , &QAction::triggered, this , &MainWindow::SlotActUnloadPgm    );
	connect( m_pActInitTest , &QAction::triggered, this , &MainWindow::SlotActInitTest     );
	connect( m_pActTest     , &QAction::triggered, this , &MainWindow::SlotActTestStart    );
	connect( m_pActStop     , &QAction::triggered, this , &MainWindow::SlotActTestStop     );
	connect( m_pActJump     , &QAction::triggered, this , &MainWindow::SlotActJump         );
	connect( m_pActFailRate , &QAction::triggered, this , &MainWindow::SlotActFailRate     );
	connect( m_pActOtDelay  , &QAction::triggered, this , &MainWindow::SlotActOTDelay      );
	connect( m_pActAlarm    , &QAction::triggered, this , &MainWindow::SlotActAlarm        );
	connect( m_pActJudge    , &QAction::triggered, this , &MainWindow::SlotActJudge        );
	connect( m_pActMCall    , &QAction::triggered, this , &MainWindow::SlotActMaintCall    );
	connect( m_pActSaveData , &QAction::triggered, this , &MainWindow::SlotActSaveData     );
	connect( m_pActLogView  , &QAction::triggered, this , &MainWindow::SlotActLogView      );
//	connect( m_pActTemp  , &QAction::triggered, this , &MainWindow::SlotActTemp      ); //yseom
    connect( rack_ignore_check_box, &QCheckBox::toggled, this, &MainWindow::SlotActSetRackIgnore);
//	connect( m_pActReadyToLoad, &QAction::triggered, [this, rack_ignore_check_box](){if (rack_ignore_check_box->isChecked()) emit SigReadyToLoad();});
	connect( m_pActLoadComplete, &QAction::triggered, [this, rack_ignore_check_box](){if (rack_ignore_check_box->isChecked()) emit SigLoadComplete();});
	connect( m_pActLotSet   , &QAction::triggered, this , &MainWindow::SlotActLotSet         );	//2024.08.14 - KWMOON
//    connect( m_pActReadyToLoad, &QAction::triggered, this, &MainWindow::slotTempInitTest);
//    connect( m_pActLoadComplete, &QAction::triggered, this, &MainWindow::slotTempTest);

	connect( m_pActReadyToLoad, &QAction::triggered, [this](){emit this->SigInitSlotData();});
//	connect( m_pActLoadComplete, &QAction::triggered, [this](){this->m_pSlotView->SlotFromUiUpdateAutoInitResult(0, 70, 100);});

	//2024.08.16 - KWMOON
	connect( m_pVerInfo, &CVersionInfo::SigInterruptMode, this, [this](bool interrupt){this->m_bInterruptMode = interrupt; this->m_pHsmsManager->SetInterruptMode(interrupt);} );

}

void MainWindow::slotTempInitTest()
{
    this->m_pHsmsManager->sigStartInitTest();
}

void MainWindow::slotTempTest()
{
//    emit sigSetChamberDoorClose();
//    this->m_pHsmsManager->SetChamberDoorClose();
}

//2024.08.14 - KWMOON
void MainWindow::SlotActLotSet()
{

}

void MainWindow::SetTestStatus(EN_UI_STATUS  stat)
{
	m_TestStatus = stat;
	//emit SigSysLogMessage( _NORMAL_LOG,	tr("[STAT] %1").arg(stat) );
}

EN_UI_STATUS MainWindow::GetTestStatus()
{
	return m_TestStatus;
}

void MainWindow::SetToolBarStatus(EN_UI_STATUS stat)
{
	switch ( stat )
	{
		case UI_INIT:
			m_pActSetup->setEnabled( true );
			m_pActLoad->setEnabled( false );
			m_pLoadBtn->setEnabled( false );
			if( m_bAutoMode &&  m_bEapConn  )
			{
				m_pActLotSet->setEnabled( true );
			}
			else
			{
				m_pActLotSet->setEnabled( false );
			}
			m_pActInitTest->setEnabled( false );
			m_pActTest->setEnabled( false );
			m_pActStop->setEnabled( false );
			m_pActJump->setEnabled( false );
			m_pActOtDelay->setEnabled( true );
			m_pActJudge->setEnabled( true );
			m_pActAlarm->setEnabled( true );
			m_pActMCall->setEnabled( false );
			break;
		case UI_SETUP_DONE:
			m_pActSetup->setEnabled( true );
			m_pActLoad->setEnabled( true );
			if( m_bAutoMode &&  m_bEapConn  )
			{
				m_pActLotSet->setEnabled( true );
			}
			else
			{
				m_pActLotSet->setEnabled( false );
			}
			m_pLoadBtn->setEnabled( false );
			m_pActInitTest->setEnabled( false );
			m_pActTest->setEnabled( false );
			m_pActStop->setEnabled( false );
			m_pActJump->setEnabled( false );
			m_pActOtDelay->setEnabled( true );
			m_pActJudge->setEnabled( true );
			m_pActAlarm->setEnabled( true );
			m_pActMCall->setEnabled( false );
			break;
		case UI_LOAD_DONE:
			m_pActSetup->setEnabled( true );
			m_pActLoad->setEnabled( true );
			m_pLoadBtn->setEnabled( true );
			if( m_bAutoMode &&  m_bEapConn  )
			{
				m_pActLotSet->setEnabled( true );
			}

			m_pActInitTest->setEnabled( true );
			m_pActTest->setEnabled( false );
			m_pActStop->setEnabled( false );
			m_pActJump->setEnabled( false );
			m_pActOtDelay->setEnabled( true );
			m_pActJudge->setEnabled( true );
			m_pActAlarm->setEnabled( true );
			m_pActMCall->setEnabled( false );
			break;
			break;
		case UI_INIT_TEST_DONE:
		case UI_TEST_DONE:
			m_pActSetup->setEnabled( true );
			m_pActLoad->setEnabled( true );
			m_pLoadBtn->setEnabled( true );
			if( m_bAutoMode &&  m_bEapConn  )
			{
				m_pActLotSet->setEnabled( true );
			}
			else
			{
				m_pActLotSet->setEnabled( false );
			}
			m_pActInitTest->setEnabled( true );
			m_pActTest->setEnabled( true );
			m_pActStop->setEnabled( false );
			m_pActJump->setEnabled( true );
			m_pActOtDelay->setEnabled( true );
			m_pActJudge->setEnabled( true );
			m_pActAlarm->setEnabled( true );
			m_pActMCall->setEnabled( false );
			break;
		case UI_TESTING:
			m_pActSetup->setEnabled( false );
			m_pActLoad->setEnabled( false );
			m_pLoadBtn->setEnabled( false );
			m_pActLotSet->setEnabled( false );
			m_pActInitTest->setEnabled( false );
			m_pActTest->setEnabled( false );
			m_pActStop->setEnabled( true );
			m_pActJump->setEnabled( false );
			m_pActOtDelay->setEnabled( true );
			m_pActJudge->setEnabled( true );
			m_pActAlarm->setEnabled( true );
			m_pActMCall->setEnabled( false );
			break;
		default:
			break;
	}
	m_UiStatus = stat;
}

void MainWindow::CreateStatusBar()
{
	int i=0;
	const int STRETCH[MAX_STATUS_LABEL] = { 4, 3, 2, 1 };
	const QString TOOLTIP[MAX_STATUS_LABEL] = { "STATUS INFO","STEP INFO","SYSTEM MODE","MGR VERSION" };
	for( i=0;i<MAX_STATUS_LABEL;i++ )
	{
		m_pStatusBarLabel[i] = new QLabel( tr("") );
		m_pStatusBarLabel[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		m_pStatusBarLabel[i]->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
		m_pStatusBarLabel[i]->setToolTip( TOOLTIP[i] );
		//m_pStatusBarLabel[i]->setMinimumWidth( WIDTH[i] );
		statusBar()->addPermanentWidget( m_pStatusBarLabel[i] , STRETCH[i] );
	}
	statusBar()->addPermanentWidget( m_pStatusBar, i+1 );
	statusBar()->setSizeGripEnabled( true );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS  , "", false );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STEP    , "", false );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_SYS_NAME, "", false );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_MGR_VER , "", false );
}

void MainWindow::SetStatusBarMessage( EN_STATUS_LABEL kind, QString msg, bool bError )
{
	QString sColor = PASS_STBAR_QCOLOR;
	if( bError ) sColor = FAIL_STBAR_QCOLOR;
	QString stylesheet = tr("QLabel { background-color : %1; color : black; }").arg(sColor);
	switch( kind )
	{
		case EN_STATUS_LABEL::STATUS_BAR_STATUS:
			m_pStatusBarLabel[STATUS_BAR_STATUS]->setText( msg );
			m_pStatusBarLabel[STATUS_BAR_STATUS]->setStyleSheet( stylesheet );
			break;
		case EN_STATUS_LABEL::STATUS_BAR_STEP:
			m_pStatusBarLabel[STATUS_BAR_STEP]->setText( msg );
			m_pStatusBarLabel[STATUS_BAR_STEP]->setStyleSheet( stylesheet );
			break;
		case EN_STATUS_LABEL::STATUS_BAR_SYS_NAME:
			m_pStatusBarLabel[STATUS_BAR_SYS_NAME]->setText( msg );
			m_pStatusBarLabel[STATUS_BAR_SYS_NAME]->setStyleSheet( stylesheet );
			break;
		case EN_STATUS_LABEL::STATUS_BAR_MGR_VER:
			m_pStatusBarLabel[STATUS_BAR_MGR_VER]->setText( msg );
			m_pStatusBarLabel[STATUS_BAR_MGR_VER]->setStyleSheet( stylesheet );
			break;
	default:
			break;
	}
}

void MainWindow::InitTimer()
{
	m_timerMgrCon   = new QTimer(this);
	m_timerSlotCon  = new QTimer(this);
	m_timerPgmCon   = new QTimer(this);
	m_timerTmsCon	= new QTimer(this);
	//m_timerPsMonCon = new QTimer(this);

	connect(m_timerMgrCon   , &QTimer::timeout, this, &MainWindow::SlotTimeoutMgrCon    );
	connect(m_timerSlotCon  , &QTimer::timeout, this, &MainWindow::SlotTimeoutSlotCon   );
	connect(m_timerPgmCon   , &QTimer::timeout, this, &MainWindow::SlotTimeoutPgmCon    );
	connect(m_timerTmsCon   , &QTimer::timeout, this, &MainWindow::SlotTimeoutTmsCon    );

	//connect(m_timerPsMonCon , &QTimer::timeout, this, &MainWindow::SlotTimeoutPsMonCon  );

	m_timerMgrCon->start ( TIMER_MGR_VALUE  );
	m_timerSlotCon->start( TIMER_SLOT_VALUE );

//	TEST
//	when AutoMode, try to connect IPC to TMS

//	if (m_bAutoMode)
		m_timerTmsCon->start ( TIMER_TMS_VALUE  );
}

void MainWindow::KillTimer()
{
	if( m_timerMgrCon )
	{
		if( m_timerMgrCon->isActive() )
		{
			m_timerMgrCon->stop();
		}
		delete m_timerMgrCon;
	}

	if( m_timerSlotCon )
	{
		if( m_timerSlotCon->isActive() )
		{
			m_timerSlotCon->stop();
		}
		delete m_timerSlotCon;
	}

	if( m_timerChmbCon )
	{
		if( m_timerChmbCon->isActive() )
		{
			m_timerChmbCon->stop();
		}
		delete m_timerChmbCon;
	}

	if( m_timerPsMonCon )
	{
		if( m_timerPsMonCon->isActive() )
		{
			m_timerPsMonCon->stop();
		}
		delete m_timerPsMonCon;
	}
}

void MainWindow::SlotTimeoutMgrCon()
{
	if( !m_bMgrConn )
	{
		m_pMgrIpc->SetHostInfo( IPC_INFO::IPC_TESTMGR_SOCK );
		emit SigIpcMgrConnect();
	}
}

void MainWindow::SlotTimeoutTmsCon()
{
	if( m_bAutoMode )		//2024.06.20  - KWMOON(AUTO MODE CHECK)
	{
		if( !m_bTmsConn )
		{
			m_pTmsIpc->SetHostInfo( IPC_INFO::IPC_TMS_SOCK );
			emit SigIpcTmsConnect();
		}
	}
}

//Loader Conn
void MainWindow::SlotTimeoutSlotCon()
{
	if( m_bMgrConn )
	{
		ReqLiveSlots();
	}
}

//Chamber Conn
void MainWindow::SlotTimeoutChmbCon()
{
	if( !m_bMgrConn )
	{
		m_pMgrIpc->SetHostInfo( IPC_INFO::IPC_TESTMGR_SOCK );
		emit SigIpcMgrConnect();
	}
}

//PsMon conn
void MainWindow::SlotTimeoutPsMonCon()
{

}

void MainWindow::SlotTimeoutPgmCon()
{
	if( !m_bMgrConn ) return;

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_LIVE_PGM_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	emit SigIpcMgrSendPacket( pBuf, size );
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	if (event->spontaneous()) return;
	if ( m_bInitialized ) return;

	m_bInitialized = true;
	m_pMassView->ResizeView();
	m_pEngrView->ResizeView();
	m_pSlotView->ResizeSlotView();
	m_pSlotConnView->ResizeView();
	InitTimer();
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "READY", false );
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent( event );

	m_pMassView->ResizeView();
	m_pEngrView->ResizeView();
	m_pSlotView->ResizeSlotView();
	m_pSlotConnView->ResizeView();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(event->type() == QEvent::Close)
	{
		QCheckBox* cbQuitAtTestMgr = new QCheckBox("Quit ATMgr");
		QMessageBox quitMsgBox;
		QAbstractButton* btnQuit = quitMsgBox.addButton(tr("Quit"), QMessageBox::YesRole);
		QAbstractButton* btnCancel = quitMsgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

		quitMsgBox.setText("Do you want to quit program?");
		quitMsgBox.setIcon(QMessageBox::Icon::Question);
		quitMsgBox.setCheckBox(cbQuitAtTestMgr);
		quitMsgBox.exec();

		if ( quitMsgBox.clickedButton() == btnQuit)
		{
			if( cbQuitAtTestMgr->isChecked() )
			{
				m_bQuitAtTestMgr = true;
			}
			else
			{
				m_bQuitAtTestMgr = false;
			}

			QSettings settings( CONFIG_INI_KEY, qApp->applicationName() );
			settings.setValue( "geometry"   , saveGeometry() );
			settings.setValue( "windowState", saveState()    );
			settings.sync();
			//qDebug() << settings.fileName();
			event->accept();
		}
		else if ( quitMsgBox.clickedButton() == btnCancel)
		{
			event->ignore();
		}
	}
}

void MainWindow::SlotActManualSetup()
{
	if ( m_bAutoMode )
    {
        //TODO
		//stRcmdStartBiCmd startBiCmd;
		//
		//memset((void*)&startBiCmd, 0x00, sizeof(stRcmdStartBiCmd));
		//
		//char* write_data = new char[sizeof(stRcmdStartBiCmd)];
		//memset(write_data, 0, sizeof(stRcmdStartBiCmd));
		//memcpy(write_data, (char*)&startBiCmd, sizeof(stRcmdStartBiCmd));
		//
		//char* env_data = getenv(ATDATA_DIR_ENV);
		//QString data_path("");
		//if( env_data != NULL )
		//    data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
		//else
		//    data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);
		//
		//QString start_bi_cmd_rcmd_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg("START_BI_CMD.rcmd"));
		//
		//m_pTestDataMgr->MAKE_FILE(start_bi_cmd_rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdStartBiCmd));

		//emit this->m_pHsmsManager->sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-IN COMPLETED"));
		//emit this->m_pHsmsManager->sigBibIdRead( GetAliveSlotBits() );
		//emit this->m_pHsmsManager->sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("BIB ID READ(0x%1) START").arg(GetAliveSlotBits(), 4, 16, QChar('0')) );

		//this->m_pHsmsManager->SetVidValue("911", "Y");
		//this->m_pHsmsManager->SetVidValue("912", "N");
		//this->m_pHsmsManager->SetVidValue("771", "Y");
		//this->m_pHsmsManager->SetVidValue("1090", "2024_07_25");
		//this->m_pHsmsManager->SetStatusId(_STATUS_ID::_RUN);
		//this->m_pHsmsManager->SetEventId(_EVENT_ID::_READY);

		//m_pActInitTest->setEnabled(true);	// for AUTO TEST
//		ReqLoaderBibId();
    }

	if( m_pSetupDlg )
	{
		m_pSetupDlg->SetAutoMode( m_bAutoMode );
		m_pSetupDlg->SetFocus();
		m_pSetupDlg->setModal( true );
		m_pSetupDlg->show();
	}
}

//from setup dialog
void MainWindow::SlotSetupDlgResult( bool mode, stSetupInfo setupinfo )
{
	memcpy( &m_stSetupInfo, &setupinfo, sizeof(stSetupInfo) );
	if( mode != m_bAutoMode )
	{
		QMessageBox::critical( this, "SETUP", tr("UI Mode(%1) SETUP Mode(%2) different!")
							   .arg(m_bAutoMode?"AUTO":"ENGR").arg(mode?"AUTO":"ENGR") );
		return;
	}

	//2024.08.14 - KWMOON(RFU) : When in automation mode, actions must be separated.
	if( m_bAutoMode )
	{
		ReqGetLoaderVersion ();		//request loader version & etc info
		ReqLoaderBibId();

		QTimer::singleShot( 3000, this, &MainWindow::DoManualSetup );
	}
	else
	{
		ReqGetLoaderVersion ();		//request loader version & etc info
		ReqLoaderBibId();

		QTimer::singleShot( 3000, this, &MainWindow::DoManualSetup );
	}
}

//2024.06.10 - KWMOON(from hsms manager)
void MainWindow::SlotAutoBibIdReq()
{
	if( !m_bAutoMode )
	{
		QMessageBox::critical( this, "SETUP", tr("UI Mode is not auto mode!") );
		return;
	}
	ReqGetLoaderVersion ();		//request loader version & etc info
	ReqLoaderBibId();
}

void MainWindow::SlotSetStatusBarText(EN_STATUS_LABEL lbl, QString msg )
{
	SetStatusBarMessage( lbl, msg, false );
}

bool MainWindow::DoProgressDlgSlotInit( QString title )
{
	bool bOk = true;
	emit SigProgressDlgInit();
	emit SigProgressDlgTitle( title );
	for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
	{
		if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_MASK )
		{
			emit SigProgressDlgValue( s, 0 );
			emit SigProgressDlgText ( s, STR_MASK, false );
		}
		else
		{
			if( m_stSlotLive.slot_status[s][CPU00] != EN_SLOT_ONLINE )
			{
				emit SigProgressDlgValue( s, 0 );
				emit SigProgressDlgText ( s, tr("SLOT OFFLINE"), true );
				bOk = false;
			}
			else
			{
				emit SigProgressDlgValue( s, 0 );
				emit SigProgressDlgText ( s, tr("SLOT ONLINE"), false );
			}
			//SLOT CPU 2EA Ignore
			if( 0 )
			{
				if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
				{
					if( m_stSlotLive.slot_status[s][CPU01] != EN_SLOT_ONLINE )
					{
						emit SigProgressDlgValue( s, 0 );
						emit SigProgressDlgText ( s, tr("CPU2 Loader offline"), true );
						bOk = false;
					}
					else
					{
						emit SigProgressDlgValue( s, 0 );
						emit SigProgressDlgText ( s, tr("CPU2 Loader online"), false );
					}
				}
			}
		}
	}
	return bOk;
}

void MainWindow::ShowProgressDlg()
{
	if( m_pPrgDlg )
	{
		m_pPrgDlg->resize( 500, 300 );
		m_pPrgDlg->setModal( true );
		m_pPrgDlg->show();
		m_pPrgDlg->ResizeView();
	}
}

bool MainWindow::DoCommonStup()
{
	RemoveTempDatFile();
	emit SigResetChmbPlot();
	if( !m_pMgrIpc->IsConn() )
	{
		QMessageBox::critical( this, "SETUP", tr("%1 is not connected!").arg(ATMGR_NAME) );
		return false;
	}
	return true;
}

void MainWindow::DoManualSetup()
{
	DoCommonStup();

	ClearSlotActionDoneAck( ACTION_SETUP );
	ShowProgressDlg();
	if( !DoProgressDlgSlotInit( "SETUP READY" ) )
	{
		emit SigProgressDlgTitle( "SETUP READY ERROR!" );
		return;
	}

	//2023.06.17 - KWMOON --> //2024.08.05 - KWMOON(Engr/Auto same procecure)
	ReqMgrEngrSetup();
}

//2024.06.28 - KWMOON(FROM HSMS MANAGER)
void MainWindow::SlotAutoSetupReq()
{
	if( !DoCommonStup() )
	{
		//ERROR
		return;
	}
	ReqMgrAutoSetup();
}

//2024.06.20 - kwmoon(not use)
/*
void MainWindow::DoManualSetupResult(uint slot)
{
	if( slot > MAX_SLOT-1 ) slot = MAX_SLOT-1;

	if( !m_stSlotTestInfo[slot].bSetup )
	{
		UpdateProgressDlg( slot, 100, "SETUP ERROR", false, true );
	}
	else
	{
		UpdateProgressDlg( slot, 100, "SETUP DONE", false, false );
	}
	bool bSetup   = m_stSlotTestInfo[slot].bSetup;
	int total     = m_stSlotTestInfo[slot].nTotalQty;
	int pass      = m_stSlotTestInfo[slot].nPassQty;
	QString bibid = QString(m_stSlotTestInfo[slot].bib_id.szBibId);
	QString lot   = m_stSlotTestInfo[slot].setup_info.lot_number;
	QString part  = m_stSlotTestInfo[slot].setup_info.part_number;

	if( bSetup )
		emit SigSlotSetupResult( slot, bSetup, bibid, total, pass, part, lot );

	bool bAbnormal = false;
	if( IsActionDone( ACTION_SETUP, bAbnormal) )
	{
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP DONE", false );
		if( IsLiveSlotBibCon() )
			SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );	//2023.08.04 - KWMOON
		else
			SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );	//2023.03.08 - KWMOON( UI_INIT -> UI_SETUP_DONE FOR TEST )
		emit SigProgressDlgHide();
	}
	else
	{
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP ERROR", true );
		SetToolBarStatus( EN_UI_STATUS::UI_INIT );
		//emit SigProgressDlgHide();
	}
}
*/

void MainWindow::UpdateProgressDlg( uint nBits, int nPercent, QString sMsg, bool isBit, bool isErr )
{
	if( isBit )
	{
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( (1<<s) & nBits )
			{
				emit SigProgressDlgValue( nBits, nPercent );
				emit SigProgressDlgText( nBits, sMsg, isErr );
			}
		}
	}
	else
	{
		emit SigProgressDlgValue( nBits, nPercent );
		emit SigProgressDlgText( nBits, sMsg, isErr );
	}
}

uint MainWindow::GetAliveSlotBits()
{
	uint nSlots = 0x0;
	for( int i=0;i<m_stSlotInfo.SlotCpu.nMaxSlot;i++ )
	{
		if( m_stSlotInfo.SlotIp[i].bMask ) continue;				//2023.08.04 - KWMOON
		if( m_stSlotLive.slot_status[i][CPU00] == EN_SLOT_ONLINE )
		{
			nSlots |= (1<<i);
		}
		//cpu per slot is 1 fix.
		if( 0 )
		{
			if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
			{
				if( m_stSlotLive.slot_status[i][CPU01] == EN_SLOT_ONLINE )
				{
					nSlots |= (1<<(i+m_stSlotInfo.SlotCpu.nMaxSlot));
				}
			}
		}
	}
	return nSlots;
}

uint MainWindow::GetBiCmdSlots()
{
    char* env_data = getenv(ATDATA_DIR_ENV);
    QString data_path("");
    if( env_data != NULL )
        data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
    else
        data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

    QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(m_pTestDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH).arg(EVENT_INI));
    uint slot_with_bib = 0x0;
    QString hex_value;
    UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

    bool bOk = false;
    slot_with_bib = hex_value.toInt(&bOk, 16);

    return slot_with_bib;
}


void MainWindow::ReqGetLoaderVersion()
{
	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_VERSION_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.slot    = GetAliveSlotBits();
	emt.ipcHeader.dest    = PROTOCOL::_ATSLOTS;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );

	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG,
						tr("[SEND][%1] %2[0x%3]").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_VERSION_REQ)).arg(emt.ipcHeader.slot,8,16,QChar('0')) );
}


void MainWindow::InitSlotTestInfo(uint slot)
{
	memset( &m_stSlotTestInfo[slot], 0x0, sizeof(stSlotTestInfo) );
	for( uint s=0;s<MAX_SLOT;s++)
	{
		m_stSlotTestInfo[s].bib_id.bBibConn     = false;
		m_stSlotTestInfo[s].bib_id.bParityError = false;
	}
}

void MainWindow::ReqLoaderBibId()
{
	uint uSlots= GetAliveSlotBits();
	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_READ_BIBID_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATSLOTS;
	emt.ipcHeader.slot    = uSlots;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );

	for( uint s=0;s<MAX_SLOT;s++)
	{
		if( (1<<s) & uSlots	)
			InitSlotTestInfo( s );
	}
	m_pMassView->SetSlotTestInfo( m_stSlotTestInfo );
	m_pEngrView->SetSlotTestInfo( m_stSlotTestInfo );
	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG,
						tr("[SEND][%1] %2[0x%3]").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_READ_BIBID_REQ)).arg(emt.ipcHeader.slot,8,16,QChar('0')) );
}

void MainWindow::ReqLoaderBibIdAutoMode()
{

	bool bError = false;
	QString sErrMsg = "";
	uint uALiveSlotBits= GetAliveSlotBits();
	m_uCorrespondingSlotBits = m_uValidSlotBit;

	for (int i = 0; i < MAX_SLOT; i++)
	{
		if( (m_uCorrespondingSlotBits & (1 << i)) )
		{
			if ( !(uALiveSlotBits & (1 << i)) )
			{
				bError = true;
				sErrMsg = tr("BIBID READ SLOT%1 IS NOT ALIVE").arg(i+1);
				break;
			}
		}
	}

	if ( bError )
	{
		emit SigSysLogMessage( LOG_TYPE::_ERROR_LOG, sErrMsg );
	}

	ClearSlotActionDoneAck( ACTION_BIBID_READ );		//2023.07.23 - kwmoon

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_READ_BIBID_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATSLOTS;
	emt.ipcHeader.slot    = m_uCorrespondingSlotBits;
	emt.ipcHeader.datalen = 0;

	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );

	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG,
						   tr("[SEND][%1] %2[0x%3]").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_READ_BIBID_REQ)).arg(emt.ipcHeader.slot,8,16,QChar('0')) );
}

void MainWindow::ReqMgrVersion()
{
	if( !m_bMgrConn ) return;

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_VERSION_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG, tr("[SEND][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_VERSION_REQ)) );
	//pBuf delete by localsocket
}

//2024.06.17 - KWMOON(ENGR SETUP)
void MainWindow::ReqMgrEngrSetup()
{
	if( !m_bMgrConn )
	{
		QMessageBox::critical( this, "ENGR SETUP", tr("TEST MGR is not connected!") );
		return;
	}

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_ENGR_SETUP_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = sizeof(stSetupInfo);

	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &m_stSetupInfo, sizeof(stSetupInfo) );

	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG,
						   tr("[SEND][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_ENGR_SETUP_REQ)) );
}

//2024.06.17 - KWMOON(ENGR SETUP)
void MainWindow::ReqMgrAutoSetup()
{
	if( !m_bMgrConn )
	{
		QMessageBox::critical( this, "AUTO SETUP", tr("TEST MGR is not connected!") );
		return;
	}

    uint slot_with_bib = GetBiCmdSlots();

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_AUTO_SETUP_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = sizeof(slot_with_bib);
	emt.pData             = nullptr;

	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &slot_with_bib, sizeof(slot_with_bib) );

	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG,
						   tr("[SEND][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_AUTO_SETUP_REQ)) );
}

/*
//2024.06.17 - KWMOON(not use)
void MainWindow::ReqMgrTestDataInit()
{
	if( !m_bMgrConn ) return;

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_TEST_DATA_INIT_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = sizeof(stSetupInfo);
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &m_stSetupInfo, sizeof(stSetupInfo) );
	emit SigIpcMgrSendPacket( pBuf, size );
	if( m_bAutoMode )
	{
		emit SigSysLogMessage( _NORMAL_LOG,
							   tr("[SEND][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_TEST_DATA_INIT_REQ)) );
	}
	else
	{
		emit SigSysLogMessage( _NORMAL_LOG,
							   tr("[SEND][%1] %2 LOTNO:%3 PARTNO:%4").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_TEST_DATA_INIT_REQ)).arg(m_stSetupInfo.lot_number).arg(m_stSetupInfo.part_number) );
	}
}
*/

void MainWindow::ReqInfoSlots()
{
	if( !m_bMgrConn ) return;

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_SLOT_INFO_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	emit SigIpcMgrSendPacket( pBuf, size );
	emit SigSysLogMessage( _NORMAL_LOG, tr("[SEND][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_SLOT_INFO_REQ)) );
	//pBuf delete by localsocket
}

void MainWindow::ReqLiveSlots()
{
	if( !m_bMgrConn ) return;

	stIpcElement emt;
	emt.ipcHeader.cmd     = PROTOCOL::CMD_LIVE_SLOT_REQ;
	emt.ipcHeader.src     = PROTOCOL::_ATMAIN_UI;
	emt.ipcHeader.dest    = PROTOCOL::_ATMGR;
	emt.ipcHeader.datalen = 0;
	uint size  = sizeof(stIpcHeader) + emt.ipcHeader.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&emt, sizeof(stIpcHeader) );
	emit SigIpcMgrSendPacket( pBuf, size );
}

//PGM live check
void MainWindow::ReqLivePgms( bool bReq )
{
	if( !m_bMgrConn ) return;
	if( !bReq )
	{
		if( m_timerPgmCon->isActive() )
			m_timerPgmCon->stop();
	}
	else
	{
		m_timerPgmCon->start(TIMER_PGM_VALUE);
	}
}

//atmgr - atsiteinfo
void MainWindow::AckInfoSlots( stIpcElement element )
{
	PROTOCOL::TCP_COMMAND cmd;
	cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	if( element.ipcHeader.datalen == sizeof(m_stSlotInfo) )
	{
		memcpy( &m_stSlotInfo, element.pData, element.ipcHeader.datalen );
		m_pSlotView->SetSlotInfo( m_stSlotInfo );	//2024.06.19 - KWMOON
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( m_stSlotInfo.SlotIp[s].bMask )
			{
				//emit SigLogMessage( LOG_SYS, _NORMAL_LOG, tr("[INFO]SLOT%1 MASK!").arg(s) );
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_MASK );
			}
			else
			{
				//emit SigLogMessage( LOG_SYS, _NORMAL_LOG, tr("[INFO]SLOT%1 OFFLINE!").arg(s) );
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
			}
		}
		//2023.03.17 - kwmoon
		ClearSlotActionDoneAck( ACTION_LOAD );
		ClearSlotActionDoneAck( ACTION_UNLOAD );
		ClearSlotActionDoneAck( ACTION_INITTEST );
		ClearSlotActionDoneAck( ACTION_TEST );
		ClearSlotActionDoneAck( ACTION_STOP );
	}
	else
	{
		emit SigSysLogMessage( _ERROR_LOG,
							tr("[RECV][%1] %2 data length error!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd) ) );
	}
}

//atloader - connection info
void MainWindow::AckLiveSlots( stIpcElement element )
{
	int enable_slots = 0x0;
	int online_slots = 0x0;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	if( element.ipcHeader.datalen == sizeof(stSlotLive) )
	{
		memcpy( &m_stSlotLive, element.pData, element.ipcHeader.datalen );
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_OFFLINE )
			{
				enable_slots |= (1<<s);
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
			}
			else if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_ONLINE )
			{
				enable_slots |= (1<<s);
				online_slots |= (1<<s);
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_ONLINE );
			}
			else
			{
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_MASK );
			}
		}
		m_pMassView->SetSlotLive( m_stSlotLive );
		m_pEngrView->SetSlotLive( m_stSlotLive );
		m_pSlotView->SetSlotLive( m_stSlotLive );	//2024.06.19 - KWMOON
		emit sigInformSlotLiveToHsmsMgr(m_stSlotLive);
	}
	else
	{
		emit SigSysLogMessage( _ERROR_LOG,
							tr("[RECV][%1] %2 data length error!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd) ) );
	}
	if( enable_slots == online_slots )
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_SLOT, true );
	else
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_SLOT, false );
}

//atloader - testmgr connection info
void MainWindow::AckLivePgms( stIpcElement element )
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	if( element.ipcHeader.datalen == sizeof(stPgmLive) )
	{
		memcpy( &m_stPgmLive, element.pData, element.ipcHeader.datalen );
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			EN_SLOT_STAT stat = EN_SLOT_MASK;
			if( m_stSlotInfo.SlotIp[s].bMask ) continue;

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN0][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN0, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, stat );

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN0][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN0, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, stat );

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN0][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN0, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET, stat );

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN1][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN1, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, stat );

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN1][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN1, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, stat );

			stat = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN1][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET] );
			emit SigSlotPgmStatusChanged( s, PROTOCOL::EN_PGM_RUN1, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET, stat );
		}
		m_pMassView->SetPgmLive( m_stPgmLive );
		m_pEngrView->SetPgmLive( m_stPgmLive );
	}
	else
	{
		emit SigSysLogMessage( _ERROR_LOG,
							tr("[RECV][%1] %2 data length error!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd) ) );
	}
}

//2024.06.17 - KWMOON(not use)
/*
void MainWindow::AckMgrTestDataInit(stIpcElement element)
{
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;
	QString msg = "";
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	LOG_TYPE lt = _NORMAL_LOG;
	if( error )
	{
		lt  = _ERROR_LOG;
		msg = tr("%1 directory check!").arg(m_sDataDir);
		//SetToolBarStatus( EN_UI_STATUS::UI_INIT );			//2023.03.08 - KWMOON( UI_INIT -> UI_SETUP_DONE FOR TEST )
		//SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP ERROR", true );
	}
	else
	{
		//bool bAbnormal = false;
		//if( IsActionDone( ACTION_SETUP, bAbnormal) )
		//{
		//	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP DONE", false );
		//	emit SigProgressDlgHide();
		//}
		//else
		//{
		//	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP ERROR", true );
		//}
		//SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );	//2023.03.08 - KWMOON( UI_INIT -> UI_SETUP_DONE FOR TEST )
	}
	emit SigSysLogMessage( lt,
					tr("[RECV][%1] %2 %3").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(msg) );

}
*/

//2023.08.17 - KWMOON
void MainWindow::AckMgrUiChamberUse(stIpcElement element)
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	int datalen= element.ipcHeader.datalen;
	char cCon  = 0;

	//2024.08.14 - KWMOON modify
	if( datalen == sizeof(char) )
	{
		memcpy( &cCon, element.pData, datalen );
	}
	bool bCon = false;
	if( cCon == 0x1 ) bCon = true;
	if( m_uiTestInfo.ui_chamberuse )
	{
		if( !bCon )
		{
			emit SigSysLogMessage( _ERROR_LOG,
							tr("[ ERR] %1 chamber connection error!").arg(ATMGR_NAME) );
			//emit SigAppendErrorPopup(tr("[ ERR] Ui <-> TestMgr chamber connection error!") );
		}
		else
		{
			//emit SigSysLogMessage( _NORMAL_LOG,
			//				tr("[RECV][%1] %2 Ui chamber connection done.").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)) );
		}
	}
}

//2024.06.17 - KWMOON
void MainWindow::AckReadBibId(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	SetSlotActionAck( ACTION_BIBID_READ, slot, CPU00, error );
	SetSlotActionAck( ACTION_BIBID_READ, slot, CPU01, error );

	stBibId bibid;
	memset( &bibid, 0x0, sizeof(stBibId) );
	memset( &m_stSlotTestInfo[slot].bib_id, 0x0, sizeof(stBibId) );
	if( nLen == sizeof(stBibId) )
	{
		memcpy( &bibid, element.pData, nLen );

		if( error == EN_BIBID_IO_ERR )
		{
			m_stSlotTestInfo[slot].bBibId = false;
			m_stSlotTestInfo[slot].bib_id.bBibConn     = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;
			m_stSlotTestInfo[slot].bib_id.nBibId       = bibid.nBibId;
			snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
		}
		else if( error == EN_BIBID_PARITY )
		{
			//2023.07.06 - kwmoon( load board setup pass )
			QString strBibId = QString(bibid.szBibId);
			if( strBibId == "086412945" )		//CAL BOARD
			{
				m_stSlotTestInfo[slot].bBibId = true;
				m_stSlotTestInfo[slot].bib_id.bParityError = false;					//2023.08.18 - KWMOON
			}
			else
			{
				m_stSlotTestInfo[slot].bBibId = false;
				m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;	//2023.08.18 - KWMOON
				emit SigSysLogMessage( _ERROR_LOG,
								tr("[RECV][SLOT%1-%2] BIBID:%3").arg(slot+1).arg(runner+1).arg(m_stSlotTestInfo[slot].bib_id.szBibId) );
			}
			m_stSlotTestInfo[slot].bib_id.bBibConn = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.nBibId    = bibid.nBibId;
			snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
		}
		else
		{
			m_stSlotTestInfo[slot].bib_id.nBibId = bibid.nBibId;
			m_stSlotTestInfo[slot].bib_id.bBibConn = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;
			if( bibid.bBibConn )
			{
				m_stSlotTestInfo[slot].bBibId = true;
				snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
			}
			else
			{
				snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", "Empty" );
				if( m_uiTestInfo.empty_bib_test )
				{
					m_stSlotTestInfo[slot].bBibId = true;
				}
				else
				{
					m_stSlotTestInfo[slot].bBibId = false;
				}
			}
		}
		LOG_TYPE _LOG_TYPE = _NORMAL_LOG;
		if( !m_stSlotTestInfo[slot].bBibId ) _LOG_TYPE = _ERROR_LOG;

		emit SigSysLogMessage( _LOG_TYPE,
						tr("[RECV][SLOT%1-%2] BIBID:%3").arg(slot+1).arg(runner+1).arg(m_stSlotTestInfo[slot].bib_id.szBibId) );
	}
	else
	{
		snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", "RECV ERR" );
		emit SigSysLogMessage( _ERROR_LOG,
						tr("[RECV][SLOT%1-%2] BIBID:%3").arg(slot+1).arg(runner+1).arg(m_stSlotTestInfo[slot].bib_id.szBibId) );
	}
	emit SigToSlotViewUpdateBibId( slot );

	if ( m_bAutoMode )
	{
//		emit SigInformResultOfReadBib(slot, m_stSlotTestInfo[slot].bBibId);
		bool bAbnormal = false;
		if( IsActionDone( ACTION_BIBID_READ, bAbnormal ) )
		{
			emit SigToHsmsMgrBibIdReadDone();
		}
	}

	// hsbae 20240819
	else
	{
		bool bAbnormal = false;
		if( IsActionDone( ACTION_BIBID_READ, bAbnormal ) )
		{
			uint valid_slot = 0x0;
			for (int slot = 0; slot< MAX_SLOT; slot += 1)
			{
				if (m_stSlotTestInfo[slot].bBibId)
					valid_slot |= (0b1 << slot);
			}

			char* env_data = getenv(ATDATA_DIR_ENV);
			QString data_path("");
			if( env_data != NULL )
				data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
			else
				data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

			QString slot_with_bib(QString("0x%1").arg(valid_slot, 4, 16, QChar('0')).toUpper());
			QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(m_pTestDataMgr->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH).arg(EVENT_INI));
			UTIL::INI::writeSettings(ini_file, KEY_SLOT_WITH_BIB, slot_with_bib);

		}
	}
}

//2024.06.17 - KWMOON
void MainWindow::AckEngrSetup(stIpcElement element)
{
	int cmd    = element.ipcHeader.cmd;
	int error  = element.ipcHeader.error;

	//emit SigToSlotViewSetupDone( slot, error?false:true );
	PROTOCOL::TCP_COMMAND tcp_cmd = static_cast<PROTOCOL::TCP_COMMAND>(cmd);
	if( error )
	{
		//SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP ERROR", true );
		emit SigSysLogMessage( _ERROR_LOG, tr("[RECV][%1] %2 : ERROR!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(tcp_cmd)));
	}
	else
	{
		//SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP DONE", false );
		emit SigSysLogMessage( _NORMAL_LOG, tr("[RECV][%1] %2 : DONE!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(tcp_cmd)) );
		emit SigProgressDlgHide();
		emit SigToSlotViewUpdateSetup();
		SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
	}

	//2024.08.05 - KWMOON( AUTO MODE ENGR SETUP DONE PROCESS )
	if( m_bAutoMode )
	{
		stRcmdStartBiCmd startBiCmd;

		memset((void*)&startBiCmd, 0x00, sizeof(stRcmdStartBiCmd));

		char* env_data = getenv(ATDATA_DIR_ENV);
		QString data_path("");
		if( env_data != NULL )
			data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
		else
			data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

		int slot_bits = 0;
		QString start_bi_cmd_rcmd_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg("START_BI_CMD.rcmd"));
		if( !m_pTestDataMgr->READ_FILE( start_bi_cmd_rcmd_file.toStdString().c_str(), (char*)&startBiCmd, sizeof(stRcmdStartBiCmd) ) )
		{
			emit SigToHsmsMgrAutoManualSetupDone( error?false:true, slot_bits );
			emit SigSysLogMessage( _ERROR_LOG, tr("[ ERR] MAINUI START_BI_CMD read error!") );
		}
		else
		{
			for( uint slot=0;slot<MAX_SLOT;slot++ )
			{
				stBibId bibid;
				if( m_pTestDataMgr->GET_BIBID( slot, bibid ) )
				{
					if( bibid.bBibConn && !bibid.bParityError )
					{
						slot_bits |= (1<<slot);
					}
				}
			}
			emit SigToHsmsMgrAutoManualSetupDone( error?false:true, slot_bits );
			emit SigSysLogMessage( _NORMAL_LOG, tr("[INFO] BIBID slot : %1").arg(slot_bits, 4, 16, QChar('0')) );
		}
	}
//	emit SigSetupView();
}

//2024.06.17 - KWMOON
void MainWindow::AckAutoSetup(stIpcElement element)
{
	int cmd    = element.ipcHeader.cmd;
	int error  = element.ipcHeader.error;

	//emit SigToSlotViewSetupDone( slot, error?false:true );
	PROTOCOL::TCP_COMMAND tcp_cmd = static_cast<PROTOCOL::TCP_COMMAND>(cmd);
	if( error )
	{
		//SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP ERROR", true );
		emit SigSysLogMessage( _ERROR_LOG, tr("[RECV][%1] %2 : ERROR!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(tcp_cmd)));
		emit SigRcmdReply(false);
	}
	else
	{
		//SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, "SETUP DONE", false );
		emit SigSysLogMessage( _NORMAL_LOG, tr("[RECV][%1] %2 : DONE!").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(tcp_cmd)) );
		emit SigProgressDlgHide();
		emit SigToSlotViewUpdateSetup();
		emit SigRcmdReply(true);
		//SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
	}
    emit SigSetupView();
//    emit SigToSlotViewUpdateSetup();
}

//2024.06.17 - KWMOON(not use)
/*
void MainWindow::AckSetupBibId(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	//LOG_TAB LOGTAB = static_cast<LOG_TAB>(LOG_SLOT1 + slot);
	if( nLen > 0 )
	{
		stBibId bibid;
		memcpy( &bibid, element.pData, nLen );
		emit SigSysLogMessage( _NORMAL_LOG,
						tr("[RECV][SLOT%1-%2] BIBID:%3[0x%4] error_CODE:%5").arg(slot+1).arg(runner+1).arg(bibid.szBibId).arg(bibid.nBibId, 8, 16, QChar('0')).arg(error) );
		emit SigProgressDlgValue( slot, 100 );

		//2024.06.14 - kwmoon
		if( m_bAutoMode )
		{
			SetSlotActionAck( ACTION_BIBID_READ, slot, CPU00, error );
			SetSlotActionAck( ACTION_BIBID_READ, slot, CPU01, error );
		}
		else
		{
			SetSlotActionAck( ACTION_SETUP, slot, CPU00, error );
			SetSlotActionAck( ACTION_SETUP, slot, CPU01, error );
		}

		memcpy( &m_stSlotTestInfo[slot].setup_info, &m_stSetupInfo, sizeof(stSetupInfo) );

		if( error == EN_BIBID_IO_ERR )
		{
			m_stSlotTestInfo[slot].bSetup = false;
			m_stSlotTestInfo[slot].bib_id.bBibConn = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;
			m_stSlotTestInfo[slot].bib_id.nBibId = bibid.nBibId;
			snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
			snprintf( m_stSlotTestInfo[slot].setup_info.part_number, sizeof(m_stSlotTestInfo[slot].setup_info.part_number), "%s", STR_BIBID_IO_ERR );
		}
		else if( error == EN_BIBID_PARITY )
		{
			//2023.07.06 - kwmoon( load board setup pass )
			QString strBibId = QString(bibid.szBibId);
			if( strBibId == "086412945" )		//CAL BOARD
			{
				m_stSlotTestInfo[slot].bSetup = true;
				m_stSlotTestInfo[slot].bib_id.bParityError = false;					//2023.08.18 - KWMOON
			}
			else
			{
				m_stSlotTestInfo[slot].bSetup = false;
				m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;	//2023.08.18 - KWMOON
			}
			m_stSlotTestInfo[slot].bib_id.bBibConn = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.nBibId = bibid.nBibId;
			snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
			//snprintf( m_stSetupSlots[slot].setup_info.part_number, sizeof(m_stSetupSlots[slot].setup_info.part_number), "%s", STR_BIBID_PARITY_ERR );
		}
		else
		{
			m_stSlotTestInfo[slot].bib_id.nBibId = bibid.nBibId;
			m_stSlotTestInfo[slot].bib_id.bBibConn = bibid.bBibConn;
			m_stSlotTestInfo[slot].bib_id.bParityError = bibid.bParityError;
			if( bibid.bBibConn )
			{
				m_stSlotTestInfo[slot].bSetup = true;
				snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", bibid.szBibId );
			}
			else
			{
				if( m_uiTestInfo.empty_bib_test )
					m_stSlotTestInfo[slot].bSetup = true;
				else
					m_stSlotTestInfo[slot].bSetup = false;
				snprintf( m_stSlotTestInfo[slot].bib_id.szBibId, sizeof(m_stSlotTestInfo[slot].bib_id.szBibId), "%s", "Empty" );
			}
		}
		DoManualSetupResult( slot );
	}
	else
	{
		emit SigProgressDlgText( slot, tr("Setup Error!"), true );
		emit SigSysLogMessage( _ERROR_LOG,
						tr("[RECV][SLOT%1-%2] %3 error ").arg(slot+1).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	}
}
*/


void MainWindow::AckPgmLoadStatus(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	if( nLen > 0 )
	{
		stPgmLoadStatus status;
		memcpy( &status, element.pData, nLen );
		emit SigProgressDlgValue( slot, status.nProgress );
		emit SigProgressDlgText( slot, status.szErrMsg, !status.bSuccess );
		//emit SigLogMessage( LOG_SYS, _NORMAL_LOG,
		//				tr("[RECV][SLOT%1-%2] %3[%4]").arg(slot+1).arg(runner+1).arg(status.szErrMsg).arg(error) );
	}
	else
	{
		emit SigProgressDlgText( slot, tr("Pgm load Error!"), true );
		emit SigProgressDlgText( slot, tr("ErrorCode:%1").arg(error), true );
		emit SigSysLogMessage( _ERROR_LOG,
						tr("[RECV][SLOT%1-%2] %3").arg(slot+1).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	}
}

void MainWindow::RecvPgmFinished(stIpcElement element)
{
	QFileInfo info(m_sLoadFile);
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	uint slot   = element.ipcHeader.slot;
	uint runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;
	bool killed = false;
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3:%4 finished!").arg(slot+1).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(info.fileName()) );
	emit SigAppendErrorPopup(tr("[SLOT%1-%2] %3 finished!")
							 .arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(info.fileName()) );

	//2023.06.02 - KWMOON( RUNTIME-ERROR )
	if( (GetTestStatus() == EN_UI_STATUS::UI_INIT_TESTING) )
	{
		killed = true;
		SetSlotActionAck( ACTION_INITTEST_DONE, slot, runner, error, killed );
		emit SigInitTestDone(slot);

		bool bAbnormal = false;
		if( IsActionDone( ACTION_INITTEST_DONE, bAbnormal ) )
		{
			emit SigTestItemListup(slot);
			SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("INIT TEST FAIL"), true );
			SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
			SetTestStatus( EN_UI_STATUS::UI_SETUP_DONE );
			emit SigSysLogMessage( _ERROR_LOG,	tr("[INFO][  ALLSLOT] %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );

			if( bAbnormal )
			{
				ReqUnloadPgm();
			}
		}
	}
	else if( GetTestStatus() == EN_UI_STATUS::UI_TESTING )
	{
		killed = true;
		SetSlotActionAck( ACTION_TEST_DONE, slot, runner, error, killed );

		bool bAbnormal = false;
		if( IsActionDone( ACTION_TEST_DONE, bAbnormal ) )
		{
			SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("TEST FAIL"), true );
			SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
			SetTestStatus( EN_UI_STATUS::UI_SETUP_DONE );
		}
	}
	else if( (GetTestStatus() == EN_UI_STATUS::UI_LOAD_DONE) )
	{
		killed = true;
		SetSlotActionAck( ACTION_LOAD, slot, runner, error, killed );

		bool bAbnormal = false;
		if( IsActionDone( ACTION_LOAD, bAbnormal ) )
		{
			SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("LOAD FAIL"), true );
			SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
			SetTestStatus( EN_UI_STATUS::UI_SETUP_DONE );
		}
	}
	else
	{

	}
}

//not used
void MainWindow::RecvLogPrint(stIpcElement element)
{
/*
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	uint slot   = element.ipcHeader.slot;
	uint runner = element.ipcHeader.runner;
	uint size   = element.ipcHeader.datalen;
	char* pData = new char[size+1];
	LOG_TAB tab = static_cast<LOG_TAB>(LOG_SLOT1 + slot);
	QString log = "";
	if( size > 0 )
	{
		memset( pData, 0x0, size+1 );
		memcpy( pData, element.pData, size );
		pData[size]='\0';
		log = QString(pData);
		//delete[] pData;
	}
	//emit SigSysLogMessage( _NORMAL_LOG,
	//				tr("[RECV][SLOT%1-%2] %3").arg(slot+1).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	//2023.08.10 - KWMOON(Comment - slot log save at atmgr )
	//emit SigPgmLogMessage( slot, runner, _NORMAL_LOG, tr("%1").arg(log) );
*/
}


void MainWindow::RecvPgmVariables( stIpcElement element )
{
	uint slot = element.ipcHeader.slot;
	emit SigTestPgmVariableFlag(slot);
}

void MainWindow::RecvPgmRuntimeError(stIpcElement element)
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	uint slot   = element.ipcHeader.slot;
	uint runner = element.ipcHeader.runner;
	uint size   = element.ipcHeader.datalen;
	char* pData = nullptr;
	//LOG_TAB tab = static_cast<LOG_TAB>(LOG_SLOT1 + slot);
	QString log = "";
	if( size > 0 )
	{
		pData = new char[size+1];
		memset( pData, 0x0, size+1 );
		memcpy( pData, element.pData, size );
		pData[size]='\0';
		log = QString(pData);
		delete[] pData;
	}
	emit SigAppendErrorPopup( tr("[ ERR][SLOT%1-%2] %3").arg(slot+1).arg(runner+1).arg(log) );
	emit SigSysLogMessage( _FAULT_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	emit SigPgmLogMessage( slot, runner, _FAULT_LOG, tr("%1").arg(log) );
}

void MainWindow::AckPgmLoadDone(stIpcElement element, bool bReload)
{
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	//SetActResponseSlot( slot );
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	//Ack from loader
	SetSlotActionAck( ACTION_LOAD, slot, CPU00, error );
	SetSlotActionAck( ACTION_LOAD, slot, CPU01, error );

	bool bAbnormal = false;
	if( IsActionDone( ACTION_LOAD, bAbnormal ) )
	{
		SetTestStatus( EN_UI_STATUS::UI_LOAD_DONE );
		SetToolBarStatus( EN_UI_STATUS::UI_LOAD_DONE );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("%1 PGM DONE").arg(bReload?"RELOAD":"LOAD"), false );

		emit SigProgressDlgHide();
		emit SigPgmLoadDone();

		//2023.06.05 - KWMOON
		if( bAbnormal)
		{
			ReqUnloadPgm();
			if( m_bAutoMode )
			{
				emit SigRcmdReply( false );
			}
		}
		else
		{
			//2024.08.02 - KWMOON( After all slot load ack receivced, wait 2000 ms ctrl socket check with m_uCorrespondingSlotBits )
			if( m_bAutoMode )
			{
				QTimer::singleShot( 2000, this, &MainWindow::AckAutoLoadDone );
			}
		}
		SetWindowTitle( m_sLoadFile, false);
	}
	else
	{
		SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("%1 PGM ERROR").arg(bReload?"RELOAD":"LOAD"), true );
	}
}

//2024.08.02 - KWMOON
void MainWindow::AckAutoLoadDone()
{
	bool bAutoLoaded = true;
	if ( m_bAutoMode )
	{
		for( int s=0;s<MAX_SLOT;s++ )
		{
			if( m_stSlotInfo.SlotIp[s].bMask ) continue;
			if( (s<<1) & m_uCorrespondingSlotBits )
			{
				EN_SLOT_STAT stat1 = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN0][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET] );
				EN_SLOT_STAT stat2 = static_cast<EN_SLOT_STAT>( m_stPgmLive.pgm_status[s][PROTOCOL::EN_PGM_RUN1][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET] );
				if( (stat1 != EN_SLOT_STAT::EN_SLOT_ONLINE) || (stat2 != EN_SLOT_STAT::EN_SLOT_ONLINE) )
				{
					bAutoLoaded = false;
				}
			}
		}
		emit SigRcmdReply( bAutoLoaded );
	}
}

void MainWindow::AckPgmUnloadDone(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	SetSlotActionAck( EN_ACTION_DONE::ACTION_UNLOAD, slot, CPU00, error );
	SetSlotActionAck( EN_ACTION_DONE::ACTION_UNLOAD, slot, CPU01, error );

	//if( GetRequestSlots() == GetActResponseSlots() )
	bool bAbnormal = false;
	if( IsActionDone( ACTION_UNLOAD, bAbnormal ) )
	{
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("UNLOAD PGM DONE"), false );
		SetToolBarStatus( EN_UI_STATUS::UI_SETUP_DONE );
		SetTestStatus( EN_UI_STATUS::UI_SETUP_DONE );
		emit SigProgressDlgHide();
		SetWindowTitle( "", true);
	}
	else
	{
		SetToolBarStatus( EN_UI_STATUS::UI_LOAD_DONE );
		SetTestStatus( EN_UI_STATUS::UI_LOAD_DONE );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("UNLOAD PGM ERROR"), true );
	}
}

void MainWindow::AckPgmInitTest(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	if( slot > MAX_SLOT-1 ) slot = MAX_SLOT-1;

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	SetSlotActionAck( ACTION_INITTEST, slot, runner, error );

	bool bAbnormal = false;
	m_bTestStart[slot][runner] = true;
	qDebug() << __func__ << "\tSLOT" << slot << "\tCPU : " << runner << "\t[" << m_bTestStart[slot][runner] << "]";

	if( IsActionDone( ACTION_INITTEST, bAbnormal ) )
	{
		SetToolBarStatus( EN_UI_STATUS::UI_TESTING );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("INIT TEST"), false );
		emit SigSysLogMessage( _NORMAL_LOG,	tr("[INFO][  ALLSLOT] %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
		SetTestStatus( EN_UI_STATUS::UI_INIT_TESTING );
		emit SigStartInitTest();
		if (m_bAutoMode)
			emit SigAutoInitTestStart();
//		m_bTestStart[slot][runner] = true;
	}
//	if (m_bAutoMode)
//	{
//		this;
//		qDebug() << __func__ << "\tSLOT" << slot << "\tCPU : " << runner << "\t[" << m_bTestStart[slot][runner] << "]";
//		if (m_bTestStart[slot][0] && m_bTestStart[slot][1])
//			AckAutoInitTestStart(slot);
//	}
}

void MainWindow::AckAutoInitTestStart(uint slot)
{
	this->m_uCorrespondingSlotBits |= (0b1 << slot);

    uint slot_with_bib = GetBiCmdSlots();

	if (this->m_uCorrespondingSlotBits == slot_with_bib)
	{
		memset(this->m_bTestStart, false, sizeof(this->m_bTestStart));
		emit SigAutoInitTestStart();
		this->m_uCorrespondingSlotBits = 0x0;
	}
}

void MainWindow::AckPgmInitTestDone(stIpcElement element)
{
	uint nLen   = element.ipcHeader.datalen;
	uint slot   = element.ipcHeader.slot;		//last init test end slot
	uint runner = element.ipcHeader.runner;
	int error   = element.ipcHeader.error;

	if( slot > MAX_SLOT-1 ) slot = MAX_SLOT-1;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	SetSlotActionAck( ACTION_INITTEST_DONE, slot, runner, error );

	emit SigInitTestDone(slot);
	emit SigTestItemListup(slot);

	m_bTestDone[slot][runner] = true;
	qDebug() << __func__ << "\tSLOT" << slot << "\tCPU : " << runner << "\t[" << m_bTestDone[slot][runner] << "]";

	bool bAbnormal = false;
	if( IsActionDone( ACTION_INITTEST_DONE, bAbnormal ) )
	{
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("INIT TEST DONE"), false );
		SetToolBarStatus( EN_UI_STATUS::UI_INIT_TEST_DONE );
		SetTestStatus( EN_UI_STATUS::UI_INIT_TEST_DONE );
		emit SigSysLogMessage( _NORMAL_LOG,	tr("[INFO][  ALLSLOT] %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
		if( bAbnormal )
		{
			ReqUnloadPgm();
//			this->m_bTestDone[slot][runner] = true;
		}
		if (m_bAutoMode)
			emit SigAutoInitTestDone();
		else
			emit SigEngrInitTestDone();		//2024.08.19 - KWMOON
	}
//	if (this->m_bAutoMode && m_bTestDone[slot][0] && m_bTestDone[slot][1])
//		AckAutoInitTestDone(slot);
}

void MainWindow::AckAutoInitTestDone(uint slot)
{
	this->m_uCorrespondingSlotBits |= (0b1 << slot);

	uint slot_with_bib = GetBiCmdSlots();

	if (this->m_uCorrespondingSlotBits == slot_with_bib)
	{
		memset(this->m_bTestDone, false, sizeof(this->m_bTestDone));
		emit SigAutoInitTestDone();
		this->m_uCorrespondingSlotBits = 0x0;
	}
}

void MainWindow::AckPgmStepIn(stIpcElement element)
{
	uint slot   = element.ipcHeader.slot;
	uint cpu    = element.ipcHeader.runner;
	stStepInInfo stepIn;
	memcpy( &stepIn, element.pData, sizeof(stStepInInfo) );
	emit SigStepIn(slot);
	emit SigPgmLogMessage( slot, cpu, _NORMAL_LOG, tr("[STEP_IN ] %1.%2").arg(stepIn.nStepNo+2).arg(stepIn.szStepName) );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STEP,
						 tr("[S#%1-%2] %3.%4").arg(slot+1).arg(cpu+1).arg(stepIn.nStepNo+2).arg(stepIn.szStepName), false );
}

void MainWindow::AckPgmStepOut(stIpcElement element)
{
	uint slot   = element.ipcHeader.slot;
	uint cpu    = element.ipcHeader.runner;
	stStepOutInfo stepOut;
	memcpy( &stepOut, element.pData, sizeof(stStepOutInfo) );
	emit SigPgmLogMessage( slot, cpu, _NORMAL_LOG, tr("[STEP_OUT] %1.%2").arg(stepOut.nStepNo+2).arg(stepOut.szStepName) );
	emit SigStepOut(slot);
}

void MainWindow::AckPgmCycleIn(stIpcElement element)
{
	uint slot   = element.ipcHeader.slot;		//last init test end slot
	uint cpu    = element.ipcHeader.runner;
	stCycleInfo cycleinfo;
	memcpy( &cycleinfo, element.pData, sizeof(stCycleInfo) );
	m_stSlotTestInfo[slot].tStartTime = cycleinfo.tCycleBegin;
	m_pMassView->SetSlotTestInfo( m_stSlotTestInfo );
	m_pEngrView->SetSlotTestInfo( m_stSlotTestInfo );
	emit SigPgmLogMessage( slot, cpu, _NORMAL_LOG, tr("TEST_START") );
}

void MainWindow::AckPgmCycleOut(stIpcElement element)
{
	uint slot   = element.ipcHeader.slot;		//last init test end slot
	stCycleInfo cycleinfo;
	memcpy( &cycleinfo, element.pData, sizeof(stCycleInfo) );
	m_stSlotTestInfo[slot].tEndTime = cycleinfo.tCycleEnd;
	m_pMassView->SetSlotTestInfo( m_stSlotTestInfo );
	m_pEngrView->SetSlotTestInfo( m_stSlotTestInfo );
}

void MainWindow::AckPgmTestDone(stIpcElement element)
{
	uint nLen   = element.ipcHeader.datalen;
	uint slot   = element.ipcHeader.slot;		//last init test end slot
	uint runner = element.ipcHeader.runner;
	int error   = element.ipcHeader.error;

	if( slot > MAX_SLOT-1 ) slot = MAX_SLOT-1;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	//2023.03.28 - kwmoon( ACTION_INITTEST_DONE -> ACTION_TEST_DONE )
	SetSlotActionAck( ACTION_TEST_DONE, slot, runner, error );
	emit SigPgmLogMessage( slot, runner, _NORMAL_LOG, tr("TEST_DONE") );

	bool bAbnormal = false;

	m_bTestStart[slot][runner] = true;
	qDebug() << __func__ << "\tSLOT" << slot << "\tCPU : " << runner << "\t[" << m_bTestStart[slot][runner] << "]";

	if( IsActionDone( ACTION_TEST_DONE, bAbnormal ) )
	{
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("TEST DONE"), false );
		SetToolBarStatus( EN_UI_STATUS::UI_TEST_DONE );
		SetTestStatus( EN_UI_STATUS::UI_TEST_DONE );
		SetSharedMemory( m_uiTestInfo.ui_chamberuse, false );	//2023.06.19 - KWMOON
		emit SigTestEndFlag();

		//m_pSaveDataDlg->SetLotNumber ( m_stSetupInfo.lot_number  );
		//m_pSaveDataDlg->SetPartNumber( m_stSetupInfo.part_number );
		emit SigMakeReport();	//2023.07.25 - KWMOON
		if (m_bAutoMode)
		{
			emit SigAutoStartTestDone();	// 2024.07.17 to notify TEST DONE in auto mode - hsbae
			ReqUnloadPgm();
		}
	}
}

void MainWindow::AckPgmTest(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;
	if( slot > MAX_SLOT-1 ) slot = MAX_SLOT-1;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	SetSlotActionAck( ACTION_TEST, slot, runner, error );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	bool bAbnormal = false;

	m_bTestStart[slot][runner] = true;
	qDebug() << __func__ << "\tSLOT" << slot << "\tCPU : " << runner << "\t[" << m_bTestStart[slot][runner] << "]";

	if( IsActionDone( ACTION_TEST, bAbnormal ) )
	{
		emit SigTestStartFlag();
		SetToolBarStatus( EN_UI_STATUS::UI_TESTING );
		SetTestStatus ( EN_UI_STATUS::UI_TESTING );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("TEST"), false );
		SetSharedMemory( m_uiTestInfo.ui_chamberuse, true );	//2023.06.19 - KWMOON
		if (m_bAutoMode)
			emit SigAutoStartTest();
	}
}

void MainWindow::AckPgmItemTest(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	SetSlotActionAck( ACTION_TEST, slot, runner, error );

	bool bAbnormal = false;
	if( IsActionDone( ACTION_TEST, bAbnormal ) )
	{
		emit SigTestStartFlag();
		SetToolBarStatus( EN_UI_STATUS::UI_TESTING );
		SetTestStatus ( EN_UI_STATUS::UI_TESTING );
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("JUMP TEST"), false );
		SetSharedMemory( m_uiTestInfo.ui_chamberuse, true );	//2023.06.19 - KWMOON
	}
}

void MainWindow::AckPgmStopTest(stIpcElement element)
{
	uint nLen  = element.ipcHeader.datalen;
	int slot   = element.ipcHeader.slot;
	int runner = element.ipcHeader.runner;
	int error  = element.ipcHeader.error;

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );

	SetSlotActionAck( ACTION_STOP, slot, runner, error );
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[RECV][SLOT%1-%2] %3").arg(slot+1,2,10,QChar('0')).arg(runner+1,2,10,QChar('0')).arg(UTIL::TCPIP::CMD_STR(cmd)) );
	bool bAbnormal = false;
	if( IsActionDone( ACTION_STOP, bAbnormal ) )
	{
		emit SigTestEndFlag();
		SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_STATUS, tr("TEST STOPPED"), true );
		SetToolBarStatus( EN_UI_STATUS::UI_TEST_DONE );
		SetTestStatus( EN_UI_STATUS::UI_TEST_DONE );
		SetSharedMemory( m_uiTestInfo.ui_chamberuse, false );	//2023.06.19 - KWMOON
	}
}

void MainWindow::SlotActLoadPgm()
{
	uint slot_bits = GetActionSlotBits( ACTION_LOAD );
	if( slot_bits == 0x0 )
	{
		QMessageBox::critical( this, "LOAD PGM", tr("There are no slots that have been set up.") );
		return;
	}

//	QString systemIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SYSTEM_INI);
//	QString section_key = tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_LOAD_EXTENSIONS);
//	QString s_value("");

//	UTIL::INI::readSettings(systemIni, section_key, s_value);

//	fprintf(stdout, "EXT LIST : %s", s_value.toStdString().c_str());

//	QStringList extension_list(s_value.split(","));
//	QString s_extension_list("");
//	for (QString& extension : extension_list)
//	{
//		s_extension_list += tr("%1 Files(*.%2);;").arg(tr("%1%2").arg(extension.at(0).toUpper()).arg(extension.mid(1))).arg(extension);
//	}
//	s_extension_list += ("All Files(*.*");


	QString sLoadFile = QFileDialog::getOpenFileName( this, "LOAD PROGRAM", m_sCurrentLoadDir, tr("Dev Files(*.dev);;Bin Files(*.bin)") );
//	QString sLoadFile = QFileDialog::getOpenFileName( this, "LOAD PROGRAM", m_sCurrentLoadDir, s_extension_list );
	if( sLoadFile.isEmpty() ) return;
	QFileInfo info(sLoadFile);

	QString fname = info.fileName();
	QString fdir  = info.dir().absolutePath();
	QString scopy = tr("%1/%2").arg(m_sProgramDir).arg(fname);
	QFile::remove( scopy );
	if( QFile::copy( sLoadFile, scopy ) )
	{
		m_sLoadFile = sLoadFile;
		SetStatusBarMessage( STATUS_BAR_STATUS, tr("Load %1").arg(sLoadFile), false );
		ReqLivePgms( true );
		ReqLoadPgm( scopy );
		m_sCurrentLoadDir = fdir;
	}
	else
	{
		m_sLoadFile = "";
		SetStatusBarMessage( STATUS_BAR_STATUS, tr("Copy %1 to %2 error").arg(fname).arg(m_sProgramDir), true );
		return;
	}
}

void MainWindow::ClearSlotActionDoneAck(EN_ACTION_DONE action)
{
	if( action == ACTION_MAX ) return;

	for( int s=0;s<MAX_SLOT;s++ )
	{
		if( m_stSlotInfo.SlotIp[s].bMask )
			m_stSlotActionDone[action][s].bSlotMask = true;
		else
			m_stSlotActionDone[action][s].bSlotMask = false;
		for( int cpu=0;cpu<MAX_SYS_CPU;cpu++ )
		{
			m_stSlotActionDone[action][s].bAck[cpu] = false;
			m_stSlotActionDone[action][s].bErr[cpu] = false;
			m_stSlotActionDone[action][s].sMsg[cpu] = "";
		}
	}
}

void MainWindow::SetSlotActionAck(EN_ACTION_DONE action, uint s, uint c, bool err, bool killed)
{
	if( action == ACTION_MAX ) return;

	m_stSlotActionDone[action][s].bAck[c] = true;
	m_stSlotActionDone[action][s].bErr[c] = err;
	m_stSlotActionDone[action][s].bKilled[c] = killed;
}

bool MainWindow::IsActionSlot( EN_ACTION_DONE action, uint slot )
{
	if( m_stSlotInfo.SlotIp[slot].bMask ) return false;
	if( m_stSlotActionDone[action][slot].bSlotMask ) return false;
	if( !m_uiTestInfo.empty_bib_test )
	{
		if( !m_stSlotTestInfo[slot].bib_id.bBibConn ) return false;	//2023.08.04 - KWMOON
		if( m_stSlotTestInfo[slot].bib_id.bParityError ) return false;	//2023.08.04 - KWMOON
	}
	return true;
}

bool MainWindow::IsLiveSlotBibCon()
{
	bool bCon = true;
	uint uSlotCount = 0;
	for( int s=0;s<MAX_SLOT;s++ )
	{
		if( m_stSlotInfo.SlotIp[s].bMask ) continue;
		if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_ONLINE )
		{
			if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
			{
				if( m_stSlotLive.slot_status[s][CPU01] == EN_SLOT_ONLINE )
				{
					if( !m_stSlotTestInfo[s].bib_id.bBibConn )
						bCon = false;
					if( m_stSlotTestInfo[s].bib_id.bParityError )
						bCon = false;
				}
			}
			else
			{
				if( !m_stSlotTestInfo[s].bib_id.bBibConn )
					bCon = false;
				if( m_stSlotTestInfo[s].bib_id.bParityError )
					bCon = false;
			}
		}
		uSlotCount++;
	}
	if( bCon )
	{
		if( uSlotCount == 0 )  bCon = false;
	}
	return bCon;
}

bool MainWindow::IsActionDone( EN_ACTION_DONE action, bool& abnormal )
{
	if( action == ACTION_MAX ) return false;

	uint done_count = 0;
	bool bDone = true;
	abnormal = false;
	for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
	{
		if (m_bAutoMode)
		{
			if ((m_uCorrespondingSlotBits & (1 << s)) >> s == 0)
			{
				continue;
			}
		}
		if( m_stSlotInfo.SlotIp[s].bMask ) continue;
		if( m_stSlotActionDone[action][s].bSlotMask ) continue;
		if( !m_uiTestInfo.empty_bib_test )
		{
			if( !m_stSlotTestInfo[s].bib_id.bBibConn ) continue;		//2023.08.04 - KWMOON
			if( m_stSlotTestInfo[s].bib_id.bParityError ) continue;		//2023.08.04 - KWMOON
		}
		for( int cpu=0;cpu<MAX_SYS_CPU;cpu++ )
		{
			if( m_stSlotActionDone[action][s].bAck[cpu] )
			{
				done_count++;
				if( m_stSlotActionDone[action][s].bErr[cpu] )
				{
					//bDone = false;
					//emit SigSysLogMessage( _ERROR_LOG,
					//					   tr("[INFO][SLOT%1-%2] %3 ERROR[%4]").arg(s+1).arg(cpu+1).arg(GetActionString(action)).arg(m_stSlotActionDone[action][s].sMsg[cpu]) );
				}
			}
			else
			{
				bDone = false;
			}

			if( m_stSlotActionDone[action][s].bKilled[cpu] )
			{
				abnormal = true;
			}
		}
	}
	if( bDone && (done_count>0) ) return true;
	return false;
}


uint MainWindow::GetActionSlotBits( EN_ACTION_DONE action )
{
	if( action == ACTION_MAX ) return 0x0;

    uint slot_bits = 0x0;

	for( int s=0;s<MAX_SLOT;s++ )
	{
        if( m_stSlotInfo.SlotIp[s].bMask ) continue;
		if( m_stSlotActionDone[action][s].bSlotMask ) continue;
		if( !m_uiTestInfo.empty_bib_test )
		{
			if( !m_stSlotTestInfo[s].bib_id.bBibConn ) continue;	//2023.08.04 - KWMOON
			if( m_stSlotTestInfo[s].bib_id.bParityError ) continue;	//2023.08.04 - KWMOON
		}
		slot_bits |= (1<<s);
	}
	return slot_bits;
}

QString MainWindow::GetActionString(EN_ACTION_DONE action)
{
	switch ( action )
	{
		case EN_ACTION_DONE::ACTION_SETUP:
			return QString("SETUP");
			break;
		case EN_ACTION_DONE::ACTION_LOAD:
			return QString("PGM LOAD");
			break;
		case EN_ACTION_DONE::ACTION_UNLOAD:
			return QString("PGM UNLOAD");
			break;
		case EN_ACTION_DONE::ACTION_INITTEST:
			return QString("INITTEST");
			break;
		case EN_ACTION_DONE::ACTION_TEST:
			return QString("TEST");
			break;
		case EN_ACTION_DONE::ACTION_STOP:
			return QString("STOP");
			break;
		default:
			return QString("UNDEF");
			break;
	}
	return QString("UNDEF");
}


void MainWindow::ReqLoadPgm( QString loadfile )
{
	QFileInfo info(loadfile);
	QString sName = info.fileName();
	QString sPath = info.path();

	//SetRequestSlots( GetAliveSlotBits() );
	//ClearSlotActionDoneAck( EN_ACTION_DONE::ACTION_LOAD );
	ClearSlotActionDoneAck( ACTION_LOAD );		//2023.03.17 - kwmoon

	stPgmDownload download;
	download.bCompress  = false;
	snprintf( download.szFileName, sizeof(download.szFileName), "%s", sName.toStdString().c_str() );
	snprintf( download.szFilePath, sizeof(download.szFilePath), "%s", sPath.toStdString().c_str() );
	download.nFileSize = info.size();

	stIpcHeader IpcHeader;
	IpcHeader.cmd     = PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_REQ;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATSLOTS;
	IpcHeader.slot    = GetActionSlotBits( ACTION_LOAD );
	IpcHeader.runner  = 0x0;
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = sizeof(stPgmDownload);

	uint uSize    = sizeof(stIpcHeader) + sizeof(stPgmDownload);
	char* pBuffer = new char[ uSize ];
	memcpy( pBuffer                    , &IpcHeader, sizeof(stIpcHeader)   );
	memcpy( pBuffer+sizeof(stIpcHeader), &download , sizeof(stPgmDownload) );

	if( m_pPrgDlg )
	{
		m_pPrgDlg->setModal( true );
		m_pPrgDlg->show();
	}
	emit SigProgressDlgInit();
	emit SigProgressDlgTitle("PGM LOAD");

	emit SigIpcMgrSendPacket( pBuffer, uSize );
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2[0x%3]")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_REQ)).arg(IpcHeader.slot,8,16, QChar('0')) );

	emit SigPgmLoad();
}

void MainWindow::ReqLoadPgmAutoMode( QString sLotId, QString sPgmFile )
{
	QString sErrMsg = "";

	ClearSlotActionDoneAck( ACTION_LOAD );
	uint uALiveSlotBits = GetActionSlotBits( ACTION_LOAD );

    uint uSlotWithBib = GetBiCmdSlots();

	if ( uSlotWithBib == 0x00 )
	{
		sErrMsg = tr("THERE ARE NO SLOTS WITH BIB").arg(sLotId);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	QFileInfo info(sPgmFile);
	QString sPgmName = info.fileName();
	QString sPath = info.path();
	bool bError = false;
	m_uCorrespondingSlotBits = 0x00;

	for ( int i = 0; i < MAX_SLOT; i++ )
	{
		if ( !(uSlotWithBib & (1 << i)) )
		{
			continue;
		}

		if ( !(uALiveSlotBits & (1 << i)) )
		{
			bError = true;
			sErrMsg = tr("SLOT %1 IS NOT ALIVE").arg(i + 1);

			break;
		}


		QString sRcmdMaintInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(m_sDataRoot).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(i).arg(EXT_RCMD);
		char * pReadMaintInfo = new char[sizeof(stRcmdBoardMaintInfo)];

		if ( !m_pTestDataMgr->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), pReadMaintInfo, sizeof(stRcmdBoardMaintInfo)) )
		{
			bError = true;
			sErrMsg = tr("FILE NOT FOUND: %1").arg(sRcmdMaintInfoFile);

			break;
		}

		stRcmdBoardMaintInfo rcmdBoardMaintInfo;
        memcpy( (char*)&rcmdBoardMaintInfo, pReadMaintInfo, sizeof(stRcmdBoardMaintInfo));
		delete[] pReadMaintInfo;

		QString Lots = rcmdBoardMaintInfo.MLB_LOT_LIST;
		QStringList LotList = Lots.split(",");

		if ( !LotList.contains(sLotId, Qt::CaseSensitive) )
		{
			continue;
		}

//		QString TestPgm = rcmdBoardMaintInfo.TEST_PGM_NO;

		//error experience in hynix
//		if ( sPgmName.left(sPgmName.lastIndexOf('.')).compare(TestPgm, Qt::CaseSensitive) != 0)
//		{
//			bError = true;
//			sErrMsg = tr("TEST PGM NO %1 DOES NOT MATCH WITH THE ONE IN SLOT_%2").arg(sPgmName).arg(i + 1);

//			break;
//		}

		m_uCorrespondingSlotBits |= (1 << i);
	}

	if (bError)
	{
		qDebug() << tr("[Error] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	if (m_uCorrespondingSlotBits == 0x00)
	{
		bError = true;
		sErrMsg = tr("THERE ARE NO CORRESPONDING SLOTS FOR LOT ID(%1)").arg(sLotId);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	stPgmDownload download;
	download.bCompress  = false;
	snprintf( download.szFileName, sizeof(download.szFileName), "%s", sPgmName.toStdString().c_str() );
	snprintf( download.szFilePath, sizeof(download.szFilePath), "%s", sPath.toStdString().c_str() );
	download.nFileSize = info.size();

	stIpcHeader IpcHeader;
	IpcHeader.cmd     = PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_REQ;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATSLOTS;
	IpcHeader.slot    = m_uCorrespondingSlotBits;
	IpcHeader.runner  = 0x0;
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = sizeof(stPgmDownload);

	uint uSize    = sizeof(stIpcHeader) + sizeof(stPgmDownload);
	char* pBuffer = new char[ uSize ];
	memcpy( pBuffer                    , &IpcHeader, sizeof(stIpcHeader)   );
	memcpy( pBuffer+sizeof(stIpcHeader), &download , sizeof(stPgmDownload) );

	if( m_pPrgDlg )
	{
		m_pPrgDlg->setModal( true );
		m_pPrgDlg->show();
	}
	emit SigProgressDlgInit();
	emit SigProgressDlgTitle("PGM LOAD AUTO MODE");

	emit SigIpcMgrSendPacket( pBuffer, uSize );
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2[0x%3]")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_REQ)).arg(IpcHeader.slot,8,16, QChar('0')) );

	emit SigPgmLoad();
}

void MainWindow::SlotActReloadPgm()
{
	ReqReLoadPgm();
}

void MainWindow::ReqReLoadPgm()
{
	ClearSlotActionDoneAck( ACTION_LOAD );

	stIpcHeader IpcHeader;
	IpcHeader.cmd     = PROTOCOL::TCP_COMMAND::CMD_PGM_RELOAD_REQ;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATSLOTS;
	IpcHeader.slot    = GetActionSlotBits( ACTION_LOAD );
	IpcHeader.runner  = 0x0;
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = 0x0;

	uint uSize    = sizeof(stIpcHeader);
	char* pBuffer = new char[ uSize ];
	memcpy( pBuffer, &IpcHeader, sizeof(stIpcHeader)   );

	if( m_pPrgDlg )
	{
		m_pPrgDlg->setModal( true );
		m_pPrgDlg->show();
	}
	emit SigProgressDlgInit();
	emit SigProgressDlgTitle("PGM RELOAD");

	emit SigIpcMgrSendPacket( pBuffer, uSize );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2[0x%3]")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND::CMD_PGM_RELOAD_REQ)).arg(IpcHeader.slot,8,16, QChar('0')) );
	emit SigPgmLoad();
}

void MainWindow::SlotActUnloadPgm()
{
	ReqUnloadPgm();
}

void MainWindow::ReqUnloadPgm()
{
	ClearSlotActionDoneAck( ACTION_UNLOAD );

	PROTOCOL::TCP_COMMAND cmd = PROTOCOL::TCP_COMMAND::CMD_PGM_UNLOAD_REQ;
	stIpcHeader IpcHeader;
	IpcHeader.cmd     = cmd;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATSLOTS;
	IpcHeader.slot    = GetActionSlotBits( ACTION_UNLOAD );
	IpcHeader.runner  = 0x0;
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = 0x0;

	if (this->m_bAutoMode)
		IpcHeader.slot = GetAliveSlotBits() & GetBiCmdSlots();

	uint uSize    = sizeof(stIpcHeader);
	char* pBuffer = new char[ uSize ];
	memcpy( pBuffer, &IpcHeader, sizeof(stIpcHeader)   );


	if( m_pPrgDlg )
	{
		m_pPrgDlg->setModal( true );
		m_pPrgDlg->show();
	}
	emit SigProgressDlgInit();
	emit SigProgressDlgTitle("PGM UNLOAD");

	emit SigIpcMgrSendPacket( pBuffer, uSize );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2[0x%3]")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(IpcHeader.slot,8,16, QChar('0')) );
}

void MainWindow::SlotActInitTest()
{
	if (this->m_bAutoMode)	// hsbae 20240703
        this->m_uCorrespondingSlotBits = GetAliveSlotBits() & GetBiCmdSlots();

	ReqUiChamberUse();		//2023.06.25 - KWMOON

	ClearSlotActionDoneAck( ACTION_INITTEST );
	ClearSlotActionDoneAck( ACTION_INITTEST_DONE );
	for( int i=0;i<m_stSlotInfo.SlotCpu.nMaxSlot;i++ )
	{
        if (this->m_bAutoMode && !(this->m_uCorrespondingSlotBits & (1 << i)) )
            continue;
		qDebug() << "START INIT TEST : " << i + 1;
		stUiInitInfo initInfo;
		if( IsActionSlot(ACTION_INITTEST, i) )
		{
			memcpy( &initInfo.stBid, &m_stSlotTestInfo[i].bib_id, sizeof(stBibId) );
			uint length = sizeof(stUiInitInfo);
			ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_START_REQ, (1<<i), length,  (char*)&initInfo );
		}
		SetTestStatus( UI_LOAD_DONE );
	}
	// TODO
	// set pfm in masstb
}

void MainWindow::SlotActTestStart()
{
    qDebug() << __func__;

	ReqUiChamberUse();		//2023.06.25 - KWMOON
	LoadSysChmbIni();			//2023.07.09 - KWMOON

	ClearSlotActionDoneAck( ACTION_TEST );
	ClearSlotActionDoneAck( ACTION_TEST_DONE );

	uint length = sizeof(stUiTestInfo);
	stUiTestInfo uitestinfo;

//	uint uSlots = GetActionSlotBits( ACTION_TEST );
	m_uCorrespondingSlotBits = GetActionSlotBits( ACTION_TEST );
    if (this->m_bAutoMode)
    {
//        uSlots &= GetBiCmdSlots();
		m_uCorrespondingSlotBits &= GetBiCmdSlots();
		qDebug() << "START TEST : " << m_uCorrespondingSlotBits;
    }
	memcpy( &uitestinfo, &m_uiTestInfo, sizeof(stUiTestInfo) );
	emit SigSysLogMessage( _NORMAL_LOG, tr("CHAMBER USE:%1").arg(uitestinfo.ui_chamberuse) );
	ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND::CMD_TEST_START_REQ, m_uCorrespondingSlotBits, length, (char*)&uitestinfo );
//	ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND::CMD_TEST_START_REQ, uSlots, length, (char*)&uitestinfo );
	m_pMassView->SetUiTestInfo( m_uiTestInfo );
	m_pEngrView->SetUiTestInfo( m_uiTestInfo );
}

void MainWindow::SlotActTestStop()
{
	ClearSlotActionDoneAck( ACTION_STOP );
	ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND::CMD_TEST_STOP_REQ, GetActionSlotBits( ACTION_STOP ), 0, nullptr );
}

void MainWindow::SlotJumpTest( stTestItem stJumpItems )
{
	ReqUiChamberUse();			//2023.06.25 - KWMOON
	LoadSysChmbIni();			//2023.07.09 - KWMOON
	stJumpItems.bChamberUse    = m_uiTestInfo.ui_chamberuse;		//2023.05.22 - KWMOON
	stJumpItems.chmb_soak_time = m_uiTestInfo.chmb_soak_time;	//2023.07.09 - KWMOON
	stJumpItems.chmb_keep_time = m_uiTestInfo.chmb_keep_time;	//2023.07.09 - KWMOON

	ClearSlotActionDoneAck( ACTION_TEST );
	ClearSlotActionDoneAck( ACTION_TEST_DONE );

	uint uSlots = GetActionSlotBits( ACTION_TEST );
	ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND::CMD_ITEM_TEST_START_REQ, uSlots, sizeof(stTestItem), (char*)&stJumpItems );
	m_pMassView->SetUiTestInfo( m_uiTestInfo );
	m_pEngrView->SetUiTestInfo( m_uiTestInfo );
}

void MainWindow::SlotJumpChamberUse(bool bChecked)
{
	if( m_bAutoMode )
	{
		m_uiTestInfo.ui_chamberuse = true;
	}
	else
	{
		m_uiTestInfo.ui_chamberuse = bChecked;
	}
	m_pMassView->SetUiTestInfo( m_uiTestInfo );
	m_pEngrView->SetUiTestInfo( m_uiTestInfo );
	//emit SigSysLogMessage( _NORMAL_LOG, tr("CHAMBER USE:%1").arg(m_uitestinfo.ui_chamberuse) );
}

void MainWindow::ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND tcpcmd, uint slot, uint size, char* pData )
{
	PROTOCOL::TCP_COMMAND cmd = tcpcmd;
	stIpcHeader IpcHeader;
	IpcHeader.cmd     = cmd;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATSLOTS;
	IpcHeader.slot    = slot;
	IpcHeader.runner  = 0x3;	//don't care
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = size;

	char* pBuffer = NULL;
	uint uSize = sizeof(stIpcHeader) + size;

	pBuffer = new char[ uSize ];
	memcpy( pBuffer                    , &IpcHeader  , sizeof(stIpcHeader)  );
	if( size > 0 )
		memcpy( pBuffer+sizeof(stIpcHeader), pData, IpcHeader.datalen  );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2:%3[0x%4]")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_bAutoMode?"AUTO":" MAN").arg(slot,8,16, QChar('0')) );

	emit SigIpcMgrSendPacket( pBuffer, uSize );
}

void MainWindow::ReqUiChamberUse()
{
	PROTOCOL::TCP_COMMAND cmd = PROTOCOL::CMD_UI_CHMBUSE_REQ;
	stIpcHeader IpcHeader;
	IpcHeader.cmd     = cmd;
	IpcHeader.src     = PROTOCOL::_ATMAIN_UI;
	IpcHeader.dest    = PROTOCOL::_ATMGR;
	IpcHeader.slot    = 0x0;
	IpcHeader.runner  = 0x0;	//don't care
	IpcHeader.error   = 0x0;
	IpcHeader.datalen = sizeof(bool);	//don't

	char* pBuffer = NULL;
	uint uSize = sizeof(stIpcHeader) + IpcHeader.datalen;

	pBuffer = new char[ uSize ];
	memcpy( pBuffer                    , &IpcHeader  , sizeof(stIpcHeader)  );
	if( IpcHeader.datalen > 0 )
		memcpy( pBuffer+sizeof(stIpcHeader), &m_uiTestInfo.ui_chamberuse, IpcHeader.datalen  );

	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[SEND][%1] %2 CHMB USE:%3")
						.arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_uiTestInfo.ui_chamberuse?"YES":" NO") );

	emit SigIpcMgrSendPacket( pBuffer, uSize );
}

void MainWindow::SlotActFailRate()
{
	if( m_pFailRate )
	{
		m_pFailRate->setModal( true );
		m_pFailRate->show();
	}
}

void MainWindow::SlotActJump()
{
	if( m_pJumpDlg )
	{
		m_pJumpDlg->setModal( true );
		m_pJumpDlg->show();
	}
}

void MainWindow::SlotActOTDelay()
{
	if( m_pOtDelay )
	{
		m_pOtDelay->setModal( true );
		m_pOtDelay->show();
	}
}

void MainWindow::SlotActAlarm()
{
	if( m_pErrPopup != nullptr )
	{
		m_pErrPopup->setModal( true );
		m_pErrPopup->show();
	}
}

void MainWindow::SlotActJudge()
{

}

void MainWindow::SlotActMaintCall()
{

}

void MainWindow::SlotActSaveData()
{
	if( m_pSaveDataDlg )
	{
		m_pSaveDataDlg->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		m_pSaveDataDlg->setModal( true );
		//m_pSaveDataDlg->SetLotNumber( m_stSetupInfo.lot_number );
		m_pSaveDataDlg->show();
	}
}

void MainWindow::SlotActLogView()
{
	qint64 pid = 0;
	bool bRun = false;
	if( UTIL::PROCESS::readPid( ATLOGVIEWUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATLOGVIEWUI_NAME ) )
		{
			emit SigSysLogMessage( _NORMAL_LOG, tr("%1 is already running!").arg(ATLOGVIEWUI_NAME) );
			bRun = true;
		}
	}
	if ( !bRun )
	{
		bRun = UTIL::PROCESS::DetachRun( ATLOGVIEWUI_NAME, QStringList{""}, pid );
	}
}

//void MainWindow::SlotActTemp()
//{
//	uint uSlots= GetAliveSlotBits();

//	for( uint s=0;s<MAX_SLOT;s++)
//	{
//		if( (1<<s) & uSlots	)
//			InitSlotTestInfo( s );
//	}
//	m_pMassView->SetSlotTestInfo( m_stSlotTestInfo );
//	m_pEngrView->SetSlotTestInfo( m_stSlotTestInfo );

//	int slot_no = 0;
//	emit sigBoardMaintInfo(slot_no);
//	slot_no = 3;
//	emit sigBoardMaintInfo(slot_no);

//	return;
//	QString sLotId = "TEST_LOT_NO_4";
//	QString sPgmFile = tr("%1/%2").arg(m_sDownloadDir).arg("eNAND/CIMCP/PE/512G/CTDBI/UMCP22/PLUG/REV0E/EFGH.dev");

//	ReqLoadPgmAutoMode(sLotId, sPgmFile);

//	return;
//	QString sLotId = "TEST_LOT_NO_4";

//	emit sigTGET(sLotId);

//	return;
//}

void MainWindow::SlotActSetRackIgnore(bool rack_ignore)
{
    emit SigSetRackIgnore(rack_ignore);
}

void MainWindow::SlotActReadyToLoad()
{

}

void MainWindow::SlotActLoadComplete()
{

}

void MainWindow::SlotActChamberCheck()
{
//	if( m_pChmbDlg )
//	{
//		Qt::WindowFlags flags = m_pChmbDlg->windowFlags();
//		Qt::WindowFlags closeFlag = Qt::WindowCloseButtonHint | Qt::WindowContextHelpButtonHint;
//		flags = flags & (~closeFlag);
//		m_pChmbDlg->setWindowFlags( flags );
//		m_pChmbDlg->setModal( true );
//		m_pChmbDlg->show();
//	}
}

void MainWindow::SlotDownloadProgress(int progress)
{
	int nProgress = progress / 2;
	qDebug() << "nProgress:" << nProgress;
}

void MainWindow::SlotDownloadFtpError(QString sLotId, QString sErrorMsg)
{
	qDebug() << tr("[FTP][ERROR] %1: %2").arg(sLotId).arg(sErrorMsg);
}

void MainWindow::SlotDownloadFtpResult(bool bSuccess, QString sLotId, QString sPgmFile)
{
//	sPgmFile.replace("download", "download1");

	if (bSuccess)
	{
		qDebug() << tr("[FTP][INFO] %1: %2").arg(sLotId).arg("PGM DOWNLOAD COMPLETE FROM TAMS.");
		SetStatusBarMessage( STATUS_BAR_STATUS, tr("Load %1").arg(sPgmFile), false );
		ReqLivePgms( true );
		ReqLoadPgmAutoMode(sLotId, sPgmFile);
	}
	else
	{
		qDebug() << tr("[FTP][INFO] %1: %2").arg(sLotId).arg("PGM DOWNLOAD FAIL FROM TAMS.");
		emit SigRcmdReply(bSuccess);
	}
}

void MainWindow::SlotPgmDownloadAutoMode(QString lot_info)
{
	QString sRcmdFileName = tr("%1/%2/%3/%4%5").arg(m_sDataRoot).arg(DIR_AUTO_DATA_PATH).arg(RCMD_TGET).arg(lot_info).arg(EXT_RCMD);
	char* pReadData = new char[sizeof(stRcmdTGET)];

	if(!m_pTestDataMgr->READ_FILE(sRcmdFileName.toStdString().c_str(), pReadData, sizeof(stRcmdTGET)))
	{
		emit SigDownloadPgmResult(false, lot_info);
		return;
	}

	stRcmdTGET rcmdTGet;
    memcpy((char*)&rcmdTGet, pReadData, sizeof(stRcmdTGET));

	QString sPgmFile = tr(rcmdTGet.TEST_PGM_NO);
	QString sFileExtension = "";
	QString systemIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SYSTEM_INI);
	QString section_key = tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_FILE_EXTENSION);
	if (UTIL::INI::readSettings( systemIni, section_key, sFileExtension))
	{
		sPgmFile += sFileExtension;
	}
	else
	{
		sPgmFile += DEFAULT_DEVEXT;
	}

	delete[] pReadData;
	m_pPgmDownloader->SetLotId(rcmdTGet.LOT_NO);
	m_pPgmDownloader->SetDownloadDir(rcmdTGet.PGM_DIR);
	m_pPgmDownloader->SetFtpInfo(rcmdTGet.TAMS_IP, 21, rcmdTGet.TAMS_ID, rcmdTGet.TAMS_PWD);
	m_pPgmDownloader->SetDownloadFileInfo(rcmdTGet.PGM_DIR, sPgmFile, 0, false);

	emit SigDownloadPgm();
}

void MainWindow::SlotIpcMgrIpcLogMessage( LOG_TYPE t, QString s )
{
	QString msg = tr("[%1] %2").arg(m_pMgrIpc->GetName()) + s;
	emit SigSysLogMessage( t, s );
}

void MainWindow::SlotIpcMgrIpcError ( QLocalSocket::LocalSocketError err, QString s )
{
	QString msg = "[MGR]" + s;
	emit SigSysLogMessage( _ERROR_LOG, msg );
}

void MainWindow::SlotIpcMgrConnectionStatus( bool bConn )
{
	m_bMgrConn = bConn;
	if( bConn )
	{
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_MGR , true );
		ReqMgrVersion();
		ReqInfoSlots();
		ReqLiveSlots();
	}
	else
	{
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_MGR , false );
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_OFFLINE )
			{
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
			}
			else if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_ONLINE )
			{
				m_stSlotLive.slot_status[s][CPU00] = EN_SLOT_OFFLINE;
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
			}
			else
			{
				emit SigSlotLoaderStatusChanged( s, EN_SLOT_MASK );
			}
		}
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_SLOT  , false );
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_TMS   , false );
		emit SigMgrTmsStatusChanged( LED_TMS_TO_EAP    , false );
	}
}

void MainWindow::SlotIpcMgrRecvPacket( stIpcElement element )
{
	int nCmd  = element.ipcHeader.cmd;
	int nSrc  = element.ipcHeader.src;
	int nDest = element.ipcHeader.dest;
	int nLen  = element.ipcHeader.datalen;
	int nErr  = element.ipcHeader.error;
	uint slot = element.ipcHeader.slot;
	char* pData = nullptr;

	switch ( nCmd )
	{
		case PROTOCOL::CMD_SLOT_INFO_ACK:
			AckInfoSlots( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_LIVE_SLOT_ACK:
			AckLiveSlots( element );
			break;
		case PROTOCOL::CMD_LIVE_PGM_ACK:
			AckLivePgms( element );
			break;

		//2024.06.17 - KWMOON(not use)
		//case PROTOCOL::CMD_SETUP_BIBID_ACK:
		//	AckSetupBibId( element );
		//	emit SigSysLogMessage( _NORMAL_LOG,
		//					tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
		//	break;

		//2024.06.17 - KWMOON)
		case PROTOCOL::CMD_READ_BIBID_ACK:
			AckReadBibId( element );
			break;

		//2024.06.17 - KWMOON)
		case PROTOCOL::CMD_ENGR_SETUP_ACK:
			AckEngrSetup( element );
			break;

		//2024.06.17 - KWMOON)
		case PROTOCOL::CMD_AUTO_SETUP_ACK:
			AckAutoSetup( element );
			break;

		case PROTOCOL::CMD_PGM_LOAD_STATUS:
			AckPgmLoadStatus( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_PGM_LOAD_ACK:
			AckPgmLoadDone( element, false );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_PGM_RELOAD_ACK:
			AckPgmLoadDone( element, true  );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_PGM_UNLOAD_ACK:
			AckPgmUnloadDone( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;

		case PROTOCOL::CMD_VERSION_ACK:
		{
			if( element.ipcHeader.src == PROTOCOL::_ATMGR )
			{
				if( nLen > 0 )
				{
					pData = new char[nLen];
					memset( pData, 0x0, nLen );
					memcpy( pData, element.pData, nLen );
					QString mgr_ver =  QString(pData);
					SetStatusBarMessage( STATUS_BAR_MGR_VER, mgr_ver, false );
					emit SigSysLogMessage( _NORMAL_LOG,
									tr("[RECV][%1] %2 %3").arg(m_pMgrIpc->GetName()).arg(ATMGR_NAME).arg(mgr_ver) );
					delete[] pData;
				}
			}
			else
			{
				int slot = element.ipcHeader.slot;
				int runner = element.ipcHeader.runner;
				//LOG_TAB LOGTAB = static_cast<LOG_TAB>(LOG_SLOT1 + slot);
				if( nLen > 0 )
				{
					pData = new char[nLen];
					memset( pData, 0x0, nLen );
					memcpy( pData, element.pData, nLen );
					emit SigSysLogMessage( _NORMAL_LOG,
									tr("[RECV][SLOT%1-%2] %3").arg(slot+1).arg(runner+1).arg(QString(pData)) );
					emit SigProgressDlgValue( slot, 50 );
					delete[] pData;
				}
			}
		}
		break;

		//2024.06.17 - KWMOON(not use)
		//case PROTOCOL::CMD_TEST_DATA_INIT_ACK:
		//{
		//	AckMgrTestDataInit( element );
		//}
		//break;

		case PROTOCOL::CMD_UI_CHMBUSE_ACK:
		{
			AckMgrUiChamberUse( element );
		}
		break;

		case PROTOCOL::CMD_PGM_FINISHED:
			RecvPgmFinished( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_INIT_TEST_START_ACK:
			AckPgmInitTest( element );
//			qDebug() << "[HS] INIT TEST START : " << element.ipcHeader.slot;
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_INIT_TEST_DONE:
			AckPgmInitTestDone( element );
//			emit SigAckPgmInitTestDone();
//			qDebug() << "[HS] INIT TEST DONE : " << element.ipcHeader.slot;
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_TEST_START_ACK:
			AckPgmTest( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_ITEM_TEST_START_ACK:
			AckPgmItemTest( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_TEST_STOP_ACK:
			AckPgmStopTest(element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_STEP_IN_INFO:
			AckPgmStepIn( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_STEP_OUT_INFO:
			AckPgmStepOut( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_CYCLE_IN_INFO:
			AckPgmCycleIn( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_CYCLE_OUT_INFO:
			AckPgmCycleOut( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;
		case PROTOCOL::CMD_TEST_DONE:
			AckPgmTestDone( element );
			emit SigSysLogMessage( _NORMAL_LOG,
							tr("[RECV][%1] %2").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
			break;

		//case PROTOCOL::CMD_PGM_ITEM_LISTUP:
		//{
		//	RecvTestItemList(element);
		//}
		//break;

		case PROTOCOL::TCP_COMMAND::CMD_LOG_PRINT:
		{
			//2023.08.10 - KWMOON( Slot Log save at atmgr )
			//RecvLogPrint( element );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_RUNTIME_ERROR:
		{
			RecvPgmRuntimeError( element );
		}
		break;

		//2023.06.11 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_DEV_PGM_VARIABLE:
		{
			RecvPgmVariables( element );
		}
		break;

		//2024.08.14 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_TEMP_MON_SET:
		{
			AckChmbTempMonSet( element );
		}
		break;

		//2024.08.14 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_TEMP_MON_END:
		{
			AckChmbTempMonEnd( element );
		}
		break;


		default:
			emit SigSysLogMessage( _ERROR_LOG,
							tr("[RECV][%1] %2 is not processed command").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(static_cast<PROTOCOL::TCP_COMMAND>(nCmd))) );
	}

	if( element.ipcHeader.datalen > 0 )
	{
		if( element.pData )
		{
			delete[] element.pData;
			element.pData = nullptr;
		}
	}
}

void MainWindow::SlotIpcTmsIpcLogMessage( LOG_TYPE t, QString s )
{
	QString msg = tr("[%1] %2").arg(m_pTmsIpc->GetName()) + s;
	emit SigSysLogMessage( t, s );
}

void MainWindow::SlotIpcTmsIpcError ( QLocalSocket::LocalSocketError err, QString s )
{
	QString msg = "[TMS]" + s;
	emit SigSysLogMessage( _ERROR_LOG, msg );
}

void MainWindow::SlotIpcTmsConnectionStatus( bool bConn )
{
	m_bTmsConn = bConn;
	if( bConn )
	{
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_TMS , true );
//		ReqMgrVersion();
//		ReqInfoSlots();
//		ReqLiveSlots();
	}
	else
	{
		emit SigMgrTmsStatusChanged( LED_MAIN_TO_TMS , false );
//		emit SigMgrTmsStatusChanged( LED_MGR , false );
//		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
//		{
//			if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_OFFLINE )
//			{
//				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
//			}
//			else if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_ONLINE )
//			{
//				m_stSlotLive.slot_status[s][CPU00] = EN_SLOT_OFFLINE;
//				emit SigSlotLoaderStatusChanged( s, EN_SLOT_OFFLINE );
//			}
//			else
//			{
//				emit SigSlotLoaderStatusChanged( s, EN_SLOT_MASK );
//			}
//		}
//		emit SigMgrTmsStatusChanged( LED_MGR_TO_SLOT  , false );
//		emit SigMgrTmsStatusChanged( LED_MGR_TO_TMS   , false );
//		emit SigMgrTmsStatusChanged( LED_MGR_TO_TMSSVR, false );
	}
}

//2023.07.09 - KWMOON
bool MainWindow::LoadSysChmbIni()
{
	QString ini = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(CHAMBER_INI);
	QString section_key = "";
	int n_value = 0;

	//SOAK TIME
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_SOAK_TIME);
	if ( UTIL::INI::readSettings( ini, section_key, n_value ) )
	{
		m_uiTestInfo.chmb_soak_time = n_value;
	}
	else
	{
		m_uiTestInfo.chmb_soak_time = DEFAULT_CHMB_SOAK_TIME;
	}

	//KEEP TIME
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_KEEP_TIME);
	if ( UTIL::INI::readSettings( ini, section_key, n_value ) )
	{
		m_uiTestInfo.chmb_keep_time = n_value;
	}
	else
	{
		m_uiTestInfo.chmb_keep_time = DEFAULT_CHMB_KEEP_TIME;
	}
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[INFO] SOAK TIME:%1 min / KEEP TIME:%2 min").arg(m_uiTestInfo.chmb_soak_time).arg(m_uiTestInfo.chmb_keep_time) );

	ini = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SYSTEM_INI);
	section_key = tr("%1/%2").arg(SECTION_SLOT_OPTION).arg(KEY_EMPTY_BIB_TEST);
	if ( UTIL::INI::readSettings( ini, section_key, n_value ) )
	{
		m_uiTestInfo.empty_bib_test = (n_value!=0)?true:false;
	}
	else
	{
		m_uiTestInfo.empty_bib_test = false;
	}
	emit SigSysLogMessage( _NORMAL_LOG,
					tr("[INFO] EMPTY BIB SLOT TEST : %1").arg(m_uiTestInfo.empty_bib_test?"YES":"NO"));
	return true;
}

void MainWindow::SlotMakeReportDone(bool done)
{
	if (!done)
		return;
	emit SigMakeReportDone(done);
//	return ;
	if( m_pSaveDataDlg != nullptr )
	{
		if( !m_pSaveDataDlg->isHidden() )
		{
			m_pSaveDataDlg->close();
		}
	}
}

void MainWindow::SlotSyncSlotTime(CSecsMsg* secs_msg)
{
	unsigned short result = 0x0;

	for (int i = 0; i < MAX_SLOT; i++)
	{
		if (this->m_stSlotLive.slot_status[i][CPU00] == EN_SLOT_ONLINE)
		{
//			qDebug() << tr("TIME SYNC : %1%2").arg("192.168.62.").arg(i + 1);
			QString scmd = tr("sshpass -p %1 ssh %2@%3%4 -T -o StrictHostKeyChecking=no 'echo %5 | sudo -S systemctl restart systemd-timesyncd.service'")
						   .arg(SSH_PASSWD)
						   .arg(SSH_ID)
						   .arg("192.168.62.")
						   .arg(i + 1)
						   .arg(SSH_PASSWD);
			int system_result = std::system ( scmd.toStdString().c_str() );
			if (system_result == 0)
			{
				result |= 1 << i;
			}
		}
		else
		{
			result |= 1 << i;
		}
	}
//	qDebug() << "TIME SYNC RESULT : " << result;
	emit SigSyncSlotTimeResult(result, secs_msg);
}

void MainWindow::SlotRecvEapConnectStatus(bool connStatus)
{
	m_bEapConn = connStatus;
	if (connStatus)
	{
		emit SigMgrTmsStatusChanged( LED_TMS_TO_EAP , true );
	}
	else
	{
		emit SigMgrTmsStatusChanged( LED_TMS_TO_EAP , false );
	}
}

//2024.06.20 - KWMOON
void MainWindow::SlotFromSlotViewCurrentSlot(uint slot)
{
	m_nCurrentSlot = slot;
}

void MainWindow::SlotFromHsmsMgrAutoDiag()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiCloseLot()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiInitialEnd()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiInitialStart()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiOpenLot()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiRecloseLot()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBiTestTime()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBoardMaintInfo(int slot)
{
	QString sErrMsg = "";
	QString sRcmdMaintInfoFile = tr("%1/%2/%3/SLOT%4%5")
			.arg(m_sDataRoot)
			.arg(m_bAutoMode ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
			.arg(BOARD_MAINT_DIR)
			.arg(slot)
			.arg(EXT_RCMD);

	stRcmdBoardMaintInfo rcmdBoardMaintInfo;

	if ( !m_pTestDataMgr->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&rcmdBoardMaintInfo, sizeof(stRcmdBoardMaintInfo)) )
	{
		sErrMsg = tr("FILE NOT FOUND: %1").arg(sRcmdMaintInfoFile);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	QString bibId = rcmdBoardMaintInfo.BIB_ID;

	if (QString::compare(bibId, m_stSlotTestInfo[slot].bib_id.szBibId, Qt::CaseInsensitive) != 0)
	{
		sErrMsg = tr("BIB ID DOES NOT MATCH: CPU%1(%2), EAP(%3)").arg(slot).arg(m_stSlotTestInfo[slot].bib_id.szBibId).arg(bibId);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);
		return;
	}

	sErrMsg = tr("BIB ID MATCH: CPU%1(%2), EAP(%3)").arg(slot).arg(m_stSlotTestInfo[slot].bib_id.szBibId).arg(bibId);
	qDebug() << tr("[SUCCESS] %1").arg(sErrMsg);
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBoardStatusInfo()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrBoardType(ushort valid_slot)
{
	QString bibType = "";

	for (uint slot = 0; slot < MAX_SLOT; slot++)
	{
		if( (1 << slot) & valid_slot )
		{
			QString bibId = m_stSlotTestInfo[slot].bib_id.szBibId;
			bibType = bibId.left(3);
			break;
		}
	}

	QString sErrMsg = "";
	QString sRcmdBoardType = tr("%1/%2/%3%4")
			.arg(m_sDataRoot)
			.arg(m_bAutoMode ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
			.arg(RCMD_BOARD_TYPE)
			.arg(EXT_RCMD);
	stRcmdBoardType rcmdBoardType;

	if ( !m_pTestDataMgr->READ_FILE(sRcmdBoardType.toStdString().c_str(), (char*)&rcmdBoardType, sizeof(stRcmdBoardMaintInfo)) )
	{
		sErrMsg = tr("FILE NOT FOUND: %1").arg(sRcmdBoardType);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	if ( QString::compare(bibType, rcmdBoardType.BIB_TYPE, Qt::CaseInsensitive) != 0 )
	{
		//error experience in hynix
		sErrMsg = tr("BIB TYPE DOES NOT MATCH: EQ(%1), EAP(%2)").arg(bibType).arg(rcmdBoardType.BIB_TYPE);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);
		return;
	}

	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrCheckInBoard()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrCheckOutBoard()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrEqEffInfo()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrHoldCheck()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrInterLockLimit()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrReadyToLoad()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrLotCall()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrLotSet()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrLotSetInfo()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrManualMaskMap()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrPmCheck()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrRackSetInfo()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrRecheckInBoard()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrAutoBypass()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrReplyAutoBypassEqpState()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrRequestBoardMaskSet()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrRequestRackSet()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrRMVOU()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrSlotMaskInfo1()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrSlotMaskInfo2()
{
	// do Auto Setup
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrStartBiCmd()
{
	uint uSlots = GetAliveSlotBits();

	for( uint s=0;s<MAX_SLOT;s++)
	{
		if( (1<<s) & uSlots	)
		{
			InitSlotTestInfo( s );
		}
	}
	m_pMassView->SetSlotTestInfo( m_stSlotTestInfo );
	m_pEngrView->SetSlotTestInfo( m_stSlotTestInfo );

	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrTGET(QString lot_info)
{
	QString sErrMsg = "";
	QString sRcmdFileName = tr("%1/%2/%3/%4%5").arg(m_sDataRoot).arg(DIR_AUTO_DATA_PATH).arg(RCMD_TGET).arg(lot_info).arg(EXT_RCMD);
	char* pReadData = new char[sizeof(stRcmdTGET)];

	if(!m_pTestDataMgr->READ_FILE(sRcmdFileName.toStdString().c_str(), pReadData, sizeof(stRcmdTGET)))
	{
		sErrMsg = tr("FILE NOT FOUND: %1").arg(sRcmdFileName);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);
		emit SigRcmdReply(false);

		return;
	}

	stRcmdTGET rcmdTGet;
    memcpy((char*)&rcmdTGet, pReadData, sizeof(stRcmdTGET));

	QString sPgmFile = tr(rcmdTGet.TEST_PGM_NO);
	QString sFileExtension = "";
	QString systemIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SYSTEM_INI);
	QString section_key = tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_FILE_EXTENSION);
	if (UTIL::INI::readSettings( systemIni, section_key, sFileExtension))
	{
		sPgmFile += sFileExtension;
	}
	else
	{
		sPgmFile += DEFAULT_DEVEXT;
	}

	delete[] pReadData;
	m_pPgmDownloader->SetLotId(rcmdTGet.LOT_NO);
	m_pPgmDownloader->SetDownloadDir(rcmdTGet.PGM_DIR);
	m_pPgmDownloader->SetFtpInfo(rcmdTGet.TAMS_IP, 21, rcmdTGet.TAMS_ID, rcmdTGet.TAMS_PWD);
	m_pPgmDownloader->SetDownloadFileInfo(rcmdTGet.PGM_DIR, sPgmFile, 0, false);

	emit SigDownloadPgm();
	m_sCurrentLotId = lot_info;
}

void MainWindow::SlotFromHsmsMgrTMVIN()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrTMVOU()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrVMVOU()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrReadyToUnload()
{
	emit SigRcmdReply(true);
}

void MainWindow::SlotFromHsmsMgrError()
{
	emit SigRcmdReply(false);
}

//2024.07.23 - KWMOON
void MainWindow::SlotFromHsmsMgrBibIdRead(ushort slot_bits)
{
	m_uValidSlotBit = slot_bits;
	ReqLoaderBibIdAutoMode();
}

//2024.08.14 - KWMOON
void MainWindow::AckChmbTempMonSet(stIpcElement element)
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	if( element.ipcHeader.datalen == sizeof(stGetTemp) )
	{
		if( m_bAutoMode )
		{
			stGetTemp temp_set;
			memcpy( &temp_set, element.pData, element.ipcHeader.datalen );
			emit SigSysLogMessage( LOG_TYPE::_NORMAL_LOG,
								   tr("[RECV][%1] %2 STEP.NO[%3] SV[%4]  PV[%5]").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(temp_set.nAck).arg(temp_set.dSv).arg(temp_set.dPv) );
			emit SigChmbTempMonSet( temp_set );
		}
	}
}

//2024.08.14 - KWMOON
void MainWindow::AckChmbTempMonEnd(stIpcElement element)
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.ipcHeader.cmd );
	if( element.ipcHeader.datalen == sizeof(stGetTemp) )
	{
		if( m_bAutoMode )
		{
			stGetTemp temp_end;
			memcpy( &temp_end, element.pData, element.ipcHeader.datalen );
			emit SigSysLogMessage( LOG_TYPE::_NORMAL_LOG,
								   tr("[RECV][%1] %2 STEP.NO[%3] SV[%4]  PV[%5]").arg(m_pMgrIpc->GetName()).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(temp_end.nAck).arg(temp_end.dSv).arg(temp_end.dPv) );
			emit SigChmbTempMonEnd( temp_end );
		}
	}
}
