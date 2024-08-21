#include "bitchart.hpp"
#include "io.hpp"
#include "log.hpp"
#include "vector.hpp"

extern CPcie g_pcie;
extern CLog  g_log;

CVector::CVector()
{
	ClearVector();
}

void CVector::ClearVector()
{
	memset( m_nVecData, 0x0, sizeof(m_nVecData) );
	m_nVecSize  = 0x0;
}

bool CVector::SetVectorData( unsigned char* pData ,unsigned int nSize )
{
	if( (nSize > MAX_VECTOR_8BIT_SIZE) || (nSize < 1 ) )
	{
		return false;
	}
	memcpy( m_nVecData, pData, nSize );
	m_nVecSize = nSize;
	return true;
}

bool CVector::SetVectorData( unsigned short* pData, unsigned int nSize )
{
	if( (nSize > MAX_VECTOR_8BIT_SIZE) || (nSize < 1 ) )
	{
		return false;
	}

	unsigned int nCount = nSize / sizeof(unsigned short);
	unsigned short* pBuffer = new unsigned short[nCount];

	memcpy( pBuffer, pData, nSize );
	for( unsigned int i=0;i<nCount;i++ )
	{
		pBuffer[i] = ((pBuffer[i]>>8)&0x00ff) | ((pBuffer[i]<<8)&0xff00);
	}
	memcpy( m_nVecData, pBuffer, nSize );
	m_nVecSize = nSize;
	delete[] pBuffer;
	return true;
}

bool CVector::SetVector( unsigned int* pData, unsigned int nSize )
{
	if( (nSize > MAX_VECTOR_8BIT_SIZE) || (nSize < 1 ) )
	{
		return false;
	}

	memcpy( m_nVecData, pData, nSize );
	m_nVecSize = nSize;
	return true;
}

void CVector::PrintVector( unsigned int fc, unsigned int start, unsigned int size )
{
	(void)fc;

	if( (size > m_nVecSize) || (size < 1 ) )
	{
		return;
	}

	for( unsigned int i=0;i<size;i++ )
	{
		int n32BitBegin = (start+i)/4;
		int n32BitOrder = (start+i)%4;

		unsigned char vector = m_nVecData[n32BitBegin]>>(n32BitOrder*8);
		g_log.Printf( "[%d] = %02X\n", start+i, vector );
	}
}

void CVector::WriteVector()
{
	unsigned int n32BitSize = m_nVecSize/sizeof(unsigned int);
	for( unsigned int i=0;i<n32BitSize;i++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_VECTOR_DATA1 + (i*4), m_nVecData[i] );
	}
}

//2022.12.10 - kwmoon(diag)
void CVector::GetVectorData(unsigned char* v, unsigned int size)
{
	unsigned char* p = v;
	unsigned int n32BitSize = size/sizeof(unsigned int);
	for( unsigned int i=0;i<n32BitSize;i++ )
	{
		int data = g_pcie.read_user( BITCHART::ALPG::_VECTOR_DATA1 + (i*4) );
		memcpy( p, &data, sizeof(int) );
		p+=4;
	}
}
