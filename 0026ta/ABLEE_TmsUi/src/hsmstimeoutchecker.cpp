#include "hsmstimeoutchecker.hpp"

CHsmsTimeOutChecker::CHsmsTimeOutChecker(QObject *parent) : QObject(parent)
{
	InitVariable();
}

CHsmsTimeOutChecker::~CHsmsTimeOutChecker()
{
	if (!m_mapTimeOuts.isEmpty())
	{
		RemoveAllTimeOut();
	}
}

void CHsmsTimeOutChecker::InitVariable()
{
	m_mapTimeOuts.clear();

	m_uTimeOut[0] = DEFAULT_TMS_T3_TIME;
	m_uTimeOut[1] = DEFAULT_TMS_T5_TIME;
	m_uTimeOut[2] = DEFAULT_TMS_T6_TIME;
	m_uTimeOut[3] = DEFAULT_TMS_T7_TIME;
	m_uTimeOut[4] = DEFAULT_TMS_T8_TIME;
}

void CHsmsTimeOutChecker::AddTimeOut(unsigned int systemBytes, TIMEOUT_TYPE timeOutType, unsigned int dataSize, char* hsmsMsg)
{
	stHsmsTimeOut HsmsTimeOut;
	HsmsTimeOut.hsmsMsg = nullptr;
	HsmsTimeOut.hsmsMsg = new char[dataSize];

	memcpy(HsmsTimeOut.hsmsMsg, hsmsMsg, dataSize);

	HsmsTimeOut.timeOutType = timeOutType;

	switch (timeOutType)
	{
		case TIMEOUT_TYPE::_T3:
			HsmsTimeOut.retrialCount = 0;//3;

			break;
		case TIMEOUT_TYPE::_T5:
		case TIMEOUT_TYPE::_T6:
		case TIMEOUT_TYPE::_T7:
			HsmsTimeOut.retrialCount = 0;

			break;
		default:

			break;
	}

	HsmsTimeOut.timerCount = m_uTimeOut[static_cast<unsigned int>(timeOutType)];
	HsmsTimeOut.dataSize = dataSize;

	m_mapTimeOuts.insert(systemBytes, HsmsTimeOut);
}

bool CHsmsTimeOutChecker::IsTimeOutEmpty()
{
	return m_mapTimeOuts.empty();
}

bool CHsmsTimeOutChecker::RemoveTimeOut(unsigned int systemBytes)
{
	if(m_mapTimeOuts.contains(systemBytes))
	{
		if (m_mapTimeOuts.find(systemBytes).value().hsmsMsg != nullptr)
		{
			delete[] m_mapTimeOuts.find(systemBytes).value().hsmsMsg;
			m_mapTimeOuts.find(systemBytes).value().hsmsMsg = nullptr;
		}

		m_mapTimeOuts.remove(systemBytes);

		return true;
	}
	else
	{
		// ignore
		return false;
	}
}

void CHsmsTimeOutChecker::RemoveAllTimeOut()
{
	for (auto iter : m_mapTimeOuts)
	{
		if (iter.hsmsMsg != nullptr)
		{
			delete[] iter.hsmsMsg;
			iter.hsmsMsg = nullptr;
		}
	}

	m_mapTimeOuts.clear();
}

void CHsmsTimeOutChecker::CheckTimeOut()
{
	if (m_mapTimeOuts.isEmpty())
	{
	}
	else
	{
		QMap<unsigned int, stHsmsTimeOut>::iterator iter = m_mapTimeOuts.begin();

		do
		{
			iter.value().timerCount -= 1;

			if (iter.value().timerCount == 0)
			{

				if (iter.value().retrialCount > 0)
				{
					iter.value().retrialCount--;
					iter.value().timerCount = m_uTimeOut[static_cast<int>(iter.value().timeOutType)];

					emit SigResendHsmsPacket(iter.value().hsmsMsg, iter.value().dataSize);
				}
				else
				{
					if (iter.value().timeOutType == TIMEOUT_TYPE::_T6)
					{
						emit SigCommunicationFailure();
					}

					if (iter.value().hsmsMsg != nullptr)
					{
						delete[] iter.value().hsmsMsg;
						iter.value().hsmsMsg = nullptr;
					}

					m_mapTimeOuts.erase(iter++);
					continue;
				}
			}

			iter++;
		}while(iter != m_mapTimeOuts.end());
	}
}
