#include "logger.hpp"

CLogger::CLogger( PROTOCOL::SOURCE_TYPE type )
{
	m_SubType = 0;
	m_AppType = type;
	m_LogDir  = QDir::currentPath();
	m_bDebugPrint = false;
	InitLogDir();
}

CLogger::CLogger( PROTOCOL::SOURCE_TYPE type, uint sub )
{
	m_AppType = type;
	m_SubType = sub;
	m_LogDir  = QDir::currentPath();
	m_bDebugPrint = false;
	InitLogDir();
}

CLogger::~CLogger()
{

}

void CLogger::InitLogDir()
{
	char* env_run = getenv(ATRUN_DIR_ENV);
	QString os_path = "";
	QString run_path = "";
	if( env_run != NULL )
	{
		run_path = QString(env_run);
	}
	else
	{
		run_path = tr("%1/%2").arg(QDir::home().absolutePath()).arg("atsystem");
	}
	SetLogDir( run_path );
}

void CLogger::SetDebugPrint(bool bprint)
{
	m_bDebugPrint = bprint;
}

void CLogger::SetLogDir(QString rundir)
{
	m_LogRoot = rundir;
	QString logdir  =  tr("%1/%2").arg(rundir).arg("log");
	QString sysdir  =  tr("%1/%2").arg(logdir).arg(LOG_SYSTEM_DIR);
	QString chmbdir =  tr("%1/%2").arg(logdir).arg(LOG_CHAMBER_DIR);
	QString sitedir =  tr("%1/%2").arg(logdir).arg(LOG_SLOT_DIR);

	QDir dir(logdir);
	if( !dir.exists( logdir)  )	dir.mkdir( logdir );
	if( !dir.exists( sysdir)  )	dir.mkdir( sysdir );
	if( !dir.exists( chmbdir) )	dir.mkdir( chmbdir );
	if( !dir.exists( sitedir) )	dir.mkdir( sitedir );

	m_LogSystem  = sysdir;
	m_LogChamber = chmbdir;
	m_LogSite    = sitedir;
	Init();
}

void CLogger::Init()
{
	switch ( m_AppType )
	{
	case PROTOCOL::_ATMAIN_UI:
		m_AppName = tr("mainui");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATMGR:
		m_AppName = tr("testmgr");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATLODER:
		m_AppName = tr("loader");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATMAIN_UI_CHMB:
		m_AppName = tr("mainui-chmb");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATCHMB_UI:
		m_AppName = tr("chamberui");
		m_LogDir  = m_LogChamber;
		break;
	case PROTOCOL::_ATCHMB_COMM:
		m_AppName = tr("chambercomm");
		m_LogDir  = m_LogChamber;
		break;
	case PROTOCOL::_ATPPSMON_UI:
		m_AppName = tr("ppsmonui");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATPPSMON_COMM:
		m_AppName = tr("ppsmoncomm");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATSYSMON_UI:
		m_AppName = tr("sysmonui");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATSYSMON_COMM:
		m_AppName = tr("sysmoncomm");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATTMS_UI:
		m_AppName = tr("tmsui");
		m_LogDir  = m_LogSystem;
		break;
	//2024.04.26 - KWMOON
	case PROTOCOL::_ATTMS_HSMS:
		m_AppName = tr("tmshsms");
		m_LogDir  = m_LogSystem;
		break;
	case PROTOCOL::_ATSLOT01:
	case PROTOCOL::_ATSLOT02:
	case PROTOCOL::_ATSLOT03:
	case PROTOCOL::_ATSLOT04:
	case PROTOCOL::_ATSLOT05:
	case PROTOCOL::_ATSLOT06:
	case PROTOCOL::_ATSLOT07:
	case PROTOCOL::_ATSLOT08:
	case PROTOCOL::_ATSLOT09:
	case PROTOCOL::_ATSLOT10:
	case PROTOCOL::_ATSLOT11:
	case PROTOCOL::_ATSLOT12:
	case PROTOCOL::_ATSLOT13:
	case PROTOCOL::_ATSLOT14:
	case PROTOCOL::_ATSLOT15:
	case PROTOCOL::_ATSLOT16:
        m_AppName = tr("slot%1_%2").arg( static_cast<int>(m_AppType-PROTOCOL::_ATSLOT01+1),2,10,QChar('0') ).arg(m_SubType+1);
		m_LogDir  = m_LogSite;
		break;
	//2024.08.19 - KWMOON
	case PROTOCOL::_ATCHMB_UI_COMPRESS:
		m_AppName = tr(".compressure");
		m_LogDir  = m_LogChamber;
		break;
	//2024.08.19 - KWMOON
	case PROTOCOL::_ATCHMB_UI_ALARM:
		m_AppName = tr("alarm");
		m_LogDir  = m_LogChamber;
		break;
	//2024.08.19 - KWMOON
	case PROTOCOL::_ATCHMB_UI_PCW:
		m_AppName = tr("pcw");
		m_LogDir  = m_LogChamber;
		break;
	default:
		break;
	}
}

