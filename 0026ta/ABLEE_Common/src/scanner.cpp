#include "scanner.hpp"


CScanner::CScanner(void)
{
	m_buf     = NULL;
	cleanup();
	//printf("current working directory -> %s\n", cwd().c_str());
}

CScanner::~CScanner(void)
{
	cleanup();
}

atstring CScanner::cwd()
{
	atstring sCwd = "";
	char buff[128];
	memset(buff, 0x0, sizeof(buff));
	GetCurrentDir(buff, FILENAME_MAX);
	sCwd.Format("%s", buff);
	return sCwd;
}

void CScanner::cleanup()
{
	if (m_buf)
	{
		delete m_buf;
		m_buf = NULL;
	}

	m_nLine = 0;
	m_nTokenIndex = 0;
	m_bInCommentBlock = false;
	m_aToken.clear();
}

bool CScanner::loadfile(const char* fn)
{
	/*
	FILE* fp;
	atstring s;
	errno_t err;

	err = fopen_s(&fp, fn, "rb");
	if ( err != 0 )
	{  
		s.Format( "can't open file %s\n", fn );
		addToken( TOK_ERROR,  s, 0 );
		return false;
	}
	cleanup();
	setFN(fn);

	fseek(fp, 0L, SEEK_END);
	m_nLength = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	m_buf = new BYTE[m_nLength + 10];
	if (m_buf == NULL) {
		fclose(fp);
		return false;
	}
	memset(m_buf, 0, m_nLength + 10);
	size_t rs = fread(m_buf, 1, m_nLength, fp);
	if ( rs < 1)
	{
		atstring s;
		s.Format("can't read file '%s'\n", fn);
		addToken(TOK_ERROR, s, 0);
		fclose(fp);
		return false;
	}
	fclose(fp);
	m_p = (BYTE*)m_buf;
	*/
	return true;
}

bool CScanner::loadsml(const char* psml)
{
	atstring s = atstring(psml);
	atstring sml = s.Trim();
	if( sml.length() < 1 )
		return false;

	cleanup();
	setFN(psml);

	m_nLength = sml.length();
	m_buf = new BYTE[m_nLength + 5];
	if (m_buf == NULL) {
		return false;
	}
	memset(m_buf, 0, m_nLength + 5);
	strncpy( (char*)m_buf, psml, m_nLength ); //  m_buf, psml, m_nLength );
	m_p = m_buf;
	return true;
}

token_t& CScanner::get(bool bPeek)
{
	if (m_nTokenIndex >= m_aToken.size()) 
	{
		 parseLine();
	}

	if (bPeek)
	{		
		m_aToken[m_nTokenIndex].s = m_aToken[m_nTokenIndex].s;
		return m_aToken[m_nTokenIndex];
	}
	else 
	{
		m_aToken[m_nTokenIndex].s = m_aToken[m_nTokenIndex].s;
		return m_aToken[m_nTokenIndex++];
	}
}

void CScanner::addToken(token_enum t, atstring s, int col)
{
	token_t tok;
	tok.col = col;
	tok.t = t;
	tok.s = s;	//.MakeUpper();
	tok.f = 0.0;
	tok.n = m_nLine;
	m_aToken.push_back(tok);
}

