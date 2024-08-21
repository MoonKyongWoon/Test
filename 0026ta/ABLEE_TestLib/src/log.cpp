#include "log.hpp"

extern CSys       g_sys;
extern CDevSocket g_logsock;
extern CDevSocket g_devsock;

CLog::CLog()
{
	Init();
}

void CLog::Init()
{
	m_mapString[_NORMAL ] = "LOG";
	m_mapString[_HW     ] = "HW";
	m_mapString[_PPS    ] = "PPS";
	m_mapString[_PG     ] = "PG";
	m_mapString[_DBM    ] = "DBM";
	m_mapString[_UBM    ] = "UBM";
	m_mapString[_DFM    ] = "DFM";
	m_mapString[_TG     ] = "TG";
	m_mapString[_CYP    ] = "CYP";
	m_mapString[_MODULE ] = "MODULE";
	m_mapString[_BIB    ] = "BIB";
	m_mapString[_SCAN   ] = "SCAN";
	m_mapString[_MASK   ] = "MASK";
	m_mapString[_TEMP   ] = "TEMP";

	ClearError();
}

void CLog::ClearError()
{
	m_vecLog.clear();
}

std::string CLog::GetDate()
{
	time_t tNow;
	struct tm* t;
	char   szLine[1024];

	tNow = time(NULL);
	t = localtime( &tNow );
	sprintf(szLine, "[%02d/%02d %02d:%02d:%02d]", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
	return std::string( szLine );
}


/*
========================================================================
//Add for LPRINT update include datetime in devpgm
========================================================================
std::string GetDate()
{
	time_t tNow;
	struct tm* t;
	char   szLine[1024];

	tNow = time(NULL);
	t = localtime( &tNow );
	sprintf(szLine, "[%02d/%02d %02d:%02d:%02d]", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
	return std::string( szLine );
}

void Printf( const char* format, ... )
{
	const int LIMIT_SIZE = 192;
	static char szBuffer[256] = {0x0,};
	static int  nBufferSize = 0;

	va_list	ap;
	if( (format == NULL) || (nBufferSize>LIMIT_SIZE) )
	{
		if( nBufferSize > 0 )
		{
			fprintf( stdout, "%s", szBuffer ); fflush( stdout );
			memset( szBuffer, 0x0, sizeof(szBuffer) );
			nBufferSize = 0;
		}
		if( format == NULL )
			return;
	}
	char szDate [64]={0x0,};
	char szLog [256]={0x0,};
	char szSum [512]={0x0,};
	memset( szDate, 0x0, sizeof(szDate) );
	memset( szLog , 0x0, sizeof(szLog)  );
	memset( szSum , 0x0, sizeof(szSum)  );

	snprintf( szDate, sizeof(szDate), "%s", GetDate().c_str() );
	va_start(ap, format);
	int nLogChar = vsnprintf(szLog, 256, format, ap);
	va_end(ap);
	snprintf( szSum, 512, "%s%s\n", szDate, szLog );

	snprintf( szBuffer + nBufferSize, LIMIT_SIZE, "%s", szSum );

	if( strlen(szSum) > 0 )
	{
		nBufferSize += strlen(szSum) ;
		szBuffer[nBufferSize]=0;
	}
}
*/

//2023.08.13 - KWMOON
void CLog::Print( const char* format, ... )
{
	const int PLIMIT_SIZE  = 4096;
	static char szPrintBuf[ 8192] = {0x0,};
	static char szPrintSum[10000] = {0x0,};
	static int  nPrintSum = 0;

	if( (format == NULL) || (nPrintSum>PLIMIT_SIZE) )
	{
		if( nPrintSum > 0 )
		{
			char szDate [64]={0x0,};
			memset( szDate, 0x0, sizeof(szDate) );
			memset( szPrintSum , 0x0, sizeof(szPrintSum)  );
			snprintf( szDate, sizeof(szDate), "%s", GetDate().c_str() );
			snprintf( szPrintSum, sizeof(szPrintSum), "%s\n%s", szDate, szPrintBuf );
			int nSummarySize = strlen(szDate) + nPrintSum;
			szPrintSum[nSummarySize] = 0;
			if( g_sys.GetAutoMode() )
			{
				g_logsock.SendLogPrint( szPrintSum, nSummarySize );
			}
			else
			{
				fprintf( stdout, "%s", szPrintBuf ); fflush( stdout );
			}
			memset( szPrintBuf, 0x0, sizeof(szPrintBuf)  );
			memset( szPrintSum, 0x0, sizeof(szPrintSum) );
			nPrintSum = 0;
		}
		if( format == NULL )
			return;
	}
	va_list	ap;
	va_start(ap, format);
	int nChars = vsnprintf(szPrintBuf + nPrintSum, PLIMIT_SIZE, format, ap);
	va_end(ap);
	if( nChars > 0 )
	{
		nPrintSum += nChars ;
		szPrintBuf[nPrintSum] = 0;
		//printf("[\n%s\n] size : %d\n", szPrintBuf, nPrintSum );
	}
}

void CLog::Printf( const char* format, ... )
{
	//const int LIMIT_SIZE = 8192;
	//static char szBuffer[10240] = {0x0,};
	const int FLIMIT_SIZE = 16384;
    static char szBuffer[20480] = {0x0,};
    static int  nBufferSize = 0;

	if( (format == NULL) || (nBufferSize>FLIMIT_SIZE) )
	{
		if( nBufferSize > 0 )
		{
			if( g_sys.GetAutoMode() )
			{
				g_logsock.SendLogPrint( szBuffer, nBufferSize );
			}
			else
			{
				fprintf( stdout, "%s", szBuffer ); fflush( stdout );
			}
			memset( szBuffer, 0x0, sizeof(szBuffer) );
			nBufferSize = 0;
		}
		if( format == NULL )
			return;
	}
	char szDate [ 64]={0x0,};
	char szLog [1024]={0x0,};
	char szSum [2048]={0x0,};
	memset( szDate, 0x0, sizeof(szDate) );
	memset( szLog , 0x0, sizeof(szLog)  );
	memset( szSum , 0x0, sizeof(szSum)  );
	snprintf( szDate, sizeof(szDate), "%s", GetDate().c_str() );
	va_list	ap;
	va_start(ap, format);
	int nChars = vsnprintf(szLog, 1024, format, ap);
	va_end(ap);
	snprintf( szSum, 2048, "%s%s", szDate, szLog );
	snprintf( szBuffer + nBufferSize, FLIMIT_SIZE, "%s", szSum );
	if( nChars > 0 )
	{
		nBufferSize += strlen(szSum) ;
		szBuffer[nBufferSize]=0;
	}
}

void CLog::Printf( DEV_LOG_TYPE type, const char* sfunc, const char* str, ... )
{
	va_list	ap;
    char buf[8192];

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	atstring serr;
	serr.Format("%s(%s) : %s\n", m_mapString[type].c_str(), sfunc, buf);
	Printf( serr.c_str() );
}

void CLog::EPrintf( DEV_LOG_TYPE type, const char* sfunc, const char* str, ... )
{
	va_list	ap;
    char buf[8192];

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	atstring serr;
	serr.Format("%s(%s) : err %s\n", m_mapString[type].c_str(), sfunc, buf);
	Printf( serr.c_str() );
}

int CLog::GetErrorCount()
{
	return m_vecLog.size();
}

const char* CLog::GetError(int idx)
{
	if (idx >= (int)m_vecLog.size())
		return NULL;

	return m_vecLog.at(idx).c_str();
}

void CLog::RuntimeError( DEV_LOG_TYPE type, const char* sfunc, const char* str, ... )
{
	va_list	ap;
	char buf[8192];

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);

	atstring serr;
	serr.Format("%s(%s) : %s\n", m_mapString[type].c_str(), sfunc, buf);
	RuntimeError( serr.c_str() );
	RuntimeError( NULL );
}

void CLog::RuntimeError( const char* format, ... )
{
	const int LIMIT_SIZE = 8192;
	static char szRunBuffer[10240] = {0x0,};
	static int  nRunBufferSize = 0;

	va_list	ap;
	if( (format == NULL) || (nRunBufferSize>LIMIT_SIZE) )
	{
		if( nRunBufferSize > 0 )
		{
			if( g_sys.GetAutoMode() )
			{
				g_devsock.SendRuntimeError( szRunBuffer, nRunBufferSize );
			}
			else
			{
				fprintf( stdout, "%s", szRunBuffer ); fflush( stdout );
			}
			memset( szRunBuffer, 0x0, sizeof(szRunBuffer) );
			nRunBufferSize = 0;
		}
		if( format == NULL )
			return;
	}
	va_start(ap, format);
	int nChars = vsnprintf(szRunBuffer + nRunBufferSize, LIMIT_SIZE, format, ap);
	va_end(ap);

	if( nChars > 0 )
	{
		nRunBufferSize += nChars ;
		szRunBuffer[nRunBufferSize]=0;
	}
}
