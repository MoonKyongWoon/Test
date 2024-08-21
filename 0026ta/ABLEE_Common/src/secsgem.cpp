#include "../sys/secsgem.hpp"

CSecsGem::CSecsGem( QObject* parent) : QObject(parent)
{
	m_sDeviceId = 0;
	m_nSystemBytes = 0;
	m_bFormatCheck = false;
	Init();
}

CSecsGem::~CSecsGem()
{
	for( int v=0;v<_VAR_MAX;v++ )
	{
		for( var_item* p : m_vecLocalVarId[v] )
		{
			delete p;
		}
		m_vecLocalVarId[v].clear();
	}
	for (auto info : this->m_CEID_map)
	{
		delete info.ceid_info;
	}
	this->m_CEID_map.clear();
}

bool CSecsGem::Init()
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(TMS_INI);

	LoadLocalCEID( ini );
	LoadLocalSVID( ini );
	initCEIDmap( ini );

	createHsmsSyntax();
	return true;
}

int CSecsGem::initCEIDmap(QString ini)
{
	int count = 0;

	QString section_key = tr("%1/%2").arg(SECTION_CEIDLIST).arg(KEY_VAR_COUNT);
	if( !UTIL::INI::readSettings( ini, section_key, count ) )
		return _FILE_IO_ERROR;

	for( stCEID_info info : this->m_CEID_map )
		delete info.ceid_info;
	this->m_CEID_map.clear();

	for (int i = 0; i < count; i++)
	{
		QString sValue = "";
		section_key = QObject::tr("%1/L%2").arg(SECTION_CEID_LIST).arg(i + 1);
		UTIL::INI::readSettings( ini, section_key, sValue );
		QStringList lval = sValue.split("|");
		if (lval.count() == 2)
		{
			stCEID_info info;
			info.ceid_info = new _var_item;
			info.ceid_info->name = lval.at(0).toStdString();
			info.ceid_info->id = lval.at(1).toUInt();
			info.ceid_info->format = "";
			info.enable = false;

			this->m_CEID_map.insert( info.ceid_info->id, info );
			this->m_RPTID_map.insert( info.ceid_info->id, QVector<uint>() );
		}
	}
	return 0;
}

bool	CSecsGem::setVIDLISTmap(uint rptid, QVector<uint>& vid_list, bool clear_flag)
{
	if (clear_flag)
	{
		for (QMap<uint, QVector<uint> >::iterator iter = this->m_VID_LIST_map.begin(); iter != this->m_VID_LIST_map.end(); iter++)
		{
			iter.value().clear();
		}
		this->m_VID_LIST_map.clear();
		return true;
	}
	auto iter = this->m_VID_LIST_map.find(rptid);
	if (iter == this->m_VID_LIST_map.end())
		this->m_VID_LIST_map.insert(rptid, vid_list);
	else
		iter.value() = vid_list;
	return true;
}

bool	CSecsGem::setRPTIDmap(uint ceid, QVector<uint>& rptid_list, bool clear_flag)
{
	if (clear_flag)
	{
		for (QMap<uint, QVector<uint> >::iterator iter = this->m_RPTID_map.begin(); iter != this->m_RPTID_map.end(); iter++)
		{
			iter.value().clear();
		}
		this->m_RPTID_map.clear();
		return true;
	}
	auto iter = this->m_RPTID_map.find(ceid);
	if (iter == this->m_RPTID_map.end())
		this->m_RPTID_map.insert(ceid, rptid_list);
	else
		iter.value() = rptid_list;
	return true;
}

bool	CSecsGem::setCEIDmap(bool enable, uint ceid, bool clear_flag)
{
	if(clear_flag)
	{

		for (QMap< uint, stCEID_info >::iterator iter = this->m_CEID_map.begin(); iter != this->m_CEID_map.end(); iter++ )
		{
			iter.value().enable = false;
		}
		return true;
	}
	auto iter = this->m_CEID_map.find(ceid);
	if (iter == this->m_CEID_map.end())
		return false;
	iter.value().enable = enable;
	return true;
}

