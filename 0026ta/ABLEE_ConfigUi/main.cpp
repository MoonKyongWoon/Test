#include "config.hpp"
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

#include "sysini.hpp"
#include "util.hpp"
#include "sysver.hpp"

bool g_IsMan = false;

bool ATSetupUi_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATSETUPUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATSETUPUI_NAME ) )
		{
			//QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running![PID:%2]").arg(ATCHMBUI_NAME).arg(pid), QMessageBox::Ok );
			//exit(-1);
			return true;
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATSETUPUI_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATSETUPUI_NAME, mypid ) )
			return false;
	}
	return true;
}

void ParseCommandLine(QApplication& app )
{
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	//manual option
	QCommandLineOption manualOption(QStringList() << "m" << "man", QCoreApplication::translate("manual", "Manual mode run"));
	parser.addOption(manualOption);
	parser.process(app);
	g_IsMan = parser.isSet( manualOption );
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable
	ParseCommandLine( app );

	app.setWindowIcon(QIcon(":/icon/icon/ablee.ico"));
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
	if( argc < 1 )
	{
		fprintf( stderr, "Check parameter program name!\n" );
		exit(-1);
	}
	if( ATSetupUi_RunCheck() )
	{
		//QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is already running!").arg(ATCHMBUI_NAME), QMessageBox::Ok );
		exit(-1);
	}

	CConfig w;
	w.show();
	return app.exec();
}

