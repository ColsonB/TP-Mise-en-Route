#include <QtCore/QCoreApplication>
#include "emission.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	Emission emission;
	
    return a.exec();
}
