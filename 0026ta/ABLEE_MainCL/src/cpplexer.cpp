#include "cpplexer.hpp"

CCppLexer::CCppLexer()
{
    m_nCurrent = 0;
    m_bInCommentBlock = false;
    m_vecToken.clear();
}


bool CCppLexer::LoadFile( atstring sCpp )
{
    QFile file( sCpp.c_str() );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        fprintf( stderr, "%s open error!", sCpp.c_str() );
        return false;
    }
    m_sFN = sCpp;

	int line = 0;
    while( !file.atEnd() )
    {
		line++;
		QByteArray ba = file.readLine();
		if( ba.trimmed().length() < 1 )
			continue;

		if( ParseLine(ba) )
        {
			Tokenize( ba, line );
        }
    }
	file.close();

	token_cpp token;
    token.t = TOKEN_EOF;
    token.s = "";
    token.l = line;
    m_vecToken.push_back( token );
    return true;
}

//prepare...not use
bool CCppLexer::LoadData( atstring name, int line, char* pData )
{
	m_sFN = name;
	char* p = pData;
	QByteArray ba;
	atstring linedata = "";
	while( *p )
	{
		char ch = *p;
		if ( ch == 0xA )
		{
			ba.append( *p );
			if( ParseLine(ba) )
			{
				Tokenize( ba, line );			
				ba.clear();
			}
			line++;
		}
		else
		{
			ba.append( *p );
		}
	}

	token_cpp token;
	token.t = TOKEN_EOF;
	token.s = "";
	token.l = line;
	m_vecToken.push_back( token );
	return true;
}

bool CCppLexer::ParseLine( QByteArray& ba )
{
	ba.replace( "\t", " " );
	QByteArray baData = ba;
    if ( m_bInCommentBlock )
    {
		int ne = baData.indexOf("*/");
		if (ne != -1 )
		{
			baData.remove( 0, ne+2 );
			m_bInCommentBlock = false;
        }
        if( m_bInCommentBlock ) return false;
    }
    else
    {
        baData = ba.trimmed();
        int nc = baData.indexOf("//");
        if( nc != -1 )
        {
            baData.remove( nc, baData.length()-nc );
        }
		if( baData.size() < 1 )
		{
			ba = baData;
			return false;
		}
    }

    int ns = baData.indexOf("/*");
    if ( ns != -1 )
    {
        int ne = baData.indexOf("*/");
        if ( (ne != -1) )
        {
            m_bInCommentBlock = false;
            baData.remove( ns, ne-ns+2 );
        }
        else
        {
            m_bInCommentBlock = true;
            baData.remove(ns, baData.length()-ns );
        }
    }
    if( m_bInCommentBlock )
    {
        if( baData.trimmed().length() < 1 )
            return false;
    }
    ba = baData.trimmed();
    return true;
}

