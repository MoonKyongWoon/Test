#ifndef ERRORPOPUP_HPP
#define ERRORPOPUP_HPP

#include <QDialog>

namespace Ui {
class CErrorPopup;
}

enum EN_TREE_COLUMN
{
	EN_TREE_DATE   ,
	EN_TREE_DETAIL ,
};

#define TREE_ALARM_QCOLOR      QColor(255,  0,  0)


class CErrorPopup : public QDialog
{
    Q_OBJECT

public:
    explicit CErrorPopup(QWidget *parent = nullptr);
    ~CErrorPopup();
public slots:
    void SlotAppendErrorPopup( QString );
    void SlotClearList();
    void SlotClose();
private:
	void Init();
private:
    Ui::CErrorPopup *ui;
};

#endif // ERRORPOPUP_HPP
