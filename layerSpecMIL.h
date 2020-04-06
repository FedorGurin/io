#ifndef LAYERSPECMIL_H
#define LAYERSPECMIL_H


#include "layerMIL.h"
#include "bits.h"
using namespace LayerMIL;
typedef union TArrayMIL_
{
    uint32_t    ui32[16];
    uint16_t    ui16[32];
}TArrayMIL;

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
//! портированная функция Фазлова Н.И.
template <class T> void ParametrInMkio(ParamMIL<T>  *params)
{
    union
    {
        float f;
        uint16_t ar[2];
        uint32_t Ud;
    }Jk;

    float  Dfloat = 0;
    double Ddouble= 0;
    int    Dint   = 0;


    TParam *param = 0;

    for (uint32_t i=0;i<params->sizeWords();i++)
    {
        param = params->at(i);

        switch (param->fromType)
        {
//        case F_DOUBLE:
//            Jk.ar[0] = params->spec()->packBuf.ui16[param->numWord - 1];
//            Jk.ar[1] = params->spec()->packBuf.ui16[param->numWord];
//            Dfloat   = Jk.f;

//            switch (param->toType)
//            {
//            case F_INT:
//                Dint=(int)Dfloat;
//                *((int*)param->addr)=Dint;
//                break;
//            case F_DOUBLE:
//                *((double*)param->addr)=(double)Dfloat;

//                break;

//            case F_FLOAT:
//                *((float*)param->addr)=Dfloat;
//                break;
//            }
//            break;
        case F_TWOWORDS:
//            Jk.ar[1]= params->spec()->packBuf.ui16[param->numWord - 1];
//            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord];
            //! специальные изменения для ЛПО
            Jk.ar[1]= params->spec()->packBuf.ui16[param->numWord];
            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord - 1];
            Ddouble=FloatSokTwoShortNew (0,15,Jk.ar[1],  //GURIN 8.07.09
                    param->lBit,param->hBit, Jk.ar[0],param->sign);

            Ddouble*=param->scale;
            switch (param->toType)
            {
            case F_INT:
                Dint=(int)Ddouble;
                Dint=*((int*)param->addr);
                break;
            case F_DOUBLE:
                *((double*)param->addr)=Ddouble;
                break;
            case F_FLOAT:
                *((float*)param->addr)=(float)Ddouble;
                break;
            default:
                break;
            }
            break;

        case F_FLOAT:
        case F_DOUBLE:
            Jk.ar[0]=params->spec()->packBuf.ui16[param->numWord - 1];
            Ddouble=FloatSokNew(param->lBit,param->hBit,Jk.ar[0],param->sign);
            Ddouble*=param->scale;
            switch (param->toType)
            {
            case F_INT:
                Dint=(int)Ddouble;
                *((int*)param->addr)=Dint;
                break;
            case F_DOUBLE:
                *((double*)param->addr)=Ddouble;
                break;
            case F_FLOAT:
                *((float*)param->addr)=(float)Ddouble;
                break;
            default:
                break;
            }
            break;

        case F_BIT:
            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord - 1];
            Dint=BitSok(param->lBit,Jk.ar[0]);
            switch (param->toType)
            {
            case F_INT:
                *((int*)param->addr)=Dint;
                break;
            case F_DOUBLE:
                Ddouble=(int)Dint;
                *((double*)param->addr)=Ddouble;
                break;
            case F_FLOAT:
                Dfloat=(float)Dint;
                *((float*)param->addr)=Dfloat;
                break;
            default:
                break;
            }
            break;
        case F_INT:
        case F_CODE:
            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord - 1];
            Dint=IntSok(param->lBit,param->hBit,Jk.ar[0] );
            switch (param->toType)
            {
            case F_INT:
                *((int*)param->addr)=Dint;
                break;
            case F_DOUBLE:
                Ddouble=(int)Dint;
                *((double*)param->addr)=Ddouble;
                break;

            case F_FLOAT:
                Dfloat=(float)Dint;
                *((float*)param->addr)=Dfloat;
                break;
            default:
                break;
            }
            break;
        }
    }
}
template <class T> void ParametrOutMkio(ParamMIL<T>  *params)
{
    union
    {
        float f;
        uint16_t ar[2];
        uint32_t Ud;
    }Jk;

    float  Dfloat  = 0;
    double Ddouble = 0;
    int    Dint    = 0;

    TParam *param=0;

    Jk.Ud=0;
    for (uint32_t i=0;i<params->sizeWords();i++)
    {
         param = params->at(i);

        switch (param->fromType)
        {
        //        case REALCONV:

        //			   switch (ParamMkio->InType)

        //			{
        //			case INTEGER:
        //						 Dint=*((int*)ParamMkio->AdrData);
        //						 Dfloat=(float)Dint;
        //						  break;
        //			case DOUBLE:
        //						 Dfloat=*((float*)(double*)ParamMkio->AdrData);
        //						  break;

        //			case FLOATM:
        //						 Dfloat=*((float*)ParamMkio->AdrData);
        //						  break;


        //			}
        //					  Jk.f=Dfloat;
        //				  ArrAtmk->SI[ParamMkio->NumberWord-1]=Jk.ar[0];
        //				  ArrAtmk->SI[ParamMkio->NumberWord]=Jk.ar[1];;
        //				 break;

        case F_TWOWORDS:
            switch (param->toType)
            {
            case F_INT:
                Dint=*((int*)param->addr);
                Ddouble=(double)Dint;
                break;
            case F_DOUBLE:
                Ddouble=*((double*)param->addr);
                break;
            case F_FLOAT:
                Ddouble=(double)*((float*)param->addr);
                break;
            default:
                break;
            }
            NewPackSokWordInTwoWords(param->lBit,
                                     param->hBit,
                                     Ddouble,
                                     param->scale,
                                     param->sign,
                                     &Jk.ar[0],&Jk.ar[1]);

//            params->spec()->packBuf.ui16[param->numWord - 1]    = Jk.ar[1];
//            params->spec()->packBuf.ui16[param->numWord]        = Jk.ar[0];
            params->spec()->packBuf.ui16[param->numWord - 1]    = Jk.ar[0];
            params->spec()->packBuf.ui16[param->numWord]        = Jk.ar[1];

            break;

        case F_FLOAT:
        case F_DOUBLE:
            switch (param->toType)
            {
            case F_INT:
                Dint=*((int*)param->addr);
                Ddouble=(double)Dint;
                break;
            case F_DOUBLE:
                Ddouble=*((double*)param->addr);
                break;
            case F_FLOAT:
                Ddouble=(double) *((float*)param->addr);
                break;
            default:
                break;
            }

            PackSokWordNew(param->lBit,
                           param->hBit,
                           Ddouble,
                           param->scale,
                           param->sign,
                           &Jk.ar[0]);

            params->spec()->packBuf.ui16[param->numWord - 1] = Jk.ar[0];

            break;
        case F_BIT:
            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord - 1];
            switch (param->toType)
            {
            case F_INT:
                Dint=*((int*)param->addr);
                break;
            case F_DOUBLE:
                Ddouble=*((double*)param->addr);
                Dint=(int)Ddouble;
                break;

            case F_FLOAT:
                Dfloat=*((float*)param->addr);
                Dint= (int)Dfloat;
                break;
            default:
                break;
            }
            PackBitSok(param->lBit,&Jk.ar[0],Dint);

            params->spec()->packBuf.ui16[param->numWord - 1] = Jk.ar[0];
            break;
        case F_INT:
        case F_CODE:
            Jk.ar[0]= params->spec()->packBuf.ui16[param->numWord - 1];

            switch (param->toType)
            {
            case F_INT:
                Dint=*((int*)param->addr);
                break;
            case F_DOUBLE:
                Ddouble=*((double*)param->addr);
                Dint=(int)Ddouble;
                break;

            case F_FLOAT:
                Dfloat=*((float*)param->addr);
                Dint= (int) Dfloat;
                break;
            default:
                break;
            }
            PackIntSok(param->lBit,param->hBit,
                       &Jk.ar[0],Dint);

            params->spec()->packBuf.ui16[param->numWord - 1] = Jk.ar[0];

            break;
        }
        Jk.Ud=0;
    }
}
//! класс описания параметров(используется как заглушка)
class ParamMILDefault
{
public:
    ParamMILDefault()
    {
    }
    ~ParamMILDefault()
    {

    }
    //! прочитать код параметра
    uint16_t codeParam16(uint8_t index) //! index - номер параметра,index - [0..
    {
        return packBuf.ui16[index];
    }
    void setCodeParam16(uint8_t index,uint16_t value) //! index - номер параметра,index - [0..
    {
       packBuf.ui16[index] = value;
    }
    //! прочитать значение параметра
    uint32_t codeParam32(uint8_t index) //! index - номер параметра, index - [0..
    {
        return packBuf.ui32[index];
    }
    //! изменить значение параметра
    void setCodeParam32(uint8_t index,uint32_t value) //! index - [0..
    {
        packBuf.ui32[index] = value;
    }
    //! добавить параметр по МКИО(в конкретный подадрес)
    void addParamMIL(TParam /*param*/)
    {
        //Q_UNUSED(param);

    }
    //! буфер с упакованными данными
    TArrayMIL packBuf;
};

