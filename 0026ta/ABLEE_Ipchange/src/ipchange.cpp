#include <stdlib.h>
#include "ipchange.hpp"
#include "bitchart.hpp"

CIpchange::CIpchange()
{

}

CIpchange::~CIpchange()
{
}

int CIpchange::Run()
{
	int rtn = 0;
	Ipchange( GetSlotId() );
	return rtn;
}

int CIpchange::Run(int id)
{
	int rtn = 0;
	Ipchange(id);
	return rtn;
}

void CIpchange::Test() {
	for (int i = -5; i < 40; i++)
		Run(i);
}

bool CIpchange::Ipchange(int id)
{
	FILE* fp = fopen("/home/ablee/.ipchange.log", "w");
	char szCmd[128] = {0x0,};
	char buffer[128] = {0x0,};
	int ip = 62;

	snprintf(buffer, sizeof(buffer), "======================================\nSLOT ID\t\t:\t%d\nOriginal IP\t:\t", id);
	fputs(buffer, fp);
	fclose(fp);
	SaveIp();

	switch ( id )
	{
		case 0x0 :
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 :
		case 0x6 :
		case 0x7 :
		case 0x8 :
		case 0x9 :
		case 0xA :
		case 0xB :
		case 0xC :
		case 0xD :
		case 0xE :
		case 0xF :
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
			ip = id+1;
			break;
		default:
			std::system("echo 'SLOT ID is not valuable'>> /home/ablee/.ipchange.log");
			return false;
			break;
	}
	snprintf( szCmd, sizeof(szCmd), "/sbin/ifconfig eth0 inet 192.168.62.%d netmask 255.255.255.0", ip );
	std::system(szCmd);

	fp = fopen("/home/ablee/.ipchange.log", "a");
	snprintf(buffer, sizeof(buffer), "Changed IP\t:\t");
	fputs(buffer, fp);
	fclose(fp);

	SaveIp();

	return true;
}

void CIpchange::SaveIp(void) {
	char ipSaveCmd[128] = {0x0,};
	snprintf(ipSaveCmd, sizeof(ipSaveCmd), "/sbin/ifconfig eth0 | grep 'inet ' | awk '{print $2}' >> /home/ablee/.ipchange.log");
	std::system(ipSaveCmd);
}

bool CIpchange::OpenPcie()
{
	m_pcie = new CPcie();
	m_pcie->SetPcieTPG( PCIE_TPG0 );
	if( !m_pcie->pcie_open() )
	{
		m_pcie->pcie_close();
		m_pcie->SetPcieTPG( PCIE_TPG1 );
		if( !m_pcie->pcie_open() )
			return false;
	}
	return true;
}

uint CIpchange::GetSlotId()
{
	if( !OpenPcie() )
		return -1;
	int slot_id = m_pcie->read_user( BITCHART::DRV_UCLK::_SLOT_ID );
	m_pcie->pcie_close();
	return slot_id;
}
