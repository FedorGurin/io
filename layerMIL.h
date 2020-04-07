#ifndef LAYERMIL_H
#define LAYERMIL_H

#include <vector>
#include "platform.h"
#include "hal.h"
// Класс абстракции для обмена данными по МКИО
namespace LayerMIL
{
//! описание линий
enum MIL_LINE
{
    CHANNEL_MIL_1 = 0,
    CHANNEL_MIL_2 = 1,
    CHANNEL_MIL_3 = 2,
    CHANNEL_MIL_4 = 3
};
enum MIL_LINE_NAME
{
    CH_MIL_OSO = CHANNEL_MIL_2,
    CH_MIL_NVG = CHANNEL_MIL_1,
    CH_MIL_KSS = CHANNEL_MIL_3
};


enum MIL_LINE_NAME_50
{
    CH_MIL_IUS_58 = CHANNEL_MIL_2,
    CH_MIL_NVG_58 = CHANNEL_MIL_1,
    CH_MIL_BP_58  = CHANNEL_MIL_3,
    CH_MIL_RSUO_58= CHANNEL_MIL_4,
};



//! номер адаптера
enum MIL_ADAPTER
{
    ADAPTER_MIL_1 = 0,
    ADAPTER_MIL_2 = 1,
    ADAPTER_MIL_3 = 2,
    ADAPTER_MIL_4 = 3,
    ADAPTER_MIL_5 = 4,
    ADAPTER_MIL_6 = 5
};
//! операции с адптером при записи и отправки информации
enum TSetFlagOU
{
    F_WRITE          = 1, /*запись(упаковка) данных в буфера адаптера*/
    F_SEND           = 2, /*только отправка в адаптера*/
    F_WRITE_AND_SEND = 3  /*F_SEND|F_WRITE*/
};
//! операции с адптером при чтении и приему информации*/
enum TGetFlagOU
{
    F_RECIVE          = 1,/*только прием*/
    F_READ            = 2,/*чтение(распаковка) данных из буфера адаптера*/
    F_RECIVE_AND_READ = 3 /*прием информации и последующее чтение данных из обновленного буфера F_RECIVE|F_READ*/
};
enum TFaultsOU
{
    NO_FAULTS       = 0,/*ошибок нет*/
    OU_FAULT        = 1,/*неисправность ОУ*/
    AB_FAULT        = 4,/*неисправность абонента*/
    BUSY_AB_FAULT   = 8,/*абонент занят*/
    ERROR_MSG_FAULT = 11/*ошибка в сообщении*/
};
enum TTransDirect{
    CONTROLLER_OU = 0, /*информация от Контроллера->в ОУ*/
    OU_CONTROLLER = 1, /*информация от ОУ->в Контроллер*/
    MONITOR       = 1  /*режим монитора*/
};
//! направление передачи данных
enum TTransDirectName{
    KOU = CONTROLLER_OU, // режим К->ОУ
    OUK = OU_CONTROLLER, // режим ОУ->К
    MON = MONITOR        // режим Монитора
};
enum TValueType
{
    F_DOUBLE    ,
    F_FLOAT     ,
    F_INT       ,
    F_TWOWORDS  ,
    F_CODE      ,
    F_BIT
};
enum TSIGN
{
    F_USIGN,
    F_SIGN
};

//! один параметр
typedef struct TParam_
{
    TParam_(void *addr_,
           int numWord_,
           TValueType toType_,
           TValueType fromType_,
           int lBit_,
           int hBit_,
           double scale_,
           TSIGN sign_)
    {
        addr     = addr_;
        numWord  = numWord_;
        toType   = toType_;
        fromType = fromType_;
        lBit     = lBit_;
        hBit     = hBit_;
        scale    = scale_;
        sign     = sign_;
    }

    void *addr;
    int numWord;
    TValueType toType;
    TValueType fromType;
    int lBit;
    int hBit;
    double scale;
    TSIGN sign;
}TParam;

template <class T> class MIL
{
public:
    MIL()
    {
       
    }
    static bool initApi()
    {
        if(initOk == false)
            initOk = T::initApi();        
        return initOk;
    }
    static uint8_t maxNumCh()
    {
        numCh = T::maxNumCh();
        return numCh;
    }
    static bool isInit(){return initOk;}
private:
    static bool initOk;
    static uint8_t numCh;
};
//bool MIL::initOk = false;
//uint8_t MIL::numCh = 0;
//! класс описания параметров
template <class T> class ParamMIL
{
public:
    ParamMIL()
    {
        params.clear();
        data = new T;
    }
    ~ParamMIL()
    {

    }
    //! прочитать код параметра
    uint16_t codeParam16(uint8_t index) //! index - номер параметра, начинается с 0
    {
        /*if(index<0) index=0;
        else if(index>31) index=31;*/

        if(index>31) index=31;
        return data->codeParam16(index);
    }
    void setCodeParam16(uint8_t index,uint16_t value) //! index - номер параметра, начинается с 0
    {
        if(index<0) index=0;
        else if(index>31) index=31;

        data->setCodeParam16(index,value);
    }
    //! прочитать значение параметра
    uint32_t codeParam32(uint8_t index) //! index - номер параметра, начинается с 0
    {
        if(index<0)
            index=0;
        else if(index>15)
            index=15;

        return data->codeParam32(index);
    }
    //! изменить значение параметра
    void setCodeParam32(uint8_t index,uint32_t value) //! index - номер параметра, начинается с 0
    {
        if(index<0)
            index=0;
        else if(index>15)
            index=15;

        data->setCodeParam32(index,value);
    }
    //! добавить параметр по МКИО(в конкретный подадрес)
    void addParamMIL(TParam param)
    {
        data->addParamMIL(param);
        params.push_back(param);
    }
    int checkKC(int RT = 0, int numParams = -1) //RT(Recive(0)/Transmit(1)
    {
        return 1;
    }

    //! кол-во слов и параметров
    uint32_t sizeWords(){return params.size();}
    TParam* at(uint32_t index)
    {
        if(index>sizeWords())
            return 0;
        return &(params[index]);
    }
    T* spec(){return data;}
private:
    //! параметры передаваемые по МКИО
    std::vector <TParam> params;
    T *data;
};

class MIL_QT
{
public:
    MIL_QT()
    {

    }
    static bool initApi()
    {
        return true;
        //return T::initMilApi();
    }
    static uint8_t maxNumCh()
    {
        return 0;
        //return T::maxNumChMIL();
    }
    static bool isInit(){return initOk;}
private:
    static bool initOk;
};

//typedef LayerMIL::MIL<LayerMIL::MIL_QT>                         LayerConfMIL;

//! обобщенный класс ОУ(минимальный общий интерфейс для всех плат МКИО в режиме ОУ)
//! для использования специфических функций адаптеров нужно использовать функцию spec()
template <class M,class P> class Ou_MIL
{
public:
    Ou_MIL(MIL_LINE line,       /*номер линии*/
           int addrOU,          /*адрес ОУ*/
           MIL_ADAPTER adapter  /*номер адаптера*/
           )
    {
        enable_ = false;
        ou      = new M(line,addrOU,adapter);
    }
    Ou_MIL(int line,       /*номер линии*/
           int addrOU,     /*адрес ОУ*/
           int adapter     /*номер адаптера*/
           )
    {
        enable_ = false;
        ou      = new M(line,addrOU,adapter);
    }
    //! инициализация адаптеров
    static void init()
    {
        M::ou->init();
    }   
    static void readFromAdapter(int adapter,    /* номер адаптера*/
                                int line,       /* номер линии*/
                                int addrOU,     /* адрес ОУ*/
                                int subAddrOU,  /* подадрес ОУ*/
                                uint32_t *value)/* массив замечаний*/
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        M::readFromAdapter(adapter,line,addrOU,subAddrOU,value);
    }
    static void writeToAdapter(int adapter,    /* номер адаптера*/
                               int line,       /* номер линии*/
                               int addrOU,     /* адрес ОУ*/
                               int subAddrOU,  /* подадрес ОУ*/
                               uint32_t *value)/* массив замечаний*/
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        M::writeToAdapter(adapter,line,addrOU,subAddrOU,value);
    }
    Ou_MIL(int idCh)
    {

    }
