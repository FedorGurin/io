#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "ICalculateElement.h"

//! Управление классами создания отчета/журналирования в зависимости от платформы загружает нужную библиотеку
class ReportManager:public ICalculateElement
{
public:
    ReportManager(uint32_t idClass);

    virtual bool bind();
    virtual void init();
    virtual void calculate();
    virtual void finite();
    virtual void eEvent(TEventModel *event);
private:
    Inst_       instDll;
    Init_       initDll;
    Event_      eventDll;
    Bind_       bindDll;
    Calculate_  calcDll;

};

#endif // REPORTMANAGER_H