void CLogger::SaveLog( LOG_TYPE t, QString s )
{
	//QMutexLocker locker( &m_mutex );
	QString msg = "";
	switch ( t )
	{
		case LOG_TYPE::_NORMAL_LOG:
			msg += UTIL::DATE::yyMMDDHHmmss(true) + "[ INFO]" + s;
			break;
		case LOG_TYPE::_ERROR_LOG:
			msg += UTIL::DATE::yyMMDDHHmmss(true) + "[ERROR]" + s;
			break;
		case LOG_TYPE::_FAULT_LOG:
			msg += UTIL::DATE::yyMMDDHHmmss(true) + "[FAULT]" + s;
		break;
	}
	QString sdate   = UTIL::DATE::yyyyMMDD();
	QString sDirLog = tr("%1/%2").arg(m_LogDir).arg(sdate);
	QDir dirLog( sDirLog );
	if( !dirLog.exists())
		dirLog.mkdir( sDirLog );
	QString savefile = tr("%1/%2-%3%4").arg(sDirLog).arg(m_AppName).arg(sdate).arg(LOG_FILE_EXTENSION);
	UTIL::FILE::AppendFile( savefile, msg );

	if( m_bDebugPrint )
		fprintf( stdout, "%s\n", msg.toStdString().c_str() );
}

//2023.08.10 - KWMOON
void CLogger::SaveSlotFile( QString msg )
{
    QString sdate   = UTIL::DATE::yyyyMMDD();
    QString sDirLog = tr("%1/%2").arg(m_LogDir).arg(sdate);
    QDir dirLog( sDirLog );
    if( !dirLog.exists())
	{
        dirLog.mkdir( sDirLog );
	}
	//2023.08.10 - KWMOON(Modify - date remove at filename )
	QString savefile = tr("%1/%2%3").arg(sDirLog).arg(m_AppName).arg(LOG_FILE_EXTENSION);
    UTIL::FILE::AppendFile( savefile, msg, false );
}

void CLogger::Append ( LOG_TYPE t, const char* str, ... )		//NORM, ERR, FAULT
{
	va_list	ap;
	char buf[4096];
	memset( buf, 0x0, sizeof(buf) );

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	QString s = QString(buf);
	SaveLog( t, s );
}

void CLogger::SlotLogMessage( LOG_TYPE t, QString s )
{
	SaveLog( t, s );
}

/*
//2023.08.10 - KWMOON(modify save routine)
void CLogger::SlotLogSlotMessage( uint slot, uint runner, LOG_TYPE t, QString s )
{
	uint myslot = (m_AppType-PROTOCOL::_ATSLOT01);
    if( (myslot != slot) || (runner != m_SubType) ) return;

    QString msg = "";
    QStringList list = s.split("\n");
    for(QString str : list )
    {
        if( str.trimmed() == "" ) continue;

        switch ( t )
        {
            case LOG_TYPE::_NORMAL_LOG:
                msg += UTIL::DATE::yyMMDDHHmmss(true) + "[ INFO]" + str + LF;
                break;
            case LOG_TYPE::_ERROR_LOG:
                msg += UTIL::DATE::yyMMDDHHmmss(true) + "[ERROR]" + str + LF;
                break;
            case LOG_TYPE::_FAULT_LOG:
                msg += UTIL::DATE::yyMMDDHHmmss(true) + "[FAULT]" + str + LF;
            break;
        }
    }
    SaveSlotFile( msg );
}
*/

//2023.08.13 - KWMOON(modify save routine)
void CLogger::SlotLogSlotMessage( uint slot, uint runner, LOG_TYPE t, QString msg )
{
	uint myslot = (m_AppType-PROTOCOL::_ATSLOT01);
	if( (myslot != slot) || (runner != m_SubType) ) return;

	SaveSlotFile( msg );
}
