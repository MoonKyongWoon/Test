#include "cpuviewer.hpp"
#include "ui_cpuviewer.h"

CCpuViewer::CCpuViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CCpuViewer)
{
	InitUi();
	InitVariables();
}

CCpuViewer::~CCpuViewer()
{
	if( m_tmrLogView.isActive() )
		m_tmrLogView.stop();
	if( m_pLogTh )
	{
		m_pLogTh->quit();
		m_pLogTh->wait();
		m_pLogTh->deleteLater();
	}
	delete ui;	
}

void CCpuViewer::SetSlotCpuNo(uint s, uint c)
{
	m_nSlotNo = s;
	m_nCpuNo  = c;
}

void CCpuViewer::InitUi()
{
	ui->setupUi(this);
	connect( ui->spinLineCount, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CCpuViewer::SlotViewLineChanged );
	connect( ui->tbViewStop   , &QToolButton::toggled, this, &CCpuViewer::SlotViewStopToggled  );
	connect( ui->btnReload    , &QPushButton::clicked, this, &CCpuViewer::SlotBtnReloadClicked );

	QFont font;
	font.setPointSize(8);
	font.setFamily("DejaVu Sans Mono");
	ui->peLog->setFont( font );
	ui->peLog->setReadOnly(true);
	ui->peLog->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->peLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
	connect( ui->peLog, &QPlainTextEdit::customContextMenuRequested, this, &CCpuViewer::SlotCustomContextMenuRequested );
}

void CCpuViewer::InitVariables()
{
	m_bAutoScroll = true;
	m_bStart = false;
	m_pLogReader = nullptr;
	m_pLogTh = nullptr;
	m_nSlotNo = 0;
	m_nCpuNo  = 0;
	m_nViewLine = ui->spinLineCount->value();
	SetLogMaxViewLine( m_nViewLine );

	m_pixmapOK.load( ":/icon/icon/green_marble.ico" );
	m_pixmapNG.load( ":/icon/icon/red_marble.ico" );
}

void CCpuViewer::SlotScrollStatusChanged( bool checked )
{
	m_bAutoScroll = checked;
}

void CCpuViewer::SlotCustomContextMenuRequested( QPoint point )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	if( !pSender ) return;

	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;

	pAction = new QAction( tr("Auto Scroll On"), pSender );
	pAction->setCheckable( true );
	if( m_bAutoScroll ) pAction->setChecked(true);
	else                pAction->setChecked(false);
	connect( pAction, &QAction::triggered, this, &CCpuViewer::SlotScrollStatusChanged );
	pMenu->addAction( pAction );

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

void CCpuViewer::SlotCpuViewerInit()
{
	InitLogThread();
	InitTimer();
	SetLogViewEnable( true );
}

void CCpuViewer::SetLogViewEnable(bool enb)
{
	if( enb )
	{
		m_bStart = true;
		const QIcon icon = QIcon(":/icon/icon/stop_circle.ico");
		ui->tbViewStop->setChecked( true );
		ui->tbViewStop->setIcon( icon );
	}
	else
	{
		m_bStart = false;
		const QIcon icon = QIcon(":/icon/icon/run.ico");
		ui->tbViewStop->setChecked( false );
		ui->tbViewStop->setIcon( icon );
	}
}

void CCpuViewer::InitLogThread()
{
	m_pLogReader = new CLogReader( m_nSlotNo, m_nCpuNo );
	m_pLogTh = new QThread;
	m_pLogReader->moveToThread( m_pLogTh );
	connect( m_pLogTh    , &QThread::finished          , m_pLogReader, &QObject::deleteLater        );
	connect( m_pLogReader, &CLogReader::SigLogData       , this        , &CCpuViewer::SlotReadLog   );
	connect( m_pLogReader, &CLogReader::SigLogFileName  , this        , &CCpuViewer::SlotLogFile    );
	connect( m_pLogReader, &CLogReader::SigUpdateStatus , this        , &CCpuViewer::SlotReadStatus );
	connect( m_pLogReader, &CLogReader::SigFileStatus   , this        , &CCpuViewer::SlotFileStatus );

	connect( this        , &CCpuViewer::SigInitLogReader, m_pLogReader, &CLogReader::SlotStandBy    );
	connect( this        , &CCpuViewer::SigReadLog      , m_pLogReader, &CLogReader::SlotReadStart  );
	connect( this        , &CCpuViewer::SigStopLog      , m_pLogReader, &CLogReader::SlotReadStop   );
	m_pLogTh->start();

	emit SigInitLogReader();
}

void CCpuViewer::InitTimer()
{
	connect( &m_tmrLogView, &QTimer::timeout, this, &CCpuViewer::SlotTimeoutLogRefresh );
	m_tmrLogView.start( LOG_REFRESH_PERIOD * 1000 );
}

void CCpuViewer::SlotTimeoutLogRefresh()
{
	if( m_bStart )
	{
		emit SigReadLog();
	}
	else
	{
		emit SigStopLog();
	}
}

void CCpuViewer::SetLogMaxViewLine(int lines)
{
	ui->peLog->setMaximumBlockCount( lines );
}

void CCpuViewer::SlotViewLineChanged(int lines)
{
	m_nViewLine = lines;
	SetLogMaxViewLine( m_nViewLine );
}

void CCpuViewer::SlotViewStopToggled(bool checked)
{
	SetLogViewEnable( checked );
}

void CCpuViewer::SlotLogFile( QString file )
{
	ui->leCpuLogPath->setText( file );
}

void CCpuViewer::SlotReadLog(QString log)
{
	ui->peLog->appendPlainText( log );

	if( m_bAutoScroll )
	{
		QTextCursor c = ui->peLog->textCursor();
		c.movePosition(QTextCursor::End);
		ui->peLog->setTextCursor(c);
	}
}

void CCpuViewer::SlotReadStatus(qint64 read, qint64 total)
{
	qint64 read_kbyte  = (read/1024);
	qint64 total_kbyte = (total/1024);
	qint64 read_mbyte  = (read_kbyte/1024);
	qint64 total_mbyte = (total_kbyte/1024);
	if( read_mbyte > 0 )
		ui->leStatus->setText( tr("%1 MB / %2 MB").arg(read_mbyte).arg(total_mbyte) );
	else
		ui->leStatus->setText( tr("%1 KB / %2 KB").arg(read_kbyte).arg(total_kbyte) );
}

void CCpuViewer::SlotFileStatus(bool ok)
{
	if( ok )
	{
		ui->ledStatus->setPixmap( m_pixmapOK );
		ui->ledStatus->setToolTip( tr("file read ok!") );
	}
	else
	{
		ui->ledStatus->setPixmap( m_pixmapNG );
		ui->ledStatus->setToolTip( tr("file read ng!") );
	}
}

void CCpuViewer::SlotBtnReloadClicked()
{
	ui->peLog->clear();
	emit SigInitLogReader();
}