//! обобщенный класс ОУ(минимальный общий интерфейс для всех плат МКИО в режиме ОУ)
//! для использования специфических функций адаптеров нужно использовать функцию spec()
class Ou_MIL_Default
{
public:
    Ou_MIL_Default(MIL_LINE line_,  /*номер линии*/
           int addrOU_,             /*адрес ОУ*/
           MIL_ADAPTER adapter_)    /*номер адаптера*/
    {
        line    = line_;
        addrOU  = addrOU_;
        adapter = adapter_;
    }
    Ou_MIL_Default(int line_,   /*номер линии*/
           int addrOU_,         /*адрес ОУ*/
           int adapter_)        /*номер адаптера*/
    {
        line    = (MIL_LINE)line_;
        addrOU  = addrOU_;
        adapter = (MIL_ADAPTER)adapter_;
    }
    void clearSubAddr(ParamMIL<ParamMILDefault>* /*param*/)
    {

    }
    static void init()
    {
        
    }
    static void enable(int line,       /* номер линии*/
                           int addrOU,     /* адрес ОУ*/
                           int adapter,    /* номер адаптера*/
                           int value)
    {
        
    }
    static void readFromAdapter(int adapter,    /* номер адаптера*/
                                   int line,       /* номер линии*/
                                   int addrOU,     /* адрес ОУ*/
                                   int subAddrOU,  /* подадрес ОУ*/
                                   uint32_t *value)/* массив замечаний*/
    {
         

    }
    static void writeToAdapter(int adapter,    /* номер адаптера*/
                                  int line,       /* номер линии*/
                                  int addrOU,     /* адрес ОУ*/
                                  int subAddrOU,  /* подадрес ОУ*/
                                  uint32_t *value)/* массив замечаний*/
    {
         
    }
    //! расчет контрольной суммы
    short int calcValidSum(LayerMIL::ParamMIL<ParamMILDefault> *param,int numWithout/*1...*/, int numWords)
    {
        unsigned short int cSum = 0;
        for (int i = 0; i < numWords; i++)
        {
            if (i != (numWithout - 1))
                cSum += param->codeParam16(i);
        }
        return cSum;
    }
    //! передать в ОУ
    void setOU(ParamMIL<ParamMILDefault>* param,TSetFlagOU flag)
    {
        if(flag==F_WRITE || flag==F_WRITE_AND_SEND)
        {
            ParametrOutMkio(param);
        }
        if(flag==F_SEND || flag==F_WRITE_AND_SEND)
        {
             //SendDataToAdaper((param->des())->DEFAROU);
        }
    }
    void setOn(bool /*value*/)
    {
        //Q_UNUSED(value);
    }

