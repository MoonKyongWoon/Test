#include "mio.hpp"
#include "sysver.hpp"
#include <QRandomGenerator>

CIO::CIO()
{
	m_pDBmBuffer = nullptr;
	m_pPcieTPG0 = nullptr;
	m_pPcieTPG1 = nullptr;
}

CIO::~CIO()
{
	if( m_pDBmBuffer )
	{
		delete m_pDBmBuffer;
		m_pDBmBuffer = nullptr;
	}

	if( m_pPcieTPG0 )
	{
		if( m_pPcieTPG0->pcie_isopen() )
		{
			m_pPcieTPG0->pcie_close();
			delete m_pPcieTPG0;
			m_pPcieTPG0 = nullptr;
		}
	}
	if( m_pPcieTPG1 )
	{
		if( m_pPcieTPG1->pcie_isopen() )
		{
			m_pPcieTPG1->pcie_close();
			delete m_pPcieTPG1;
			m_pPcieTPG1 = nullptr;
		}
	}
}

bool CIO::InitPcie()
{
    bool bTpg0, bTpg1;
    m_pPcieTPG0 = new CPcie();
    m_pPcieTPG1 = new CPcie();

    m_pPcieTPG0->SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
    m_pPcieTPG1->SetPcieTPG( PCIE_DEVICE::PCIE_TPG1 );

    bTpg0 = m_pPcieTPG0->pcie_open();
    bTpg1 = m_pPcieTPG1->pcie_open();
    return bTpg0&bTpg1;
}

bool CIO::OpenPcie(int cpu)
{
	if( (cpu < 1) || (cpu > 3) ) return false;
	bool bTpg;
	if( cpu == 1 )
	{
		m_pPcieTPG0 = new CPcie();
		m_pPcieTPG0->SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
		bTpg = m_pPcieTPG0->pcie_open();
	}
	else if( cpu == 2 )
	{
		m_pPcieTPG1 = new CPcie();
		m_pPcieTPG1->SetPcieTPG( PCIE_DEVICE::PCIE_TPG1 );
		bTpg = m_pPcieTPG1->pcie_open();
	}
	else
	{
		m_pPcieTPG0 = new CPcie();
		m_pPcieTPG1 = new CPcie();
		m_pPcieTPG0->SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
		m_pPcieTPG1->SetPcieTPG( PCIE_DEVICE::PCIE_TPG1 );
		bTpg = m_pPcieTPG1->pcie_open() && m_pPcieTPG1->pcie_open();
	}
	return bTpg;
}

void CIO::ShowHelp()
{
	printf("atio -cpu {no} -a {address} | -w {data} | -dmaw | -r | -dmar | -d | -h\n");
	printf("  -cpu  <1/2/3>            1CPU(1:FC1/2, 2:FC3/4), 2CPU(1:CPU1, 2:CPU2), 3:ALL CPU\n");
	printf("  -a    <addr>             bitchart address\n");
	printf("  -w    <data>             Write Data\n");
	printf("  -l                       Loopback Test\n");
	printf("  -r                       Read Data\n");
	printf("  -f                       DMA Write Fix Data\n");
	printf("  -dbmw <byte count>       DBM Write DMA Random Data\n");
	printf("  -dbmr <byte count>       DBM Read DMA byte-size\n");
	printf("  -dfmw <byte count>       DFM Write DMA Random Data\n");
	printf("  -dfmr <byte count>       DFM Read DMA byte-size\n");
	printf("  -g                       Debug           \n");
	printf("  -h                       Help            \n");
	printf("  -v                       Version         \n");
	return;
}