//    static void set(int adapter,    /* номер адаптера*/
//                    int line,       /* номер линии*/
//                    int addrOU,     /* адрес ОУ*/
//                    int subAddrOU,  /* подадрес ОУ*/
//                    int idParam)
//    {
//        M::set(idCh,idParam);
//    }
//    static void get(uint32_t idCh,    //! идентификатор канала
//                    uint32_t idParam)
//    {
//        M::get(idCh,idParam);
//    }

    static bool isRead(uint16_t idCh)
    {
        TCh* ch = HAL::obj()->findCh(idCh);
        return (ch->setting.ioCh == 0);
    }
    static uint16_t* getPtrToValue(uint16_t idCh,uint16_t guidParam)
    {
        uint16_t idParam;
        TCh* ch = HAL::obj()->findCh(idCh);

        TParamHAL * param = HAL::obj()->findParam(guidParam);
        //TTablePackParam *pack = HAL::obj()->findPack(idCh,guidParam);
        if(ch == 0 || param == 0)
            return 0;

        TDesMIL * mil = (TDesMIL*)ch->desData;
        //for(int i=0;i<HAL::obj()->paramTable.num;i++)
        for(int i=0;i<param->num;i++)
        {
            if(param->idCh[i] == idCh || param->num == 1)
            {
                idParam = param->idParamCh[i];
                //! анализ длины упаковки
//                if((pack->hBit - pack->lBit)>sizeof(uint16_t))
//                    return &(mil->s.ui16[idParam - 1]);
//                else
                    return &(mil->s.ui16[idParam - 1]);
            }
        }
        return 0;
    }
