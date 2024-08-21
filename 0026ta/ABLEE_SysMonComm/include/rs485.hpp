#ifndef CRS485_HPP
#define CRS485_HPP

#include <QObject>

class CRS485 : public QObject
{
	Q_OBJECT
public:
	explicit CRS485(QObject *parent = nullptr);

signals:

};

#endif // CRS485_HPP
