#include "decodificadorasterix.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QtEndian>



QJsonObject json;


DecodificadorASTERIX::DecodificadorASTERIX()
{
    sock = new QUdpSocket(this);
    sock->bind(QHostAddress::AnyIPv4, 2020);
    pre_carga();
}

void DecodificadorASTERIX::RecibirTrama(Paquete p)
{
    int start_azimut = p.getSTART_AZ();
    int start_range = p.getSTART_RG();
    int valid_cells = p.getValid_Cells();

    //QByteArray video_block = p.getVIDEO_BLOCK();
    uint16_t* video_block = (uint16_t*) p.getVIDEO_BLOCK().data();

    //int bi = round((start_azimut * 8142)/65535);
    //qDebug() << start_azimut;

    //int disAct = start_range + valid_cells;

    //verificar cantidad de muestras superan start_range + valid_cells a 4504
    //verificar prf
    /*
    if(disAct >= maxDis){
        maxDis = disAct;
        qDebug() << "Maxima distancia: " << maxDis;
    }

*/
    qDebug() << "Start Range: " << start_range;
    qDebug() << "Start Azimut: " << start_azimut;
    qDebug() << "--------------------";

    int muestra;
    for (int i = 0; i < valid_cells; i++){
        //char c = video_block.at(i);
        //char c1 = video_block.at(i + 1);
        muestra =  qFromBigEndian(video_block[i]) & 0x3fff;
        //qDebug() << "Muestra: " << muestra;
        muestra = color(muestra);
        //int muestra = color(c1, c);
        //qDebug() << "Muestra: " << muestra;

        cargar_matriz(start_azimut, start_range + i, muestra);
    }
}

int DecodificadorASTERIX::color(int c)
{
    /*
   QString hexC = QString(c);
   QString hexC1 = QString(c1);
   QString con = hexC + hexC1;
   bool ok;
   */
   int color; //= con.toInt(&ok,16);
   //qDebug() << "color: " << color;

/*
   if(color <= 42){
       color = 0;
   }
   if(color <= 84 && color > 42){
       color = 1;
   }
   if(color <= 126 && color > 84){
       color = 2;
   }
   if(color <= 168 && color > 126){
       color = 3;
   }
   if(color <= 210 && color > 168){
       //qDebug() << "color: " << color;
       color = 4;
   }
   if(color > 210){
       color = 5;
   }
*/
/*
   if(color > 225){
       //qDebug() << "color: " << color;
       color = 5;
   }else{
       color = 0;
   }
*/
   color = floor(c / 2731);

   return color;
}

void DecodificadorASTERIX::cargar_matriz(int angulo, int rango, int muestra)
{
    //qDebug() << "Color: " << muestra;
    if(muestra != M[angulo][rango]){//manda unicamente los cambios
        int coordX= puntosX[angulo][rango];
        int coordY= puntosY[angulo][rango];

        M[angulo][rango] = muestra;

        if (coordX<altoPantalla && coordY<altoPantalla){
            if (muestra != matrizPuntos[coordX][coordY]){ 
                matrizPuntos[coordX][coordY] = muestra;
                json["coordenadaX"] = coordX; //debe ir lo de la precarga
                json["coordenadaY"] = coordY; //debe ir lo de la precarga
                json["color"] = muestra;
                sock->writeDatagram(QJsonDocument(json).toJson(), QHostAddress::LocalHost, 2020);
                //qDebug() << "X: " << coordX << "Y: " << coordY << "Muestra: " <<muestra;
            }
        }
    }
}

/*
void DecodificadorASTERIX::cargar_matriz(int f, int c, int m){
    qDebug() << "Fila: " << f << "Columna: " << c << "Muestra: " <<m;
}
*/

void DecodificadorASTERIX::pre_carga()
{
    qDebug() << "---PRECARGA INICIADA---";
    /*
    for(int i = 0; i < nfilas; i++){
        for(int j = 0; j < ncolumnas; j++){
            M[i][j] = 0;
        }
    }

    for(int i = 0; i<4800; i++){
        for(int j = 0; j < 4800; j++){
            matrizPuntos[i][j] = 0;
        }
    }*/

    for(int f = 0; f < nfilas; f++){
        Mf[f] = (90 - (f * 360.0) / nfilas) * (3.1416 / 180);
        //qDebug()<< "Mf: " << Mf[f];
    }

    // qDebug()<< "Valor " << Mf[65530];
    qDebug()<< "Radio:  " << rangMax;
    for(int c = 0; c < ncolumnas; c++){
        double radio = ((c * rangMax)/ ncolumnas) ;//rangMax
        //qDebug()<< "Radio:  " << radio;
        radioEscalado[c] = (radio * ( radioCanvas - 3) ) / 256;
        //qDebug()<< "Radio Escalado " << radioEscalado[c];
    }

    for(int f = 0; f < nfilas; f++){
        for(int c = 0; c < ncolumnas; c++){
            puntosX[f][c] = round(radioCanvas + (std::cos(Mf[f]) * radioEscalado[c] ));
            puntosY[f][c] = round(radioCanvas - (std::sin(Mf[f]) * radioEscalado[c] ));
            //printf("%i ",puntosX[f][c]);
            //qDebug()<< "PuntosX: " << puntosX[f][c];
        }
    }

    qDebug() << "---PRECARGA TERMINADA---";


    //código para generar círculos de prueba
   /*for(int f = 0; f < nfilas; f++){
            float radio = ((500 * rangMax) / ncolumnas) ;
            float radioEscalado = (radio * ( radioCanvas - 3) ) / 256;

            int puntoX = round(radioCanvas + (std::cos(Mf[f]) * radioEscalado ));
            int puntoY = round(radioCanvas - (std::sin(Mf[f]) * radioEscalado ));

            //std::string jsonStr = "{\"coordenadaX\": " + std::to_string(puntoX) + ", \"coordenadaY\": " + std::to_string(puntoY) + ", \"color\": " + std::to_string(4) +"}";

            json["coordenadaX"] = puntosX[f][1000]; //debe ir lo de la precarga
            json["coordenadaY"] = puntosY[f][1000]; //debe ir lo de la precarga
            json["color"] = 2;
            sock->writeDatagram(QJsonDocument(json).toJson(), QHostAddress::LocalHost, 2020);
            //send(s, jsonStr.c_str(), jsonStr.size(), 0);

    }*/





}
