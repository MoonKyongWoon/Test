#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QDialog>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include "sysini.hpp"
#include "syslog.hpp"
#include "util.hpp"

namespace Ui {
	class CConfig;
}

class CConfig : public QDialog
{
	Q_OBJECT

public:
	explicit CConfig(QWidget *parent = nullptr);
	~CConfig();
signals:
	void SigConfigRestartService();
	void SigConfigSaved();
public slots:
	void SlotSaveIni();
	void SlotRestartService();
protected:
	void showEvent(QShowEvent *event);
private:
	void InitVars();
	void InitUi();
	void LoadOsEnv();
	bool LoadTmsIni();
	bool SaveTmsIni();
private:
	QString m_sOsDir;
	QString m_sTmsIni;
private:
	Ui::CConfig *ui;
	QDoubleSpinBox* m_pSpinBox[KEY_TIMEOUT_MAX];
	double m_dTimeoutDefault[KEY_TIMEOUT_MAX];

};

#endif // CONFIG_HPP