//    static TParamHAL* getPtrToValue(uint16_t idCh,uint16_t idParam)
//    {
//        return 0;

//    }
    //! передать в ОУ
    void setOU(ParamMIL<P> *param,TSetFlagOU flag=F_WRITE_AND_SEND)
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        ou->setOU(param,flag);
    }
    //! расчет контррльной суммы
    short int calcValidSum(LayerMIL::ParamMIL<P> *param,int numWithout/*1...*/, int numWords);


    //! получить из ОУ
    void getOU(ParamMIL<P> *param,TGetFlagOU flag=F_RECIVE_AND_READ)
    {
//        if(LayerConfMIL::isInit() == false)
//            return;
        ou->getOU(param,flag);
    }
    //! добавление подадресов
    void addParamToSubAddr(int subAdr,ParamMIL<P>  *param)
    {
        ou->addParamToSubAddr(subAdr,param);
    }

    //! вернуть указатель на специф. объект
    M *spec()
    {
        return ou;
    }
    void clearSubAddr(ParamMIL<P> *param)
    {
         ou->clearSubAddr(param);
    }
    //! проверка прихода КС для данного ОУ
    /*bool setCheckKC(int formatKC)
    {
        return ou->setCheckKC(formatKC);
    }*/
    //! проверка прихода КС
    static bool checkKC(int16_t idCh)
    {
        TCh* ch = HAL::obj()->findCh(idCh);

        if(ch == 0)
            return false;
        if(ch->typeCh!=E_CH_MIL)
            return false;

        if(ch->typeNode != HAL::obj()->typeCurrentNode ||
             ch->idNode   != HAL::obj()->currentNode )
          return false;

        TDesMIL * mil = (TDesMIL*)ch->desData;
        //! определяем код КС
        uint16_t KC = 0x0000;
        //! номер оконечника, прием/передача
        KC = (mil->addr<<11)|(mil->subAddr<<5)| (mil->typeTrans<<10) | mil->numWord;
        return M::checkKC(ch->setting.numAdapter,ch->setting.numCh,KC);
    }
    //! функции включения/отключения адаптера
    void switchOn()
    {
//        if(LayerConfMIL::isInit() == false)
//          return;
        if(enable_ == false)
        {
            enable_= true;
            ou->setOn(true);
        }
    }

    //! cформировать отказ по линии связи
    void setFault(TFaultsOU value)
    {
//        if(LayerConfMIL::isInit() == false)
//            return;
        ou->setFault(value);
    }
    //! выключить ОУ
    void switchOff()
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        if(enable_ == true)
        {
            enable_=false;
            ou->setOn(false);
        }
    }
    //! выключить/выключить ОУ
    void enable(bool value)
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        if(enable_ != value)
        {
            enable_ = value;
            ou->setOn(value);
        }
    }
    static void enable(int line,       /* номер линии*/
                       int addrOU,     /* адрес ОУ*/
                       int adapter,    /* номер адаптера*/
                       int value)
    {
//        if(LayerConfMIL::isInit() == false)
//            return;
        M::enable(line,
                 addrOU,
                 adapter,
                 value);
    }
    //! выключить/выключить ОУ
    void enable(int value)
    {
//        if(LayerConfMIL::isInit() == false)
//           return;
        if(enable_ != value)
        {
            enable_=value;
            ou->setOn(value);
        }
    }
