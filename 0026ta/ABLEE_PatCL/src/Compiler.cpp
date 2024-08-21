#include "Compiler.h"

CCompiler::CCompiler()
 :m_iCompOpt(0), m_iStartPc(0), m_iErrCnt(0)
{
}

CCompiler::~CCompiler()
{
	Init();
}

void CCompiler::Init()
{
	m_iCompOpt = 0;
	m_iStartPc  = 0;
	m_iErrCnt  = 0;
	macro_map.clear();
	m_strErr.clear();
	sub_map.clear();

	for( NoLabelList::iterator nLit=m_noLabelRemain.begin(); nLit!=m_noLabelRemain.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;

		delete nolabel;
		nolabel = NULL;
	}
	m_noLabelRemain.clear();
}

void CCompiler::SetCompOpt(unsigned int iCompOpt)
{
	m_iCompOpt = iCompOpt;
}

char *CCompiler::macro(char *word, const char *token/*=" "*/)
{
	static string	define;
	static char		szMacro[80]={0,};

	if( ! word ) word = strtok(NULL, token);

	Str2StrMap::iterator it;
	if ( word ) it =  macro_map.find( word );
	if ( word && it != macro_map.end()  )
	{
		strncpy( szMacro, it->second.c_str(), _countof(szMacro) );
		return szMacro;
	}
	return word;
}

void CCompiler::add_err(atstring strSection, uint32_t line, const char* pszMsg, const char* pszSyntax )
{
	char szErr[256];

	sprintf(szErr, "%s:%d: %s : '%s'\n", (char *)strSection, line, pszMsg, pszSyntax );
//	fprintf(stderr, "%s", szErr);

	++m_iErrCnt;
	m_strErr += szErr;
}
