#include "../sys/secsmsg.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM BASE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemBase::CItemBase( HSMS_FMT_CODE fmt,int size )
{
	fmt_code = fmt;
	fmt_size = size;
	data_count = 0;
}

void CItemBase::IncreaseDataCount()
{
	data_count++;
}

void CItemBase::DecreaseDataCount()
{
	data_count--;
}

void CItemBase::SetDataCount(int count)
{
	data_count = count;
}

int CItemBase::GetDataCount()
{
	return data_count;
}
int CItemBase::GetFormatSize()
{
	return fmt_size;
}

HSMS_FMT_CODE CItemBase::GetFormat()
{
	return fmt_code;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM LIST
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemList::CItemList( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	SetDataCount( 0 );
}

void CItemList::SetListCount( uint count )
{
	SetDataCount( count );
}

void CItemList::IncreaseListCount()
{
	IncreaseDataCount();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM BIN
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemBin::CItemBin( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemBin::~CItemBin()
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
}

int CItemBin::SetData( short* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	pData = new short[count];
	this->SetDataCount(count);	// hsbae 2024.06.07
	memcpy( pData, ptr, count*sizeof(short) );
	return _SUCCESS;
}

int CItemBin::GetData( short* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(short)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM BOOLEAN
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemBool::CItemBool( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemBool::~CItemBool()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemBool::SetData( short* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new short[count];
	memcpy( pData, ptr, count*sizeof(short) );
	return _SUCCESS;
}

int CItemBool::GetData( short* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(short)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM ASCII
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemAscii::CItemAscii( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemAscii::~CItemAscii()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemAscii::SetData( char* ptr, uint length )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( length );
	pData = new char[length];
	memcpy( pData, ptr, length*sizeof(char) );
	return _SUCCESS;
}

int CItemAscii::GetData( char* ptr, uint length )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = length;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(char)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM I1
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemI1::CItemI1( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	this->pData = nullptr;
}

CItemI1::~CItemI1()
{
	if( this->pData != nullptr )
	{
		delete this->pData;
	}
}

int CItemI1::SetData( short* ptr, uint count )
{
	if( this->pData != nullptr )
	{
		delete pData;
		this->pData = nullptr;
	}
	SetDataCount( count );
	this->pData = new short[count];
	memcpy( this->pData, ptr, count*sizeof(short) );
	return _SUCCESS;
}

int CItemI1::GetData( short* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(short)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM I2
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemI2::CItemI2( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemI2::~CItemI2()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemI2::SetData( short* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new short[count];
	memcpy( pData, ptr, count*sizeof(short) );
	return _SUCCESS;
}

int CItemI2::GetData( short* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(short)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM I4
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemI4::CItemI4( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemI4::~CItemI4()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemI4::SetData( int* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new int[count];
	memcpy( pData, ptr, count*sizeof(int) );
	return _SUCCESS;
}

int CItemI4::GetData( int* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(int)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM I8
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemI8::CItemI8( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemI8::~CItemI8()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemI8::SetData( long int* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new long int[count];
	memcpy( pData, ptr, count*sizeof(long int) );
	return _SUCCESS;
}

int CItemI8::GetData( long int* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(long int)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM UI1
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemU1::CItemU1( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemU1::~CItemU1()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemU1::SetData( ushort* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new ushort[count];
	memcpy( pData, ptr, count*sizeof(ushort) );
	return _SUCCESS;
}

int CItemU1::GetData( ushort* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(ushort)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM UI2
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemU2::CItemU2( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemU2::~CItemU2()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemU2::SetData( ushort* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new ushort[count];
	memcpy( pData, ptr, count*sizeof(ushort) );
	return _SUCCESS;
}

int CItemU2::GetData( ushort* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(ushort)*getsize );
	return getsize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM U4
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemU4::CItemU4( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemU4::~CItemU4()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemU4::SetData( uint* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new uint[count];
	memcpy( pData, ptr, count*sizeof(uint) );
	return _SUCCESS;
}

int CItemU4::GetData( uint* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(uint)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM U8
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemU8::CItemU8( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemU8::~CItemU8()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemU8::SetData( unsigned long* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new unsigned long [count];
	memcpy( pData, ptr, count*sizeof(unsigned long) );
	return _SUCCESS;
}

int CItemU8::GetData( unsigned long* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(unsigned long)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM F4
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemF4::CItemF4( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemF4::~CItemF4()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemF4::SetData( float* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new float[count];
	memcpy( pData, ptr, count*sizeof(float) );
	return _SUCCESS;
}

int CItemF4::GetData( float* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(float)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ITEM F8
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CItemF8::CItemF8( HSMS_FMT_CODE fmt,int size ) : CItemBase( fmt, size )
{
	pData = nullptr;
}

CItemF8::~CItemF8()
{
	if( pData != nullptr )
	{
		delete pData;
	}
}

int CItemF8::SetData( double* ptr, uint count )
{
	if( pData != nullptr )
	{
		delete pData;
		pData = nullptr;
	}
	SetDataCount( count );
	pData = new double[count];
	memcpy( pData, ptr, count*sizeof(double) );
	return _SUCCESS;
}

int CItemF8::GetData( double* ptr, uint count )
{
	if( (ptr == nullptr) || (pData == nullptr) )
	{
		return _ERROR_;
	}
	int getsize = count;
	if( getsize > GetDataCount() )
		getsize = GetDataCount();
	memcpy( ptr, this->pData, sizeof(double)*getsize );
	return getsize;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
///SECS MESSAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSecsMsg::CSecsMsg()
{
	CSecsMsg(0,0,0,0);
}

CSecsMsg::CSecsMsg( short s, short f, short w, short d )
{
	Init();
	m_nStream = s;
	m_nFunction = f;
	m_nWait = w;
	m_nDeviceId = d;
	memset( &m_stHsmsElement.hsmsHeader, 0x0, sizeof(stHsmsHeader) );
	m_stHsmsElement.hsmsHeader.stMsgHeader.cStream   = static_cast<char>(s);
	m_stHsmsElement.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(f);
	m_stHsmsElement.hsmsHeader.stMsgHeader.cWbit     = static_cast<char>(w);
	m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidMsb = (d>>8)&0x7f;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidLsb = (d>>0)&0xff;
}

CSecsMsg::~CSecsMsg()
{
	for( CItemBase* item : m_listItem )
	{
		delete item;
	}
	for( CItemList* item : m_stackList )
	{
		delete item;
	}
	m_listItem.clear();
	m_stackList.clear();
}

void CSecsMsg::Init()
{
	m_nStream = 0;
	m_nFunction = 0;
	m_nWait = 0;
	m_nSystemBytes = 0;
	m_nItemPointer = 0;
	m_nDeviceId = 0;
	m_nRBit = 0;
	memset( &m_stHsmsElement, 0x0, sizeof(stHsmsElement) );
	for( CItemBase* item : m_listItem )
	{
		delete item;
	}
	for( CItemList* item : m_stackList )
	{
		delete item;
	}
	m_listItem.clear();
	m_stackList.clear();
}

uint CSecsMsg::GetSystemBytes()
{
	return m_nSystemBytes;
}

int CSecsMsg::GetDeviceId()
{
	return m_nDeviceId;
}

int CSecsMsg::GetRBit()
{
	return m_nRBit;
}

void CSecsMsg::SetSystemBytes(int sb)
{
	m_nSystemBytes = sb;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[0] = ( sb>>24) & 0xff;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[1] = ( sb>>16) & 0xff;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[2] = ( sb>> 8) & 0xff;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[3] = ( sb>> 0) & 0xff;
}

void CSecsMsg::SetDeviceId(uint id)
{
	m_nDeviceId = id;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidMsb = (id>>8)&0x7f;
	m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidLsb = (id>>0)&0xff;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HSMS PACKET TO MESSAGE & USER MAKED MESSAGE TO HSMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSecsMsg::HsmsToMsg(char* ptr, uint len)
{
	unsigned int nDataLen = 0;
	unsigned int nMsgLength = 0x0;
	if( len < sizeof(stHsmsHeader) )
		return _ERROR_;

	Init();
	memcpy( &m_stHsmsElement.hsmsHeader, ptr, sizeof(stHsmsHeader) );

	// hsbae
	m_nStream = 0;
	m_nStream |= m_stHsmsElement.hsmsHeader.stMsgHeader.cStream;

	m_nFunction = 0;
	m_nFunction |= m_stHsmsElement.hsmsHeader.stMsgHeader.cFunction;

	m_nWait = 0;
	m_nWait |= m_stHsmsElement.hsmsHeader.stMsgHeader.cWbit;

	m_nSystemBytes = 0;
	m_nSystemBytes |= ( ( m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[0]&0xff) << 24 );
	m_nSystemBytes |= ( ( m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[1]&0xff) << 16 );
	m_nSystemBytes |= ( ( m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[2]&0xff) <<  8 );
	m_nSystemBytes |= ( ( m_stHsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[3]&0xff) <<  0 );

	m_nDeviceId = (m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidMsb << 8 ) | m_stHsmsElement.hsmsHeader.stMsgHeader.cDevidLsb;
	m_nRBit     = m_stHsmsElement.hsmsHeader.stMsgHeader.cRbit;

	nMsgLength |= ( (m_stHsmsElement.hsmsHeader.szMsgLength[0]&0xff) << 24);
	nMsgLength |= ( (m_stHsmsElement.hsmsHeader.szMsgLength[1]&0xff) << 16);
	nMsgLength |= ( (m_stHsmsElement.hsmsHeader.szMsgLength[2]&0xff) <<  8);
	nMsgLength |= ( (m_stHsmsElement.hsmsHeader.szMsgLength[3]&0xff) <<  0);

	nDataLen = nMsgLength - sizeof(stHsmsMsgHeader);

	QByteArray baPacket;
	unsigned int iPos = 0;
	if( nDataLen > 0 )
	{
		baPacket.append( ptr+ sizeof(stHsmsHeader), nDataLen );
		while( iPos < nDataLen )
		{
			char format_byte = baPacket.at(iPos++);
			HSMS_FMT_CODE fmt_code  = static_cast<HSMS_FMT_CODE>((format_byte >> 2) & 0x3f);
			unsigned int num_of_len_byte = format_byte & 0x3;
			unsigned int data_count = 0x0;
			for( unsigned int lb=0;lb<num_of_len_byte;lb++ )
			{
				unsigned char byte = baPacket.at(iPos++);
				unsigned int shift = (num_of_len_byte-1-lb)*8;
				data_count |= (byte<<shift);
			}
			switch( fmt_code )
			{
				case FMT_LIST:
				{
					AddListItem( data_count );
				}
				break;

				case FMT_BIN  :
				{
					short* pBin = new short[data_count];
					memset( pBin, 0x0, sizeof(short)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						pBin[i] = baPacket.at(iPos++)&0xFF;
					}
					AddBinaryItem( pBin, data_count );
					delete[] pBin;
				}
				break;

				case FMT_BOOL :
				{
					short* pBool = new short[data_count];
					memset( pBool, 0x0, sizeof(short)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						pBool[i] = baPacket.at(iPos++)&0xFF;
					}
					AddBooleanItem( pBool, data_count );
					delete[] pBool;
				}
				break;

				case FMT_ASCII:
				{
					char* pAscii = new char[data_count+1];
					memset( pAscii, 0x0, data_count+1 );
					for( unsigned int i=0;i<data_count;i++ )
					{
						pAscii[i] = baPacket.at(iPos++);
					}
					pAscii[data_count] = '\0';
					AddAsciiItem( pAscii, data_count );
					delete[] pAscii;
				}
				break;

				case FMT_JIS_8:
				{

				}
				break;

				case FMT_I8   :
				{
					const unsigned int DEPTH = 8;
					data_count /= DEPTH;
					long int* pI8 = new long int[data_count];
					memset( pI8, 0x0, sizeof(long int)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							long int lba = (baPacket.at(iPos++)&0xFF);
							pI8[i] |= ( lba << ((DEPTH-1-d)*8) );
						}
					}
					AddI8Item( pI8, data_count );
					delete[] pI8;
				}
				break;

				case FMT_I1   :
				{
					const unsigned int DEPTH = 1;
					data_count /= DEPTH;
					short* pI1 = new short[data_count];
					memset( pI1, 0x0, sizeof(short)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pI1[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddI1Item( pI1, data_count );
					delete[] pI1;
				}
				break;

				case FMT_I2   :
				{
					const unsigned int DEPTH = 2;
					data_count /= DEPTH;
					short* pI2 = new short[data_count];
					memset( pI2, 0x0, sizeof(short)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pI2[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddI2Item( pI2, data_count );
					delete[] pI2;
				}
				break;

				case FMT_I4   :
				{
					const unsigned int DEPTH = 4;
					data_count /= DEPTH;
					int* pI4 = new int[data_count];
					memset( pI4, 0x0, sizeof(int)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pI4[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddI4Item( pI4, data_count );
					delete[] pI4;
				}
				break;

				case FMT_F8   :
				{
					const unsigned int DEPTH = 8;
					data_count /= DEPTH;
					double* pF8 = new double[data_count];
					memset( pF8, 0x0, sizeof(double)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						long int lF8 = 0;						
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							long int lba = (baPacket.at(iPos++)&0xFF);
							lF8 |= ( lba << ((DEPTH-1-d)*8) );
						}
						memcpy( &pF8[i], &lF8, sizeof(double) );
						//pF8[i] = *reinterpret_cast<double*>(&lF8);
					}
					AddF8Item( pF8, data_count );
					delete[] pF8;
				}
				break;

				case FMT_F4   :
				{
					const unsigned int DEPTH = 4;
					data_count /= DEPTH;
					float* pF4 = new float[data_count];
					memset( pF4, 0x0, sizeof(float)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						int iF4 = 0;
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							iF4 |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
						memcpy( &pF4[i], &iF4, sizeof(float) );
						//pF4[i] = *reinterpret_cast<float*>(&iF4);
					}
					AddF4Item( pF4, data_count );
					delete[] pF4;
				}
				break;

				case FMT_U8   :
				{
					const unsigned int DEPTH = 8;
					data_count /= DEPTH;
					unsigned long int* pU8 = new unsigned long int[data_count];
					memset( pU8, 0x0, sizeof(unsigned long int)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							long int lba = (baPacket.at(iPos++)&0xFF);
							pU8[i] |= ( lba << ((DEPTH-1-d)*8) );
						}
					}
					AddU8Item( pU8, data_count );
					delete[] pU8;
				}
				break;

				case FMT_U1   :
				{
					const unsigned int DEPTH = 1;
					data_count /= DEPTH;
					ushort* pU1 = new ushort[data_count];
					memset( pU1, 0x0, sizeof(ushort)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pU1[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddU1Item( pU1, data_count );
					delete[] pU1;
				}
				break;

				case FMT_U2   :
				{
					const unsigned int DEPTH = 2;
					data_count /= DEPTH;
					ushort* pU2 = new ushort[data_count];
					memset( pU2, 0x0, sizeof(ushort)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pU2[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddU2Item( pU2, data_count );
					delete[] pU2;
				}
				break;

				case FMT_U4   :
				{
					const unsigned int DEPTH = 4;
					data_count /= DEPTH;
					uint* pU4 = new uint[data_count];
					memset( pU4, 0x0, sizeof(uint)*data_count );
					for( unsigned int i=0;i<data_count;i++ )
					{
						for( unsigned int d=0;d<DEPTH;d++ )
						{
							pU4[i] |= ( (baPacket.at(iPos++)&0xFF) << ((DEPTH-1-d)*8) );
						}
					}
					AddU4Item( pU4, data_count );
					delete[] pU4;
				}
				break;

				default:
					break;
			}
		}
	}
	return _SUCCESS;
}

atstring CSecsMsg::getTypeCode(HSMS_FMT_CODE code)
{
	switch(code)
	{
		case FMT_LIST   :
			return "L";
		case FMT_BIN    :
			return "B";
		case FMT_BOOL   :
			return "BOOLEAN";
		case FMT_JIS_8  :
			return "J"; // temp
		case FMT_I1     :
			return "I1";
		case FMT_U1     :
			return "U1";
		case FMT_ASCII  :
			return "A";
		case FMT_I2     :
			return "I2";
		case FMT_U2     :
			return "U2";
		case FMT_I4     :
			return "I4";
		case FMT_U4     :
			return "U4";
		case FMT_F4     :
			return "F4";
		case FMT_I8     :
			return "I8";
		case FMT_U8     :
			return "U8";
		case FMT_F8     :
			return "F8";
		default:
			return "UNKNOWN";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///MESSAGE TO SML STRING
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSecsMsg::MsgToSML(atstring& rSML)
{
	atstring tmp_str = "";
	atstring title = "";
	unsigned int i = 0;

	if( m_stHsmsElement.hsmsHeader.stMsgHeader.cWbit )
		tmp_str.Format("S%dF%d [W]\n", m_stHsmsElement.hsmsHeader.stMsgHeader.cStream, m_stHsmsElement.hsmsHeader.stMsgHeader.cFunction);
	else
		tmp_str.Format("S%dF%d\n", m_stHsmsElement.hsmsHeader.stMsgHeader.cStream, m_stHsmsElement.hsmsHeader.stMsgHeader.cFunction);

	rSML = tmp_str;

	int indentation = 0;
	QVector<int> list_count = {0};
	for( CItemBase* ptr : m_listItem )
	{
		tmp_str = "";
		for (int depth = 0; depth < indentation; depth++)
		{
			tmp_str += "\t";
		}
		switch( ptr->GetFormat() )
		{
			case FMT_LIST  :
			{
				CItemList* ptrList = static_cast<CItemList*>(ptr);
				if( ptrList != nullptr )
				{
					int count = ptrList->GetDataCount();
					title.Format("<%s [%d]", getTypeCode(FMT_LIST).c_str(), count);
					tmp_str += title;
					if (count == 0)
					{
						tmp_str += ">";
					}
					else
					{
						indentation++;
						list_count.push_back(count + 1);
					}
					rSML += tmp_str;
				}
			}
			break;

			case FMT_BIN   :
			{
				CItemBin* ptrBin = static_cast<CItemBin*>(ptr);
				if( ptrBin != nullptr )
				{
					int stored_count = ptrBin->GetDataCount();
					short* pBin = new short[stored_count];
					int count = ptrBin->GetData( pBin, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_BIN).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" 0x%X", pBin[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pBin;
				}
			}
			break;

			case FMT_BOOL  :
			{
				CItemBool* ptrBool = static_cast<CItemBool*>(ptr);
				if( ptrBool != nullptr )
				{
					int stored_count = ptrBool->GetDataCount();
					short* pBool = new short[stored_count];
					int count = ptrBool->GetData( pBool, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_BOOL).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pBool[i] );
							tmp_str += title;
						}
						tmp_str += ">";
					}
					rSML += tmp_str;
					delete[] pBool;
				}
			}
			break;

			case FMT_ASCII :
			{
				CItemAscii* ptrAsc = static_cast<CItemAscii*>(ptr);
				if( ptrAsc != nullptr )
				{
					int stored_count = ptrAsc->GetDataCount();
					char* pAscii = new char[stored_count+1];
					memset( pAscii, 0x0, stored_count+1 );
					int count = ptrAsc->GetData( pAscii, stored_count );
					if( count > 0 )
					{
						pAscii[count]='\0';
						title.Format("<%s [%d] \"%s\">", getTypeCode(FMT_ASCII).c_str(), count, pAscii );
						tmp_str += title;
					}
					else
					{
						title.Format("<%s [%d]>", getTypeCode(FMT_ASCII).c_str(), count );
						tmp_str += title;
					}
					rSML += tmp_str;
					delete[] pAscii;
				}
			}
			break;

			case FMT_JIS_8 :
				break;

			case FMT_I8    :
			{
				CItemI8* ptrI8 = static_cast<CItemI8*>(ptr);
				if( ptrI8 != nullptr )
				{
					int stored_count = ptrI8->GetDataCount();
					long int* pI8 = new long int[stored_count];
					int count = ptrI8->GetData( pI8, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_I8).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %lld", pI8[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pI8;
				}
			}
			break;

			case FMT_I1    :
			{
				CItemI1* ptrI1 = static_cast<CItemI1*>(ptr);
				if( ptrI1 != nullptr )
				{
					int stored_count = ptrI1->GetDataCount();
					short* pI1 = new short[stored_count];
					int count = ptrI1->GetData( pI1, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_I1).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pI1[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pI1;
				}
			}
			break;

			case FMT_I2    :
			{
				CItemI2* ptrI2 = static_cast<CItemI2*>(ptr);
				if( ptrI2 != nullptr )
				{
					int stored_count = ptrI2->GetDataCount();
					short* pI2 = new short[stored_count];
					int count = ptrI2->GetData( pI2, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_I2).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pI2[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pI2;
				}
			}
			break;

			case FMT_I4    :
			{
				CItemI4* ptrI4 = static_cast<CItemI4*>(ptr);
				if( ptrI4 != nullptr )
				{
					int stored_count = ptrI4->GetDataCount();
					int* pI4 = new int[stored_count];
					int count = ptrI4->GetData( pI4, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_I4).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pI4[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pI4;
				}
			}
			break;

			case FMT_F8    :
			{
				CItemF8* ptrI8 = static_cast<CItemF8*>(ptr);
				if( ptrI8 != nullptr )
				{
					int stored_count = ptrI8->GetDataCount();
					double* pF8 = new double[stored_count];
					int count = ptrI8->GetData( pF8, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_F8).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %f", pF8[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pF8;
				}
			}
			break;

			case FMT_F4    :
			{
				CItemF4* ptrF4 = static_cast<CItemF4*>(ptr);
				if( ptrF4 != nullptr )
				{
					int stored_count = ptrF4->GetDataCount();
					float* pF4 = new float[stored_count];
					int count = ptrF4->GetData( pF4, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_F4).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %f", pF4[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pF4;
				}
			}
			break;

			case FMT_U8    :
			{
				CItemU8* ptrU8 = static_cast<CItemU8*>(ptr);
				if( ptrU8 != nullptr )
				{
					int stored_count = ptrU8->GetDataCount();
					unsigned long* pU8 = new unsigned long[stored_count];
					int count = ptrU8->GetData( pU8, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_U8).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %lld", pU8[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pU8;
				}
			}
			break;

			case FMT_U1    :
			{
				CItemU1* ptrU1 = static_cast<CItemU1*>(ptr);
				if( ptrU1 != nullptr )
				{
					int stored_count = ptrU1->GetDataCount();
					ushort* pU1 = new ushort[stored_count];
					int count = ptrU1->GetData( pU1, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_U1).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pU1[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pU1;
				}
			}
			break;

			case FMT_U2    :
			{
				CItemU2* ptrU2 = static_cast<CItemU2*>(ptr);
				if( ptrU2 != nullptr )
				{
					int stored_count = ptrU2->GetDataCount();
					ushort* pU2 = new ushort[stored_count];
					int count = ptrU2->GetData( pU2, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_U2).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pU2[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pU2;
				}
			}
			break;

			case FMT_U4    :
			{
				CItemU4* ptrU4 = static_cast<CItemU4*>(ptr);
				if( ptrU4 != nullptr )
				{
					int stored_count = ptrU4->GetDataCount();
					uint* pU4 = new uint[stored_count];
					int count = ptrU4->GetData( pU4, stored_count );
					if( count >= 0 )
					{
						title.Format("<%s [%d]", getTypeCode(FMT_U4).c_str(), count );
						tmp_str += title;
						for( int i=0;i<count;i++ )
						{
							title.Format(" %d", pU4[i] );
							tmp_str += title;
						}
						tmp_str += ">";
						rSML += tmp_str;
					}
					delete[] pU4;
				}
			}
			break;

			default:
			{

			}
			break;
		}
		if (list_count[indentation] > 0)
		{
			list_count[indentation]--;
			rSML += "\n";
		}

		if (list_count[indentation] == 0)
		{
			while (indentation > 0)
			{
				indentation--;
				list_count.pop_back();
				list_count[indentation]--;

				for (int depth = 0; depth < indentation; depth++)
				{
					rSML += "\t";
				}

				rSML += ">";

				if(indentation == 0)
				{
					break;
				}
				else
				{
					if (list_count[indentation] > 0)
					{
						rSML += "\n";

						break;
					}
					rSML += "\n";
				}
			}
		}
	}
	rSML += ".";
	return _SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MESSAGE TO HSMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSecsMsg::MakeHsmsBody()
{
	m_baMsgBody.clear();
	for( CItemBase* ptr : m_listItem )
	{
		QByteArray baFmtLength;
		QByteArray baData;

		baFmtLength.clear();
		if( ptr->GetDataCount() == 0 )
		{
			baFmtLength.append( ptr->GetFormat()<<2 | 1 );
			baFmtLength.append( ptr->GetDataCount()&0xff );
		}
		else if( ptr->GetDataCount() < 0xff )
		{
			baFmtLength.append( ptr->GetFormat()<<2 | 1 );
			baFmtLength.append( ptr->GetDataCount()*ptr->GetFormatSize()&0xff );
		}
		else if( ptr->GetDataCount() < 0xffff )
		{
			baFmtLength.append( ptr->GetFormat()<<2 | 2 );
			baFmtLength.append( (ptr->GetDataCount()*ptr->GetFormatSize()>>8)&0xff );
			baFmtLength.append( (ptr->GetDataCount()*ptr->GetFormatSize()>>0)&0xff );
		}
		else if( ptr->GetDataCount() < 0xffffff )
		{
			baFmtLength.append( ptr->GetFormat()<<2 | 3 );
			baFmtLength.append( (ptr->GetDataCount()*ptr->GetFormatSize()>>16)&0xff );
			baFmtLength.append( (ptr->GetDataCount()*ptr->GetFormatSize()>> 8)&0xff );
			baFmtLength.append( (ptr->GetDataCount()*ptr->GetFormatSize()>> 0)&0xff );
		}
		else
		{
			return _ERROR_;
		}
		m_baMsgBody.append( baFmtLength );

		baData.clear();
		switch( ptr->GetFormat() )
		{
			case FMT_LIST  :
			{
			}
			break;

			case FMT_BIN   :
			{
				CItemBin* ptrBin = static_cast<CItemBin*>(ptr);
				if( ptrBin != nullptr )
				{
					int stored_count = ptrBin->GetDataCount();
					short* pBin = new short[stored_count];
					int count = ptrBin->GetData( pBin, stored_count );
					for( int i=0;i<count;i++ )
					{
						baData.append( pBin[i] );
					}
					delete[] pBin;
				}
			}
			break;

			case FMT_BOOL  :
			{
				CItemBool* ptrBool = static_cast<CItemBool*>(ptr);
				if( ptrBool != nullptr )
				{
					int stored_count = ptrBool->GetDataCount();
					short* pBool = new short[stored_count];
					int count = ptrBool->GetData( pBool, stored_count );
					for( int i=0;i<count;i++ )
					{
						baData.append( pBool[i] );
					}
					delete[] pBool;
				}
			}
			break;

			case FMT_ASCII :
			{
				CItemAscii* ptrAsc = static_cast<CItemAscii*>(ptr);
				if( ptrAsc != nullptr )
				{
					int stored_count = ptrAsc->GetDataCount();
					char* pAscii = new char[stored_count+1];
					memset( pAscii, 0x0, stored_count+1 );
					int count = ptrAsc->GetData( pAscii, stored_count );
					if( count > 0 )
					{
						baData.append(pAscii, count );
					}
					delete[] pAscii;
				}
			}
			break;

			case FMT_JIS_8 :
				break;

			case FMT_I8    :
			{
				CItemI8* ptrI8 = static_cast<CItemI8*>(ptr);
				if( ptrI8 != nullptr )
				{
					int stored_count = ptrI8->GetDataCount();
					long int* pI8 = new long int[stored_count];
					int count = ptrI8->GetData( pI8, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pI8[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pI8;
				}
			}
			break;

			case FMT_I1    :
			{
				CItemI1* ptrI1 = static_cast<CItemI1*>(ptr);
				if( ptrI1 != nullptr )
				{
					int stored_count = ptrI1->GetDataCount();
					short* pI1 = new short[stored_count];
					int count = ptrI1->GetData( pI1, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pI1[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pI1;
				}
			}
			break;

			case FMT_I2    :
			{
				CItemI2* ptrI2 = static_cast<CItemI2*>(ptr);
				if( ptrI2 != nullptr )
				{
					int stored_count = ptrI2->GetDataCount();
					short* pI2 = new short[stored_count];
					int count = ptrI2->GetData( pI2, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pI2[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pI2;
				}
			}
			break;

			case FMT_I4    :
			{
				CItemI4* ptrI4 = static_cast<CItemI4*>(ptr);
				if( ptrI4 != nullptr )
				{
					int stored_count = ptrI4->GetDataCount();
					int* pI4 = new int[stored_count];
					int count = ptrI4->GetData( pI4, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pI4[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pI4;
				}
			}
			break;

			case FMT_F8    :
			{
				CItemF8* ptrI8 = static_cast<CItemF8*>(ptr);
				if( ptrI8 != nullptr )
				{
					int stored_count = ptrI8->GetDataCount();
					double* pF8 = new double[stored_count];
					int count = ptrI8->GetData( pF8, stored_count );
					for( int i=0;i<count;i++ )
					{
						char szF8[8]={0x0,};
						memcpy( szF8, &pF8[i], sizeof(double) );
						for( int d=ptr->GetFormatSize()-1;d>=0;d-- )
						{
							baData.append( szF8[d] );
						}
					}
					delete[] pF8;
				}
			}
			break;

			case FMT_F4    :
			{
				CItemF4* ptrF4 = static_cast<CItemF4*>(ptr);
				if( ptrF4 != nullptr )
				{
					int stored_count = ptrF4->GetDataCount();
					float* pF4 = new float[stored_count];
					int count = ptrF4->GetData( pF4, stored_count );
					for( int i=0;i<count;i++ )
					{
						char szF4[4]={0x0,};
						memcpy( szF4, &pF4[i], sizeof(float) );
						for( int d=ptr->GetFormatSize()-1;d>=0;d-- )
						{
							baData.append( szF4[d] );
						}
					}
					delete[] pF4;
				}
			}
			break;

			case FMT_U8    :
			{
				CItemU8* ptrU8 = static_cast<CItemU8*>(ptr);
				if( ptrU8 != nullptr )
				{
					int stored_count = ptrU8->GetDataCount();
					unsigned long* pU8 = new unsigned long[stored_count];
					int count = ptrU8->GetData( pU8, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pU8[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pU8;
				}
			}
			break;

			case FMT_U1    :
			{
				CItemU1* ptrU1 = static_cast<CItemU1*>(ptr);
				if( ptrU1 != nullptr )
				{
					int stored_count = ptrU1->GetDataCount();
					ushort* pU1 = new ushort[stored_count];
					int count = ptrU1->GetData( pU1, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pU1[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pU1;
				}
			}
			break;

			case FMT_U2    :
			{
				CItemU2* ptrU2 = static_cast<CItemU2*>(ptr);
				if( ptrU2 != nullptr )
				{
					int stored_count = ptrU2->GetDataCount();
					ushort* pU2 = new ushort[stored_count];
					int count = ptrU2->GetData( pU2, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pU2[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pU2;
				}
			}
			break;

			case FMT_U4    :
			{
				CItemU4* ptrU4 = static_cast<CItemU4*>(ptr);
				if( ptrU4 != nullptr )
				{
					int stored_count = ptrU4->GetDataCount();
					uint* pU4 = new uint[stored_count];
					int count = ptrU4->GetData( pU4, stored_count );
					for( int i=0;i<count;i++ )
					{
						for( int d=0;d<ptr->GetFormatSize();d++ )
						{
							baData.append( (pU4[i]>>(ptr->GetFormatSize()-1-d)*8) );
						}
					}
					delete[] pU4;
				}
			}
			break;

			default:
				break;
		}
		m_baMsgBody.append( baData );
	}
	return _SUCCESS;
}

int CSecsMsg::MakeHsmsHeader()
{
	int MsgLen = m_baMsgBody.length() + sizeof(stHsmsMsgHeader);
	m_stHsmsElement.hsmsHeader.szMsgLength[0] = (MsgLen>>24)&0xFF;
	m_stHsmsElement.hsmsHeader.szMsgLength[1] = (MsgLen>>16)&0xFF;
	m_stHsmsElement.hsmsHeader.szMsgLength[2] = (MsgLen>> 8)&0xFF;
	m_stHsmsElement.hsmsHeader.szMsgLength[3] = (MsgLen>> 0)&0xFF;
	m_baMsgHeader.clear();
	m_baMsgHeader.append( (char*)&m_stHsmsElement.hsmsHeader, sizeof(stHsmsHeader) );
	return _SUCCESS;
}

int CSecsMsg::MsgToHsms(QByteArray& baHsms)
{
	bool bMaked = true;
	baHsms.clear();
	if( MakeHsmsBody()   < 0 ) bMaked = false;
	if( MakeHsmsHeader() < 0 ) bMaked = false;
	if( bMaked )
	{
		baHsms.append( m_baMsgHeader );
		baHsms.append( m_baMsgBody   );
		return _SUCCESS;
	}
	return _ERROR_;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ADD ITEM  API
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSecsMsg::OpenListItem()
{
	CItemList* ptr = new CItemList( FMT_LIST, 1 );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	m_stackList.push( ptr );
	return _SUCCESS;
}

int CSecsMsg::CloseListItem()
{
	if( !m_stackList.isEmpty() )
	{
		m_stackList.pop();
	}
	return _SUCCESS;
}

int CSecsMsg::AddListItem(int count)
{
	CItemList* ptr = new CItemList( FMT_LIST, 1 );
	ptr->SetListCount( count);
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
	//m_nCurListCount = 0;
	//stOpenList* pOpenList = new stOpenList;
	//pOpenList->have_parent = false;
	//if ( !m_vecList.isEmpty() )
	//{
	//	pOpenList->have_parent = true;
	//}
	//pOpenList->count = count;
	//m_vecList.append( pOpenList );
}

int CSecsMsg::AddBinaryItem( short* pValue, uint count )
{
	CItemBin* ptr = new CItemBin( FMT_BIN, 1 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddBooleanItem( short* pValue, uint count )
{
	CItemBool* ptr = new CItemBool( FMT_BOOL, 1 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddAsciiItem( char* pValue, uint length )
{
	CItemAscii* ptr = new CItemAscii( FMT_ASCII, 1 );
	ptr->SetData( pValue, length );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddAsciiItem( atstring str )
{
	CItemAscii* ptr = new CItemAscii( FMT_ASCII, 1 );
	ptr->SetData( (char*)str.c_str(), str.length() );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddI1Item( short* pValue, uint count )
{
	CItemI1* ptr = new CItemI1( FMT_I1, 1 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddI2Item( short* pValue, uint count )
{
	CItemI2* ptr = new CItemI2( FMT_I2, 2 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddI4Item( int* pValue, uint count )
{
	CItemI4* ptr = new CItemI4( FMT_I4, 4 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddI8Item( long int* pValue, uint count )
{
	CItemI8* ptr = new CItemI8( FMT_I8, 8 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddU1Item( ushort* pValue, uint count )
{
	CItemU1* ptr = new CItemU1( FMT_U1, 1 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddU2Item( ushort* pValue, uint count )
{
	CItemU2* ptr = new CItemU2( FMT_U2, 2 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddU4Item( uint* pValue, uint count )
{
	CItemU4* ptr = new CItemU4( FMT_U4, 4 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddU8Item( unsigned long* pValue, uint count )
{
	CItemU8* ptr = new CItemU8( FMT_U8, 8 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddF4Item( float* pValue, uint count )
{
	CItemF4* ptr = new CItemF4( FMT_F4, 4 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::AddF8Item( double* pValue, uint count )
{
	CItemF8* ptr = new CItemF8( FMT_F8, 8 );
	ptr->SetData( pValue, count );
	m_listItem.push_back( ptr );
	if( !m_stackList.isEmpty() )
	{
		m_stackList.top()->IncreaseListCount();
	}
	return _SUCCESS;
}

int CSecsMsg::SetMsgInfo( short s, short f, short w)
{
	m_nItemPointer = 0;
	if( (s>0) && (f>0) )
	{
		m_nStream = s;
		m_nFunction = f;
		m_nWait = w;
		return _SUCCESS;
	}
	return _ERROR_;

}

int CSecsMsg::GetMsgInfo( short& s, short& f, short& w )
{
	m_nItemPointer = 0;
	if( (m_nStream>0) && (m_nFunction>0) )
	{
		s = m_nStream;
		f = m_nFunction;
		w = m_nWait;
		return _SUCCESS;
	}
	return _ERROR_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GET ITEM  API
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CSecsMsg::GetListItem()
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_LIST )
		 {
			 return _ERROR_;
		 }
		 m_nItemPointer++;
		 return static_cast<CItemList*>(ptr)->GetDataCount();
	}
	return _ERROR_;
}

int CSecsMsg::GetAsciiItem(char* pAscii, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_ASCII )
		 {
			 return _ERROR_;
		 }
		 CItemAscii* item = static_cast<CItemAscii*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData( pAscii, length );
	}
	return _ERROR_;
}

int CSecsMsg::GetAsciiItem(std::string& rstr)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		ptr = m_listItem.at(m_nItemPointer);
		if( ptr->GetFormat() != FMT_ASCII )
		{
			return _ERROR_;
		}
		CItemAscii* item = static_cast<CItemAscii*>(ptr);
		if( item == nullptr )
			return _ERROR_;

		int ascii_length = item->GetDataCount();
		char* pStr = new char[ascii_length+1];
		memset( pStr, 0x0, ascii_length+1 );

		int read_size = item->GetData( pStr, ascii_length );
		if( read_size >= 0 )
		{
			rstr = std::string( pStr );
			m_nItemPointer++;
		}
		delete[] pStr;
		return read_size;
	}
	return _ERROR_;
}

int CSecsMsg::GetBinaryItem(short* pBin, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_BIN )
		 {
			 return _ERROR_;
		 }
		 CItemBin* item = static_cast<CItemBin*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pBin, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetBooleanItem(short* pBool, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_BOOL )
		 {
			 return _ERROR_;
		 }
		 CItemBool* item = static_cast<CItemBool*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pBool, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetI1Item( short* pI1, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_I1 )
		 {
			 pI1 = nullptr;
			 return _ERROR_;
		 }
		 CItemI1* item = static_cast<CItemI1*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pI1, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetI2Item( short* pI2, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_I2 )
		 {
			 pI2 = nullptr;
			 return _ERROR_;
		 }
		 CItemI2* item = static_cast<CItemI2*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pI2, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetI4Item( int* pI4, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_I4 )
		 {
			 return _ERROR_;
		 }
		 CItemI4* item = static_cast<CItemI4*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pI4, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetI8Item( long* pI8, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_I8 )
		 {
			 return _ERROR_;
		 }
		 CItemI8* item = static_cast<CItemI8*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pI8, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetU1Item( ushort* pU1, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_U1 )
		 {
			 return _ERROR_;
		 }
		 CItemU1* item = static_cast<CItemU1*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pU1, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetU2Item( ushort* pU2, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_U2 )
		 {
			 return _ERROR_;
		 }
		 CItemU2* item = static_cast<CItemU2*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pU2, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetU4Item( uint* pU4, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_U4 )
		 {
			 return _ERROR_;
		 }
		 CItemU4* item = static_cast<CItemU4*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pU4, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetU8Item( unsigned long* pU8, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_U8 )
		 {
			 return _ERROR_;
		 }
		 CItemU8* item = static_cast<CItemU8*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pU8, length);
	}
	return _ERROR_;
}


int CSecsMsg::GetF4Item( float* pF4, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_F4 )
		 {
			 return _ERROR_;
		 }
		 CItemF4* item = static_cast<CItemF4*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pF4, length);
	}
	return _ERROR_;
}

int CSecsMsg::GetF8Item( double* pF8, uint length)
{
	CItemBase* ptr = nullptr;
	if( m_listItem.size() > m_nItemPointer )
	{
		 ptr = m_listItem.at(m_nItemPointer);
		 if( ptr->GetFormat() != FMT_F8 )
		 {
			 return _ERROR_;
		 }
		 CItemF8* item = static_cast<CItemF8*>(ptr);
		 if( item == nullptr )
			 return _ERROR_;
		 m_nItemPointer++;
		 return item->GetData(pF8, length);
	}
	return _ERROR_;
}

