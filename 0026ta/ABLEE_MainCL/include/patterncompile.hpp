#ifndef PATTERNCOMPILE_HPP
#define PATTERNCOMPILE_HPP

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <atstring.hpp>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QFileInfoList>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <unistd.h>
#include "cpplexer.hpp"
#include "atcl_common.hpp"
#include "atstring.hpp"
#include "systype.hpp"
#include "patcl.hpp"

class CPatternCompile
{
public:
    explicit CPatternCompile(vector<atstring>&);

public:
	void setMainCpp(QString);
	void setDefineOpt(std::vector<atstring>);
	void setDebugOpt(bool);
	void initSyntaxValid();
	void makeSyntaxMap();
	void viewCDefine();
	bool compile();
	bool addCDefine( atstring ,atstring );
	bool isCDefine( atstring );
	bool removeCDefine( atstring );
	bool addSDefine ( int, atstring );
	bool viewSDefine( atstring );
	bool isFunctionDefine();
	bool isCallPatternFunction(atstring);
private:
	void init();
	void addPatError  ( EN_RESULT, atstring, const char* fmt, ... );
	void addBuildError( EN_RESULT, const char* fmt, ... );
	void printBuildMsg( EN_RESULT, const char* fmt, ... );
	bool doCompile();
	bool doCMacro();
	bool doLetter(atstring);
	bool doSetDefineCompile(atstring);	//function name
	bool doSetSubCompile(atstring, bool);	//function name
	bool doCompileFinish( atstring );
private:
	QString m_sCpp;
    QString m_sRunDir;
    QString m_sBuildDir;
	uint32_t m_nDefineFlag;		//Define flag A~Z
	bool m_bDebugOpt;
private:
	atstring m_sFN;
	std::vector<atstring>& m_rVecError;
	CCppLexer m_lexer;
	CPatCl m_patcl;
private:
	devsyntax m_devsyntax;
	Str2WordMap m_mapSyntax;
	std::vector<cppdefine> m_veccdefine;	//#define

private:
	std::map< atstring, std::vector<pat_line_data> > m_mapsdefine;	//SDEF
	std::map< atstring, std::vector< std::pair<atstring, std::vector<pat_line_data> > > > m_mapsub;		//SET_SUB
	std::map< atstring, std::vector< std::pair<atstring, std::vector<pat_line_data> > > > m_mappatgroup;	//PATTERN
};

#endif // PATTERNCOMPILE_HPP
