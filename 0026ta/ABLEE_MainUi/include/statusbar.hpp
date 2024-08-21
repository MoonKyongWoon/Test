#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QLabel>
#include <QPixmap>
#include <QWidget>

#include "sysver.hpp"


namespace Ui {
	class CStatusBar;
}

enum LED_DESC
{
	LED_MAIN_TO_MGR   ,
	LED_MAIN_TO_SLOT  ,
	LED_MAIN_TO_TMS   ,
	LED_TMS_TO_EAP ,
	LED_COUT          ,
};

class CStatusBar : public QWidget
{
	Q_OBJECT

public:
	explicit CStatusBar(QWidget *parent = nullptr);
	~CStatusBar();
private:
	void InitUi();
	void SetStatus( LED_DESC, bool );
public slots:
	void SlotStatusChanged( LED_DESC, bool );
private:
	Ui::CStatusBar *ui;
	QLabel* m_pLed[LED_COUT];
	QPixmap m_pixmapON;
	QPixmap m_pixmapOFF;
};

#endif // STATUSBAR_HPP
