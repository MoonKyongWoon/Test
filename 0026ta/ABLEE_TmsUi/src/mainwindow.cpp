#include "mainwindow.hpp"
#include "ui_mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow)
{
	ui->setupUi(this);
	RegisterMetaType();
	InitVariables();
	InitUi();
}

CMainWindow::~CMainWindow()
{
	delete ui;
}

void CMainWindow::RegisterMetaType()
{
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<_LOG_TAB>("_LOG_TAB");
	qRegisterMetaType<_STATUS_LABEL>("_STATUS_LABEL");
	qRegisterMetaType<stHsmsElement>("stHsmsElement");
}

void CMainWindow::InitVariables()
{
	m_bInitialized = false;
	m_pConfigDlg   = new CConfig( this );
	m_pStatusBar   = new CStatusBar( this );
	m_pTmsUi       = new CTmsUi( this );
}

void CMainWindow::InitUi()
{
	CreateToolbar();
	CreateStatusBar();

	connect( this            , &CMainWindow::SigStatusChanged , m_pStatusBar , &CStatusBar::SlotStatusChanged );
	connect( this            , &CMainWindow::SigStartService  , m_pTmsUi     , &CTmsUi::SlotStart             );
	connect( this            , &CMainWindow::SigRestartService, m_pTmsUi     , &CTmsUi::SlotRestart           );
	connect( this            , &CMainWindow::SigSysLog        , m_pTmsUi     , &CTmsUi::SlotSysLog            );
	connect( m_pTmsUi        , &CTmsUi::SigIpcStatusChanged   , this         , &CMainWindow::SlotIpcStatusChanged);
	connect( m_pTmsUi        , &CTmsUi::SigTcpStatusChanged   , this         , &CMainWindow::SlotTcpStatusChanged);
	connect( m_pTmsUi        , &CTmsUi::SigHsmsStatusChanged  , this         , &CMainWindow::SlotHsmsStatusChanged);
	connect( m_pActConfig    , &QAction::triggered            , this         , &CMainWindow::SlotActConfig    );
	connect( m_pActResend    , &QAction::triggered            , this         , &CMainWindow::SlotActReSend    );

	connect( m_pConfigDlg    , &CConfig::SigConfigSaved       , this         , &CMainWindow::SlotConfigSaved          );
	connect( m_pConfigDlg    , &CConfig::SigConfigRestartService, this         , &CMainWindow::SlotConfigRestartService );

	setCentralWidget( m_pTmsUi );

	QString text = QString("%1 %2 %3").arg(ATTMSUI_NAME).arg(ATTMSUI_VER).arg( "[" ATTMSUI_DATE "]") ;
	setWindowTitle( text );
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent( event );
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
	if(event->type() == QEvent::Close)
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, ATTMSUI_NAME, tr("Do you want to quit program?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			if( 1 )
			{
				QSettings settings( CONFIG_INI_KEY, ATTMSUI_NAME );
				settings.setValue( "geometry"   , saveGeometry() );
				settings.setValue( "windowState", saveState()    );
				settings.sync();
				//qDebug() << settings.fileName();
			}
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
}

void CMainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
	if (event->spontaneous()) return;
	if ( m_bInitialized ) return;
	m_bInitialized = true;

	emit SigStartService();

	SetStatusMessage( "READY" );
}

void CMainWindow::SetStatusMessage( QString msg )
{
	statusBar()->showMessage( msg );
}

void CMainWindow::CreateToolbar()
{
	QToolBar *fileToolBar = addToolBar(tr("Toolbar"));
	fileToolBar->setFloatable(false);
	fileToolBar->setMovable(false);
	fileToolBar->setHidden(false);
	fileToolBar->setIconSize(QSize(28,28));
	fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	fileToolBar->setObjectName("ToolBar");
	//fileToolBar->setFont( QFont(fileToolBar->font().family(), 7) );

	//Setup
	const QIcon icoSetup = QIcon::fromTheme( tr("Config"), QIcon(":/icon/icon/settings.ico"));
	m_pActConfig = new QAction(icoSetup, tr("CONFIG"), this);
	m_pActConfig->setToolTip(tr("TMS Config"));
	//m_pActConfig->setStatusTip( tr("TMS Config"));
	fileToolBar->addAction(m_pActConfig);
	fileToolBar->addSeparator();
	//Load
	const QIcon icoLoad = QIcon::fromTheme( tr("Resend"), QIcon(":/icon/icon/reload0.ico"));
	m_pActResend = new QAction(icoLoad, tr("RE-SEND"), this);
	//m_pActResend->setStatusTip( tr("Resend report files"));
	m_pActResend->setToolTip(tr("Resend report files"));
	fileToolBar->addAction(m_pActResend);
	fileToolBar->addSeparator();
}

void CMainWindow::CreateStatusBar()
{
	statusBar()->addPermanentWidget( m_pStatusBar, 0 );
	statusBar()->setSizeGripEnabled( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotIpcStatusChanged
/// \details IPC connection status received from TMS UI
void CMainWindow::SlotIpcStatusChanged(bool bIpcConnected)
{
	emit SigStatusChanged(_LED_MAINUI, bIpcConnected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotTcpStatusChanged
/// \details TCP connection status received from TMS UI
void CMainWindow::SlotTcpStatusChanged(bool bTcpConnected)
{
	emit SigStatusChanged(_LED_EAP_TCP, bTcpConnected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotHsmsStatusChanged
/// \details HSMS status received from TMS UI
void CMainWindow::SlotHsmsStatusChanged(bool bHsmsSelected)
{
	emit SigStatusChanged(_LED_EAP_SELECT, bHsmsSelected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotActConfig
/// \details CONFIG button clicked
void CMainWindow::SlotActConfig()
{
	if( m_pConfigDlg->isVisible() ) return;

	m_pConfigDlg->setModal( true );
	m_pConfigDlg->show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotActReSend
/// \details RE-SEND button clicked
void CMainWindow::SlotActReSend()
{
	QStringList files = QFileDialog::getOpenFileNames( this,
													   "SELECT SEND FILES", QDir::currentPath(), "Files(*.FLS *.cumFailBlock)");
	if( files.size() < 1 )
	{
		QMessageBox::critical( this,
							   this->windowTitle(), tr("There are no files") );
		return;
	}
	//file send procedure....
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotConfigSaved
/// \details from config dlg
void CMainWindow::SlotConfigSaved()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainWindow::SlotConfigRestartService
/// \details from config dlg
void CMainWindow::SlotConfigRestartService()
{
	//if hsms client is connected, ignore
	//if main ui connected, ignore
	emit SigRestartService();
}
