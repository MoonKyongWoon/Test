#include "bitchart.hpp"
#include "dbm.hpp"
#include "hw.hpp"
#include "io.hpp"
#include "log.hpp"
#include "pg.hpp"
#include "sys.hpp"

extern CPcie g_pcie;
extern CHw   g_hw;
extern CLog  g_log;
extern CPg   g_pg;
extern CSys  g_sys;

CDbm::CDbm()
{
	m_pDbm = nullptr;
	m_pBgDbm = nullptr;
	m_nBgSize = 0;
	m_nSize = MAX_DBM_SIZE;
}

CDbm::~CDbm()
{
	DeleteBuf();
}

void CDbm::NewBuf( int size )
{
	DeleteBuf();
	m_nSize = size;
	m_pDbm = new DBM_STRUCT[ size ];
	memset( (char*)m_pDbm, 0x0, sizeof(DBM_STRUCT)*size );
	//printf( "New Size : %ld\n", sizeof(DBM_STRUCT)*size );
}

void CDbm::DeleteBuf()
{
	if( m_pDbm != nullptr )
	{
		delete[] m_pDbm;
		m_pDbm = nullptr;
		//printf( "Delete\n" );
	}
	if( m_pBgDbm != nullptr )
	{
		delete[] m_pBgDbm;
		m_pBgDbm = nullptr;
		//printf( "Delete\n" );
	}
}

void CDbm::DbmCtrlReset()
{
	int delay = 1000;
	g_pcie.write_user(BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, 0x1 );
	if( 0 ) g_log.Printf( "DBM CTRL RESET(0x%08X) %d MS", BITCHART::GLOBAL_MODE_REG::_DBM_CTRL_RESET, delay );
	g_sys.SysDelay( delay MS );
}

void CDbm::ClearDbm()
{
	NewBuf( MAX_DBM_SIZE_64M );
	WriteDbmMemory();
	DeleteBuf();
}

void CDbm::SetDbmData( unsigned char* p, unsigned int size, bool ddr )
{
	//size = 102400;
	unsigned int nSize  = size;
	bool sdr   = true;
	bool bIoDdr = g_pg.GetIoDdrModeGlobal();
	bool bAdDdr = g_pg.GetAddrDdrModeGlobal();
	if( ddr && bIoDdr && bAdDdr )
		sdr = false;

	if( sdr )
	{
		if( size > MAX_DBM_SIZE_32M )
		{
			g_log.EPrintf( _DBM, "DBM", "%s error : SDR Mode DBM size limit over %d [max_size:%d]\n", __FUNCTION__, size, MAX_DBM_SIZE_32M );
			return;
		}
		nSize = size;
	}
	else
	{
		if( size > MAX_DBM_SIZE_64M )
		{
			g_log.EPrintf( _DBM, "DBM", "%s error : DDR Mode DBM size limit over %d [max_size:%d]\n", __FUNCTION__, size, MAX_DBM_SIZE_64M );
			return;
		}
		nSize = size/2;
	}
	NewBuf( nSize );	//Spare + 1

	if( sdr )
	{
		for( uint i=0;i<nSize;i++ )
		{
			m_pDbm[i].sdr_byte0 = p[i];
			m_pDbm[i].sdr_byte1 = p[i];
			m_pDbm[i].ddr_byte0 = 0x0;
			m_pDbm[i].ddr_byte1 = 0x0;
			//if( i < 64 )
			//	printf("[%05d]0x%04X\n", i, m_pDbm[i].dword );
		}
	}
	else
	{
		for( uint i=0;i<size;i+=2 )
		{
			m_pDbm[i/2+0].sdr_byte0 = p[i*2+0];
			m_pDbm[i/2+0].sdr_byte1 = p[i*2+0];
			m_pDbm[i/2+0].ddr_byte0 = p[i*2+1];
			m_pDbm[i/2+0].ddr_byte1 = p[i*2+1];
		}
	}
	m_nSize = nSize;
	WriteDbmMemory();
}

