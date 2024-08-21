#include "PLCAddressDlg.hpp"
#include "ui_PLCAddressDlg.h"

CPLCAddressDlg::CPLCAddressDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPLCAddressDlg)
{
    ui->setupUi(this);
    InitUi();
    InitSigSlots();
}

CPLCAddressDlg::~CPLCAddressDlg()
{
    delete ui;
}

void CPLCAddressDlg::InitUi()
{

}

void CPLCAddressDlg::InitSigSlots()
{
    connect( ui->btnAdd     , &QPushButton::clicked, this, &CPLCAddressDlg::SlotBtnAddClicked   );
    connect( ui->btnClose   , &QPushButton::clicked, this, &CPLCAddressDlg::SlotBtnCloseClicked );
}

void CPLCAddressDlg::SetAddressType(int nType)
{
    ui->cboBitWord->setCurrentIndex(nType);
    if( nType == 0 )    //bit
    {
        ui->leValue->setInputMask("B");
        ui->leValue->setText("0");
        ui->leAddr->setText("");
    }
    else
    {
        ui->leValue->setInputMask("9999");
        ui->leValue->setText("0000");
        ui->leAddr->setText("");
    }
}

void CPLCAddressDlg::SlotBtnAddClicked()
{
    bool bSaved = true;
    if( ui->leAddr->text().isEmpty() )
    {
        QMessageBox::critical( this, "PLC Address", tr("ADDRESS를 입력하세요."), QMessageBox::Ok );
        ui->leAddr->setFocus();
        return;
    }
    if( ui->leValue->text().isEmpty() )
    {
        QMessageBox::critical( this, "PLC Value", tr("ADDRESS의 VALUE를 입력하세요."), QMessageBox::Ok );
        ui->leValue->setFocus();
        return;
    }
    bool bOk = false;
    QString section   = "";
    QString sDataAddr = ui->leAddr->text().toUpper().trimmed();
    QString sDataType = ui->cboBitWord->currentText().trimmed();
    QString sDataRW   = ui->cboRW->currentText().trimmed();
    QString sDataVal  = ui->leValue->text().trimmed();
    int nDataAddr     = sDataAddr.mid(1).toInt(&bOk);
    QString sDataDesc = ui->leDEscription->text().trimmed();

    if( g_pIniPLC->existSection(sDataAddr) )
    {
        QMessageBox::critical( this, "EXIST ERROR", tr("[%1] ADDRESS IS ALREADY EXIST!").arg(sDataAddr).arg(sDataType), QMessageBox::Ok );
        return;
    }

    section = tr("%1/%2").arg(sDataAddr).arg(INI_KEY_TYPE);
    bSaved = g_pIniPLC->writeSettings( section, sDataType );
    if( !bSaved )
    {
        QMessageBox::critical( this, "SAVE ERROR", tr("[%1]%2 Save Error").arg(section).arg(sDataType), QMessageBox::Ok );
        return;
    }

    section = tr("%1/%2").arg(sDataAddr).arg(INI_KEY_RW);
    bSaved = g_pIniPLC->writeSettings( section, sDataRW );
    if( !bSaved )
    {
        QMessageBox::critical( this, "SAVE ERROR", tr("[%1]%2 Save Error").arg(section).arg(sDataType), QMessageBox::Ok );
        return;
    }

    section = tr("%1/%2").arg(sDataAddr).arg(INI_KEY_VALUE);
    bSaved = g_pIniPLC->writeSettings( section, sDataVal );
    if( !bSaved )
    {
        QMessageBox::critical( this, "SAVE ERROR", tr("[%1]%2 Save Error").arg(section).arg(sDataType), QMessageBox::Ok );
        return;
    }

    section = tr("%1/%2").arg(sDataAddr).arg(INI_KEY_DESC);
    bSaved = g_pIniPLC->writeSettings( section, sDataDesc );
    if( !bSaved )
    {
        QMessageBox::critical( this, "SAVE ERROR", tr("[%1]%2 Save Error").arg(section).arg(sDataType), QMessageBox::Ok );
        return;
    }

    section = tr("%1/%2").arg(sDataAddr).arg(INI_KEY_ADDR);
    bSaved = g_pIniPLC->writeSettings( section, nDataAddr );
    if( !bSaved )
    {
        QMessageBox::critical( this, "SAVE ERROR", tr("[%1]%2 Save Error").arg(section).arg(nDataAddr), QMessageBox::Ok );
        return;
    }

    emit SigReloadConfigIni();
    QMessageBox::information( this, "ADDRESS SAVE", tr("PLC ADDRESS가 저장되었습니다."), QMessageBox::Ok );
}

void CPLCAddressDlg::SlotBtnCloseClicked()
{
    close();
}

void CPLCAddressDlg::SlotDeleteAddress(QString address)
{
    g_pIniPLC->removeSection(address);
    emit SigReloadConfigIni();
}

void CPLCAddressDlg::SlotUpdateAddressValue(QString address, QString new_value)
{
    QString section = tr("%1/%2").arg(address).arg(INI_KEY_VALUE);
    bool bSaved = g_pIniPLC->writeSettings( section, new_value );
    if( !bSaved )
    {
        QMessageBox::critical( this, "UPDATE ERROR", tr("[%1] Update to %2 Error").arg(section).arg(new_value), QMessageBox::Ok );
        return;
    }
    emit SigReloadConfigIni();
}
