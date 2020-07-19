#ifndef LAYERSPECRTM_H
#define LAYERSPECRTM_H
#include "platform.h"
#include <vector>
#include "bits.h"
#include "hal.h"
//#define MINOR_SCALE(X) X<<
#define MAJOR_SCALE(X) X*2

#ifdef VXWORKS_PLATFORM
#include <Ar16t16rAPI.h>
#endif
#ifdef QNX_PLATFORM
#include "qnxLayerArApi.h"
#endif
class LayerArincVx;

template <class T> class AR_CONF
{
public:
    AR_CONF()
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
    static bool isReady(){return (initOk && (numCh>0));}
    static bool isInit(){return initOk;}
private:
    static bool initOk;
    static uint8_t numCh;
};
class AR_CONF_QT
{
public:
    AR_CONF_QT()
    {

    }
    static bool initApi()
    {
        return true;
    }
    static uint8_t maxNumCh()
    {
        return 0;
    }
};
//! описание обмена данных по Arinc
template <typename T> class LayerArincTempl
{
public:
    //! частота принимаемого сигнала
    enum TFreq
    {
        KBs_12_5 = 0x2,
        KBs_50   = 0x0,
        KBs_100  = 0x1
    };
    //! настройки передачи данных
    enum TTrans
    {
        ALWAYS = 0,//циклическая выдача(передавать всегда)
        ONE    = 1 //однократная выдача(один раз)
    };
    //! тип ГОСТа
    enum TGOST
    {
        REV_RTM2,
        REV_RTM3
    };
    enum TDir
    {
        F_RECIVE = 0,
        F_SEND   = 1
    };      
    

//    static int openArAPI()
//    {
//        return OK;
//    }
    static int maxNumChAr()
    {
          return 16;
    }
    //! зададим циклограмму
    LayerArincTempl& operator<<(uint32_t value)
    {
        listAddr[lenAddr] = value;
        lenAddr++;
        return *this;
    }

    void writeCode(uint8_t addr,uint32_t value,uint8_t io = 1)
    {

    }
    void clear()
    {
        lenAddr = 0;
    }
    void switchOn()
    {
//        if(LayerConfAr::isReady() == false)
//            return;
        T::switchCh(numCh,io,1);
    }
    static void switchAr(uint32_t numCh_, uint8_t io_,uint8_t value_)
    {
//        if(LayerConfAr::isReady() == false)
//            return;
        T::switchCh(numCh_,io_,value_);
    }
    void switchOff()
    {
//        if(LayerConfAr::isReady() == false)
//            return;
        T::switchCh(numCh,io,0);
    }
    static void writeTo(uint16_t numCh,uint8_t io,uint32_t val,uint8_t addr, uint8_t indexAddr)
    {
//        if(LayerConfAr::isReady() == false)
//            return;
        T::writeTo(numCh,io,val,addr,indexAddr);
    }
    static uint32_t readFrom(uint16_t numCh,uint8_t io,uint8_t addr, uint8_t indexAddr)
    {
//        if(LayerConfAr::isReady() == false)
//            return 0x0;
        return T::readFrom(numCh,io,addr,indexAddr);
    }

//    void pack()
//    {
//        TDesAr *ar = &(HAL::obj()->chTable.ch[numCh]->desData);
//         
//        for(int i=0;i<ar->numIndAddr[ar->curIndArray];i++)
//        {
//            val[i] = ar->value[ar->indAddr[i][ar->curIndArray]];
//            T::writeTo(numCh,io,val,ar->addr[ar->indAddr[i][ar->curIndArray]],i);
//             
//        }
//        /*for(int i=0;i<lenAddr;i++)
//        {
//            T::writeTo(numCh,io,val,listAddr[i],i);
//        }*/
//    }
//    void unPack()
//    {
//        for(int i=0;i<lenAddr;i++)
//        {
//            T::readFrom(numCh,io,val,listAddr[i],i);
//        }
//    }
    LayerArincTempl()
    {
        lenAddr = 0;
        numCh   = 0;
        val     = 0;
    }
    //! деструктор
    ~LayerArincTempl(){}
    //! инициализация канала
    void initCh(uint16_t numCh_,
                TDir     io_    = F_SEND,  /*направление работы канала передача/прием*/
                TFreq    freq_  = KBs_100, /*скорость канала*/
                TTrans   trans_ = ALWAYS,  /*передача слов разовая/постоянная*/
                TGOST    g_     = REV_RTM2)
    {
        numCh   = numCh_;
        io      = io_;
        freq    = freq_;
        trans   = trans_;
        g       = g_;
        if(io == F_RECIVE)
            val = T::valChIn;
        else
            val = T::valChOut;
//        if(LayerConfAr::isReady() == false)
//            return ;
        T::init(numCh,io,listAddr,lenAddr,trans,freq,3,0,0);

    }
    static void initCh(uint16_t numCh_,
            uint8_t *listAddr_,
            uint8_t lenAddr_,             
            uint8_t io_    = F_SEND,  /*направление работы канала передача/прием*/
            uint8_t freq_  = KBs_100, /*скорость канала*/
            uint8_t trans_ = ALWAYS,  /*передача слов разовая/постоянная*/
            uint8_t g_     = REV_RTM2)
    {
//        if(LayerConfAr::isReady() == false)
//            return ;
        T::init(numCh_,io_,listAddr_,lenAddr_,trans_,freq_,3,0,0);
    }
    static bool isRead(uint16_t idCh)
    {
        return (HAL::obj()->chTable.ch[idCh].setting.ioCh == 0);       
    }
    //! получить ссылку на элемент внутр. буфера
    uint32_t& getPtrToValue(uint8_t addr)
    {
        return val[(numCh << 9) + addr];
    }
    static uint32_t* getPtrToValue(uint16_t idCh,uint16_t guidParam)
    {
        uint16_t idParam;
        
        HAL *tH = HAL::obj();
        TDesAr *ar = (TDesAr*)(tH->chTable.ch[idCh].desData);
        TParamHAL *param = &(tH->paramTable.param[guidParam]);       
        for(int i=0;i<param->num;i++)
        {
            if(param->idCh[i] == idCh || param->num == 1)
            {
                idParam = param->idParamCh[i];               
            }
        }

        for(int i =0; i<ar->numAddr;i++)
        {
            if(ar->addr[i] == idParam)
                return &(ar->value[i]);
        }

        return 0;
    }
    //! список адресов для формирования циклограммы
    uint8_t listAddr[256];
    //! кол-во элементов в списке
    uint8_t lenAddr;
    //! номер канала
    uint16_t numCh;
    //! частота сигнала
    TFreq freq;
    //! тип передачи
    TTrans trans;
    //! направление передачи данных
    TDir io;
    //! гост передачи данных
    TGOST g;
    //! указатель на буфер(входной или выходной зоны)
    uint32_t *val;
};

