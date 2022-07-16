#include "fault.h"
#include "state_machine.h"

// Generates an external event. Called once per external event 
// to start the state machine executing
void _sm_external_event(sm_state_machine_t* self, const sm_state_machine_const_t* self_const, BYTE new_state, void* p_event_data)
{
    // If we are supposed to ignore this event
    if (new_state == EVENT_IGNORED) 
    {
        // Just delete the event data, if any
        if (p_event_data)
            sm_xfree(p_event_data);
    }
    else 
    {
        // TODO - capture software lock here for thread-safety if necessary

        // Generate the event 
        _sm_internal_event(self, new_state, p_event_data);

        // Execute state machine based on type of state map defined
        if (self_const->state_map)
            _sm_state_engine(self, self_const);
        else
            _sm_state_engine_ex(self, self_const);

        // TODO - release software lock here 
    }
}

// Generates an internal event. Called from within a state 
// function to transition to a new state
void _sm_internal_event(sm_state_machine_t* self, BYTE new_state, void* p_event_data)
{
    ASSERT_TRUE(self);

    self->p_event_data = p_event_data;
    self->event_generated = TRUE;
    self->new_state = new_state;
}

// The state engine executes the state machine states
void _sm_state_engine(sm_state_machine_t* self, const sm_state_machine_const_t* self_const)
{
    void* pDataTemp = NULL;

    ASSERT_TRUE(self);
    ASSERT_TRUE(self_const);

    // While events are being generated keep executing states
    while (self->event_generated)
    {
        // Error check that the new state is valid before proceeding
        ASSERT_TRUE(self->new_state < self_const->states_max);

        // Get the pointers from the state map
        sm_state_func_t state = self_const->state_map[self->new_state].p_state_func;

        // Copy of event data pointer
        pDataTemp = self->p_event_data;

        // Event data used up, reset the pointer
        self->p_event_data = NULL;

        // Event used up, reset the flag
        self->event_generated = FALSE;

        // Switch to the new current state
        self->current_state = self->new_state;

        // Execute the state action passing in event data
        ASSERT_TRUE(state != NULL);
        state(self, pDataTemp);

        // If event data was used, then delete it
        if (pDataTemp)
        {
            sm_xfree(pDataTemp);
            pDataTemp = NULL;
        }
    }
}

// The state engine executes the extended state machine states
void _sm_state_engine_ex(sm_state_machine_t* self, const sm_state_machine_const_t* self_const)
{
    BOOL guardResult = TRUE;
    void* pDataTemp = NULL;

    ASSERT_TRUE(self);
    ASSERT_TRUE(self_const);

    // While events are being generated keep executing states
    while (self->event_generated)
    {
        // Error check that the new state is valid before proceeding
        ASSERT_TRUE(self->new_state < self_const->states_max);

        // Get the pointers from the extended state map
        sm_state_func_t state = self_const->state_map_ex[self->new_state].p_state_func;
        sm_guard_func_t guard = self_const->state_map_ex[self->new_state].p_guard_func;
        sm_entry_func_t entry = self_const->state_map_ex[self->new_state].p_entry_func;
        sm_exit_func_t exit = self_const->state_map_ex[self->current_state].p_exit_func;

        // Copy of event data pointer
        pDataTemp = self->p_event_data;

        // Event data used up, reset the pointer
        self->p_event_data = NULL;

        // Event used up, reset the flag
        self->event_generated = FALSE;

        // Execute the guard condition
        if (guard != NULL)
            guardResult = guard(self, pDataTemp);

        // If the guard condition succeeds
        if (guardResult == TRUE)
        {
            // Transitioning to a new state?
            if (self->new_state != self->current_state)
            {
                // Execute the state exit action on current state before switching to new state
                if (exit != NULL)
                    exit(self);

                // Execute the state entry action on the new state
                if (entry != NULL)
                    entry(self, pDataTemp);

                // Ensure exit/entry actions didn't call sm_internal_event by accident 
                ASSERT_TRUE(self->event_generated == FALSE);
            }

            // Switch to the new current state
            self->current_state = self->new_state;

            // Execute the state action passing in event data
            ASSERT_TRUE(state != NULL);
            state(self, pDataTemp);
        }

        // If event data was used, then delete it
        if (pDataTemp)
        {
            sm_xfree(pDataTemp);
            pDataTemp = NULL;
        }
    }
}
