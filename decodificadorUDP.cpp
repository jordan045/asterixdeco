#include "decodificadorUDP.h"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
//#include <endian.h>
#include <QtEndian>

const uint8_t I240_010=1<<7;    //Data Source Identifier
const uint8_t I240_000=1<<6;    //Message Type
const uint8_t I240_020=1<<5;    //Video recorder Header
//const uint8_t I240_030=1<<4;    //Video Summary
const uint8_t I240_040=1<<3;    //Video Header nano
const uint8_t I240_041=1<<2;    //Video header femto
const uint8_t I240_048=1<<1;    //Video Cells Resolution & Data Compression Indicator
//const uint8_t FSPEC_EXTEND=1;   //
const uint8_t I240_049=1<<7;    //Video Octets & Video Cells Counters
const uint8_t I240_050=1<<6;    //Video Block Low Data Volume
const uint8_t I240_051=1<<5;    //Video Block Medium Data Volume
const uint8_t I240_052=1<<4;    //Video Block High Data Volume
const uint8_t I240_140=1<<3;    //Time of Day (aun no lo usamos)

typedef struct __attribute__((packed)){
    uint8_t cat;
    uint16_t length;
    uint8_t fspec0;
    uint8_t fspec1;
    //char *data_fields;
} asterix_data_block;

//QJsonObject json;

