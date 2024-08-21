#include "savedatadlg.hpp"
#include "ui_savedatadlg.h"

CSaveDataDlg::CSaveDataDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CSaveDataDlg)
{
	ui->setupUi(this);
	Init();
}

CSaveDataDlg::~CSaveDataDlg()
{
	StopMovie();
	delete ui;
}

void CSaveDataDlg::keyPressEvent(QKeyEvent *e) {
	if(e->key() != Qt::Key_Escape)
		QDialog::keyPressEvent(e);
	else {/* minimize */}
}

void CSaveDataDlg::Init()
{
	m_bAutoScroll = false;

	m_pMovie = nullptr;
	m_pMovie = new QMovie(":/icon/icon/processing.gif");
	//m_pMovie->setScaledSize( QSize(600,200) );
	ui->lblSpinner->setMovie( m_pMovie );

	QFont font = ui->peLog->font();
	font.setPointSize(9);
	font.setFamily("DejaVu Sans Mono");
	ui->peLog->setFont(font);
	ui->peLog->setReadOnly(true);
	ui->peLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
	ui->peLog->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->peLog->setMaximumBlockCount( 65535 );
	ui->peLog->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );

	connect( ui->btnSaveData, &QPushButton::clicked, this, &CSaveDataDlg::SlotBtnSaveClicked );
	connect( ui->btnHide    , &QPushButton::clicked, this, &CSaveDataDlg::SlotBtnHideClicked );
}

void CSaveDataDlg::ClearLog()
{
	ui->peLog->clear();
	ui->prgBar->setValue(0);
}

void CSaveDataDlg::AppendLog(LOG_TYPE t, QString s)
{
	QPlainTextEdit* pEditor = ui->peLog;
	if( pEditor->document()->blockCount()> 65535 )
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

	if( m_bAutoScroll )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}

	emit SigSysLogMessage( t, tr("[SAVEDATA] %1").arg(s) );
}

void CSaveDataDlg::Test()
{
	RunMovie();
}

void CSaveDataDlg::RunMovie()
{
	m_pMovie->start();
}

void CSaveDataDlg::StopMovie()
{
	m_pMovie->stop();
}

void CSaveDataDlg::SlotMakeReport()
{
	if( this->isHidden() )
		this->show();
	MakeData();
}

bool CSaveDataDlg::MakeData()
{
	ClearLog();
	if( RunSaveData() )
	{
		char* env_data = getenv(ATDATA_DIR_ENV);
		QString data_path("");
		if( env_data != NULL )
			data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
		else
			data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);
		QString uimode_ini(tr("%1/%2").arg(data_path).arg(UIMODE_INI));
		QString section_key = tr("%1/%2").arg(SECTION_UIMODE).arg(KEY_UIMODE);
		QString sValue("");
		UTIL::INI::readSettings(uimode_ini, section_key, sValue);

		if (sValue.compare("auto") == 0)
			SendSavedMap();
		RunMovie();
	}
	else
	{
		StopMovie();
	}
	return true;
}

void CSaveDataDlg::SlotBtnSaveClicked()
{
	MakeData();
}

void CSaveDataDlg::SlotBtnHideClicked()
{
	accept();
}

bool CSaveDataDlg::RunSaveData()
{
	QString program        = QString(ATSAVEDATA_NAME);
	QStringList parameters = { "" };
	QProcess* process = new QProcess;
	connect( process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this  , &CSaveDataDlg::SlotSaveDataFinished  );
	connect( process, &QProcess::readyReadStandardOutput                           , this  , &CSaveDataDlg::SlotSaveDataStdOutput );

	process->start( program, parameters );
	process->waitForStarted(3000);
	if (process->state() == QProcess::ProcessState::Running)
	{
		AppendLog( LOG_TYPE::_NORMAL_LOG, tr("START %1").arg(program) );
		return true;
	}
	else
	{
		AppendLog( LOG_TYPE::_ERROR_LOG, tr("PROCESS IS NOT RUNNING %1").arg(program) );
		return false;
	}
	return false;
}

bool CSaveDataDlg::SendSavedMap()
{
	QString program        = QString(ATSAVEMAP_NAME);
	QStringList parameters;
	parameters << "-send";

	QString cmd = program;
	for (QString& parameter: parameters)
	{
		cmd += tr(" %1").arg(parameter);
	}

	QProcess* process = new QProcess;
	connect( process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){this->SlotResultSendSavedMap(exitCode, exitStatus, cmd); process->deleteLater();} );
	connect( process, &QProcess::readyReadStandardOutput                           , this  , &CSaveDataDlg::SlotSaveDataStdOutput );
	process->start(program, parameters);
	process->waitForStarted(3000);
	if (process->state() == QProcess::ProcessState::Running)
	{
		AppendLog( LOG_TYPE::_NORMAL_LOG, tr("START %1").arg(cmd) );
		return true;
	}
	else
	{
		AppendLog( LOG_TYPE::_ERROR_LOG, tr("PROCESS IS NOT RUNNING %1").arg(cmd) );
		return false;
	}
	return false;
}


void CSaveDataDlg::SlotSaveDataFinished(int exit, QProcess::ExitStatus stat)
{
	AppendLog( _NORMAL_LOG, tr(" %1 done...[%2]").arg(ATSAVEDATA_NAME).arg(exit) );
	emit SigMakeReportDone( true );
}

void CSaveDataDlg::SlotSaveDataStdOutput()
{
	QProcess* process = qobject_cast<QProcess*>(QObject::sender());
	if( process )
	{
		QByteArray ba = process->readAllStandardOutput();
		QList<QByteArray> list = ba.split('\n');
		for( QByteArray text : list )
		{
			if( text.contains("PRG:") )
			{
				bool bok = false;
				int i = text.mid(4).toInt(&bok,10);
				ui->prgBar->setValue( i );
				continue;
			}
			if( text.trimmed() == "" ) continue;
			AppendLog( _NORMAL_LOG, text );
		}
		//fprintf(stdout, "%s", process->readAllStandardOutput().toStdString().c_str() );
		//fflush( stdout );
	}
}

void CSaveDataDlg::SlotResultSendSavedMap(int exit, QProcess::ExitStatus stat, QString cmd)
{
	AppendLog( _NORMAL_LOG, tr(" %1 done...[%2]").arg(ATSAVEMAP_NAME).arg(exit) );
}

