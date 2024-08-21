#ifndef CSITEMON_HPP
#define CSITEMON_HPP

#include <QObject>

class CSiteMon : public QObject
{
	Q_OBJECT
public:
	explicit CSiteMon(QObject *parent = nullptr);

signals:

};

#endif // CSITEMON_HPP
