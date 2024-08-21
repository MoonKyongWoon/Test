#include "deviceapp.hpp"
#include "deviceif.h"

CDeviceInstance::CDeviceInstance()
{
	m_bOpen = false;
	//std::cout << __FUNCTION__ << "The current thread is " << QThread::currentThread() <<std::endl;
}

CDeviceInstance::~CDeviceInstance()
{

}
void CDeviceInstance::SetProgramName(std::string name)
{

}

void CDeviceInstance::SetDeviceIf(CApi* api)
{
    m_pApi = api;
}

void CDeviceInstance::SetStop( bool bStop )
{
	if( bStop )
	{
		fprintf( stdout, "[DEVAPP] SET STOP\n" ); fflush( stdout );
		m_pApi->SET_RUNMODE( false );
		m_pApi->JUMPMODE = false;
	}
}

bool CDeviceInstance::IsStopMode()
{
	return m_pApi->GET_STOPMODE();
}

void CDeviceInstance::InitTest()
{
	std::chrono::steady_clock::time_point time_begin = std::chrono::steady_clock::now();

	m_pApi->CLR_SCANMASK();
	m_pApi->SetInitStep( true );
	m_pApi->initTest();
	m_pApi->initTestEnd();
	m_pApi->SetInitStep( false );

	std::chrono::steady_clock::time_point time_end   = std::chrono::steady_clock::now();
	long long mil = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin).count();
	if( !m_pApi->GetAutoMode() )
	{
		m_pApi->PRINTF("InitTest Elapsed Time : %lld sec %lld msec\n", mil/1000, mil%1000 );
		m_pApi->PRINTF(NULL);
	}
}

void CDeviceInstance::TestEntry()
{
	std::chrono::steady_clock::time_point time_begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point time_end   = std::chrono::steady_clock::now();
	time_begin = std::chrono::steady_clock::now();

	m_pApi->CYCLE_BEGIN();
	m_pApi->testEntry();
	m_pApi->CYCLE_END();

	time_end   = std::chrono::steady_clock::now();
	long long mil = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin).count();
	if( !m_pApi->GetAutoMode() )
	{
		m_pApi->PRINTF("TestEntry Elapsed Time : %lld sec %lld msec\n", mil/1000, mil%1000 );
	}
	else
	{
	}
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotOpenDevice(int slot, int cpu)
{
	if( cpu == 1 )
		m_bOpen = m_pApi->HwOpen( slot, CPU00 );
	else
		m_bOpen = m_pApi->HwOpen( slot, CPU01 );

	emit SigOpenDeviceResult( slot, cpu, m_bOpen );
}

void CDeviceInstance::SlotPcieValidCheck( int slot, int cpu )
{
	if( cpu == 1 )
	{
		bool bMatch = true;
		int nChip = m_pApi->SysRead( BITCHART::DRV_IOA::_VER_CHIP );
		if( CHIP1_IOA_VER != nChip ) bMatch = false;
		emit SigPcieValidCheckResult( slot, cpu, nChip, bMatch );
	}
	else
	{
		bool bMatch = true;
		int nChip = m_pApi->SysRead( BITCHART::DRV_IOA::_VER_CHIP );
		if( CHIP2_IOA_VER != nChip ) bMatch = false;
		emit SigPcieValidCheckResult( slot, cpu, nChip, bMatch );
	}
}

//Log/Data Socket port different with diag/cal and normal device pgm
void CDeviceInstance::SlotConnectToHost( int slot, int cpu, bool bTool )
{
	m_bToolPgm = bTool;
	m_pApi->SetToolPgm( bTool );
	bool bConn = m_pApi->ConnectToEwsMgr();

	emit SigConnectToHostResult( slot, cpu, bConn );
}

