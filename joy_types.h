#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H
#include "platform.h"
enum ControllerType {
    KEYBOARD = 1,
    JOYSTICK = 2,
};

// положение 3-х позиционной кнопки
// только одно из трех положений
enum ThreeButtonPos {
    CENTER = 0, // кнопка отжата
    UP = 1, // вверх или влево
    DOWN = 2, // вниз или вправо
};

// Трех-позиционная кнопка
typedef struct _TThreePosButton {
    ThreeButtonPos pos;
} TThreePosButton;

// Четырех-позиционная кнопка
// Представляет из себя комбинацию из двух 3-х позиционных кнопок
typedef struct _TFourPosButton {
    // положение по вертикали
    ThreeButtonPos vpos;
    // положение по горизонтали
    ThreeButtonPos hpos;
} TFourPosButton;

typedef struct _TRUSParams {
    // axis: x - влево(-)/вправо(+)
    float x;
    // axis: y - от себя(-)/на себя(+)
    float y;
    // button: Триггер
    uint8_t trigger;
    // button: Триггер (сильное нажатие)
    uint8_t trigger_strong;
    // button
    uint8_t wpn_rel;
    // button
    uint8_t nws;
    // button
    uint8_t bottom;
    // button: Временное отключение САУ (?)
    uint8_t bottom_trigger;
    // button
    TFourPosButton trim;
    // button
    TFourPosButton bottomleft;
    // button
    TFourPosButton bottomright;
    // button
    TFourPosButton thumb;
} TRUSParams;
typedef struct _TPEDParams {

	float x;
	float y;
	float z;
}TPEDParams;
typedef struct _TRUDParams {
    // axis: z - от себя(-)/на себя(+)
    float z;
    // axis: ant_elev
    float ant_elev;
    // axis: man_rng
    float man_rng;
    // РУД:
    uint8_t enable;
    // РУД:
    uint8_t uncage;
    // РУД:
    TThreePosButton dog_fight; // 3-х позиционная
    // РУД:
    TThreePosButton spd_brk; // 3-х позиционная
    // РУД:
    TFourPosButton vuhf_iff;
} TRUDParams;

// Параметры РУС и РУД от джойстика
// size = 100
typedef struct _TControllerParams {
    TRUSParams rusParams;
    TRUDParams rudParams;
    TPEDParams pedParams;
} TControllerParams;

typedef TControllerParams params_t;


/*
 * Те же параметры, но с битовыми полями для кнопок.
 * На случай, если потребуется снизить траффик в сети.
 */
typedef struct _TRUSParamsB {
    float x;
    float y;
    uint32_t buttons;
} TRUSParamsB;

typedef struct _TRUDParamsB {
    float z;
    float ant_elev;
    float man_rng;
    uint32_t buttons;
} TRUDParamsB;

typedef struct _TControllerParamsB {
    TRUSParamsB rus;
    TRUDParamsB rud;
} TControllerParamsB;

#endif // CONTROLLER_TYPES_H
