#ifndef DLGSV_HPP
#define DLGSV_HPP

#include <QDialog>
#include <QMessageBox>

namespace Ui {
	class CDlgSv;
}

enum EN_SV_MODE
{
	SV_TEMP,
	SV_AMB ,
};

class CDlgSv : public QDialog
{
	Q_OBJECT

public:
	explicit CDlgSv(QWidget *parent = nullptr);
	~CDlgSv();
public:
	void setMode(EN_SV_MODE);
	void setTempLimit(double, double);
public slots:
	void SlotBtnSetClicked();
signals:
	void SigSvSet( EN_SV_MODE, double );
private:
	void setText(QString);
private:
	Ui::CDlgSv *ui;
	QString m_dSv;
	EN_SV_MODE m_SvMode;
	double m_dLimitLow;
	double m_dLimitHigh;
};

#endif // DLGSV_HPP
