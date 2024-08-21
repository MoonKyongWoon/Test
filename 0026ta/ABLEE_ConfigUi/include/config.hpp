#ifndef CCONFIG_HPP
#define CCONFIG_HPP

#include <QCheckBox>
#include <QCloseEvent>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include <QtSerialPort/QSerialPortInfo>
#include "util.hpp"
#include "sysini.hpp"
#include "popup.hpp"

#define DISABLE_CPU_QCOLOR				QColor(230,230,230)
#define DISABLE_ROW_QCOLOR				QColor(240,240,240)
#define ENABLE_ROW_QCOLOR				QColor(255,255,255)
#define ERROR_ROW_QCOLOR				QColor(255,  0,  0)

#define DISABLE_ROW_QSS_COLOR			"rgb(240,240,240)"
#define ENABLE_ROW_QSS_COLOR			"rgb(255,255,255)"

#define STR_DISABLE						"DISABLE"
#define STR_ENABLE						"ENABLE "
#define STR_DISABLE_STAT				"MASK SLOT"
#define STR_ENABLE_STAT					"USE SLOT"

namespace Ui {
	class CConfig;
}

enum SITE_TABLE_HEADER
{
	SITE_HEAD_SLOT   ,
	SITE_HEAD_MASK   ,
	SITE_HEAD_CPU1_IP,
	SITE_HEAD_CPU2_IP,
	SITE_HEAD_STATUS ,
	SITE_HEAD_UPDATE ,
};

class CConfig : public QWidget
{
	Q_OBJECT
public:
	explicit CConfig(QWidget *parent = nullptr);
	~CConfig();
private:
	void InitUi();
	bool InitSlotTab();
	bool InitSystemTab();
	bool InitFtpTab();
    bool InitChamberTab();
	bool InitServerTab();
	bool InitTmsTab();
private:
	void LoadIni();
	bool LoadSiteIni();
	bool LoadSystemIni();
	bool LoadFtpIni();
    bool LoadChamberIni();
	bool LoadServerIni();
	bool LoadTmsIni();
	bool SaveSiteIni();
	bool SaveServerIni();
private:
	bool LoadOsEnv();
private:
    void setQComboBoxStyle();
protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
public slots:
	void SlotSlotCpuPerSlotChanged(int);
	void SlotSlotMaskChecked();
	void SlotSlotBtnReloadClicked();
	void SlotSlotBtnSaveClicked();
public slots:
	void SlotFtpCboHostListChanged(int);
	void SlotFtpBtnReloadClicked();
	void SlotFtpBtnSaveClicked();
public slots:
    void SystemBtnSaveClicked();
    void SystemBtnReloadClicked();
public slots:
    void ChamberBtnSaveClicked();
    void ChamberBtnReloadClicked();
public slots:
	void SlotServerBtnSaveClicked();
	void SlotServerBtnReloadClicked();
private:
	QString m_sOsDir;
	QString m_sSiteIni;
	QString m_sSystemIni;
	QString m_sFtpIni;
    QString m_sChamberIni;
	QString m_sServerIni;
	QString m_sTmsIni;
private:
	Ui::CConfig *ui;
	QCheckBox* m_pCheckMask[32];
public:
    void showPopup(const QString& message);
};

#endif // CCONFIG_HPP
