#include "fb_allocator.h"
#include "data_types.h"
#include "fault.h"
#include <string.h>

// Define USE_LOCK to use the default lock implementation
#define USE_LOCKS
#ifdef USE_LOCKS
    #include "lock_guard.h"
    static LOCK_HANDLE _lock_handle;
#else
    #pragma message("WARNING: Define software lock.")
    typedef int LOCK_HANDLE;
    static LOCK_HANDLE _lock_handle;

    #define lk_create()     (1)
    #define lk_destry(h)  
    #define lk_lock(h)    
    #define lk_unlock(h)  
#endif

// Get a pointer to the client's area within a memory block
#define GET_CLIENT_PTR(_block_ptr_) \
    (_block_ptr_ ? ((void*)((char*)_block_ptr_)) : NULL)

// Get a pointer to the block using a client pointer
#define GET_BLOCK_PTR(_client_ptr_) \
    (_client_ptr_ ? ((void*)((char*)_client_ptr_)) : NULL)

static void* alloc_new_block(alloc_allocator_t* alloc);
static void alloc_push(alloc_allocator_t* alloc, void* p_block);
static void* alloc_pop(alloc_allocator_t* alloc);

//----------------------------------------------------------------------------
// alloc_new_block
//----------------------------------------------------------------------------
static void* alloc_new_block(alloc_allocator_t* self)
{
    allock_block* p_block = NULL;

    lk_lock(_lock_handle);

    // If we have not exceeded the pool maximum
    if (self->pool_index < self->blocks_max)
    {
        // Get pointer to a new fixed memory block within the pool
        p_block = (void*)(self->p_pool + (self->pool_index++ * self->block_size));
    }

    LK_UNLOCK(_lock_handle);

    if (!p_block)
    {
        // Out of fixed block memory
        ASSERT();
    }

    return p_block;
} 

//----------------------------------------------------------------------------
// alloc_push
//----------------------------------------------------------------------------
static void alloc_push(alloc_allocator_t* self, void* p_block)
{
    if (!p_block)
        return;

    // Get a pointer to the client's location within the block
    allock_block* pClient = (allock_block*)GET_CLIENT_PTR(p_block);

    lk_lock(_lock_handle);

    // Point client block's next pointer to head
    pClient->p_next = self->p_head;

    // The client block is now the new head
    self->p_head = pClient;

    LK_UNLOCK(_lock_handle); 
}

//----------------------------------------------------------------------------
// alloc_pop
//----------------------------------------------------------------------------
static void* alloc_pop(alloc_allocator_t* self)
{
    allock_block* p_block = NULL;

    lk_lock(_lock_handle);

    // Is the free-list empty?
    if (self->p_head)
    {
        // Remove the head block
        p_block = self->p_head;

        // Set the head to the next block
        self->p_head = self->p_head->p_next;
    }

    LK_UNLOCK(_lock_handle); 
    return GET_BLOCK_PTR(p_block);
} 

//----------------------------------------------------------------------------
// alloc_init
//----------------------------------------------------------------------------
void alloc_init()
{
    _lock_handle = lk_create();
} 

//----------------------------------------------------------------------------
// alloc_term
//----------------------------------------------------------------------------
void alloc_term()
{
    lk_destroy(_lock_handle);
}

//----------------------------------------------------------------------------
// alloc_alloc
//----------------------------------------------------------------------------
void* alloc_alloc(ALLOC_HANDLE hAlloc, size_t size)
{
    alloc_allocator_t* self = NULL;
    void* p_block = NULL;

    ASSERT_TRUE(hAlloc);

    // Convert handle to an alloc_allocator_t instance
    self = (alloc_allocator_t*)hAlloc;

    // Ensure requested size fits within memory block 
    ASSERT_TRUE(size <= self->block_size);

    // Get a block from the free-list
    p_block = alloc_pop(self);

    // If the free-list empty?
    if (!p_block)
    {
        // Get a new block from the pool
        p_block = alloc_new_block(self);
    }

    if (p_block)
    {
        // Keep track of usage statistics
        self->allocations++;
        self->blocks_in_use++;
        if (self->blocks_in_use > self->max_blocks_in_use)
        {
            self->max_blocks_in_use = self->blocks_in_use;
        }
    }

    return GET_CLIENT_PTR(p_block);
} 

//----------------------------------------------------------------------------
// alloc_calloc
//----------------------------------------------------------------------------
void* alloc_calloc(ALLOC_HANDLE hAlloc, size_t num, size_t size)
{
    void* pMem = NULL;
    size_t n = 0;

    ASSERT_TRUE(hAlloc);

    // Compute the total size of the block
    n = num * size;

    // Allocate the memory
    pMem = alloc_alloc(hAlloc, n);

    if (pMem != NULL)
    {
        // Initialize memory to 0 per calloc behavior 
        memset(pMem, 0, n);
    }

    return pMem;
}

//----------------------------------------------------------------------------
// ALLOC_Free
//----------------------------------------------------------------------------
void alloc_free(ALLOC_HANDLE hAlloc, void* p_block)
{
    alloc_allocator_t* self = NULL;

    if (!p_block)
        return;

    ASSERT_TRUE(hAlloc);

    // Cast handle to an allocator instance
    self = (alloc_allocator_t*)hAlloc;

    // Get a pointer to the block
    p_block = GET_BLOCK_PTR(p_block);

    // Push the block onto a stack (i.e. the free-list)
    alloc_push(self, p_block);

    // Keep track of usage statistics
    self->deallocations++;
    self->blocks_in_use--;
} 



