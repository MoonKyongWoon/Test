#include "downloader.hpp"

CDownloader::CDownloader(QObject *parent)
	: QObject(parent)
{
	m_bFtpInfo  = false;
	m_bCompress = false;
	m_pNetMgr = nullptr;
	InitDir();
	//InitFtp();
}

CDownloader::~CDownloader()
{
	if( m_pNetMgr )
	{
		delete m_pNetMgr;
		m_pNetMgr = nullptr;
	}
}

void CDownloader::InitDir()
{
	char* env_run  = getenv(ATRUN_DIR_ENV);
	QString run_dir = "";
	if( env_run == NULL )
	{
		run_dir = "/home/ablee/atsystem";	//QDir::home().absolutePath();
	}
	else
	{
		run_dir = QString(env_run);
	}
	m_sDownloadDir = tr("%1/%2").arg(run_dir).arg(DIR_DOWN_PATH);
	QDir dir(m_sDownloadDir);
	if( !dir.exists( m_sDownloadDir)  )	dir.mkdir( m_sDownloadDir );
}

void CDownloader::InitFtp()
{
	if( m_pNetMgr )
	{
		delete m_pNetMgr;
		m_pNetMgr = nullptr;
	}
	m_pNetMgr = new QNetworkAccessManager( this );
	//m_pNetMgr->setNetworkAccessible( QNetworkAccessManager::Accessible );
}

void CDownloader::SetLotId(QString sLotId)
{
	m_sLotId = sLotId;
}

void CDownloader::SetDownloadDir(QString downloadDir)
{
	m_sDownloadDir += tr("/%1").arg(downloadDir);
	QDir dir(m_sDownloadDir);
	if( !dir.exists( m_sDownloadDir) )	dir.mkpath( m_sDownloadDir );
}

void CDownloader::SetFtpInfo(QString ip, quint16 port, QString id, QString passwd )
{
	m_sFtpHost = ip;
	m_sFtpId = id;
	m_sFtpPasswd = passwd;
	m_uFtpPort = port;
	m_bFtpInfo = true;
}

void CDownloader::SetDownloadFileInfo( QString path, QString file, qint64 size, bool bCompress )
{
    QString abspath = tr("/%1/%2").arg(path).arg(file);
	m_sFtpPath  = abspath;
	m_sFileName = file;
	m_nFileSize = size;
	m_bCompress = bCompress;
}

void CDownloader::SlotDownloadFile()
{
	InitFtp();

	if( !m_bFtpInfo )
	{
		m_sErrMsg = "No FTP connection information.";
		emit SigDownloadErrorReport( m_sLotId, m_sErrMsg );
		return;
	}
	m_pNetMgr->clearConnectionCache();

	QString sDownFile = tr("%1/%2").arg(m_sDownloadDir).arg(m_sFileName);
	QUrl url;
	url.setScheme("ftp");
	url.setHost( m_sFtpHost );
	url.setPath( m_sFtpPath );
	url.setPort( m_uFtpPort );
	url.setUserName( m_sFtpId );
	url.setPassword( m_sFtpPasswd );

	QNetworkRequest request(url);

	m_pFile = new QFile( sDownFile );
	m_pFile->open( QIODevice::WriteOnly );
	if( m_pFile->isOpen() )
	{
		m_pReply = m_pNetMgr->get(request);
		connect( m_pReply , &QNetworkReply::readyRead         , this, &CDownloader::SlotReadyRead        );
		connect( m_pReply , &QNetworkReply::downloadProgress  , this, &CDownloader::SlotDownloadProgress );
		connect( m_pReply , &QNetworkReply::errorOccurred     , this, &CDownloader::SlotDownloadError    );
		connect( m_pReply , &QNetworkReply::finished          , this, &CDownloader::SlotReplyFinished    );
	}
	else
	{
		m_sErrMsg = "FTP download start failed!";
		emit SigDownloadErrorReport( m_sLotId, m_sErrMsg );
	}
}

void CDownloader::SlotUploadFile()
{

}

void CDownloader::SlotReadyRead()
{
	m_pFile->write( m_pReply->readAll() );
}

void CDownloader::SlotReplyFinished()
{
	QString sPgmFilePath = tr("%1/%2").arg(m_sDownloadDir).arg(m_sFileName);

	InitDir();

	if( m_pReply->error() != QNetworkReply::NoError )
	{
		emit SigDownloadResult( false, m_sLotId, sPgmFilePath );
		m_pFile->remove();
		return;
	}
	m_pReply->deleteLater();
	m_pFile->flush();
	m_pFile->close();
	m_pFile->deleteLater();
	emit SigDownloadResult( true, m_sLotId, sPgmFilePath );

	if( m_pNetMgr )
	{
		delete m_pNetMgr;
		m_pNetMgr = nullptr;
	}
}

bool CDownloader::FileSizeCheck()
{
	QString sDownFile = tr("%1/%2").arg(m_sDownloadDir).arg(m_sFileName);
	QFileInfo info(sDownFile);
	if( info.size() != m_nFileSize ) return false;
	return true;
}

void CDownloader::SlotDownloadError(QNetworkReply::NetworkError error)
{
	Q_UNUSED(error)
	m_sErrMsg = m_pReply->errorString();
	emit SigDownloadErrorReport( m_sLotId, m_sErrMsg );
}

void CDownloader::SlotDownloadProgress(qint64 bytesProcess,qint64 bytesTotal)
{
	double dProcess = static_cast<double>(bytesProcess);
	double dTotal   = static_cast<double>(bytesTotal);
	int percent = static_cast<int>((dProcess/dTotal)*100.0);
	emit SigDownloadProgress( percent );
}
