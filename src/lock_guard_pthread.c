#include "lock_guard.h"
#include "fault.h"

#include <pthread.h>
#include <stdlib.h>

/**
 * @brief Create Locking Object
 * 
 * @return LOCK_HANDLE 
 */
LOCK_HANDLE lk_create(void)
{
    pthread_mutex_t * lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock, NULL);
    return lock;
}

/**
 * @brief Destroy Locking Object
 * 
 * @param hLock 
 */
void lk_destroy(LOCK_HANDLE hLock)
{
    ASSERT_TRUE(hLock);
    free(hLock);
}

/**
 * @brief Assign Lock to Locking Object
 * 
 * @param hLock 
 */
void lk_lock(LOCK_HANDLE hLock)
{
    ASSERT_TRUE(hLock);
    pthread_mutex_lock(hLock);
}

/**
 * @brief Unlock Locking Object
 * 
 * @param hLock 
 */
void lk_unlock(LOCK_HANDLE hLock)
{
    ASSERT_TRUE(hLock);
    pthread_mutex_unlock(hLock);
}

