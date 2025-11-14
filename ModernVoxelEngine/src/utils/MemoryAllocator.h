#pragma once

#include <stdint.h>
#include "StackWalker.h"
#include "src/CStyleExternal/tlsf.h"
namespace memory {
    typedef const char* cstring;
    struct MemoryServiceConfiguration {

        size_t                       maximum_dynamic_size = 32 * 1024 * 1024;    // Defaults to max 32MB of dynamic memory.

    }; // struct MemoryServiceConfiguration

    struct Allocator {
        virtual ~Allocator() {}
        virtual void* allocate(size_t size, size_t alignment) = 0;
        virtual void* allocate(size_t size, size_t alignment, cstring file, int32_t line) = 0;

        virtual void  deallocate(void* pointer) = 0;
    }; // struct Alloc

    struct HeapAllocator : public Allocator {

        ~HeapAllocator() override;

        void init(size_t size);
        void shutdown();
        void* allocate(size_t size, size_t alignment) override;
        void* allocate(size_t size, size_t alignment, cstring file, int32_t line) override;
        void deallocate(void* pointer) override;
        void* tlsf_handle;
        void* memory;
        size_t allocated_size = 0;
        size_t max_size = 0;
    };
    struct LinearAllocator : public Allocator {

        ~LinearAllocator();

        void init(size_t size);
        void shutdown();
        void* allocate(size_t size, size_t alignment) override;
        void* allocate(size_t size, size_t alignment, cstring file, int32_t line) override;
        void deallocate(void* pointer) override;
        void clear();

        uint8_t* memory = nullptr;
        size_t                       total_size = 0;
        size_t                       allocated_size = 0;
    }; // struct LinearAllocator

    struct StackAllocator : public Allocator {
        void    init(size_t size);
        void shutdown();
        void* allocate(size_t size, size_t alignment) override;
        void* allocate(size_t size, size_t alignment, cstring file, int32_t line) override;
        void deallocate(void* pointer) override;
        size_t get_marker();
        void free_marker(size_t marker);

        void clear();

        uint8_t* memory = nullptr;
        size_t  total_size = 0;
        size_t  allocated_size = 0;



    };
    struct MemoryService {
        static MemoryService* instance();
        void init(void* configuration);
        void shutdown();

        LinearAllocator             scratch_allocator;
        HeapAllocator               system_allocator;

        void test();

        static constexpr   cstring      k_name = "memeory_service";

        
    };
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

