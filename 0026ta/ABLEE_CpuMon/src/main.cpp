#include <QCoreApplication>
#include "cpumon.hpp"

int main( int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");

	CCpuMon cpumon;
	cpumon.Start();
	return a.exec();
}