//not use
bool CCppLexer::TokenizeSplit( QByteArray& ba, int line )
{
	QString sBa = ba;
	//\b (word boundary), to split the string into an alternating sequence of non-word and word tokens:
	QStringList listBa = sBa.split( QRegExp("\\b"), Qt::SkipEmptyParts );
	qDebug() << listBa;
	for( QString element : listBa )
	{
		QByteArray baEmt = element.toUtf8();

		if( baEmt.trimmed() == "" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_SPACE;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
			continue;
		}

		baEmt = baEmt.trimmed();
		if( baEmt == "#" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_SHARP;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt == "(" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_LPARENTHESIS;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt == ")" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_RPARENTHESIS;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt == "{" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_LBRACE;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt == "}" )
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_RBRACE;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt.left(2).toUpper() == "0X" )
		{
			bool bDigit = true;
			int nLength = baEmt.length()-2;
			QRegExp hexMatcher( QObject::tr("^[0-9A-F]{%1}$").arg(nLength), Qt::CaseInsensitive);
			if( !hexMatcher.exactMatch( baEmt.mid(2) ) )
				bDigit = false;
			token_cpp token;
			if( bDigit )
				token.t = TOKEN_DIGIT;
			else
				token.t = TOKEN_LETTER;
			token.l = line;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( baEmt.toLower() == "char"     || baEmt.toLower() == "short"  || baEmt.toLower() == "int"    ||
				 baEmt.toLower() == "long"     || baEmt.toLower() == "float"  || baEmt.toLower() == "double" ||
				 baEmt.toLower() == "unsigned" || baEmt.toLower() == "signed" || baEmt.toLower() == "void"   || baEmt.toLower() == "const" )
		{
			token_cpp token;
			token.t = TOKEN_CDATATYPE;
			token.l = line;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
		else if( QChar(baEmt.at(0)).isLetter() )	//ex)aa(oo, aa{int
		{
			atstring letter;
			letter += baEmt.at(0);
			for( int npos=1;npos<baEmt.length();npos++ )
			{
				QChar ch = QChar(baEmt.at(npos));
				if( ch.isSpace() ) continue;
				else if( ch.isPunct() )
				{
					token_cpp token;
					token.t = TOKEN_PUNC;
					token.l = line;
					token.s += baEmt.at(npos);
				}
				else
				{
					token_cpp token;
					token.t = TOKEN_LETTER;
					token.l = line;
					token.s = letter;
					m_vecToken.push_back( token );
					letter = "";
				}
			}
		}
		else
		{
			bool bLetter = true;
			bool bPunc   = true;
			bool bDigit  = true;
			for( int npos=0;npos<baEmt.length();npos++ )
			{
				QChar ch = baEmt.at(npos);
				if( !ch.isPunct() ) bPunc  = false;			//!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
				if( !ch.isDigit() ) bDigit = false;
				if( !ch.isLetter() && !ch.isDigit() && !'_' ) bLetter = false;
			}
			token_cpp token;
			if( bPunc )
				token.t = TOKEN_PUNC;
			else if( bLetter )
				token.t = TOKEN_LETTER;
			else if( bDigit )
				token.t = TOKEN_DIGIT;
			else
				token.t = TOKEN_LETTER;
			token.l = line;
			token.s = baEmt.toStdString();
			m_vecToken.push_back( token );
		}
	}
	token_cpp token;
	token.l = line;
	token.t = TOKEN_EOL;
	token.s = "";
	m_vecToken.push_back( token );
	return true;
}

bool CCppLexer::Tokenize( QByteArray& baLine, int line )
{
	QList<atstring> list;
	QByteArray ba = baLine.trimmed();
	char* p = ba.data();
	atstring stoken = "";
	while( *p )
	{
		char ch = *p;
		if ( (ch == 0x0D) | (ch<0x20) | (ch>0x7E) ) 	//ignore reference ascii table
		{
			p++;
			continue;
		}

		if( isspace(ch) )
		{
			stoken += *p++;
			while( isspace(*p) || (*p =='\t') ) p++;
			token_cpp token;
			token.t = TOKEN_SPACE;
			token.s = stoken;
			token.l = line;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if
		(
			(ch == '=' && *(p+1) == '=') ||
			(ch == '!' && *(p+1) == '=') ||
			(ch == '>' && *(p+1) == '=') ||
			(ch == '<' && *(p+1) == '=') ||
			(ch == '&' && *(p+1) == '&') ||
			(ch == '|' && *(p+1) == '|') ||
			(ch == '+' && *(p+1) == '+') ||
			(ch == '-' && *(p+1) == '-') ||
			(ch == '+' && *(p+1) == '=') ||
			(ch == '-' && *(p+1) == '=') ||
			(ch == '<' && *(p+1) == '<') ||
			(ch == '>' && *(p+1) == '>') )
		{
			stoken += *p++;
			stoken += *p++;

			token_cpp token;
			token.t = TOKEN_OPERATOR;
			token.s = stoken;
			token.l = line;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if ( ch == '_' || isalpha(ch) || ch == '~')	//Letter
		{
			stoken += *p++;
			while (isalnum(*p) || *p == '_'  || *p == '.' )	//a10 or a11_11 or deviceif.h
			{
				stoken += *p++;
			}
			token_cpp token;
			atstring slower = stoken;//.MakeLower();
			if( slower == "char"     || slower == "short"  || slower == "int"    ||
				slower == "long"     || slower == "float"  || slower == "double" ||
				slower == "unsigned" || slower == "signed" || slower == "void"   || slower == "const" )
			{
				token.t = TOKEN_CDATATYPE;
			}
			else
				token.t = TOKEN_LETTER;
			token.s = stoken;
			token.l = line;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if ( ch == '0' && toupper(*(p+1)) == 'X' ) // hex digits
		{
			stoken += *p++;
			stoken += *p++;
			while( isxdigit(*p) )
			{
				stoken += *p++;
			}
			token_cpp token;
			token.t = TOKEN_DIGIT;
			token.s = stoken;
			token.l = line;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if ( isdigit(ch) ) // number
		{
			stoken += *p++;
			while( isdigit(*p) )
			{
				stoken += *p++;
			}
			token_cpp token;
			token.t = TOKEN_DIGIT;
			token.s = stoken;
			token.l = line;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if( ispunct(ch) )	//!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
		{
			stoken += *p++;
			token_cpp token;
			token.s = stoken;
			token.l = line;
				 if( stoken == "(" ) token.t = TOKEN_LPARENTHESIS;
			else if( stoken == ")" ) token.t = TOKEN_RPARENTHESIS;
			else if( stoken == "{" ) token.t = TOKEN_LBRACE;
			else if( stoken == "}" ) token.t = TOKEN_RBRACE;
			else if( stoken == "#" ) token.t = TOKEN_SHARP;
			else                     token.t = TOKEN_PUNC;
			//list.append( stoken );
			m_vecToken.push_back( token );
			stoken = "";
		}
		else
		{
			token_cpp token;
			token.l = line;
			token.t = TOKEN_ERROR;
			token.s = "";
			//list.append( stoken );
			m_vecToken.push_back( token );
		}
	}
	token_cpp token;
	token.l = line;
	token.t = TOKEN_EOL;
	token.s = "";
	m_vecToken.push_back( token );

	if( 0 )
	{
		for( atstring str : list )
		{
			fprintf( stdout, "%s|", str.c_str() );
		}
		fprintf( stdout, "\n" );
	}
	return true;
}

token_cpp& CCppLexer::Get(bool bPeek)
{
    if( bPeek )
    {
		if( m_nCurrent > 0 )
			return m_vecToken[--m_nCurrent];
		return m_vecToken[m_nCurrent];
    }
    else
    {
		if( m_nCurrent < m_vecToken.size() )
			return m_vecToken[m_nCurrent++];
		return m_vecToken[m_nCurrent];
    }
}
