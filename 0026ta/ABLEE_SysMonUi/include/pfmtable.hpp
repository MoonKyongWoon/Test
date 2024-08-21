#ifndef PFMTABLE_HPP
#define PFMTABLE_HPP

#include <QWidget>
#include "commstruct.hpp"

#define STATUS_WORD_MASK				( 0b1111111111000011)
#define STATUS_VOUT_MASK				( 0b01101011)

enum class PFM_STATUS_TYPE {
	OK,
	SW_VOUT_OV_FAULT, // 0b 0000 0000 0010 0000
	SW_IOUT_OC_FAULT, // 0b 0000 0000 0001 0000
	SW_VIN_UV_FAULT,  // 0b 0000 0000 0000 1000
	SV_VOUT_OV_FAULT, // 0b 1000 0000
	SV_VOUT_UV_FAULT, // 0b 0001 0000
};

namespace Ui {
	class CPfmTable;
}

class CPfmTable : public QWidget
{
	Q_OBJECT

public:
	explicit CPfmTable(QWidget *parent = nullptr);
	~CPfmTable();

private:
	Ui::CPfmTable *ui;

private:
	void	initUi();

public:
	void			updateData(pfm_unit_data data);
	void			updateTime(pfm_time time);
	void			updateCPU(int cpu_no);
	void			setItem(int row, int col, QString text);
	PFM_STATUS_TYPE	getPfmStatus(int status_word, int status_vout);
	QString			getQStringFromPfmStatusType(PFM_STATUS_TYPE pfm_status_type);
	Qt::GlobalColor	getGlobalColorFromPfmStatysType(PFM_STATUS_TYPE pfm_status_type);
};

#endif // PFMTABLE_HPP
