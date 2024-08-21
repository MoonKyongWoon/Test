#pragma once

#include "Compiler.h"
#include "systype.hpp"

class  CAlpgCompiler : public CCompiler
{
public:
	CAlpgCompiler();
	~CAlpgCompiler();

//patcl Interface
public:
	void Reset();
    bool SetDefine    ( atstring, std::vector<pat_line_data> );	//funcname, line data
    bool SetSubCompile( atstring, std::vector<pat_line_data> );	//subname , line data
	bool ProcNoLabelRemain();

	void create_syntax();

public:
	PGDATA pattern;

public:
	void Init();

protected:
	bool isPreValid ( char  );
	bool isPostValid( char );

	void replace_ds      ( string&, const char *, const char * );	//dual space
	void replace_PrimeReg( string&, const char *, const char * );	//D1A, D2A, ND1, BD1
	void replace_Hex     ( string&, const char *, const char * );	//2023.06.13 - kwmoon(add fatric)	//"0x" -> " 0x"
	void replace_DsetReg ( string&, const char *, const char * );	//DSET REG Valid(Decimal operand only)
	void replace_Dbmap   ( string&, const char *, const char * );	//DBMAP<digit3, DBMAP<DBMAP+digit3, DBMAP<DBMAP-digit3
	void replace_Addr    ( string&, const char *, const char * );	//XB<digit1, XB<XB+digit1, XB<XB-digit1, YB<digit2, YB<YB+digit2, YB<YB-digit2
																	//XC, XS, XK, YC, YS, YK

	int  ParseIFE      ( atstring  strExpr );
	int  GetIdxCompCond( atstring& strCond );
	bool PreprocIFE    ( atstring&, std::vector<pat_line_data>& );	//subname , line data
	bool PreprocSDEF   ( atstring&, std::vector<pat_line_data>& );	//subname , line data
	bool PreprocDsetReg( atstring&, std::vector<pat_line_data>& );	//subname , line data
	bool PreprocOutMode( atstring&, std::vector<pat_line_data>& );	//subname , line data

	bool ParsePattern( atstring&, std::vector<pat_line_data>& );	//subname , line data

};
