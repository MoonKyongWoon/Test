#pragma once
#ifdef _WIN32
	#include <direct.h>
	#define GetCurrentDir _getcwd
	typedef unsigned char	byte;
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
	typedef unsigned char	BYTE;
#endif

#include "atstring.hpp"
#include <stack>
#include <vector>
#include <map>
#include <queue> 

using namespace std;

#define CHAR_CR			0xD
#define CHAR_LF			0xA
#define TAB_SIZE		4


typedef unsigned int	dword;

enum token_enum
{
	TOK_EOF		= 0,	// end of file
	TOK_EOL,			// end of line
	TOK_LT ,			//
	TOK_GT ,			//
	TOK_TYPE,			// Stream/Function & Format Code
						// i.e.) STRINGs
	TOK_ID,				// identifier that leading alphabet, and then alphabet or number follows
						// i.e.) STRINGs
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_NUM,
	TOK_PUNC,			//ETC
	TOK_ERROR = -1	// error during parse text
};

enum compileError_enum 
{
	CE_ERROR = 0,
	CE_WARN     ,
};

typedef struct tokenStruct {
	token_enum	t;		// token type
	atstring	s;		// string value
	dword		n;		// numeric value
	double		f;		// real value
	int			col;	// column position of this token
} token_t;

class CScanner
{
public:
	CScanner(void);
	virtual ~CScanner(void);

public:
	bool		loadfile(const char* fn);
	bool		loadsml (const char* fn);
	bool		isEOF() { return (m_p >= m_buf + m_nLength); }
	bool		isEOL() { return (*m_p == CHAR_LF); }
	bool		parseLine();
	bool		setFN(const char* fn);
	char*		stristr(char *str, char *substr);
	atstring&	gets(bool bPeek = false) { return get(bPeek).s; }
	atstring&	getFN() { return m_sFN; }
	token_enum	gett(bool bPeek = false) { return get(bPeek).t; }
	BYTE*		findKeyword(atstring s);
	dword		getn(bool bPeek = false) { return get(bPeek).n; }
	token_t&	get(bool bPeek = false);
	token_t&	peek() { return get(true); }
	int			getLine() { return m_nLine; }
	void		setLine(int nline) { m_nLine = nline; }
	void		cleanup();
	void		addToken(token_enum t, atstring s, int col);
	atstring	cwd();
private:
	int			m_nLine;			// current line
	bool		m_bInCommentBlock;	
	atstring	m_sFN;				// filename
	size_t		m_nTokenIndex;		// current token index of token array
	size_t		m_nLength;			// total length of the source
	BYTE*		m_buf;				// source buffer
	BYTE*		m_p;				// current buffer pointer
		
	vector<token_t> m_aToken;
};
