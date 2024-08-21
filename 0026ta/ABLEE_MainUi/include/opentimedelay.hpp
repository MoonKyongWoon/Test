#ifndef OPENTIMEDELAY_HPP
#define OPENTIMEDELAY_HPP

#include <QDialog>
#include <QCheckBox>
#include <QMessageBox>
#include <QSpinBox>
#include "common.hpp"
#include "commstruct.hpp"

namespace Ui {
	class COpenTimeDelay;
}

class COpenTimeDelay : public QDialog
{
	Q_OBJECT

public:
	explicit COpenTimeDelay(QWidget *parent = nullptr);
	~COpenTimeDelay();
public slots:
	void SlotBtnSaveClicked();
	void SlotBtnExitClicked();
public slots:
	void SlotChkEnableToggled(bool);
private:
	void InitVars();
	void InitUi();
	void LoadIni();
	bool SaveIni();
private:
	Ui::COpenTimeDelay *ui;
};

#endif // OPENTIMEDELAY_HPP
