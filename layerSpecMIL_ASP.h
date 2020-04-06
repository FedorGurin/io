#ifndef LAYERSPECMIL_IASP
#define LAYERSPECMIL_IASP

#include "layerMIL.h"
#include "DesMkioVx.h"
#include "MonMil.h"
#include "MonNik_api.h"
#include <vector>
//#include "layerSpecMIL_ASP.h"
//#include "IAspMkioVx.h"
using namespace LayerMIL;


#ifndef VXWORKS_PLATFORM
//! класс описания параметров(используется как заглушка)
class ParamMIL_IASP
{
public:
    ParamMIL_IASP()
    {
    }
    ~ParamMIL_IASP()
    {

    }
    //! прочитать код параметра
    unsigned short int codeParam16(int index) //! index - номер параметра, начинается с 0
    {
        Q_UNUSED(index);
        return 0;
    }
    void setCodeParam16(int index,int value) //! index - номер параметра, начинается с 0
    {
        Q_UNUSED(index);
        Q_UNUSED(value);

    }
    //! прочитать значение параметра
    unsigned int codeParam32(int index) //! index - номер параметра, начинается с 0
    {
    Q_UNUSED(index);
        return 0;
    }
    //! изменить значение параметра
    void setCodeParam32(int index,int value) //! index - номер параметра, начинается с 0
    {
        Q_UNUSED(index);
        Q_UNUSED(value);

    }
    //! добавить параметр по МКИО(в конкретный подадрес)
    void addParamMIL(TParam param)
    {
        TDesParamMkio param;

        param.AdrData=      addr;
        param.NumberWord=   numWord;

        param.InType=       toType;
        param.OutType=      fromType;

        param.Beg=          lBit;
        param.End=          hBit;

        param.Scale=        scale;
        param.Sign=         sign;
    }

private:
    //! параметры передаваемые по МКИО
    vector <TDesParamMkio> params;
    //! описание параметров(массив)
    TDesDataMkio  des_data;
};

#endif
//! класс описания параметров
class ParamMIL_Vx
{
public:
    ParamMIL_Vx()
    {
        params.clear();

        des_data.NumParam=0;
        des_data.DEFAROU=0;
        des_data.DesParamMkio=0;
    }
    ~ParamMIL_Vx()
    {

    }
    unsigned short int codeParam16(int index) //! index - номер параметра, начинается с 0
    {
        if(index<0) index=0;
        else if(index>31) index=31;

        return des_data.DEFAROU->ArrAtmk.SI[index];
    }
    void setCodeParam16(int index,int value) //! index - номер параметра, начинается с 0
    {
        if(index<0) index=0;
        else if(index>31) index=31;

        des_data.DEFAROU->ArrAtmk.SI[index]=value;
    }
    //! прочитать значение параметра
    unsigned int codeParam32(int index) //! index - номер параметра, начинается с 0
    {
        if(index<0) index=0;
        else if(index>15) index=15;

        return des_data.DEFAROU->ArrAtmk.LI[index];
    }
    //! изменить значение параметра
    void setCodeParam32(int index,int value) //! index - номер параметра, начинается с 0
    {
        if(index<0) index=0;
        else if(index>15) index=15;

        des_data.DEFAROU->ArrAtmk.LI[index]=value;
    }
    //! добавить параметр по МКИО(в конкретный подадрес)
       void addParamMIL(TParam in)
       {
           TDesParamMkio param;

           param.AdrData=      in.addr;
           param.NumberWord=   in.numWord;

           if(in.toType == F_DOUBLE)
               param.InType = DOUBLE;
           else if(in.toType == F_FLOAT)
               param.InType = FLOATM;
           else if(in.toType == F_CODE)
               param.InType = CODEM;
           else if(in.toType == F_INT)
               param.InType = INTEGER;
           else if(in.toType == F_TWOWORDS)
               param.InType = TWOWORDS;

           if(in.fromType == F_DOUBLE)
               param.OutType = DOUBLE;
           else if(in.fromType == F_FLOAT)
               param.OutType = FLOATM;
           else if(in.fromType == F_CODE)
               param.OutType = CODEM;
           else if(in.fromType == F_INT)
               param.OutType = CODEM;
           else if(in.fromType == F_BIT)
                      param.OutType = BITM;

           else if(in.fromType == F_TWOWORDS)
               param.OutType = TWOWORDS;
           /*param.InType=       in.toType;
           param.OutType=      in.fromType;*/

           param.Beg=          in.lBit;
           param.End=          in.hBit;

           param.Scale=        in.scale;
           param.Sign=         in.sign;

           params.push_back(param);
       }
    //! добавить параметр по МКИО(в конкретный подадрес)
    void addParamMIL(void *addr,
                     int numWord,
                     TTypeConv toType,
                     TTypeConv fromType,
                     int lBit,
                     int hBit,
                     double scale,
                     int sign)
    {
        TDesParamMkio param;

        param.AdrData=      addr;
        param.NumberWord=   numWord;

        param.InType=       toType;
        param.OutType=      fromType;

        param.Beg=          lBit;
        param.End=          hBit;

        param.Scale=        scale;
        param.Sign=         sign;

        params.push_back(param);
    }
    //! создать параметры
    void endOfParams()
    {       
        if(params.empty() == true)
        {
            des_data.NumParam=params.size();
            des_data.DesParamMkio = 0;            
            return;
        }
        des_data.NumParam=params.size();
        des_data.DesParamMkio=new TDesParamMkio[params.size()];

        int maxNumber = 0;
        for(unsigned i=0;i<params.size();i++)
        {
            des_data.DesParamMkio[i]=params[i];
            maxNumber++;
            if(params[i].OutType == TWOWORDS)
            	maxNumber++;
            
        }
        des_data.NumParam=params.size();
        
    }
   
