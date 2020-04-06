#ifndef LAYERRK_H
#define LAYERRK_H
#include "platform.h"
#ifdef VXWORKS_PLATFORM

#include "isa_defs.h"

#include <trrel48.h>
#include <rsrel48.h>

#include <ioLib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "hal.h"

#endif

//#define VXWORKS_PLATFORM
//! абстрактный класс для разовых команд(для приема и передачи)
//! синглтон
template<typename T> class LayerTemplRK //! синглтон
{
public:   
    //! задать значение РК
    static bool set(uint16_t  numCh, /*номер канала*/
                    uint8_t  value) /*значение*/
    {
        //obj();
//        if(spec->rksOut == 0)
//            return false;

//        TCh* ch = HAL::obj()->findCh(numCh);
//
//        //! сохранить данные в канале
//        TDesRK * rk = (TDesRK*)ch->desData;
//        rk->value = value;

        if(spec == 0)
            return false;
        if(numCh>=spec->maxRksOut)
            return false;
        spec->rksOut[numCh] = value;
        

        return false;
    }
    //! прочитать РК из памяти
    static bool get(uint16_t numCh,
                    uint8_t &value)
    {
//        TCh* ch = HAL::obj()->findCh(numCh);
//
//        //! сохранить данные в канале
//        TDesRK * rk = (TDesRK*)ch->desData;
//        rk->value = value;
                
        if(spec == 0)
            return false;
        if(numCh>=spec->maxRksIn)
                   return false;
        value = spec->rksIn[numCh];
       

        return false;
    }

    uint16_t maxRksIn()
    {
        return spec->maxRksIn;
    }
    uint16_t maxRksOut()
    {
        return spec->maxRksout;
    }
    //! принудительная запись в устройство
    void writeToDevice()
    {
        spec->writeToDevice();
    }
    //! принудительное чтение из устройства
    void readFromDevice()
    {
        spec->readFromDevice();
    }
    //! обновление устройства
    void updateState()
    {
        obj();
        spec->updateState();
    }
    //! инстанциирование класса
    static LayerTemplRK *obj()
    {
        if(layer == 0)
            layer = new LayerTemplRK<T>;

        return layer;
    }
    uint8_t* rk(){return spec->rksIn;}
    uint16_t maxRK(){return spec.maxNumRK;}

private:

    LayerTemplRK<T>()
    {
        spec = new T;
    }

    static LayerTemplRK<T> *layer;
    static T *spec;
};
#define RK_LEN_DEFAULT 512
//! класс для работы с РК(по умолчанию)
class LayerRKDefault
{
public:
    LayerRKDefault()
    {
        maxRksIn = RK_LEN_DEFAULT;
        maxRksOut= RK_LEN_DEFAULT;
        rksIn      = 0;
        rksOut      = 0;
        rksIn      = new uint8_t[RK_LEN_DEFAULT];//
        rksOut      = new uint8_t[RK_LEN_DEFAULT];//
    }

    //! обновление устройства
    void updateState()
    {
        //scin_work();
        //scout_work();
    }
    //! принудительная запись в устройство
    void writeToDevice()
    {

    }
    //! принудительное чтение из устройства
    void readFromDevice()
    {

    }
    //! указатель на массив РК
    uint8_t *rksIn;
    //! указатель на массив РК
    uint8_t *rksOut;
    //! максимальное кол-во РК в массиве
    uint16_t maxRksIn;
    uint16_t maxRksOut;
};


//#define VXWORKS_PLATFORM
#ifdef VXWORKS_PLATFORM

//#define REL48_CHANNELS_PER_BOARD 48
//#include <rsrel48.h>
//#include <ioLib.h>

//typedef struct adapter_info_t {
//    char *devname;
//    char *venname;
//    unsigned device_count;
//}TAdapterInfo;
//
typedef struct sc_info_t {
    TAdapterInfo common;
    unsigned channel_count;
}TScInfo;

//! модуль взаимодействия с адаптерами РК
class LayerRKVx
{
public:
    LayerRKVx();
  

    //! общая инициализация устройств
    void init();
    
    //! инициализация типа устройства
    int init(sc_info_t   *sc_info,     /*информация об устройстве*/
             int          &sc_devices, /*идентификатор устройства*/
             TAdapterISA *devices,     /*кол-во устройств*/
             uint8_t     **sc_buf,
             char dev_name[]);     /*буфер*/
    
    int sc_in_read();
    
    //! выдача всех разовых команд которые накопили в буфере
    int sc_out_write();
   
    //! принудительная запись в устройство
    void writeToDevice();
  
    //! обновление устройства
    void updateState();
  
    //! принудительное чтение из устройства
    void readFromDevice();
   
    //! освобождение ресурсов
    void clear(sc_info_t* sc,
               int *devices,
               uint8_t *buf);

    //! указатель на массив РК
    uint8_t *rksIn;
    uint8_t *rksOut;
    //! дескрипторы
    int in_devices;
    int out_devices;
    //! информация об устройстве
    TScInfo* sc_in;
    TScInfo* sc_out;
    //! данные для отправки
    struct trrel48WriteCmd cmdTr;
    struct rsrel48ReadCmd  cmdRs;

    //! максимальное кол-во РК в массиве
    uint16_t maxRksIn;
    uint16_t maxRksOut;
    
    uint16_t bitsRead;
    uint16_t reg[3];
    uint8_t bit;
    uint16_t word;
    uint8_t *rks;
    //uint16_t maxLocalRksIn;
    //uint16_t maxLocalRksOut;
};
#endif

#ifdef VXWORKS_PLATFORM
#ifdef VXWORKS_SIM
typedef LayerTemplRK<LayerRKVx > LayerRK;
typedef LayerRKVx TypeSpecRK;
#else
typedef LayerTemplRK<LayerRKVx > LayerRK;
typedef LayerRKVx TypeSpecRK;
#endif
#else
typedef LayerTemplRK<LayerRKDefault> LayerRK;
typedef LayerRKDefault TypeSpecRK;
#endif

#endif // LAYERRK_H

