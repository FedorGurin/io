#ifndef LAYER_ANALOG_H
#define LAYER_ANALOG_H
#include "platform.h"
#include <math.h>

#include "isa_defs.h"
#include "hal.h"

#include <fcntl.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
//#define VXWORKS_PLATFROM
//! абстрактный класс для аналоговых команд(для приема и передачи)
//! синглтон
//! в разработке class T - может быть классом содержащим один большой enum


template<typename T> class LayerTemplAnalog //! синглтон
{
public:   
    //! преобразовать аналог в дискретное значение
    static uint16_t getADC(uint16_t numCh)
    {        
        return 0;
    }
    //! преоборазование цифрового кода в аналоговый вид
    static bool setDACW(uint16_t numCh, /*номер канала*/
            float value,               /*напряжение, В */
            float dValue=0,            
            float dt=0)
    {
        uint16_t d = (uint16_t)floor(value + (10.0/20.0 * 8191) + 0.5);
        return true;
    }
    //! преоборазование цифрового кода в аналоговый вид
    static bool setDAC(uint16_t numCh, /*номер канала*/
              float value,               /*напряжение, В */
               float dValue=0,            
               float dt=0)
    {
           uint16_t d = (uint16_t)floor(value + (10.0/20.0 * 8191) + 0.5);
           return true;
    }
    //! имитация сопротивления
    static bool setR(uint16_t numCh, float value)
    {
        return true;
    }
    //! имитация термопар
    static bool setIT(uint16_t numCh, float value)
    {
       return true;
    }
    //! задать выходное питание
    static bool setU(uint16_t numCh, float value)
    {
        return true;
    }
    //! задать выходной постоянный ток
    static bool setA(uint16_t numCh, float value)
    {
        return true;
    }
    //! задать частоту переменного тока
    static bool setAFreq(uint16_t numCh, float freq)
    {
        return true;
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
    //! инстанциирование класса
    static LayerTemplAnalog *obj()
    {
        if(layer == 0)
            layer = new LayerTemplAnalog<T>;

        return layer;
    }
private:
    LayerTemplAnalog()
    {
        //maxNumRK = 0;
        //rks      = 0;
        //rks      = new uint8_t[296];
        
        spec = new T;
    }
    //! указатель на массив РК
    uint8_t *rks;
    //! максимальное кол-во РК в массиве
    uint16_t maxNumRK;
    //!
    static LayerTemplAnalog<T> *layer;
    static T *spec;
};


#ifndef VXWORKS_PLATFORM
//! модуль взаимодействия с адаптерами ЦАП
class LayerAnalogDefault
{
public:
    LayerAnalogDefault()
    {

    }


    //! общая инициализация устройств
    void init()
    {

    }



    //! принудительная запись в устройство
    void writeToDevice()
    {

    }

    //! обновление устройства
    void updateState()
    {

    }

    //! принудительное чтение из устройства
    void readFromDevice(){

    }




};
#endif

#ifdef VXWORKS_PLATFORM
#include <dacwav.h>
#include <dac16.h>
#include <ioLib.h>
#include <unistd.h>
typedef struct _dac_info_t
{
    float v_start;
    float dv;
    unsigned time; /* us */
    unsigned align;
} dac_data_t;

//! модуль взаимодействия с адаптерами ЦАП
class LayerAnalogVx
{
public:
    LayerAnalogVx();
  

    //! общая инициализация устройств
    void init();
    
    //! инициализация типа устройства
    int init(dev_info_t   *dac_info,     /*информация об устройстве*/
             int          &a_devices, /*идентификатор устройства*/
             TAdapterISA *devices,     /*кол-во устройств*/
             uint8_t     **dac_buf,
             char dev_name[]);     /*буфер*/
    
    //int sc_in_read();
    
    //! выдача всех разовых команд которые накопили в буфере
    //int sc_out_write();
   
    //! принудительная запись в устройство
    void writeToDevice();
  
    //! обновление устройства
    void updateState();
  
    //! принудительное чтение из устройства
    void readFromDevice();
    
   
   
    //! освобождение ресурсов
    //void clear(dac_info_t* sc,
    //           int *devices,
    //           uint8_t *buf);
 
    //! дескрипторы
    int dacw_devices;
    int dac16_devices;
    int adc_devices;
    //int out_devices;
    //! информация об устройстве
    TDevInfo* dacw_out;
    TDevInfo* dac16_out;
    //! данные для отправки
    dacwav_write_cmd cmdDacWav;
    dac16_write_cmd cmdDac16;   
};
#endif


#if defined(QNX_PLATFORM)
#include "qnxLayerAnalog.h"
typedef LayerTemplAnalog<QnxLayerAnalog> LayerAnalog;
typedef QnxLayerAnalog TypeSpecAnalog;
#endif

#if !defined(VXWORKS_PLATFORM) && !defined(QNX_PLATFORM)
#include "./qt/qtUdpSocket.h"
typedef LayerTemplRK<LayerRKDefault> LayerRK;
typedef LayerRKDefault TypeSpecRK;
#endif

#if defined(VXWORKS_PLATFORM)
#include "vxUdpSocket.h"
typedef LayerTemplRK<LayerRKVx > LayerRK;
typedef LayerRKVx TypeSpecRK;
#endif
#endif // LAYERANALOG_H