bool CSecsGem::hasCEIDinRPTIDmap(uint ceid)
{
	return this->m_RPTID_map.find(ceid) != this->m_RPTID_map.end();
}

bool CSecsGem::hasRPTIDinVIDLISTmap(uint rptid)
{
	return this->m_VID_LIST_map.find(rptid) != this->m_VID_LIST_map.end();
}

bool CSecsGem::getRPTIDmapLength(uint ceid, uint& value)
{
	if (!hasCEIDinRPTIDmap(ceid))
		return false;
	value = this->m_RPTID_map.find(ceid).value().length();
	return true;
}

bool CSecsGem::getVIDLISTmapLength(uint rptid, uint& value)
{
	if (!hasRPTIDinVIDLISTmap(rptid))
		return false;
	value = this->m_VID_LIST_map.find(rptid).value().length();
	return true;
}

bool CSecsGem::getRptidValue(uint ceid, uint index, uint& value)
{
	if (!hasCEIDinRPTIDmap(ceid))
		return false;
	uint len = 0;
	if (!getRPTIDmapLength(ceid, len))
		return false;
	value = this->m_RPTID_map.find(ceid).value().at(index);
	return true;
}

bool CSecsGem::getVidValue(uint rptid, uint index, uint& value)
{
	if (!hasRPTIDinVIDLISTmap(rptid))
		return false;
	uint len = 0;
	if (!getVIDLISTmapLength(rptid, len))
		return false;
	value = this->m_VID_LIST_map.find(rptid).value().at(index);
	return true;
}

HSMS_FMT_CODE CSecsGem::getVidType(uint vid)
{
	short ref_vid(vid);
	QVector<var_item*>::iterator iter = this->m_vecLocalVarId[_VAR_SVID].begin();
	for (; iter != this->m_vecLocalVarId[_VAR_SVID].end(); iter++)
	{
		if ( (*iter)->id == ref_vid )
		{
			QString format((*iter)->format);
			if (format == "A")
				return HSMS_FMT_CODE::FMT_ASCII;
			if (format == "U2")
				return HSMS_FMT_CODE::FMT_U2;
		}
	}
	return HSMS_FMT_CODE::FMT_UNKNOWN;
}


//Local CEID ini read
int CSecsGem::LoadLocalCEID( QString ini )
{
	int count = 0;
	QString section_key = tr("%1/%2").arg(SECTION_CEIDLIST).arg(KEY_VAR_COUNT);
	if( !UTIL::INI::readSettings( ini, section_key, count ) )
		return _FILE_IO_ERROR;

	for( var_item* p : m_vecLocalVarId[_VAR_CEID] )
		delete p;

	m_vecLocalVarId[_VAR_CEID].clear();
	for( int i=0;i<count;i++ )
	{
		QString val = "";
		QString key = tr("L%1").arg(i+1);
		section_key = tr("%1/%2").arg(SECTION_CEIDLIST).arg(key);
		if( !UTIL::INI::readSettings( ini, section_key, val ) ) continue;
		QStringList lval = val.split("|");
		if( lval.count() == 2 )
		{
			var_item* varid = new var_item;
			varid->name = lval.at(0).toStdString();
			varid->id = lval.at(1).toInt();
			varid->format = "";
			m_vecLocalVarId[_VAR_CEID].push_back( varid );
		}
	}
	return _SUCCESS;
}

//Local SVID ini read
int CSecsGem::LoadLocalSVID( QString ini )
{
	int count = 0;
	QString section_key = tr("%1/%2").arg(SECTION_VIDLIST).arg(KEY_VAR_COUNT);
	if( !UTIL::INI::readSettings( ini, section_key, count ) )
		return _FILE_IO_ERROR;

	for( var_item* p : m_vecLocalVarId[_VAR_SVID] )
		delete p;
	m_vecLocalVarId[_VAR_SVID].clear();
	for( int i=0;i<count;i++ )
	{
		QString val = "";
		QString key = tr("L%1").arg(i+1);
		section_key = tr("%1/%2").arg(SECTION_VIDLIST).arg(key);
		if( !UTIL::INI::readSettings( ini, section_key, val ) ) continue;
		QStringList lval = val.split("|");
		if( lval.count() == 3 )
		{
			var_item* varid = new var_item;
			varid->name = lval.at(0).toStdString();
			varid->id = lval.at(1).toInt();
			varid->format = lval.at(2).toStdString();
			m_vecLocalVarId[_VAR_SVID].push_back( varid );
		}
	}
	return _SUCCESS;
}

