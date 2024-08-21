#ifndef CPATCL_HPP
#define CPATCL_HPP

#include <vector>
#include "systype.hpp"
#include "atstring.hpp"
#include "AlpgCompiler.h"

class CPatCl
{
private:
	CAlpgCompiler*	m_pComp;
	unsigned int	m_iCompOpt;

public:
	CPatCl();
	~CPatCl();
private:
	std::vector<atstring> m_vecErrors;
public:
	void Reset();
	void SetCompOpt(unsigned int iCompOpt);
	bool SetDefine    ( atstring, std::vector<pat_line_data> );	//funcname, line data
	bool SetSubCompile( atstring, std::vector<pat_line_data> );	//subname , line data
	bool ProcNoLabelRemain();

public:
	bool         GetSubPc( atstring,int& );		//"#100", "#200" PC
	unsigned int GetMaxPc();
	int          GetSubRegister(atstring);	//need to define return type(register list)
	PGDATA*      GetPatternData();			//Compiled pattern data
public:
	unsigned int          GetErrCnt();
	std::vector<atstring> GetErrors();
//debug
public:

};

#endif // CPATCL_HPP
