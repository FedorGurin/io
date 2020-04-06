#ifndef IMPORTOBJDLL_H
#define IMPORTOBJDLL_H

#include "IEvent.h"

#define IMPORTOBJ __declspec( dllimport )
//! инициализация  модели
extern "C" IMPORTOBJ void Init();
//! основная точка входа
extern "C" IMPORTOBJ void Calcualte(float step);
//! процедура внтуреннего связывания моделей
extern "C" IMPORTOBJ bool Bind();
//! отправка сообщений в модели
extern "C" IMPORTOBJ void eEvent(TEventModel *event);
//! инстациирование объектов модели
extern "C" IMPORTOBJ void Inst();

#endif
