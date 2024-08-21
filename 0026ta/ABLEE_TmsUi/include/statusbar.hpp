#ifndef _STATUSBAR_HPP
#define _STATUSBAR_HPP

#include <QLabel>
#include <QPixmap>
#include <QWidget>

enum _LED_DESC
{
	_LED_EAP_TCP    ,
	_LED_EAP_SELECT ,
	_LED_MAINUI     ,
	_LED_COUNT      ,
};

namespace Ui {
	class CStatusBar;
}

class CStatusBar : public QWidget
{
	Q_OBJECT
public:
	explicit CStatusBar(QWidget *parent = nullptr);
	~CStatusBar();
private:
	void InitUi();
	void SetStatus( _LED_DESC, bool );
public slots:
	void SlotStatusChanged( _LED_DESC, bool );
private:
	Ui::CStatusBar *ui;
	QLabel* m_pLed[_LED_COUNT];
	QPixmap m_pixmapON;
	QPixmap m_pixmapOFF;
};

#endif // STATUSBAR_HPP
