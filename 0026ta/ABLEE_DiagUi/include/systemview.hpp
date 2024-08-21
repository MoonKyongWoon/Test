#ifndef SYSTEMVIEW_HPP
#define SYSTEMVIEW_HPP

#include <QWidget>

namespace Ui {
	class CSystemView;
}

class CSystemView : public QWidget
{
	Q_OBJECT

public:
	explicit CSystemView(QWidget *parent = nullptr);
	~CSystemView();

private:
	Ui::CSystemView *ui;
};

#endif // SYSTEMVIEW_HPP