void CDbm::SetDbmData( unsigned short* p, unsigned int size, bool ddr )
{
	unsigned int nSize  = size;
	unsigned int nCount = nSize / sizeof(unsigned int);
	bool sdr   = true;
	bool bIoDdr = g_pg.GetIoDdrModeGlobal();
	bool bAdDdr = g_pg.GetAddrDdrModeGlobal();
	if( ddr && bIoDdr && bAdDdr )
		sdr = false;

	if( sdr )
	{
		if( size > MAX_DBM_SIZE_32M )
		{
			g_log.EPrintf( _DBM, "DBM", "%s SDR Mode DBM size limit over %d [max_size:%d]", __FUNCTION__, size, MAX_DBM_SIZE_32M );
			return;
		}
		nSize = size;
	}
	else
	{
		if( size > MAX_DBM_SIZE_64M )
		{
			g_log.EPrintf( _DBM, "DBM", "%s DDR Mode DBM size limit over %d [max_size:%d]", __FUNCTION__, size, MAX_DBM_SIZE_64M );
			return;
		}
		nSize = size/2;
	}

	NewBuf( nSize+1 );	//Spare + 1
	if( sdr )
	{
		for( uint i=0;i<nCount;i++ )
		{
			m_pDbm[i*2+0].sdr_byte0 = (p[i]>>8)&0xFF;
			m_pDbm[i*2+0].sdr_byte1 = (p[i]>>8)&0xFF;
			m_pDbm[i*2+0].ddr_byte0 = 0x0;
			m_pDbm[i*2+0].ddr_byte1 = 0x0;

			m_pDbm[i*2+1].sdr_byte0 = (p[i]>>0)&0xFF;
			m_pDbm[i*2+1].sdr_byte1 = (p[i]>>0)&0xFF;
			m_pDbm[i*2+1].ddr_byte0 = 0x0;
			m_pDbm[i*2+1].ddr_byte1 = 0x0;			
		}

	}
	else
	{
		for( uint i=0;i<nCount;i+=2 )
		{
			unsigned char f_high = (p[i+0]>>8)&0xff;
			unsigned char f_low  = (p[i+0]>>0)&0xff;
			unsigned char s_high = (p[i+1]>>8)&0xff;
			unsigned char s_low  = (p[i+1]>>0)&0xff;

			m_pDbm[i*2+0].sdr_byte0 = f_high;
			m_pDbm[i*2+0].sdr_byte1 = 0x0;
			m_pDbm[i*2+0].ddr_byte0 = s_high;
			m_pDbm[i*2+0].ddr_byte1 = 0x0;

			m_pDbm[i*2+1].sdr_byte0 = f_low;
			m_pDbm[i*2+1].sdr_byte1 = 0x0;
			m_pDbm[i*2+1].ddr_byte0 = s_low;
			m_pDbm[i*2+1].ddr_byte1 = 0x0;
		}
	}
	m_nSize = nSize;
	WriteDbmMemory();
}

#define DBM_MEMORY_ADDR_OFFSET	(8)


