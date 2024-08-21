#include "pfmtable.hpp"
#include "ui_pfmtable.h"

CPfmTable::CPfmTable(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CPfmTable)
{
	ui->setupUi(this);
	initUi();
}

CPfmTable::~CPfmTable()
{
	delete ui;
}

void				CPfmTable::initUi() {
	this->ui->tableWidget->setSpan(0, 0, 8, 1);
	this->ui->tableWidget->setSpan(8, 0, 8, 1);
	this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int row = 0; row < 16; row += 1) {
		for (int column = 1; column < 6; column += 1) {
			this->ui->tableWidget->item(row, column)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
		}
	}
	this->ui->tableWidget->item(0, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	this->ui->tableWidget->item(8, 0)->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
}

void				CPfmTable::setItem(int row, int col, QString text) {
	this->ui->tableWidget->item(row, col)->setText(text);
}

void				CPfmTable::updateCPU(int cpu_no) {
	this->ui->label_cpu->setText(tr("CPU %1").arg(cpu_no));
}

void				CPfmTable::updateTime(pfm_time time) {
	QString updated_time = QString::asprintf("%02d:%02d:%02d", time.t.tm_hour, time.t.tm_min, time.t.tm_sec);
	this->ui->label_time->setText(updated_time);
}

void				CPfmTable::updateData(pfm_unit_data data) {
	this->updateTime(data.time);

	PFM_STATUS_TYPE pfm_status_type;

	for (int i = 0; i < 8; i += 1) {
		this->setItem(i, 2, QString("").asprintf("%6.2f", data.pps[i].set_voltage) + " V");
		this->setItem(i, 3, QString("").asprintf("%6.2f",data.pps[i].present_voltage) + " V");
		this->setItem(i, 4, QString("").asprintf("%6.2f",data.pps[i].present_current) + " A");
		pfm_status_type = getPfmStatus(data.pps[i].status_word, data.pps[i].status_vout);
		this->setItem(i, 5, getQStringFromPfmStatusType(pfm_status_type));
		this->ui->tableWidget->item(i, 5)->setForeground(QBrush(getGlobalColorFromPfmStatysType(pfm_status_type)));

		this->setItem(i + 8, 2, QString("").asprintf("%6.2f", data.s1ps[i].set_voltage) + " V");
		this->setItem(i + 8, 3, QString("").asprintf("%6.2f", data.s1ps[i].present_voltage) + " V");
		this->setItem(i + 8, 4, QString("").asprintf("%6.2f", data.s1ps[i].present_current) + " A");
		pfm_status_type = getPfmStatus(data.s1ps[i].status_word, data.s1ps[i].status_vout);
		this->setItem(i + 8, 5, getQStringFromPfmStatusType(pfm_status_type));
		this->ui->tableWidget->item(i + 8, 5)->setForeground(QBrush(getGlobalColorFromPfmStatysType(pfm_status_type)));
	}
}

PFM_STATUS_TYPE		CPfmTable::getPfmStatus(int status_word, int status_vout) {
	int masked_status_word = status_word & ~STATUS_WORD_MASK;	// 1111 1111 1100 0011
	int masked_status_vout = status_vout & ~STATUS_VOUT_MASK;	//			 0110 1011
	if (masked_status_word & 0x0020)
		return PFM_STATUS_TYPE::SW_VOUT_OV_FAULT;
	if (masked_status_word & 0x0010)
		return PFM_STATUS_TYPE::SW_IOUT_OC_FAULT;
	if (masked_status_word & 0x0008)
		return PFM_STATUS_TYPE::SW_VIN_UV_FAULT;
	if (masked_status_vout & 0x0080)
		return PFM_STATUS_TYPE::SV_VOUT_OV_FAULT;
	if (masked_status_vout & 0x0010)
		return PFM_STATUS_TYPE::SV_VOUT_UV_FAULT;
	return PFM_STATUS_TYPE::OK;
}

QString				CPfmTable::getQStringFromPfmStatusType(PFM_STATUS_TYPE pfm_status_type) {
	switch (pfm_status_type) {
		case PFM_STATUS_TYPE::OK:
			return "OK";
			break;
		case PFM_STATUS_TYPE::SW_VOUT_OV_FAULT:
			return "OV";
			break;
		case PFM_STATUS_TYPE::SW_IOUT_OC_FAULT:
			return "OC";
			break;
		case PFM_STATUS_TYPE::SW_VIN_UV_FAULT:
			return "UV";
			break;
		case PFM_STATUS_TYPE::SV_VOUT_OV_FAULT:
			return "OV";
			break;
		case PFM_STATUS_TYPE::SV_VOUT_UV_FAULT:
			return "UV";
			break;
		default:
			return "undefined";
			break;
	}
}

Qt::GlobalColor		CPfmTable::getGlobalColorFromPfmStatysType(PFM_STATUS_TYPE pfm_status_type) {
	if (pfm_status_type == PFM_STATUS_TYPE::OK)
		return Qt::black;
	return Qt::red;
}
