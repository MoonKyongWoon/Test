#ifndef MASSTAB_HPP
#define MASSTAB_HPP

#include <QDebug>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QSettings>
#include <QTableWidget>
#include <vector>
#include <QTimer>
#include <QTableWidget>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>
#include <QSharedMemory>
#include "common.hpp"
#include "commstruct.hpp"
#include "logger.hpp"
#include "filemaplib.hpp"
#include "plotchamber.hpp"


#define STATUS_VOUT_MASK				( 0b01101011)
#define STATUS_WORD_MASK				( 0b1111111111000011)

enum class PFM_STATUS_TYPE {
	OK,
	SW_VOUT_OV_FAULT, // 0b 0000 0000 0010 0000
	SW_IOUT_OC_FAULT, // 0b 0000 0000 0001 0000
	SW_VIN_UV_FAULT,  // 0b 0000 0000 0000 1000
	SV_VOUT_OV_FAULT, // 0b 1000 0000
	SV_VOUT_UV_FAULT, // 0b 0001 0000
};

namespace Ui {
class CMassTab;
}

class CPlotData
{
public:
	std::vector<double> m_x;
	std::vector<double> m_y;
};

enum MASS_TIMER_VALUE
{
	MASS_TIMER_TEST  = 1000,
	MASS_TIMER_CHMB  = 5000,
	MASS_TIMER_PWR   = 1000,
    MASS_TIMER_PFM   = 1000,
};

class CMassTab : public QWidget
{
    Q_OBJECT

public:
    explicit CMassTab(QWidget *parent = nullptr);
    ~CMassTab();
public:
	void SetUiTestInfo(stUiTestInfo&);
	void SetSlotTestInfo(stSlotTestInfo*);
	void SetSlotLive(stSlotLive&);
	void SetPgmLive(stPgmLive&);
private:
	void InitUi();
	void InitVariables();
	void InitSigSlot();
	void InitLotInfoView();
	void InitPpsView();
	void InitPlotView();
	void DisplayTestTime(uint);
	void DisplaySlotInfo(uint);
private:
	void StartTestTimer();
	void StartPwrTimer();
	void StopTimer();
private:
	void ResizeLotInfoView();
	void ResizePpsView();
public:
	void ResizeView();
	void SetTestDataMgrPtr(CFileMap*);
public slots:
	void SlotSlotSetupResult( int, bool, QString, int, int, QString, QString );	//Setup Result
	void SlotTestStartFlag();
	void SlotTestEndFlag();
	void SlotTestPgmVariableFlag(uint);
public slots:
	void SlotFromMainUiLoadExistData();
	void SlotFromSlotViewCurrentSlot(uint);
	void SlotStepOut(uint);
public slots:
	void SlotTimeoutTest();
	void SlotTimeoutPwr();
public slots:
	void SlotStartChmbPlot();
	void SlotStopChmbPlot();
	void SlotResetChmbPlot();
public slots:
	void SlotRecvRcmd(_RCMD rcmd);
signals:
	void SigSysLogMessage ( LOG_TYPE, QString );
	void SigStartChmbGraph();
	void SigStopChmbGraph();
	void SigResetChmbGraph();
protected:
	void resizeEvent(QResizeEvent *event);
private:
    Ui::CMassTab *ui;
	QTableWidget* m_pPsView[2];
	QwtPlotCurve* m_pPlotCurve;
	CPlotChamber* m_pPlotChamber;
	CFileMap* m_pTestDataMgr;
private:
	stUiTestInfo m_stUiTestInfo;
	stSlotTestInfo m_stSlotTestInfo[MAX_SLOT];
	stSlotLive m_stSlotLive;
	stPgmLive  m_stPgmLive;
	uint m_nCurrentSlot;
private:
	QTimer* m_pTmrTest;
	QTimer* m_pTmrPwr;
	QDateTime m_dtTestStart;
	QDateTime m_dtTestEnd;
	QDateTime m_dtUpdateTime[MAX_SLOT];
	bool m_bTest;
private:
    QSharedMemory*  m_pShmPfm;
    QTimer*         m_pTmrPfm;
private:
	void			attachSharedMemory();
	void			setInitValuePpsView();
	bool			isStatusFault(int status_word, int status_vout);
	PFM_STATUS_TYPE	getPfmStatus(int status_word, int status_vout);
	QString			getQStringFromPfmStatusType(PFM_STATUS_TYPE pfm_status_type);
	Qt::GlobalColor	getGlobalColorFromPfmStatysType(PFM_STATUS_TYPE pfm_status_type);
    void            StartPfmTimer();

public slots:
	void slotSetPfm();
    void SlotInitTestStartFlag();
};

#endif // MASSTAB_HPP
