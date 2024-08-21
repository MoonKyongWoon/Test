#include "kill.hpp"
#include "sysver.hpp"
#include "util.hpp"
#include <QRandomGenerator>

CKill::CKill()
{

}

CKill::~CKill()
{

}

void CKill::ShowHelp()
{
	printf("atkill {program name}\n");
	printf("  program name : ablee test system program name\n");
}

int CKill::parseArgument(int argc, char* argv[] )
{
	if( argc < 2 )
	{
		ShowHelp();
		return -1;
	}
	m_sApp = argv[0];
	std::string name = argv[1];
	return KillProcess( name );
}

int CKill::KillProcess(std::string name)
{
	qint64 pid = 0;
	QString prg = name.c_str();
	if( UTIL::PROCESS::readPid(prg, pid) )
	{
		if( UTIL::PROCESS::IsRun( pid, name, "") )
		{
			if( UTIL::PROCESS::Kill( pid ) )
			{
				fprintf( stdout, "%s '%s'[pid:%lld]\n", m_sApp.c_str(), name.c_str(), pid );
				return 1;
			}
		}
		else
		{
			if( UTIL::PROCESS::Kill( name, "" ) )
			{
				fprintf( stdout, "%s '%s'[pid:%lld]\n", m_sApp.c_str(),  name.c_str(), pid );
				return 1;
			}
		}
	}
	else
	{
		if( UTIL::PROCESS::Kill( name, "" ) )
		{
			fprintf( stdout, "%s '%s'[pid:%lld]\n", m_sApp.c_str(),  name.c_str(), pid );
			return 1;
		}
	}
	fprintf( stdout, "%s '%s' process not found!\n", m_sApp.c_str(),  name.c_str() );
	return 0;
}
