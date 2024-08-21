#include <QCoreApplication>
#include <cstdlib>
#include <csignal>
#include <signal.h>
#include "chmbmanager.hpp"


bool ATChmbComm_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATCHMBCOMM_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATCHMBCOMM_NAME ) )
		{
			printf( "%s is already running!\n",  ATCHMBCOMM_NAME );
			exit(-1);
		}
		if( UTIL::PROCESS::writePid( ATCHMBCOMM_NAME, mypid ) )
		{
			return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::IsRun( ATCHMBCOMM_NAME, "") )
		{
			printf( "%s is already running!\n",  ATCHMBCOMM_NAME );
			exit(-1);
		}
		if( UTIL::PROCESS::writePid( ATCHMBCOMM_NAME, mypid ) )
			return false;
	}
	return true;
}


CChmbManager* pChmbMgr;

void SignalHandler( int signum )
{
	switch( signum )
	{
		case SIGINT:     //Ctrl+C
		case SIGQUIT:    //Ctrl+'\'
		case SIGSEGV:    //Segment fault
		case SIGTERM:
		{
			qApp->exit( signum );
		}
		break;
	}
}

int main( int argc, char* argv[])
{
	ATChmbComm_RunCheck();

	signal( SIGINT ,  SignalHandler );
	signal( SIGQUIT,  SignalHandler );
	signal( SIGSEGV,  SignalHandler );
	signal( SIGTERM,  SignalHandler );

	QCoreApplication a(argc, argv);	
	CChmbManager manager;
	pChmbMgr = &manager;
	manager.run( argc, argv );
	return a.exec();
}
