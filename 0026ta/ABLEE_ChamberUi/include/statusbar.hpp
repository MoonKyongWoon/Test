#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include "sysver.hpp"

namespace Ui {
	class CStatusBar;
}

enum CHMB_LED
{
	CHMB_LED_IPC     ,
	CHMB_LED_PLC     ,
	CHMB_LED_MAX     ,
};

class CStatusBar : public QWidget
{
	Q_OBJECT

public:
	explicit CStatusBar(QWidget *parent = nullptr);
	~CStatusBar();
public slots:
	void SlotChmbConnStatus( bool, bool );
private:
	void InitUi();
	void SetStatus( CHMB_LED desc, bool conn );
private:
	Ui::CStatusBar *ui;
	QLabel* m_pLed[CHMB_LED_MAX];
	QPixmap m_pixmapON;
	QPixmap m_pixmapOFF;
};

#endif // STATUSBAR_HPP
