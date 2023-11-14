#ifndef PAQUETE_H
#define PAQUETE_H

#include <QObject>
#include <QByteArray>

class Paquete
{
public:
    Paquete();

    bool inicializado();

    void setSIC(uint8_t s);
    void setSAC(uint8_t s);
    void setMessageType(quint32 m);
    void setMsg_Index(double msg);
    void setSTART_AZ(int s);
    void setEND_AZ(double e);
    void setSTART_RG(int s);
    void setCELL_DUR(double c);
    void setCompression(bool c);
    void setResolution(uint8_t r);
    void setValid_Octets(quint16 o);
    void setValid_Cells(quint32 c);
    void setREP(uint8_t r);
    void setVIDEO_BLOCK(QByteArray s);

    uint8_t getSIC();
    uint8_t getSAC();
    quint32 getMessageType();
    double  getMsg_Index();
    int  getSTART_AZ();
    int  getEND_AZ();
    int  getSTART_RG();
    double  getCELL_DUR();
    bool    getCompression();
    uint8_t getResolution();
    quint16 getValid_Octets();
    quint32 getValid_Cells();
    uint8_t getREP();
    QByteArray getVIDEO_BLOCK();



private:
    bool ini = false;
    uint8_t SIC;
    uint8_t SAC;
    quint32 message_type;
    double msg_index;
    int START_AZ;
    int END_AZ;
    int START_RG;
    double CELL_DUR;
    bool comp;
    uint8_t res;
    quint16 oct;
    quint32 cel;
    uint8_t rep;
    QByteArray block;
};

#endif // PAQUETE_H
