#include "io.hpp"
#include "sysdef.hpp"
#include <chrono>

CPcie::CPcie()
    : m_PciIdx(PCIE_TPG0), m_fdUser(-1), m_fdH2C(-1), m_fdC2H(-1), m_bOpen(false), m_pMapBase(nullptr)
{

}

CPcie::~CPcie()
{
	pcie_close();
}

void CPcie::SetPcieTPG(PCIE_DEVICE idx)
{
    if( idx == PCIE_DEVICE::PCIE_TPG0 )
    {
        strncpy( m_cdevs[0], XDMA_TPG0_USR_CDEV, sizeof(m_cdevs[0]) );
        strncpy( m_cdevs[1], XDMA_TPG0_H2C_CDEV, sizeof(m_cdevs[1]) );
        strncpy( m_cdevs[2], XDMA_TPG0_C2H_CDEV, sizeof(m_cdevs[2]) );
        m_MemBase = XDMA_TPG_MEM_BASE;
    }
    else
    {
        strncpy( m_cdevs[0], XDMA_TPG1_USR_CDEV, sizeof(m_cdevs[0]) );
        strncpy( m_cdevs[1], XDMA_TPG1_H2C_CDEV, sizeof(m_cdevs[1]) );
        strncpy( m_cdevs[2], XDMA_TPG1_C2H_CDEV, sizeof(m_cdevs[2]) );
        m_MemBase = XDMA_TPG_MEM_BASE;
    }
}

void CPcie::delay( double time )
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	long long wait_time = static_cast<long long>( time * 1.0e9 );	//NS
	while ( true )
	{
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		long long now_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		if( now_time > wait_time ) break;
	}
}

bool CPcie::pcie_open()
{
    bool bOpen = false;
#ifdef SIMULATION
    bOpen   = true;
    m_bOpen = bOpen;
    return true;
#else
    if( pcie_isopen() ) return true;
    bOpen  = open_user( m_cdevs [PCIE_XDMA_USR] );
    bOpen &= open_h2c ( m_cdevs [PCIE_XDMA_H2C] );
    bOpen &= open_c2h ( m_cdevs [PCIE_XDMA_C2H] );
    m_bOpen = bOpen;
    return bOpen;
#endif
}

bool CPcie::pcie_isopen()
{
    return m_bOpen;
}

bool CPcie::pcie_close()
{
    if( !pcie_isopen() ) return true;

    bool bClose = true;
    bClose &= close_user();
    bClose &= close_h2c();
    bClose &= close_c2h();
    m_bOpen = !bClose;
    return bClose;
}

bool CPcie::open_user( const char* cdev )
{
#ifdef SIMULATION
    (void)(cdev);
    return true;
#else
    m_fdUser = open( cdev, O_RDWR | O_SYNC );
    if( m_fdUser < 0 )
    {
        fprintf( stderr, "%s open error!\n", cdev );
        return false;
    }

    m_pMapBase = (void*)mmap(0, XDMA_TPG_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_fdUser, XDMA_TPG_MEM_BASE );
    if ( m_pMapBase == (void*)-1 )
    {
        close(m_fdUser);
        fprintf( stderr, "%s mmap error!\n", cdev );
        return false;
    }
	//fprintf( stdout, "%s mmap success!!\n", cdev );

    return true;
#endif
}

bool CPcie::open_h2c( const char* cdev )
{
#ifdef SIMULATION
    (void)(cdev);
    return true;
#else
    m_fdH2C = open(cdev, O_RDWR);
    if (m_fdH2C < 0 )
    {
        fprintf( stderr, "%s open error!\n", cdev );
        return false;
    }
	//fprintf( stdout, "%s open h2c success!!\n", cdev );
    return true;
#endif
}

bool CPcie::open_c2h( const char* cdev )
{
#ifdef SIMULATION
    (void)(cdev);
    return true;
#else
    m_fdC2H = open(cdev, O_RDWR); // open(cdev, O_RDWR | O_TRUNC);
    if (m_fdC2H < 0 )
    {
        fprintf( stderr, "%s open error!\n", cdev );
        return false;
    }
	//fprintf( stdout, "%s open c2h success!!\n", cdev );
    return true;
#endif
}

bool CPcie::close_user()
{
#ifdef SIMULATION
    return true;
#else
    if ( munmap( m_pMapBase, XDMA_TPG_MEM_SIZE ) == -1 )
    {
        FATAL; return false;
    }
    close(m_fdUser);
    return true;
#endif
}

bool CPcie::close_h2c()
{
    return true;
}

bool CPcie::close_c2h()
{
    return true;
}

bool CPcie::write_user( uint32_t addr, uint32_t data )
{
#ifdef SIMULATION
    (void)addr;
    (void)data;
    return true;
#else
    if( !pcie_isopen() ) return false;
    off_t nPhysicalOffset = addr - m_MemBase;
    *(uint *)( (char*)m_pMapBase+nPhysicalOffset) = data;
	//delay( 100 NS );
    return true;
#endif
}