//Index Write 32bit, don't care sdr/ddr
//operation error, one address read & write
void CDbm::WriteDbm( unsigned int index, unsigned int v )
{
	uint64_t dbm_read_data       = 0x0;
	uint64_t dbm_write_data      = 0x0;
	const uint64_t dbm_main_addr = index*DBM_MEMORY_ADDR_OFFSET;	//2023.02.27 - KWMOON(DBM MODIFY - (index/2)*DBM_MEMORY_ADDR_OFFSET;)
	const uint64_t dbm_address   = DBM_START_ADDR + dbm_main_addr;
	uint dbm_data_offset         = index % 2;

	if( 0 )
	{
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
		g_sys.SysDelay( 1 US );

		g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address  , sizeof(uint64_t) );
		g_sys.SysDelay( 1 US );
		g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , (char*)&dbm_read_data, sizeof(uint64_t) );
		g_sys.SysDelay( 1 US );

		dbm_write_data = dbm_read_data;
		if( dbm_data_offset == 0 )
		{
			dbm_write_data &= 0xffffffff00000000LL;
			dbm_write_data |= (v&0xffffffffLL);
		}
		else
		{
			uint64_t temp = v;
			dbm_write_data &= 0x00000000ffffffffLL;
			dbm_write_data |= ((temp&0xffffffffLL)<<32);
		}
		if( index < 16 )
		{
			g_log.Printf( "setDbm(%d,0x%x) dbm_address:0x%llx", index, v, dbm_address);
			g_log.Printf( "index:%02d read  = 0x%llX", index, dbm_read_data );
			g_log.Printf( "index:%02d write = 0x%llX", index, dbm_write_data );
		}
	}
	dbm_write_data = v;
	//DbmCtrlReset();
	//g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	//g_sys.SysDelay( 100 US );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_sys.SysDelay( 1 US );

	g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address   , sizeof(uint64_t) );
	g_sys.SysDelay( 1 US );
	g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA   , (char*)&dbm_write_data, sizeof(uint64_t) );
	g_sys.SysDelay( 1 US );
}

//Index Write 32bit, don't care sdr/ddr
//for debug function
uint CDbm::ReadDbm( unsigned int index )
{
	uint64_t dbm_read_data   = 0x0;
	uint64_t dbm_main_addr   = index*DBM_MEMORY_ADDR_OFFSET;	//2023.02.27 - KWMOON( org : (index/2)*DBM_MEMORY_ADDR_OFFSET;)
	uint64_t dbm_address     = DBM_START_ADDR + dbm_main_addr;
	uint dbm_data_offset     = 0x0;	//2023.02.27 - KWMOON( ORG : index % 2; )
	uint dbm_data            = 0x0;

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_sys.SysDelay( 1 US );
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	g_sys.SysDelay( 1 US );

	g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address  , sizeof(uint64_t) );
	g_sys.SysDelay( 1 US );
	g_pcie.read_dma ( BITCHART::ALPG::_DMA_DATA   , (char*)&dbm_read_data, DBM_MEMORY_ADDR_OFFSET );
	g_sys.SysDelay( 1 US );

	if( dbm_data_offset == 0 )
	{
		dbm_data = dbm_read_data&0xffffffff;
	}
	else
	{
		dbm_data = (dbm_read_data>>32)&0xffffffff;
	}
	//g_log.Printf( "getDbm(%d) dbm_address:0x%llx", index, dbm_address);
	//g_log.Printf( "index:%02d read  = 0x%llX", index, dbm_read_data );
	return dbm_data;
}

void CDbm::PrintfDbm( unsigned int fc, unsigned int start, unsigned int end )
{
	(void)fc;
	PrintfDbm( start, end );
}

void CDbm::PrintfDbm( unsigned int start, unsigned int end )
{
	char* pData = nullptr;
	uint64_t dbm_main_addr   = (start / 2)*8;
	uint64_t dbm_data_offset = start % 2;
	uint64_t dbm_address     = 0x100000000 | dbm_main_addr;
	int range        = (end-start);
	int loop_count   = range / DMA_WR_MAX_SIZE;
	int remain_count = range - ( loop_count * DMA_WR_MAX_SIZE);

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL  , 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_sys.SysDelay( 1 US );

	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	g_sys.SysDelay( 1 US );

	//uint64_t dbm_address    = 0x100000000;
	//g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x1 );	//D[0] : 0 Write Operation, 1 Read Operation
	//g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	//g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_WRITE_SEL, 0x0 );	//D[0] : 0 DMA Write, 1 DFM Write

}