int CIO::parseArgument(int argc, char* argv[] )
{
	bool bAddress      = false;
	bool bSingleWrite  = false;
	bool bFixData      = false;
	bool bDBMWrite     = false;
	bool bDBMRead      = false;
	bool bDFMWrite     = false;
	bool bDFMRead      = false;
	bool bLoopTest     = false;
	bool bSingleRead   = false;
	bool bCpu          = false;
	bool bDebug        = false;
	bool bHelp         = false;
	bool bVersion      = false;

	int nData = 0x0;
	int  nCpu          = 0x0;
	int  nLoop         = 0x0;
	int  nDMAWriteSize = 0x0;
	int  nDMAReadSize  = 0x0;
	int  nFixData      = -1;
	uint64_t  ullAddr  = 0x0;
	int  nAddress      = 0x0;
	int  nSingleData   = 0x0;

	for (int i = 1; i < argc; i++)
	{
			 if ( strncasecmp(argv[i], "-a"    , 2) == 0 ) { bAddress     = true; ullAddr       = strtoull(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-cpu"  , 4) == 0 ) { bCpu         = true; nCpu          = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-w"    , 2) == 0 ) { bSingleWrite = true; nSingleData   = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-f"    , 2) == 0 ) { bFixData     = true; nFixData      = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-dbmw" , 5) == 0 ) { bDBMWrite    = true; nDMAWriteSize = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-dfmw" , 5) == 0 ) { bDFMWrite    = true; nDMAWriteSize = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-r"    , 2) == 0 ) { bSingleRead  = true; }
		else if ( strncasecmp(argv[i], "-l"    , 2) == 0 ) { bLoopTest    = true; nLoop         = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-dbmr" , 5) == 0 ) { bDBMRead     = true; nDMAReadSize  = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-dfmr" , 5) == 0 ) { bDFMRead     = true; nDMAReadSize  = strtol(argv[++i], NULL, 0); }
		else if ( strncasecmp(argv[i], "-g"    , 2) == 0 ) { bDebug       = true;  }
		else if ( strncasecmp(argv[i], "-h"    , 2) == 0 ) { bHelp        = true;  }
		else if ( strncasecmp(argv[i], "-v"    , 2) == 0 ) { bVersion     = true;  }
		else
		{
			ShowHelp();
			return -1;
		}
	}
	if( bVersion )
	{
		printf("%s - %s[%s]\n", argv[0], TESTLIB_VER, TESTLIB_DATE);
		return 0;
	}
	if( bHelp )
	{
		ShowHelp();
		return 0;
	}

	if( !OpenPcie(nCpu) ) { ShowHelp(); return -1; }

	if( bDBMWrite )
	{
		fprintf( stdout, "DBM CTRL Reset\n" );
		switch ( nCpu )
		{
			case 1:
				m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, 0x1 );
				m_pPcieTPG0->delay( 500 MS );
				break;
			case 2:
				m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, 0x1 );
				m_pPcieTPG1->delay( 500 MS );
				break;
			case 3:
				m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, 0x1 );
				m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, 0x1 );
				m_pPcieTPG0->delay( 500 MS );
				break;
		}
	}
	if( bDFMWrite )
	{
		fprintf( stdout, "DFM CTRL Reset\n" );
		switch ( nCpu )
		{
			case 1:
				m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET, 0x1 );
				m_pPcieTPG0->delay( 1000 MS );
				break;
			case 2:
				m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET, 0x1 );
				m_pPcieTPG1->delay( 1000 MS );
				break;
			case 3:
				m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET, 0x1 );
				m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_CTRL_RESET, 0x1 );
				m_pPcieTPG0->delay( 1000 MS );
				break;
		}
	}

	if( bAddress )
	{
		nAddress = static_cast<int>( ullAddr&0xFFFFFFFF );
		if( bLoopTest )
		{
			bool bPass = true;
			for( int i=0;i<nLoop;i++ )
			{
				uint nWrite = QRandomGenerator::global()->generate();
				m_pPcieTPG0->write_user( nAddress, nWrite );
				uint nRead = m_pPcieTPG0->read_user(nAddress);
				if( nWrite != nRead )
				{
					bPass = false;
					printf("[%04d] EXPECT:%08X, READ:0x%08X\n", i, nWrite, nRead );
				}
				else
				{
					if( (i%100000) == 0)
						printf("[%04d] EXPECT:%08X, READ:0x%08X\n", i, nWrite, nRead );
				}
			}
			if( !bPass )
			{
				printf("0x%08x loopback test count:%d error!\n", nAddress, nLoop );
			}
			else
			{
				printf("0x%08x loopback test count:%d success!\n", nAddress, nLoop );
			}
		}
		else if( bSingleWrite )
		{
			switch ( nCpu )
			{
				case 1:
					printf("CPU1 write(0x%08X,0x%08X)\n", nAddress, nSingleData );
					m_pPcieTPG0->write_user( nAddress, nSingleData );
					break;
				case 2:
					printf("CPU2 write(0x%08X,0x%08X)\n", nAddress, nSingleData );
					m_pPcieTPG1->write_user( nAddress, nSingleData );
					break;
				case 3:
					printf("CPU1 write(0x%08X,0x%08X)\n", nAddress, nSingleData );
					m_pPcieTPG0->write_user( nAddress, nSingleData );
					printf("CPU2 write(0x%08X,0x%08X)\n", nAddress, nSingleData );
					m_pPcieTPG1->write_user( nAddress, nSingleData );
					break;
			}
		}
		else if( bSingleRead )
		{
			int nData = 0x0;
			switch ( nCpu )
			{
				case 1:
					nData = m_pPcieTPG0->read_user(nAddress);
					printf("CPU1 read(0x%08X) = 0x%08X\n", nAddress, nData );
					break;
				case 2:
					nData = m_pPcieTPG1->read_user(nAddress);
					printf("CPU2 read(0x%08X) = 0x%08X\n", nAddress, nData );
					break;
				case 3:
					nData = m_pPcieTPG0->read_user(nAddress);
					printf("CPU1 read(0x%08X) = 0x%08X\n", nAddress, nData );
					nData = m_pPcieTPG1->read_user(nAddress);
					printf("CPU2 read(0x%08X) = 0x%08X\n", nAddress, nData );
					break;
			}
		}
		else if( bDBMWrite )
		{
			//printf("bFixData:%s, nFixData:0x%X\n", bFixData?"true":"false", nFixData);
			DBMRamdomWrite( nCpu, ullAddr, nDMAWriteSize, nFixData );
		}
		else if( bDBMRead )
		{
			DBMRead( nCpu, ullAddr, nDMAReadSize );
		}
		else if( bDFMWrite )
		{
			//printf("bFixData:%s, nFixData:0x%X\n", bFixData?"true":"false", nFixData);
			DFMRamdomWrite( nCpu, ullAddr, nDMAWriteSize, nFixData );
		}
		else if( bDFMRead )
		{
			DFMRead( nCpu, ullAddr, nDMAReadSize );
		}
	}
	else
	{
		printf("-a {address} undefined!\n" );
		ShowHelp();
		return -1;
	}
	return nData;
}

