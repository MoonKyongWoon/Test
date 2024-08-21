#include "hsms_cnv.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HSMS_OBJ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSmlObject::CSmlObject()
{
	Clear();
}

CSmlObject::~CSmlObject()
{
	Clear();
}

void CSmlObject::Clear()
{
	for( stSmlType* obj : m_vecSmlObj )
	{
		delete obj;
	}
	m_vecSmlObj.clear();
}

void CSmlObject::SetStreamFunction( int s, int f, bool ps )
{
	m_nStream   = s;
	m_nFunction = f;
	m_bPrimary  = ps;
}

void CSmlObject::SetPrimaryFlag(bool ps)
{
	m_bPrimary = ps;
}

void CSmlObject::SetSessionId( uint devid )
{
	m_nSessionId = devid;
}

void CSmlObject::SetSystemBytes( uint  sysbytes )
{
	m_nSystemBytes = sysbytes;
}

uint CSmlObject::GetSessionId()
{
	return m_nSessionId;
}

uint CSmlObject::GetSystemBytes()
{
	return m_nSystemBytes;
}

void CSmlObject::AddType( stSmlType* obj )
{
	m_vecSmlObj.push_back( obj );
}

int CSmlObject::GetStream()
{
	return m_nStream;
}

int CSmlObject::GetFunction()
{
	return m_nFunction;
}

bool CSmlObject::IsPrime()
{
	return m_bPrimary;
}

uint CSmlObject::TypeSize()
{
	return m_vecSmlObj.size();
}

stSmlType* CSmlObject::GetType(uint at)
{
	if( at > m_vecSmlObj.size() ) return nullptr;
	return m_vecSmlObj.at(at);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HSMS_CNV
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHsmsCnv::CHsmsCnv()
{
	m_vecError.clear();
	createSyntax();
}

CHsmsCnv::~CHsmsCnv()
{
	m_vecError.clear();
	m_mapSyntax.clear();	
}

void CHsmsCnv::createSyntax()
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
	//HSMS_SYNTAX( "P"   , HSMS_PRIMARY , 0        , 0 );
	//HSMS_SYNTAX( "S"   , HSMS_SLAVE   , 0        , 0 );
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

	Str2WordMap::iterator it = m_mapSyntax.begin();
	if( 0 )
	{
		while (it != m_mapSyntax.end())
		{
			printf( "%s\n", it->first.c_str() );
			it++;
		}
	}
}

void CHsmsCnv::addError( int line, int col, atstring str )
{
	atstring msg = "";
	msg.Format( "%d(%d) : %s", line, col, str.c_str() );
	m_vecError.push_back( msg );
}

std::vector<atstring> CHsmsCnv::getErrors()
{
	return m_vecError;
}

void CHsmsCnv::SetSessionId( uint id )
{
	m_SmlObject.SetSessionId( id );
}

void CHsmsCnv::SetSystemBytes( uint sysbytes )
{
	m_SmlObject.SetSystemBytes( sysbytes );
}

