#include <QApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QTextStream>
#include <QTranslator>
#include <QSharedMemory>
#include <QString>
#include <QMessageBox>
#include "sysmonui.hpp"


bool ATSysMonUi_RunCheck()
{
    qint64 pid = 0;
    qint64 mypid = QCoreApplication::applicationPid();
    if( UTIL::PROCESS::readPid( ATSYSMONUI_NAME, pid ) )
    {
        if( UTIL::PROCESS::IsRun( pid, ATSYSMONUI_NAME ) )
        {
            QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running![PID:%2]").arg(ATSYSMONUI_NAME).arg(pid), QMessageBox::Ok );
            exit(-1);
        }
        else
        {
            if( UTIL::PROCESS::writePid( ATSYSMONUI_NAME, mypid ) )
                return false;
        }
    }
    else
    {
        if( UTIL::PROCESS::writePid( ATSYSMONUI_NAME, mypid ) )
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable

    ATSysMonUi_RunCheck();

	app.setWindowIcon(QIcon(":/icon/icon/ablee.ico"));
	QString qss = "";
	//QFile f("://qss/WordOffice_m.qss");
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
	if( argc < 1 )
	{
		fprintf( stderr, "Check parameter program name!\n" );
		exit(-1);
	}
	CSysMonUi monui;
	monui.show();
	return app.exec();
}