class LayerArincDefault
{
public:
    //! временная переменная на внутр. буфер с параметрами(пока только для отладки)
    //static uint32_t val[256/*адресов*/*32/**/];
    //! список адресов для формирования циклограммы
    //std::vector<uint32_t> list;

    static void init(uint8_t numCh,
                     uint8_t io,
                     uint8_t *listAddr,
                     uint8_t lenAddr,
                     uint8_t trans,
                     uint8_t freq,
                     uint8_t interval,
                     uint8_t swicthBuf,
                     uint8_t gotov)
    {

    }
    static void switchCh(uint16_t numCh,uint8_t io,uint8_t value)
    {
      //  SetStartArincAPI(numCh,io,value);
        //trrel48IosInit();
    }

    static void writeTo(uint16_t numCh,uint8_t io,uint32_t val,uint8_t addr, uint8_t indexAddr)
    {
        //WriteLongWordArincAPI(numCh,io,addr,val[(numCh << 9) + addr]);

    }
    static uint32_t readFrom(uint16_t numCh,uint8_t io,uint8_t val,uint8_t addr)
    {
        return 0;
      //  val[(numCh << 9) + addr] = ReadLongWordArincAPI(numCh,io,addr);
    }
    //! временная переменная на внутр. буфер с параметрами(пока только для отладки)
    static uint32_t valChOut[256/*адресов*/*32/**/];
    static uint32_t valChIn[256/*адресов*/*32/**/];
};
class InitCh
{
public:
    uint8_t numCh;
    uint8_t lenAddr;
    uint8_t *listAddr;
    uint8_t trans;
    uint8_t freq;
    uint8_t gotov;
    uint8_t switchBuf;
    uint8_t interval;
};
#ifdef QNX_PLATFORM
class AR_CONF_VX
{
public:
    AR_CONF_VX()
    {
        
    }
    static bool initApi()
    {       
        return OpenArincAPI() == OK;      
    }
    static uint8_t maxNumCh()
    {
        return numChannelArinc();        
    }
};
class LayerArincQnx
{
public:
    //! временная переменная на внутр. буфер с параметрами(пока только для отладки)
    static uint32_t valChOut[256/*адресов*/*32/*бит слова*/];
    static uint32_t valChIn[256/*адресов*/*32/*бит слова*/];
    
