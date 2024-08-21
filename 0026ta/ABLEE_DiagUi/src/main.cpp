#include "mainwindow.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTranslator>
#include <QSharedMemory>
#include <QString>
#include <QMessageBox>

bool g_IsMan = false;

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
	//QFile f(":/stylesheet/qss/Medize.qss");
	QFile f(":/stylesheet/qss/MailSy.qss");
	//QFile f(":/stylesheet/qss/ExcelOffice.qss");
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
	CDiagUi w( argv[0] );
	w.resize( QSize(1152,768) );
    w.ResizeWidgets();
	w.show();
	return app.exec();
}
