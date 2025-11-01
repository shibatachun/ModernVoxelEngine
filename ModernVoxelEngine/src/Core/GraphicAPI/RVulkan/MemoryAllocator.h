#pragma once
namespace rvulkan {
    struct MemoryServiceConfiguration {

        size_t                       maximum_dynamic_size = 32 * 1024 * 1024;    // Defaults to max 32MB of dynamic memory.

    }; // struct MemoryServiceConfiguration

// Macro helpers //////////////////////////////////////////////////////
#define ralloca(size, allocator)    ((allocator)->allocate( size, 1, __FILE__, __LINE__ ))
#define rallocam(size, allocator)   ((u8*)(allocator)->allocate( size, 1, __FILE__, __LINE__ ))
#define rallocat(type, allocator)   ((type*)(allocator)->allocate( sizeof(type), 1, __FILE__, __LINE__ ))

#define rallocaa(size, allocator, alignment)    ((allocator)->allocate( size, alignment, __FILE__, __LINE__ ))

#define rfree(pointer, allocator) (allocator)->deallocate(pointer)

#define rkilo(size)                 (size * 1024)
#define rmega(size)                 (size * 1024 * 1024)
#define rgiga(size)                 (size * 1024 * 1024 * 1024)
}

