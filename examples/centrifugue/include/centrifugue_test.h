#ifndef _CENTRIFUGE_TEST_H
#define _CENTRIFUGE_TEST_H

#include "data_types.h"
#include "state_machine.h"

// Declare the private instance of CentrifugeTest state machine
SM_DECLARE(centrifugue_test_state_machine)

// State machine event functions
EVENT_DECLARE(cfg_start, no_event_data_t)
EVENT_DECLARE(cfg_cancel, no_event_data_t)
EVENT_DECLARE(cfg_poll, no_event_data_t)

BOOL cfg_is_poll_active();

#endif // _CENTRIFUGE_TEST_H
