#include "pethisa.h"
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "string.h"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hal.h"
#include <unistd.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>



#ifdef VXWORKS_PLATFORM
#include <sockLib.h>
#include <net/utils/ifconfig.h> 
#include <ipProto.h>
#include <ipnet/ipioctl.h>
#endif

//! время ожидания от платы E-ISA, мсек
#define TIME_OUT_ETHISA_MSEC     100
//! локальный порт приема данного узла
#define LOCAL_PORT               5677
//! порт E-ISA
#define BROADCAST_PORT_EISA      99

#define IP_EISA "192.168.1.100"
#define PORT_EISA 30000
#define FOLDER_FIRMWARE "/ata00:2/drv/"
bool PEthIsa::bindTo()
{
    int result=0;

    sockAddrRec.sin_family      = AF_INET;
    sockAddrRec.sin_port        = htons(localPort);
    sockAddrRec.sin_addr.s_addr = INADDR_ANY;

    result = bind(socketRec,(struct sockaddr*)&sockAddrRec,sizeof(sockAddrRec));//блокируемый сокет

    if (result == 0)        
        return true;
    else
    {
        perror("E-ISA: func = bindTo(), bind");
        return false;
    }
}

PEthIsa::PEthIsa()
{
    noInc       = (uint16_t)((uint16_t)(1)<<14);
    socketSend  = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(socketSend <0)
    {
        perror("E-ISA: func = PEthIsa() socketSend, socket():");
    }
    socketRec   = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(socketRec <0)
    {
        perror("E-ISA: func = PEthIsa() socketRec, socket():");
    }    
    localPort   = LOCAL_PORT;
    mconnected  = false;
    addrlen     = 0;
}

PEthIsa::~PEthIsa()
{
    close(socketRec);
    close(socketSend);
    //mudp->abort();
    //delete mudp;
}

//! загрузка прошивок
void PEthIsa::loadCard(std::string name, quint32 init, quint32 load)
{
    if (mconnected)
    {
        int fd;
        int bytesRead = 0;
        int size = 0;
        char buffer[255];
#ifdef VXWORKS_PLATFORM
        std::string nameFile = FOLDER_FIRMWARE + name;
#else
        std::string nameFile = std::string(".") + FOLDER_FIRMWARE + name;
#endif

        //! запись прошивок
        fd = open(nameFile.c_str(), O_RDONLY,0);
        if (fd < 0)
        {
            std::cout<<"E-ISA: Can`t open file = "<<nameFile<<std::endl;
            return;
        }

        lseek(fd,0,SEEK_SET);
        //! запись 0 для прошивок
        writeW(init,0);
        do {
            lseek(fd,size,SEEK_SET);
            bytesRead = read(fd,(char*)buffer, 255);
            size +=bytesRead;

            if(bytesRead <0)
            {
                std::cout<<"E_ISA: Can`t read file\n"<<std::endl;
                close(fd);
            }

            command.header.flags    = 0;
            command.header.comm     = EthIsa::SerWR;
            command.header.len      = bytesRead;
            command.port = htons(localPort);
            command.addr = htonl(load);
            memcpy(command.data,buffer,bytesRead);
            //! отправляем команду
            int32_t bytes = sendto(socketSend,
                                   (caddr_t)&command,
                                   (sizeof(EthIsa::Command)-MAX_EISA_BUF)+bytesRead,
                                   0,
                                   (struct sockaddr*)&sockAddrRec,
                                   sizeof(sockAddrRec));
            if(bytes <0)
            {
                perror("E-ISA: func = loadCard(), sendto:");
                ethIsaLost();
                close(fd);
                return;
            }

            bytes = recvfrom(socketRec,
                             NULL,//(char*)&commandAns,
                             0,//sizeof(commandAns),
                             0 ,
                             (struct sockaddr *)&sockAddrRec,
                             &addrlen);

            if(bytes <0)
            {
                perror("E-ISA: func = loadCard(), recvfrom:");
                ethIsaLost();
                close(fd);
                return;
            }
        } while (bytesRead!=0);

        //! убрать дискрипторов
        close(fd);
    }
}

