#ifndef JOY_NET_H
#define JOY_NET_H

#include "globalData.h"
#include "templateUdpSocket.h"
#include "joy_types.h"
#include <iostream>
//! специализированный класс для работы с джойстиком
class  JoyNet
{
public:
    JoyNet();

    //! инициалзизация
    void init(uint16_t port,std::string ip);

    //! получение параметров от джойстика
    params_t getParams();

    //! получение данных
    void reciveAnswer(void);

    static JoyNet *obj()
    {
        if(joyObj == 0)
            joyObj = new JoyNet();
        return joyObj;
    }
    uint8_t isConnect(){return isRecived;}
private:
    UdpSocket * udpSocketRec;
    params_t              params;
    bool initOk;
    //! признак приема данных от джойстика
    uint8_t isRecived;
    //! кол-во попыток чтения данных от сокета
    uint8_t count;
    uint8_t maxCount;
    static JoyNet *joyObj;
};

#endif
