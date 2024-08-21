#ifndef MAINCL_HPP
#define MAINCL_HPP

#include <iostream>
#include <queue>
#include <map>
#include <stack>
#include <thread>
#include <vector>

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QtXml>

#include "atstring.hpp"
#include "string.h"
#include "atcl_common.hpp"
#include "maincompile.hpp"
#include "patterncompile.hpp"

class CAbleeCompiler
{
public:
    CAbleeCompiler(void);
    virtual ~CAbleeCompiler(void);
public:
    int run(int, char*[]);
private:
    void usage();
    void version();
	void LoadExtension();
    void init();
    void removeBuildDir();
    void initErrorLog();
    void createsyntax();
    void compile();
    void printWarnErrMsg ( EN_RESULT, const char* szFmt, ... );
    void addMakeListError( EN_RESULT, const char* szFmt, ... );
    void displayMakeListError();
    bool SourceExistCheck();
    bool isCpp(QString);
private:
    bool createCompilerInstance();
private:
    bool m_bDefineOpt;
    atstring m_sProgramName;
    atstring m_sSourceFile;
    atstring m_sExtension;
private:
    std::vector<atstring> m_vecDefine;
    std::vector<atstring> m_vecMakeListError;
    std::vector<atstring> m_vecMainError;
    std::vector<atstring> m_vecPatError;
private:
    CMainCompile* m_pMainCompiler;
    CPatternCompile* m_pPatCompiler;
};

#endif
