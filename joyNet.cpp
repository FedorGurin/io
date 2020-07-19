#include "joyNet.h"

JoyNet* JoyNet::joyObj = 0;
JoyNet::JoyNet()
{   
    initOk      = false;
    isRecived   = 0;
    count       = 0;
    maxCount    = 10;
    memset((void*)&params,0, sizeof(params));
}
void JoyNet::init(uint16_t port, std::string ip = "")
{
    if(initOk == true)
        return;

    udpSocketRec = new UdpSocket;
    udpSocketRec->init(port, ip);
    udpSocketRec->bindTo();
    udpSocketRec->setBlock(false);
    initOk = true;
}
void JoyNet::reciveAnswer()
{
    int bytes = 0;
    int iter  = 0;
    if(count > maxCount)
        isRecived = 0;

    do{    
        bytes = udpSocketRec->reciveFrom((uint8_t*)&params, sizeof(params));
        iter++;
    }while(bytes != -1);

    if(iter>0)
        isRecived = 1;

    if(iter == 0)
        count++;
}
//! получение параметров от джойстика
params_t JoyNet::getParams()
{
    reciveAnswer();
    return params;
}


