/*H**********************************************************************
* FILENAME : downloader.hpp
*
* DESCRIPTION :
*       Device Program FTP Downloader
*
* CHANGES :
* DATE          VER       AUTHOR    DETAIL
* 2023/01/03    v1.0.0    kwmoon    first write
*
*H*/

#ifndef CDOWNLOADER_HPP
#define CDOWNLOADER_HPP

#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "syslog.hpp"
#include "sysdef.hpp"
#include "sysver.hpp"
#include "util.hpp"

class CDownloader : public QObject
{
	Q_OBJECT

public:
	CDownloader(QObject *parent = nullptr);
	~CDownloader();
private:
	void InitFtp();
	void InitDir();
	bool FileSizeCheck();
public:
	void SetFtpInfo(QString, quint16, QString, QString );
	void SetDownloadFileInfo( QString, QString, qint64, bool );
signals:
	void SigDownloadProgress(int);
	void SigDownloadResult(bool, QString);
public slots:
	void SlotDownloadFile();
	void SlotUploadFile();
private slots:
	void SlotReadyRead();
	void SlotReplyFinished();
	void SlotDownloadError(QNetworkReply::NetworkError);
	void SlotDownloadProgress(qint64 bytesSent,qint64 bytesTotal);
private:
	bool m_bFtpInfo;
	QString m_sFtpHost;
	QString m_sFtpId;
	QString m_sFtpPasswd;
	quint16 m_uFtpPort;
	QString m_sFtpPath;
	QString m_sFileName;
	QString m_sDownloadDir;
	qint64  m_nFileSize;
	bool    m_bCompress;
private:
	QFile* m_pFile;
	QNetworkAccessManager* m_pNetMgr;
	QNetworkReply*  m_pReply;
	QString m_sErrMsg;
};

#endif
