#include "PLCSimulator.hpp"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QSharedMemory>
#include <QTranslator>
#include <QTextStream>
#include "IniHandler.hpp"

CIniHandler* g_pIniPLC;        //Global Instance

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable

    qRegisterMetaType<EN_TCP_CONN>("EN_TCP_CONN");
    qRegisterMetaType<qintptr>("qintptr");

    QString qss = "";
    QFile f(":/qss/Ubuntu.qss");
    //QFile f(":/qss/Aqua.qss");
    //QFile f(":/qss/Integrid.qss");
    if ( !f.exists() )
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qss = ts.readAll();
        a.setStyleSheet(qss);
    }

    g_pIniPLC = new CIniHandler(CONFIG_INI_FILE);

    //MainWindow Instance
    a.setWindowIcon( QIcon(":/icon/corona.ico") );

    CPLCSimulator w;
    w.show();
    return a.exec();
}
