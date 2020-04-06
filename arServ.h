#ifndef ARSERV_H
#define ARSERV_H

#include "ICalculateElement.h"


//! класс аэродинамики
class ArServ : public ICalculateElement
{
public:
    ArServ(uint32_t idClass, uint32_t idCh_, float step_);

    //! обобщенный интерфейс
    virtual void init();
    virtual void calculate();
    virtual void finite(){}
private:

    uint32_t idCh;
};

#endif
