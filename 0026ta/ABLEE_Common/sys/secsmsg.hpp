#ifndef CSECSMSG_HPP
#define CSECSMSG_HPP

#include "atstring.hpp"
#include "commstruct.hpp"
#include "hsms.hpp"
#include "scanner.hpp"
#include <QByteArray>
#include <QStack>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///START SML COMPILE

#define SYNTAX(a,b,c)								( (a<<24) | (b<<16) | c )
#define HSMS_SYNTAX(syntax, command, opt, bit)		m_mapSyntax[syntax]  = SYNTAX(command, opt, bit);

union HSMSCMD
{
	unsigned int		value;
	struct
	{
		unsigned int	bit     : 16;
		unsigned int	opt     : 8;
		unsigned int	command : 8;
	};
};

typedef map< atstring, unsigned int >	Str2WordMap;

enum HSMS_SYNTAX
{
	HSMS_SF         ,
	HSMS_LT         ,	//LESS THAN
	HSMS_GT         ,	//GREATER THAN
	HSMS_WBIT       ,	//GREATER THAN
	HSMS_LBRACKET   ,
	HSMS_RBRACKET   ,
	HSMS_NUM        ,
	HSMS_LIST       ,
	HSMS_BIN        ,
	HSMS_BOOLEAN    ,
	HSMS_ASCII      ,
	HSMS_J          ,	//JIS-8
	HSMS_I8         ,	//8BYTE
	HSMS_I1         ,	//1BYTE SIGNED INT
	HSMS_I2         ,	//2BYTE SIGNED INT
	HSMS_I4         ,	//4BYTE SIGNED INT
	HSMS_F8         ,	//8BYTE FLOATING POINT
	HSMS_F4         ,	//4BYTE FLOATING POINT
	HSMS_U8         ,	//8BYTE UNSIGNED INT
	HSMS_U1         ,	//1BYTE UNSIGNED INT
	HSMS_U2         ,	//1BYTE UNSIGNED INT
	HSMS_U4         ,	//1BYTE UNSIGNED INT
};

typedef struct _stSmlType
{
	HSMS_FMT_CODE type;
	int type_size;
	int count;
	atstring svalue;
}stSmlType;

class CSmlObject
{
public:
	CSmlObject();
	~CSmlObject();
public:
	void Clear();
	stSmlType* GetType(uint);
public:
	void SetStreamFunction( int s, int f, bool ps);
	void SetPrimaryFlag(bool);
	void SetSessionId( uint );
	void SetSystemBytes( uint );
	bool IsPrime();
	uint GetSessionId();
	uint GetSystemBytes();
	int GetStream();
	int GetFunction();
	void AddType( stSmlType* );
	uint TypeSize();
private:
	int m_nStream;
	int m_nFunction;
	int m_nSessionId;
	int m_nSystemBytes;
	bool m_bPrimary;
	std::vector<stSmlType*> m_vecSmlObj;
};

///END SML COMPILE
////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum _MSG_ERR_CODE
{
	_ERROR_            = -1,
	_FILE_NOT_FOUND    = -100,
	_FILE_IO_ERROR     = -101,
	_NO_ITEM           = -200,
	_TYPE_MISMATCH     = -201,
	_SIZE_MISMATCH     = -202,
	_INVALID_MSGID     = -203,
	_INVALID_STREAM    = -204,
	_INVALID_FUNCTION  = -205,
	_DUPLICATE_SYSBYTE = -206,
	_SUCCESS           = 0,
};

typedef struct _var_item
{
	atstring name;
	short id;
	atstring format;
}var_item;

class CItemBase
{
public:
	CItemBase( HSMS_FMT_CODE fmt,int size );
public:
	void IncreaseDataCount();
	void DecreaseDataCount();
	void SetDataCount(int);
	int GetDataCount();
	int GetFormatSize();
	HSMS_FMT_CODE GetFormat();
private:
	HSMS_FMT_CODE fmt_code;
	int fmt_size;
	int data_count;
};

class CItemList : public CItemBase
{
public:
	CItemList( HSMS_FMT_CODE fmt,int size );
public:
	void SetListCount(uint);
	void IncreaseListCount();
};

class CItemBin : public CItemBase
{
public:
	CItemBin( HSMS_FMT_CODE fmt,int size );
	~CItemBin();
public:
	int SetData( short* ptr, uint count );
	int GetData( short*, uint count );
private:
	short* pData;
};

class CItemBool : public CItemBase
{
public:
	CItemBool( HSMS_FMT_CODE fmt,int size );
	~CItemBool();
public:
	int SetData( short* ptr, uint count );
	int GetData( short*, uint count );
private:
	short* pData;
};

class CItemAscii : public CItemBase
{
public:
	CItemAscii( HSMS_FMT_CODE fmt,int size );
	~CItemAscii();
public:
	int SetData( char* ptr, uint length );
	int GetData( char*, uint length );
private:
	char* pData;
};

class CItemI1 : public CItemBase
{
public:
	CItemI1( HSMS_FMT_CODE fmt,int size );
	~CItemI1();
public:
	int SetData( short*, uint length );
	int GetData( short*, uint length );
private:
	short* pData;
};

