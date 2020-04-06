#include "layerRK.h"

#ifdef VXWORKS_PLATFORM
/*template<>*/ LayerRK* LayerRK::layer=0;
/*template<>*/ TypeSpecRK *LayerRK::spec=0;
#else
template<> LayerRK* LayerRK::layer   = 0;
template<> TypeSpecRK *LayerRK::spec = 0;
#endif

#ifdef VXWORKS_PLATFORM
LayerRKVx::LayerRKVx()
{       
        maxRksIn  = 0;
        maxRksOut = 0;

        rksIn    = 0;//new uint8_t[];
        rksOut   = 0;
        
        in_devices = -1;
        out_devices= -1;

        sc_in = (struct sc_info_t *) malloc(sizeof(struct sc_info_t));
        memset((void*)sc_in, 0,sizeof(struct sc_info_t));
        sc_in->common.devname = "RSREL48";
        sc_in->common.venname = "UNKNOWN";
        

        sc_out = (struct sc_info_t *) malloc(sizeof(struct sc_info_t));
        memset((void*)sc_out, 0,sizeof(struct sc_info_t));
        sc_out->common.devname = "TRREL48";
        sc_out->common.venname = "UNKNOWN";
        
        init();
    }

    //! общая инициализация устройств
    void LayerRKVx::init()
    {
        int stTr, stRs;
        stTr = init(sc_out,out_devices,HAL::obj()->numISA(E_A_TRREL48), &rksOut,TRREL48_DEVPATH);
        stRs = init(sc_in, in_devices, HAL::obj()->numISA(E_A_RSREL48), &rksIn ,RSREL48_DEVPATH);
        //! максимальное кол-во  разовых команд
        maxRksOut = sc_out->channel_count;
        maxRksIn  = sc_in->channel_count;
        //! подчитываем команды в расширителе
        if(HAL::obj()->eisaAvailable == true)
        {
           maxRksOut += HAL::obj()->numISA(E_A_TRREL48, E_NODE_EISA)->numBaseAddr * REL48_CHANNELS_PER_BOARD;   
           maxRksIn  += HAL::obj()->numISA(E_A_RSREL48, E_NODE_EISA)->numBaseAddr * REL48_CHANNELS_PER_BOARD;
        }
        rksOut= new uint8_t[maxRksOut];        
        rksIn = new uint8_t[maxRksIn];
        
        memset((void*)rksOut,0,maxRksOut * sizeof(uint8_t));
        memset((void*)rksIn, 0,maxRksIn  * sizeof(uint8_t));
        
        if(stTr!=-1)
            std::cout<<std::setw(20)<<std::left<<"HAL: Detected  TR RK ... "<<maxRksOut<<" Channels " <<" [READY]"<<std::endl;
        if(stRs!=-1)
            std::cout<<std::setw(20)<<std::left<<"HAL: Detected  RS RK ... "<<maxRksIn<<" Channels "<<" [READY]"<<std::endl;
    }
    //! инициализация типа устройства
    int LayerRKVx::init(sc_info_t   *sc_info,     /*информация об устройстве*/
             int          &sc_devices, /*идентификатор устройства*/
             TAdapterISA *devices,     /*кол-во устройств*/
             uint8_t     **sc_buf,
             char dev_name[])     /*буфер*/
    {
      
       
        int i = 0, retval;
        char buffer[20];
        if(devices == 0)
            return -1;
        sprintf(buffer, "%s", dev_name);
        if ( (sc_devices = open(buffer, O_RDONLY,0644)) <= 0)
        {
            //perror("LayerRK::init() in func: open() error = ");
            /* errno should be already set by system call */
            close(sc_devices);
            return ERROR;
        }
        for(int i=0;i<devices->numBaseAddr;i++)
        {
            if(ioctl(sc_devices,REL48_IO_ADD_BASE_ADR,devices->baseAddr[i])== ERROR)
            {
                perror("LayerRK::init() in func: open() error = ");
                close(sc_devices);
                return ERROR;
            }
        }
        
        //!
        sc_info->common.device_count = devices->numBaseAddr;
        sc_info->channel_count       = devices->numBaseAddr * REL48_CHANNELS_PER_BOARD;
              
        /**sc_buf = (uint8_t *) calloc(sc_info->channel_count, sizeof(uint8_t));
        memset( (void *)*sc_buf, 0, sizeof(uint8_t) * sc_info->channel_count);*/
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
        

    }
    int LayerRKVx::sc_in_read()
    {       
        ssize_t retval;
       
        if(in_devices == ERROR)
            return ERROR;
        
        cmdRs.startChannel = 0;
        cmdRs.channelCount = sc_in->channel_count;
           
        if ( (retval = read(in_devices,
                            (char*)&cmdRs,
                             sizeof(struct rsrel48ReadCmd)))!= sc_in->channel_count) 
        {
             /* errno should be set by read call */
            perror("LayerRK::sc_in_read() error = ");
            return ERROR;
        }
        //! копируем данные из драйвера
        memcpy(rksIn,cmdRs.values,
               sizeof(uint8_t) * sc_in->channel_count);
         
    }
    //! выдача всех разовых команд которые накопили в буфере
    int LayerRKVx::sc_out_write()
    {
        int i = 0;
        ssize_t retval;
       
        if(out_devices == ERROR)
            return -1;
        //! начальный канал
        cmdTr.startChannel = 0;
        cmdTr.channelCount = sc_out->channel_count;
        //! копируем данные
        memcpy(cmdTr.values, rksOut,
               sizeof(uint8_t) * sc_out->channel_count);

            if( (retval = write(out_devices,
                                 (char*)&cmdTr,
                                 sizeof(struct trrel48WriteCmd))) != sc_out->channel_count)
            {
                perror("LayerRK::sc_out_write() error = ");
                /* errno should be set by read call */
                return ERROR;
            }     
            return OK;
    }
    //! принудительная запись в устройство
    void LayerRKVx::writeToDevice()
    {
       bitsRead = 0;
        
        sc_out_write();
        //! выдаем сразу все каналы РК в EISA
        if(HAL::obj()->eisaAvailable == true)
        {
            TAdapterISA *isa = HAL::obj()->numISA(E_A_TRREL48,E_NODE_EISA);            
            rks = &rksOut[sc_out->channel_count];
            
            for(int i=0; i<isa->numBaseAddr;i++)
            {           
                for(int j=0;j<REL48_REGS;j++)
                {
                    bit  = 0;
                    word = 0;
                    do{
                        word |= rks[bitsRead++]<<bit;
                        bit++;
                    }while(REL48_WORD!=bit);
                    reg[j] = word;
                }                
                HAL::obj()->eisa->sendRequest(isa->baseAddr[i],1,(uint8_t*)&reg,REL48_REGS*sizeof(uint16_t));
                
            }
        }
    }
    //! обновление устройства
    void LayerRKVx::updateState()
    {
        //sc_in_read();
        //sc_out_write();
    }
    //! принудительное чтение из устройства
    void LayerRKVx::readFromDevice()
    {
        bitsRead = 0;
        sc_in_read();
        //! читаем сразу все каналы РК в EISA
        if(HAL::obj()->eisaAvailable == true)
        {
            TAdapterISA *isa = HAL::obj()->numISA(E_A_RSREL48,E_NODE_EISA);
            rks = &rksIn[sc_in->channel_count];

            for(int i=0; i<isa->numBaseAddr;i++)
            {
                for(int j=0;j<REL48_REGS;j++)
                {
                    bit  = 0;
                    word = 0;
                    do{
                        word |= rks[bitsRead++]<<bit;
                        bit++;
                    }while(REL48_WORD!=bit);
                    reg[j] = word;
                }
                HAL::obj()->eisa->sendRequest(isa->baseAddr[i],1,(uint8_t*)&reg,REL48_REGS*sizeof(uint16_t));

            }
        }
    }
    //! освобождение ресурсов
    void LayerRKVx::clear(sc_info_t* sc,
               int *devices,
               uint8_t *buf)
    {
//        int i=0;
//        while (i < sc->common.device_count)
//        {
//            close(*(devices + i));
//            i++;
//        }
//        //! освобождение ресурсов
//        delete buf;
//        delete devices;
//        delete sc;
    }
  #endif
