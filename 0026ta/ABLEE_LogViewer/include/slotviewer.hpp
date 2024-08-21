#ifndef SLOTVIEWER_HPP
#define SLOTVIEWER_HPP

#include <QTabWidget>
#include <QWidget>

namespace Ui {
	class CSlotViewer;
}

class CSlotViewer : public QWidget
{
	Q_OBJECT

public:
	explicit CSlotViewer(uint slot, QWidget *parent = nullptr);
	~CSlotViewer();
private:
	void InitUi();
	void InitCpuLog();
	void CreateStatusBar();
signals:
	void SigCpuViewerInit();
private:
	Ui::CSlotViewer *ui;
	uint m_nSlotNo;
};

#endif // SLOTVIEWER_HPP
