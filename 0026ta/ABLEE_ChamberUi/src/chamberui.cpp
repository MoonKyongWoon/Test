#include "chamberui.hpp"
#include "ui_chamberui.h"

CChamberUi::CChamberUi(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CChamberUi)
{
	RegisterMetaType();
	InitVariable();
	InitUi();
	InitInstance();
}

CChamberUi::~CChamberUi()
{
    if( trayIcon != nullptr )
    {
        if( trayIcon->isVisible() )
        {
            trayIcon->setVisible( false );
        }
        delete trayIcon;
    }
	delete ui;
}

void CChamberUi::InitUi()
{
	ui->setupUi(this);
    setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));
    CreateActions();
    CreateTrayIcon();
    SetTrayIcon( true );
    trayIcon->show();
}

void CChamberUi::InitVariable()
{
	m_bShow = false;
}

void CChamberUi::RegisterMetaType()
{
	qRegisterMetaType<stTcpElement>("stTcpElement");
	qRegisterMetaType<stIpcElement>("stIpcElement");
	qRegisterMetaType<EN_SV_MODE>("EN_SV_MODE");
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<PASSWD_MODE>("PASSWD_MODE");
	qRegisterMetaType<QLocalSocket::LocalSocketError>("LocalSocketError");
}

void CChamberUi::InitInstance()
{
	InitStatusBar();
	m_pChmbView = new CChamberView;
	setCentralWidget( m_pChmbView );
	connect( this, &CChamberUi::SigStartChmbIpc              , m_pChmbView , &CChamberView::SlotStartChamberIpc  );
	connect( m_pChmbView, &CChamberView::SigIpcChmbCommStatus, m_pStatusBar, &CStatusBar::SlotChmbConnStatus     );
}

void CChamberUi::InitStatusBar()
{
	m_pStatusBar = new CStatusBar;
	QLabel* pVersion = new QLabel( tr("SW VER : %1 [Release:%2] ").arg(PROGRAM_VERSION).arg(PROGRAM_RELEASE_DATE), this );
	statusBar()->addWidget( pVersion );
	statusBar()->addPermanentWidget( m_pStatusBar );
}

void CChamberUi::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible())
	{
		QMessageBox::information(this, qApp->applicationName(),
									tr("The program will keep running in the system tray. To terminate the program, "
									"choose <b>Quit</b> in the context menu " "of the system tray entry."));
		hide();
		event->ignore();
	}
/*
	if(event->type() == QEvent::Close)
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, qApp->applicationName(), tr("Do you want to quit program?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			QSettings settings(CONFIG_INI_KEY, qApp->applicationName());
			settings.setValue("geometry", saveGeometry());
			settings.setValue("windowState", saveState());
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
*/
}

void CChamberUi::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	if (event->spontaneous()) return;
	if (m_bShow) return;
	m_bShow = true;

//	SetTrayIcon( true );
//	trayIcon->show();
	emit SigStartChmbIpc();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Tray Icon Application
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CChamberUi::QuitProgram()
{
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, qApp->applicationName(), "Do you want to quit program?",
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		QCoreApplication::quit();
	}
}

void CChamberUi::CreateActions()
{
	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, &QAction::triggered, this, &CChamberUi::QuitProgram);
}

void CChamberUi::SetTrayIcon(bool ok)
{
	if( ok )
	{
		const QIcon icon = QIcon("://icon/ambient.ico");
		trayIcon->setIcon(icon);
		setWindowIcon(icon);
	}
	else
	{
		const QIcon icon = QIcon("://icon/sv.ico");
		trayIcon->setIcon(icon);
		setWindowIcon(icon);
	}
	trayIcon->setToolTip( qApp->applicationName() );
}

void CChamberUi::CreateTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}
