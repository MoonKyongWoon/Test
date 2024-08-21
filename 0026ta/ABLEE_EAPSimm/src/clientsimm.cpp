#include "clientsimm.hpp"

CClientSimm::CClientSimm()
{
	this->initVariable();
	this->createSocket();
	this->createList();
	this->createTimer();
	this->startTimer();
}

CClientSimm::~CClientSimm()
{
	if (this->socket)
		this->socket->deleteLater();
	if (this->timer)
	{
		if (this->timer->isActive())
			this->timer->stop();
		this->timer->deleteLater();
	}
	if (this->control_message_list)
	{
		this->clearMessageList();
//		qDeleteAll(*this->control_message_list);
//		this->control_message_list->clear();
		delete this->control_message_list;
		this->control_message_list = nullptr;
	}
}

void CClientSimm::initVariable()
{
	this->socket = nullptr;
	this->timer = nullptr;
	this->control_message_list = nullptr;
	this->system_bytes = 0;
}

void CClientSimm::createList()
{
	this->control_message_list = new QList<stControlListItem*>;
}

void CClientSimm::createTimer()
{
	this->timer = new QTimer;
	connect(this->timer, &QTimer::timeout, this, &CClientSimm::slotCheckList);
}

void CClientSimm::clearMessageList()
{
	if (this->control_message_list)
	{
		this->control_message_list->erase(this->control_message_list->begin(), this->control_message_list->end());
	}
}

void CClientSimm::slotCheckList()
{
	QList<stControlListItem*>::iterator iter = this->control_message_list->begin();
	while (iter != this->control_message_list->end())
	{
		stControlListItem *item = *iter;
		if (item->count <= 0)
		{
			if (item->timeout_type == TIME_OUT_TYPE::T6)
			{
				this->clearMessageList();
				qDebug() << "ERROR!";
				emit sigDisconnectFromHost();
				return ;
			}
		}
		item->count -= 1;
		iter++;
	}
}

void CClientSimm::createSocket()
{
	this->socket = new CHsmsSocket;
	this->socket->SetHostInfo(TEST_IP, TEST_PORT);
	connect(this,			&CClientSimm::sigConnectToHost,			this->socket,	&CHsmsSocket::SlotConnectToHost);
	connect(this,			&CClientSimm::sigDisconnectFromHost,	this->socket,	&CHsmsSocket::SlotDisconnectFromHost);
	connect(this->socket,	&CHsmsSocket::SigStatusChanged,			this,			&CClientSimm::slotStatusChanged);
	connect(this->socket,	&CHsmsSocket::SigReceivePacket,			this,			&CClientSimm::slotRecvPacket);
	connect(this,			&CClientSimm::sigSendPacket,			this->socket,	&CHsmsSocket::SlotSendPacket);
}

void CClientSimm::startTimer()
{
	this->timer->start(1000);
}

void CClientSimm::addItemToList(TIME_OUT_TYPE type, int system_byte)
{
	stControlListItem *item = new stControlListItem;
	item->timeout_type = type;
	item->system_byte = system_byte;
	switch (type)
	{
		case TIME_OUT_TYPE::T3:
			item->count = T3_TYPICAL_VALUE;
			break;
		case TIME_OUT_TYPE::T5:
			item->count = T5_TYPICAL_VALUE;
			break;
		case TIME_OUT_TYPE::T6:
			item->count = T6_TYPICAL_VALUE;
			break;
		case TIME_OUT_TYPE::T7:
			item->count = T7_TYPICAL_VALUE;
			break;
		case TIME_OUT_TYPE::T8:
			item->count = T8_TYPICAL_VALUE;
			break;
	}
	this->control_message_list->append(item);
	qDebug() << "ADD SYSTEM BYTE : " << item->system_byte;
}

void CClientSimm::setHsmsStatus(HSMS_STATUS status)
{
	this->hsms_status = status;
	switch(status)
	{
		case HSMS_STATUS::NOT_CONNECTED:
			emit sigSetTcpStatus(false);
			break;
		case HSMS_STATUS::CONNECTED:
			emit sigSetTcpStatus(true);
			break;
		case HSMS_STATUS::NOT_SELECTED:
			emit sigSetHsmsStatus(false);
			break;
		case HSMS_STATUS::SELECTED:
			emit sigSetHsmsStatus(true);
			break;
		default:
			break;
	}
}

void CClientSimm::hsmsSelect(stHsmsElement element)
{
	if (element.hsmsHeader.stMsgHeader.cFunction == static_cast<char>(SELECT_STATUS::COMMUNICATION_ESTABLISHED))
	{
		emit sigSetHsmsStatus(true);
	}
}

