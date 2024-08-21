#ifndef CPCIE_HPP
#define CPCIE_HPP

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <byteswap.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "sysdev.hpp"

/*
* man 2 write:
* On Linux, write() (and similar system calls) will transfer at m
ost
* 	0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
*	actually transferred.  (This is true on both 32-bit and 64-bit
*	systems.)
*/

#define RW_MAX_SIZE	0x7ffff000
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, __FILE__, errno, strerror(errno)); } while(0)

class  CPcie
{
public:
    CPcie();
    ~CPcie();
    void SetPcieTPG(PCIE_DEVICE);
public:
    bool pcie_open();
    bool pcie_close();
    bool pcie_isopen();
public:
	bool write_user(uint32_t,uint32_t);
	int  read_user(uint32_t);
	bool write_dma( uint32_t, char*, uint32_t);
	bool read_dma ( uint32_t, char*, uint32_t);
	void delay( double );
private:
    bool open_user( const char* );
    bool open_h2c ( const char* );
    bool open_c2h ( const char* );
    bool close_user();
    bool close_h2c();
    bool close_c2h();
private:
    PCIE_DEVICE m_PciIdx;
    char  m_cdevs[PCIE_XDMA_MAX_NUM][64];
    int   m_MemSize;
    int   m_MemBase;
    int m_fdUser;
    int m_fdH2C;
    int m_fdC2H;
    bool m_bOpen;
    //uint32_t* m_pMapBase;
    void* m_pMapBase;
};

#endif // CPCIE_HPP
