/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * DBM
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CDBM_HPP
#define CDBM_HPP

#include <cmath>
#include <iostream>
#include <exception>
#include "testlib_global.h"
#include "sysdef.hpp"
#include "atstring.hpp"

#define DBM_START_ADDR			( 0x100000000 )
#define MAX_DBM_SIZE			( 256 * 1024 * 1024 )	// 256 M * 4
#define MAX_DBM_SIZE_32M		(  32 * 1024 * 1024 )	//  32 MByte
#define MAX_DBM_SIZE_64M		(  64 * 1024 * 1024 )	//  64 MByte * 4BYTE
#define MAX_DBM_SIZE_128M		( 128 * 1024 * 1024 )	// 128 MByte / 2  * 4BYTE

#pragma pack(1)
/*
union DBM_STRUCT
{
	unsigned int	dword;
	struct
	{
		unsigned char	sdr_byte0;
		unsigned char	sdr_byte1;
		unsigned char	ddr_byte0;
		unsigned char	ddr_byte1;
	};
};
*/

//2023.02.27 - KWMOON
union DBM_STRUCT
{
	unsigned long int	dword;
	struct
	{
		unsigned char	sdr_byte0;
		unsigned char	sdr_byte1;
		unsigned char	ddr_byte0;
		unsigned char	ddr_byte1;
		unsigned int    dummy;
	};
};
#pragma pack()


class TESTLIBSHARED_EXPORT CDbm
{
public:
    CDbm();
	~CDbm();
public:
	void DbmCtrlReset();
	void ClearDbm();
	void SetDbmData( unsigned char*, unsigned int size, bool ddr = false );
	void SetDbmData( unsigned short*, unsigned int size, bool ddr = false );
	void WriteDbm(unsigned int, unsigned int );
	void PrintfDbm( unsigned int, unsigned int, unsigned int );
	void PrintfDbm( unsigned int, unsigned int );
	uint ReadDbm(unsigned int );	//for debug
	bool CreateBgMemory();		//BG Write
	bool SetBgDbm(uint,int);	//BG Write
	bool WriteDbmMemory( bool BGWrite = false);
private:
	void NewBuf( int );
	void DeleteBuf();
private:
	DBM_STRUCT* m_pDbm;
	DBM_STRUCT* m_pBgDbm;
	int m_nSize;
	int m_nBgSize;
};

#endif // DBM_HPP
