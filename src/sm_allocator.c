// SMALLOC allocates either a 32 or 128 byte block depending 
// on the requested size. 

#include "sm_allocator.h"
#include "x_allocator.h"

// Maximum number of blocks for each size
#define MAX_32_BLOCKS   10
#define MAX_128_BLOCKS	5

// Define size of each block including meta data overhead
#define BLOCK_32_SIZE     32 + XALLOC_BLOCK_META_DATA_SIZE
#define BLOCK_128_SIZE    128 + XALLOC_BLOCK_META_DATA_SIZE

// Define individual fb_allocators
ALLOC_DEFINE(smDataAllocator32, BLOCK_32_SIZE, MAX_32_BLOCKS)
ALLOC_DEFINE(smDataAllocator128, BLOCK_128_SIZE, MAX_128_BLOCKS)

// An array of allocators sorted by smallest block first
static alloc_allocator_t* allocators[] = {
    &smDataAllocator32Obj,
    &smDataAllocator128Obj
};

#define MAX_ALLOCATORS   (sizeof(allocators) / sizeof(allocators[0]))

static x_alloc_data_t self = { allocators, MAX_ALLOCATORS };

//----------------------------------------------------------------------------
// smalloc_alloc
//----------------------------------------------------------------------------
void* smalloc_alloc(size_t size)
{
    return xalloc_alloc(&self, size);
}

//----------------------------------------------------------------------------
// smalloc_free
//----------------------------------------------------------------------------
void smalloc_free(void* ptr)
{
    xalloc_free(ptr);
}

//----------------------------------------------------------------------------
// smalloc_realloc
//----------------------------------------------------------------------------
void* smalloc_realloc(void *ptr, size_t new_size)
{
    return xalloc_realloc(&self, ptr, new_size);
}

//----------------------------------------------------------------------------
// smalloc_calloc
//----------------------------------------------------------------------------
void* smalloc_calloc(size_t num, size_t size)
{
    return xalloc_calloc(&self, num, size);
}