class CItemI2 : public CItemBase
{
public:
	CItemI2( HSMS_FMT_CODE fmt,int size );
	~CItemI2();
public:
	int SetData( short*, uint length );
	int GetData( short*, uint length );
private:
	short* pData;
};

class CItemI4 : public CItemBase
{
public:
	CItemI4( HSMS_FMT_CODE fmt,int size );
	~CItemI4();
public:
	int SetData( int*, uint length );
	int GetData( int*, uint length );
private:
	int* pData;
};

class CItemI8 : public CItemBase
{
public:
	CItemI8( HSMS_FMT_CODE fmt,int size );
	~CItemI8();
public:
	int SetData( long int*, uint length );
	int GetData( long int*, uint length );
private:
	long int* pData;
};

class CItemU1 : public CItemBase
{
public:
	CItemU1( HSMS_FMT_CODE fmt,int size );
	~CItemU1();
public:
	int SetData( ushort*, uint length );
	int GetData( ushort*, uint length );
private:
	ushort* pData;
};

class CItemU2 : public CItemBase
{
public:
	CItemU2( HSMS_FMT_CODE fmt,int size );
	~CItemU2();
public:
	int SetData( ushort*, uint length );
	int GetData( ushort*, uint length );
private:
	ushort* pData;
};

class CItemU4 : public CItemBase
{
public:
	CItemU4( HSMS_FMT_CODE fmt,int size );
	~CItemU4();
public:
	int SetData( uint*, uint length );
	int GetData( uint*, uint length );
private:
	uint* pData;
};

class CItemU8 : public CItemBase
{
public:
	CItemU8( HSMS_FMT_CODE fmt,int size );
	~CItemU8();
public:
	int SetData( unsigned long *, uint length );
	int GetData( unsigned long *, uint length );
private:
	unsigned long * pData;
};

class CItemF4 : public CItemBase
{
public:
	CItemF4( HSMS_FMT_CODE fmt,int size );
	~CItemF4();
public:
	int SetData( float*, uint length );
	int GetData( float*, uint length );
private:
	float* pData;
};

class CItemF8 : public CItemBase
{
public:
	CItemF8( HSMS_FMT_CODE fmt,int size );
	~CItemF8();
public:
	int SetData( double*, uint length );
	int GetData( double*, uint length );
private:
	double* pData;
};


class CSecsMsg
{
public:
	CSecsMsg();
	CSecsMsg( short s, short f, short w, short di = 0 );
	~CSecsMsg();
	void Init();
public:
	short GetStream();
	short GetFunction();
	short GetWaitBit();
	uint GetSystemBytes();
	int GetDeviceId();
	int GetRBit();

	char* GetRcmd();
	int SetMsgInfo( short, short, short );
	int GetMsgInfo( short&, short&, short& );
	void SetSystemBytes(int);
	void SetDeviceId(uint);
public:
	int HsmsToMsg(char*,uint);
	int MsgToHsms(QByteArray&);
	int MsgToSML(atstring&);
	atstring getTypeCode(HSMS_FMT_CODE code);
private:
	int MakeHsmsBody();
	int MakeHsmsHeader();
public:
	int OpenListItem();
	int AddListItem(int);
	int CloseListItem();
	int AddBinaryItem( short* pValue, uint count );
	int AddBooleanItem( short* pValue, uint count );
	int AddAsciiItem( char* pValue, uint count );
	int AddAsciiItem( atstring );
	int AddI1Item( short* pValue, uint count );
	int AddI2Item( short* pValue, uint count );
	int AddI4Item( int* pValue, uint count );
	int AddI8Item( long int* pValue, uint count );
	int AddU1Item( ushort* pValue, uint count );
	int AddU2Item( ushort* pValue, uint count );
	int AddU4Item( uint* pValue, uint count );
	int AddU8Item( unsigned long int* pValue, uint count );
	int AddF4Item( float* pValue, uint count );
	int AddF8Item( double* pValue, uint count );
public:
	int GetListItem();
	int GetAsciiItem(char*, uint);
	int GetAsciiItem(std::string&);
	int GetBinaryItem(short*, uint);
	int GetBooleanItem(short*, uint);
	int GetI1Item( short*, uint );
	int GetI2Item( short*, uint );
	int GetI4Item( int*, uint );
	int GetI8Item( long*, uint );
	int GetU1Item( ushort*, uint );
	int GetU2Item( ushort*, uint );
	int GetU4Item( uint*, uint );
	int GetU8Item( unsigned long*, uint );
	int GetF4Item( float* pValue, uint );
	int GetF8Item( double* pValue, uint );

private:
	short m_nStream;
	short m_nFunction;
	short m_nWait;
	uint m_nSystemBytes;
	int m_nItemPointer;
	int m_nDeviceId;
	int m_nRBit;
private:
	stHsmsElement m_stHsmsElement;
	QVector<CItemBase*> m_listItem;
	QStack<CItemList*> m_stackList;
private:
	QByteArray m_baMsgBody;
	QByteArray m_baMsgHeader;
};

#endif // CSECSMSG_HPP
