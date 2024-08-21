#ifndef CUTIL_HPP
#define CUTIL_HPP

#include <iostream>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QTcpSocket>
#include <QByteArray>
#include "Common.hpp"

namespace Ablee
{
    class CUtil
    {
    public:
        CUtil();
    public:
        static QString getCurrentDateTime(bool bBracket = true);
        static QString getCurrentDate(bool bBracket = true);
        static QString getOnlyCurrentDateTime();
        static QString getyyMMDDHHmmss();
        static bool AppendFile( QString, QString );
        static bool IsValidIpV4Format( std::string sIpV4);
    };
    class CSockErr
    {
    public:
        static QString GetSocketErrDescription( QAbstractSocket::SocketError );
    };
}

#endif // CUTIL_HPP
