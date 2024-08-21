#include "../include/devmgr.hpp"

CDevMgr::CDevMgr(QObject *parent) : QObject(parent)
{
	m_mapDev.clear();
	m_pExtract = nullptr;
	m_sDevice       = "";
	m_sRunDir       = "";
	m_sDownloadDir  = "";
	m_bTwoDeviceRun = false;
	m_DevExpectAck  = EXPECT_NONE;
	m_nCpuAck = 0x3;	//CPU1 & CPU2
	m_nCpuSum = 0x0;
}

CDevMgr::~CDevMgr()
{
	PopProcess( PROTOCOL::EN_PGM_RUN0 );
	if( m_bTwoDeviceRun )
		PopProcess( PROTOCOL::EN_PGM_RUN1 );
}

void CDevMgr::SlotProgramInfo( QString path, QString fname )
{
	QString org = tr("%1/%2").arg(path).arg(fname);
	m_sDevice   = tr("%1/%2").arg(m_sRunDir).arg(fname);
}

void CDevMgr::SetRunDir(QString sdir)
{
	m_sRunDir = sdir;
}

QProcess::ProcessState CDevMgr::Execute(uint runner)	//cpu
{
	QString program        = QString(m_sDevice);
	QStringList parameters = { "-id", tr("%1").arg(m_nMySlot+1), "-cpu", tr("%1").arg(runner+1), "-auto" };
	QProcess* process = new QProcess;
	connect( process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this  , &CDevMgr::SlotDevPgmFinished  );
    //2023.03.28 - kwmoon(comment)
    //connect( process, &QProcess::readyReadStandardOutput                           , this  , &CDevMgr::SlotDevPgmStdOutput );

	if( !UTIL::VALIDATION::IsExecutable( program ) )
	{
		emit SigLogMessage( _ERROR_LOG, tr("Device Program '%1' is not executable file!").arg(program) );
		return QProcess::ProcessState::NotRunning;
	}

	QString sParam ="";
	foreach (const QString &str, parameters)
	{
		sParam += str + " ";
	}
	process->setWorkingDirectory( m_sRunDir );
	process->start( program, parameters );
	process->waitForStarted(3000);

	emit SigLogMessage( _NORMAL_LOG,
						tr("[SLOT%1][PGM%2] %3 %4 Execute [PID:%5]!").arg(m_nMySlot+1).arg(runner+1).arg(program).arg(sParam).arg(process->processId()) );

	PushProcess( runner, process );
	return process->state();
}

bool CDevMgr::Terminate(uint runner)
{
	return PopProcess( runner );
}

//not use
bool CDevMgr::Extract( QString file )
{
	int nWait = 0;
	QString source  = tr("%1/%2").arg(m_sDownloadDir).arg(file);

	if( !source.toLower().contains("gz") ) return true;

	QString dest    = m_sRunDir;
	QString program = "tar";
	QStringList parameter = { "xzvf", source, "--directory", dest };

	emit SigLogMessage( _NORMAL_LOG, tr("%1").arg(program) );
	m_pExtract = new QProcess();
	m_pExtract->setWorkingDirectory(m_sRunDir);
	m_pExtract->start( program, parameter);    //, QStringList() << parameter );

	if( !m_pExtract->waitForStarted() )
	{
		emit SigLogMessage( _ERROR_LOG, tr("%1 start error!").arg(source) );
		return false;
	}
	const int RETRY_COUNT = 5;
	do
	{
		if( m_pExtract->waitForFinished(3000) ) break;
		if( m_pExtract->state() == QProcess::Running ) continue;
	}while( ++nWait < RETRY_COUNT );
	if( nWait >= RETRY_COUNT )
	{
		emit SigLogMessage( _ERROR_LOG, tr("%1 uncompress error!").arg(source) );
		return false;
	}
	delete m_pExtract;
	return true;
}

bool CDevMgr::PushProcess( uint runner, QProcess* process )
{
	QProcess* previous = nullptr;
	if( m_mapDev.contains(runner) )
		previous = m_mapDev.value(runner);

	if( previous )
		PopProcess( runner );
	m_mapDev[runner] = process;
	return true;
}

bool CDevMgr::PopProcess(uint runner)
{
	QProcess* previous = nullptr;
	if( !m_mapDev.contains(runner) ) return false;

	previous = m_mapDev.value(runner);
	m_mapDev.remove( runner );
	previous->terminate();
	previous->waitForFinished();		//2023.05.22 - KWMOON( 1500 -> default 30000 change )
	emit SigLogMessage( _NORMAL_LOG, tr("[SLOT%1][PGM%2] Terminate!").arg(m_nMySlot+1).arg(runner+1) );
	return true;
}

uint CDevMgr::GetProcessCpu(QProcess* handle)
{
	uint key = CPU00;
	QMap<uint, QProcess*>::Iterator it;
	for( it=m_mapDev.begin();it!=m_mapDev.end();it++ )
	{
		QProcess* value = it.value();
		if( handle == value )
		{
			key = it.key();
			break;
		}
	}
	return key;
}

