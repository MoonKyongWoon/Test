#include "atcl.hpp"


CAbleeCompiler::CAbleeCompiler()
{
    init();
}

CAbleeCompiler::~CAbleeCompiler()
{
    if( m_pMainCompiler )
    {
        delete m_pMainCompiler;
        m_pMainCompiler = nullptr;
    }
    if( m_pPatCompiler )
    {
        delete m_pPatCompiler;
        m_pPatCompiler = nullptr;
    }
}

void CAbleeCompiler::usage()
{
    printf(" %s usage : \n", m_sProgramName.c_str() );
    printf(" %s {file.cpp} -d {DEFINE} -ext {.extension} -h \n", m_sProgramName.c_str());
    printf(" -d      ----- define option for main(.cpp) \n");
    printf(" -ext    ----- file extension. default (.dev) \n");
	printf(" -g      ----- debug mode.\n");
	printf(" -h      ----- show usage.\n");
	printf(" -m      ----- Main C++ Compile\n");
	printf(" -p      ----- Pattern Compile\n");
	printf(" -v      ----- show version.\n");
}

void CAbleeCompiler::version()
{
    atstring BuildDate = "";
    BuildDate.Format("%04d-%02d-%02d", BUILD_YEAR, BUILD_MONTH, BUILD_DAY);
    printf("%s - %s [Build:%s]\n", m_sProgramName.c_str(), ATCL_VERSION, BuildDate.c_str() );
}

void CAbleeCompiler::LoadExtension()
{
	char* env_os  = getenv(ATOS_DIR_ENV);
	QString os_path = "";
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini_file    = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(SYSTEM_INI);
	QString section_key = QObject::tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_FILE_EXTENSION);
	m_sExtension        = DEFAULT_DEVEXT;
	if( !UTIL::INI::readSettings( ini_file, section_key, m_sExtension ) )
	{
		m_sExtension = DEFAULT_DEVEXT;
	}
	if( m_sExtension.Trim().Left(1) != "." )
		m_sExtension = "." + m_sExtension.Trim();

}

void CAbleeCompiler::init()
{
    QString current = QDir::currentPath();
    QDir::setCurrent( current );

    m_pMainCompiler = nullptr;
    m_pPatCompiler  = nullptr;

    m_bDefineOpt  = false;
    m_vecDefine.clear();

	m_sExtension = DEFAULT_DEVEXT;

    removeBuildDir();
    initErrorLog();
    createCompilerInstance();
	LoadExtension();
}

void CAbleeCompiler::removeBuildDir()
{

}

void CAbleeCompiler::initErrorLog()
{
    m_vecMakeListError.clear();
    m_vecMainError.clear();
    m_vecPatError.clear();
}

