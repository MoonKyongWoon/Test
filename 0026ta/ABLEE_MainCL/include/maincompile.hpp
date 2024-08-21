#ifndef CMAKEBUILD_HPP
#define CMAKEBUILD_HPP

#include <iostream>
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
#include "atcl_common.hpp"
#include "atstring.hpp"
#include "util.hpp"
#include "sysdef.hpp"
#include "sysver.hpp"

enum EN_BUILD_TYPE
{
	EN_BUILD_CMAKE    ,
	EN_BUILD_MAKE     ,
	EN_BUILD_CXX      ,
	EN_BUILD_TARGET   ,
};

class CMainCompile : public QObject
{
public:
    explicit CMainCompile(vector<atstring>&);
public:
    void setMainCpp(QString);
    void setDefineOpt(std::vector<atstring>);
	void setDebugOpt(bool);
	void setDevFileExt(QString);
	bool cxxCompile();

public slots:
    void SlotCMakeErrorOccurred(QProcess::ProcessError);
private:
	bool makeBuildDir();
    bool makeCMakeFile();
    bool doCMake();
    bool doMake( QStringList parameter );
	bool isTargetInstalled();
	void addBuildError( EN_BUILD_TYPE, EN_RESULT, const char* fmt, ... );
private:
	bool m_bDebugOpt;
    QString m_sRunDir;
    QString m_sBuildDir;
    QString m_sMainCpp;
    QString m_sMainDev;
    std::vector<atstring> m_vecDefine;
    std::vector<atstring>& m_rVecError;
    std::vector<atstring> m_vecMacros;
};

#endif // CMAKEBUILD_HPP