//Initial
void CDeviceInstance::SlotInitProgram()
{
	m_pApi->SetAutoMode( true );
	m_pApi->PRINTF( "S/W LIB VER : %s", m_pApi->GetLibVersion() );
	m_pApi->HwInit();
	m_pApi->SwInit();
	m_pApi->SetDevStatus( PROTOCOL::DEV_STATUS::DEV_PROGRAM_START_IN  );
	m_pApi->programStart();
	m_pApi->SYS_SEND_DEVPGM_VARS();	//2023.06.11 - KWMOON
	m_pApi->IS_SITE_FAULT();		//_bi_find - don't care
	m_pApi->SetDevStatus( PROTOCOL::DEV_STATUS::DEV_PROGRAM_START_OUT );
	m_pApi->SET_RUNMODE( false );
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotInitTest( stUiInitInfo initInfo )
{
	m_pApi->PRINTF("Begin InitTest");
	m_pApi->SET_MAKTEITEM( false );
	m_pApi->SET_JUMPMODE( false );
	m_pApi->SET_TESTDONE( false );
	m_pApi->SET_STOPMODE( false );	//2023.05.22 - KWMOON

	m_pApi->SET_RUNMODE( true );
	m_pApi->SET_UIINITINFO( initInfo );	//2023.03.10 - KWMOON
	InitTest();
	m_pApi->SET_RUNMODE( false );
	m_pApi->SET_MAKTEITEM( true );
	TestEntry();
	m_pApi->SET_MAKTEITEM( false );
	m_pApi->SEND_ITEM();
	m_pApi->SET_STOPMODE( true );	//2023.05.22 - KWMOON
	m_pApi->PRINTF("End InitTest %d", m_pApi->GET_STOPMODE()?0x1:0x0 );
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotTest( stUiTestInfo testinfo )
{
	m_pApi->PRINTF("Begin Test");
	m_pApi->SET_MAKTEITEM( false );
	m_pApi->SET_JUMPMODE( false );
	m_pApi->SET_TESTDONE( false );
	m_pApi->SET_STOPMODE( false );	//2023.05.22 - KWMOON

	m_pApi->SET_RUNMODE( true );
	m_pApi->SET_UITESTINFO( testinfo );	//2023.03.10 - KWMOON
	TestEntry();
	m_pApi->SET_RUNMODE( false );
	m_pApi->SET_TESTDONE( true );
	m_pApi->SET_STOPMODE( true );	//2023.05.22 - KWMOON
	m_pApi->PRINTF("End Test");
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotJumpTest( stTestItem Items )
{
	m_pApi->PRINTF("Begin Item(Jump) Test Loop:%05d", Items.nLoopCount);
	m_pApi->SET_MAKTEITEM( false );
	m_pApi->SET_TESTITEM( Items );
	m_pApi->SET_JUMPMODE( true );
	m_pApi->SET_STOPMODE( false );					//2023.05.22 - KWMOON
	m_pApi->SET_UICHMBUSE( Items.bChamberUse );		//2023.05.22 - KWMOON
	for( int i=0;i<Items.nLoopCount;i++ )
	{
		m_pApi->SET_RUNMODE( true );		//step no init
		m_pApi->PRINTF("Item(Jump) Test start:(%05d/%05d)",i+1, Items.nLoopCount );
		TestEntry();
		m_pApi->PRINTF("Item(Jump) Test end:(%05d/%05d)\n"  ,i+1, Items.nLoopCount );
		m_pApi->PRINTF(NULL);
		if( !m_pApi->GET_RUNMODE()  ) break;
	}
	m_pApi->SET_JUMPMODE( false );
	m_pApi->SET_RUNMODE( false );
	m_pApi->SET_TESTDONE( true );
    m_pApi->SET_STOPMODE( true );	//2023.05.22 - KWMOON
    m_pApi->PRINTF("End Item(Jump) Test");
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotStopTest()
{
	m_pApi->PRINTF("Set Stop");
	m_pApi->SET_RUNMODE( false );
	m_pApi->SET_JUMPMODE( false );
	m_pApi->SetDevStatus( PROTOCOL::DEV_STATUS::DEV_TEST_STOP  );
	m_pApi->PRINTF(NULL);
}

void CDeviceInstance::SlotManualRun()
{
	m_pApi->SetAutoMode( false );
	m_pApi->PRINTF( "S/W LIB VER : %s", m_pApi->GetLibVersion() );
	m_pApi->HwInit();
	m_pApi->SwInit();
	m_pApi->programStart();
	m_pApi->SYS_SEND_DEVPGM_VARS();	//2023.06.11 - KWMOON
	m_pApi->PRINTF( NULL );			//2023.09.14 - KWMOON
    Menu();
	if( !m_pApi->IS_TERMINATED() )
	{
		m_pApi->SwInit();
		m_pApi->HwInit();
		m_pApi->SET_TESTDONE( true  );
		m_pApi->PRINTF(NULL);		//2023.09.14 - KWMOON
		exit(0);
	}
	m_pApi->SET_TESTDONE( true  );
	m_pApi->PRINTF( "Manual Run Done" );
	m_pApi->PRINTF(NULL);
}

//2023.04.03 - kwmoon
void CDeviceInstance::SetTerminate()
{
	//printf("Device App :: %s\n", __FUNCTION__ );
	m_pApi->SET_TERMINATED( true );
}

bool CDeviceInstance::WaitTerminated()
{
	int i = 0;
	std::chrono::steady_clock::time_point BeginPoint = std::chrono::steady_clock::now();
	while( !m_pApi->GET_STOPMODE() )
	{
		printf( "[%04d] %s Wait for test done.... %d\n", i++, __FUNCTION__, m_pApi->GET_STOPMODE()?0x1:0x0 ); fflush(stdout);
		std::chrono::steady_clock::time_point EndPoint = std::chrono::steady_clock::now();
		long long seconds  = std::chrono::duration_cast<std::chrono::seconds>(EndPoint - BeginPoint).count();
		if( seconds > 10 ) break;
		QThread::msleep( 1000 );
		QThread::msleep( 0 );
	}
	m_pApi->SwInit(false);
	m_pApi->HwInit();

	return true;
}

void CDeviceInstance::Menu()
{
    std::string sInput = "";
	static int iNO = 0;
    while ( true )
    {
		printf("#####################################");
        printf("\n\n\t - MENU - \n\n");
        printf("\t1. Init test\n");
        printf("\t2. Test entry\n");
        printf("\tq. Quit\n\n");
        printf("\tInput (%d): ", iNO);
        std::getline( std::cin, sInput );
        printf("#####################################\n");

        if ( !strcmp(sInput.c_str(), "q") || !strcmp(sInput.c_str(), "Q") )
        {
            break;
        }
        else
        {
			if( sInput.length() > 0 )
			{
				int no = strtol(sInput.c_str(), NULL, 10);
				if( no >0 && no < 3 )
					iNO = no;
				else
					continue;

			}
			m_pApi->SET_STOPMODE( false  );
			m_pApi->SET_TESTDONE( false );
			switch( iNO )
            {
            case 1:
				m_pApi->SET_RUNMODE ( true );
				InitTest();
				m_pApi->SET_RUNMODE ( false );
				break;
			case 2:
				m_pApi->SET_RUNMODE ( true );
				TestEntry();
				m_pApi->SET_RUNMODE ( false );
				break;
            default:
                printf("%d Invalid Cmd Number...\n", iNO );
                break;
			}
			m_pApi->SET_STOPMODE( true  );
		}
		if( m_pApi->IS_TERMINATED() ) break;
	}
}