quint16 PEthIsa::readW(quint32 addr)
{
    quint16 data;
    readFrom(addr,(char*)&data,2,false);
    return data;
}

void PEthIsa::writeW(quint32 addr, quint16 data)
{
    writeTo(addr,(char*)&data,2,false);
}

void PEthIsa::readFrom(quint32 addr, char *data, quint8 count, bool increment)
{
    //addrlen = 0;
    memset((caddr_t)&commandAns,0,sizeof(commandAns));
    //! подготовка команды
    command.header.flags    = increment?0:htons(noInc);
    command.header.comm     = EthIsa::IORD;
    command.header.len      = count;
    command.port = htons(localPort);
    command.addr = htonl(addr);
    //! отправляем команду
    int32_t bytes = sendto(socketSend,
                           (caddr_t)&command,
                           (sizeof(EthIsa::Command)-MAX_EISA_BUF)+count,
                           0,
                           (struct sockaddr*)&sockAddrRec,
                           sizeof(sockAddrRec));
    if(bytes<0)
    {
        perror("E-ISA: func = readFrom(), sendto");
        return;
    }
    //! ожидаем прихода команды
    bytes = recvfrom(socketRec,
                     (caddr_t)&commandAns,
                     sizeof(EthIsa::CommAnswer),
                     0 ,
                     (struct sockaddr *)&sockAddrRec,
                     &addrlen);

    if(bytes<0)
    {
        perror("E-ISA: func = readFrom(), recvfrom");
        ethIsaLost();
        return;
    }

    //! копируем данные
    if (data)
        memcpy(data,commandAns.data,count);
}

void PEthIsa::writeTo(quint32 addr,
                    const char *data,
                    quint8 count,
                    bool increment)
{
    //addrlen = 0;
    command.header.flags    = increment?0:htons(noInc);
    command.header.comm     = EthIsa::IOWR;
    command.header.len      = count;

    command.port            = htons(localPort);
    command.addr            = htonl(addr);
    memcpy(command.data,data,count);
    int32_t bytes = sendto(socketSend,
                           (caddr_t)&command,
                           (sizeof(EthIsa::Command) - MAX_EISA_BUF)+count,
                           0,
                           (struct sockaddr*)&sockAddrRec,
                           sizeof(sockAddrRec));
    if(bytes<0)
    {
        perror("E-ISA: func = writeTo(), sendto");
        return;
    }
    bytes = recvfrom(socketRec,
                     (char*)&commandAns,
                     sizeof(EthIsa::CommAnswer),
                     0 ,
                     (struct sockaddr *)&sockAddrRec,
                     &addrlen);
    if(bytes<0)
    {
        perror("E-ISA: func = writeTo(), recvfrom");
        //! потеря соединения
        ethIsaLost();
        return;
    }
    //std::cout<<"E_ISA: func=writeTo, recvfrom is OK"<<std::endl;
}
void PEthIsa::setSetting()
{
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = TIME_OUT_ETHISA_MSEC*1000;//1000msec
    int res = setsockopt(socketRec, SOL_SOCKET, SO_RCVTIMEO,(char*)&tv,sizeof(tv));
    if(res < 0)
        perror("E-ISA: func = setSetting(), setsockopt");
}
bool PEthIsa::connectToEisa()
{
    struct sockaddr_in sockAddr;
    int32_t bytes = 0;

    memset((void*)&sockAddr, 0,sizeof(sockAddr));
    memset((void*)&findAns,  0,sizeof(findAns));
    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_port        = htons(BROADCAST_PORT_EISA);
    sockAddr.sin_addr.s_addr = INADDR_BROADCAST;

    int  broadcast = 1;
    if(setsockopt(socketSend, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(int)) < 0)
    {
        perror("E-ISA: func = connectToEisa(), setsockopt(trying set broadcast option)");
    }
#ifdef VXWORKS_PLATFORM
    char netif[] = "eth0";
#else
    char netif[] = "enp0s31f6";
#endif
    struct ifreq ifr;
    memset(&ifr,0,sizeof(ifr));
    snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),"gei1");
#ifdef VXWORKS_PLATFORM
    ioctl(socketSend,SIOCGIFINDEX,&ifr);
