#ifndef CLOGGER_HPP
#define CLOGGER_HPP

#include <QObject>

class CLogger : public QObject
{
	Q_OBJECT
public:
	explicit CLogger(QObject *parent = nullptr);

signals:

};

#endif // CLOGGER_HPP
