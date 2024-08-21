#include "maincompile.hpp"

CMainCompile::CMainCompile(vector<atstring>& rv) : m_rVecError(rv)
{
    m_sRunDir   = QDir::currentPath();
    m_sBuildDir = QObject::tr("%1/%2").arg(m_sRunDir).arg(BUILD_DIR);
     qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
}

//////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainCompile::compile
/// \return
/// \details main c++ compile method. CMakeLists.txt -> cd .build ->cmake .. -> make -j 10
bool CMainCompile::cxxCompile()
{
    if( makeBuildDir() )
    {
        if( makeCMakeFile() )
        {
            if( doCMake() )
            {
                QStringList args={CXX_CLEAN,};
                if( doMake( args ) )
                {
					QStringList args= {"-j", "4"};
                    if( doMake( args ) )
                    {
                        QStringList args={CXX_INSTALL};
                        if( doMake( args ) )
                        {
							if( !isTargetInstalled() ) return false;
							fprintf( stdout, COLOR_GREEN "%s target installed ----- success!\n" COLOR_NC, m_sMainDev.toStdString().c_str() );
							return true;
                        }
                        else
                        {
							addBuildError( EN_BUILD_TARGET, EN_RESULT::EN_ERROR, COLOR_RED "Do make install error!" COLOR_NC );
                        }
                    }
                    else
                    {
						addBuildError( EN_BUILD_CXX, EN_RESULT::EN_ERROR, COLOR_RED "Do make compile error!" COLOR_NC );
                    }
                }
                else
                {
					addBuildError( EN_BUILD_CXX, EN_RESULT::EN_ERROR, COLOR_RED "Do make clean error!" COLOR_NC );
                }
            }
            else
            {
				addBuildError( EN_BUILD_CMAKE, EN_RESULT::EN_ERROR, COLOR_RED "Do cmake error!" COLOR_NC );
            }
        }
        else
        {
			addBuildError( EN_BUILD_CMAKE, EN_RESULT::EN_ERROR, COLOR_RED "Generate cmake file error!" COLOR_NC );
        }
    }
    else
    {
		addBuildError( EN_BUILD_CXX, EN_RESULT::EN_ERROR, COLOR_RED "Create build directory error!" COLOR_NC );
    }
    return false;
}

void CMainCompile::setMainCpp(QString maincpp)
{
    m_sMainCpp = maincpp;
    int posdot = maincpp.indexOf(".");
    if( posdot <= 0 )
		m_sMainDev = maincpp + DEFAULT_DEVEXT;
    else
		m_sMainDev = maincpp.left(posdot) + DEFAULT_DEVEXT;
}

void CMainCompile::setDevFileExt(QString extension)
{
    int posdot = m_sMainCpp.indexOf(".");
    if( posdot <= 0 )
	{
        m_sMainDev = m_sMainCpp + "." + extension;
	}
    else
	{
        m_sMainDev = m_sMainCpp.left(posdot) + extension;
	}
	fprintf( stdout, COLOR_GREEN "%s extension(%s)\n" COLOR_NC, m_sMainCpp.toStdString().c_str(), extension.toStdString().c_str() );
}

void CMainCompile::setDefineOpt(std::vector<atstring> macros)
{
    m_vecMacros = macros;
}

