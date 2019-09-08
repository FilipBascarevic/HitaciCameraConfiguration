#include <QCoreApplication>
#include "hitachiapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    hitachiAPI hitaci;

    hitaci.open("COM1");
    quint16 level = 7;
    hitaci.ZoomPositionGet(level);

    return a.exec();
}
