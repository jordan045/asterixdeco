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

    static const int nfilas = 8142;//8142 65535
    static const int ncolumnas = 4504;
    static const int altoPantalla = 4800;
    static const int radioCanvas = altoPantalla / 2;
    constexpr static const double nTRG = 3700.0;
    constexpr static const double pTRG = 1 / nTRG;
    constexpr static const double rangMax = pTRG * 299792458.0 * 0.00053996;

    double Mf[nfilas];

    int M[nfilas][ncolumnas];
    int color(int c);
    void cargar_matriz(int f, int c, int col);
    void pre_carga();


    int puntosX[nfilas][ncolumnas];
    int puntosY[nfilas][ncolumnas];
    int matrizPuntos[altoPantalla][altoPantalla];
    double radioEscalado[ncolumnas];
};

#endif // DECODIFICADORASTERIX_H
