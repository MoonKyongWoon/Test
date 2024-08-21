#ifndef CSECSGEM_HPP
#define CSECSGEM_HPP

#include <QObject>
#include "secsmsg.hpp"
#include "scanner.hpp"
#include "atstring.hpp"
#include "sysini.hpp"
#include "syslog.hpp"
#include "sysver.hpp"
#include "util.hpp"

enum _VAR_ID
{
	_VAR_CEID,
	_VAR_SVID,
	_VAR_MAX ,
};

struct stCEID_info {
	_var_item* ceid_info;
	bool enable;
};

class CSecsGem : public QObject
{
	Q_OBJECT
public:
	CSecsGem( QObject* parent = nullptr );
	~CSecsGem();
private:
	int LoadLocalCEID( QString );
	int LoadLocalSVID( QString );
private:
	int initCEIDmap( QString );
public:
	bool setCEIDmap(bool enable, uint ceid = 0, bool clear_flag = false);
	bool setRPTIDmap(uint ceid, QVector<uint>& rptid_list, bool clear_flag = false);
	bool setVIDLISTmap(uint rptid, QVector<uint>& vid_list, bool clear_flag = false);
	bool hasCEIDinRPTIDmap(uint ceid);
	bool hasRPTIDinVIDLISTmap(uint rptid);
	bool getRPTIDmapLength(uint ceid, uint& value);
	bool getVIDLISTmapLength(uint rptid, uint& value);
	bool getRptidValue(uint ceid, uint index, uint& value);
	bool getVidValue(uint rptid, uint index, uint& value);
	HSMS_FMT_CODE getVidType(uint vid);
public:
	bool Init();
	void SetFormatCheck(bool);
	bool GetFormatCheck();
	void SetDeviceId( ushort );
	ushort GetDeviceId();
	void SetSystemByte( CSecsMsg*, uint );
public:
	CSecsMsg* CreateMsg(char*, uint);
	CSecsMsg* CreateMsg( short s, short f, short w, short d=0 );
	CSecsMsg* CreateReplyMsg( CSecsMsg* );
	int FinishMsg( CSecsMsg* );	//Generate hsms msg
	int CloseMsg( CSecsMsg* );
	int GetMsgInfo( CSecsMsg*, short& stream, short& func, short& wait );
	int GetMsgSML( CSecsMsg*, atstring& );
	int GetMsgHSMS( CSecsMsg*, QByteArray& );
	uint GetSystemByte(CSecsMsg*, uint&);
	int GetDeviceId(CSecsMsg*, int&);
	int GetRBit(CSecsMsg*, int&);
public:
	int OpenListItem( CSecsMsg* );
	int AddListItem( CSecsMsg*, int );
	int CloseListItem( CSecsMsg* );
	int AddBinaryItem( CSecsMsg*, short* pValue, uint count );
	int AddBooleanItem( CSecsMsg*, short* pValue, uint count );
	int AddAsciiItem( CSecsMsg*, char* pValue, uint count );
	int AddAsciiItem( CSecsMsg*, atstring );
	int AddI1Item( CSecsMsg*, short* pValue, uint count );
	int AddI2Item( CSecsMsg*, short* pValue, uint count );
	int AddI4Item( CSecsMsg*, int* pValue, uint count );
	int AddI8Item( CSecsMsg*, long int* pValue, uint count );
	int AddUI1Item( CSecsMsg*, ushort* pValue, uint count );
	int AddUI2Item( CSecsMsg*, ushort* pValue, uint count );
	int AddUI4Item( CSecsMsg*, uint* pValue, uint count );
	int AddUI8Item( CSecsMsg*, unsigned long int* pValue, uint count );
	int AddF4Item( CSecsMsg*, float* pValue, uint count );
	int AddF8Item( CSecsMsg*, double* pValue, uint count );
	//RFU
	//int AddCEID( int ceid, const char* name, const char* desc );
	//int AddSVID( int svid, const char* name, const char* desc );
	//int AddRCMD( const char* rcmd );
	//int SetSVIDValue( int svid, const char* );
public:
	int GetListItemOpen( CSecsMsg* );
	int GetListItemClose( CSecsMsg* );
	int GetAsciiItem( CSecsMsg* p, char* s, uint l);
	int GetAsciiItem( CSecsMsg* p, std::string& );
	int GetBinaryItem( CSecsMsg* p, short* s, uint l=1 );
	int GetBooleanItem( CSecsMsg* p, short* s, uint l=1 );
	int GetI1Item ( CSecsMsg* p, short* s, uint l=1);
	int GetI2Item ( CSecsMsg* p, short* s, uint l=1 );
	int GetI4Item ( CSecsMsg* p, int* s, uint l=1 );
	int GetI8Item ( CSecsMsg* p, long int* s, uint l=1 );
	int GetUI1Item( CSecsMsg* p, ushort* s, uint l=1 );
	int GetUI2Item( CSecsMsg* p, ushort* s, uint l=1 );
	int GetUI4Item( CSecsMsg* p, uint* s, uint l=1 );
	int GetUI8Item( CSecsMsg* p, unsigned long int* s, uint l=1 );
public:
	CSecsMsg* CompileSML( atstring sml, uint systembytes = 0, uint sessionid = 0 );
	std::vector<atstring> getCompileErrors();
	void addCompileError( int line, int col, atstring str );
	void createHsmsSyntax();
private:
	bool   m_bFormatCheck;
	ushort m_sDeviceId;
	int    m_nSystemBytes;
	QMap< uint, stCEID_info >	m_CEID_map;
	QMap< uint, QVector<uint> >	m_RPTID_map;
	QMap< uint, QVector<uint> >	m_VID_LIST_map;
private:
	QVector<var_item*> m_vecLocalVarId[_VAR_MAX];
private:
	CScanner m_scanner;
	Str2WordMap		m_mapSyntax;
	std::vector<atstring> m_vecError;
};

#endif // CSECSGEM_HPP