void CSecsGem::SetFormatCheck(bool enable)
{
	m_bFormatCheck = enable;
}

bool CSecsGem::GetFormatCheck()
{
	return m_bFormatCheck;
}

void CSecsGem::SetDeviceId( ushort id )
{
	m_sDeviceId = id;
}

ushort CSecsGem::GetDeviceId()
{
	return m_sDeviceId;
}

void CSecsGem::SetSystemByte( CSecsMsg* pMsg, uint system_byte )
{
	pMsg->SetSystemBytes(system_byte);
}

//HSMS to MSG
CSecsMsg* CSecsGem::CreateMsg(char* pHsms, uint uLen )
{
	CSecsMsg* msg = new	CSecsMsg();
	if( msg->HsmsToMsg( pHsms, uLen) < 0 )
		return nullptr;
	return msg;
}

CSecsMsg* CSecsGem::CreateMsg( short s, short f, short w, short d )
{
	CSecsMsg* pMsg = new CSecsMsg( s, f, w, d );
	// TODO
	// ADD SYSTEM BYTE
	// secegem has systembyte -> transfer to secsmsg
	return pMsg;
}

CSecsMsg* CSecsGem::CreateReplyMsg( CSecsMsg* pMsg )
{
	if( pMsg == nullptr ) return nullptr;

	short s = 0, f = 0, w = 0;
	if( pMsg->GetMsgInfo( s, f, w ) < 0 )
		return nullptr;
	int b = pMsg->GetSystemBytes();

//	CSecsMsg* pRepMsg = new CSecsMsg( s, f+1, w );
	CSecsMsg* pRepMsg = new CSecsMsg( s, f+1, 0 );
	pRepMsg->SetSystemBytes( b );
	return pRepMsg;
}

int CSecsGem::FinishMsg( CSecsMsg* pMsg )
{
	if( pMsg != nullptr )
	{
		//MsgToHsms
	}
	return _INVALID_MSGID;
}

int CSecsGem::CloseMsg( CSecsMsg* pMsg )
{
	if( pMsg != nullptr )
	{
		delete pMsg;
		pMsg = nullptr;
		return _SUCCESS;
	}
	return _INVALID_MSGID;
}

int CSecsGem::GetMsgInfo( CSecsMsg* pMsg, short& stream, short& func, short& wait )
{
	if( pMsg == nullptr )
	{
		return _INVALID_MSGID;
	}
	return pMsg->GetMsgInfo( stream, func, wait );
}

int CSecsGem::GetMsgSML( CSecsMsg* pMsg, atstring& rstr )
{
	if( pMsg == nullptr )
	{
		return _INVALID_MSGID;
	}
	return pMsg->MsgToSML( rstr );
}

int CSecsGem::GetMsgHSMS( CSecsMsg* pMsg, QByteArray& baHsms )
{
	if( pMsg == nullptr )
	{
		return _INVALID_MSGID;
	}
	return pMsg->MsgToHsms( baHsms );
}

uint CSecsGem::GetSystemByte(CSecsMsg* pMsg, uint& rSysByte)
{
	if( pMsg == nullptr )
	{
		rSysByte = 0;
		return _INVALID_MSGID;
	}
	rSysByte = pMsg->GetSystemBytes();
	return _SUCCESS;
}

int CSecsGem::GetDeviceId(CSecsMsg* pMsg, int& id)
{
	if( pMsg == nullptr )
	{
		id = 0;
		return _INVALID_MSGID;
	}
	id = pMsg->GetDeviceId();
	return _SUCCESS;
}

int CSecsGem::GetRBit(CSecsMsg* pMsg, int& rbit)
{
	if( pMsg == nullptr )
	{
		rbit = 0;
		return _INVALID_MSGID;
	}
	rbit = pMsg->GetRBit();
	return _SUCCESS;
}

