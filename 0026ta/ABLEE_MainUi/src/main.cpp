#include "mainui.hpp"
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTranslator>
#include <QSharedMemory>
#include <QString>
#include <QMessageBox>
#include "mainui.hpp"
#include "util.hpp"

bool ATMain_RunCheck( bool bAutoMode )
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATMAIN_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATMAIN_NAME ) )
		{
			QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running![PID:%2]").arg(ATMAIN_NAME).arg(pid), QMessageBox::Ok );
			exit(-1);
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATMAIN_NAME, mypid ) )
			{
				return false;
			}
		}
	}
	else
	{
		if( UTIL::PROCESS::IsRun( ATMAIN_NAME, "") )
		{
			printf( "%s is already running!\n",  ATMAIN_NAME );
			exit(-1);
		}
		if( UTIL::PROCESS::writePid( ATMAIN_NAME, mypid ) )
			return false;
	}
	return true;
}

bool ATMgrRun()
{
    bool bRun = false;
    QString os_path = "";
    QString mgr_path = "";
    char* env_os  = getenv(ATOS_DIR_ENV);
    if( env_os != NULL )
    {
        os_path = QString(env_os);
    }
    else
    {
		os_path = DIR_ATOS_PATH;
    }

	qint64 pid = 0;
	if( UTIL::PROCESS::readPid( ATMGR_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATMGR_NAME ) )
		{
			printf( "%s is running!\n",  ATMGR_NAME );
			bRun = true;
		}
	}

	if( !bRun )
	{
		mgr_path = QObject::tr("%1/bin/%2").arg(os_path).arg(ATMGR_NAME);
		bRun = UTIL::PROCESS::DetachRun( mgr_path.toStdString(), QStringList{""}, pid );
		QThread::msleep( 10 );
		printf( "%s start %s!\n",  mgr_path.toStdString().c_str(), bRun?"success":"error" );
        //UTIL::PROCESS::writePid( ATMGR_NAME, pid );
        //bRun = true;
	}
	return bRun;
}

bool ATTMSUiRun()
{
	bool bRun = false;
	QString os_path = "";
	QString mgr_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}

	qint64 pid = 0;
	if( UTIL::PROCESS::readPid( ATTMSUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATTMSUI_NAME ) )
		{
			printf( "%s is running!\n",  ATTMSUI_NAME );
			bRun = true;
		}
	}

	if( !bRun )
	{
		mgr_path = QObject::tr("%1/bin/%2").arg(os_path).arg(ATTMSUI_NAME);
		bRun = UTIL::PROCESS::DetachRun( mgr_path.toStdString(), QStringList{""}, pid );
		QThread::msleep( 10 );
		printf( "%s start %s!\n",  mgr_path.toStdString().c_str(), bRun?"success":"error" );
		//UTIL::PROCESS::writePid( ATMGR_NAME, pid );
		//bRun = true;
	}
	return bRun;
}

bool ATChmbUiRun()
{
	bool bRun = false;
	QString os_path = "";
	QString chmbui_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}

	qint64 pid = 0;

    chmbui_path = QObject::tr("%1/bin/%2").arg(os_path).arg(ATCHMBUI_NAME);
    bRun = UTIL::PROCESS::DetachRun( chmbui_path.toStdString(), QStringList{""}, pid );
	QThread::msleep( 10 );
    printf( "%s start %s!\n",  chmbui_path.toStdString().c_str(), bRun?"success":"error" );
    return bRun;
}


bool ATSysmonUiRun()
{
    bool bRun = false;
    QString os_path = "";
    QString chmbui_path = "";
    char* env_os  = getenv(ATOS_DIR_ENV);
    if( env_os != NULL )
    {
        os_path = QString(env_os);
    }
    else
    {
        os_path = DIR_ATOS_PATH;
    }

    qint64 pid = 0;

    chmbui_path = QObject::tr("%1/bin/%2").arg(os_path).arg(ATSYSMONUI_NAME);
    bRun = UTIL::PROCESS::DetachRun( chmbui_path.toStdString(), QStringList{""}, pid );
    QThread::msleep( 10 );
    printf( "%s start %s!\n",  chmbui_path.toStdString().c_str(), bRun?"success":"error" );
    return bRun;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable

	bool bAutoMode = true;
	if( argc > 1 )
	{
		for (int i = 1; i < argc; i++)
		{
			if ( strncasecmp(argv[i], "-man"  , 4) == 0 )
			{
				bAutoMode  = false;
			}
			else if ( strncasecmp(argv[i], "/man"  , 4) == 0 )
			{
				bAutoMode  = false;
			}			
			//else if ( strncasecmp(argv[i], "-auto"  , 4) == 0 )
			//{
			//	bAutoMode  = true;
			//}
		}
	}
	ATMain_RunCheck( bAutoMode );

	//ATMGR	
	int nExecCount = 0;
	do
	{
		if( ATMgrRun() ) break;
		nExecCount++;
	}while( nExecCount<5);

	if( nExecCount >= 5 )
	{
		QMessageBox::critical( 0, app.applicationName(), QObject::tr("%1 is not running or execute fail").arg(ATMGR_NAME), QMessageBox::Ok );
		return -1;
	}

	//ATTMS UI - 2024.06.20 - KWMOON
	if( bAutoMode )
	{
		int nExecCount = 0;
		do
		{
			if( ATTMSUiRun() ) break;
			nExecCount++;
		}while( nExecCount<5);

		if( nExecCount >= 5 )
		{
			QMessageBox::critical( 0, app.applicationName(), QObject::tr("%1 is not running or execute fail").arg(ATTMSUI_NAME), QMessageBox::Ok );
			return -1;
		}
	}

    QString qss = "";
	QFile f(":/stylesheet/qss/Medize.qss");
	if ( !f.exists() )
    {
        fprintf( stderr, "Unable to set stylesheet, file not found\n" );
    }
    else
    {
        f.open( QFile::ReadOnly | QFile::Text );
        QTextStream ts(&f);
        qss = ts.readAll();
        app.setStyleSheet(qss);
    }
    app.setWindowIcon(QIcon(":/icon/icon/ablee.ico"));

    MainWindow w;
    bool bRestored = true;
	if( 1 )
    {
        QSettings settings( CONFIG_INI_KEY, app.applicationName() );
        QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() ) w.restoreGeometry( saved_geometry );
        else bRestored = false;
        QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			w.restoreState(saved_state);
		else
			bRestored = false;
		qDebug() << settings.fileName();
        if( !bRestored ) w.resize( 1280, 1024 );
	}
	w.SetAutoMode( bAutoMode );
	w.setMinimumSize( 1024, 768 );
    w.show();

	//if( bAutoMode )
	{
		//ATCHAMBER_UI
		if( !ATChmbUiRun() )
		{
			QMessageBox::critical( 0, app.applicationName(), QObject::tr("%1 is not running or execute fail").arg(ATCHMBUI_NAME), QMessageBox::Ok );
		}

        //ATSYSMON_UI
        if( !ATSysmonUiRun() )
        {
            QMessageBox::critical( 0, app.applicationName(), QObject::tr("%1 is not running or execute fail").arg(ATSYSMONUI_NAME), QMessageBox::Ok );
        }
	}
    return app.exec();
}