private:
    //! спецификация ОУ
    M *ou;
    bool enable_;
};

//! класс монитора МКИО
template <class M,class P> class Monitor_MIL
{
public:
    Monitor_MIL(int line,
                int addrOU)
    {
        mon = new M(line,addrOU);
    }
    //! связывание подадреса с параметрами
    void addParamToSubAddr(int subAdr,TTransDirect trans, ParamMIL<P>  *param)
    {
        mon->addParamToSubAddr(subAdr,trans,param);
    }
    //! функция чтения данных из монитора
    static void readFromAdapter(int adapter,    /* номер адаптера*/
                                int line,       /* номер линии*/
                                int addrOU,     /* адрес ОУ*/
                                int subAddrOU,  /* подадрес ОУ*/
                                uint32_t *value)/* массив замечаний*/
    {
        //M::readFromAdapter(adapter,line,addrOU,subAddrOU,value);
    }
    //! получить все данные из Монитора
    void getMonitor()
    {
        mon->getMonitor();
    }
private:
    //! спецификация ОУ
    M *mon;
};

}



#ifdef VXWORKS_PLATFORM
#ifdef VXWORKS_SIM
#include "layerSpecMIL.h"
typedef LayerMIL::Ou_MIL<Ou_MIL_Default,ParamMILDefault>        LayerOuMIL;
typedef LayerMIL::ParamMIL<ParamMILDefault>                     LayerParamMIL;
typedef LayerMIL::Monitor_MIL<Ou_MIL_Default,ParamMILDefault>   LayerMonMIL;

#else
#include "layerSpecMIL_ASP.h"
typedef LayerMIL::Ou_MIL<Ou_MIL_4Ch_Vx<ParamMIL_Vx>,ParamMIL_Vx>        LayerOuMIL;//LayerTemplRK<LayerRKDefault> LayerRK;
typedef LayerMIL::ParamMIL<ParamMIL_Vx>                                 LayerParamMIL;
typedef LayerMIL::Monitor_MIL<Ou_MIL_4Ch_Vx<ParamMIL_Vx>,ParamMIL_Vx>   LayerMonMIL;
typedef LayerMIL::MIL<MIL_4Ch>   LayerConfMIL;
#endif

#else

#include "layerSpecMIL.h"

#endif

#endif

