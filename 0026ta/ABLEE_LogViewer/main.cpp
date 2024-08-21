#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTextStream>

#include "logviewer.hpp"
#include "sysini.hpp"
#include "sysver.hpp"
#include "util.hpp"

bool LogViewer_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATLOGVIEWUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATLOGVIEWUI_NAME ) )
		{
			return true;
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATLOGVIEWUI_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATLOGVIEWUI_NAME, mypid ) )
			return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable
	app.setWindowIcon(QIcon(":/icon/icon/ablee.ico"));

	QString qss = "";
	QFile f(":/qss/qss/Medize.qss");
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

	if( LogViewer_RunCheck() )
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is running!").arg(ATLOGVIEWUI_NAME), QMessageBox::Ok );
		exit(-1);
	}

	CLogViewer w;
	w.setWindowFlags( w.windowFlags() & ~Qt::WindowContextHelpButtonHint);	//2023.05.22 - kwmoon
	w.resize( 1024, 768);
	w.show();

	return app.exec();
}
