#include "servKPR.h"
#include "IMsg.h"
#include "globalNameID.h"

#define UPD_RECIVE_UCVS 5081

ServKPR::ServKPR(uint32_t idClass):ICalculateElement(idClass)
{
    scanIn = new TScanIn;
    //! сформируем команду на сканирование
    scanIn->Com.Com = Com_Scan;
    scanIn->AnsPort = UPD_RECIVE_UCVS;

    //! находим модуль  для отправки пакетов
    bindTo(new TEventAddrStat());
    setFreq(ICalculateElement::Hz12_5);
    setStart();
}
void ServKPR::calculate()
{

}
void ServKPR::scanKPR()
{
    //! регистрация буффера для отправки
    TEventAddrStat eventSend(ID_CONNECT_TO_NET,
                             (uintptr_t)(&scanIn),
                             TEventAddrStat::SET_POINTER,
                             this,
                             TEventAddrStat::E_UP);
    eventSend.sizeAddr = sizeof(scanIn);

    MsgRegNet msgRegNetSend(eventSend); //! сообщение
    msgRegNetSend.port   = UDP_SERV_PROC_PORT;
    msgRegNetSend.ip     = "255.255.255.255";
    msgRegNetSend.rs     = true;

    ICalculateElement::eEvent(&msgRegNetSend);

}
void ServKPR::init()
{
    //! поиск устройств КПР
    scanKPR();
    ICalculateElement::init();
}
void ServKPR::finite()
{
    //ICalculateElement::finite();
}
bool ServKPR::bind()
{
    return ICalculateElement::bind();
}
