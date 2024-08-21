#include "plotchamber.hpp"
#include "ui_plotchamber.h"


CPlotChamber::CPlotChamber(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CPlotChamber) {

	ui->setupUi(this);

	qRegisterMetaType<std::vector<QString>>("std::vector<QString>");

	this->curve_pv = new QwtPlotCurve("PV curve");
	this->curve_sv = new QwtPlotCurve("SV curve");
	this->drc = new CPlotDataReaderController(this);

	connect(this, &CPlotChamber::sigStart, this->drc, &CPlotDataReaderController::slotStartTimer);
	connect(this, &CPlotChamber::sigStop , this->drc, &CPlotDataReaderController::slotStopTimer );
	connect(this, &CPlotChamber::sigReset, this->drc, &CPlotDataReaderController::slotInitReadLine );
	connect(this->drc, &CPlotDataReaderController::sigSendReadData, this, &CPlotChamber::slotAddData);
	connect(this->drc, &CPlotDataReaderController::sigDataError, this, &CPlotChamber::slotDataError);

	initPlot();
}

CPlotChamber::~CPlotChamber() {
    if (this->curve_pv) delete this->curve_pv;
    if (this->curve_sv) delete this->curve_sv;
	this->ui->qwtPlot->detachItems(QwtPlotItem::Rtti_PlotCurve, true);
	this->ui->qwtPlot->detachItems(QwtPlotItem::Rtti_PlotGrid, true);

	if (this->drc) delete this->drc;
	delete ui;
}

void    CPlotChamber::initPlot()
{
	initQwt();
	initLcd();
}

void	CPlotChamber::initQwt() {

	// set Config
	this->ui->qwtPlot->setAutoReplot(true);

	// set Y axis
	QwtScaleDiv y_scale_div(-55, 155);
	QList<double> tick_values;
	tick_values << -50 << -25 << 0 << 25 << 50 << 75 << 100 << 125 << 150;
	y_scale_div.setTicks(QwtScaleDiv::MajorTick, tick_values);
	this->ui->qwtPlot->setAxisScaleDiv(QwtPlot::yLeft, y_scale_div);
	this->ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "temperature");

	// set Grid
	this->grid = new QwtPlotGrid();
	this->grid->enableXMin(true);

	// set Pen for Grid
	QPen pen;
	pen.setColor(Qt::gray);
	pen.setStyle(Qt::DotLine);
	grid->setPen(pen);
	grid->attach(this->ui->qwtPlot);

	// set Pen for Graph
	this->curve_pv->setPen(GREEN, 2);
	this->curve_sv->setPen(ORANGE, 2);

	// set Picker
//	this->picker = new QwtPicker(this->ui->qwtPlot);
//	this->picker->setStateMachine(new QwtPickerDragPointMachine());
//	this->picker->setTrackerMode(QwtPicker::ActiveOnly);
//	this->picker->setRubberBand(QwtPicker::RectRubberBand);
//	this->picker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
//	this->picker = new QwtPlotPicker(this->ui->qwtPlot->xBottom, this->ui->qwtPlot->yLeft, QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, this->ui->qwtPlot->canvas());
	this->picker = new QwtPlotPicker(this->curve_pv->xAxis(), this->curve_pv->yAxis(), QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, this->ui->qwtPlot->canvas());
	this->machine = new QwtPickerClickPointMachine();
	this->picker->setStateMachine(this->machine);

	resetQwt();
}

void	CPlotChamber::initLcd() {
	this->ui->lcdNumber_pv->setStyleSheet("color: #00cc00; background-color: #000000;");
	this->ui->lcdNumber_sv->setStyleSheet("color: #e78200; background-color: #000000;");
	this->ui->lcdNumber_pv->setDigitCount(6);
	this->ui->lcdNumber_sv->setDigitCount(6);
	this->ui->lcdNumber_pv->setSmallDecimalPoint(false);
	this->ui->lcdNumber_sv->setSmallDecimalPoint(false);
	resetLcd();
}

void	CPlotChamber::resetPlot() {
	resetQwt();
	resetLcd();
}

void	CPlotChamber::resetQwt() {
	this->running_time = 0;
	this->is_start_line = true;
	this->drc->setXAxisUnit(AXIS_SEC);

	this->x_axis_range = 60;
	this->x_axis_max = 600;
	this->x_axis_increase = 1;
	this->ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, x_axis_range);
	this->ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "sec");

	this->points_pv.clear();
	this->points_sv.clear();
	this->curve_pv->setSamples(points_pv);
	this->curve_pv->attach(this->ui->qwtPlot);
	this->curve_sv->setSamples(points_sv);
	this->curve_sv->attach(this->ui->qwtPlot);
	this->ui->qwtPlot->update();	
}

void	CPlotChamber::resetLcd() {
	this->ui->lcdNumber_pv->display(0);
	this->ui->lcdNumber_sv->display(0);
}

bool	CPlotChamber::getChamberEnable() {
	return this->ui->checkBox_chamber_enable->isChecked();
}

void	CPlotChamber::setChamberEnable(bool flag) {
	this->ui->checkBox_chamber_enable->setChecked(flag);
}

