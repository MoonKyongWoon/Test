#ifndef PLOTDATAREADER_HPP
#define PLOTDATAREADER_HPP

#include <QObject>
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <vector>

//#define	LOG_FILE_PATH	"/nvme/chamber/temperature.dat"

enum X_AXIS_UNIT {
	AXIS_SEC,
	AXIS_MIN,
	AXIS_HOUR,
};

class CPlotDataReader : public QObject
{
	Q_OBJECT
public:
	explicit CPlotDataReader(QObject *parent = nullptr);

private:
	int		read_line;
	QString m_sDatFile;

public:
	void    setTemperatureDat(QString);

signals:
	void	sigFinishReadData(std::vector<QString> datas);
	void	sigNoTempFile();

public slots:
	void	slotReadData();
	void	slotInitReadData();
};

class CPlotDataReaderController : public QObject{
	Q_OBJECT
	QThread dataReaderThread;

private:
	CPlotDataReader	*data_reader;
	QTimer			*timer;
	QString			m_sDatFile;
	X_AXIS_UNIT		x_axis_unit;

private:
	void Init();
public:
	CPlotDataReaderController(QObject *parent = nullptr);
	~CPlotDataReaderController();

public:
	QString getDatFileName();
	void		setXAxisUnit(X_AXIS_UNIT unit);
	X_AXIS_UNIT	getXAxisUnit();

signals:
	void	sigSendReadData(std::vector<QString> datas);
	void	sigInitReadLine();
	void    sigDataError();

public slots:
	void	slotFinishReadData(std::vector<QString> datas);
	void	slotStartTimer();
	void	slotStopTimer();
	void	SlotDataError();
	void	slotInitReadLine();
};


#endif // PLOTDATAREADER_HPP
