#include "motor.h"
#include "state_machine.h"
#include <stdio.h>

// State enumeration order must match the order of state
// method entries in the state map
enum States
{
    ST_IDLE,
    ST_STOP,
    ST_START,
    ST_CHANGE_SPEED,
    ST_MAX_STATES
};

// State machine state functions
STATE_DECLARE(Idle, no_event_data_t)
STATE_DECLARE(Stop, no_event_data_t)
STATE_DECLARE(Start, motor_data_t)
STATE_DECLARE(ChangeSpeed, motor_data_t)

// State map to define state function order
BEGIN_STATE_MAP(Motor)
    STATE_MAP_ENTRY(ST_Idle)
    STATE_MAP_ENTRY(ST_Stop)
    STATE_MAP_ENTRY(ST_Start)
    STATE_MAP_ENTRY(ST_ChangeSpeed)
END_STATE_MAP(Motor)

// Set motor speed external event
EVENT_DEFINE(mtr_set_speed, motor_data_t)
{
    // Given the SetSpeed event, transition to a new state based upon 
    // the current state of the state machine
    BEGIN_TRANSITION_MAP                        // - Current State -
        TRANSITION_MAP_ENTRY(ST_START)          // ST_Idle       
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop       
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_Start      
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_ChangeSpeed
    END_TRANSITION_MAP(Motor, p_event_data)
}

// Halt motor external event
EVENT_DEFINE(mtr_halt, no_event_data_t)
{
    // Given the Halt event, transition to a new state based upon 
    // the current state of the state machine
    BEGIN_TRANSITION_MAP                        // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_Start
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_ChangeSpeed
    END_TRANSITION_MAP(Motor, p_event_data)
}

// State machine sits here when motor is not running
STATE_DEFINE(Idle, no_event_data_t)
{
    printf("%s ST_Idle\n", self->name);
}

// Stop the motor 
STATE_DEFINE(Stop, no_event_data_t)
{
    // Get pointer to the instance data and update currentSpeed
    Motor* p_instance = sm_get_instance(Motor);
    p_instance->currentSpeed = 0;

    // Perform the stop motor processing here
    printf("%s ST_Stop: %d\n", self->name, p_instance->currentSpeed);

    // Transition to ST_Idle via an internal event
    sm_internal_event(ST_IDLE, NULL);
}

// Start the motor going
STATE_DEFINE(Start, motor_data_t)
{
    ASSERT_TRUE(p_event_data);

    // Get pointer to the instance data and update currentSpeed
    Motor* p_instance = sm_get_instance(Motor);
    p_instance->currentSpeed = p_event_data->speed;

    // Set initial motor speed processing here
    printf("%s ST_Start: %d\n", self->name, p_instance->currentSpeed);
}

// Changes the motor speed once the motor is moving
STATE_DEFINE(ChangeSpeed, motor_data_t)
{
    ASSERT_TRUE(p_event_data);

    // Get pointer to the instance data and update currentSpeed
    Motor* p_instance = sm_get_instance(Motor);
    p_instance->currentSpeed = p_event_data->speed;

    // Perform the change motor speed here
    printf("%s ST_ChangeSpeed: %d\n", self->name, p_instance->currentSpeed);
}

// Get current speed
GET_DEFINE(mtr_get_speed, INT)
{
    Motor* p_instance = sm_get_instance(Motor);
    return p_instance->currentSpeed;
}