bool CScanner::parseLine()
{
	bool bLT = false;
	bool bGT = false;
	bool bType = false;
	atstring line = "", s = "", es = "";
	
	if ( m_bInCommentBlock ) 
	{
		while (m_bInCommentBlock) 
		{
			while (!isEOF() && isEOL())
				line += *m_p++;
			m_p++;
			m_nLine++;

			size_t ns = line.find("/*");
			size_t ne = line.find("*/");
			if( ns != string::npos) 
			{
				addToken(TOK_ERROR, "line comment(/* ~ */) use error!!", 0);
				return false;
			}
			if (ne != string::npos) 
			{
				line.erase(0, ne+2);
				m_bInCommentBlock = false;
				break;
			}
			line = "";
		}
	} 
	else {
		while (!isEOF() && *m_p != CHAR_LF)
			line += *m_p++;
		m_p++;
		m_nLine++;
	}

	size_t ns = line.find("/*");
	if (ns != string::npos) 
	{
		size_t ne = line.find("*/");
		if ( (ne != string::npos) ) 
		{
			if( ne < ns )
			{
				addToken(TOK_ERROR, "line comment(/* ~ */) use error!!", 0);
				return false;
			}
			m_bInCommentBlock = false;
			line.erase(ns, ne-ns+2);		
		}
		else
		{
			m_bInCommentBlock = true;
			line.erase(ns, line.size()-ns );		
		}
	}

	line.ExpandTab(TAB_SIZE);	
	line = line.TrimRight();

	m_aToken.clear();
	m_nTokenIndex = 0;

	BYTE* p = line;

	int col = 0;
	int colsave = 0;
	while ( *p )
	{
		BYTE c = *p;
		if ( c == CHAR_CR  ) 	//ignore
		{
			p++;
		}
		else if ( c == ' ' )
		{
			col++;
			p++;
			continue;
		}
		else if (c == '/' && *(p+1) == '/')
		{
			addToken(TOK_EOL, "", 0);
			break;
		}		
		else if ( c == CHAR_LF )
		{
			addToken(TOK_EOL, "", 0);
			break;
		}
		else if( c == '<' )
		{
			colsave = col;
			s += *p++;
			col++;
			addToken(TOK_LT, s, colsave);

			s = "";			
			if( !bLT )
			{
				bLT = true;
				bool bAdd = false;
				c = *p;
				while( (c != ' ') &&  (c != CHAR_LF) && (*p) )
				{
					s += *p++;
					col++;
					c = *p;
					bAdd = true;
				}

				if( bAdd )
				{
					colsave = col;
					//s.MakeUpper().Trim();
					s.Trim();
					addToken(TOK_TYPE, s, colsave);
					s = "";
					bType = true;
				}
			}
		}
		else if( c == '>' )
		{
			colsave = col;
			s += *p++;
			col++;
			addToken(TOK_GT, s, colsave);
			s = "";
		}
		else if( c == '[' )
		{
			colsave = col;
			s += *p++;
			col++;
			addToken(TOK_LBRACKET, s, colsave);
			s = "";

			c = *p;
			while( isdigit(c) )
			{
				s += *p++;
				col++;
				c = *p;
			}
			addToken(TOK_NUM, s, colsave);
			s = "";
		}
		else if( c == ']' )
		{
			colsave = col;
			s += *p++;
			col++;
			addToken(TOK_RBRACKET, s, colsave);
			s = "";
		}
		else if( c == '"' )	//ASCII
		{
			p++;
			col++;
			colsave = col;
			c = *p;
			while( c != '"' )
			{
				s += *p++;
				col++;
				c = *p;
			}
			addToken(TOK_ID, s, colsave);
			s = "";
			p++;
			col++;
		}
		//else if (ispunct(c)) {			// other
		//	colsave = col;
		//	s += *p++;
		//	col++;
		//	addToken(TOK_PUNC, s, colsave);
		//	s = "";
		//}
		else if( *p )
		{
			colsave = col;
			s += *p++;
			c = *p;
			while ( (c != '<') &&  (c != '>') && (c != '[') &&  (c != ']') &&(c != CHAR_LF) && (c != ' ') && *p )
			{
				s += *p++;
				col++;
				c = *p;
			}
			//s.MakeUpper();					// id is case insensitive!
			addToken( TOK_ID  , s, colsave);
			s = "";
		}
		else
		{
			addToken(TOK_ERROR, "invalid character found in this line", 0);
			return false;
		}
	}

	addToken(TOK_EOL, "", 0);

	if ( isEOF() )
		addToken(TOK_EOF, "", 0);

	return true;
}

BYTE* CScanner::findKeyword(atstring s)
{
	return (BYTE*)stristr((char*)m_p,(char*)s.c_str());
}

bool CScanner::setFN(const char* fn)
{
	m_sFN = fn;
	return true;
}

char* CScanner::stristr(char *str, char *substr)
{
	long l, x = 0, l2;

	l = strlen(substr);
	l2 = strlen(str);

	for(x=0; x<l2; x++)
	{
		if(!strcasecmp(str + x, substr) )
			return(str + x);
	}

  return(NULL);
}