void CClientSimm::hsmsDeselect(stHsmsElement element)
{
	if (element.hsmsHeader.stMsgHeader.cFunction == static_cast<char>(DESELECT_STATUS::COMMUNICATION_ENDED))
	{
		emit sigSetHsmsStatus(false);
	}
}

uint CClientSimm::getSystemByte()
{
	return this->system_bytes;
}

void CClientSimm::slotConnectToHost()
{
	emit this->sigConnectToHost();
}

void CClientSimm::slotDisconnectFromHost()
{
	emit this->sigDisconnectFromHost();
}

void CClientSimm::slotStatusChanged(bool bConn)
{
	if (bConn)
	{
		this->setHsmsStatus(HSMS_STATUS::CONNECTED);
		this->setHsmsStatus(HSMS_STATUS::NOT_SELECTED);
	}
	else
	{
		this->setHsmsStatus(HSMS_STATUS::NOT_CONNECTED);
	}
}

void CClientSimm::makeSelectRequestMessage(stHsmsElement &element)
{
	element.hsmsHeader.stMsgHeader.cDevidMsb = 0b00011000;
	element.hsmsHeader.stMsgHeader.cDevidLsb = 0b00111000;
	element.hsmsHeader.stMsgHeader.cByte2 = 0;
	element.hsmsHeader.stMsgHeader.cFunction = 0;
	element.hsmsHeader.stMsgHeader.cPType = 0;
	element.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::SELECT_REQ);
    for( int i=0;i<4;i++ )
	{
        element.hsmsHeader.stMsgHeader.cSystemBytes[i] = (this->system_bytes >>(24-i*8)&0xFF);
    }
	this->system_bytes += 1;

	int MsgLen = sizeof(stHsmsMsgHeader);
	element.hsmsHeader.szMsgLength[0] = (MsgLen<<24)&0xFF;
	element.hsmsHeader.szMsgLength[1] = (MsgLen<<16)&0xFF;
	element.hsmsHeader.szMsgLength[2] = (MsgLen<< 8)&0xFF;
	element.hsmsHeader.szMsgLength[3] = (MsgLen<< 0)&0xFF;
}

void CClientSimm::makeDeselectRequestMessage(stHsmsElement &element)
{
	element.hsmsHeader.stMsgHeader.cDevidMsb = 0b00011000;
	element.hsmsHeader.stMsgHeader.cDevidLsb = 0b00111000;
	element.hsmsHeader.stMsgHeader.cByte2 = 0;
	element.hsmsHeader.stMsgHeader.cFunction = 0;
	element.hsmsHeader.stMsgHeader.cPType = 0;
	element.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::DESELECT_REQ);
    for( int i=0;i<4;i++ )
        element.hsmsHeader.stMsgHeader.cSystemBytes[i] = (this->system_bytes >>(24-i*8)&0xFF);

//	element.hsmsHeader.stMsgHeader.nSystemBytes = this->system_bytes;
	this->system_bytes += 1;

	int MsgLen = sizeof(stHsmsMsgHeader);
	element.hsmsHeader.szMsgLength[0] = (MsgLen<<24)&0xFF;
	element.hsmsHeader.szMsgLength[1] = (MsgLen<<16)&0xFF;
	element.hsmsHeader.szMsgLength[2] = (MsgLen<< 8)&0xFF;
	element.hsmsHeader.szMsgLength[3] = (MsgLen<< 0)&0xFF;
}

void CClientSimm::makeSeperateRequestMessage(stHsmsElement &element)
{
	element.hsmsHeader.stMsgHeader.cDevidMsb = 0b00011000;
	element.hsmsHeader.stMsgHeader.cDevidLsb = 0b00111000;
	element.hsmsHeader.stMsgHeader.cByte2 = 0;
	element.hsmsHeader.stMsgHeader.cFunction = 0;
	element.hsmsHeader.stMsgHeader.cPType = 0;
	element.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::SEPARATE_REQ);
    for( int i=0;i<4;i++ )
        element.hsmsHeader.stMsgHeader.cSystemBytes[i] = (this->system_bytes >>(24-i*8)&0xFF);

    //element.hsmsHeader.stMsgHeader.nSystemBytes = this->system_bytes;
	this->system_bytes += 1;

	int MsgLen = sizeof(stHsmsMsgHeader);
	element.hsmsHeader.szMsgLength[0] = (MsgLen<<24)&0xFF;
	element.hsmsHeader.szMsgLength[1] = (MsgLen<<16)&0xFF;
	element.hsmsHeader.szMsgLength[2] = (MsgLen<< 8)&0xFF;
	element.hsmsHeader.szMsgLength[3] = (MsgLen<< 0)&0xFF;

	this->hsms_status = HSMS_STATUS::NOT_SELECTED;
	emit sigSetHsmsStatus(false);
}

