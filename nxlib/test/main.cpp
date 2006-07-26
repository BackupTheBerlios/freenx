#include <QCoreApplication>

#include "nxlib.h"

int main(int argc, char **argv)
{
	QCoreApplication *qApp = new QCoreApplication(argc, argv);
	NXLib lib(qApp);
	lib.invokeNXSSH("default" ,"lithium", true);
	return qApp->exec();
}