#endif
#ifdef QNX_PLATFORM
    int ifIndex = if_nametoindex(ifr.ifr_name);
    //ifr.ifr_ifindex = ifIndex;
#endif
    if(setsockopt(socketSend, SOL_SOCKET, SO_BINDTODEVICE, (char*)&ifr, sizeof(ifr)) < 0)
    {
        //perror("E-ISA: func = connectToEisa(), setsockopt(trying set select net interface)");
    }
    if (bindTo() == false)
    {
        std::cout<<"E-ISA: func = connectToEisa(), Can`t bind port"<<std::endl;
        
        //return false; 
    }
    //! подготовка команды для поиска платы расширения E-ISA
    EthIsa::CommFind com;
    com.flags = 0;
    com.comm  = EthIsa::Find;
    com.port  = htons(localPort);

    //std::cout<<"sizeof(EthIsa::CommFind) = "<<sizeof(EthIsa::CommFind)<<std::endl;
    bytes = sendto(socketSend,
                   (caddr_t)&com,
                   sizeof(EthIsa::CommFind),
                   0,
                   (struct sockaddr*)&sockAddr,
                   sizeof(sockAddr));
    if(bytes<0)
    {
        perror("E-ISA: func = connectToEisa(), sendto");
        return false;
    }

    //! время ожидания ответа
    struct timeval tv;
    tv.tv_sec = 1;//1000msec
    tv.tv_usec = 0;
    if(setsockopt(socketRec, SOL_SOCKET, SO_RCVTIMEO,(char*)&tv,sizeof(tv))<0)
    {
        perror("E-ISA: func = connectToEisa(), setsockopt");
        return false;
    }
    //std::cout<<"sizeof(EthIsa::FindAnswer) = "<<sizeof(EthIsa::FindAnswer)<<std::endl;
    addrlen = sizeof(sockAddrRec);
    bytes = recvfrom(socketRec,
                     (char *)&findAns,
                     sizeof(EthIsa::FindAnswer),
                     MSG_WAITALL ,
                     (struct sockaddr *)&sockAddrRec,
                     &addrlen);

    if(bytes<0)
    {
        std::cout<<"\tE-ISA: E-ISA not found"<<std::endl;
        return false;
    }
    //! нужно сохранить порт и ip передатчика
    eisaPort = ntohs(sockAddrRec.sin_port);
    eisaIp   = std::string(inet_ntoa(sockAddrRec.sin_addr));
    eisaId   = ntohl(findAns.id);
    eisaType = ntohl(findAns.type);
    eisaVer  = ntohl(findAns.ver);
    for(int i=0;i<12;i++)
        eisaSn   += findAns.sn[i];

    if (findAns.header.comm == 0)
    {
        std::cout<<std::endl<<std::endl;
        std::cout<<"========================================"<<std::endl;
        std::cout<<"\tE-ISA: Connected to E-ISA("<<eisaIp<<")"<<std::endl;
        std::cout<<"========================================"<<std::endl;
        std::cout<<"\tE-ISA: IP   = "<<eisaIp<<std::endl;
        std::cout<<"\tE-ISA: Port = "<<eisaPort<<std::endl;
        std::cout<<"\tE-ISA: SN   = "<<eisaSn<<std::endl;
        std::cout<<"\tE-ISA: ID   = "<<eisaId<<std::endl;
        std::cout<<"\tE-ISA: Type = "<<eisaType<<std::endl;
        std::cout<<"\tE-ISA: Ver  = "<<std::hex<<eisaVer<<std::endl;
        std::cout<<"========================================"<<std::endl;

        mconnected = true;
        setSetting();
        //! загрузка прошивок
        flashFirmware();

    }else
        std::cout<<"\tE-ISA: E-ISA not found"<<std::endl;

    std::cout<<std::dec;
    return true;
}
bool PEthIsa::checkCard(uint16_t baseAddr,uint8_t offset, uint16_t mask,uint16_t code)
{
    uint16_t curCode=0x0;
    readFrom(baseAddr+offset,(char*)&curCode,2,false);

    if((curCode&mask) == code)
        return true;

    return false;
}
void PEthIsa::flashFirmware()
{
    int numAdapters = HAL::obj()->confISA.numAdapters;
    for(int i=0;i<numAdapters;i++)
    {
        TAdapterISA *isa = &(HAL::obj()->confISA.isa[i]);
        if(isa->typeNode == E_NODE_EISA)
        {
            if(isa->idNode == 0)
            {
                switch (isa->typeAdapter)
                {
                case E_A_GEN:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Burning GEN adapters"<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        sleep(2);
                        loadCard("gen.bit",isa->baseAddr[j],isa->baseAddr[j] + 2);
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: GEN "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: GEN "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_IP:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Burning IP adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;


                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        sleep(2);
                        loadCard("ip.bit",isa->baseAddr[j],isa->baseAddr[j] + 2);
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: IP "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: IP "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_IR:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Burning IR adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        sleep(2);
                        loadCard("ir.bit",isa->baseAddr[j],isa->baseAddr[j] + 2);
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: IR "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: IR "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_TRREL48:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Checking TRREL48 adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: TTREL48 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: TRREL48 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_RSREL48:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Checking RSREL48 adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: RSREL48 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: RSREL48 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_DAC16:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Checking DAC16 adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: DAC16 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: DAC16 "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                case E_A_ITP:
                {
                    std::cout<<"========================================"<<std::endl;
                    std::cout<<"E-ISA: Checking ITP adapters "<<std::endl;
                    std::cout<<"========================================"<<std::endl;

                    for(int j = 0;j < isa->numBaseAddr;j++)
                    {
                        if(checkCard(isa->baseAddr[j],isa->offsetCheck,isa->maskCheck,isa->codeCheck))
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: ITP "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is ready"<<std::endl;
                        else
                            std::cout<<std::setw(20)<<std::left<<"E-ISA: ITP "<<"(base = "<<std::hex<<isa->baseAddr[j]<<")"<<" is fail"<<std::endl;
                    }
                    break;
                }
                default:
                    break;
                };
            }

        }
    }
}
void PEthIsa::ethIsaLost()
{
    std::cout<<"E-ISA: E-ISA not respond"<<std::endl;
    mconnected = false;
}

