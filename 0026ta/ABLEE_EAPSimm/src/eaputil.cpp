#include "eaputil.hpp"

QString CEapUtil::Convert::convertFromCharToBitsQString(char data)
{
	QString rtn = "";
	for (int i = 7; i >= 0; i -= 1)
		rtn += QString::number((data >> i) & 1);
	return rtn;
}

QString CEapUtil::Convert::convertFromIntToBitsQString(int data)
{
	QString rtn = "";
	std::bitset<32> bit_value(data);
	for (int i = 31; i >= 0; i -= 1)
		rtn += bit_value.test(i) ? "1" : "0";
	return rtn;
}

QString CEapUtil::Convert::convertFromControlMessageToQString(stHsmsElement element)
{
	QString rtn = "";

	int nMsgLength = 0x0;
	nMsgLength |= (element.hsmsHeader.szMsgLength[0] << 24 );
	nMsgLength |= (element.hsmsHeader.szMsgLength[1] << 16 );
	nMsgLength |= (element.hsmsHeader.szMsgLength[2] <<  8 );
	nMsgLength |= (element.hsmsHeader.szMsgLength[3] <<  0 );

	QString bit_length = CEapUtil::Convert::convertFromIntToBitsQString(nMsgLength);
	rtn += "LENGTH\n";
	for (int i = 0; i < 4; i += 1)
	{
		rtn += QObject::tr("  [%1] : %2\n").arg(i).arg(bit_length.mid(i * 8, 8));
	}

	rtn += "HEADER\n";

	rtn += QObject::tr("  [%1] : %2\n").arg(0).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cDevidMsb & 0xFF));
	rtn += QObject::tr("  [%1] : %2\n").arg(1).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cDevidLsb & 0xFF));
	rtn += QObject::tr("  [%1] : %2\n").arg(2).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cByte2 & 0xFF));
	rtn += QObject::tr("  [%1] : %2\n").arg(3).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cFunction & 0xFF));
	rtn += QObject::tr("  [%1] : %2\n").arg(4).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cPType & 0xFF));
	rtn += QObject::tr("  [%1] : %2\n").arg(5).arg(CEapUtil::Convert::convertFromCharToBitsQString(element.hsmsHeader.stMsgHeader.cSType & 0xFF));

    //QString bit_system_bytes = CEapUtil::Convert::convertFromIntToBitsQString(element.hsmsHeader.stMsgHeader.nSystemBytes);
	for (int i = 0; i < 4; i += 1)
        rtn += QObject::tr("  [%1] : %2\n").arg(i + 6).arg( QString("").asprintf("%02X", element.hsmsHeader.stMsgHeader.cSystemBytes[i]));
	return rtn;
}
