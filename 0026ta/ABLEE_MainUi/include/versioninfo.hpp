#ifndef VERSIONINFO_HPP
#define VERSIONINFO_HPP

#include <string>
#include <QWidget>

#include "util.hpp"
#include "sysver.hpp"

namespace Ui {
	class CVersionInfo;
}

class CVersionInfo : public QWidget
{
	Q_OBJECT

private:
	QString GetHostname();
	QString GetVersion();
	void DisplayVersion();
	void DisplayInterruptMode();
public:
	explicit CVersionInfo(QWidget *parent = nullptr);
	~CVersionInfo();
public:
	void SetUiAutoMode(bool);
signals:
	void SigInterruptMode(bool);
public slots:
	void SlotBtnInterruptChanged(bool);
private:
	Ui::CVersionInfo *ui;
	bool m_bUiAutoMode;
	bool m_bInterrupt;		//AutoMode:false, Manual:true
};

#endif // VERSIONINFO_HPP
