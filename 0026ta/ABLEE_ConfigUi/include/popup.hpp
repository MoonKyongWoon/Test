#ifndef POPUP_HPP
#define POPUP_HPP

#include <QWidget>

namespace Ui {
class Popup;
}

class Popup : public QWidget
{
    Q_OBJECT

public:
    explicit Popup(QWidget *parent = nullptr);
    ~Popup();

private:
    Ui::Popup *ui;
};

#endif // POPUP_H
