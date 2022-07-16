#include "centrifugue_test.h"
#include "state_machine.h"
#include <stdio.h>

// centrifugue_test_t object structure
typedef struct
{
    INT speed;
    BOOL pollActive;
} centrifugue_test_t;

// Define private instance of motor state machine
centrifugue_test_t centrifugue_test_tObj;
SM_DEFINE(centrifugue_test_state_machine, &centrifugue_test_tObj)

// State enumeration order must match the order of state
// method entries in the state map
enum States
{
    ST_IDLE,
    ST_COMPLETED,
    ST_FAILED,
    ST_START_TEST,
    ST_ACCELERATION,
    ST_WAIT_FOR_ACCELERATION,
    ST_DECELERATION,
    ST_WAIT_FOR_DECELERATION,
    ST_MAX_STATES
};

// State machine state functions
STATE_DECLARE(Idle, no_event_data_t)
ENTRY_DECLARE(Idle, no_event_data_t)
STATE_DECLARE(Completed, no_event_data_t)
STATE_DECLARE(Failed, no_event_data_t)
STATE_DECLARE(StartTest, no_event_data_t)
GUARD_DECLARE(StartTest, no_event_data_t)
STATE_DECLARE(Acceleration, no_event_data_t)
STATE_DECLARE(WaitForAcceleration, no_event_data_t)
EXIT_DECLARE(WaitForAcceleration)
STATE_DECLARE(Deceleration, no_event_data_t)
STATE_DECLARE(WaitForDeceleration, no_event_data_t)
EXIT_DECLARE(WaitForDeceleration)

// State map to define state function order
BEGIN_STATE_MAP_EX(centrifugue_test_t)
    STATE_MAP_ENTRY_ALL_EX(ST_Idle, 0, EN_Idle, 0)
    STATE_MAP_ENTRY_EX(ST_Completed)
    STATE_MAP_ENTRY_EX(ST_Failed)
    STATE_MAP_ENTRY_ALL_EX(ST_StartTest, GD_StartTest, 0, 0)
    STATE_MAP_ENTRY_EX(ST_Acceleration)
    STATE_MAP_ENTRY_ALL_EX(ST_WaitForAcceleration, 0, 0, EX_WaitForAcceleration)
    STATE_MAP_ENTRY_EX(ST_Deceleration)
    STATE_MAP_ENTRY_ALL_EX(ST_WaitForDeceleration, 0, 0, EX_WaitForDeceleration)
END_STATE_MAP_EX(centrifugue_test_t)

EVENT_DEFINE(cfg_start, no_event_data_t)
{
    BEGIN_TRANSITION_MAP                                // - Current State -
        TRANSITION_MAP_ENTRY(ST_START_TEST)             // ST_IDLE
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_COMPLETED
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_FAILED
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_START_TEST
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_ACCELERATION
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_WAIT_FOR_ACCELERATION
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_DECELERATION
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_WAIT_FOR_DECELERATION
    END_TRANSITION_MAP(centrifugue_test_t, p_event_data)
}

EVENT_DEFINE(cfg_cancel, no_event_data_t)
{
    BEGIN_TRANSITION_MAP                                // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_IDLE
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_COMPLETED
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_FAILED
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_START_TEST
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_WAIT_FOR_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_DECELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_WAIT_FOR_DECELERATION
    END_TRANSITION_MAP(centrifugue_test_t, p_event_data)
}

EVENT_DEFINE(cfg_poll, no_event_data_t)
{
    BEGIN_TRANSITION_MAP                                    // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_IDLE
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_COMPLETED
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_FAILED
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_START_TEST
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_ACCELERATION)      // ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_ACCELERATION)      // ST_WAIT_FOR_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_DECELERATION)      // ST_DECELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_DECELERATION)      // ST_WAIT_FOR_DECELERATION
    END_TRANSITION_MAP(centrifugue_test_t, p_event_data)
}

static void StartPoll(void)
{
    centrifugue_test_tObj.pollActive = TRUE;
}

static void StopPoll(void)
{
    centrifugue_test_tObj.pollActive = FALSE;
}

BOOL cfg_is_poll_active(void) 
{ 
    return centrifugue_test_tObj.pollActive;
}

STATE_DEFINE(Idle, no_event_data_t)
{
    printf("%s ST_Idle\n", self->name);
}

ENTRY_DEFINE(Idle, no_event_data_t)
{
    printf("%s EN_Idle\n", self->name);
    centrifugue_test_tObj.speed = 0;
    StopPoll();
}

STATE_DEFINE(Completed, no_event_data_t)
{
    printf("%s ST_Completed\n", self->name);
    sm_internal_event(ST_IDLE, NULL);
}

STATE_DEFINE(Failed, no_event_data_t)
{
    printf("%s ST_Failed\n", self->name);
    sm_internal_event(ST_IDLE, NULL);
}

// Start the centrifuge test state.
STATE_DEFINE(StartTest, no_event_data_t)
{
    printf("%s ST_StartTest\n", self->name);
    sm_internal_event(ST_ACCELERATION, NULL);
}

// Guard condition to determine whether StartTest state is executed.
GUARD_DEFINE(StartTest, no_event_data_t)
{
    printf("%s GD_StartTest\n", self->name);
    if (centrifugue_test_tObj.speed == 0)
        return TRUE;    // Centrifuge stopped. OK to start test.
    else
        return FALSE;   // Centrifuge spinning. Can't start test.
}

// Start accelerating the centrifuge.
STATE_DEFINE(Acceleration, no_event_data_t)
{
    printf("%s ST_Acceleration\n", self->name);

    // Start polling while waiting for centrifuge to ramp up to speed
    StartPoll();
}

// Wait in this state until target centrifuge speed is reached.
STATE_DEFINE(WaitForAcceleration, no_event_data_t)
{
    printf("%s ST_WaitForAcceleration : Speed is %d\n", self->name, centrifugue_test_tObj.speed);
    if (++centrifugue_test_tObj.speed >= 5)
        sm_internal_event(ST_DECELERATION, NULL);
}

// Exit action when WaitForAcceleration state exits.
EXIT_DEFINE(WaitForAcceleration)
{
    printf("%s EX_WaitForAcceleration\n", self->name);

    // Acceleration over, stop polling
    StopPoll();
}

// Start decelerating the centrifuge.
STATE_DEFINE(Deceleration, no_event_data_t)
{
    printf("%s ST_Deceleration\n", self->name);

    // Start polling while waiting for centrifuge to ramp down to 0
    StartPoll();
}

// Wait in this state until centrifuge speed is 0.
STATE_DEFINE(WaitForDeceleration, no_event_data_t)
{
    printf("%s ST_WaitForDeceleration : Speed is %d\n", self->name, centrifugue_test_tObj.speed);
    if (centrifugue_test_tObj.speed-- == 0)
        sm_internal_event(ST_COMPLETED, NULL);
}

// Exit action when WaitForDeceleration state exits.
EXIT_DEFINE(WaitForDeceleration)
{
    printf("%s EX_WaitForDeceleration\n", self->name);

    // Deceleration over, stop polling
    StopPoll();
}