bool CHsmsCnv::SmlCompile( atstring psml )
{
	if( !m_scanner.loadsml( psml.c_str() ) )
	{
		printf( "%s\n", psml.c_str() );
		return false;
	}

	int nAngleBracketInc = 0;
	bool bStartBracket = false;
	bool bTypeDone = false;
	token_t token;
	atstring word = "";

	m_SmlObject.Clear();
	stSmlType* pSmlType = nullptr;
	while( 1 )
	{
		token = m_scanner.get();
		word  = token.s;
		printf( "word:%s[%d]\n", word.c_str(), token.t );

		if (token.t == TOK_EOL)
		{
			bStartBracket = false;
			bTypeDone = false;
			continue;
		}
		else if (token.t == TOK_EOF)
		{
			 break;
		}
		//else if (token.t == TOK_ERROR)
		//{
		//	//addError(CE_ERROR, token.s);
		//	continue;
		//}
		HSMSCMD cmd = { 0 };
		Str2WordMap::iterator it = m_mapSyntax.find(word);
		if (it != m_mapSyntax.end())
		{
			cmd.value = it->second;
			switch ( cmd.command )
			{
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

				// TYPE
				case HSMS_SF:
				{
					if( !bStartBracket )
					{
						//error
					}
					else
					{
						int stream   = cmd.opt;
						int function = cmd.bit;
						m_SmlObject.SetStreamFunction( stream, function, false );
						bTypeDone = true;

						token = m_scanner.get();
						if( token.t == TOK_ID )
						{
								 if( token.s == "P" ) m_SmlObject.SetPrimaryFlag( true );
							else if( token.s == "S" ) m_SmlObject.SetPrimaryFlag( false );
							else
							{
								addError( token.n, token.col, token.s );
							}
						}
					}
				}
				break;

				case HSMS_LIST:
				case HSMS_BIN:
				case HSMS_BOOLEAN:
				case HSMS_ASCII:
				case HSMS_J:
				case HSMS_I8:
				case HSMS_I1:
				case HSMS_I2:
				case HSMS_I4:
				case HSMS_F8:
				case HSMS_F4:
				case HSMS_U8:
				case HSMS_U1:
				case HSMS_U2:
				case HSMS_U4:
				{
					if( !bStartBracket )
					{
						//error
					}
					else
					{
						pSmlType = new stSmlType;
						pSmlType->type      = static_cast<HSMS_FMT_CODE>(cmd.opt);
						pSmlType->type_size = cmd.bit;

						atstring svalue = "", slen = "";
						token = m_scanner.get();
						if( token.t == TOK_ID )
						{
							slen = token.s.Trim();
							pSmlType->count = std::atoi( slen.c_str() );
							if( cmd.command == HSMS_ASCII )
							{
								bool bSummary = false;
								int nSummary = 0;
								while( nSummary < pSmlType->count )
								{
									token = m_scanner.get();
									//가나다 + < + 라마바 + > + ABC | >
									if( (token.t == TOK_ID) || (token.t != TOK_LT) || (token.t != TOK_GT) )
									{
										svalue += token.s;
										nSummary += token.s.length();
									}
									else if( (token.t == TOK_EOL) || (token.t != TOK_EOF) )
									{
										break;
										//error
									}
								}
								if( nSummary != pSmlType->count )
								{
									atstring msg = "ASCII processing error!";
									addError( token.n, token.col, msg );
									delete pSmlType;
									pSmlType = nullptr;
								}
								else
								{
									pSmlType->svalue = svalue;
									m_SmlObject.AddType( pSmlType );
								}
							}
							else if( cmd.command == HSMS_LIST )
							{
								m_SmlObject.AddType( pSmlType );
							}
							else
							{
								token = m_scanner.get();
								if( token.t == TOK_ID )
								{
									//value
									svalue = token.s;
									pSmlType->svalue = svalue;
									m_SmlObject.AddType( pSmlType );
								}
								else
								{
									//error
									break;
								}
							}
						}
						else
						{

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
	if( m_vecError.size() > 0 ) return false;
	return true;
}

bool CHsmsCnv::GetHsmsPacket( QByteArray& rBa )
{
	bool bMaked = true;
	rBa.clear();
	bMaked &= MakeMsmsMsg();
	bMaked &= MakeHsmsMsgHeader();
	if( bMaked )
	{
		rBa.append( m_arrHeader );
		rBa.append( m_arrMsg );
	}
	return bMaked;
}


#define HSMS_MESSAGE_HEAD_SIZE		sizeof(stHsmsMsgHeader)
bool CHsmsCnv::MakeHsmsMsgHeader()
{
	memset( &m_hsmsHeader, 0x0, sizeof(m_hsmsHeader) );
	//m_hsmsHeader.nMsgLength = sizeof(stHsmsMsgHeader);
	m_hsmsHeader.stMsgHeader.cRbit = 0x0;
	m_hsmsHeader.stMsgHeader.cDevidMsb = ( m_SmlObject.GetSessionId() >>8 ) & 0x7f;
	m_hsmsHeader.stMsgHeader.cDevidLsb = ( m_SmlObject.GetSessionId() >>0 ) & 0xff;
	m_hsmsHeader.stMsgHeader.cStream   = m_SmlObject.GetStream()&0xff;
	m_hsmsHeader.stMsgHeader.cWbit     = m_SmlObject.IsPrime() ? 1 : 0;
	m_hsmsHeader.stMsgHeader.cFunction = m_SmlObject.GetFunction()&0xff;
    m_hsmsHeader.stMsgHeader.cSystemBytes[0] = (m_SmlObject.GetSystemBytes() >> 24 ) & 0xFF;
    m_hsmsHeader.stMsgHeader.cSystemBytes[1] = (m_SmlObject.GetSystemBytes() >> 16 ) & 0xFF;
    m_hsmsHeader.stMsgHeader.cSystemBytes[2] = (m_SmlObject.GetSystemBytes() >>  8 ) & 0xFF;
    m_hsmsHeader.stMsgHeader.cSystemBytes[3] = (m_SmlObject.GetSystemBytes() >>  0 ) & 0xFF;

	int MsgLen = m_arrMsg.length() + sizeof(stHsmsMsgHeader);
	m_hsmsHeader.szMsgLength[0] = (MsgLen>>24)&0xFF;
	m_hsmsHeader.szMsgLength[1] = (MsgLen>>16)&0xFF;
	m_hsmsHeader.szMsgLength[2] = (MsgLen>> 8)&0xFF;
	m_hsmsHeader.szMsgLength[3] = (MsgLen>> 0)&0xFF;

	m_arrHeader.clear();
	m_arrHeader.append( (char*)&m_hsmsHeader, sizeof(stHsmsHeader) );
	return true;
}

bool CHsmsCnv::MakeTypeToHsms(stSmlType* pType, QByteArray&  rBa )
{
	bool bMakeType = true;
	QByteArray baFmtLength;
	QByteArray baData;

	baFmtLength.clear();

	rBa.clear();
	if( pType->count == 0 )
	{
		baFmtLength.append( pType->type<<2 | 1 );
		baFmtLength.append( pType->count&0xff );
	}
	else if( pType->count < 0xff )
	{
		baFmtLength.append( pType->type<<2 | 1 );
		baFmtLength.append( pType->count*pType->type_size&0xff );
	}
	else if( pType->count < 0xffff )
	{
		baFmtLength.append( pType->type<<2 | 2 );
		baFmtLength.append( (pType->count*pType->type_size>>8)&0xff );
		baFmtLength.append( (pType->count*pType->type_size>>0)&0xff );
	}
	else if( pType->count < 0xffffff )
	{
		baFmtLength.append( pType->type<<2 | 3 );
		baFmtLength.append( (pType->count*pType->type_size>>16)&0xff );
		baFmtLength.append( (pType->count*pType->type_size>> 8)&0xff );
		baFmtLength.append( (pType->count*pType->type_size>> 0)&0xff );
	}
	else
	{
		bMakeType = false;
	}
	if( !bMakeType ) return false;

	rBa.append( baFmtLength );
	if( pType->count == 0 ) return true;

	baData.clear();
	switch( pType->type )
	{
		case HSMS_FMT_CODE::FMT_LIST:
		{
		}
		break;

		case HSMS_FMT_CODE::FMT_BIN:
		{
			int bin = 0;
			if( pType->svalue.Left(2) == "0X" ) bin = std::stoi( pType->svalue.Mid(2), 0, 16  );
			else                                bin = std::stoi( pType->svalue, 0, 10 );
			baData.append( bin&0xff );
		}
		break;

		case HSMS_FMT_CODE::FMT_BOOL:
		{
			int bin = std::stoi( pType->svalue );
			baData.append( bin&0xff );
		}
		break;

		case HSMS_FMT_CODE::FMT_ASCII:
		{
			for( int c=0;c<pType->count;c++ )
			{
				if( c < static_cast<int>(pType->svalue.length()) )
					baData.append( pType->svalue.at(c) );
			}
		}
		break;

		case HSMS_FMT_CODE::FMT_JIS_8:
			break;

		case HSMS_FMT_CODE::FMT_I8:
		case HSMS_FMT_CODE::FMT_I1:
		case HSMS_FMT_CODE::FMT_I2:
		case HSMS_FMT_CODE::FMT_I4:
		{
			long long signed int i8 = 0;
			if( pType->svalue.Left(2) == "0X" ) i8 = std::stoll( pType->svalue.Mid(2), 0, 16  );
			else                                i8 = std::stoll( pType->svalue, 0, 10 );

			for( int t=0;t<pType->type_size;t++ )
			{
				baData.append( (i8>>(pType->type_size-1-t)*8) );
			}
		}
		break;

		case HSMS_FMT_CODE::FMT_F8:
		{
			char szF8[8]={0x0,};
			double f8 = std::stod( pType->svalue);
			memcpy( szF8, &f8, sizeof(double) );
			for( int t=sizeof(double)-1;t>=0;t-- )
			{
				baData.append( szF8[t] );
			}
		}
		break;

		case HSMS_FMT_CODE::FMT_F4:
		{
			char szF4[8]={0x0,};
			float f4 = std::stof( pType->svalue);
			memcpy( szF4, &f4, sizeof(float) );
			float tf4 = 0.0;
			memcpy(&tf4, szF4, 4 );
			for( int t=sizeof(float)-1;t>=0;t-- )
			{
				baData.append( szF4[t] );
			}
		}
		break;

		case HSMS_FMT_CODE::FMT_U8:
		case HSMS_FMT_CODE::FMT_U1:
		case HSMS_FMT_CODE::FMT_U2:
		case HSMS_FMT_CODE::FMT_U4:
		{
			long long unsigned int ui8 = 0;	//std::stoull( pType->svalue );
			if( pType->svalue.Left(2) == "0X" ) ui8 = std::stoull( pType->svalue.Mid(2), 0, 16  );
			else                                ui8 = std::stoull( pType->svalue, 0, 10 );

			for( int t=0;t<pType->type_size;t++ )
			{
				baData.append( (ui8>>(pType->type_size-1-t)*8) );
			}
		}
		break;

		default:
			break;
	}
	rBa.append( baData );
	return true;
}

bool CHsmsCnv::MakeMsmsMsg()
{
	m_arrMsg.clear();
	for( uint n=0;n<m_SmlObject.TypeSize();n++ )
	{
		QByteArray ba;
		ba.clear();
		stSmlType* pType = m_SmlObject.GetType(n);
		if( !MakeTypeToHsms( pType, ba ) )
		{
			atstring sErr;
			sErr.Format( "%d:%d:%s make hsms message error!", pType->type, pType->count, pType->svalue );
			addError( 0, 0, sErr );
			break;
		}
		m_arrMsg.append( ba );
	}
	return true;
}

QString CHsmsCnv::getTypeCode(HSMS_FMT_CODE code)
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

bool CHsmsCnv::HsmsToSml(char* pHsms, uint len, atstring& rSml )
{
	stHsmsHeader hsmsheader;
	m_vecError.clear();

	if(len < sizeof(stHsmsHeader))
	{
		atstring errMsg = "";
		errMsg.Format("%s : packet header size error!", __FUNCTION__);
		addError(0, 0, errMsg);

		return false;
	}

	atstring sFmt = "";
	QByteArray ba;
	memcpy(&hsmsheader, pHsms, sizeof(stHsmsHeader));

	unsigned int nMsgLength = 0x0;
	nMsgLength |= (hsmsheader.szMsgLength[0] << 24);
	nMsgLength |= (hsmsheader.szMsgLength[1] << 16);
	nMsgLength |= (hsmsheader.szMsgLength[2] <<  8);
	nMsgLength |= (hsmsheader.szMsgLength[3] <<  0);

	unsigned int nDataLen     = nMsgLength - sizeof(stHsmsMsgHeader);

	sFmt.Format("S%dF%d %c", hsmsheader.stMsgHeader.cStream, hsmsheader.stMsgHeader.cFunction, hsmsheader.stMsgHeader.cWbit? 'W' : ' ');
	rSml += sFmt;

	if (nDataLen > 0)
	{
		rSml += "\n";
		ba.append(pHsms + sizeof(stHsmsHeader), nDataLen);
		int nDepth = 0;
		QVector<int> list_count = {0};
		unsigned int i = 0;

		while (i < nDataLen)
		{
			char format_byte = ba.at(i++);
			HSMS_FMT_CODE code  = static_cast<HSMS_FMT_CODE>((format_byte >> 2) & 0x3f);
			unsigned int num_of_len_byte = format_byte & 0x3;
			unsigned int len_byte = 0x0;
			unsigned char len_ch = 0x0;
			int shift = 0;

			for (unsigned int len = 0; len < num_of_len_byte; len++)
			{
				len_ch = ba.at(i++);
				shift = (num_of_len_byte - 1 - len) * 8;
				len_byte |= (len_ch << shift);
			}

			QString temp = "";

			for (int depth = 0; depth < nDepth; depth++)
			{
				temp += "\t";
			}

			switch(code)
			{
				case FMT_LIST:
				{
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte);

					if (len_byte == 0)
					{
						temp += ">";
					}
					else
					{
						nDepth++;
						list_count.push_back(len_byte + 1);
					}

					rSml += temp.toStdString();

					break;
				}
				case FMT_BIN:
				{
					char bin = 0x0;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte);

					while (len_byte > 0)
					{
						bin |= ( (ba.at(i++) & 0xFF) << (((len_byte - 1) % 1) * 8) );

						if ((len_byte - 1) % 1 == 0)
						{
							temp += QString("").asprintf(" 0x%02X", bin);
							bin = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_BOOL:
				{
					bool b;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte);
					b = 0x0;

					while (len_byte > 0)
					{
						b |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 1) * 8));

						if ((len_byte - 1) % 1 == 0)
						{
							temp += QString("").asprintf(" 0x%02X", b);
							b = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_JIS_8:
					break;
				case FMT_I1:
				{
					int8_t num_8;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte);
					num_8 = 0x0;

					while (len_byte > 0)
					{
						num_8 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 1) * 8));

						if ((len_byte - 1) % 1 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(num_8));
							num_8 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_U1:
				{
					uint8_t u_num_8;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte);
					u_num_8 = 0x0;

					while (len_byte > 0)
					{
						u_num_8 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 1) * 8));

						if ((len_byte - 1) % 1 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(u_num_8));
							u_num_8 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_ASCII:
				{
					temp += QObject::tr("<%1 [%2] \"").arg(getTypeCode(code)).arg(len_byte);

					while(len_byte > 0)
					{
						temp += ba.at(i++);
						len_byte--;
					}

					temp += "\">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_I2:
				{
					int16_t num_16;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 2);
					num_16 = 0x0;

					while (len_byte > 0)
					{
						num_16 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 2) * 8));

						if ((len_byte - 1) % 2 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(num_16));
							num_16 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_U2:
				{
					uint16_t u_num_16;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 2);
					u_num_16 = 0x0;

					while (len_byte > 0)
					{
						u_num_16 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 2) * 8));

						if ((len_byte - 1) % 2 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(u_num_16));
							u_num_16 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_I4:
				{
					int32_t num_32;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 4);
					num_32 = 0x0;

					while (len_byte > 0)
					{
						num_32 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 4) * 8));

						if ((len_byte - 1) % 4 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(num_32));
							num_32 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_U4:
				{
					uint32_t u_num_32;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 4);
					u_num_32 = 0x0;

					while (len_byte > 0)
					{
						u_num_32 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 4) * 8));

						if ((len_byte - 1) % 4 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(u_num_32));
							u_num_32 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_F4:
				{
					int32_t num_32;
					float float_32;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 4);
					num_32 = 0x0;
					float_32 = 0x0;

					while (len_byte > 0)
					{
						num_32 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 4) * 8));

						if ((len_byte - 1) % 4 == 0)
						{
							float_32 = *reinterpret_cast<float*>(&num_32);
							//float_32 = static_cast<float>(num_32);
							temp += QObject::tr(" %1").arg(QString::number(float_32, 'f'));
							float_32 = 0x0;
							num_32 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_I8:
				{
					int64_t num_64;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 8);
					num_64 = 0x0;

					while (len_byte > 0)
					{
						num_64 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 8) * 8));

						if ((len_byte - 1) % 8 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(num_64));
							num_64 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_U8:
				{
					uint64_t u_num_64;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 8);
					u_num_64 = 0x0;

					while (len_byte > 0)
					{
						u_num_64 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 8) * 8));

						if ((len_byte - 1) % 8 == 0)
						{
							temp += QObject::tr(" %1").arg(QString::number(u_num_64));
							u_num_64 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
				case FMT_F8:
				{
					int64_t num_64;
					double float_64;
					temp += QObject::tr("<%1 [%2]").arg(getTypeCode(code)).arg(len_byte / 8);
					num_64 = 0x0;
					float_64 = 0x0;

					while (len_byte > 0)
					{
						num_64 |= ((ba.at(i++) & 0xFF) << (((len_byte - 1) % 8) * 8));

						if ((len_byte - 1) % 8 == 0)
						{
							float_64 = *reinterpret_cast<double*>(&num_64);
							//float_64 = static_cast<double>(num_64);
							temp += QObject::tr(" %1").arg(QString::number(float_64, 'f'));
							float_64 = 0x0;
							num_64 = 0x0;
						}

						len_byte--;
					}

					temp += ">";
					rSml += temp.toStdString();

					break;
				}
			}

			if (list_count[nDepth] > 0)
			{
				list_count[nDepth]--;
				rSml += "\n";
			}

			if (list_count[nDepth] == 0)
			{
				while (nDepth > 0)
				{
					nDepth--;
					list_count.pop_back();
					list_count[nDepth]--;

					for (int depth = 0; depth < nDepth; depth++)
					{
						rSml += "\t";
					}

					rSml += ">";

					if(nDepth == 0)
					{
						break;
					}
					else
					{
						if (list_count[nDepth] > 0)
						{
							rSml += "\n";

							break;
						}

						rSml += "\n";
					}
				}
			}
		}
	}

	rSml += " .\n";

	return true;
}
