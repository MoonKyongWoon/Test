#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTextStream>

#include "chamberui.hpp"
#include "sysini.hpp"
#include "util.hpp"


bool ATChmbUi_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATCHMBUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATCHMBUI_NAME ) )
		{
			//QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running![PID:%2]").arg(ATCHMBUI_NAME).arg(pid), QMessageBox::Ok );
			//exit(-1);
			return true;
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATCHMBUI_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATCHMBUI_NAME, mypid ) )
			return false;
	}
	return true;
}


bool ATChmbCommRun()
{
    bool bRun = false;
    QString os_path = "";
    QString comm_path = "";
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
	if( UTIL::PROCESS::readPid( ATCHMBCOMM_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATCHMBCOMM_NAME ) )
		{
			printf( "%s is running!\n",  ATCHMBCOMM_NAME );
			bRun = true;
		}
	}
	if( !bRun )
	{
		comm_path = QObject::tr("%1/bin/%2").arg(os_path).arg(ATCHMBCOMM_NAME);
		bRun = UTIL::PROCESS::DetachRun( comm_path.toStdString(), QStringList{""}, pid );
		QThread::msleep( 100 );
		printf( "%s start %s!\n",  comm_path.toStdString().c_str(), bRun?"success":"error" );
		UTIL::PROCESS::writePid( ATCHMBCOMM_NAME, pid );
        //bRun = true;
	}
	return bRun;
}

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
	QApplication::setQuitOnLastWindowClosed(false);		//2023.05.22 - kwmoon( tray icon app )
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable
    QString qss = "";

	QFile f("://qss/Medize.qss");
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
	if( !ATChmbCommRun() )
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is not running!").arg(ATCHMBCOMM_NAME), QMessageBox::Ok );
		exit(-1);
	}

    if( ATChmbUi_RunCheck() )
    {
		//QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running!").arg(ATCHMBUI_NAME), QMessageBox::Ok );
        exit(-1);
    }
	CChamberUi w;
	w.setWindowFlags( w.windowFlags() & ~Qt::WindowContextHelpButtonHint);	//2023.05.22 - kwmoon

	if( 0 )
	{
		bool bRestored = true;
		QSettings settings( CONFIG_INI_KEY, app.applicationName() );
		QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() ) w.restoreGeometry( saved_geometry );
		else bRestored = false;
		QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			w.restoreState(saved_state);
		else
			bRestored = false;
		if( !bRestored ) w.resize( 1000, 700 );
	}

//	if ( !QSystemTrayIcon::isSystemTrayAvailable() )
//	{
//		QMessageBox::critical(nullptr, QObject::tr("Systray"),
//							  QObject::tr("I couldn't detect any system tray on this system."));
//		exit(0);
//	}

	app.setWindowIcon(QIcon("://icon/ablee.ico"));
	w.show();
	//w.hide();

    return app.exec();
}
