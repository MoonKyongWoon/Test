#include "logreader.hpp"

CLogReader::CLogReader( uint slot, uint cpu, QObject *parent) : QObject(parent)
{
	m_nSlotNo = slot;
	m_nCpuNo  = cpu;
	m_bBusy   = false;
}

CLogReader::~CLogReader()
{

}

void CLogReader::LogFileSet()
{
	QString run_path  = "";
	char* env_run  = getenv(ATRUN_DIR_ENV);
	if( env_run != NULL )
	{
		run_path = QString(env_run);
	}
	else
	{
		run_path = tr("%1/%2").arg(QDir::home().absolutePath()).arg("atsystem");
	}
	QString sdate  = UTIL::DATE::yyyyMMDD();
	QString path   = tr("%1/%2/%3/%4").arg(run_path).arg("log").arg(LOG_SLOT_DIR).arg(sdate);
	QString name   = tr("slot%1_%2%3").arg(m_nSlotNo+1,2,10,QChar('0')).arg(m_nCpuNo+1).arg(LOG_FILE_EXTENSION);
	QString full   = tr("%1/%2").arg(path).arg(name);

	QFileInfo info(full);
	m_fileinfo.ymd = sdate;
	m_fileinfo.filesize   = info.size();
	m_fileinfo.lastoffset = 0;
	m_fileinfo.readsize   = 0;
	m_fileinfo.filename = name;
	m_fileinfo.filepath = path;
	m_fileinfo.filefullname = full;
	emit SigLogFileName(full);
	emit SigUpdateStatus(0,0);
}

bool CLogReader::IsDateChanged()
{
	QString sdate  = UTIL::DATE::yyyyMMDD();
	if( m_fileinfo.ymd != sdate ) return true;
	return false;
}

void CLogReader::SlotStandBy()
{
	LogFileSet();
}

void CLogReader::SlotReadStart()
{
	if( m_bBusy ) return;

	if( IsDateChanged() )
		LogFileSet();

	FILE* fp = ::fopen( m_fileinfo.filefullname.toStdString().c_str(), "r" );
	if( !fp )
	{
		emit SigFileStatus(false);
		return;
	}
	fseek( fp, 0, SEEK_END);
	m_fileinfo.filesize = ftell( fp );
	fseek( fp, 0, SEEK_SET);
	if( m_fileinfo.filesize < 1 )
	{
		::fclose(fp);
		emit SigFileStatus(false);
		return;
	}

	qint64 gap = m_fileinfo.filesize - m_fileinfo.lastoffset;
	if( gap < 0 )
	{
		::fclose(fp);
		LogFileSet();
		emit SigUpdateStatus( m_fileinfo.lastoffset, m_fileinfo.filesize );
		emit SigFileStatus(false);
		return;
	}
	else if( gap == 0 )
	{
		emit SigUpdateStatus( m_fileinfo.lastoffset, m_fileinfo.filesize );
		::fclose(fp);
		return;
	}

	qint64 toread = 0;
	qint64 readsize = 0;
	qint64 readloop = 0;
	if( gap < ONCE_READ_SIZE )
	{
		toread = gap;
	}
	else
	{
		if( m_fileinfo.lastoffset == 0 )	//first read
		{
			m_fileinfo.lastoffset = (gap-ONCE_READ_SIZE);
			toread  = ONCE_READ_SIZE;
		}
		else
		{
			readloop = (gap/(ONCE_READ_SIZE));
			if( readloop > 10 ) readloop = 10;		//size limit
			toread   = readloop*(ONCE_READ_SIZE);
		}
	}
	fseek( fp, m_fileinfo.lastoffset, SEEK_CUR);
	readsize = toread;

	m_bBusy = true;
	char* pLog  = new char[toread+1];
	char* pBuff = pLog;
	memset( pLog, 0x0, toread+1 );
	while( toread > 0 )
	{
		qint64 readbyte = fread( pBuff, sizeof(char), toread, fp );
		if( readbyte < 0 )
		{
			m_bBusy = false;
			delete[] pLog;
			::fclose(fp);
			emit SigFileStatus(false);
			return;
		}
		m_fileinfo.lastoffset += readbyte;
		pBuff += readbyte;
		toread -= readbyte;
	}
	pLog[readsize] = 0;
	QString sLog = QString(pLog);
	emit SigLogData( sLog );
	delete[] pLog;
	pLog = NULL;
	::fclose(fp);
	emit SigUpdateStatus( m_fileinfo.lastoffset, m_fileinfo.filesize );
	emit SigFileStatus(true);
	m_bBusy = false;
}

void CLogReader::SlotReadStop()
{

}
