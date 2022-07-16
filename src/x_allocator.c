#include "x_allocator.h"
#include "fb_allocator.h"
#include "data_types.h"
#include "fault.h"
#include <string.h>

static void* xalloc_put_allocator_ptr_in_block(void* block, alloc_allocator_t* allocator);
static alloc_allocator_t* xalloc_get_allocator_ptr_from_block(void* block);
static alloc_allocator_t* xalloc_get_allocator(x_alloc_data_t* self, size_t size);

//----------------------------------------------------------------------------
// xalloc_put_allocator_ptr_in_block
//----------------------------------------------------------------------------
static void* xalloc_put_allocator_ptr_in_block(void* block, alloc_allocator_t* allocator)
{
    alloc_allocator_t** p_allocator_in_block;

    ASSERT_TRUE(block);
    ASSERT_TRUE(allocator);

    // Cast raw block memory to alloc_allocator_t**
    p_allocator_in_block = (alloc_allocator_t**)(block);

    // Store the allocator pointer in the memory block 
    *p_allocator_in_block = allocator;

    // Advance the pointer past the alloc_allocator_t* and return a
    // pointer to the client's memory region
    return ++p_allocator_in_block;
}

//----------------------------------------------------------------------------
// xalloc_get_allocator_ptr_from_block
//----------------------------------------------------------------------------
static alloc_allocator_t* xalloc_get_allocator_ptr_from_block(void* block)
{
    alloc_allocator_t** p_allocator_in_block;

    ASSERT_TRUE(block);

    // Cast raw memory block to alloc_allocator_t**
    p_allocator_in_block = (alloc_allocator_t**)(block);

    // Backup one alloc_allocator_t* position to get the stored allocator instance
    p_allocator_in_block--;

    // Return the allocator instance stored within the memory block
    return *p_allocator_in_block;
}

//----------------------------------------------------------------------------
// XALLOC_GetBlockPtr
//----------------------------------------------------------------------------
static void* XALLOC_GetBlockPtr(void* block)
{
    alloc_allocator_t** p_allocator_in_block;

    ASSERT_TRUE(block);

    // Cast the client memory to alloc_allocator_t* 
    p_allocator_in_block = (alloc_allocator_t**)(block);

    // Back up one alloc_allocator_t* position and return raw memory block pointer
    return --p_allocator_in_block;
}

//----------------------------------------------------------------------------
// xalloc_get_allocator
//----------------------------------------------------------------------------
static alloc_allocator_t* xalloc_get_allocator(x_alloc_data_t* self, size_t size)
{
    UINT16 i = 0;
    alloc_allocator_t* pAllocator = NULL;

    ASSERT_TRUE(self);

    // Each block stores additional meta data (i.e. an alloc_allocator_t pointer). 
    // Add overhead for the additional memory required.
    size += XALLOC_BLOCK_META_DATA_SIZE;

    // Iterate over all allocators 
    for (i=0; i<self->allocators_max; i++)
    {
        // Can the allocator instance handle the requested size?
        if (self->allocators[i] && self->allocators[i]->block_size >= size)
        {
            // Return allocator instance to handle memory request
            pAllocator = self->allocators[i];
            break;
        }
    }

    return pAllocator;
} 

//----------------------------------------------------------------------------
// XALLOC_Alloc
//----------------------------------------------------------------------------
void* xalloc_alloc(x_alloc_data_t* self, size_t size)
{
    alloc_allocator_t* pAllocator;
    void* pBlockMemory = NULL;
    void* pClientMemory = NULL;

    ASSERT_TRUE(self);

    // Get an allocator instance to handle the memory request
    pAllocator = xalloc_get_allocator(self, size);

    // An allocator found to handle memory request?
    if (pAllocator)
    {
        // Get a fixed memory block from the allocator instance
        pBlockMemory = alloc_alloc(pAllocator, size + XALLOC_BLOCK_META_DATA_SIZE);
        if (pBlockMemory)
        {
            // Set the block alloc_allocator_t* ptr within the raw memory block region
            pClientMemory = xalloc_put_allocator_ptr_in_block(pBlockMemory, pAllocator);
        }
    }
    else
    {
        // Too large a memory block requested
        ASSERT();
    }

    return pClientMemory;
} 

//----------------------------------------------------------------------------
// XALLOC_Free
//----------------------------------------------------------------------------
void xalloc_free(void* ptr)
{
    alloc_allocator_t* pAllocator = NULL;
    void* pBlock = NULL;

    if (!ptr)
        return;

    // Extract the original allocator instance from the caller's block pointer
    pAllocator = xalloc_get_allocator_ptr_from_block(ptr);
    if (pAllocator)
    {
        // Convert the client pointer into the original raw block pointer
        pBlock = XALLOC_GetBlockPtr(ptr);

        // Deallocate the fixed memory block
        alloc_free(pAllocator, pBlock);
    }
} 

//----------------------------------------------------------------------------
// XALLOC_Realloc
//----------------------------------------------------------------------------
void* xalloc_realloc(x_alloc_data_t* self, void *ptr, size_t new_size)
{
    void* pNewMem = NULL;
    alloc_allocator_t* pOldAllocator = NULL;
    size_t oldSize = 0;

    ASSERT_TRUE(self);

    if (!ptr)
        pNewMem = xalloc_alloc(self, new_size);
    else if (0 == new_size)
        xalloc_free(ptr);
    else
    {
        // Create a new memory block
        pNewMem = xalloc_alloc(self, new_size);
        if (pNewMem != 0)
        {
            // Get the original allocator instance from the old memory block
            pOldAllocator = xalloc_get_allocator_ptr_from_block(ptr);
            oldSize = pOldAllocator->block_size - XALLOC_BLOCK_META_DATA_SIZE;

            // Copy the bytes from the old memory block into the new (as much as will fit)
            memcpy(pNewMem, ptr, (oldSize < new_size) ? oldSize : new_size);

            // Free the old memory block
            xalloc_free(ptr);
        }
    }

    // Return the client pointer to the new memory block
    return pNewMem;
} 

//----------------------------------------------------------------------------
// XALLOC_Calloc
//----------------------------------------------------------------------------
void* xalloc_calloc(x_alloc_data_t* self, size_t num, size_t size)
{
    void* pMem = NULL;
    size_t n;

    ASSERT_TRUE(self);

    // Compute the total block size
    n = num * size;

    // Allocate the memory
    pMem = xalloc_alloc(self, n);

    if (pMem)
    {
        // Initialize memory to 0
        memset(pMem, 0, n);
    }

    return pMem;
} 



