#include "RVulkanRenderer.h"
namespace rvulkan {
	void init() {
		using namespace memory;
		MemoryServiceConfiguration	memory_configuration;
		memory_configuration.maximum_dynamic_size = rgiga(2ull);

		MemoryService::instance()->init(&memory_configuration);
		memory::Allocator* allocator = &MemoryService::instance()->system_allocator;

		StackAllocator scratch_allocator;
		scratch_allocator.init(rmega(8));

		enki::TaskSchedulerConfig config;
		config.numTaskThreadsToCreate += 1;
		enki::TaskScheduler task_scheduler;


	}
	void shutdown() {

	}
}
