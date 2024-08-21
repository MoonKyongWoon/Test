#include "sysmonui.hpp"
#include "ui_sysmonui.h"

CSysMonUi::CSysMonUi(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSysMonUi)
{
	RegisterMetaType();
	InitUi();
	InitTab();
}

CSysMonUi::~CSysMonUi()
{
	delete ui;
	if (m_pSysPfmUi)
		delete m_pSysPfmUi;
	if (m_pSysPwrUi)
		delete m_pSysPwrUi;
}

void CSysMonUi::RegisterMetaType()
{
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<SIDE_AB>("SIDE_AB");
	qRegisterMetaType<SMPS_LINE>("SMPS_LINE");
	qRegisterMetaType<LINE_ITEM>("LINE_ITEM");
	qRegisterMetaType<SMPS_MODULE>("SMPS_MODULE");
	qRegisterMetaType<MODULE_ITEM>("MODULE_ITEM");
	qRegisterMetaType<PMBUS_COMMAND>("PMBUS_COMMAND");
	qRegisterMetaType<ERROR_CODE>("ERROR_CODE");
	qRegisterMetaType<DEVICE_ADDRESS>("DEVICE_ADDRESS");
	qRegisterMetaType<PMBUS_OPENRATION>("PMBUS_OPENRATION");
	qRegisterMetaType<PMBUS_DATA_FORMAT>("PMBUS_DATA_FORMAT");
	qRegisterMetaType<stUiRequest>("stUiRequest");
	qRegisterMetaType<stPmubusCommand>("stPmubusCommand");
	qRegisterMetaType<stPmbusReqHead>("stPmbusReqHead");
	qRegisterMetaType<stPmbusResponse>("stPmbusResponse");
	qRegisterMetaType<stPmbusError>("stPmbusError");
	qRegisterMetaType<stLineMeas>("stLineMeas");
	qRegisterMetaType<stModulelMeas>("stModulelMeas");
	qRegisterMetaType<pfm_unit_data>("pfm_unit_data");

}

void CSysMonUi::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		QMessageBox::information(this, qApp->applicationName(),
									tr("The program will keep running in the system tray. To terminate the program, "
									"choose <b>Quit</b> in the context menu " "of the system tray entry."));
		hide();
		event->ignore();
	}
}

void CSysMonUi::QuitProgram()
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

void CSysMonUi::CreateActions()
{
	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, &QAction::triggered, this, &CSysMonUi::QuitProgram);
}

void CSysMonUi::SetTrayIcon(bool ok)
{
	if( ok )
	{
		const QIcon icon = QIcon("://icon/syspwr.ico");
		trayIcon->setIcon(icon);
		setWindowIcon(icon);
	}
	else
	{
		const QIcon icon = QIcon("://icon/ablee.ico");
		trayIcon->setIcon(icon);
		setWindowIcon(icon);
	}
	trayIcon->setToolTip( qApp->applicationName() );
}

void CSysMonUi::CreateTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}

void CSysMonUi::InitUi()
{
	ui->setupUi(this);
	m_pSysPwrUi = new CSysPwr();
	m_pSysPfmUi = new CSysPfm();
	m_bShow = false;
	CreateActions();
	CreateTrayIcon();
	SetTrayIcon(true);
	trayIcon->show();
}

void CSysMonUi::InitTab()
{
	ui->tabMon->clear();
	ui->tabMon->addTab( m_pSysPfmUi, QIcon("://icon/syspwr.ico"), "PPS MON" );
	ui->tabMon->addTab( m_pSysPwrUi, QIcon("://icon/smps.ico")  , "PWR MON" );
}
