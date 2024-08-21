#ifndef CINIHANDLER_HPP
#define CINIHANDLER_HPP

#include <QSettings>
#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <string>
#include "Common.hpp"

class CIniHandler
{
public:
    explicit CIniHandler(QString file);
private:
    QMutex m_mutex;
    QString m_file;
public:
    bool writeSettings(QString, int);
    bool writeSettings(QString, QString);
    bool readSettings(QString, uint&);
    bool readSettings(QString, int&);
    bool readSettings(QString, QString&);
    bool readSettings(QString, std::string&);
    bool readSettings(QString, double&);
    bool removeSection(QString);
    bool existSection(QString);
    QStringList allKeys();
};

#endif // CINIHANDLER_HPP
