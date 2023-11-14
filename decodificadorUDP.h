#ifndef DECODIFICADORUDP_H
#define DECODIFICADORUDP_H

#include <QUdpSocket>
#include "decodificadorasterix.h"

class QSettings;
class QFile;
class paqueteASTERIX;


class DecodificadorUDP : QObject
{
    Q_OBJECT

public:
    DecodificadorUDP();
    ~DecodificadorUDP();

private:
    QUdpSocket *socket_asterix;
    QUdpSocket *socket_json;
    QSettings *settings;
    QFile *file;
    DecodificadorASTERIX *decoASTX;

private slots:
    void readyRead();
};

#endif // DECODIFICADORUDP_H