int CSecsGem::OpenListItem( CSecsMsg* pMsg )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->OpenListItem();
}

int CSecsGem::AddListItem( CSecsMsg* pMsg, int count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddListItem( count );
}

int CSecsGem::CloseListItem( CSecsMsg* pMsg )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->CloseListItem();
}

int CSecsGem::AddBinaryItem( CSecsMsg* pMsg, short* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddBinaryItem( pValue, count );
}

int CSecsGem::AddBooleanItem( CSecsMsg* pMsg, short* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddBooleanItem( pValue, count );
}

int CSecsGem::AddAsciiItem( CSecsMsg* pMsg, char* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddAsciiItem( pValue, count );
}

int CSecsGem::AddAsciiItem( CSecsMsg* pMsg, atstring str)
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddAsciiItem( (char*)str.c_str(), str.length() );
}

int CSecsGem::AddI1Item( CSecsMsg* pMsg, short* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddI1Item( pValue, count );
}

int CSecsGem::AddI2Item( CSecsMsg* pMsg, short* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddI2Item( pValue, count );
}

int CSecsGem::AddI4Item( CSecsMsg* pMsg, int* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddI4Item( pValue, count );
}

int CSecsGem::AddI8Item( CSecsMsg* pMsg, long int* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddI8Item( pValue, count );
}

int CSecsGem::AddUI1Item( CSecsMsg* pMsg, ushort* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddU1Item( pValue, count );
}

int CSecsGem::AddUI2Item( CSecsMsg* pMsg, ushort* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddU2Item( pValue, count );
}
int CSecsGem::AddUI4Item( CSecsMsg* pMsg, uint* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddU4Item( pValue, count );
}
int CSecsGem::AddUI8Item( CSecsMsg* pMsg, unsigned long int* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddU8Item( pValue, count );
}
int CSecsGem::AddF4Item( CSecsMsg* pMsg, float* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddF4Item( pValue, count );
}
int CSecsGem::AddF8Item( CSecsMsg* pMsg, double* pValue, uint count )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->AddF8Item( pValue, count );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GET METHOD
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSecsGem::GetListItemOpen( CSecsMsg* pMsg)
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetListItem();
}

int CSecsGem::GetListItemClose( CSecsMsg* pMsg )
{
	return _SUCCESS;
}

int CSecsGem::GetAsciiItem( CSecsMsg* pMsg, char* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetAsciiItem( pVal, length );
}

int CSecsGem::GetAsciiItem( CSecsMsg* pMsg, std::string& str )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetAsciiItem( str );
}

int CSecsGem::GetBinaryItem( CSecsMsg* pMsg, short* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetBinaryItem( pVal, length );
}

int CSecsGem::GetBooleanItem( CSecsMsg* pMsg, short* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetBooleanItem( pVal, length );
}

int CSecsGem::GetI1Item( CSecsMsg* pMsg, short* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetI1Item( pVal, length );
}


int CSecsGem::GetI2Item( CSecsMsg* pMsg, short* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetI2Item( pVal, length );
}

int CSecsGem::GetI4Item( CSecsMsg* pMsg, int* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetI4Item( pVal, length );
}

int CSecsGem::GetI8Item( CSecsMsg* pMsg, long int* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetI8Item( pVal, length );
}

int CSecsGem::GetUI1Item( CSecsMsg* pMsg, ushort* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetU1Item( pVal, length );
}

int CSecsGem::GetUI2Item( CSecsMsg* pMsg, ushort* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetU2Item( pVal, length );
}

int CSecsGem::GetUI4Item( CSecsMsg* pMsg, uint* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetU4Item( pVal, length );
}

