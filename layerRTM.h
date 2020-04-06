#ifndef LAYERRTM_H
#define LAYERRTM_H

#include <intLib.h >
#include <stl.h>
#include "ArincVx.h"
#include "AdapterExtFunc.h"

//! максимальное кол-во каналов Arinc
#define MAX_ARINC_CHANNEL 32

namespace LayerRTM
{
//! описание канала для передачи данных по стандарту ARINC-429
class ArincChannel
{
public:
    enum TFrec
    {
        KBs_50,
        KBs_100
    };
    //! настройки передачи данных
    enum TTrans
    {
        ALL,//циклическая выдача(передавать всегда)
        ONE //однократная выдача(один раз)
    };
    enum TGOST
    {
        REV_RTM2,
        REV_RTM3
    };
    enum TDIRECTION
    {
        F_RECIVE,
        F_SEND
    };
    enum TTO_TYPE
    {

    };
    enum TFROM_TYPE
    {

    };

    ArincChannel()
    {
        params.clear();
        words.clear();
        numChannel=-1;
        init=0;
    }
//    //! распаковка параметров
//    void unPack()
//    {
//        if(numChannel == -1)
//        {
//            return;
//        }

//        for(unsigned i=0;i<params.size();i++)
//            ParametrInRecArinc(&(params[i]));
//    }
//    //! упаковка параметров
//    void pack()
//    {
//        if(numChannel == -1)
//        {
//            return;
//        }

//        for(unsigned i=0;i<params.size();i++)
//            ParametrOutRecArinc(&(params[i]));

//        if((ch.Regim&TRANSONE)!=0)
//            StartCanalTransOneCycle(&ch);
//        IncCounterTrans(&ch);
//    }
    int numCh(){return numChannel;}

    //! обнуление параметров приемной зоны
    void setCodeRecZone(int addr,unsigned value)
    {
        if(numChannel == -1)
        {
            return;
        }
        SetCodeToInRec8Arinc(numChannel,addr,value);
    }

    //! получить значение
    unsigned int getValue(int addr/*8ричной С*/)
    {
        if(numChannel == -1)
            return 0;

        return GetCodeFromInRecArinc(numChannel,addr);
    }

    //! инициализация канала по передачи
    //! (вызвать нужно после добавления параметров и настройки номера канала)
    void initChannel(int nCh,                   /*номер канала от 1..*/
                     TDIRECTION direct  = F_SEND,  /*направление работы канала передача/прием*/
                     TFrec frec         = KBs_100,        /*скорость канала*/
                     TTrans trans       = ALL,          /*передача слов разовая/постоянная*/
                     TGOST g            = REV_RTM2)          /*стандарт упаковки 31,32 бита в слове*/
    {
        numChannel=nCh;
        ch.NumCanal = nCh;

        //скорость передачи информации
        if(frec == KBs_50)
            ch.Regim=SETUPRCVCANAL;
        else if(frec==KBs_100)
            ch.Regim=SETUPTRANCANAL100;

        if(g == REV_RTM2)
            ch.DesCanalAdr.Kind=0;
        else
            ch.DesCanalAdr.Kind=1;
        //кол-во адресов
        ch.DesCanalAdr.NumberAdr=words.size();

        ch.DesCanalAdr.K.PtrInt=new int[words.size()];
        for(unsigned i=0;i<words.size();i++)
        {
            ch.DesCanalAdr.K.PtrInt[i]=words[i];
        }
        for(unsigned i=0;i<params.size();i++)
        {
            params[i].NumCanal=numChannel;
        }
        //! напавление передачи
        if(direct == F_SEND)
        {
            if(trans == ONE)
                ch.Regim|=TRANSONE;

            if(init == 0)
            {
                for(unsigned i=0;i<params.size();i++)
                {
                    params[i].NomerWord=searchAdrInList(words,params[i].NomerWord);
                }
            }

            if(trans == ONE)
                InitCanalTransInt(&ch,0);
            else
                InitCanalTransInt(&ch);
        }
        else
        {
            InitCanalRecInt(&ch);
        }

        init=1;
    }

    void switchOn()
    {

    }
    void switchOff()
    {

    }

    //добавить параметр для према
    void addParam(int adr,               /*адрес в 8-ричной системе*/
                  void *addrValue,       /*указатель на переменную*/
                  TParametrs typeFrom,   /*из типа*/
                  TParametrs typeTo,     /*в тип*/
                  int beg,               /*начальный разряд*/
                  int end,               /*конечный разряд*/
                  float scale_,          /*масштаб*/
                  int sign_)             /*знак*/
    {
        //Поиск адреса в уже сохраненном списке адресов
        int res=searchAdrInList(words,adr);
        if(res==-1)
            words.push_back(adr);

        //////////////////////////////////////////////
        TDesTransParametrArinc newParam;

        newParam.NumCanal   = numChannel;
        newParam.NomerWord  = adr;
        newParam.Ptr        = addrValue;

        newParam.InKind     = typeFrom;
        newParam.OutKind    = typeTo;
        newParam.Beg        = beg;
        newParam.End        = end;
        newParam.Scale      = scale_;
        newParam.Sign       = sign_;

        params.push_back(newParam);
    }
    //! поиск адреса adr в списке list,
    //! если обнаружен, то возвращается индекс найденного элемента
    //! иначе -1
    int searchAdrInList(vector<int> &list, int adr)
    {
        for(unsigned i=0;i<list.size();i++)
        {
            if(list[i]==adr) return i;
        }
        return -1;
    }

    //! описание на канал
    TDesCananlArinc* channel(){return &ch;}
private:
    //! структуры для приема/передачи данных по стандарту Arinc
    TDesCananlArinc ch;
    //! список параметров для приема/отправки значений
    vector<TDesTransParametrArinc>   params;
    //! направление прием/передача
    TDIRECTION direct;
    //! списки с адресами
    vector<int>   words;
    //! номер канала(если -1, то канал не инициализирован, обмена не будет)
    int numChannel;
    //! признак проведения 1 инициализации
    int init;

};
}
#endif