    //! получить из ОУ
    void getOU(ParamMIL<ParamMILDefault>* params,TGetFlagOU flag)
    {
        if(flag==F_RECIVE || flag==F_RECIVE_AND_READ)
        {

            //GetDataFromAdaper(param->des()->DEFAROU);
        }
        if(flag==F_READ || flag==F_RECIVE_AND_READ)
        {
            ParametrInMkio(params);
        }
    }
    void clearSubAddr(ParamMIL<ParamMILDefault>/* *param*/)
    {
         //Q_UNUSED(param);
    }

    //! добавление подадресов
    void addParamToSubAddr(int /*subAdr*/,ParamMIL<ParamMILDefault>*  /*param*/)
    {

    }
    //! функции включения/отключения адаптера
    void switchOn(bool /*value*/)
    {
        //Q_UNUSED(value);
    }
    //! вернуть указатель на специф. объект
    Ou_MIL_Default *spec()
    {
        return 0;
    }
    //! cформировать отказ по линии связи
    void setFault(TFaultsOU/* value*/)
    {
        //Q_UNUSED(value);

    }
    static bool checkKC(uint8_t numAdapter, uint8_t numLine, uint16_t KC)
    {
        return false;        
    }
    //! выключить ОУ
    void switchOff()
    {

    }
    //! выключить/выключить ОУ
    void enable(bool /*value*/)
    {
        //Q_UNUSED(value);

    }
    //! выключить/выключить ОУ
    void enable(int/* value*/)
    {
       // Q_UNUSED(value);
    }

    MIL_LINE line;
    int addrOU;
    MIL_ADAPTER adapter;

    //std::vector<>
    //! список параметров
    std::vector<ParamMIL<ParamMILDefault>* > params;
};

//#ifdef VXWORKS_PLATFORM
//typedef LayerTemplRK<LayerRKVx > LayerRK;
//typedef LayerRKVx TypeSpecRK;
//#else
//typedef LayerMIL::Ou_MIL<Ou_MIL_Default,ParamMILDefault> LayerOuMIL;//LayerTemplRK<LayerRKDefault> LayerRK;
//typedef LayerMIL::ParamMIL<ParamMILDefault> LayerParamMIL;
//#endif

typedef LayerMIL::MIL<LayerMIL::MIL_QT>                         LayerConfMIL;
typedef LayerMIL::Ou_MIL<Ou_MIL_Default,ParamMILDefault>        LayerOuMIL;
typedef LayerMIL::ParamMIL<ParamMILDefault>                     LayerParamMIL;
typedef LayerMIL::Monitor_MIL<Ou_MIL_Default,ParamMILDefault>   LayerMonMIL;

#endif // LAYERSPECMIL

