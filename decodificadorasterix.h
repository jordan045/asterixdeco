#ifndef DECODIFICADORASTERIX_H
#define DECODIFICADORASTERIX_H

#include <QObject>
#include <QUdpSocket>
#include "paquete.h"

class DecodificadorASTERIX : QObject
{
public:
    DecodificadorASTERIX();
    void RecibirTrama(Paquete p);

private:

    QUdpSocket *sock;

    static const int NFILAS = 8192;//8192 65535
    static const int CANT_COLORES = 6;
    static const int MUESTRA_MAX = 65535; //2^16 muestras - 65535
    static const int NCOLUMNAS = 4504;
    static const int ALTO_PANTALLA = 16000;
    static const int RADIO_CANVAS = ALTO_PANTALLA / 2;
    constexpr static const double nTRG = 3700.0;
    constexpr static const double pTRG = 1 / nTRG;
    constexpr static const double RANG_MAX = pTRG * 299792458.0 * 0.00053996;

    QString toSend = "";

    double Mf[NFILAS];

    int M[NFILAS][NCOLUMNAS];
    int color(int c);
    void cargar_matriz(int f, int c, int col);
    void pre_carga();


    int puntosX[NFILAS][NCOLUMNAS];
    int puntosY[NFILAS][NCOLUMNAS];
    int matrizPuntos[ALTO_PANTALLA][ALTO_PANTALLA];
    double radioEscalado[NCOLUMNAS];
};

#endif // DECODIFICADORASTERIX_H