int  CPcie::read_user( uint32_t addr )
{
#ifdef SIMULATION
    (void)addr;
	int nData = rand() & 0xFFFFFFFF;
    return nData;
#else
    if( !pcie_isopen() ) return -1;
    off_t nPhysicalOffset = addr - m_MemBase;
    int nData = *((uint32_t*) ((char*)m_pMapBase+nPhysicalOffset));
    return nData;
#endif
}

//H2C
bool CPcie::write_dma( uint32_t addr, char* pwdata, uint32_t size )
{
#ifdef SIMULATION
    (void)addr;
    (void)pwdata;
    (void)size;
    return true;
#else
    char* wBuffer = nullptr;
    char* memptr  = nullptr;
    size_t bytes_size = size*sizeof(char);
    //struct timespec ts_start, ts_end;

    posix_memalign( (void **)&memptr, 4096 /*alignment */ , bytes_size + 4096 );
    if ( !memptr )
    {
        FATAL; return false;
    }
    wBuffer = memptr;
    memcpy( wBuffer, pwdata, bytes_size );

    //for( unsigned int i=0;i<size;i++ )
    //    fprintf( stdout, "W [%02d]0x%02X\n", i, wBuffer[i]&0xFF );

    ssize_t rc        = 0;
    size_t total_size = bytes_size;
    size_t write_size = 0;
    off_t offset      = addr;

    //fprintf( stdout, "%s addr:0x%x, offset:0x%lx, size:%d\n", __FUNCTION__, addr, offset, size );
    while( write_size < total_size )
    {
        uint32_t bytes = total_size - write_size;
        if (bytes > RW_MAX_SIZE)
            bytes = RW_MAX_SIZE;
        if ( offset )
        {
            rc = lseek( m_fdH2C, offset, SEEK_SET );
            if (rc != offset)
            {
                fprintf(stderr, "%s, seek off 0x%lx != 0x%lx.\n", __FUNCTION__, rc, offset);
				return false;
            }
        }
        rc = write(m_fdH2C, wBuffer, bytes);
        if (rc < 0)
        {
			return false;
        }
		//fprintf( stdout, "write size : %d, written size : %ld\n", bytes, rc );
        write_size += rc;
        if (rc != bytes)
        {
            fprintf(stderr, "writeDMA underflow 0x%lx/0x%x @ 0x%lx.\n", rc, bytes, offset);
			return false;
        }
        wBuffer += bytes;
        offset  += bytes;
    }

    if( write_size != total_size )
    {
        fprintf(stderr, "writeDMA underflow 0x%lx/0x%lx.\n", write_size, total_size);
        return false;
    }
    free(memptr);
    return true;
#endif
}

//C2H
bool CPcie::read_dma ( uint32_t addr, char* prdata, uint32_t size )
{
#ifdef SIMULATION
    (void)addr;
    size_t bytes_size = size*sizeof(char);
    for( size_t i=0;i<bytes_size;i++ )
        prdata[i] = 0x10;    //rand() & 0xFF;
    return true;
#else
    char* rBuffer = nullptr;
    char* rMemory = nullptr;
    char* memptr  = nullptr;
    size_t bytes_size = size*sizeof(char);
    //struct timespec ts_start, ts_end;

    //fprintf( stdout, "%s addr:0x%x, size:%d, bytes_size:%ld\n", __FUNCTION__, addr, size, bytes_size );

    posix_memalign( (void **)&memptr, 4096 /*alignment */ , bytes_size + 4096 );
    if ( !memptr )
    {
        FATAL; return false;
    }
    rBuffer = memptr;
    rMemory = memptr;

    ssize_t rc        = 0;
    size_t total_size = bytes_size;
    size_t read_size  = 0;
    off_t offset      = addr;

	//fprintf( stderr, "start %s addr:0x%x, offset:0x%lx, size:%d\n", __FUNCTION__, addr, offset, size );
    while( read_size < total_size )
    {
        uint32_t bytes = total_size - read_size;

        if (bytes > RW_MAX_SIZE)
            bytes = RW_MAX_SIZE;

        if ( offset )
        {
            rc = lseek(m_fdC2H, offset, SEEK_SET);
            if (rc != offset)
            {
                fprintf(stderr, "seek off 0x%lx != 0x%lx.\n", rc, offset);
				return false;
            }
        }
        rc = read(m_fdC2H, rBuffer, bytes);
        if (rc < 0)
        {
            fprintf(stderr, "%s, read size:0x%x addr:0x%lx failed %ld.\n", __FUNCTION__, bytes, offset, rc);
			return false;
        }
        //fprintf( stdout, "read size : %d, read size : %ld\n", bytes, rc );

        read_size += rc;
        if (rc != bytes)
        {
            fprintf(stderr, "readDMA underflow 0x%lx/0x%x @ 0x%lx.\n", rc, bytes, offset);
			return false;
        }
        rBuffer += bytes;
        offset  += bytes;
    }

    //for( unsigned int i=0;i<size;i++ )
    //    fprintf( stdout, "R [%02d]0x%02X\n", i, rMemory[i]&0xFF );
    memcpy( prdata, rMemory, bytes_size );
    free(memptr);
	//fprintf( stdout, "end %s start_addr:0x%x, end_address:0x%lx, size:%d\n", __FUNCTION__, addr, offset, size );

    return true;
#endif
}
