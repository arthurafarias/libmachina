#ifndef _SM_ALLOCATOR_H
#define _SM_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* smalloc_alloc(size_t size);
void smalloc_free(void* ptr);
void* smalloc_realloc(void *ptr, size_t new_size);
void* smalloc_calloc(size_t num, size_t size);

#ifdef __cplusplus
}
#endif

#endif // _SM_ALLOCATOR_H
