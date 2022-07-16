#ifndef _MOTOR_H
#define _MOTOR_H

#include "data_types.h"
#include "state_machine.h"

// Motor object structure
typedef struct
{
    INT currentSpeed;
} Motor;

// Event data structure
typedef struct
{
    INT speed;
} motor_data_t;

// State machine event functions
EVENT_DECLARE(mtr_set_speed, motor_data_t)
EVENT_DECLARE(mtr_halt, no_event_data_t)

// Public accessor
GET_DECLARE(mtr_get_speed, INT);

#endif // _MOTOR_H
