#include "logviewer.hpp"
#include "ui_logviewer.h"

CLogViewer::CLogViewer(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CLogViewer)
{
	InitUi();
	InitValriables();
}

CLogViewer::~CLogViewer()
{
	delete ui;
}

void CLogViewer::InitUi()
{
	ui->setupUi(this);
	ui->tabWidget->clear();
	for( uint s=0;s<MAX_SLOT;s++ )
	{
		CSlotViewer* pSlot = new CSlotViewer(s);
		ui->tabWidget->addTab( pSlot,tr("SLOT%1").arg(s+1) );
	}
	CreateStatusBar();
	setWindowTitle( tr("%1").arg(ATLOGVIEWER_VERSION) );
}

void CLogViewer::CreateStatusBar()
{
	int i=0;
	const int STRETCH[MAX_STATUS_LABEL] = { 1, 1 };
	const QString TOOLTIP[MAX_STATUS_LABEL] = { "CPU1 INFO","CPU2 INFO" };
	for( i=0;i<MAX_STATUS_LABEL;i++ )
	{
		m_pStatusBarLabel[i] = new QLabel( tr("") );
		m_pStatusBarLabel[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		m_pStatusBarLabel[i]->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
		m_pStatusBarLabel[i]->setToolTip( TOOLTIP[i] );
		statusBar()->addPermanentWidget( m_pStatusBarLabel[i] , STRETCH[i] );
	}
	statusBar()->setSizeGripEnabled( false );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_CPU0  , "Ready", false );
	SetStatusBarMessage( EN_STATUS_LABEL::STATUS_BAR_CPU1  , "Ready", false );
}

void CLogViewer::SetStatusBarMessage( EN_STATUS_LABEL kind, QString msg, bool bError )
{
	QString sColor = PASS_STBAR_QCOLOR;
	if( bError ) sColor = FAIL_STBAR_QCOLOR;
	QString stylesheet = tr("QLabel { background-color : %1; color : black; }").arg(sColor);
	switch( kind )
	{
		case EN_STATUS_LABEL::STATUS_BAR_CPU0:
			m_pStatusBarLabel[STATUS_BAR_CPU0]->setText( tr("[CPU0] %1").arg(msg) );
			m_pStatusBarLabel[STATUS_BAR_CPU0]->setStyleSheet( stylesheet );
			break;
		case EN_STATUS_LABEL::STATUS_BAR_CPU1:
			m_pStatusBarLabel[STATUS_BAR_CPU1]->setText( tr("[CPU1] %1").arg(msg) );
			m_pStatusBarLabel[STATUS_BAR_CPU1]->setStyleSheet( stylesheet );
			break;
	default:
			break;
	}
}

void CLogViewer::InitValriables()
{

}

void CLogViewer::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent( event );
}

void CLogViewer::closeEvent(QCloseEvent *event)
{
	if(event->type() == QEvent::Close)
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, qApp->applicationName(), tr("Do you want to quit program?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
}
