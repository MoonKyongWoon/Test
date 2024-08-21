#ifndef ENGRTAB_HPP
#define ENGRTAB_HPP

#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedWidget>
#include <QSettings>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include "common.hpp"
#include "commstruct.hpp"
#include "dutmap.hpp"
#include "logger.hpp"
#include "syslog.hpp"
#include "sysini.hpp"
#include "util.hpp"
#include "filemaplib.hpp"

#define DEFAULT_BIB_ROWS		( 16 )
#define DEFAULT_BIB_COLS		( 12 )
#define DEFAULT_SLOT_VIEW		(  8 )

enum EN_BTN_RESULT
{
	BTN_NORMAL,
	BTN_MAKER ,
	BTN_DEV_TYPE,
	BTN_FAIL_CNT,
	BTN_ADD_FAIL,
	BTN_BIN,
	BTN_RESULT_COUNT,
};


namespace Ui {
class CEngrTab;
}

enum LOG_TAB
{
	LOG_SLOT,
	LOG_MAX_TAB,
};

class CEngrTab : public QWidget
{
    Q_OBJECT

public:
    explicit CEngrTab(QWidget *parent = nullptr);
    ~CEngrTab();
public:
	void ResizeView();
	void ResizeBibMapView();
public:
	void SetTestDataMgrPtr(CFileMap*);	
	void SetUiTestInfo(stUiTestInfo&);
	void SetSlotTestInfo(stSlotTestInfo*);
	void SetSlotLive(stSlotLive&);
	void SetPgmLive(stPgmLive&);

private:
	void InitVariables();
	void InitUi();
	void InitSigSlot();
	void InitSlotLogTextView();
	void InitBibMapView();
	void InitBtnResult();
	void ReInitBibMapView(int,int);
	void CreateBibDut();
	void InitSlotBibDut();
	void AppendPgmTextEdit ( uint, uint, LOG_TYPE, QString );
	void AppendDutDataTextEdit( LOG_TYPE, QString );
	void ClearDutDataTextEdit();
	void DisplayBibmapTestData(uint);
	void DisplayPgmVariables(uint);
	void DisplayTestItemList(uint);
	bool GetDutToXy(int,int&,int&);
	void DisplaySlotData(uint);
	void DisplaySlotInfo(uint);
	void ShowSlotLogView(uint);

private:
	bool LoadDevPgmVar( uint, uint );
	bool LoadInitFail(uint);
	bool LoadStepFail(uint,uint,uint);	//NORMAL
	bool LoadFailCnt(uint,uint,uint);	//FAIL CNT
	bool LoadBin(uint);					//BIN
	bool LoadChipInfo(uint);
	bool LoadFailChip(uint);
	bool LoadDutFailBlock(uint,uint,uint,uint);
	bool MakeCumFailBlock(const char*,uint);
	bool MakeCumFailChip (const char*,uint);
private:
	uint GetStepFailDutBlockCount( uint slot, uint maxdut, uint dut, uint step, const char* );
	bool GetFailBlockAddress( std::vector<uint>& v, const char* p, uint dut, uint ce, uint chip, uint step=0 );
	bool IsFailBlockIn(uint);
	bool IsChipFail(char* pFailChip, uint dut, uint step=0);
	bool IsChipFail( char* pFailChip, uint dut, uint ce, uint chip, uint step=0 );
	void mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr);
	bool IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr);

private:
	bool DisplayBibStepFail(uint);
	bool DisplayBibSortBin(uint);
	bool DisplayBibFailCnt(uint);
	bool DisplayDutFailBlock(uint,uint);
	//bool DisplaySlotViewPassTotal(uint);	//2023.08.22 - KWMOON
protected:
	void resizeEvent(QResizeEvent *event);
signals:
	void SigAppendSystemLog( LOG_TYPE, QString );
	void SigSaveSysLog ( LOG_TYPE, QString );
	void SigSaveChmbLog( LOG_TYPE, QString );
	void SigSaveSlotLog( uint, uint, LOG_TYPE, QString );
public slots:
	void SlotFromSlotViewCurrentSlot(uint);
	void SlotFromMainUiLoadExistData();
	void SlotCellOfBibMapClicked(int,int);
	void SlotCellOfBibMapChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged();
	void SlotBtnResultKindChange();

public slots:
	void SlotPgmLogMessage ( uint, uint, LOG_TYPE, QString );
	void SlotSlotSetupResult( int, bool, QString, int, int, QString, QString );	//Setup Result
	void SlotPgmLoad();
	void SlotPgmLoadDone();
	void SlotInitTestDone(uint);
	void SlotViewPgmVariables(uint);
	void SlotTestItemListup(uint);
	void SlotItemSelectChanged(int);
	void SlotTestStartFlag();
	void SlotTestEndFlag();
	void SlotStepIn(uint);
	void SlotStepOut(uint);
	void SlotTestPgmVariableFlag(uint);
private:
	//CBibView* m_pBibView[MAX_SLOT];
	bool m_bAutoScrollSlot[MAX_SLOT][MAX_SYS_CPU];
	CLogger*  m_pLoggerSlot[MAX_SLOT][MAX_SYS_CPU];
	QStackedWidget* m_pStackLog[MAX_SYS_CPU];
	QPlainTextEdit* m_pSlotTextEdit[MAX_SLOT][MAX_SYS_CPU];
private:
    Ui::CEngrTab *ui;
	CFileMap* m_pTestDataMgr;
private:
	stUiTestInfo m_stUiTestInfo;
	stSlotLive m_stSlotLive;
	stPgmLive  m_stPgmLive;
	stSlotTestInfo m_stSlotTestInfo[MAX_SLOT];
	uint m_nCurrentSlot;
private:
	char* m_pInitFail;
	char* m_pStepFail;
	char* m_pFailChip;
	char* m_pCumFailChip;
	char* m_pCumBlockFail;
	uint* m_pSortBin;
	uint* m_pFailCnt;
	stChipId* m_pstChipId;
	bool m_bTest;
	std::vector<uint> m_vecFailBlk;
	stDevPgmVariables m_stDevPgmVars;
private:
	int m_nBibRows;
	int m_nBibCols;
	bool m_bHasItemList;
	EN_BTN_RESULT m_ResultType;
	QPushButton* m_arrayBtn[BTN_RESULT_COUNT];
};


#endif // ENGRTAB_HPP