    TDesDataMkio*  des(){return &des_data;}
private:
    //! параметры передаваемые по МКИО
    std::vector <TDesParamMkio> params;
    //! описание параметров(массив)
    TDesDataMkio  des_data;
};

class MIL_4Ch
{
public:
    MIL_4Ch()
    {
        
    }
    static bool initApi()
    {
        return InitMonitorMil();
        //return T::initMilApi();
    }
    static uint8_t maxNumCh()
    {
        return numAdapterMOM();
        //return T::maxNumChMIL();
    }
};//! 4ех канальный адаптер МКИО в режиме ОУ
template<typename T>  class Ou_MIL_4Ch_Vx
{
public:
    Ou_MIL_4Ch_Vx(MIL_LINE line,  /*номер линии*/
           int addrOU,          /*адрес ОУ*/
           MIL_ADAPTER adapter  /*номер адаптера*/
           )
    {
        ou_.NumberAdapter   = adapter;
        ou_.NumbrtInterface = line;
        ou_.AdrAbonent      = addrOU;
        faults_word         = 0x0;
        des.clear();
        numDes              = 0;
        ou_.StatData        = 0;
        initOk              = false;
        //SetRegimeRT(adapter,line);
    }
    Ou_MIL_4Ch_Vx(int line,       /*номер линии*/
           int addrOU,     /*адрес ОУ*/
           int adapter     /*номер адаптера*/
           )
    {
        ou_.NumberAdapter=   adapter;
        ou_.NumbrtInterface= line;
        ou_.AdrAbonent=      addrOU;
        ou_.StatData  = 0;
        faults_word=0x0;
        numDes = 0;
        initOk              = false;
        //SetRegimeRT(adapter,line);
    }
   //!   
    void setOU(ParamMIL<T> *param,TSetFlagOU flag=F_WRITE_AND_SEND)
    {

        if(flag==F_WRITE || flag==F_WRITE_AND_SEND)
        {
            ParametrOutMkio(param->spec()->des());
        }
        if(flag==F_SEND || flag==F_WRITE_AND_SEND)
        {
             SendDataToAdaperMOM(param->spec()->des()->DEFAROU);
        }
    }
    static void init()
    {
        initOk = InitMonitorMil();
        //return initOk;
    }
//    static void set(uint32_t idCh,    //! идентификатор канала
//                    uint32_t idParam)
//    {
////        HAL::obj()->
////        M::set(idCh,idParam);
//    }
//    static void get(uint32_t idCh,    //! идентификатор канала
//                    uint32_t idParam)
//    {
////        M::get(idCh,idParam);
//    }
    void setOn(bool on)
    {
        SetUpOu(&ou_,on);
        SetRegimeRT(ou_.NumberAdapter,ou_.NumbrtInterface,on);
    }

    static void readFromAdapter(int adapter,       /* номер адаптера*/
                                   int line,       /* номер линии*/
                                   int addrOU,     /* адрес ОУ*/
                                   int subAddrOU,  /* подадрес ОУ*/
                                   uint32_t *value)
    {
        GetDataFromAdaperMOM(adapter,line,addrOU,subAddrOU,value,0);
    }
    static void writeToAdapter(int adapter,      /* номер адаптера*/
                                 int line,       /* номер линии*/
                                 int addrOU,     /* адрес ОУ*/
                                 int subAddrOU,  /* подадрес ОУ*/
                                 uint32_t *value)
    {
        SendDataToAdaperMOM(adapter,line,addrOU,subAddrOU,value,1);
    }

    //! проверка прихода произвольного КС
    static bool checkKC(uint8_t numAdapter, uint8_t numLine, uint16_t KC)
    {
        int res = MOMVerifyInKC(numAdapter,numLine,KC);
        
        return (res==1);
    }
    
    //!   
    void getOU(ParamMIL<T> *param,TGetFlagOU flag=F_RECIVE_AND_READ)
    {
        if(flag==F_RECIVE || flag==F_RECIVE_AND_READ)
        {
            GetDataFromAdaperMOM(param->spec()->des()->DEFAROU);
        }
        if(flag==F_READ || flag==F_RECIVE_AND_READ)
        {
            ParametrInMkio(param->spec()->des());
        }
    }
    void clearSubAddr(ParamMIL<T>  *param)
    {
        memset((void*)&(param->des()->DEFAROU->ArrAtmk),0,sizeof(TArrAtmk));
        SendDataToAdaperMOM(param->des()->DEFAROU,1);
        SendDataToAdaperMOM(param->des()->DEFAROU,0);
    }
    //! добавление подадресов
    void addParamToSubAddr(int subAddr,ParamMIL<T>  *param)
    {
        param->spec()->endOfParams();
        //////////////////////////////////////////////////////////
        TKDEFAROU *temp_des=new TKDEFAROU;

        temp_des->SubAdr=subAddr;
        temp_des->DEFOU4MOM=&ou_;
        temp_des->Buffering=BUFFERING;

        memset((void*)(temp_des->ArrAtmk.LI),0,16* sizeof(unsigned int));
//        for(int i=0;i<16;i++)
//            temp_des->ArrAtmk.LI[i]=0;
        
        des[numDes++] = temp_des;
        ///////////связываем параметры с подадресами
        param->spec()->des()->DEFAROU=temp_des;

        //! сохраним указатель
        params.push_back(param);

    }
    //! функции включения/отключения адаптера
    void switchOn()
    {
        SetUpOu(&ou_,1);
        SetRegimeRT(ou_.NumberAdapter,ou_.NumbrtInterface,1);
    }
    //! вернуть указатель на специф. объект
    Ou_MIL_4Ch_Vx *spec()
    {
        return this;
    }
    //! cформировать отказ по линии связи
    void setFault(TFaultsOU value)
    {
        //Q_UNUSED(value);
    }
    //! выключить ОУ
    void switchOff()
    {
        SetUpOu(&ou_,0);
        //SetRegimeRT(&ou_,0);
    }
    static void enable(int line,       /* номер линии*/
                       int addrOU,     /* адрес ОУ*/
                       int adapter,    /* номер адаптера*/
                       int value)
    {
        SetUpOu(adapter,line,addrOU,value);
        //SetRegimeRT(adapter,line,value);
    }
    //! выключить/выключить ОУ
    void enable(bool value)
    {
        SetUpOu(&ou_,value);
        //SetRegimeRT(kk,value);
    }

private:
    //! результат инициализации
    bool initOk;
    //! слово отказов
    int faults_word;
    //! описание 4ех канального адапетра
    TKDEFOU4MOM  ou_;
    uint8_t numDes;
    //! описание данных(у каждого описания свои передаваемые параметры)
    TKDEFAROU*   des[8];
    //! параметры передаваемые по МКИО
    std::vector <ParamMIL<T> * >    params;

};
#endif // LAYERSPECMIL

