#ifndef UTIL_HPP
#define UTIL_HPP

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QTcpSocket>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include "syslog.hpp"
#include "sysini.hpp"
#include "protocol.hpp"

namespace UTIL
{
    class DATE
    {
    public:
        static QString yyyyMMDDHHmmsszzz( bool bracket = false );
        static QString yyyyMMDDHHmmss   ( bool bracket = false );
        static QString yyMMDDHHmmss     ( bool bracket = false );
        static QString yyyyMMDD         ( bool bracket = false );
        static QString yyMMDD           ( bool bracket = false );
    };
    class FILE
    {
    public:
        static bool AppendFile( QString, QString, bool bNl=true);
        static bool WriteFile ( QString, QString, bool bNl=true);
    };

    class VALIDATION
    {
    public:
        static bool IsValidIpV4( std::string );
        static bool IsExistDir ( QString );
        static bool IsExistFile( QString );
        static bool IsExecutable( QString );
    };

	class TCPIP
    {
    public:
        static QString GetSocketErrDescription(QAbstractSocket::SocketError);
		static QString CMD_STR(PROTOCOL::TCP_COMMAND);
		static std::string GetHostName();
		static QString SOURCE_STR(PROTOCOL::SOURCE_TYPE);	//2023.05.09 - KWMOON
	};

    class INI
    {
    public:
		static bool writeSettings(QString, QString, const QVariant, bool sync=true);
		static bool writeSettings(QString, QString, QStringList, QStringList);
        static bool readSettings (QString, QString, QVariant&);
        static bool readSettings (QString, QString, uint&);
        static bool readSettings (QString, QString, int&);
        static bool readSettings (QString, QString, QString&);
        static bool readSettings (QString, QString, std::string&);
        static bool readSettings (QString, QString, double&);
		static bool readSettings (QString, QString, bool&);
        static QStringList getAllGroups(QString);
        static QStringList getAllKeys(QString,QString);
        static QStringList getAllKeys(QString,std::string);
		static void syncIni(QString);
	public:
		static bool writeSettings(std::string, std::string, const double, bool sync=true);
		static bool writeSettings(std::string, std::string, const int, bool sync=true);
		static bool writeSettings(std::string, std::string, std::string, bool sync=true);
		static bool readSettings (std::string, std::string, double&);
		static bool readSettings (std::string, std::string, int&);
		static bool readSettings(std::string, std::string, std::string&);
	public:
		static bool removeSection( QString, QString );
		static bool removeKey( QString, QString, QString );

	};

	class _SIGNAL
    {
    public:
        static std::string GetSigName( int signum );
    };

	//2022.10.28 - TIMER Appended by fatric.
	class TIMER
	{
	private:
		std::chrono::steady_clock::time_point   begin;
		double									setTime;
	public:
		TIMER();
		TIMER(double dSec);
		void	setTimer(double dSec);
		void	start();
		bool	isEnd();
		double	getElapsedTime();
	};

	//2023.01.02 - KWMOON
	class PROCESS
	{
	public:
		static bool IsRun    ( long mypid, std::string prg , std::string arg="");
		static bool IsRun    ( std::string prg , std::string arg="");
        static bool DetachRun( std::string prg, QStringList args, qint64& pid);
		static bool Kill     ( std::string prg , std::string arg="");
		static bool Kill     ( qint64 pid);
		static bool IsProcessRun( std::string atprg );
	public:
		static bool writePid( QString atname, const QVariant pid );	//2023.07.10 - kwmoon
		static bool readPid ( QString atname, qint64& pid );	//2023.07.10 - kwmoon
	};
	//2023.01.02 - KWMOON
	class CHAMBER
	{
	public:
		static QString PLCCMD_STR (PROTOCOL::PLC_COMMAND);
		static QString PLCADDR_STR(PLC_ADDRESS_MAP);
	};

	//2024.08.06 - fatric
	class FTP
	{
	public:
		static int PUT (const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFile , const char *pszMode);
		static int GET (const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFile , const char *pszMode);
		static int MPUT(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFiles, const char *pszMode);
		static int MGET(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFiles, const char *pszMode);
	};

};
#endif // UTIL_HPP
