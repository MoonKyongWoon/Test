#ifndef BIBVIEW_HPP
#define BIBVIEW_HPP

#include "common.hpp"
#include "syslog.hpp"
#include "sysini.hpp"
#include "util.hpp"

#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>

namespace Ui {
	class CBibView;
}

class CBibView : public QWidget
{
	Q_OBJECT

public:
	explicit CBibView(uint, QWidget *parent = nullptr);
	~CBibView();
private:
	void InitVars();
	void InitUi();
	void InitBtnResult();
	void InitSignalSlots();
public:
	void SetBibRowCol(uint,uint);
	QTableWidget* GetBibMap();
	QComboBox* GetStepCombo();
public:
	void ClearStepCombo();
	void ClearBibMap();
	void DrawBibMap();
private:
	Ui::CBibView *ui;
	uint m_nSlotNo;
	uint m_nRow;
	uint m_nCol;
	EN_BTN_RESULT m_ResultType;
	QPushButton* m_arrayBtn[BTN_RESULT_COUNT];
};

#endif // BIBVIEW_HPP
