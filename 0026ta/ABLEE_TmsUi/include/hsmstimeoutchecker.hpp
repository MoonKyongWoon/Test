#ifndef CHSMSTIMEOUTCHECKER_HPP
#define CHSMSTIMEOUTCHECKER_HPP

#include <QObject>
#include <QMap>
#include <QDebug>
#include "commstruct.hpp"
#include "sysini.hpp"
#include "hsms.hpp"

typedef struct _stHsmsTimeOut
{
	TIMEOUT_TYPE timeOutType;
	double timerCount;
	unsigned int retrialCount;
	unsigned int dataSize;
	char* hsmsMsg;
}stHsmsTimeOut;

class CHsmsTimeOutChecker : public QObject
{
	Q_OBJECT
public:
	explicit CHsmsTimeOutChecker(QObject *parent = nullptr);
	~CHsmsTimeOutChecker();

private:
	QMap<unsigned int, stHsmsTimeOut> m_mapTimeOuts;

public:
	unsigned int m_uTimeOut[KEY_TIMEOUT_MAX];

private:
	void InitVariable();

public:
	void AddTimeOut(unsigned int, TIMEOUT_TYPE, unsigned int, char*);
	bool IsTimeOutEmpty();
	bool RemoveTimeOut(unsigned int);
	void RemoveAllTimeOut();
	void CheckTimeOut();

public slots:

signals:
	void SigResendHsmsPacket(char*, unsigned int);
	void SigSeparationOver();
	void SigCommunicationFailure();
};

#endif // CHSMSTIMEOUTCHECKER_HPP
