#pragma once
#include "CompType.h"
#include "atstring.hpp"

class  CCompiler {
public:
	CCompiler();
	virtual ~CCompiler();

//protected:
public:
	Str2StrMap		macro_map;
	Str2WordMap		syntax_map;
	Str2WordMap		sub_map;
	Str2WordMap		burst_map;

	unsigned int	m_iCompOpt;
	unsigned int	m_iStartPc;
	unsigned int	m_iErrCnt;
	string			m_strErr;
	NoLabelList		m_noLabelRemain;	//JSR Remain

protected:
	char* macro( char *word , const char *token=" ");

public:
	void  Init();
	void  SetCompOpt(unsigned int iCompOpt);
	void  add_err(atstring strSection, uint32_t line, const char *pszMsg, const char* pszSyntax );
};
