#ifndef CVECTOR_HPP
#define CVECTOR_HPP

#include <cmath>
#include "testlib_global.h"
#include "sysdef.hpp"
#include "atstring.hpp"

#define MAX_VECTOR_32BIT_SIZE		( 64 * 1024 )	// 64K * 8bit
#define MAX_VECTOR_8BIT_SIZE		(256 * 1024 )	//256K * 8bit

class TESTLIBSHARED_EXPORT CVector
{
public:
	CVector();
public:
	bool SetVectorData(unsigned char*, unsigned int);
	bool SetVectorData(unsigned short*,unsigned int);
	bool SetVector(unsigned int*,unsigned int);
	void ClearVector();
	void PrintVector(unsigned int, unsigned int, unsigned int );
	void WriteVector();
	void GetVectorData(unsigned char*, unsigned int);
private:
	unsigned int m_nVecData[MAX_VECTOR_32BIT_SIZE];
	unsigned int m_nVecSize;
};

#endif // CVECTOR_HPP