void testEISA()
{
/*#ifdef VXWORKS_PLATFORM
    int status = ipAttach(1,"gei");
    if(status == ERROR)
        std::cout<<"E-ISA: ipAttach() Can`t create eth interface"<<std::endl;
    status = ifconfig("gei1 inet add 192.168.1.111 netmask 255.255.255.0");
    if(status == ERROR)
        std::cout<<"E-ISA: ifconfig() Fail setting network interface"<<std::endl;
    status = ifconfig("gei1 up");
    if(status == ERROR)
       std::cout<<"E-ISA: Faild up interface"<<std::endl;
   

   sleep(15);
#endif*/
    
    PEthIsa eisa;
//    bool flag = false;
//    do
//    {
    int status;
       status = eisa.connectToEisa();
   // }while(status == false);
     while(1)
    {

     sleep(1);
     uint32_t value1 = 0x10012001;
     uint32_t value0 = 0x00000000;
     uint16_t valUInt16 =0;

     eisa.writeTo(0x120,(char*)&value1,4,true);
     sleep(1);
     eisa.readFrom(0x120,(char*)&value1, 2,false);
     std::cout<<" read TRR="<<std::hex<<value1<<std::endl;
     eisa.readFrom(0x122,(char*)&value1, 2,false);
     std::cout<<" read TRR="<<std::hex<<value1<<std::endl;

     sleep(1);
     eisa.writeTo(0x120,(char*)&value0, 4,false);
     sleep(1);
     eisa.readFrom(0x120,(char*)&value0, 4,false);
     std::cout<<" read TRR="<<std::hex<<value0<<std::endl;

     sleep(1);
     eisa.readFrom(0x160 + 4,(char*)&value0, 2,false);
     std::cout<<" test GEN1="<<std::hex<<value0<<std::endl;
     sleep(1);
     eisa.readFrom(0x1d0 + 4,(char*)&value0, 2,false);
     std::cout<<" test GEN0="<<std::hex<<value0<<std::endl;
    }
  
}
