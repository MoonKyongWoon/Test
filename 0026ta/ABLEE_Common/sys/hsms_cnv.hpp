#ifndef CHSMS_CNV_HPP
#define CHSMS_CNV_HPP

#include "commstruct.hpp"
#include "scanner.hpp"
#include "syslog.hpp"
#include "protocol.hpp"
#include "util.hpp"
#include "hsms.hpp"

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

#define SESSION_ID			(0)		//RFU
class CHsmsCnv
{
public:
	CHsmsCnv();
	~CHsmsCnv();
public:
	void SetSessionId( uint );
	void SetSystemBytes( uint );
	bool SmlCompile( atstring );
	std::vector<atstring> getErrors();
	bool GetHsmsPacket( QByteArray& );
	bool MakeHsmsMsgHeader();
	bool MakeMsmsMsg();
	bool HsmsToSml(char*,uint, atstring&);
private:	
	void createSyntax();
	void addError(int, int, atstring);
	bool MakeTypeToHsms(stSmlType*, QByteArray& );
	QString getTypeCode(HSMS_FMT_CODE code);
private:
	CScanner m_scanner;
	Str2WordMap		m_mapSyntax;
	std::vector<atstring> m_vecError;
	CSmlObject m_SmlObject;
	QByteArray m_arrHsms;
	stHsmsHeader m_hsmsHeader;
	QByteArray m_arrMsg;
	QByteArray m_arrHeader;
};


#endif
