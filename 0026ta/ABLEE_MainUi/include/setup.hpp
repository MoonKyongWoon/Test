#ifndef SETUP_HPP
#define SETUP_HPP

#include <QDialog>
#include <QMessageBox>
#include "common.hpp"

namespace Ui {
	class CSetup;
}

class CSetup : public QDialog
{
	Q_OBJECT

public:
	explicit CSetup(QWidget *parent = nullptr);
	~CSetup();
public:
	void SetFocus();
	void SetAutoMode(bool);
	void SetUiValue(QString, QString);
private:
	void InitUi();
	void InitUiValue();
	void Setup();
private:
	Ui::CSetup *ui;
signals:
	void SigSetupDlgResult(bool, stSetupInfo);
public slots:
	void SlotBtnOkClicked();
	void SlotBtnCancelClicked();
private:
	bool m_bAutoMode;
	stSetupInfo m_stSetupInfo;
};

#endif // SETUP_HPP
