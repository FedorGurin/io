#ifndef SERV_KPR_H
#define SERV_KPR_H

#include "servProtKPR.h"
#include "ICalculateElement.h"

typedef struct TOutputModel_
{

}TOutputModel;

//! обслуживанеие и работа с КПР
class ServKPR:public ICalculateElement
{
public:
    ServKPR(uint32_t name);

    //! обязательный интерфейс
    virtual bool bind();
    virtual void init();
    virtual void calculate();
    virtual void finite();

    //! сканирование КПР
    void scanKPR();
    //! запрос на сканирование
    TScanIn*  scanIn;
    TScanOut* scanOut;
    TCom com;

private:
    TOutputModel output;
};

#endif