void CDevMgr::InitAckSum( DEV_EXPECT_ACK expect_ack )
{
	m_DevExpectAck = expect_ack;
	m_nCpuSum = 0x0;
}

void CDevMgr::SlotDevPgmFinished(int exit, QProcess::ExitStatus stat )
{
	QProcess* event = qobject_cast<QProcess*>(QObject::sender());
	uint runner = GetProcessCpu( event );
	PopProcess( runner );
	emit SigLogMessage( _NORMAL_LOG, tr("[SLOT%1][PGM%2] %3 Finished!").arg(m_nMySlot+1).arg(runner+1).arg(m_sDevice) );

	if( m_DevExpectAck == DEV_EXPECT_ACK::EXPECT_RUN )
		emit SigPgmFinished( runner );
}

void CDevMgr::SlotDevPgmStdOutput()
{
	QProcess* process = qobject_cast<QProcess*>(QObject::sender());
	if( process )
	{
		fprintf(stdout, "%s", process->readAllStandardOutput().toStdString().c_str() );
		fflush( stdout );
	}
}

/*
 * not use
void CDevMgr::SlotDevPgmErrorOccurred(QProcess::ProcessError code)
{

	QProcess* event = qobject_cast<QProcess*>(QObject::sender());
	uint runner = GetProcessCpu( event );

	switch ( code )
	{
		case QProcess::FailedToStart:
			if( m_DevExpectAck == EXPECT_RUN )
			{
				emit SigLogMessage( _ERROR_LOG, tr("[PGM%1] error [%2]").arg(runner).arg(event->errorString()) );
			}
			break;
		case QProcess::Crashed:
			emit SigPgmFinished( QProcess::Crashed );
			emit SigLogMessage( _ERROR_LOG, tr("[PGM%1] runtime error [%2]").arg(runner).arg(event->errorString()) );
			break;
		default:
			emit SigPgmFinished( QProcess::UnknownError );
			break;
	}
}
*/

void CDevMgr::SlotSlotCpuInfo(int slot, int cpux2, int pgmx2)
{
	if( slot > 0 ) slot = slot-1;
	m_nMySlot = slot;
	if( cpux2 == 1 )
	{
		if( pgmx2 )
		{
			m_nCpuAck = 0x3;
			m_bTwoDeviceRun = true;
		}
		else
		{
			m_nCpuAck = 0x1;
			m_bTwoDeviceRun = false;
		}
	}
	else
	{
		m_nCpuAck = 0x1;
		m_bTwoDeviceRun = false;
	}
	QString sHead = tr("[%1]").arg("DEVMGR", 7, QChar(' ') );
	emit SigLogMessage( _NORMAL_LOG, tr("%1 SLOT_NO:%2, CPU_PER_SLOT:%3, PGM_PER_SLOT:%4")
						.arg(sHead).arg(slot).arg(cpux2).arg(pgmx2) );
}

void CDevMgr::SlotRunDevice(bool bRerun)
{
	QProcess::ProcessState stat1 = QProcess::ProcessState::NotRunning;
	QProcess::ProcessState stat2 = QProcess::ProcessState::NotRunning;

	InitAckSum( DEV_EXPECT_ACK::EXPECT_QUIT );
	Terminate( PROTOCOL::EN_PGM_RUN0 );
	Terminate( PROTOCOL::EN_PGM_RUN1 );

	InitAckSum( DEV_EXPECT_ACK::EXPECT_RUN );
	stat1 = Execute( PROTOCOL::EN_PGM_RUN0 );
	if( m_bTwoDeviceRun )
		stat2 = Execute( PROTOCOL::EN_PGM_RUN1 );

	if( (stat1 != QProcess::ProcessState::NotRunning) && (stat2 != QProcess::ProcessState::NotRunning) )
	{
		if( !bRerun ) emit SigPgmRunAck  ( true, "Success!" );
		else          emit SigPgmRerunAck( true, "Success!" );
	}
	else
	{
		QString msg = "";
		if(stat1 == QProcess::ProcessState::NotRunning ) msg += "RUN#1 NG! ";
		if(stat2 == QProcess::ProcessState::NotRunning ) msg += "RUN#2 NG! ";
		if( !bRerun ) emit SigPgmRunAck  ( false, msg );
		else          emit SigPgmRerunAck( false, msg );
	}
	emit SigLogMessage( _NORMAL_LOG, tr("[DEVM][INFO] PGM1 STAT : %1").arg(stat1) );
	emit SigLogMessage( _NORMAL_LOG, tr("[DEVM][INFO] PGM2 STAT : %1").arg(stat2) );
}

void CDevMgr::SlotTerminateDevice( bool bAck )
{
	InitAckSum( DEV_EXPECT_ACK::EXPECT_QUIT );
	PopProcess( CPU00 );
	if( m_bTwoDeviceRun )
		PopProcess( CPU01 );

	QString msg = "Unload Success";
	bool bOk = true;
	if( m_mapDev.size() > 0 )
	{
		bOk = false;
		msg = "Unload NG!";
	}
	if( bAck )
		emit SigPgmUnloadAck( bOk, msg );
}
