// https://www.codeproject.com/Articles/1272619/A-Fixed-Block-Memory-Allocator-in-C
//
// The fb_allocator is a fixed block memory allocator that handles a 
// single block size. 
//
// Create an allocator instance using the ALLOC_DEFINE macro. Call 
// alloc_init() one time at startup. alloc_alloc() allocates a fixed 
// memory block. alloc_free() frees the block. 
//
// #include "fb_allocator.h"
// ALLOC_DEFINE(myAllocator, 32, 5)
//
// void main() 
// {
//      void* block;
//      alloc_init();
//      block = alloc_alloc(myAllocator, 32);
//      alloc_free(myAllocator, block);
// }

#ifndef _FB_ALLOCATOR_H
#define _FB_ALLOCATOR_H

#include <stdlib.h>
#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* ALLOC_HANDLE;

typedef struct 
{
    void* p_next;
} allock_block;

// Use ALLOC_DEFINE to declare an alloc_allocator_t object
typedef struct
{
    const char* name;
    const char* p_pool;
    const size_t object_size;
    const size_t block_size;
    const UINT32 blocks_max;
    allock_block* p_head;
    UINT16 pool_index;
    UINT16 blocks_in_use;
    UINT16 max_blocks_in_use;
    UINT16 allocations;
    UINT16 deallocations;
} alloc_allocator_t;

// Align fixed blocks on X-byte boundary based on CPU architecture.
// Set value to 1, 2, 4 or 8.
#define ALLOC_MEM_ALIGN   (1)

// Get the maximum between a or b
#define ALLOC_MAX(a,b) (((a)>(b))?(a):(b))

// Round _numToRound_ to the next higher _multiple_
#define ALLOC_ROUND_UP(_numToRound_, _multiple_) \
    (((_numToRound_ + _multiple_ - 1) / _multiple_) * _multiple_)

// Ensure the memory block size is: (a) is aligned on desired boundary and (b) at
// least the size of a alloc_allocator_t*. 
#define ALLOC_BLOCK_SIZE(_size_) \
    (ALLOC_MAX((ALLOC_ROUND_UP(_size_, ALLOC_MEM_ALIGN)), sizeof(alloc_allocator_t*)))

// Defines block memory, allocator instance and a handle. On the example below, 
// the alloc_allocator_t instance is myAllocatorObj and the handle is myAllocator.
// _name_ - the allocator name
// _size_ - fixed memory block size in bytes
// _objects_ - number of fixed memory blocks 
// e.g. ALLOC_DEFINE(myAllocator, 32, 10)
#define ALLOC_DEFINE(_name_, _size_, _objects_) \
    static char _name_##Memory[ALLOC_BLOCK_SIZE(_size_) * (_objects_)] = { 0 }; \
    static alloc_allocator_t _name_##Obj = { #_name_, _name_##Memory, _size_, \
        ALLOC_BLOCK_SIZE(_size_), _objects_, NULL, 0, 0, 0, 0, 0 }; \
    static ALLOC_HANDLE _name_ = &_name_##Obj;

void alloc_init(void);
void alloc_term(void);
void* alloc_alloc(ALLOC_HANDLE hAlloc, size_t size);
void* alloc_calloc(ALLOC_HANDLE hAlloc, size_t num, size_t size);
void alloc_free(ALLOC_HANDLE hAlloc, void* pBlock);

#ifdef __cplusplus
}
#endif

#endif  // _FB_ALLOCATOR_H




