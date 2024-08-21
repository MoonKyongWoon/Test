#include "logview.hpp"
#include "ui_logview.h"

CLogView::CLogView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CLogView), m_bAutoScrollSysLog(true)
{
	InitUi();
}

CLogView::~CLogView()
{
	if( m_pSysLogger != nullptr )
	{
		delete m_pSysLogger;
		m_pSysLogger = nullptr;
	}
	delete ui;
}

void CLogView::InitUi()
{
	ui->setupUi(this);
	m_pSysLogger = new CLogger( PROTOCOL::_ATMAIN_UI );
	InitSysLogView();
}

void CLogView::InitSysLogView()
{
	QFont font = ui->teSys->font();
	font.setPointSize(9);
	font.setFamily("DejaVu Sans Mono");

	ui->teSys->setFont(font);
	ui->teSys->setReadOnly(true);
	ui->teSys->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
	ui->teSys->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->teSys->setMaximumBlockCount( MAX_LOG_LINE_NUM );
	ui->teSys->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
	connect( ui->teSys, &QPlainTextEdit::customContextMenuRequested, this        , &CLogView::SlotCustomContextMenuRequested );
	connect( this     , &CLogView::SigSaveSysLog                   , m_pSysLogger, &CLogger::SlotLogMessage                  );
}

void CLogView::SlotCustomContextMenuRequested( QPoint point )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;
	bool bSender = false;
	if( pSender == ui->teSys )
	{
		bSender = true;
		pAction = new QAction( tr("Auto Scroll On"), pSender );
		pAction->setCheckable( true );
		if( m_bAutoScrollSysLog ) pAction->setChecked(true);
		else                      pAction->setChecked(false);
		connect( pAction, &QAction::triggered, this, &CLogView::SlotScrollStatusChanged );
		pMenu->addAction( pAction );
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

void CLogView::SlotScrollStatusChanged()
{
	QAction* pSender = static_cast<QAction*>(QObject::sender());
	if( pSender->parent() == ui->teSys )
	{
		m_bAutoScrollSysLog = !m_bAutoScrollSysLog;
	}
}

void CLogView::SlotAppendSysLog( LOG_TYPE t, QString s )
{
	AppendSysLog( t, s );
	emit SigSaveSysLog( t, s );
}

void CLogView::AppendSysLog ( LOG_TYPE t, QString s )
{
	QPlainTextEdit* pEditor = ui->teSys;

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

	if( m_bAutoScrollSysLog )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}
}