void	CPlotChamber::setChamberState(int state) {
	switch(state) {
		case 0:
			this->ui->label_run->setStyleSheet("");
			this->ui->label_amb->setStyleSheet("");
			this->ui->label_stop->setStyleSheet("color: #ff0000; border-style: solid; border-width: 1px; border-color: #ff0000; border-radius: 5%; ");
			break;
		case 1:
			this->ui->label_run->setStyleSheet("color: #00CC00; border-style: solid; border-width: 1px; border-color: #00CC00; border-radius: 5%; ");
			this->ui->label_amb->setStyleSheet("");
			this->ui->label_stop->setStyleSheet("");
			break;
		case 2:
			this->ui->label_run->setStyleSheet("");
            this->ui->label_amb->setStyleSheet("color: #3467eb; border-style: solid; border-width: 1px; border-color: #3467eb; border-radius: 5%; ");
			this->ui->label_stop->setStyleSheet("");
			break;
		default:
			break;
	}
}

void	CPlotChamber::addData(QString data) {
	QDateTime passed_time = getPassedTime(data);
	if (this->is_start_line) {
		this->time_start = passed_time;
		this->is_start_line = false;
	}
	double passed_sec = double(getPassedSec(this->time_start, passed_time));
	this->running_time = passed_sec;
	this->points_pv << QPointF(passed_sec * this->x_axis_increase, data.split(",")[2].toDouble());
	this->points_sv << QPointF(passed_sec * this->x_axis_increase, data.split(",")[3].toDouble());
	this->ui->lcdNumber_pv->display(data.split(",")[2].toDouble());
	this->ui->lcdNumber_sv->display(data.split(",")[3].toDouble());

	/*
	 * Chamber State
	 * 0 : STOP
	 * 1 : START
	 * 2 : AMBIENT
	 */
	setChamberState(data.split(",")[1].toInt());
}

void	CPlotChamber::resetData() {
	QFile *log_file = new QFile( drc->getDatFileName() );
	this->points_pv.clear();
	this->points_sv.clear();

	QByteArray data;
	if (!log_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "File open Error";
		return ;
	}
	while (!log_file->atEnd()) {
		data = log_file->readLine();
		QDateTime passed_time = getPassedTime(data);
		double passed_sec = double(getPassedSec(this->time_start, passed_time)) * this->x_axis_increase;
		this->points_pv << QPointF(passed_sec, QString(data).split(",")[2].toDouble());
		this->points_sv << QPointF(passed_sec, QString(data).split(",")[3].toDouble());
	}
	log_file->close();
}

bool	CPlotChamber::changeUnit() {
	if (this->drc->getXAxisUnit() == AXIS_SEC && this->running_time >= this->x_axis_max) {
		this->drc->setXAxisUnit(AXIS_MIN);
		this->x_axis_range = 20;
		this->x_axis_increase = (double)1 / 60;
//		this->x_axis_max = 720;
		this->x_axis_max = 60; // test
		this->ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "[min]");
		return true;
	}
	if (this->drc->getXAxisUnit() == AXIS_MIN && this->running_time / 60 >= this->x_axis_max) {
		this->drc->setXAxisUnit(AXIS_HOUR);
		this->x_axis_increase = (double)1 / 3600;
//		this->x_axis_range = 18;
		this->x_axis_range = 2; // test
		this->x_axis_max = 120;
		this->ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "[hour]");
		return true;
	}
	return false;
}

//RFU
void    CPlotChamber::slotDataError()
{

}

void	CPlotChamber::slotAddData(std::vector<QString> datas) {
	for (uint i = 0; i < datas.size(); i += 1) {
		addData(datas[i]);
	}

	if (changeUnit()) {
		resetData();
	}

	double compensation = this->drc->getXAxisUnit() == AXIS_SEC ? 1 :
						  this->drc->getXAxisUnit() == AXIS_MIN ? 60 : 3600;

	if (this->x_axis_range < this->running_time / compensation * 1.1) {
		x_axis_range = std::min((x_axis_range * 1.3) / 10 * 10, x_axis_max);
	}

	this->ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, x_axis_range);
	this->curve_pv->setSamples(points_pv);
	this->curve_pv->attach(this->ui->qwtPlot);
	this->curve_sv->setSamples(points_sv);
	this->curve_sv->attach(this->ui->qwtPlot);
	this->ui->qwtPlot->update();
}

void	CPlotChamber::slotInitPlot() {
	initQwt();
}

void	CPlotChamber::slotStartPlot() {
	emit sigStart();
}

void	CPlotChamber::slotStopPlot() {
	emit sigStop();
}

void	CPlotChamber::slotResetPlot() {
	resetPlot();
	emit sigReset();
}

qint64	CPlotChamber::getPassedSec(QDateTime time_start, QDateTime time_pass) {
	return time_start.secsTo(time_pass);
}

QDateTime	CPlotChamber::getPassedTime(QString data) {
	//QString sDateTime = data.split(",")[0].trimmed();
	//QDateTime sFromStr  = QDateTime::fromString( sDateTime, "yyyy-MM-dd hh:mm:ss");
	//return sFromStr;

	int year = data.split(",")[0].split(" ")[0].split("-")[0].toInt();
	int month = data.split(",")[0].split(" ")[0].split("-")[1].toInt();
	int day = data.split(",")[0].split(" ")[0].split("-")[2].toInt();
	int hour = data.split(",")[0].split(" ")[1].split(":")[0].toInt();
	int minute = data.split(",")[0].split(" ")[1].split(":")[1].toInt();
	int second = data.split(",")[0].split(" ")[1].split(":")[2].toInt();
	return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}
