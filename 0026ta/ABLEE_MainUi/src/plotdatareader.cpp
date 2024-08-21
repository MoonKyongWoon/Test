#include "plotdatareader.hpp"
#include "syslog.hpp"
#include "sysini.hpp"

CPlotDataReader::CPlotDataReader(QObject *parent) : QObject(parent), read_line(0) {

}

void CPlotDataReader::setTemperatureDat(QString dat)
{
	m_sDatFile = dat;
}

void	CPlotDataReader::slotReadData() {
	QFile *log_file = new QFile(m_sDatFile);
	QByteArray data;
	if (!log_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		return ;
	}
	else {
		for (int i = 0; i < read_line; i++) {
			log_file->readLine();
		}
		std::vector<QString> datas;
		while (!log_file->atEnd()) {
			data=log_file->readLine();
			datas.push_back(QString(data));
			read_line += 1;
		}
		emit sigFinishReadData(datas);
	}
	log_file->close();
}

void	CPlotDataReader::slotInitReadData() {
	this->read_line = 0;
}


CPlotDataReaderController::CPlotDataReaderController(QObject *parent) : QObject(parent) {
	Init();
	this->data_reader = new CPlotDataReader;
	this->timer = new QTimer;
	this->x_axis_unit = AXIS_SEC;
//    qDebug() << "bf --- this->data_reader->moveToThread(&dataReaderThread);";
    this->data_reader->moveToThread(&dataReaderThread);
//    qDebug() << "af --- this->data_reader->moveToThread(&dataReaderThread);";

    connect(&dataReaderThread, &QThread::finished, data_reader, &QObject::deleteLater);
	connect(this->timer, &QTimer::timeout, this->data_reader, &CPlotDataReader::slotReadData);
	connect(this->data_reader, &CPlotDataReader::sigFinishReadData, this, &CPlotDataReaderController::slotFinishReadData);
	connect(this->data_reader, &CPlotDataReader::sigNoTempFile, this, &CPlotDataReaderController::slotStopTimer);
	connect(this, &CPlotDataReaderController::sigInitReadLine, this->data_reader, &CPlotDataReader::slotInitReadData);
	connect(this, &CPlotDataReaderController::sigInitReadLine, this, &CPlotDataReaderController::slotStopTimer);
	data_reader->setTemperatureDat( m_sDatFile );
    dataReaderThread.start();
}

void CPlotDataReaderController::Init()
{
	char* env_data  = getenv(ATDATA_DIR_ENV);
	if( env_data != NULL )
	{
		m_sDatFile = tr("%1/%2/%3").arg(env_data).arg(DIR_TEMPERATURE).arg(DAT_TEMPERATURE);
	}
	else
	{
		m_sDatFile = tr("%1/%2/%3").arg(DIR_ATDATA_PATH).arg(DIR_TEMPERATURE).arg(DAT_TEMPERATURE);
	}
}

void CPlotDataReaderController::setXAxisUnit(X_AXIS_UNIT unit) {
	this->x_axis_unit = unit;
}

X_AXIS_UNIT CPlotDataReaderController::getXAxisUnit() {
	return this->x_axis_unit;
}

QString CPlotDataReaderController::getDatFileName()
{
	return m_sDatFile;
}

CPlotDataReaderController::~CPlotDataReaderController() {
	dataReaderThread.quit();
	dataReaderThread.wait();
}

void	CPlotDataReaderController::slotStartTimer() {
	this->timer->start(5000);
}

void	CPlotDataReaderController::SlotDataError() {
	emit sigDataError();
}

void	CPlotDataReaderController::slotStopTimer() {
	this->timer->stop();
}

void	CPlotDataReaderController::slotFinishReadData(std::vector<QString> datas) {
	emit sigSendReadData(datas);
}

void	CPlotDataReaderController::slotInitReadLine() {
	emit sigInitReadLine();
}
