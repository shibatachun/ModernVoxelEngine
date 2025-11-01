#include "RVulkanRenderer.h"
namespace rvulkan {
	void init() {
		MemoryServiceConfiguration	memory_configuration;
		memory_configuration.maximum_dynamic_size = rgiga(2ull);

		MemoryService::instance()->init(&memory_configuration);
		Allocator* allocator = &MemoryService::instance()->system_allocator;


	}
	void shutdown() {

	}
}
