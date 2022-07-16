// https://www.codeproject.com/Articles/1275479/State-Machine-Design-in-C
//
// The StateMachine module is a C language implementation of a finite state 
// machine (FSM).
//
// All event data must be created dynamically using SM_XAlloc. Use a fixed 
// block allocator or the heap as desired. 
//
// The standard version (non-EX) supports state and event functions. The 
// extended version (EX) supports the additional guard, entry and exit state
// machine features. 
//
// Macros are used to assist in creating the state machine machinery. 

#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "data_types.h"
#include "fault.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define USE_SM_ALLOCATOR to use the fixed block allocator instead of heap
#define USE_SM_ALLOCATOR
#ifdef USE_SM_ALLOCATOR
    #include "sm_allocator.h"
    #define sm_xalloc(size)    smalloc_alloc(size)
    #define sm_xfree(ptr)      smalloc_free(ptr)
#else
    #include <stdlib.h>
    #define sm_xalloc(size)    malloc(size)
    #define sm_xfree(ptr)      free(ptr)
#endif

enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN = 0xFF };

typedef void no_event_data_t;

// State machine constant data
typedef struct
{
    const CHAR* name;
    const BYTE states_max;
    const struct sm_state_t* state_map;
    const struct sm_state_ex_t* state_map_ex;
} sm_state_machine_const_t;

// State machine instance data
typedef struct 
{
    const CHAR* name;
    void* p_instance;
    BYTE new_state;
    BYTE current_state;
    BOOL event_generated;
    void* p_event_data;
} sm_state_machine_t;

// Generic state function signatures
typedef void (*sm_state_func_t)(sm_state_machine_t* self, void* p_event_data);
typedef BOOL (*sm_guard_func_t)(sm_state_machine_t* self, void* p_event_data);
typedef void (*sm_entry_func_t)(sm_state_machine_t* self, void* p_event_data);
typedef void (*sm_exit_func_t)(sm_state_machine_t* self);

typedef struct sm_state_t
{
    sm_state_func_t p_state_func;
} sm_state_t;

typedef struct sm_state_ex_t
{
    sm_state_func_t p_state_func;
    sm_guard_func_t p_guard_func;
    sm_entry_func_t p_entry_func;
    sm_exit_func_t p_exit_func;
} sm_state_ex_t;

// Public functions
#define sm_event(_sm_name_, _event_func_, _event_data_) \
    _event_func_(&_sm_name_##obj, _event_data_)
#define SM_Get(_sm_name_, _get_func_) \
    _get_func_(&_sm_name_##obj)

// Protected functions
#define sm_internal_event(_newState_, _event_data_) \
    _sm_internal_event(self, _newState_, _event_data_)
#define SM_GetInstance(_instance_) \
    (_instance_*)(self->p_instance);

// Private functions
void _sm_external_event(sm_state_machine_t* self, const sm_state_machine_const_t* selfconst, BYTE new_state, void* p_event_data);
void _sm_internal_event(sm_state_machine_t* self, BYTE new_state, void* p_event_data);
void _sm_state_engine(sm_state_machine_t* self, const sm_state_machine_const_t* selfconst);
void _sm_state_engine_ex(sm_state_machine_t* self, const sm_state_machine_const_t* selfconst);

#define SM_DECLARE(_sm_name_) \
    extern sm_state_machine_t _sm_name_##obj; 

#define SM_DEFINE(_sm_name_, _instance_) \
    sm_state_machine_t _sm_name_##obj = { #_sm_name_, _instance_, \
        0, 0, 0, 0 }; 

#define EVENT_DECLARE(_event_func_, _event_data_) \
    void _event_func_(sm_state_machine_t* self, _event_data_* p_event_data);

#define EVENT_DEFINE(_event_func_, _event_data_) \
    void _event_func_(sm_state_machine_t* self, _event_data_* p_event_data)

#define GET_DECLARE(_get_func_, _getData_) \
    _getData_ _get_func_(sm_state_machine_t* self);

#define GET_DEFINE(_get_func_, _getData_) \
    _getData_ _get_func_(sm_state_machine_t* self)

#define STATE_DECLARE(_state_func_, _event_data_) \
    static void ST_##_state_func_(sm_state_machine_t* self, _event_data_* p_event_data);

#define STATE_DEFINE(_state_func_, _event_data_) \
    static void ST_##_state_func_(sm_state_machine_t* self, _event_data_* p_event_data)

#define GUARD_DECLARE(_guard_func_, _event_data_) \
    static BOOL GD_##_guard_func_(sm_state_machine_t* self, _event_data_* p_event_data);

#define GUARD_DEFINE(_guard_func_, _event_data_) \
    static BOOL GD_##_guard_func_(sm_state_machine_t* self, _event_data_* p_event_data)

#define ENTRY_DECLARE(_entry_func_, _event_data_) \
    static void EN_##_entry_func_(sm_state_machine_t* self, _event_data_* p_event_data);

#define ENTRY_DEFINE(_entry_func_, _event_data_) \
    static void EN_##_entry_func_(sm_state_machine_t* self, _event_data_* p_event_data)

#define EXIT_DECLARE(_exit_func_) \
    static void EX_##_exit_func_(sm_state_machine_t* self);

#define EXIT_DEFINE(_exit_func_) \
    static void EX_##_exit_func_(sm_state_machine_t* self)

#define BEGIN_STATE_MAP(_sm_name_) \
    static const sm_state_t _sm_name_##state_map[] = { 

#define STATE_MAP_ENTRY(_state_func_) \
    { (sm_state_func_t)_state_func_ },

#define END_STATE_MAP(_sm_name_) \
    }; \
    static const sm_state_machine_const_t _sm_name_##const = { #_sm_name_, \
        (sizeof(_sm_name_##state_map)/sizeof(_sm_name_##state_map[0])), \
        _sm_name_##state_map, NULL };

#define BEGIN_STATE_MAP_EX(_sm_name_) \
    static const sm_state_ex_t _sm_name_##state_map[] = { 

#define STATE_MAP_ENTRY_EX(_state_func_) \
    { (sm_state_func_t)_state_func_, NULL, NULL, NULL },

#define STATE_MAP_ENTRY_ALL_EX(_state_func_, _guard_func_, _entry_func_, _exit_func_) \
    { (sm_state_func_t)_state_func_, (sm_guard_func_t)_guard_func_, (sm_entry_func_t)_entry_func_, (sm_exit_func_t)_exit_func_ },

#define END_STATE_MAP_EX(_sm_name_) \
    }; \
    static const sm_state_machine_const_t _sm_name_##const = { #_sm_name_, \
        (sizeof(_sm_name_##state_map)/sizeof(_sm_name_##state_map[0])), \
        NULL, _sm_name_##state_map };

#define BEGIN_TRANSITION_MAP \
    static const BYTE TRANSITIONS[] = { \

#define TRANSITION_MAP_ENTRY(_entry_) \
    _entry_,

#define END_TRANSITION_MAP(_sm_name_, _event_data_) \
    }; \
    _sm_external_event(self, &_sm_name_##const, TRANSITIONS[self->current_state], _event_data_); \
    C_ASSERT((sizeof(TRANSITIONS)/sizeof(BYTE)) == (sizeof(_sm_name_##state_map)/sizeof(_sm_name_##state_map[0])));

#ifdef __cplusplus
}
#endif
 
#endif // _STATE_MACHINE_H
