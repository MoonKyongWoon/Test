#include <QApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTextStream>

#include "mainwindow.hpp"
#include "sysini.hpp"
#include "sysver.hpp"
#include "util.hpp"

bool TmsUi_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATTMSUI_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATTMSUI_NAME ) )
		{
			return true;
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATTMSUI_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATTMSUI_NAME, mypid ) )
			return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable
	app.setWindowIcon( QIcon(":/icon/icon/ablee.ico") );

	QString qss = "";
	//QFile f(":/qss/qss/MailSy.qss");
	QFile f(":/qss/qss/Ubuntu.qss" );
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

	if( TmsUi_RunCheck() )
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is running!").arg(ATTMSUI_NAME), QMessageBox::Ok );
		exit(-1);
	}

	CMainWindow w;
	w.setWindowFlags( w.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	bool bRestored = true;
	if( 1 )
	{
		QSettings settings( CONFIG_INI_KEY, ATTMSUI_NAME );
		QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() ) w.restoreGeometry( saved_geometry );
		else bRestored = false;
		QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			w.restoreState(saved_state);
		else
			bRestored = false;
//		qDebug() << settings.fileName();
		if( !bRestored ) w.resize( 800, 600);
	}
	w.show();

	return app.exec();
}
