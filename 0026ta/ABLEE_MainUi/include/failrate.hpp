#ifndef FAILRATE_HPP
#define FAILRATE_HPP

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include "common.hpp"
#include "commstruct.hpp"
#include "util.hpp"

namespace Ui {
	class CFailRate;
}

#define FAIL_RATE_MAX_BIN			(9)

class CFailRate : public QDialog
{
	Q_OBJECT

public:
	explicit CFailRate(QWidget *parent = nullptr);
	~CFailRate();
public slots:
	void SlotBtnSaveClicked();
	void SlotBtnLoadClicked();
	void SlotBtnExitClicked();
public slots:
	void SlotChkFailRateToggled(bool);
	void SlotChkBinsToggled(bool);
	void SlotSpinBinValueChanged(int);
private:
	void InitVars();
	void InitUi();
private:
	Ui::CFailRate *ui;
	QCheckBox* m_pChkBin[FAIL_RATE_MAX_BIN];
	QSpinBox* m_pSpinBin[FAIL_RATE_MAX_BIN];
};

#endif // FAILRATE_HPP
