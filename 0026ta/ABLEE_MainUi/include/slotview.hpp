#ifndef SLOTVIEW_HPP
#define SLOTVIEW_HPP

#include <QProgressBar>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include "common.hpp"
#include "commstruct.hpp"
#include "filemaplib.hpp"
#include "sysini.hpp"

#define DEFAULT_VHEAD_WIDTH		( 30 )
#define DEFAULT_HHEAD_HEIGHT	( 30 )

namespace Ui {
	class CSlotView;
}

class CSlotView : public QWidget
{
	Q_OBJECT

public:
	explicit CSlotView(QWidget *parent = nullptr);
	~CSlotView();
protected:
	void resizeEvent(QResizeEvent *event);
signals:
	void SigToMainUiCurrentSlot(uint);
	void SigToMainUiSetupResult(bool);
	void SigSysLogMessage(LOG_TYPE, QString );
public slots:
	void SlotFromMainUiUpdateBibId( uint );
	void SlotFromMainUiSetupView();
	void SlotFromMainUiTestStartFlag();
	void SlotFromMainUiTestEndFlag();
	void SlotFromMainUiStepOut(uint);
	void SlotFromMainUiLoadExistData();
	void SlotFromMainUiUpdateSetup();
	void SlotFromUiUpdateAutoInitResult(uint slot, uint pass_qty,uint total_qty);
	void SlotInitSlotData();
	void SlotFromMainUiInitTestDone();
//	void SlotFromMainUiUpdatePartNo( uint );
//	void SlotFromMainUiUpdatePartNo();
//	void SlotFromMainUiUpdateMlbCode();
//	void SlotFromMainUiUpdateLotNo();
//	void SlotFromMainUiUpdatePassAndTotal();
public slots:
	void SlotCellOfSlotClicked(int,int);
public:
	void SetTestDataMgrPtr(CFileMap*);
	void SetSlotInfo(stSlotInfo&);
	void SetSlotLive(stSlotLive&);
	void ResizeSlotView();

private:
	void InitVariables();
	void InitUi();
	void InitSlotView();
	void InitSlotData();
private:
	void InitSlotProgress(uint);
	void UpdateSlotProgress(uint);
	void DisplaySlotInfo(uint);
	bool DisplaySlotViewPassTotal(uint slot);
	void DisplayBoardMaintInfo();
	void DisplayBibId(uint);
//	void UpdatePartNo( uint );
//	void UpdateMlbCode( uint );
//	void UpdateLotNo( uint );
//	void UpdatePassAndTotal( uint );
private:
	Ui::CSlotView *ui;
	CFileMap* m_pTestDataMgr;
	stSlotInfo m_stSlotInfo;
	stSlotLive m_stSlotLive;
	bool m_bTestRun;
};

#endif // SLOTVIEW_HPP