//2022.12.06 - kwmoon( Add BG Write Mode )
bool CDbm::WriteDbmMemory( bool BGW )
{
	DbmCtrlReset();

	int nCount = 0x0;
	char* pDbm = nullptr;
	if( BGW )
	{
		nCount = m_nBgSize;
		pDbm   = (char*)m_pBgDbm;
	}
	else
	{
		nCount = m_nSize;
		pDbm   = (char*)m_pDbm;
	}
	if( pDbm == nullptr )
	{
		//g_log.EPrintf( DEV_LOG_TYPE::_DBM, "WRITE DBM", "DBM Pointer is null" );
		return false;
	}

	g_log.Printf( _DBM, "DBM", "CDbm::WriteDbmMemory ------\n" );

	uint64_t dbm_address = BITCHART::DMA_MEMORY_ADDRESS::DBM_MEM_ADDRESS;
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DMA_WR_RD_SEL, 0x0 );	//D[0] : 0 Write Operation, 1 Read Operation
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_DFM_SEL  , 0x0 );	//DBM:0, DFM:1
	g_sys.SysDelay( 1 US );

	int total_size = sizeof(DBM_STRUCT) * nCount;
	int loop_count = total_size / DMA_WR_MAX_SIZE;
	int remain_size= total_size - ( loop_count * DMA_WR_MAX_SIZE);

	if( 0 )	g_log.Printf( _DBM, "DBM", "[%s WRTIE]Total Count : %d , Loop : %d, Remain : %d \n", (BGW)?"DBM BG":"DBG", nCount, loop_count, remain_size );

	for( int i=0;i<loop_count;i++ )
	{
		if( !g_sys.GetRunmode() ) break;	//2023.04.27 - KWMOON
		if( pDbm != nullptr )
		{
			if( !g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) ) )
			{

			}
			if( !g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA   , pDbm, DMA_WR_MAX_SIZE ) )
			{

			}
			//printf("DBM WRITE %p:0x%lx ... %s (%d/%d)\n", pDbm, dbm_address, bWPass?"PASS":"FALSE", i+1, loop_count );
		}
		dbm_address += DMA_WR_MAX_SIZE;
		pDbm += DMA_WR_MAX_SIZE;
	}
	if( g_sys.GetRunmode() )	//2023.04.27 - KWMOON
	{
		if( remain_size > 0 )
		{
			if( pDbm != nullptr )
			{
				g_pcie.write_dma( BITCHART::ALPG::_DMA_ADDRESS, (char*)&dbm_address, sizeof(uint64_t) );
				g_pcie.write_dma( BITCHART::ALPG::_DMA_DATA   , pDbm, remain_size );
			}
		}
	}
	if( BGW )
	{
		delete[] m_pBgDbm;
		m_pBgDbm  = nullptr;
		m_nBgSize = 0;
	}
	return true;
}

//2022.12.06 - kwmoon
bool CDbm::CreateBgMemory()
{
	if( 0 ) g_log.Printf( "[DBM] Create BG Memory");
	m_nBgSize = 0;
	if( m_pBgDbm )
	{
		memset( m_pBgDbm, 0x0, sizeof(DBM_STRUCT)*MAX_DBM_SIZE_64M );
		return true;
	}
	m_pBgDbm = new ( std::nothrow ) DBM_STRUCT[MAX_DBM_SIZE_64M];
	if( !m_pBgDbm )  return false;
	memset( m_pBgDbm, 0x0, sizeof(DBM_STRUCT)*MAX_DBM_SIZE_64M );
	return true;
}

//2022.12.06 - kwmoon
bool CDbm::SetBgDbm(uint index, int v)
{
	if( m_pBgDbm )
	{
		if( index > (MAX_DBM_SIZE_64M-1) )
		{
			//g_log.EPrintf( _DBM, "setDbm", "index limit over %d[Limit:%d]", index, MAX_DBM_SIZE_64M );
			return true;
		}
		m_pBgDbm[index].dword = v;
		m_nBgSize++;
		return true;
	}
	return false;
}