void CClientSimm::makeLinkTestRequestMessage(stHsmsElement &element)
{
    element.hsmsHeader.stMsgHeader.cDevidMsb = 0b1111111;
	element.hsmsHeader.stMsgHeader.cDevidLsb = 0b11111111;
	element.hsmsHeader.stMsgHeader.cByte2 = 0;
	element.hsmsHeader.stMsgHeader.cFunction = 0;
	element.hsmsHeader.stMsgHeader.cPType = 0;
	element.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::LINKTEST_REQ);
    for( int i=0;i<4;i++ )
	{
        element.hsmsHeader.stMsgHeader.cSystemBytes[i] = (this->system_bytes >>(24-i*8)&0xFF);
    }

    //element.hsmsHeader.stMsgHeader.nSystemBytes = this->system_bytes;
	this->system_bytes += 1;

	int MsgLen = sizeof(stHsmsMsgHeader);
	element.hsmsHeader.szMsgLength[0] = (MsgLen<<24)&0xFF;
	element.hsmsHeader.szMsgLength[1] = (MsgLen<<16)&0xFF;
	element.hsmsHeader.szMsgLength[2] = (MsgLen<< 8)&0xFF;
	element.hsmsHeader.szMsgLength[3] = (MsgLen<< 0)&0xFF;

	//element.hsmsHeader.nMsgLength = sizeof(stHsmsMsgHeader);
}

void CClientSimm::makeRejectRequestMessage(stHsmsElement &element)
{
	element.hsmsHeader.stMsgHeader.cDevidMsb = 0b00000000;
	element.hsmsHeader.stMsgHeader.cDevidLsb = 0b00000000;
	element.hsmsHeader.stMsgHeader.cByte2 = 0b01010101; // temp -> rejected message's s type or p type
	element.hsmsHeader.stMsgHeader.cFunction = 0b01010101; // temp -> reason code
	element.hsmsHeader.stMsgHeader.cPType = 0;
	element.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::REJECT_REQ);
    for( int i=0;i<4;i++ )
        element.hsmsHeader.stMsgHeader.cSystemBytes[i] = (this->system_bytes >>(24-i*8)&0xFF);

    //element.hsmsHeader.stMsgHeader.nSystemBytes = this->system_bytes;
	this->system_bytes += 1;

	int MsgLen = sizeof(stHsmsMsgHeader);
	element.hsmsHeader.szMsgLength[0] = (MsgLen<<24)&0xFF;
	element.hsmsHeader.szMsgLength[1] = (MsgLen<<16)&0xFF;
	element.hsmsHeader.szMsgLength[2] = (MsgLen<< 8)&0xFF;
	element.hsmsHeader.szMsgLength[3] = (MsgLen<< 0)&0xFF;
}

//void CClientSimm::slotSendDataMessage(QByteArray data)
//{
//
//}

void CClientSimm::slotSendControlMessage(SType s_type)
{
	if (!this->socket->IsConn())
	{
		emit sigAppendSystemLog("TCP NOT CONNEDTED");
		return ;
	}

	stHsmsElement element;
	memset(&element, 0, sizeof(stHsmsElement));
	element.pData = nullptr;

	switch(s_type)
	{
		case SType::SELECT_REQ:
			this->makeSelectRequestMessage(element);
			break;
		case SType::DESELECT_REQ:
			this->makeDeselectRequestMessage(element);
			break;
		case SType::SEPARATE_REQ:
			this->makeSeperateRequestMessage(element);
			break;
		case SType::LINKTEST_REQ:
			this->makeLinkTestRequestMessage(element);
			break;
		case SType::REJECT_REQ:
			this->makeRejectRequestMessage(element);
			break;
		default:
			return ;
	}

	char* data = new char[sizeof(stHsmsHeader)];
	memcpy(data, &element, sizeof(stHsmsHeader));
	this->socket->SendPacket(data, sizeof(stHsmsHeader));

	if (s_type != SType::SEPARATE_REQ && s_type != SType::REJECT_REQ)
	{
        uint nSysBytes = 0;
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[0] << 24);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[1] << 16);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[2] <<  8);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[3] <<  0);
        this->addItemToList(TIME_OUT_TYPE::T6, nSysBytes );
    }
	//TODO after recv, find system_byte, and stop timer and erase from list

	emit sigAppendBinLog("---[SEND]---\n");
	emit sigAppendBinLog(CEapUtil::Convert::convertFromControlMessageToQString(element));
}

