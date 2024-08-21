#ifndef LEXER_HPP
#define LEXER_HPP

#ifdef _WIN32
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif

#include "atstring.hpp"
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <map>
#include <queue> 

using namespace std;

#define CCR (0x0D)
#define CLF (0x0A)
#define TAB_SIZE 4

typedef unsigned char	Byte;
typedef unsigned int	Dword;

enum SOURCE_TYPE
{
    _CPP   = 0x0,
    _ASC        ,
};

enum EN_TIME_UNIT
{
    EN_PS = 0       ,
    EN_NS           ,
    EN_US           ,
    EN_MS,
    EN_S,
};

enum TOKEN_ENUM
{
    TOK_EOF		= 0,	// end of file
    TOK_EOL,			// end of line
    TOK_NUM,			// numeric
    TOK_HEX,			// hex, decimal
    TOK_FLOAT,          //float
    TOK_ID,				// identifier that leading alphabet, and then alphabet or number follows
                                            // i.e.) "SDEF", "TIMESET", "BCLK", "CCLK"
    TOK_OPR,			// token is puncture character include double operator
                                            // i.e.) "++" ,"+=", "*", "^", "=",  etc
    TOK_PUNC,			//ETC
    TOK_TUNIT,			//PS, NS, US, S
    TOK_ERROR = -1      // error during parse text
};

enum compileError_enum 
{
    CE_ERROR = 0,
    CE_WARN     ,
};

typedef struct tokenStruct
{
    TOKEN_ENUM	t;		// token type
    atstring    s;		// string value
    Dword	n;      	// numeric value
    double	f;          // real value
    int		col;    	// column position of this token
} token_t;

class CLexer
{
public:
    CLexer(void);
    virtual ~CLexer(void);

public:
	bool		loadfile(const char* fn);
	bool		isEOF() { return (m_p >= m_buf + m_nLength); }
    bool		isEOL() { return (*m_p == CLF); }
	bool		parseLine();
	bool		setFN(const char* fn);
    void        setLexerType( SOURCE_TYPE );
	char*		stristr(char *str, char *substr);
    atstring&	gets(bool bPeek = false) { return get(bPeek).s; }
    atstring&	getFN() { return m_sFN; }
    TOKEN_ENUM	gett(bool bPeek = false) { return get(bPeek).t; }
    Byte*		findKeyword(atstring s);
    Dword		getn(bool bPeek = false) { return get(bPeek).n; }
	token_t&	get(bool bPeek = false);
	token_t&	peek() { return get(true); }
	int			getLine() { return m_nLine; }
	void		setLine(int nline) { m_nLine = nline; }
	void		cleanup();
    void		addToken(TOKEN_ENUM t, atstring s, int col);
    atstring	cwd();
private:
    SOURCE_TYPE getType();
private:
	int			m_nLine;			// current line
	bool		m_bInCommentBlock;	
    atstring	m_sFN;				// filename
	size_t		m_nTokenIndex;		// current token index of token array
	size_t		m_nLength;			// total length of the source
    Byte*		m_buf;				// source buffer
    Byte*		m_p;				// current buffer pointer
		
	vector<token_t> m_aToken;
    SOURCE_TYPE m_type;
};

#endif
