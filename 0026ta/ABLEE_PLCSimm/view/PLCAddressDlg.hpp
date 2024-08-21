#ifndef PLCADDRESSDLG_HPP
#define PLCADDRESSDLG_HPP

#include <QDialog>
#include <QMessageBox>
#include "Common.hpp"
#include "Util.hpp"
#include "IniHandler.hpp"

extern CIniHandler* g_pIniPLC;        //Global Instance

namespace Ui {
class CPLCAddressDlg;
}

class CPLCAddressDlg : public QDialog
{
    Q_OBJECT
private:
    void InitUi();
    void InitSigSlots();
public:
    explicit CPLCAddressDlg(QWidget *parent = nullptr);
    ~CPLCAddressDlg();
public:
    void SetAddressType(int);
signals:
    void SigReloadConfigIni();
public slots:
    void SlotBtnAddClicked();
    void SlotBtnCloseClicked();
    void SlotUpdateAddressValue(QString,QString);
    void SlotDeleteAddress(QString);
private:
    Ui::CPLCAddressDlg *ui;
};

#endif // PLCADDRESSDLG_HPP