void CIO::DBMRamdomWrite( int cpu, uint64_t address, int byte_count, int fixdata )
{
	if( m_pDBmBuffer ) { delete m_pDBmBuffer; m_pDBmBuffer = nullptr; }

	m_pDBmBuffer = new char[byte_count];
	for( int i=0;i<byte_count;i++ )
	{
		//printf("nFixData:0x%X[%d]\n",  fixdata,  fixdata);
		if( fixdata == -1 )
			m_pDBmBuffer[i] = i;	//QRandomGenerator::global()->generate();
		else
			m_pDBmBuffer[i] = fixdata;	//QRandomGenerator::global()->generate();
		printf("CPU%d [%04d] 0x%02X\n", cpu, i, m_pDBmBuffer[i]&0xFF );
	}

	int nData = 0x0;
	int nByteSize = byte_count;
	switch ( cpu )
	{
		case 1:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG0->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_DATA   , m_pDBmBuffer, nByteSize );
			m_pPcieTPG1->delay( 1 MS );

			fprintf( stdout, "DBM TPG0 Write( 0x%X, 0x%llX)\n"    , BITCHART::ALPG::_DMA_ADDRESS, address   );
			fprintf( stdout, "DBM TPG0 Write( 0x%X ) Bytes : %d\n", BITCHART::ALPG::_DMA_DATA   , nByteSize );
			break;
		case 2:
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG1->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );
			nData = m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG0->delay( 100 US );
			nData = m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_DATA    , m_pDBmBuffer, nByteSize );
			m_pPcieTPG1->delay( 1 MS );
			fprintf( stdout, "DBM TPG1 Write( 0x%X, 0x%llX)\n"    , BITCHART::ALPG::_DMA_ADDRESS, address   );
			fprintf( stdout, "DBM TPG1 Write( 0x%X ) Bytes : %d\n", BITCHART::ALPG::_DMA_DATA   , nByteSize );
			break;
		case 3:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG1->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			nData = m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG1->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_DATA   , m_pDBmBuffer, nByteSize );
			nData = m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_DATA   , m_pDBmBuffer, nByteSize );
			m_pPcieTPG1->delay( 1 MS );
			break;
	}
	//delete[] m_pDBmBuffer;
}

