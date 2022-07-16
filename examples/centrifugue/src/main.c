#include "fb_allocator.h"
#include "state_machine.h"
#include "motor.h"
#include "centrifugue_test.h"

// Define motor objects
static Motor motorObj1;
static Motor motorObj2;

// Define two public Motor state machine instances
SM_DEFINE(Motor1SM, &motorObj1)
SM_DEFINE(Motor2SM, &motorObj2)

int main(void)
{
    alloc_init();

    motor_data_t* data;

    // Create event data
    data = sm_xalloc(sizeof(motor_data_t));
    data->speed = 100;

    // Call mtr_set_speed event function to start motor
    sm_event(Motor1SM, mtr_set_speed, data);

    // Call mtr_set_speed event function to change motor speed
    data = sm_xalloc(sizeof(motor_data_t));
    data->speed = 200;
    sm_event(Motor1SM, mtr_set_speed, data);

    // Get current speed from Motor1SM
    INT currentSpeed = SM_Get(Motor1SM, mtr_get_speed);

    // Stop motor again will be ignored
    sm_event(Motor1SM, mtr_halt, NULL);

    // Motor2SM example
    data = sm_xalloc(sizeof(motor_data_t));
    data->speed = 300;
    sm_event(Motor2SM, mtr_set_speed, data);
    sm_event(Motor2SM, mtr_halt, NULL);

    // centrifugue_test_state_machine example
    sm_event(centrifugue_test_state_machine, cfg_cancel, NULL);
    sm_event(centrifugue_test_state_machine, cfg_start, NULL);
    while (cfg_is_poll_active())
        sm_event(centrifugue_test_state_machine, cfg_poll, NULL);

    alloc_term();

    return 0;
}