void CClientSimm::slotRecvPacket(stHsmsElement element)
{
	QString bin_log = "---[RECV]---\n";
	bin_log += CEapUtil::Convert::convertFromControlMessageToQString(element);
	emit sigAppendBinLog(bin_log);

	QList<stControlListItem*>::iterator iter = this->control_message_list->begin();
	while (iter != this->control_message_list->end())
	{
		stControlListItem *item = *iter;
        uint nSysBytes = 0;
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[0] << 24);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[1] << 16);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[2] <<  8);
        nSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[3] <<  0);
		if (item->system_byte == nSysBytes )
		{
			this->control_message_list->erase(iter);
			qDebug() << "RECV SYSTEM BYTE : " << item->system_byte;
			break;
		}
		iter++;
	}

	CSecsGem secsgem;
	//CHsmsCnv converter;
	atstring data_str;
	char* data = nullptr;
	int len = 0x0;
	uint nRecvSysBytes = 0;

	switch(element.hsmsHeader.stMsgHeader.cSType)
	{
		case 0:
		{
			emit sigAppendSystemLog("RECV DATA MESSAGE");
			len |= (element.hsmsHeader.szMsgLength[0] << 24);
			len |= (element.hsmsHeader.szMsgLength[1] << 16);
			len |= (element.hsmsHeader.szMsgLength[2] << 8);
			len |= (element.hsmsHeader.szMsgLength[3] << 0);
			data = new char[len + 4];
			memcpy(data, element.hsmsHeader.szMsgLength, sizeof(element.hsmsHeader.szMsgLength));
			memcpy(data + sizeof(element.hsmsHeader.szMsgLength), &element.hsmsHeader.stMsgHeader, sizeof(element.hsmsHeader.stMsgHeader));
			memcpy(data + sizeof(element.hsmsHeader.szMsgLength) + sizeof(element.hsmsHeader.stMsgHeader), element.pData, len - sizeof(element.hsmsHeader.stMsgHeader));
			for (int i = 4; i < len + 4; i += 1)
			{
//				qDebug() << tr("[%1] : %2").arg(i).arg(CEapUtil::Convert::convertFromCharToBitsQString(data[i]));
//				qDebug() << tr("[%1] : %2").arg(i).arg(QString("").sprintf("%02X",data[i]));
				bin_log+= tr("[%1] : %2\n").arg(i).arg(CEapUtil::Convert::convertFromCharToBitsQString(data[i]));
//				emit sigAppendBinLog(tr("[%1] : %2").arg(i).arg(CEapUtil::Convert::convertFromCharToBitsQString(data[i])));
			}
			nRecvSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[0] << 24 );
			nRecvSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[1] << 16 );
			nRecvSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[2] <<  8 );
			nRecvSysBytes |= ( element.hsmsHeader.stMsgHeader.cSystemBytes[3] <<  0 );
			this->system_bytes = nRecvSysBytes;
//			emit sigRecvSystemBytes( nRecvSysBytes );
			emit sigAppendBinLog(bin_log);
			CSecsMsg* pMsg = secsgem.CreateMsg( data, len+4 );
			if( pMsg != nullptr )
			{
				secsgem.GetMsgSML( pMsg, data_str );
				emit sigAppendSmlLog(QString(data_str).replace("\t", "    "));
				delete pMsg;
			}
			if (element.pData != nullptr )
				delete[] element.pData;
			if (data != nullptr )
				delete[] data;
		}
		break;

		case 1:
			emit sigAppendSystemLog("RECV SELECT REQ");
			break;
		case 2:
			emit sigAppendSystemLog("RECV SELECT RSP");
			hsmsSelect(element);
			break;
		case 3:
			emit sigAppendSystemLog("RECV DESELECT REQ");
			break;
		case 4:
			emit sigAppendSystemLog("RECV DESELECT RSP");
			hsmsDeselect(element);
			break;
		case 5:
			emit sigAppendSystemLog("RECV LINKTEST REQ");
			break;
		case 6:
			emit sigAppendSystemLog("RECV LINKTEST RSP");
			break;
		case 7:
			emit sigAppendSystemLog("RECV REJECT REQ");
			break;
		case 9:
			emit sigAppendSystemLog("RECV SEPERATE REQ");
			break;
		default:
			break;
	}
}
