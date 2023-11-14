#include <QCoreApplication>
#include "decodificadorUDP.h"

DecodificadorUDP *d;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    d = new DecodificadorUDP();
    //iniciar();
    return a.exec();
}
