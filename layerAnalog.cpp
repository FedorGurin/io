#include "layerAnalog.h"

#ifdef defined(VXWORKS_PLATFORM) || defined(QNX_PLATFORM)
/*template<>*/ LayerAnalog* LayerAnalog::layer=0;
/*template<>*/ TypeSpecAnalog *LayerAnalog::spec=0;

#else
template<> LayerAnalog* LayerAnalog::layer=0;
template<> TypeSpecAnalog *LayerAnalog::spec=0;
#endif


#ifdef VXWORKS_PLATFORM
LayerAnalogVx::LayerAnalogVx()
{
//        maxRksIn  = 0;
//        maxRksOut = 0;

//        rksIn    = 0;//new uint8_t[];
//        rksOut   = 0;

//        in_devices = -1;
//        out_devices= -1;

        dacw_out = (TDevInfo *) malloc(sizeof(TDevInfo));
        dacw_out->common.devname = "DACWAV";
        dacw_out->common.venname = "UNKNOWN";

//        sc_in = (struct sc_info_t *) malloc(sizeof(struct sc_info_t));
//        sc_in->common.devname = "RSREL48";
//        sc_in->common.venname = "UNKNOWN";


//        sc_out = (struct sc_info_t *) malloc(sizeof(struct sc_info_t));
//        sc_out->common.devname = "TRREL48";
//        sc_out->common.venname = "UNKNOWN";

        init();
    }

