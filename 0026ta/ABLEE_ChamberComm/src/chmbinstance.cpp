#include "chmbinstance.hpp"

CChmbInstance::CChmbInstance(quintptr handle, QObject *parent) : QObject(parent)
{
	m_sIpcName = "";
	m_pYkgPlc  = new CYokogawaPlc();
	m_pLocIpc  = new CLocalClient( handle );
	connect( m_pLocIpc, &CLocalClient::SigIpcConnectionStatus , this      , &CChmbInstance::SlotIpcStatusChanged );
	connect( m_pLocIpc, &CLocalClient::SigIpcRecvPacket       , this      , &CChmbInstance::SlotIpcRecvPacket    );
	connect( m_pLocIpc, &CLocalClient::SigIpcLogMessage       , this      , &CChmbInstance::SlotIpcLogMessage    );
	connect( this     , &CChmbInstance::SigIpcSendPacket      , m_pLocIpc , &CLocalClient::SlotSendPacket   );
}

CChmbInstance::~CChmbInstance()
{
	if( m_pLocIpc )
	{
		m_pLocIpc->DisconnectServer();
		m_pLocIpc->deleteLater();
	}
	if( m_pYkgPlc )
	{
		m_pYkgPlc->deleteLater();
	}
}

void CChmbInstance::SetPlcTcpInfo(QString ip, uint port)
{
	if( m_pYkgPlc != nullptr )
	{
		m_pYkgPlc->SetPlcTcpInfo( ip, port );
	}
}

void CChmbInstance::SlotIpcStatusChanged(bool conn)
{
	if( !conn )
		emit SigIpcDisconnected( m_sIpcName );
}

void CChmbInstance::SlotIpcRecvPacket(stIpcElement ipcelement)
{

}

void CChmbInstance::SlotIpcLogMessage(LOG_TYPE type, QString msg)
{

}

void CChmbInstance::SlotPlcLogMessage( LOG_TYPE type, QString msg )
{

}
