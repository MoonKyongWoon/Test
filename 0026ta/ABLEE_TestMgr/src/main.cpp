#include <cstdlib>
#include <csignal>
#include <signal.h>
#include <testmgr.hpp>

CTestMgr* pTestMgr;


bool ATMgr_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATMGR_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATMGR_NAME ) )
		{
			printf( "%s is already running!\n",  ATMGR_NAME );
			exit(-1);
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATMGR_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATMGR_NAME, mypid ) )
			return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
/// \brief SignalHandler
/// \param signum
/// \details SIGKILL, SIGSTOP user can't control signal
void SignalHandler( int signum )
{
	switch( signum )
	{
		case SIGINT:     //Ctrl+C
		case SIGQUIT:    //Ctrl+'\'
		case SIGSEGV:    //Segment fault
		case SIGTERM:
		{
			pTestMgr->Terminate( signum );
		}
		break;
	}
}

int main( int argc, char* argv[])
{
    ATMgr_RunCheck();
	CTestMgr testmgr;
	pTestMgr = &testmgr;
	signal( SIGINT ,  SignalHandler );
	signal( SIGQUIT,  SignalHandler );
	signal( SIGSEGV,  SignalHandler );
	signal( SIGTERM,  SignalHandler );
	return testmgr.run( argc, argv );
}