//! общая инициализация устройств
void LayerAnalogVx::init()
{
        int stDac;
        stDac = init(dacw_out,dacw_devices,HAL::obj()->numISA(E_A_DACWAV), 0,DACWAV_DEVPATH);

}
void LayerAnalogVx::readFromDevice()
{
    
}
//! принудительная запись в устройство
void LayerAnalogVx::writeToDevice()
{
    TAdapterISA *isa = 0;
        if(HAL::obj()->eisaAvailable == true)
        {
        //! пробегаем по всем каналам
        for(int  i = 0; i<HAL::obj()->chTable.numCh;i++)
        {
            TCh *ch = &(HAL::obj()->chTable.ch[i]);
            if(ch->typeNode == E_NODE_PV && ch->idNode == 1 )
            {
                //ch->setting.readyToWrite = 0;
                if(ch->typeCh == E_IR )
                {
                    isa = HAL::obj()->numISA(E_A_IR,E_NODE_EISA);
                }else if(ch->typeCh == E_IP)
                {
                    isa = HAL::obj()->numISA(E_A_IP,E_NODE_EISA);
                }else if(ch->typeCh == E_GEN_U)
                {
                    uint16_t chGen = 0;
                    isa = HAL::obj()->numISA(E_A_GEN,E_NODE_EISA);
                    //! номер адаптера
                    uint8_t numA = (ch->setting.numCh/isa->maxChAdapter);
                    uint8_t numCh= ch->setting.numCh - numA *isa->maxChAdapter;
                    if((numCh%2) > 0)
                    {
                        chGen = 1;
                        chGen<<=15;
                    }                    
                    //! описание данных
                    TDesGen_U *desGenU = (TDesGen_U*)(ch->desData);                    
                    //! номер канала
                    uint16_t word = desGenU->u | desGenU->phase<<12  | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 4,1,(uint8_t*)&(word),   sizeof(word));
                    word = desGenU->f | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 6,1,(uint8_t*)&(word),   sizeof(word));
                    word = desGenU->ff | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 8,1,(uint8_t*)&(word),   sizeof(word));

                }else if(ch->typeCh == E_GEN_NU)
            {
                    uint16_t chGen = 0;
                    isa = HAL::obj()->numISA(E_A_GEN,E_NODE_EISA);
                    //! номер адаптера
                    uint8_t numA = (ch->setting.numCh/isa->maxChAdapter);
                    uint8_t numCh= ch->setting.numCh - numA *isa->maxChAdapter;
                    if((numCh%2) > 0)
                    {
                        chGen = 1;
                        chGen<<=15;
                    }
                    //! описание данных
                    TDesGen_NU *desGenNU = (TDesGen_NU*)(ch->desData);                   
                    //! номер канала
                    uint16_t word = desGenNU->f | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 10,1,(uint8_t*)&(word),   sizeof(word));
                    word = desGenNU->ff | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 12,1,(uint8_t*)&(word),   sizeof(word));
                    word = desGenNU->f | chGen;
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 14,1,(uint8_t*)&(word),   sizeof(word));

                }else if(ch->typeCh == E_ITP)//не стандартная плата
                {
                    uint8_t offset = 0;
                    isa = HAL::obj()->numISA(E_A_ITP,E_NODE_EISA);
                    //! номер адаптера
                    uint8_t numA = (ch->setting.numCh/isa->maxChAdapter);
                    uint8_t numCh= ch->setting.numCh - numA *isa->maxChAdapter;
                    //! описание данных
                    TDesITP *desItp = (TDesITP*)(ch->desData);
                    if((numCh%2) > 0)
                        offset = 2;
                    //! номер канала
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + offset,1,(uint8_t*)&(desItp->value),   sizeof(desItp->value));
                }else if(ch->typeCh == E_CH_DAC)
                {                    
                    isa = HAL::obj()->numISA(E_A_DAC16,E_NODE_EISA);
                    //! номер адаптера
                    uint8_t numA = (ch->setting.numCh/isa->maxChAdapter);
                    uint8_t numCh= ch->setting.numCh - numA *isa->maxChAdapter;
                    //! описание данных
                    TDesDAC *desDAC = (TDesDAC*)(ch->desData);
                    
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 2,1,(uint8_t*)&(numCh),          sizeof(numCh));
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA],    1,(uint8_t*)&(desDAC->volt),   sizeof(desDAC->volt));                
                    
                }
                    
                if(ch->typeCh == E_IR || ch->typeCh == E_IP)
                {
                    //! номер адаптера
                    uint8_t numA = (ch->setting.numCh/isa->maxChAdapter);
                    uint8_t numCh= ch->setting.numCh - numA *isa->maxChAdapter;
                    //! описание данных
                    TDesIR *desIr = (TDesIR*)(ch->desData);
                    //! номер канала
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 4,1,(uint8_t*)&(numCh),   sizeof(numCh));
                    //! значение кода
                    HAL::obj()->eisa->sendRequest(isa->baseAddr[numA] + 6,1,(uint8_t*)&(desIr->value),sizeof(desIr->value));
            }
        }
    }
    }
}

    //! инициализация типа устройства
       int LayerAnalogVx::init(dev_info_t   *dac_info,     /*информация об устройстве*/
                int          &dac_devices, /*идентификатор устройства*/
                TAdapterISA *devices,     /*кол-во устройств*/
                uint8_t     **dac_buf,
                char dev_name[])     /*буфер*/
       {

           char buffer[20];
           if(devices == 0)
               return -1;
           sprintf(buffer, "%s", dev_name);
           if ( (dac_devices = open(buffer, O_RDONLY,0644)) <= 0)
           {
               //perror("LayerRK::init() in func: open() error = ");
               /* errno should be already set by system call */
               close(dac_devices);
               return -1;
           }
           for(int i=0;i<devices->numBaseAddr;i++)
           {
               if(ioctl(dac_devices,USER_IO_ADD_BASE_ADR,devices->baseAddr[i])== ERROR)
               {
                   perror("LayerRK::init() in func: open() error = ");
                   close(dac_devices);
                   return -1;
               }
           }

           //!
           dac_info->common.device_count = devices->numBaseAddr;
           dac_info->channel_count       = devices->numBaseAddr * DACWAV_CHANNELS_PER_BOARD;

   //        *sc_buf = (uint8_t *) calloc(sc_info->channel_count, sizeof(uint8_t));
   //        memset( (void *)*sc_buf, 0, sizeof(uint8_t) * sc_info->channel_count);
   //        if (*sc_buf == NULL)
   //        {
   //            //free(sc_devices);
   //            //close(fd);
   //            errno = ENOMEM;
   //            return -1;
   //        }
               /*cmd.startChannel = 0;
               cmd.channelCount = REL48_CHANNELS_PER_BOARD;
               cmd.asBitField = 0;
               cmd.buffer = sc_buffer + i * REL48_CHANNELS_PER_BOARD;
               retval = read(fd, (char*)&cmd, sizeof(cmd));*/


           return 0;
       }
#endif
