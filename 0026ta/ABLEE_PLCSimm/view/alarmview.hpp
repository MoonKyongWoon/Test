#ifndef ALARMVIEW_HPP
#define ALARMVIEW_HPP

#include <QWidget>
#include <QPushButton>
#include "Common.hpp"

namespace Ui {
	class CAlarmView;
}

class CAlarmView : public QWidget
{
	Q_OBJECT

public:
	explicit CAlarmView(QWidget *parent = nullptr);
	~CAlarmView();
public:
	bool DoReadAlarm( EN_PLC_ADDRESS_MATCH_CMD, int & );
public slots:
	void SlotBtnToggled(bool);
	void SlotAlarmReset();
private:
	void Init();
	void InitButtons();
	void InitBtnPtr();
private:
	Ui::CAlarmView *ui;
	QPushButton* m_pAlrmBtn[10][16];
};

#endif // ALARMVIEW_HPP
