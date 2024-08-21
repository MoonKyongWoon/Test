#ifndef CCPPLEXER_HPP
#define CCPPLEXER_HPP
#include <QDebug>
#include <QFile>
#include <QString>
#include "atstring.hpp"

enum TOKEN_CPP
{
	TOKEN_EOL            ,
	TOKEN_EOF            ,
	TOKEN_LETTER         ,
	TOKEN_DIGIT          ,
	TOKEN_SHARP          ,
	TOKEN_PUNC           ,
	TOKEN_LPARENTHESIS   ,	//(
	TOKEN_RPARENTHESIS   ,	//)
	TOKEN_LBRACE         ,	//{
	TOKEN_RBRACE         ,	//}
	TOKEN_SPACE          ,	//Space/Tab
	TOKEN_CDATATYPE      ,
	TOKEN_OPERATOR       ,	//++, --
	TOKEN_ERROR          ,	//Unknown
};

typedef struct _cppstruct
{
	TOKEN_CPP   t;
	atstring    s;      //line string
	uint32_t    l;     //line number
} token_cpp;

typedef struct _cppdefine
{
	atstring key;
	atstring value;
} cppdefine;

typedef unsigned char	Byte;
typedef unsigned int	Dword;

class CCppLexer
{
public:
    CCppLexer();
	bool LoadData( atstring, int, char* );
	bool LoadFile( atstring );
	bool ParseLine( QByteArray& );
	bool Tokenize( QByteArray&, int );
	bool TokenizeSplit( QByteArray&, int );
	token_cpp& Get(bool bPeek=false);
private:
	atstring m_sFN;
	bool m_bInCommentBlock;
	uint32_t m_nCurrent;
	std::vector<token_cpp> m_vecToken;
	Byte* m_buf;
	Byte* m_p;
};

#endif // CCPPLEXER_HPP
