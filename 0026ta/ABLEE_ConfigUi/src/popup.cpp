#include "popup.hpp"
#include "ui_popup.h"

Popup::Popup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Popup)
{
    ui->setupUi(this);
}

Popup::~Popup()
{
    delete ui;
}