DecodificadorUDP::DecodificadorUDP()
{

    settings = new QSettings("settings.ini", QSettings::IniFormat, this);
    qint16 port = settings->value("main/asterix port", 10001).toInt();
    //QString filename = settings->value("main/filename", "salida.bin").toString();

    socket_asterix = new QUdpSocket(this);
    socket_asterix->bind(QHostAddress::AnyIPv4, port);
    connect(socket_asterix, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket_json = new QUdpSocket(this);
    decoASTX = new DecodificadorASTERIX();
    ///file = new QFile();
    //file->setFileName(filename);
    //file->open(QIODevice::WriteOnly);

}

DecodificadorUDP::~DecodificadorUDP(){
    file->close();
}

void DecodificadorUDP::readyRead()
{
    Paquete *p = new Paquete();
    QByteArray Buffer;
    QHostAddress sender;
    quint16 senderPort;
    asterix_data_block *data_block;
    uint8_t *data_fields;
    int offset;

    while(socket_asterix->hasPendingDatagrams()){
        //qDebug() <<"recibo algo ";
        Buffer.resize(socket_asterix->pendingDatagramSize());
        socket_asterix->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
        data_block = (asterix_data_block *)Buffer.data();
        data_fields = (uint8_t *)Buffer.data()+5; // CAT:1 byte LEN:2 bytes FSPEC:2 bytes
        offset = 0;

        /*
        qDebug() << "|| cat:" << data_block->cat
                 << "|| length:" << qFromBigEndian(data_block->length)
                 << "|| buffer size: " << Buffer.size();

        */
        if(Buffer.size() != qFromBigEndian(data_block->length)){
            qDebug() << "incorrect header length, skipping packet";
            continue;
        }
        if(data_block->fspec0 & I240_010){
            // data source identifier
            p->setSIC(data_fields[offset]);
            //json["SIC"] = data_fields[offset];

            p->setSAC(data_fields[offset+1]);
            //json["SAC"] = data_fields[offset+1];

            offset+=2;
        }
        if(data_block->fspec0 & I240_000){
            // message type
            // data_fields[offset] = 1 video summary message
            //                     = 2 video message
            p->setMessageType(data_fields[offset]);

            //json["message_type"] = data_fields[offset];

            offset+=1;
        }
        if(p->getMessageType() == 2){ // video message
            if(data_block->fspec0 & I240_020){
                // video record header;
                quint32 msg_index = *(quint32 *)(data_fields+offset);
                msg_index = qFromBigEndian(msg_index);
                p->setMsg_Index(double(msg_index));
                //json["msg_index"] = double(msg_index);
                offset+=4;
            }
            // puede seguir el nano o el femto
            if(data_block->fspec0 & I240_040){
                // video header nano
                quint16 start_az = *(quint16 *)(data_fields+offset);
                start_az = qFromBigEndian(start_az);
                quint16 end_az = *(quint16 *)(data_fields+offset+2);
                end_az = qFromBigEndian(end_az);
                quint32 start_rg = *(quint32 *)(data_fields+offset+4);
                start_rg = qFromBigEndian(start_rg);
                quint32 cell_dur = *(quint32 *)(data_fields+offset+8);
                cell_dur = qFromBigEndian(cell_dur);

                p->setSTART_AZ(double(start_az));
                //json["start_az"] = double(start_az);

                p->setEND_AZ(double(end_az));
                //json["end_az"]   = double(end_az);

                p->setSTART_RG(double(start_rg));
                //json["start_rg"] = double(start_rg);

                p->setCELL_DUR(double(cell_dur));
                //json["cell_dur"] = double(cell_dur);
                offset += 12;
            }
            else if(data_block->fspec0 & I240_041){
                // video header femto
                quint16 start_az = *(quint16 *)(data_fields+offset);
                start_az = qFromBigEndian(start_az);
                //start_az= start_az >> 3;
                quint16 end_az = *(quint16 *)(data_fields+offset+2);
                end_az = qFromBigEndian(end_az);
                quint32 start_rg = *(quint32 *)(data_fields+offset+4);
                start_rg = qFromBigEndian(start_rg);
                quint32 cell_dur = *(quint32 *)(data_fields+offset+8);
                cell_dur = qFromBigEndian(cell_dur);

                //qDebug() << start_rg;
                p->setSTART_AZ(double(start_az));
                //json["start_az"] = double(start_az);

                p->setEND_AZ(double(end_az));
                //json["end_az"]   = double(end_az);

                p->setSTART_RG(double(start_rg));
                //json["start_rg"] = double(start_rg);

                p->setCELL_DUR(double(cell_dur));
                //json["cell_dur"] = double(cell_dur);
                offset += 12;
            }
            if(data_block->fspec0 & I240_048){
                // cell resolution and compression
                bool compression = *(quint8 *)(data_fields+offset) & 1<<7;
                quint8 resolution = *(quint8 *)(data_fields+offset+1);

                p->setCompression(compression);
                //json["compression"] = compression;

                p->setResolution(resolution);
                //json["resolution"] = resolution;
                offset += 2;
            }
            if(data_block->fspec1 & I240_049){
                // octets and cells counters
                quint16 valid_octets = *(quint16 *)(data_fields+offset);
                valid_octets = qFromBigEndian(valid_octets);
                quint32 valid_cells = *(quint32 *)(data_fields+offset+2) & 0xffffff;
                valid_cells = qFromBigEndian(valid_cells<<8);

                p->setValid_Octets(valid_octets);
                //json["valid_octect"] = valid_octets;

                p->setValid_Cells(valid_cells);
                //json["valid_cells"] = double(valid_cells);
                offset += 5;
            }

            if(data_block->fspec1 & I240_050){
                // video block low data volume
            }
            else if(data_block->fspec1 & I240_051){
                // video block medium data volume
                quint8 rep = *(quint8 *)(data_fields+offset);
                int size = rep * 64; // 64 bytes en 512 bits (tamaño en bits del bloque medium data volume)
                QByteArray video_block = QByteArray::fromRawData((char *)data_fields+offset+1, size);

                p->setREP(rep);
                //json["rep"] = rep;

                //p->setVIDEO_BLOCK(video_block);

                p->setVIDEO_BLOCK(video_block.toHex('\x0'));

                //json["video_block"] = QString(video_block.toBase64());
            }
            else if(data_block->fspec1 & I240_052){
                // video block high dat volume"
            }
            if(data_block->fspec1 & I240_140){
                // time of day
            }


            decoASTX->RecibirTrama(*p);

            //q->enqueue(*p);
            //p->getVIDEO_BLOCK().size();

            /*
            qDebug()<<"SIC: " << p->getSIC() << "\n" <<
                      "SAC: " << p->getSAC() << "\n" <<
                      "START_AZ: " << p->getSTART_AZ() << "\n" <<
                      "ENDA_AZ: " << p->getEND_AZ() << "\n" <<
                      "START_RG: " << p->getSTART_RG() << "\n" <<
                      "CELL_DUR: " << p->getCELL_DUR() << "\n" <<
                      "COMPRESSION: " << p->getCompression() << "\n" <<
                      "RESOLUTION: " << p->getResolution() << "\n" <<
                      "VALID_OCTETS: " << p->getValid_Octets() << "\n" <<
                      "VALID_CELLS: " << p->getValid_Cells() << "\n" <<
                      "REP: " << p->getREP() << "\n";
                      //"VIDEO_BLOCK: " << p->getVIDEO_BLOCK() << "\n";
            */
            //socket_json->writeDatagram(QJsonDocument(json).toJson(), QHostAddress::LocalHost, 12345);

        }
        else{
            // video summary
            //este campo solo va cuando estamos en un video summary message
            //if(data_block->fspec0 & I240_030){
            //    qDebug() << "video summary";
            //}
        }
    }
}