void CIO::DBMRead( int cpu, uint64_t address, int byte_count )
{
	char* pData1 = new char[byte_count];
	char* pData2 = new char[byte_count];
	int nByteSize = byte_count;

	switch ( cpu )
	{
		case 1:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );

			m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG1->delay( 100 US );
			fprintf( stdout, "TPG0 DBMRead Write( 0x%X, 0x%llX)\n", BITCHART::ALPG::_DMA_ADDRESS, address );
			m_pPcieTPG0->read_dma( BITCHART::ALPG::_DMA_DATA, (char*)pData1, nByteSize );
			m_pPcieTPG1->delay( 1 MS );
			break;
		case 2:			
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG1->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );

			m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG1->delay( 100 US );
			fprintf( stdout, "TPG1 DBMRead Write( 0x%X, 0x%llX)\n", BITCHART::ALPG::_DMA_ADDRESS, address );
			m_pPcieTPG1->read_dma( BITCHART::ALPG::_DMA_DATA, (char*)pData2, nByteSize );
			m_pPcieTPG1->delay( 1 MS );
			break;
		case 3:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );

			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG1->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x0 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );

			m_pPcieTPG0->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			fprintf( stdout, "TPG0 Write( 0x%X, 0x%llX)\n", BITCHART::ALPG::_DMA_ADDRESS, address );
			m_pPcieTPG0->read_dma( BITCHART::ALPG::_DMA_DATA, (char*)pData1, nByteSize );

			m_pPcieTPG1->write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			fprintf( stdout, "TPG1 Write( 0x%X, 0x%llX)\n", BITCHART::ALPG::_DMA_ADDRESS, address );
			m_pPcieTPG1->read_dma( BITCHART::ALPG::_DMA_DATA, (char*)pData2, nByteSize );
			break;
	}

	bool bPass = true;
	switch ( cpu )
	{
		case 1:
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData1[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					//printf("CPU1 COUNT:%05d  EXPECT[0x%04X]  READ[0x%04X]\n", i, m_pDBmBuffer[i]&0xFF, pData1[i]&0xFF );
					printf("CPU1 COUNT:%05d  READ[0x%04X]\n", i, pData1[i]&0xFF );
				}
			}
			break;
		case 2:
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData2[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU2 COUNT:%05d  READ[0x%04X]\n", i, pData2[i]&0xFF );
					//printf("CPU2 COUNT:%05d  EXPECT[0x%04X]  READ[0x%04X]\n", i, m_pDBmBuffer[i]&0xFF, pData2[i]&0xFF );
				}
			}
			break;
		case 3:
			for( int i=0;i<byte_count;i++ )
			{
				if( pData1[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU1 COUNT:%05d  EXPECT[0x%04X]  READ[0x%04X]\n", i, m_pDBmBuffer[i]&0xFF, pData1[i]&0xFF );
				}
			}
			for( int i=0;i<byte_count;i++ )
			{
				if( pData2[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU2 COUNT:%05d  EXPECT[0x%04X]  READ[0x%04X]\n", i, m_pDBmBuffer[i]&0xFF, pData2[i]&0xFF );
				}			}
			break;
	}
	delete[] pData1;
	delete[] pData2;
}

