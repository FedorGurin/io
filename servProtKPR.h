// Сервисный протокол

#ifndef SERVPROC_H
#define SERVPROC_H

#define TrueStudio
#define FResp 0xFE6E  // флаг ответа на команду
#define FCom  0xFC6E  // флаг команды

#define UDP_SERV_PROC_PORT 2080

#ifdef TrueStudio
#include "platform.h"
#define quint32 uint32_t
#define quint16 uint16_t
#endif

/**
 * Сервисные команды
 */
enum EServCommands
{
    Com_Scan = 0, Com_SetKPRState = 1, Com_SetKPRFrec = 2, Com_SetKPROut = 3, Com_GetKPROut = 4, Com_SetModBusEq = 5, Com_GetModBusEq = 6
};

/**
 * Режим: Ожидание, Работа, Настройка, Программирование
 */
enum EKPRState
{
    PS_Wait = 0, PS_Work = 1, PS_Config = 2, PS_Prog = 3
};

enum EKPRType
{
    KPR_NC = 0, KPR_AC = 1, KPR_PK = 2, KPR_DPK = 3
};

enum EMode
{
    Mode_Auto = 0, Mode_Manual = 1
};

#ifdef VXWORKS_PLATFORM
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct TCom
{
    quint32 Header : 16;
    quint32 Com : 8;
    quint32 Flags : 8;
};

struct TKPRConfig
{
    quint32 IP_Range_Start; // минимальный IP адрес устройства
    quint32 IP_Range_End; // максимальный IP адрес устройства
    quint32 UDPPort; // UDP порт для приема команд
    quint32 NetMask; // маска сети
    struct
    {
        quint32 MAC0 : 8;
        quint32 MAC1 : 8;
        quint32 MAC2 : 8;
        quint32 MAC3 : 8;
        quint32 MAC4 : 8;
        quint32 MAC5 : 8;
        quint32 res1 : 8;
        quint32 res2 : 8;
    } MAC; // MAC адрес
    char SN[12]; // серийный номер устройства
    quint32 KPRType; // Тип КПР (AC, PK, DPK ...) EKPRType
    quint32 Flags;
};

struct TScanIn //Запрос информации об устройстве
{
    struct TCom Com;
    quint32 AnsPort;
};

struct TScanOut //Ответ на запрос состояния
{
    struct TCom Com;
    quint32 Frec; // частота опроса в герцах
    struct TKPRConfig KPRConfig;
    struct
    {
    	quint32 Day : 8;
        quint32 Month : 4;
        quint32 Year : 12;
        quint32 Ver : 8;
    } VerNum; // номер версии ПО
    quint32 KPRType; // Тип КПР (AC, PK, DPK ...) EKPRType
    quint32 Errors; // флаги аппаратных ошибок 0 - нет ошибок
    quint32 State; // режим КПР 0 - Ожидание, 1 - Работа, 2 - Настройка, 3 - Программирование EKPRState
    quint32 ID_H;
    quint32 ID_M;
    quint32 ID_L;
    quint32 Reserv1; // резерв
    quint32 Reserv2; // резерв
};

//local---------------------------------------------------------------------------
struct TScanOutwIP //Ответ на запрос состояния в моем списке
{
    struct TScanOut ScanOut;
    quint32 IP: 32;
};

struct TSetKPRFrecIn //Запрос изменения частоты обновления
{
    struct TCom Com;
    quint32 AnsPort;
    quint32 Frec; // частота опроса в герцах
};

struct TSetKPRFrecOut //Ответ на запрос изменения частоты обновления
{
    struct TCom Com;
};

struct TSetKPRStateIn //Запрос смены состояния КПР
{
    struct TCom Com;
    quint32 AnsPort;
    quint32 NewState; // состояние пульта 0 - Ожидание, 1 - Работа, 2 - Настройка, 3 - Программирование EKPRState
    quint32 Key; // код разрешения смены состояния. При смене состояния Ожидание <-> Работа ключ игнорируется.
};

struct TSetKPRStateOut //Ответ на запрос смены состояния пульта
{
    struct TCom Com;
};


struct TKPROut
{
    struct
    {
        quint32 URP1 : 1; // 0 - KBO; 1 - INLINE
        quint32 URP2 : 1;
        quint32 URP3 : 1;
        quint32 URP4 : 1;
        quint32 URP5 : 1;
        quint32 URP6 : 1;
        quint32 URP7 : 1;
        quint32 URP8 : 1;
        quint32 URP9 : 1;
        quint32 URP10 : 1;
        quint32 URP11 : 1;
        quint32 URP12 : 1;
        quint32 URP13 : 1;
        quint32 URP14 : 1;
        quint32 URP15 : 1;
        quint32 URP16 : 1;
        quint32 URP17 : 1;
        quint32 URP18 : 1;
        quint32 URP19 : 1;
        quint32 URP20 : 1;
        quint32 URP21 : 1;
        quint32 URP22 : 1;
        quint32 URP23 : 1;
        quint32 URP24 : 1;
        quint32 URP25 : 1;
        quint32 URP26 : 1;
        quint32 URP27 : 1;
        quint32 URP28 : 1;
        quint32 URP29 : 1;
        quint32 URP30 : 1;
        quint32 URP31 : 1;
        quint32 URP32 : 1;
    } w1;
    struct
    {
        quint32 URP33 : 1; // 0 - KBO; 1 - INLINE
        quint32 URP34 : 1;
        quint32 URP35 : 1;
        quint32 URP36 : 1;
        quint32 URP37 : 1;
        quint32 URP38 : 1;
        quint32 URP39 : 1;
        quint32 URP40 : 1;
        quint32 URP41 : 1;
        quint32 URP42 : 1;
        quint32 URP43 : 1;
        quint32 URP44 : 1;
        quint32 URP45 : 1;
        quint32 URP46 : 1;
        quint32 URP47 : 1;
        quint32 URP48 : 1;
        quint32 AR : 1; // Режим работы КПР. 0 - Авто, 1 - Ручной. Поле только на чтение.
        quint32 Reserv : 15;
    } w2;
};

struct TSetKPROutIn //Запрос смены состояния выходов КПР
{
    struct TCom Com;
    quint32 AnsPort;
    struct TKPROut KPROut;
};

struct TSetKPROutOut //Ответ на запрос смены состояния выходов КПР
{
    struct TCom Com;
    struct TKPROut KPROut;
};

struct TGetKPROutIn //Запрос состояния выходов КПР
{
    struct TCom Com;
    quint32 AnsPort;
};

struct TGetKPROutOut //Ответ на запрос состояния выходов КПР
{
    struct TCom Com;
    struct TKPROut KPROut;
};

//ModBus
//Для управления состоянием приборов подключенных по протоколу modbus через управляющую программу
// Тут In - к программе, Out - ответ программы
struct TSetModBusEqIn //Смена состояния выходов приборов на modbus
{
    struct TCom Com;
    quint32 AnsPort;
    int MbAddr;
    quint16 MobBusState[16];
};

struct TSetModBusEqOut //Ответ на смену состояния выходов приборов на modbus
{
    struct TCom Com;
};

struct TGetModBusEqIn //Запрос состояния выходов приборов на modbus
{
    struct TCom Com;
    quint32 AnsPort;
};

struct TGetModBusEqOut //Ответ на запрос состояния выходов приборов на modbus
{
    struct TCom Com;
    quint16 MobBusState[9][16];
};
#ifdef VXWORKS_PLATFORM
#pragma pack(0)
#else
#pragma pack(pop)
#endif

#endif
