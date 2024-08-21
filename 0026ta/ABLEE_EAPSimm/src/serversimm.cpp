#include "serversimm.hpp"

// TODO
// add ip, add port
CServerSimm::CServerSimm()
{
	this->initVariable();
	this->createServer();
}

CServerSimm::~CServerSimm()
{
	if (this->client)
		this->client->deleteLater();
	if (this->server)
		this->server->deleteLater();
	delete m_pSecs;
}

void CServerSimm::initVariable()
{
	this->server = nullptr;
	this->client = nullptr;
	this->hsms_status = HSMS_STATUS::NOT_CONNECTED;
	this->system_bytes = 0;
	this->b_connect = false;
	m_pSecs = new CSecsGem();
}

void CServerSimm::createServer()
{
	this->server = new CTcpServer;
	this->server->listen(QHostAddress::AnyIPv4, TEST_PORT);
	connect(this->server, &CTcpServer::SigIncommingConnection, this, &CServerSimm::slotIncommingConnection);
}

void CServerSimm::setHsmsStatus(HSMS_STATUS status)
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

uint CServerSimm::getSystemByte()
{
	return this->system_bytes;
}

bool CServerSimm::getConnect()
{
	return this->b_connect;
}

void CServerSimm::hsmsSelect(stHsmsElement element)
{
	stHsmsElement rsp;
	memcpy(&rsp, &element, sizeof(stHsmsElement));
	if (this->hsms_status == HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::COMMUNICATION_ALREADY_ACTIVE);
	}
	if (this->hsms_status == HSMS_STATUS::NOT_CONNECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::CONNECTION_NOT_READY);
	}
	if (this->hsms_status == HSMS_STATUS::NOT_SELECTED)
	{
		this->hsms_status = HSMS_STATUS::SELECTED;
		emit sigSetHsmsStatus(true);
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::COMMUNICATION_ESTABLISHED);
	}
	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::SELECT_RSP);
	char *data = new char[sizeof(stHsmsHeader)];
	memcpy(data, &rsp, sizeof(stHsmsHeader));
	this->client->SendPacket(data, sizeof(stHsmsHeader));
	if (data)
		delete[] data;
	QString bin_log = "---[SEND]---\n";
	bin_log += CEapUtil::Convert::convertFromControlMessageToQString(rsp);
	emit sigAppendBinLog(bin_log);
}

void CServerSimm::hsmsDeselect(stHsmsElement element)
{
	stHsmsElement rsp;
	memcpy(&rsp, &element, sizeof(stHsmsElement));
	// TODO
	// add DESELECT_STATUS::BUSY
	if (this->hsms_status == HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(DESELECT_STATUS::COMMUNICATION_ENDED);
		this->hsms_status = HSMS_STATUS::NOT_SELECTED;
		emit sigSetHsmsStatus(false);
	}
	else if (this->hsms_status != HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(DESELECT_STATUS::COMMUNICATION_NOT_ESTABLISHED);
	}
	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::DESELECT_RSP);
	char *data = new char[sizeof(stHsmsHeader)];
	memcpy(data, &rsp, sizeof(stHsmsHeader));
	this->client->SendPacket(data, sizeof(stHsmsHeader));
	if (data)
		delete[] data;
	QString bin_log = "---[SEND]---\n";
	bin_log += CEapUtil::Convert::convertFromControlMessageToQString(rsp);
	emit sigAppendBinLog(bin_log);
}

void CServerSimm::hsmsLinkTest(stHsmsElement element)
{
	stHsmsElement rsp;
	memcpy(&rsp, &element, sizeof(stHsmsElement));

	if (this->hsms_status != HSMS_STATUS::SELECTED)
	{
		return;
	}
	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::LINKTEST_RSP);
	char *data = new char[sizeof(stHsmsHeader)];
	memcpy(data, &rsp, sizeof(stHsmsHeader));
	this->client->SendPacket(data, sizeof(stHsmsHeader));
	if (data)
		delete[] data;
	QString bin_log = "---[SEND]---\n";
	bin_log += CEapUtil::Convert::convertFromControlMessageToQString(rsp);
	emit sigAppendBinLog(bin_log);
}

