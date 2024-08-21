#include "lexer.hpp"


CLexer::CLexer(void)
{
    m_type    = _CPP;
    m_buf     = NULL;
    cleanup();
    //printf("current working directory -> %s\n", cwd().c_str());
}

CLexer::~CLexer(void)
{
    cleanup();
}

//2022.07.12 - KWMOON
void CLexer::setLexerType( SOURCE_TYPE t )
{
    m_type = t;
}

//2022.07.12 - KWMOON
SOURCE_TYPE CLexer::getType()
{
    return m_type;
}

atstring CLexer::cwd()
{
    atstring sCwd = "";
    char buff[FILENAME_MAX];
    memset(buff, 0x0, sizeof(buff));
    GetCurrentDir(buff, FILENAME_MAX);
    sCwd.Format("%s", buff);
    return sCwd;
}

void CLexer::cleanup()
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

bool CLexer::loadfile(const char* fn)
{
    FILE* fp;
    atstring s;

    fp = fopen( fn, "rb");
    if ( !fp )
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
    m_buf = new Byte[m_nLength + 10];
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
    m_p = (Byte*)m_buf;
    return true;
}

token_t& CLexer::get(bool bPeek)
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

void CLexer::addToken(TOKEN_ENUM t, atstring s, int col)
{
    token_t tok;
    char *p;    //, *q;

    tok.col = col;
    tok.t   = t;

    tok.s = s.MakeUpper();
    if (t == TOK_HEX)		//prepare - maybe use
    {
        p = s;
        if(*p == '$')
            p++;
        //q = p + s.size();
        tok.n = stol(p);	//strtoul(p, &q, 16);
        tok.t = TOK_NUM;
    }
    else if (t == TOK_NUM)
    {
        p = s;
        //q = p + s.size();
        tok.n = stol(p);	//strtoul(p, &q, 16);
        tok.f = stod(p);	//strtof(p, &q);
        tok.t = TOK_NUM;
    }
    else if (t == TOK_TUNIT)
    {
		//p = s;
        //q = p + s.size();
        tok.n = col;
        tok.t = TOK_TUNIT;
    }
    m_aToken.push_back(tok);
}

bool CLexer::parseLine()
{
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
        while (!isEOF() && *m_p != CLF)
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

    Byte* p = line;

    int col = 0;
    int colsave = 0;
    while (*p )
    {
        Byte c = *p;

        if ( (c == CCR) | (c<0x20) | (c>0x7E) ) 	//ignore reference ascii table
        {
            p++;
        }
        else if (c == ' ')
        {
            col++;
            p++;
            continue;
        }
        else if (c == ',')		//skip
        {
            if( getType() == _CPP ) //cpp continue
            {
                colsave = col;
                s += *p;
                addToken(TOK_PUNC, s, colsave );
                continue;
            }
            col++;
            p++;
            continue;
        }
        else if (c == '/' && *(p+1) == '/') // should not parse after line comment
        {
            addToken(TOK_EOL, "", 0);
            break;
        }

        if (c == ';')
        {
            if( getType() == _CPP ) //cpp continue
            {
                p++;
                continue;
            }                      //pattern end of line
            break;
        }
        else if
        (
            (c == '=' && *(p+1) == '=') ||
            (c == '!' && *(p+1) == '=') ||
            (c == '>' && *(p+1) == '=') ||
            (c == '<' && *(p+1) == '=') ||
            (c == '&' && *(p+1) == '&') ||
            (c == '|' && *(p+1) == '|') ||
            (c == '+' && *(p+1) == '+') ||
            (c == '-' && *(p+1) == '-') ||
            (c == '+' && *(p+1) == '=') ||
            (c == '-' && *(p+1) == '=') ||
            (c == '<' && *(p+1) == '<') ||
            (c == '>' && *(p+1) == '>') )
        {
            colsave = col;
            s += *p++;
            s += *p++;
            col += 2;
            if( getType() == _CPP )
            {
                addToken(TOK_OPR, s, colsave);		//cpp operator
                s= "";
            }
            else
            {
                s.Format("%s not support operator!", s.c_str());
                addToken(TOK_ERROR, s, colsave);		//not support operation.
                s= "";
            }
        }
        else if (c == '_' || isalpha(c) || c == '~')
        {
            colsave = col;
            s += *p++;
            while (isalnum(*p) || *p == '_')
            {
                s += *p++;
                col++;
            }
            s.MakeUpper();					// id is case insensitive!

                 if ( s == "PS" ) addToken( TOK_TUNIT, s, EN_PS  );
            else if ( s == "NS" ) addToken( TOK_TUNIT, s, EN_NS  );
            else if ( s == "US" ) addToken( TOK_TUNIT, s, EN_US  );
            else if ( s ==  "S" ) addToken( TOK_TUNIT, s, EN_S   );
            else                  addToken( TOK_ID   , s, colsave);
            s = "";
        }
		else if ( c == '#' ) // hex digits or c/c++ macro(define/include...)
        {
            if( getType() == _CPP )
            {
				s += *p++;
				while( isalpha(*p) )
				{
					s+=*p++;
					col++;
				}
				colsave = col;
				addToken(TOK_ID, s, colsave);
				s = "";
            }
            else
            {
                colsave = col;
                s += *p++;
                s += *p++;
                col += 2;
                while ( isxdigit(*p) || *p == '`' )
                {
                    if ( isxdigit(*p) )
                        s += *p;
                    p++;
                    col++;
                }
                addToken(TOK_HEX, s, colsave);
                s = "";
            }
        }
		else if ( c == '0' && toupper(*(p+1)) == 'X' ) // hex digits
		{
			colsave = col;
			s += *p++;
			s += *p++;
			col += 2;
			while ( isxdigit(*p) || *p == '`' )
			{
				if ( isxdigit(*p) )
					s += *p;
				p++;
				col++;
			}
			addToken(TOK_HEX, s, colsave);
			s = "";
		}
        else if ( isdigit(c) ) // number
        {
            bool bDot = false;
            colsave = col;
            s += *p++;
            while ( isdigit(*p) || (*p == '.') )
            {
                s += *p;
                if (*p == '.')		//실수
                {
                    if (bDot) addToken(TOK_ERROR, s, 0);
                    bDot = true;
                }
                p++;
                col++;
            }
            addToken(TOK_NUM, s, colsave);
            s = "";
        }
        else if (ispunct(c)) {			// other
            colsave = col;
            s += *p++;
            col++;
            addToken(TOK_PUNC, s, colsave);
            s = "";
        }
        else
        {
            addToken(TOK_ERROR, "invalid character found in this line", 0);
            return false;
        }
    }

    addToken(TOK_EOL, "", 0);

    if (isEOF())
        addToken(TOK_EOF, "", 0);

    return true;
}

Byte* CLexer::findKeyword(atstring s)
{
    return (Byte*)stristr((char*)m_p,(char*)s.c_str());
}

bool CLexer::setFN(const char* fn)
{
    m_sFN = fn;
    return true;
}

char* CLexer::stristr(char *str, char *substr)
{
    long l, x = 0, l2;

    l  = strlen(substr);
    l2 = strlen(str);

    for(x=0; x<l2; x++)
    {
//        if(!_strnicmp(str + x, substr, l))
        if( !strncasecmp(str + x, substr, l))
            return(str + x);
    }

  return NULL;
}
