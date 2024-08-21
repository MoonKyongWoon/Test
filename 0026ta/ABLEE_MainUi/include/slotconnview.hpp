#ifndef SLOTCONNVIEW_HPP
#define SLOTCONNVIEW_HPP

#include <QDebug>
#include <QLabel>
#include <QObject>
#include <QScrollBar>
#include <QWidget>
#include "common.hpp"
#include "commstruct.hpp"
#include "sysdef.hpp"
#include "protocol.hpp"


namespace Ui {
	class CSlotConnView;
}

enum SLOT_CONN_ROWS
{
	ROW_SLOT_LOADER   ,
	ROW_RUN1_CTRL     ,
	ROW_RUN1_DATA     ,
	ROW_RUN1_LOG      ,
	ROW_RUN2_CTRL     ,
	ROW_RUN2_DATA     ,
	ROW_RUN2_LOG      ,
	SLOT_CONN_MAX_ROW ,
};

#define SLOTCONN_VHEAD_WIDTH		( 100 )
#define SLOTCONN_HHEAD_HEIGHT		( 30 )

class CSlotConnView : public QWidget
{
	Q_OBJECT

public:
	explicit CSlotConnView(QWidget *parent = nullptr);
	~CSlotConnView();
private:
	void InitUi();
	void InitStatus();
public:
	void ResizeView();
	void SetLoaderStatus( int, EN_SLOT_STAT );
	void SetRunnerStatus( int, int, PROTOCOL::EN_PGM_SOCKET_TYPE, EN_SLOT_STAT );
protected:
	void resizeEvent(QResizeEvent *event);
public slots:
	void SlotLoaderStatusChanged(int, EN_SLOT_STAT );
	void SlotPgmStatusChanged( int slot, int runner, PROTOCOL::EN_PGM_SOCKET_TYPE type, EN_SLOT_STAT stat );
private:
	Ui::CSlotConnView *ui;
	EN_SLOT_STAT m_SlotStat[MAX_SLOT][SLOT_CONN_MAX_ROW];
	int m_nRows;
	int m_nCols;

};

#endif // SLOTCONNVIEW_HPP
