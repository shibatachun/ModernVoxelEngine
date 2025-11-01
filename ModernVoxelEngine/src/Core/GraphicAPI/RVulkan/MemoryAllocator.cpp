#include "MemoryAllocator.h"

namespace rvulkan {

	static MemoryService		s_memory_service;
	static size_t s_size = rmega(32) + tlsf_size() + 8;
	MemoryService* MemoryService::instance() {
		return &s_memory_service;
	}

	void MemoryService::init(void* configuration)
	{
		MemoryServiceConfiguration* memory_configuration = static_cast<MemoryServiceConfiguration*>(configuration);
		system_allocator.init(memory_configuration ? memory_configuration->maximum_dynamic_size : s_size);
	}

	void MemoryService::test()
	{
	}

	HeapAllocator::~HeapAllocator()
	{
	}

	LinearAllocator::~LinearAllocator()
	{
	}

	void HeapAllocator::init(size_t size)
	{
	}

	void* HeapAllocator::allocate(size_t size, size_t alignment)
	{
		return nullptr;
	}

	void* HeapAllocator::allocate(size_t size, size_t alignment, cstring file, int32_t line)
	{
		return nullptr;
	}

	void* LinearAllocator::allocate(size_t size, size_t alignment)
	{
		return nullptr;
	}

	void* LinearAllocator::allocate(size_t size, size_t alignment, cstring file, int32_t line)
	{
		return nullptr;
	}

	void LinearAllocator::deallocate(void* pointer)
	{
	}

	void HeapAllocator::deallocate(void* pointer)
	{
	}

}