void CMainCompile::setDebugOpt(bool debug)
{
	m_bDebugOpt = debug;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief CMainCompile::makeBuildDir
/// \return .build directory exists? true : false
/// \details create build directory and remove all directory/files in build dir
bool CMainCompile::makeBuildDir()
{
    QDir dir(m_sBuildDir);
    if( dir.exists() )
    {
        dir.removeRecursively();
    }
    dir.mkdir( dir.absolutePath() );
    if( !dir.exists() )
        return false;
    return true;

    //remove all file in build directory
    //QFileInfoList entrylist = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files );
    //for( QFileInfo info : entrylist )
    //{
    //    if( info.isDir() )
    //    {
    //        QDir dir( info.absoluteFilePath() );
    //        dir.removeRecursively();
    //    }
    //    else if( info.isFile() )
    //    {
    //        QFile::remove( info.absoluteFilePath() );
    //    }
    //}
    //qDebug() << "--- make BuildDir Done : "  << dir.exists();
    //
    //if( dir.exists() ) return true;
    //else               return false;
}

///////////////////////////////////////////////////////////////////////////
/// \brief CMainCompile::makeCMakeFile
/// \return
/// \details CMakeLists.txt create at build directory
bool CMainCompile::makeCMakeFile()
{
    QDir dir(m_sBuildDir);
    dir.setCurrent( dir.absolutePath() );

    QFile CMakeListTxt(CMAKELIST_TXT);
    if( !CMakeListTxt.open(QIODevice::WriteOnly | QIODevice::Text ) )
    {
		addBuildError( EN_BUILD_CMAKE, EN_RESULT::EN_ERROR, "file create error!" );
        return false;
    }
    QTextStream out(&CMakeListTxt);
	out << "cmake_minimum_required(VERSION 3.1.11)"                                             << "\n";
	out << "if ( NOT DEFINED ENV{ATOS_DIR} ) "                                                  << "\n";
	out << "    message( FATAL_ERROR \"Please set $ATOS_DIR system environment variable!!\")"   << "\n";
	out << "endif()"                                                                            << "\n";
	out << "project( \"" << m_sMainDev << "\" )"                                                << "\n";
	out << "set( TARGET ${PROJECT_NAME} )"                                                      << "\n";
	out << "set( LIB_PATH $ENV{ATOS_DIR}/lib )"                                                 << "\n";
	out << "set( INC_PATH $ENV{ATOS_DIR}/include )"                                             << "\n";

	out << "set( CMAKE_INCLUDE_CURRENT_DIR ON )"                                                << "\n";
	out << "set( CMAKE_CXX_STANDARD 17 )"                                                       << "\n";
	out << "set( CMAKE_VERBOSE_MAKEFILE OFF )"                                                  << "\n";
	out << "set( CMAKE_CXX_FLAGS_DEBUG   \"-g  -fopenmp\" )"                                    << "\n";
	out << "set( CMAKE_CXX_FLAGS_RELEASE \"-O2 -fopenmp\" )"                                    << "\n";
//	out << "set( CMAKE_CXX_FLAGS_RELEASE \"-g -O2 -fopenmp\" )"                                    << "\n";
	out << "set( CMAKE_BUILD_TYPE Release ) "                                                   << "\n";
	out << "set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/.. )"                         << "\n";
    for( unsigned int m=0;m<m_vecMacros.size();m++ )
    {
        QString sMacro = m_vecMacros.at(m).c_str();
		out << "add_compile_definitions(" << sMacro << ")"                                                                                                  << "\n";
	}
	out << "find_package(Qt5Core REQUIRED)"                                                                                                                 << "\n";
	out << "find_package(Qt5Network REQUIRED)"                                                                                                              << "\n";
	out << "set(QT5_LIBRARIES Qt5::Core Qt5::Network )"                                                                                                     << "\n";
	out << "set(SOURCE ../" << m_sMainCpp << " )"                                                                                                           << "\n";
	out << "include_directories( ${INC_PATH}/lexer  ${INC_PATH}/pgc ${INC_PATH}/sys ${INC_PATH}/tcp ${INC_PATH}/tool )" << "\n";
	out << "link_directories( ${LIB_PATH} )"                                                                                                                << "\n";
	out << "link_libraries(testlib)        "                                                                                                                << "\n";
	out << "add_compile_options( -g -Wall -Wextra -fPIC -fopenmp -Wno-write-strings -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare )"  << "\n";
	out << "add_executable( ${TARGET} ${SOURCE} ${HEADERS} )  "                                                                                             << "\n";
	out << "target_link_libraries( ${TARGET} ${QT5_LIBRARIES} )"                                                                                            << "\n";
	out << "install ( TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX} ) "                                                      << "\n";
	out << "install ( TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${CMAKE_INSTALL_PREFIX} ) "                                                      << "\n";
    CMakeListTxt.close();
    //qDebug() << "--- Generate CMakeList.txt done";
    return true;
}

void CMainCompile::SlotCMakeErrorOccurred(QProcess::ProcessError pe)
{
    qDebug() << __FUNCTION__ << " ExitCode : " << pe;
}

//////////////////////////////////////////////////////////////////////
/// \brief CMainCompile::doCMake
/// \return
/// \details "cmake .. " execute in .build directory & check Makefile created.
bool CMainCompile::doCMake()
{
    QDir dir(m_sBuildDir);
    QProcess cmake;
    QStringList args = { CMAKELIST_TXT };

    dir.setCurrent( dir.absolutePath() );

    connect( &cmake, &QProcess::errorOccurred , this, &CMainCompile::SlotCMakeErrorOccurred );
    cmake.setWorkingDirectory(dir.absolutePath());
    cmake.start( QString(CMAKE), args );    //, QStringList() << parameter );
    if( !cmake.waitForStarted() )
    {
		addBuildError( EN_BUILD_CMAKE, EN_ERROR, "cmake start error!!" );
        return false;
    }

    int nWait = 0;
    do
    {
        if( cmake.waitForFinished(1000) ) break;
        if( cmake.state() == QProcess::Running ) continue;
    }while( ++nWait < 10 );
    if( nWait >= 10 )
    {
		addBuildError( EN_BUILD_CMAKE, EN_ERROR, "Generate build file error!" );
        return false;
    }
    int nExitCode = cmake.exitCode();
    if( nExitCode != 0 )
    {
        QByteArray ba = cmake.readAllStandardError();
        QString s = QString(ba.toStdString().c_str());
        auto parts = s.split("\n");
        for( QString part : parts )
        {
			addBuildError( EN_BUILD_CMAKE, EN_ERROR, "%s", part.toUtf8().constData() );
        }
    }
    //qDebug() << cmake.readAllStandardOutput();
    if( QFile::exists(CXXMAKEFILE) ) return true;
    else                             return false;
}

//////////////////////////////////////////////////////////////////////
/// \brief CMainCompile::doMake
/// \return
/// \details make execute in .build dir
bool CMainCompile::doMake( QStringList params )
{
    QDir dir(m_sBuildDir);
    dir.setCurrent( dir.absolutePath() );

    QProcess make;
    make.setWorkingDirectory(dir.absolutePath());
    make.start( QString(CXXCOMPILE), params );
    if( !make.waitForStarted() )
    {
		addBuildError( EN_BUILD_MAKE, EN_ERROR, "compile execute error!" );
        return false;
    }

    int nWait = 0;
    do
    {
        if( make.waitForFinished(3000) ) break;
        if( make.state() == QProcess::Running ) continue;
    }while( ++nWait < 10 );
    if( nWait >= 10 )
    {
		addBuildError( EN_BUILD_MAKE, EN_ERROR, "compile execute error!" );
        return false;
    }
    //qDebug() << "Make Return Code : " << make.exitCode();
    int nExitCode = make.exitCode();
    if( nExitCode != 0 )
    {
        QByteArray ba = make.readAllStandardError();
        QString s = QString(ba.toStdString().c_str());
        auto parts = s.split("\n");
        for( QString part : parts )
        {
			addBuildError( EN_BUILD_CXX, EN_ERROR, "%s", part.toUtf8().constData() );
        }
    }
    //std::cout << ba.toStdString();

    if( QFile::exists(CXXMAKEFILE) ) return true;
    else                             return false;
}

bool CMainCompile::isTargetInstalled()
{
	QString sFileDev = QString("%1/%2").arg(m_sRunDir).arg(m_sMainDev);
	if( QFile::exists(sFileDev) ) return true;
	else                          return false;
}


void CMainCompile::addBuildError( EN_BUILD_TYPE type, EN_RESULT code, const char *szFmt, ... )
{
    char cBuff[1024 * 10];
    va_list argptr;
    va_start(argptr, szFmt);
    vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
    va_end(argptr);

	atstring sType = "";
	switch( type )
	{
		case EN_BUILD_CMAKE : sType = CMAKELIST_TXT; break;
		case EN_BUILD_MAKE  : sType = CXXMAKEFILE  ; break;
		case EN_BUILD_CXX   : sType = m_sMainCpp.toStdString(); break;
		case EN_BUILD_TARGET: sType = m_sMainDev.toStdString(); break;
		default:              sType = m_sMainCpp.toStdString(); break;
	}

	(void)code;
	atstring msg;
	if( code == EN_RESULT::EN_ERROR )
		msg.Format( COLOR_RED       " >%s : %s" COLOR_NC, sType.c_str(), cBuff );
	if( code == EN_RESULT::EN_WARNING )
		msg.Format( COLOR_LIGHT_RED " >%s : %s" COLOR_NC, sType.c_str(), cBuff );
	else
		msg.Format( COLOR_NC        " >%s : %s" COLOR_NC, sType.c_str(), cBuff );

	m_rVecError.push_back(msg);
}
