#include "arServ.h"
#include "IEvent.h"
#include "hal.h"
#include "globalNameID.h"
#include <string.h>
#include "layerAr.h"
//! модуль обслуживания спец. циклограмм выдачи по Arinc
ArServ::ArServ(uint32_t idClass,uint32_t idCh_, float step_):ICalculateElement(idClass,step_)
{
    idCh = idCh_;
    setFreq(ICalculateElement::HzInf);
    setStart();
}
void ArServ::init()
{
    ICalculateElement::init();   
}

void ArServ::calculate()
{
    TDesAr *ar = 0;
    TCh *ch = &(HAL::obj()->chTable.ch[idCh]);
    uint8_t bufAddr[64];
    uint8_t numAddr=0;
    int index = 0;
    if (ch->typeCh == E_CH_AR)
    {
        ar = (TDesAr*) (ch->desData);
        if(ar->numArray<=ar->curIndArray)
            ar->curIndArray = 0;
        else
            ar->curIndArray++;

        numAddr = ar->numIndAddr[ar->curIndArray];
        for(int i = 0;i<ar->numIndAddr[ar->curIndArray];i++)
        {
            bufAddr[i] = ar->addr[ar->indAddr[i][ar->curIndArray]];
        }
         //! инициализируем канал
        LayerArinc::initCh(ch->setting.numCh, bufAddr, numAddr, ch->setting.ioCh, ar->freq, ar->type, ar->rev);
        //LayerArinc::switchAr(ch->setting.numCh, ch->setting.ioCh,1);
        if(ch->setting.ioCh == 1)
        {
            for(int i=0;i<ar->numIndAddr[ar->curIndArray];i++)
            {
               index = ar->indAddr[i][ar->curIndArray];
               LayerArinc::writeTo(ch->setting.numCh,ch->setting.ioCh,ar->value[index],ar->addr[index],i);
            }            
             
        }
            //LayerArinc::initCh(ch->setting.numCh, ar->addr[ar->curIndArray], ar->numAddr[ar->curIndArray], ch->setting.ioCh, ar->freq, ar->type, ar->rev);
        
    }
}