int CSecsGem::GetUI8Item( CSecsMsg* pMsg, unsigned long int* pVal, uint length )
{
	if( pMsg == nullptr ) return _INVALID_MSGID;
	return pMsg->GetU8Item( pVal, length );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///SML COMPILE START
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<atstring> CSecsGem::getCompileErrors()
{
	return m_vecError;
}

void CSecsGem::addCompileError( int line, int col, atstring str )
{
	atstring msg = "";
	msg.Format( "%d(%d) : %s", line, col, str.c_str() );
	m_vecError.push_back( msg );
}

void CSecsGem::createHsmsSyntax()
{
	m_mapSyntax.clear();

	//STREAM/FUNCTION SYNTAX
	for( int stream=0;stream<64;stream++ )
	{
		for( int function=0;function<64;function++ )
		{
			char szSf[16]={0x0,};
			memset( szSf, 0x0, sizeof(szSf) );
			sprintf( szSf, "S%dF%d", stream, function );
			atstring sf = szSf;
			HSMS_SYNTAX( sf.c_str(), HSMS_SF, stream, function );
		}
	}

	HSMS_SYNTAX( "<"   , HSMS_LT      , 0        , 0 );
	HSMS_SYNTAX( ">"   , HSMS_GT      , 0        , 0 );
	HSMS_SYNTAX( "W"   , HSMS_WBIT    , 0        , 0 );
	HSMS_SYNTAX( "L"   , HSMS_LIST    , FMT_LIST , 1 );
	HSMS_SYNTAX( "B"   , HSMS_BIN     , FMT_BIN  , 1 );
	HSMS_SYNTAX( "BOOL", HSMS_BOOLEAN , FMT_BOOL , 1 );
	HSMS_SYNTAX( "A"   , HSMS_ASCII   , FMT_ASCII, 1 );
	HSMS_SYNTAX( "J"   , HSMS_J       , FMT_JIS_8, 1 );
	HSMS_SYNTAX( "I8"  , HSMS_I8      , FMT_I8   , 8 );
	HSMS_SYNTAX( "I1"  , HSMS_I1      , FMT_I1   , 1 );
	HSMS_SYNTAX( "I2"  , HSMS_I2      , FMT_I2   , 2 );
	HSMS_SYNTAX( "I4"  , HSMS_I4      , FMT_I4   , 4 );
	HSMS_SYNTAX( "F8"  , HSMS_F8      , FMT_F8   , 8 );
	HSMS_SYNTAX( "F4"  , HSMS_F4      , FMT_F4   , 4 );
	HSMS_SYNTAX( "U8"  , HSMS_U8      , FMT_U8   , 8 );
	HSMS_SYNTAX( "U1"  , HSMS_U1      , FMT_U1   , 1 );
	HSMS_SYNTAX( "U2"  , HSMS_U2      , FMT_U2   , 2 );
	HSMS_SYNTAX( "U4"  , HSMS_U4      , FMT_U4   , 4 );
}

CSecsMsg* CSecsGem::CompileSML( atstring sml, uint systembyte, uint sessionid  )
{
	if( !m_scanner.loadsml( sml.c_str() ) )
	{
		printf( "%s\n", sml.c_str() );
		return nullptr;
	}
	CSecsMsg* pSecsMsg = nullptr;

	int nAngleBracketInc = 0;
	bool bStartBracket = false;
	bool bStreamFunc = false;
	token_t token;
	atstring word = "";
	m_vecError.clear();
	while( 1 )
	{
		token = m_scanner.get();
		word  = token.s;
		//printf( "word:%s[%d]\n", word.c_str(), token.t );
		if (token.t == TOK_EOL)
		{
			bStreamFunc = false;
			//bTypeDone = false;
			continue;
		}
		else if (token.t == TOK_EOF)
		{
			 break;
		}

		HSMSCMD cmd = { 0 };
		Str2WordMap::iterator it = m_mapSyntax.find(word);
		if (it != m_mapSyntax.end())
		{
			cmd.value = it->second;
			switch ( cmd.command )
			{
				case HSMS_SF:
				{
					uint stream   = cmd.opt;
					uint function = cmd.bit;
					uint wbit = 0;
					if( !bStreamFunc )
					{
						bStreamFunc = true;
						token = m_scanner.get();
						if( token.t == TOK_ID )
						{
								 if( (token.s == "W") ||(token.s == "P") ) wbit = 1;
							else if( token.s == "S" ) wbit = 0;
							else
							{
								addCompileError( token.n, token.col, token.s );
								return nullptr;
							}
							pSecsMsg = CreateMsg( stream, function, wbit, sessionid );
							pSecsMsg->SetSystemBytes( systembyte );
						}
						else
						{
							pSecsMsg = CreateMsg( stream, function, wbit, sessionid );
							pSecsMsg->SetSystemBytes( systembyte );
						}
					}
					else
					{
						addCompileError( token.n, token.col, "stream/function duplicate!" );
						return nullptr;
					}
				}
				break;

				// <
				case HSMS_LT:
				{
					if( !bStartBracket )
					{
						bStartBracket = true;
					}
					nAngleBracketInc++;
				}
				break;

				//<L [12]
				case HSMS_LIST:
				{
					int nListCount = 0;
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )
					{
						token = m_scanner.get();
						//token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nListCount = std::stoi( token.s, 0, 10 );
							pSecsMsg->AddListItem( nListCount );

						}
						else
						{
							//error
						}
						token = m_scanner.get();
						if( token.t == TOK_RBRACKET ) token = m_scanner.get();
					}
					else if( token.t == TOK_ID )
					{
						nListCount = std::stoi( token.s, 0, 10 );
						pSecsMsg->AddListItem( nListCount );
					}
				}
				break;

				//<B [1] 0x15>
				case HSMS_BIN:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}

					int nBinCount = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nBinCount = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nBinCount;i++ )
							{
								if( token.t == TOK_ID )
								{
									short bin = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) bin = std::stoi( token.s.Mid(2), 0, 16  );
									else                                      bin = std::stoi( token.s, 0, 10 );

									pSecsMsg->AddBinaryItem( &bin, 1 );
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nBinCount = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nBinCount;i++ )
						{
							if( token.t == TOK_ID )
							{
								short bin = 0;
								if( token.s.Left(2).MakeUpper() == "0X" ) bin = std::stoi( token.s.Mid(2), 0, 16  );
								else                                      bin = std::stoi( token.s, 0, 10 );

								pSecsMsg->AddBinaryItem( &bin, 1 );
								token = m_scanner.get();
							}
						}
					}
				}
				break;

				case HSMS_BOOLEAN:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}
					int nBoolCount = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nBoolCount = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nBoolCount;i++ )
							{
								if( token.t == TOK_ID )
								{
									short sBool = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) sBool = std::stoi( token.s.Mid(2), 0, 16  );
									else                          sBool = std::stoi( token.s, 0, 10 );

									pSecsMsg->AddBooleanItem( &sBool, 1 );
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nBoolCount = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nBoolCount;i++ )
						{
							if( token.t == TOK_ID )
							{
								short bin = 0;
								if( token.s.Left(2).MakeUpper() == "0X" ) bin = std::stoi( token.s.Mid(2), 0, 16  );
								else                          bin = std::stoi( token.s, 0, 10 );

								pSecsMsg->AddBooleanItem( &bin, 1 );
								token = m_scanner.get();
							}
						}
					}

				}
				break;

				case HSMS_ASCII:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}

					uint nAsciiLen = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nAsciiLen = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							if( token.t == TOK_ID )
							{
								if( token.s.length() != nAsciiLen )
								{
									atstring fmt = "";
									fmt.Format("A [%d] is different with %s length", nAsciiLen, token.s.c_str() );
									addCompileError( token.n, token.col, fmt );
									break;
								}
								else
								{
									pSecsMsg->AddAsciiItem( token.s );
								}
							}
							else
							{
								atstring empty_string = "";
								pSecsMsg->AddAsciiItem( empty_string );
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nAsciiLen = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						if( token.t == TOK_ID )
						{
							if( token.s.length() != nAsciiLen )
							{
								atstring fmt = "";
								fmt.Format("A [%d] is different with %s length", nAsciiLen, token.s.c_str() );
								addCompileError( token.n, token.col, fmt );
								break;
							}
							else
							{
								pSecsMsg->AddAsciiItem( token.s );
							}
						}
						else
						{
							atstring empty_string = "";
							pSecsMsg->AddAsciiItem( empty_string );
						}
					}
				}
				break;

				case HSMS_J:
					break;

				case HSMS_I8:
				case HSMS_I1:
				case HSMS_I2:
				case HSMS_I4:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}
					int nICount = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nICount = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nICount;i++ )
							{
								if( token.t == TOK_ID )
								{
									if( cmd.command == HSMS_I8 )
									{
										long int i8 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i8 = std::stol( token.s.Mid(2), 0, 16  );
										else                          i8 = std::stol( token.s, 0, 10 );
										pSecsMsg->AddI8Item( &i8, 1 );
									}
									else if( cmd.command == HSMS_I4 )
									{
										signed int i4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
										else                          i4 = std::stoi( token.s, 0, 10 );
										pSecsMsg->AddI4Item( &i4, 1 );
									}
									else if( cmd.command == HSMS_I2 )
									{
										short i12 = 0;
										signed int i4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
										else                          i4 = std::stoi( token.s, 0, 10 );
										i12 = static_cast<short>(i4);
										pSecsMsg->AddI2Item( &i12, 1 );
									}
									else if( cmd.command == HSMS_I1 )
									{
										short i12 = 0;
										signed int i4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
										else                          i4 = std::stoi( token.s, 0, 10 );
										i12 = static_cast<short>(i4);
										pSecsMsg->AddI1Item( &i12, 1 );
									}
									else
									{

									}
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nICount = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nICount;i++ )
						{
							if( token.t == TOK_ID )
							{
								if( cmd.command == HSMS_I8 )
								{
									long int i8 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i8 = std::stol( token.s.Mid(2), 0, 16  );
									else                          i8 = std::stol( token.s, 0, 10 );
									pSecsMsg->AddI8Item( &i8, 1 );
								}
								else if( cmd.command == HSMS_I4 )
								{
									signed int i4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
									else                          i4 = std::stoi( token.s, 0, 10 );
									pSecsMsg->AddI4Item( &i4, 1 );
								}
								else if( cmd.command == HSMS_I2 )
								{
									short i12 = 0;
									signed int i4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
									else                          i4 = std::stoi( token.s, 0, 10 );
									i12 = static_cast<short>(i4);
									pSecsMsg->AddI2Item( &i12, 1 );
								}
								else if( cmd.command == HSMS_I1 )
								{
									short i12 = 0;
									signed int i4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoi( token.s.Mid(2), 0, 16  );
									else                          i4 = std::stoi( token.s, 0, 10 );
									i12 = static_cast<short>(i4);
									pSecsMsg->AddI1Item( &i12, 1 );
								}
								else
								{

								}
								token = m_scanner.get();
							}
						}
					}

				}
				break;

				case HSMS_F8:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}

					int nF8Count = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nF8Count = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nF8Count;i++ )
							{
								if( token.t == TOK_ID )
								{
									double dF8 = std::stod( token.s);
									pSecsMsg->AddF8Item( &dF8, 1 );
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nF8Count = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nF8Count;i++ )
						{
							if( token.t == TOK_ID )
							{
								double dF8 = std::stod( token.s);
								pSecsMsg->AddF8Item( &dF8, 1 );
								token = m_scanner.get();
							}
						}
					}
				}
				break;

				case HSMS_F4:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}

					int nF4Count = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();
						if( token.t == TOK_NUM )
						{
							nF4Count = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nF4Count;i++ )
							{
								if( token.t == TOK_ID )
								{
									float dF4 = std::stof( token.s);
									pSecsMsg->AddF4Item( &dF4, 1 );
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )	//[
					{
						nF4Count = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nF4Count;i++ )
						{
							if( token.t == TOK_ID )
							{
								float dF4 = std::stof( token.s);
								pSecsMsg->AddF4Item( &dF4, 1 );
								token = m_scanner.get();
							}
						}
					}
				}
				break;

				case HSMS_U8:
				case HSMS_U1:
				case HSMS_U2:
				case HSMS_U4:
				{
					if( !bStartBracket )
					{
						addCompileError( token.n, token.col, "< needed, start TYPE" );
						return nullptr;
					}

					int nUCount = 0;
					token = m_scanner.get();
					if( token.t == TOK_LBRACKET )	//[
					{
						token = m_scanner.get();

						if( token.t == TOK_NUM )
						{
							nUCount = std::stoi( token.s, 0, 10 );
							token = m_scanner.get();
							if( token.t == TOK_RBRACKET ) token = m_scanner.get();
							for( int i=0;i<nUCount;i++ )
							{
								if( token.t == TOK_ID )
								{
									if( cmd.command == HSMS_U8 )
									{
										long unsigned int i8 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i8 = std::stoul( token.s.Mid(2), 0, 16  );
										else                          i8 = std::stoul( token.s, 0, 10 );
										pSecsMsg->AddU8Item( &i8, 1 );
									}
									else if( cmd.command == HSMS_U4 )
									{
										long unsigned int lu4 = 0;
										uint u4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) lu4 = std::stoul( token.s.Mid(2), 0, 16  );
										else                          lu4 = std::stoul( token.s, 0, 10 );
										u4 =  static_cast<uint>(lu4);
										pSecsMsg->AddU4Item( &u4, 1 );
									}
									else if( cmd.command == HSMS_U2 )
									{
										unsigned short i12 = 0;
										long unsigned int i4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoul( token.s.Mid(2), 0, 16  );
										else                          i4 = std::stoul( token.s, 0, 10 );
										i12 = static_cast<short>(i4);
										pSecsMsg->AddU2Item( &i12, 1 );
									}
									else if( cmd.command == HSMS_U1 )
									{
										unsigned short i12 = 0;
										long unsigned int i4 = 0;
										if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoul( token.s.Mid(2), 0, 16  );
										else                          i4 = std::stoul( token.s, 0, 10 );
										i12 = static_cast<short>(i4);
										pSecsMsg->AddU1Item( &i12, 1 );
									}
									else
									{

									}
									token = m_scanner.get();
								}
							}
							if( token.t == TOK_GT )
							{
								nAngleBracketInc--;
							}
						}
						else
						{
							//error
						}
					}
					else if( token.t == TOK_ID )
					{
						nUCount = std::stoi( token.s, 0, 10 );
						token = m_scanner.get();
						for( int i=0;i<nUCount;i++ )
						{
							if( token.t == TOK_ID )
							{
								if( cmd.command == HSMS_U8 )
								{
									long unsigned int i8 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i8 = std::stoul( token.s.Mid(2), 0, 16  );
									else                          i8 = std::stoul( token.s, 0, 10 );
									pSecsMsg->AddU8Item( &i8, 1 );
								}
								else if( cmd.command == HSMS_U4 )
								{
									long unsigned int lu4 = 0;
									uint u4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) lu4 = std::stoul( token.s.Mid(2), 0, 16  );
									else                          lu4 = std::stoul( token.s, 0, 10 );
									u4 =  static_cast<uint>(lu4);
									pSecsMsg->AddU4Item( &u4, 1 );
								}
								else if( cmd.command == HSMS_U2 )
								{
									unsigned short i12 = 0;
									long unsigned int i4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoul( token.s.Mid(2), 0, 16  );
									else                          i4 = std::stoul( token.s, 0, 10 );
									i12 = static_cast<short>(i4);
									pSecsMsg->AddU2Item( &i12, 1 );
								}
								else if( cmd.command == HSMS_U1 )
								{
									unsigned short i12 = 0;
									long unsigned int i4 = 0;
									if( token.s.Left(2).MakeUpper() == "0X" ) i4 = std::stoul( token.s.Mid(2), 0, 16  );
									else                          i4 = std::stoul( token.s, 0, 10 );
									i12 = static_cast<short>(i4);
									pSecsMsg->AddU1Item( &i12, 1 );
								}
								else
								{

								}
								token = m_scanner.get();
							}
						}
					}
				}
				break;

				// >
				case HSMS_GT:
				{
					nAngleBracketInc--;
				}
				break;

				default:
				{
				}
				break;
			}
		}
		else
		{
			//addErrorF(CE_ERROR, "undefined syntax '%s'", word.c_str());
			//return false;
		}
	}
	if( m_vecError.size() > 0 )
	{
		if( pSecsMsg != nullptr ) delete pSecsMsg;
		return nullptr;
	}
	if( pSecsMsg != nullptr ) return pSecsMsg;
	return nullptr;
}
