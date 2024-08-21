#include <cstdlib>
#include <csignal>
#include <signal.h>
#include <loader.hpp>

CLoader* pLoader;

//////////////////////////////////////////////////////////////////////////
/// \brief SignalHandler
/// \param signum
/// \details SIGKILL, SIGSTOP user can't control signal
void SignalHandler( int signum )
{
    printf( "SignalHandler:%d\n", signum );
    switch( signum )
    {
		case SIGINT:	//Ctrl+C
		case SIGQUIT:	//Ctrl+'\'
		case SIGSEGV:	//Segment fault
		case SIGTERM:	//kill -15
        {
			qApp->exit( signum );
		}
        break;
    }
}

int main( int argc, char* argv[])
{
    CLoader loader;
    pLoader = &loader;
    signal( SIGINT ,  SignalHandler );
    signal( SIGQUIT,  SignalHandler );
    signal( SIGSEGV,  SignalHandler );
	signal( SIGTERM,  SignalHandler );
	return loader.parseArguments( argc, argv );
}
