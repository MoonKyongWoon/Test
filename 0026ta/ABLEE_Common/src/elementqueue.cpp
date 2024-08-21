#include <QDebug>
#include "elementqueue.hpp"

CElementQueue::CElementQueue()
{
    m_nIndex = 0;
    m_nSize = 0;
}

CElementQueue::~CElementQueue(void)
{

}

bool CElementQueue::AddData(char* pData, int nData)
{
    bool bResult = false;

	if ( m_nSize + nData < PACKET_BUFFER_SIZE )
    {
		int nEnd = (m_nIndex + m_nSize) % PACKET_BUFFER_SIZE;

		if (nEnd + nData < PACKET_BUFFER_SIZE)
        {
            memcpy( &m_pBuffer[nEnd], pData, static_cast<size_t>(nData) );
            m_nSize += nData;
        }
        else
        {
			int nMid = PACKET_BUFFER_SIZE - nEnd;
            memcpy( &m_pBuffer[nEnd], pData, static_cast<size_t>(nMid) );
            memcpy( m_pBuffer, &pData[nMid], static_cast<size_t>(nData - nMid) );
            m_nSize += nData;
        }
        bResult = true;
    }
    else
    {
        qDebug() << "Buffer full !!! \n";
    }

    return bResult;
}

bool CElementQueue::GetElement(stTcpElement* pEmt)
{
    int nHeader = sizeof(stTcpHeader);

    if (m_nSize >= nHeader)
    {
		SmartCopy( (char*)&pEmt, m_pBuffer, m_nIndex, PACKET_BUFFER_SIZE, nHeader);
		int nPacket = pEmt->tcpHeader.datalen;
        if (m_nSize >= nPacket + nHeader)
        {
            m_nSize -= nHeader;
            m_nIndex += nHeader;
			m_nIndex %= PACKET_BUFFER_SIZE;

            pEmt->pData = new char[nPacket];
			SmartCopy( pEmt->pData, m_pBuffer, m_nIndex, PACKET_BUFFER_SIZE, nPacket);

            m_nSize  -= nPacket;
            m_nIndex += nPacket;
			m_nIndex %= PACKET_BUFFER_SIZE;
            return true;
        }
    }
    return false;
}

bool CElementQueue::GetPacket( QByteArray& baBuffer )
{
    int nHeader = sizeof(stTcpHeader);
    if (m_nSize >= nHeader)
    {
        stTcpHeader header;
		SmartCopy( (char*)&header, m_pBuffer, m_nIndex, PACKET_BUFFER_SIZE, nHeader);
		int nPacket = header.datalen;
        if (m_nSize >= nPacket + nHeader)
        {
            int nTotalSize = static_cast<uint>(nHeader + nPacket);
            baBuffer.resize(nTotalSize+1);
            char* pBuffer = baBuffer.data();
            memcpy( pBuffer, (char*)&header, nHeader );

            m_nSize -= nHeader;
            m_nIndex += nHeader;
			m_nIndex %= PACKET_BUFFER_SIZE;

			SmartCopy( pBuffer+nHeader, m_pBuffer, m_nIndex, PACKET_BUFFER_SIZE, nPacket);

            m_nSize -= nPacket;
            m_nIndex += nPacket;
			m_nIndex %= PACKET_BUFFER_SIZE;
            return true;
        }
    }
    return false;
}

void CElementQueue::SmartCopy(char* pDst, char* pSrc, int nSrcIndex, int nSrcSize, int nCopy)
{
    int nEnd = nSrcIndex + nCopy;

    if (nSrcSize < nEnd)
    {
        int nFW = nSrcSize - nSrcIndex;
        int nBW = nEnd - nSrcSize;
        memcpy(pDst, &pSrc[nSrcIndex], nFW);
        memcpy(&pDst[nFW], pSrc, nBW);
    }
    else
    {
        memcpy(pDst, &pSrc[nSrcIndex], nCopy);
    }
}
