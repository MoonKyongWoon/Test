#ifndef CUBM_HPP
#define CUBM_HPP

#include <cmath>
#include "testlib_global.h"
#include "sysdef.hpp"
#include "atstring.hpp"

union UBM_STRUCT
{
	unsigned long long int io48;
	struct
	{
		unsigned int low32  : 32;
		unsigned int high16 : 16;
	};
};

class TESTLIBSHARED_EXPORT CUbm
{
public:
    CUbm();
	~CUbm();
public:
	void SetUdata0x00();
	void SetUdata0xFF();
	void SetUdata32( unsigned int fc, int index, int index2, unsigned int data );
	void SetUdata( unsigned int fc, unsigned int ioab, unsigned int index, unsigned long long int data );
	void SetUdataOr( unsigned int fc, unsigned int ioab, int index, unsigned long long int data );
	//void SetUdataDut( unsigned int dut_index, int real_scan, unsigned int failth, unsigned int data, int jump );
	unsigned int GetUdata32( unsigned int fc, int index, int index2 );
	unsigned long long GetUdata( unsigned int fc, unsigned int ioab, int index );
	void PrintfUbm( int fc, int size = UBM_DEPTH);

};

#endif // UBM_HPP