void CIO::DFMRamdomWrite( int cpu, uint64_t address, int byte_count, int fixdata )
{
	int DMA_ADDRESS = 0x6FF0000;
	int DMA_DATA    = 0x7000000;
	if( m_pDBmBuffer ) { delete m_pDBmBuffer; m_pDBmBuffer = nullptr; }

	m_pDBmBuffer = new char[byte_count];
	for( int i=0;i<byte_count;i++ )
	{
		//printf("nFixData:0x%X[%d]\n",  fixdata,  fixdata);
		if( fixdata == -1 )
			m_pDBmBuffer[i] = i;	//QRandomGenerator::global()->generate();
		else
			m_pDBmBuffer[i] = fixdata;	//QRandomGenerator::global()->generate();
		printf("DFMDATA CPU%d [%04d] 0x%02X\n", cpu, i, m_pDBmBuffer[i]&0xFF );
	}

	int nData = 0x0;
	int nByteSize = byte_count;
	switch ( cpu )
	{
		case 1:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DFM write
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			fprintf( stdout, "DFMWrite TPG0( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			nData = m_pPcieTPG0->write_dma( DMA_DATA   , m_pDBmBuffer, nByteSize );
			fprintf( stdout, "DFMWrite TPG0( 0x%X , Bytes : %d)\n", DMA_DATA, nByteSize );
			break;
		case 2:
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DFM write
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );
			nData = m_pPcieTPG1->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			fprintf( stdout, "DFMWrite TPG1( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			nData = m_pPcieTPG1->write_dma( DMA_DATA   , m_pDBmBuffer, nByteSize );
			fprintf( stdout, "DFMWrite TPG1( 0x%X , Bytes : %d)\n", DMA_DATA, nByteSize );
			break;
		case 3:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DFM write
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DFM write
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//WRITE:0, READ:1
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );
			nData = m_pPcieTPG0->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			nData = m_pPcieTPG1->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			nData = m_pPcieTPG0->write_dma( DMA_DATA   , m_pDBmBuffer, nByteSize );
			nData = m_pPcieTPG1->write_dma( DMA_DATA   , m_pDBmBuffer, nByteSize );
			break;
	}
	//delete[] m_pDBmBuffer;
}

void CIO::DFMRead( int cpu, uint64_t address, int byte_count )
{
	int DMA_ADDRESS = 0x6FF0000;
	int DMA_DATA    = 0x7000000;

	fprintf( stdout, "CPU%d DFM READ START.....\n", cpu );

	char* pData1 = new char[byte_count];
	char* pData2 = new char[byte_count];
	int nByteSize = byte_count;

		 if( cpu == 1 )	m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DMA write
	else if( cpu == 2 ) m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DMA write
	else if( cpu == 3 )
	{
		m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DMA write
		m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//DMA write
	}
	usleep( 100 );

	switch ( cpu )
	{
		case 1:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );

			fprintf( stdout, "DFMRead TPG0 Write( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			m_pPcieTPG0->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG0->read_dma( DMA_DATA, (char*)pData1, nByteSize );
			break;
		case 2:
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );

			fprintf( stdout, "DFMRead TPG1 Write( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			m_pPcieTPG1->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG1->read_dma( DMA_DATA, (char*)pData2, nByteSize );
			break;
		case 3:
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG0->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG0->delay( 100 US );

			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//WRITE:0, READ:1
			m_pPcieTPG1->write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL, 0x1 );	//DBM:0, DFM:1
			m_pPcieTPG1->delay( 100 US );

			fprintf( stdout, "DFM TPG0 Write( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			m_pPcieTPG0->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG0->read_dma( DMA_DATA, (char*)pData1, nByteSize );

			fprintf( stdout, "DFM TPG1 Write( 0x%X, 0x%llX)\n", DMA_ADDRESS, address );
			m_pPcieTPG1->write_dma( DMA_ADDRESS, (char*)&address, sizeof(uint64_t) );
			m_pPcieTPG1->read_dma( DMA_DATA, (char*)pData2, nByteSize );
			break;
	}

	bool bPass = true;
	switch ( cpu )
	{
		case 1:
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData1[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU1 COUNT:%05d  READ[0x%04X]\n", i, pData1[i]&0xFF );
				}
			}
			break;
		case 2:
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData2[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU2 COUNT:%05d  READ[0x%04X]\n", i, pData2[i]&0xFF );
				}
			}
			break;
		case 3:
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData1[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU1 COUNT:%05d  READ[0x%04X]\n", i, pData1[i]&0xFF );
				}
			}
			for( int i=0;i<byte_count;i++ )
			{
				//if( pData2[i] != m_pDBmBuffer[i] )
				{
					bPass = false;
					printf("CPU2 COUNT:%05d  READ[0x%04X]\n", i, pData2[i]&0xFF );
				}			}
			break;
	}
	delete[] pData1;
	delete[] pData2;
}