    //! произведем действие
    //static void init(uint8_t io, T objInit)
    static void init(uint8_t numCh,
                     uint8_t io,
                     uint8_t *listAddr,
                     uint8_t lenAddr,
                     uint8_t trans,
                     uint8_t freq,
                     uint8_t interval,
                     uint8_t swicthBuf,
                     uint8_t gotov)
    {
        /*if(io == 1)
            SetModeTRArincAPI(numCh,
                              lenAddr,
                              listAddr,
                              trans,
                              freq,
                              interval,
                              swicthBuf,
                              gotov);
        else
            SetModeRCArincAPI(numCh,
                              lenAddr,
                              listAddr,
                              freq,
                              swicthBuf,
                              gotov);*/
    }
    static void writeTo(uint16_t numCh,uint8_t io,uint32_t val,uint8_t addr,uint8_t indexAddr)
    {
       // WriteLongWordArincAPI(numCh,io,indexAddr,swapChAddr(addr)|val);
    }
    static uint32_t readFrom(uint16_t numCh,uint8_t io,uint8_t addr,uint8_t indexAddr)
    {
        return 0;//ReadLongWordArincAPI(numCh,io,indexAddr);
    }
    static void switchCh(uint16_t numCh,uint8_t io,uint8_t value)
    {
        //SetStartArincAPI(numCh,io,value);        
    }
};
#endif
#ifdef VXWORKS_PLATFORM

class AR_CONF_VX
{
public:
    AR_CONF_VX()
    {
        
    }
    static bool initApi()
    {       
        return OpenArincAPI() == OK;      
    }
    static uint8_t maxNumCh()
    {
        return numChannelArinc();        
    }
};
class LayerArincVx
{
public:
    //! временная переменная на внутр. буфер с параметрами(пока только для отладки)
    static uint32_t valChOut[256/*адресов*/*32/*бит слова*/];
    static uint32_t valChIn[256/*адресов*/*32/*бит слова*/];
    
    //! произведем действие
    //static void init(uint8_t io, T objInit)
    static void init(uint8_t numCh,
                     uint8_t io,
                     uint8_t *listAddr,
                     uint8_t lenAddr,
                     uint8_t trans,
                     uint8_t freq,
                     uint8_t interval,
                     uint8_t swicthBuf,
                     uint8_t gotov)
    {
        if(io == 1)
            SetModeTRArincAPI(numCh,
                              lenAddr,
                              listAddr,
                              trans,
                              freq,
                              interval,
                              swicthBuf,
                              gotov);
        else
            SetModeRCArincAPI(numCh,
                              lenAddr,
                              listAddr,
                              freq,
                              swicthBuf,
                              gotov);
    }
    static void writeTo(uint16_t numCh,uint8_t io,uint32_t val,uint8_t addr,uint8_t indexAddr)
    {
        WriteLongWordArincAPI(numCh,io,indexAddr,swapChAddr(addr)|val);
    }
    static uint32_t readFrom(uint16_t numCh,uint8_t io,uint8_t addr,uint8_t indexAddr)
    {
        return ReadLongWordArincAPI(numCh,io,indexAddr);
    }
    static void switchCh(uint16_t numCh,uint8_t io,uint8_t value)
    {
        SetStartArincAPI(numCh,io,value);        
    }
};
#endif


#ifdef VXWORKS_SIM
typedef LayerArincTempl<LayerArincDefault > LayerArinc;
typedef LayerArincTempl<LayerArincDefault > LayerAr;
typedef  AR_CONF<AR_CONF_VX> LayerConfAr;
#else
#ifdef QNX_PLATFORM
typedef LayerArincTempl<LayerArincQNX > LayerArinc;
typedef LayerArincTempl<LayerArincQNX > LayerAr;
typedef  AR_CONF<AR_CONF_QNX> LayerConfAr;
#else
#ifdef VXWORKS_PLATFORM
typedef LayerArincTempl<LayerArincVx> LayerArinc;
typedef LayerArincTempl<LayerArincVx> LayerAr;
typedef  AR_CONF<AR_CONF_VX> LayerConfAr;
#else
typedef LayerArincTempl<LayerArincDefault > LayerArinc;
typedef LayerArincTempl<LayerArincDefault > LayerAr;
typedef  AR_CONF<AR_CONF_QT> LayerConfAr;
#endif
#endif // LAYERSPECRTM_H
