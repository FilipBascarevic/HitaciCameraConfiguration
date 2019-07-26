#include <QCoreApplication>
#include "hitachiapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    hitachiAPI hitaci;

    hitaci.open("COM1");
    QString mode = "AVERAGE";
    hitaci.LightControlMode(WRITE, mode);

    return a.exec();
}
