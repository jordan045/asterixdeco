#include "paquete.h"

Paquete::Paquete()
{
    ini = true;
}

bool Paquete::inicializado()
{
    return ini;
}

//SETTERS
void Paquete::setSIC(uint8_t s){ SIC = s;}
void Paquete::setSAC(uint8_t s){ SAC = s;}
void Paquete::setMessageType(quint32 m){ message_type = m;}
void Paquete::setMsg_Index(double msg){ msg_index = msg;}
void Paquete::setSTART_AZ(int s){ START_AZ = s;}
void Paquete::setEND_AZ(double e){ END_AZ = e;}
void Paquete::setSTART_RG(int s){ START_RG = s;}
void Paquete::setCELL_DUR(double c){ CELL_DUR = c;}
void Paquete::setCompression(bool c){ comp = c;}
void Paquete::setResolution(uint8_t r){ res = r;}
void Paquete::setValid_Octets(quint16 o){ oct = o;}
void Paquete::setValid_Cells(quint32 c){ cel = c;}
void Paquete::setREP(uint8_t r){ rep = r;}
void Paquete::setVIDEO_BLOCK(uint8_t *s){ block = s; }

//GETTERS
uint8_t Paquete::getSIC(){ return SIC; }
uint8_t Paquete::getSAC(){ return SAC; }
quint32 Paquete::getMessageType(){ return message_type; }
double Paquete::getMsg_Index(){ return msg_index; }
int Paquete::getSTART_AZ(){ return START_AZ; }
int Paquete::getEND_AZ(){ return END_AZ; }
int Paquete::getSTART_RG(){ return START_RG; }
double Paquete::getCELL_DUR(){ return CELL_DUR; }
bool Paquete::getCompression(){ return comp; }
uint8_t Paquete::getResolution(){ return res; }
quint16 Paquete::getValid_Octets(){ return oct; }
quint32 Paquete::getValid_Cells(){ return cel; }
uint8_t Paquete::getREP(){ return rep; }
uint8_t* Paquete::getVIDEO_BLOCK(){ return block; }
