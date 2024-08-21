#ifndef PLOTCHAMBER_HPP
#define PLOTCHAMBER_HPP

#include <QtGui>
#include <QWidget>
#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_plot_grid.h"
#include "qwt/qwt_plot_picker.h"
#include "qwt/qwt_picker_machine.h"
#include "plotdatareader.hpp"

#define RED				"#ff0000"
#define BLUE			"#0000ff"
#define GREEN			"#00CC00"
#define YELLOW			"#ffff00"
#define ORANGE			"#e78200"
namespace Ui {
	class CPlotChamber;
}

class CPlotChamber : public QWidget
{
	Q_OBJECT

public:
	explicit CPlotChamber(QWidget *parent = nullptr);
	~CPlotChamber();
	bool						getChamberEnable();
	void						setChamberEnable(bool flag);

private:
	Ui::CPlotChamber			*ui;
	QPolygonF					points_pv;
	QPolygonF					points_sv;
	QwtPlotCurve				*curve_pv;
	QwtPlotCurve				*curve_sv;
	QwtPlotGrid					*grid;
	QwtPlotPicker				*picker;
	QwtPickerClickPointMachine	*machine;
	CPlotDataReaderController	*drc;
	double						x_axis_range;
	double						x_axis_max;
	double						x_axis_increase;
	double						running_time;
	QDateTime					time_start;
	bool						is_start_line;
private:
	void                        initPlot();
	void						initQwt();
	void						initLcd();
	void						resetPlot();
	void						resetQwt();
	void						resetLcd();
	void						setChamberState(int state);
	void						addData(QString data);
	void						resetData();
	bool						changeUnit();
	qint64						getPassedSec(QDateTime time_start, QDateTime time_pass);
	QDateTime					getPassedTime(QString data);
signals:
	void						sigStart();
	void						sigStop();
	void						sigReset();
public slots:
	void						slotAddData(std::vector<QString> datas);
	void						slotInitPlot();
	void						slotStartPlot();
	void						slotStopPlot();
	void						slotResetPlot();
	void						slotDataError();
};

#endif // PLOTCHAMBER_HPP
