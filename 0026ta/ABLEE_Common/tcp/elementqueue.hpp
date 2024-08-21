#ifndef ELEMENTQUEUE_HPP
#define ELEMENTQUEUE_HPP

#include "commstruct.hpp"

class CElementQueue
{
public:
    CElementQueue();
    ~CElementQueue();
public:
    bool AddData(char* pData, int nData);
    bool GetElement(stTcpElement*);
    bool GetPacket(QByteArray&);
private:
    void SmartCopy(char* pDst, char* pSrc, int nSrcIndex, int nSrcSize, int nCopy);
private:
	const static int PACKET_BUFFER_SIZE = 1024000;
	char m_pBuffer[PACKET_BUFFER_SIZE];
    int  m_nIndex;
    int  m_nSize;
};

#endif // ELEMENTQUEUE_HPP
