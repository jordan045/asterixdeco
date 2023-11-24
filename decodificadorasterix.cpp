#include "decodificadorasterix.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QtEndian>


QJsonObject json;

// El constructor crea el socket y conecta al puerto 2020
// Luego manda a la precarga
DecodificadorASTERIX::DecodificadorASTERIX()
{
    sock = new QUdpSocket(this);
    sock->bind(QHostAddress::AnyIPv4, 2020);
    pre_carga();
}

// Recibe los paquetes p conteniendo información de video que se procesa
// y se manda a cargar a la matriz
void DecodificadorASTERIX::RecibirTrama(Paquete p)
{

    int start_azimut = p.getSTART_AZ();     // Azimut: Angulo obtenido
    int start_range = p.getSTART_RG();      // Range: Distancia del polo
    int valid_cells = p.getValid_Cells();   // ValidCells: Cantidad de celdas con info relevante

    start_azimut = start_azimut >> 3;   //Corrimiento, debe ser a la DERECHA

    uint16_t* video_block = 0;
    video_block = (uint16_t*) p.getVIDEO_BLOCK();
    int muestra = 0;
    toSend.append("{");
    for (int i = 0; i < valid_cells; i++){
        muestra =  qFromBigEndian(video_block[i]);
        muestra = color(muestra);
        cargar_matriz(start_azimut, start_range + i, muestra);
    }
    toSend.append("}");

    QByteArray arr = QByteArray::fromStdString(toSend.toStdString());
    sock->writeDatagram(arr, QHostAddress::LocalHost, 2020);
    toSend = "";
}

// Mapea la muestra al color
int DecodificadorASTERIX::color(int c)
{
   int color = floor(c / (MUESTRA_MAX / CANT_COLORES));
   return color;
}

void DecodificadorASTERIX::cargar_matriz(int angulo, int rango, int muestra)
{
    if(muestra != M[angulo][rango]){
        int coordX= puntosX[angulo][rango];
        int coordY= puntosY[angulo][rango];

        M[angulo][rango] = muestra;

        if (coordX<ALTO_PANTALLA && coordY<ALTO_PANTALLA){
            if (muestra != matrizPuntos[coordX][coordY]){

                matrizPuntos[coordX][coordY] = muestra;
                toSend.append(",{\"coordenadaX:\"");
                toSend.append(QString::number(coordX));
                toSend.append("\",coordenadaY:\"");
                toSend.append(QString::number(coordY));
                toSend.append(",\"color:\"");
                toSend.append(QString::number(muestra));
                toSend.append("}");

            }
        }
    }
}

void DecodificadorASTERIX::pre_carga()
{
    qDebug() << "---PRECARGA INICIADA---";
    for(int f = 0; f < NFILAS; f++){
        Mf[f] = (90 - (f * 360.0) / NFILAS) * (3.1416 / 180);
    }

    for(int c = 0; c < NCOLUMNAS; c++){
        double radio = ((c * RANG_MAX)/ NCOLUMNAS) ;//rangMax
        radioEscalado[c] = (radio * ( RADIO_CANVAS - 3) ) / 256;
    }

    for(int f = 0; f < NFILAS; f++){
        for(int c = 0; c < NCOLUMNAS; c++){
            puntosX[f][c] = round(RADIO_CANVAS + (std::cos(Mf[f]) * radioEscalado[c] ));
            puntosY[f][c] = round(RADIO_CANVAS - (std::sin(Mf[f]) * radioEscalado[c] ));
        }
    }

    qDebug() << "---PRECARGA TERMINADA---";

    //GENERA CIRCULOS DE PRUEBA
   /*for(int f = 0; f < NFILAS; f++){
            float radio = ((500 * RANG_MAX) / NCOLUMNAS) ;
            float radioEscalado = (radio * ( RADIO_CANVAS - 3) ) / 256;

            int puntoX = round(RADIO_CANVAS + (std::cos(Mf[f]) * radioEscalado ));
            int puntoY = round(RADIO_CANVAS - (std::sin(Mf[f]) * radioEscalado ));

            //std::string jsonStr = "{\"coordenadaX\": " + std::to_string(puntoX) + ", \"coordenadaY\": " + std::to_string(puntoY) + ", \"color\": " + std::to_string(4) +"}";

            json["coordenadaX"] = puntosX[f][1000]; //debe ir lo de la precarga
            json["coordenadaY"] = puntosY[f][1000]; //debe ir lo de la precarga
            json["color"] = 2;
            sock->writeDatagram(QJsonDocument(json).toJson(), QHostAddress::LocalHost, 2020);
            //send(s, jsonStr.c_str(), jsonStr.size(), 0);

    }*/
}