bool CAbleeCompiler::createCompilerInstance()
{
    bool bMain = false, bPat = false;
    m_pMainCompiler = new CMainCompile   ( m_vecMainError );
    m_pPatCompiler  = new CPatternCompile( m_vecPatError  );
    if( m_pMainCompiler != nullptr ) bMain = true;
    if( m_pPatCompiler  != nullptr ) bPat  = true;
    return ( bMain && bPat );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CMainCL::run
/// \param argc
/// \param argv
/// \return
/// \details atcl -c mpsb -Ddefine
/// \details Makelist.txt ini file reference
int CAbleeCompiler::run( int argc, char* argv[] )
{
    bool bFileExt = false;
    bool bSource  = false;
	bool bDebugOpt   = false;
	bool bPatternOpt = false;
	bool bMainOpt    = false;

	atstring sDefineOpt  = "";

    m_sProgramName = argv[0];
    for (int i = 1; i < argc; i++)
    {
		bool bDefineOpt = false;
		bool bVersionOpt = false;
             if ( strncasecmp(argv[i], "-D"  , 2) == 0 ) { bDefineOpt  = true; sDefineOpt   = argv[++i];  }
        else if ( strncasecmp(argv[i], "-EXT", 4) == 0 ) { bFileExt    = true; m_sExtension = argv[++i];  }
        else if ( strncasecmp(argv[i], "-G"  , 2) == 0 ) { bDebugOpt   = true; }
		else if ( strncasecmp(argv[i], "-M"  , 2) == 0 ) { bMainOpt    = true; }
		else if ( strncasecmp(argv[i], "-P"  , 2) == 0 ) { bPatternOpt = true; }
		else if ( strncasecmp(argv[i], "-V"  , 2) == 0 ) { bVersionOpt = true; }
		else if ( strchr( argv[i], '.' ) != NULL )
        {
            if( bSource )
            {
                printWarnErrMsg( EN_ERROR, "already define source file '%s'", m_sExtension.c_str() );
                exit(0);
            }
            bSource = true;
            m_sSourceFile = atstring( argv[i++] );
        }
        else
        {
            usage();
            exit(-1);
        }

        if( bDefineOpt )
        {
            m_bDefineOpt = true;
            m_vecDefine.push_back( sDefineOpt );
        }
        else if( bVersionOpt )
        {
            version();
            exit(0);
        }
    }
	if( bFileExt )
	{
		if( m_sExtension.Left(1) != "." )
			m_sExtension = "." + m_sExtension;
	}
    if( !bSource )
    {
        printWarnErrMsg( EN_ERROR, "Please set target program(*.cpp)." );
        exit(-1);
    }

    QString sCpp = QString( m_sSourceFile.c_str() );
    if( !isCpp( sCpp ) )
    {
        printWarnErrMsg( EN_ERROR, "Only cpp files can be compiled, but use '%s'", m_sSourceFile.c_str() );
        exit(-1);
    }

	if( bDebugOpt )
	{
		m_pMainCompiler->setDebugOpt( true );
		m_pPatCompiler->setDebugOpt( true );
		m_pMainCompiler->setDefineOpt( m_vecDefine );
		m_pPatCompiler->setDefineOpt( m_vecDefine );
	}

	if( m_vecDefine.size() > 0 )
	{
		for( atstring sdef : m_vecDefine )
		{
			fprintf( stdout, "EXTERN DEFINE : %s\n", sdef.c_str() );
		}
	}

    if( SourceExistCheck() )
    {
        initErrorLog();
        compile();
    }

    if ( m_vecMakeListError.size() > 0 ) displayMakeListError();
    return 0;
}

bool CAbleeCompiler::isCpp(QString src)
{
    int posdot = src.indexOf(".cpp", Qt::CaseSensitivity::CaseInsensitive );
    if( posdot <= 0 )
        return false;
    return true;
}

bool CAbleeCompiler::SourceExistCheck()
{
    bool bOk = true;
    QString sFile( m_sSourceFile.c_str() );
    if( !QFile::exists( sFile ) )
    {
        addMakeListError( EN_ERROR, "'%s' is not exist!", sFile.toStdString().c_str() );
        bOk = false;
    }
    return bOk;
}

void CAbleeCompiler::compile()
{
	//Main Compile
    QString cpp = QString( m_sSourceFile.c_str() );
    QString ext = QString( m_sExtension.c_str()  );

    m_pMainCompiler->setMainCpp( cpp );
	m_pMainCompiler->setDevFileExt( ext );
    m_pMainCompiler->setDefineOpt( m_vecDefine );

	fprintf( stdout, COLOR_GREEN "%s compile start!\n" COLOR_NC, m_sSourceFile.c_str() );
	thread t1( &CMainCompile::cxxCompile, m_pMainCompiler );
	t1.join();

	if( m_vecMainError.size() > 0 )
	{
		for( atstring errmsg : m_vecMainError )
		{
			printf("%s\n", errmsg.c_str() );
		}
		fprintf( stderr, COLOR_RED "%s compile end ----- error!\n" COLOR_NC, m_sSourceFile.c_str() );
		return;
	}
	fprintf( stdout, COLOR_GREEN "%s compile end ----- success!\n" COLOR_NC, m_sSourceFile.c_str() );

	fprintf( stdout, COLOR_GREEN "%s pattern compile start!\n" COLOR_NC, m_sSourceFile.c_str() );

	m_pPatCompiler->setMainCpp( cpp );
	thread t2( &CPatternCompile::compile, m_pPatCompiler  );
	t2.join();
	if( m_vecPatError.size() > 0 )
	{
		for( atstring errmsg : m_vecPatError )
		{
			printf("%s\n", errmsg.c_str() );
		}
		fprintf( stderr, COLOR_RED "%s pattern compile end ----- error!\n" COLOR_NC, m_sSourceFile.c_str() );
		return;
	}
	fprintf( stdout, COLOR_GREEN "%s pattern compile end!\n" COLOR_NC, m_sSourceFile.c_str() );
}

void CAbleeCompiler::printWarnErrMsg( EN_RESULT code, const char *szFmt, ... )
{
    char cBuff[1024 * 10];
    va_list argptr;
    va_start(argptr, szFmt);
    vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
    va_end(argptr);

    atstring t;
    if( code == EN_RESULT::EN_WARNING )
        t.Format(COLOR_LIGHT_RED "> %s : warn  %s\n" COLOR_NC, m_sSourceFile.c_str(), cBuff);
    else
        t.Format(COLOR_RED "> %s : error %s\n" COLOR_NC, m_sSourceFile.c_str(), cBuff);
    fprintf( stderr, "%s", t.c_str() );
}

void CAbleeCompiler::addMakeListError( EN_RESULT code, const char *szFmt, ... )
{
    char cBuff[1024 * 10];
    va_list argptr;
    va_start(argptr, szFmt);
    vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
    va_end(argptr);

    atstring t;
    if( code == EN_RESULT::EN_WARNING )
        t.Format(COLOR_LIGHT_RED "> %s : warn  %s\n" COLOR_NC, m_sSourceFile.c_str(), cBuff);
    else
        t.Format(COLOR_RED "> %s : error %s\n" COLOR_NC, m_sSourceFile.c_str(), cBuff);
    m_vecMakeListError.push_back(t);
}

void CAbleeCompiler::displayMakeListError()
{
    for ( unsigned int nErr = 0; nErr < m_vecMakeListError.size(); nErr++ )
    {
		fprintf( stderr, "%s", m_vecMakeListError[nErr].c_str());
    }
}

/*
//////////////////////////////////////////////////////////////////////
/// \brief CMainCL::parseMakefileXml
/// \return
/// \details MakeList.xml parsing
/// \details make main(c++)/patterns/socket/bib(scan) list
bool CAbleeCompiler::parseMakeListXml()
{
    bool bOk = true;
    char szCWD[FILENAME_MAX];
    memset(szCWD, 0x0, sizeof(szCWD));
    getcwd(szCWD, FILENAME_MAX);

    if( access(MAKELIST_XML, F_OK ) != 0 )
    {
        addMakeListError( EN_ERROR, "The file does not exist. First create a file %s", MAKELIST_XML );
        return false;
    }

    QDomDocument doc( "MakeList" );
    QFile file( MAKELIST_XML) ;
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if ( !doc.setContent(&file) )
    {
        addMakeListError( EN_ERROR, "Please check the syntax and contents of the file!" );
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    //qDebug() << docElem.tagName();
    if(  docElem.tagName() != ELEMENT_DOCUMENT )
    {
        addMakeListError( EN_ERROR, "Element in the %s must start with 'COMPILE', but %s", MAKELIST_XML, docElem.tagName().toUtf8().constData() );
        return false;
    }

    QDomNode dn = docElem.firstChild();
    std::vector<atstring>* pVector = nullptr;
    while( !dn.isNull() )
    {
        QDomElement e = dn.toElement();
        if( !e.isNull() )
        {
            QString build_type = e.tagName();       //MAIN/PATTERN/SOCKET/BIB

                 if( build_type == ELEMENT_MAIN    ) pVector = &m_vecFiles[EN_MAIN];
            else if( build_type == ELEMENT_PATTERN ) pVector = &m_vecFiles[EN_PATTERN];
            else if( build_type == ELEMENT_SOCKET  ) pVector = &m_vecFiles[EN_SOCKET];
            else if( build_type == ELEMENT_BIB     ) pVector = &m_vecFiles[EN_BIB];

            QDomNodeList items = e.elementsByTagName(ELEMENT_FILE); // <FILE></FILE>
            for( int n=0;n<items.count();n++ )
            {
                QDomNode    dnode = items.at(n);
                QDomElement delem = dnode.toElement();
                if( !delem.isNull() )
                {
                    pVector->push_back( delem.text().trimmed().toStdString() );
                }
            }
        }
        dn = dn.nextSibling();
    }
    return bOk;
}

    //xml example
    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    qDebug() << docElem.tagName();
    QDomNode n = docElem.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( !e.isNull() )
        {
            qDebug() << e.tagName(); // the node really is an element.
            QDomNodeList items = e.elementsByTagName("FILE");
            for( int n=0;n<items.count();n++ )
            {
                QDomNode node = items.at(n);
                QDomElement enode = node.toElement();
                qDebug() << " --- " << node.nodeName() <<  " : " << enode.text();
            }
        }
        n = n.nextSibling();
    }
*/