void CServerSimm::slotIncommingConnection(qintptr t)
{
	if (this->client)
	{
		CHsmsClient* temp_client = new CHsmsClient(t);
		delete temp_client;
		return ;
	}
	this->client = new CHsmsClient(t);
	this->b_connect = true;
	this->setHsmsStatus(HSMS_STATUS::CONNECTED);
	this->setHsmsStatus(HSMS_STATUS::NOT_SELECTED);
	connect(this->client,	&CHsmsClient::SigStatusChanged, this,			&CServerSimm::slotStatusChanged);
	connect(this->client,	&CHsmsClient::SigReceivePacket, this,			&CServerSimm::slotReceivePacket);
	connect(this,			&CServerSimm::sigSendPacket,	this->client,	&CHsmsClient::SlotSendPacket);
}

void CServerSimm::slotStatusChanged(bool bConn)
{
	if (bConn)
	{
		this->setHsmsStatus(HSMS_STATUS::CONNECTED);
		this->setHsmsStatus(HSMS_STATUS::NOT_SELECTED);
	}
	else
	{
		this->setHsmsStatus(HSMS_STATUS::NOT_CONNECTED);
		this->b_connect = false;
		if (this->client)
		{
			this->client->deleteLater();
			this->client = nullptr;
		}
	}
}

void CServerSimm::slotReceivePacket(stHsmsElement element)
{
	QString bin_log = "---[RECV]---\n";
	//CHsmsCnv converter;
	CSecsGem secs;
	atstring data_str;
	char* data = nullptr;
	int len = 0x0;
	//uint nRecvSysBytes = 0;
	switch (element.hsmsHeader.stMsgHeader.cSType)
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

			  emit sigAppendBinLog(bin_log);
			CSecsMsg* pMsg = secs.CreateMsg( data, len+4 );
			if( pMsg != nullptr )
			{
				short stream =0, function = 0, wbit = 0;
				int deviceid =0, rbit = 0;
				uint sysbyte = 0;
				secs.GetDeviceId(pMsg, deviceid);
				secs.GetRBit(pMsg, rbit);
				secs.GetMsgSML( pMsg, data_str );
				secs.GetMsgInfo( pMsg, stream, function, wbit );
				secs.GetSystemByte(pMsg, sysbyte);
				emit sigRecvSystemBytes( static_cast<uint>(sysbyte) );
				emit sigAppendHsmsInfo( tr("[RECV]S%1F%2 Wbit:%3 SystemBytes:%4 DeviceId:%5 Rbit:%6").arg(stream).arg(function).arg(wbit).arg(sysbyte).arg(deviceid).arg(rbit) );
				emit sigAppendSmlLog( QString(data_str).replace("\t", "    "));
				secs.CloseMsg( pMsg );
			}
			else
			{
				emit sigAppendSmlLog( "error" );
			}
			//converter.HsmsToSml(data, len + 4, data_str);
			if (element.pData != nullptr )
				delete[] element.pData;
			if (data != nullptr )
				delete[] data;
		}
		break;

		case 1:
			emit sigAppendSystemLog("RECV SELECT REQ");
			bin_log += CEapUtil::Convert::convertFromControlMessageToQString(element);
			emit sigAppendBinLog(bin_log);
			this->hsmsSelect(element);
			break;
		case 2:
			emit sigAppendSystemLog("RECV SELECT RSP");
			break;
		case 3:
			emit sigAppendSystemLog("RECV DESELECT REQ");
			bin_log += CEapUtil::Convert::convertFromControlMessageToQString(element);
			emit sigAppendBinLog(bin_log);
			this->hsmsDeselect(element);
			break;
		case 4:
			emit sigAppendSystemLog("RECV DESELECT RSP");
			break;
		case 5:
			emit sigAppendSystemLog("RECV LINKTEST REQ");
			bin_log += CEapUtil::Convert::convertFromControlMessageToQString(element);
			emit sigAppendBinLog(bin_log);
			this->hsmsLinkTest(element);
			break;
		case 6:
			emit sigAppendSystemLog("RECV LINKTEST RSP");
			break;
		case 7:
			emit sigAppendSystemLog("RECV REJECT REQ");
			break;
		case 9:
			emit sigAppendSystemLog("RECV SEPERATE REQ");
			bin_log += CEapUtil::Convert::convertFromControlMessageToQString(element);
			emit sigAppendBinLog(bin_log);
			this->hsms_status = HSMS_STATUS::NOT_SELECTED;
			emit sigSetHsmsStatus(false);
			break;
		default:
			break;
	}
}
