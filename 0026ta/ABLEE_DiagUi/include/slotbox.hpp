#ifndef SLOTBOX_HPP
#define SLOTBOX_HPP

#include <QCheckBox>
#include <QLabel>
#include <QWidget>
#include "sysdef.hpp"

namespace Ui {
class CSlotBox;
}

class CSlotBox : public QWidget
{
    Q_OBJECT

public:
    explicit CSlotBox(QWidget *parent = nullptr);
    ~CSlotBox();
private:
    void InitVariables();
    void InitUi();
    void InitUiConnStatus();
    void InitUiCheckStatus();
    void InitPixmap();
    void InitSigSlots();
public slots:
	void SlotAllSlotSelected(int);
private:
    Ui::CSlotBox *ui;
	QCheckBox* m_pchkSlot[MAX_SLOT];
	QCheckBox* m_pchkAllSlot;
	QLabel*    m_pLblStat[MAX_SLOT];
	QPixmap    m_pixmapR;
    QPixmap    m_pixmapG;
};

#endif // SLOTBOX_HPP
