#ifndef _LOCK_GUARD_H
#define _LOCK_GUARD_H

#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LOCK_HANDLE;

#define LK_CREATE()     lk_create()
#define LK_DESTROY(h)   lk_destroy(h)
#define LK_LOCK(h)      lk_lock(h)
#define LK_UNLOCK(h)    lk_unlock(h)

LOCK_HANDLE lk_create(void);
void lk_destroy(LOCK_HANDLE hLock);
void lk_lock(LOCK_HANDLE hLock);
void lk_unlock(LOCK_HANDLE hLock);

#ifdef __cplusplus
}
#endif

#endif 
