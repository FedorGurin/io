#ifndef PETHISA_H
#define PETHISA_H


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdlib.h>
#include <string>

#include "platform.h"
typedef uint16_t quint16;
typedef uint8_t quint8;
typedef uint32_t quint32;

#define MAX_EISA_BUF 1500

#ifdef VXWORKS_PLATFORM
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
namespace EthIsa {
    struct CommFind {
        quint16 flags;
        quint16 comm;
        quint16 port;
    };
    struct Header {
        quint16 flags;
        quint8  len;
        quint8  comm;
    };
    struct FindAnswer {
        Header  header;
        quint32 ipAddr;
        quint16 port;
        quint32 type;
        quint32 id;
        char    sn[12];
        quint32 ver;
        uint8_t   reserv[16];
    };
    struct CommAnswer {
        Header  header;
        quint32 addr;
        quint8  data[MAX_EISA_BUF];
    };
    struct Command {
        Header  header;
        quint16 port;
        quint32 addr;
        quint8  data[MAX_EISA_BUF];
    };
    enum CommandCodes {
        Find = 0, //команда поиска E-ISA
        IORD = 5, //команда для записи в IO ISA
        IOWR = 6, //команда для чтения из IO ISA
        SerWR = 8 //сервисная команда для прошивки адаптеров
    };
}
#ifdef VXWORKS_PLATFORM
#pragma pack(0)
#else
#pragma pack(pop)
#endif
//! модуль для работы с расширителем PEthIsa
class PEthIsa
{
public:
    PEthIsa();
    ~PEthIsa();
    //! загрузка прошивок
    void loadCard(std::string name, quint32 init, quint32 load);
    //! проверка исправности карты
    bool checkCard(uint16_t baseAddr,uint8_t offset, uint16_t mask,uint16_t code);
    //! чтение данных по адресу
    quint16 readW(quint32 addr);

    void writeW(quint32 addr, quint16 data);

    //! функции для работы с адаптерам через EISA
    void readFrom(quint32 addr, char *data, quint8 count, bool increment = true);
    void writeTo(quint32 addr, const char *data, quint8 count, bool increment = true);
    //! подключение к Eisa
    bool connectToEisa();
    //! связывание с портом
    bool bindTo();
    //! установить настройки
    void setSetting();
    //! загрузка прошивок
    void flashFirmware();

private:
    //! дескрипторы сокетов
    int socketSend;
    int socketRec;
    //! структура которую прислал отправитель
    struct sockaddr_in sockAddrRec;
#ifndef VXWORKS_PLATFORM
    socklen_t addrlen;
#else
    int addrlen;
#endif

    //! флаг "Отключение инкремента"
    uint16_t noInc;
    //! порт данного выичслителя/узла
    uint16_t localPort;
    //! порт приема на плате eisa
    uint16_t eisaPort;
    uint32_t eisaId;
    uint32_t eisaType;
    uint32_t eisaVer;

    //! ip-адрес расширителя ISA
//    char* eisaIp;
//    char* eisaSn;

    std::string eisaIp;
    std::string eisaSn;

    EthIsa::Command     command;
    EthIsa::CommAnswer  commandAns;
    EthIsa::FindAnswer  findAns;
    EthIsa::Header      header;

    //! признак подключения
    bool mconnected;
    //! модуль не отвечает
    void ethIsaLost();
};

#endif // PETHISA_H
