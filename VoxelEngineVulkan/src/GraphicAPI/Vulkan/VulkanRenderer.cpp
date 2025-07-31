#include "VulkanRenderer.h"


std::vector<Vertex1> test_vertices = {
	{.pos = {-0.5f, -0.5f, 0.0f},	.color = {1.0f,0.0f,0.0f,1.0f}},
	{.pos = {0.5f, -0.5f, 0.0f},	.color = {0.0f,1.0f,0.0f,0.0f}},
	{.pos = {0.5f, 0.5f, 0.0f},		.color = {0.0f,0.0f,1.0f,1.0f}},
	{.pos = {-0.5f, 0.5f, 0.0f},	.color = {1.0f,1.0f,1.0f,1.0f}},

};
std::vector<uint16_t> indices = {
	0,1,2,2,3,0
};
bool vulkan::VulkanRenderer::Init()
{
	
	InitVulkan();
	
	return true;
	 
}

void vulkan::VulkanRenderer::DrawFrame()
{
	vkWaitForFences(_devices->Handle(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(_devices->Handle(), _swapchain->Handle(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	vkResetFences(_devices->Handle(), 1, &_inFlightFences[_currentFrame]);
	vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);
	updateUniformBuffer(_currentFrame);
	///////
	PipelineEntry entry;
	entry.pipeline = "test_triangle_vulkan";
	entry.layout = "default";
	VulkanRenderObject it = _resouceManager->GetRenderObject("test");
	recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex,it);
	/////
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];
	
	
	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	vkQueueSubmit(_devices->GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]);
	//Check(vkQueueSubmit(_devices->GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]), "Submit to graphics queue family");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { _swapchain->Handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	result = vkQueuePresentKHR(_devices->PresentQueue(), &presentInfo);
	vkDeviceWaitIdle(_devices->Handle());
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized){
		_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS){
		throw std::runtime_error("failed to present swap chain Image!");
	}

	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	
}

void vulkan::VulkanRenderer::Cleanup()
{
	
	_devices->WaitIdle();
	_swapchain->CleanUpSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(_devices->Handle(), _uniformBuffers[i], nullptr);
		vkFreeMemory(_devices->Handle(), _uniformBuffersMemory[i], nullptr);
	}
	_descriptorPools.reset();
	_descriptorLayouts.reset();

	_resouceManager.reset();
	vkDestroyBuffer(_devices->Handle(), _indexBuffer, nullptr);
	vkFreeMemory(_devices->Handle(), _indexBufferMemory, nullptr);

	vkDestroyBuffer(_devices->Handle(), _vertexBuffer, nullptr);
	vkFreeMemory(_devices->Handle(), _vertexBufferMemory, nullptr);

	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(_devices->Handle(), _renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(_devices->Handle(), _imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(_devices->Handle(), _inFlightFences[i], nullptr);
	}
	_renderPass->Destroy();
	_graphicsPipline.reset();
	_commandPools.reset();
	_devices.reset();
	_surface.reset();
#ifdef _DEBUG
	_debugMessenger.reset();
#endif // _DEBUG
	_instance.reset();
}

bool vulkan::VulkanRenderer::InitVulkan()
{
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
	SetPhysicalDevices();
	SetUpDescriptorLayoutManager();
	SetUpDescriptorPoolsManager();
	SetUpGraphicPipelineManager();
	SetUpCommandPools();
	SetUpBufferManager();
	SetUpVulkanResouceManager();

	CreateFrameBuffer();
	CreateCommandBuffer(QueueFamily::GRAPHIC);

	CreateSyncObjects();

	//createVertexBuffer();
	//CreateIndexBuffer();
	CreateUniformBuffers();
	ConfigureDescriptorSet();
	return true;
}

//Set up Devices
void vulkan::VulkanRenderer::SetPhysicalDevices()
{
	if (_devices)
	{
		throw std::logic_error("physical devices has already been set");
		return;
	}
	std::vector<const char*> requeredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	auto isDeviceSuitable = [&](VkPhysicalDevice device)->bool {
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader;
		};
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	
	for (VkPhysicalDevice device : _instance->PhysicalDevices())
	{
		if (isDeviceSuitable(device))
		{
			_physicalDevice = device;
			
		}
	}
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(_physicalDevice, &props);
	std::cout << "Using physical device : " << props.deviceName << std::endl;
	_devices.reset(new vulkan::Device(_physicalDevice, *_surface, requeredExtensions, nullptr));
	SetSwapChain();


}

//Set up SwapChain
void vulkan::VulkanRenderer::SetSwapChain()
{
	while (isMinimized())
	{
		glfwWaitEvents();
	}

	_swapchain.reset(new vulkan::SwapChain(*_devices, _presentMode));


}

//Create Pipeline
void vulkan::VulkanRenderer::SetUpGraphicPipelineManager()
{
	_renderPass.reset(new vulkan::RenderPass(*_swapchain));
	_graphicsPipline.reset(new vulkan::GraphicPipeline(_assetManager.getShaderAssets(), _devices->Handle(),*_swapchain,*_renderPass));
	LayoutConfig config;
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	config.bindings.push_back(uboLayoutBinding);
	_descriptorLayouts->CreateDescriptorSetLayout(config);
	//_graphicsPipline->CreateGraphicsPipeline("Triangle_Vulkan", _renderPass->GetRenderPass(), _descriptorLayouts->GetDescriptorSetLayout(config));
	_graphicsPipline->createPipelineLayout("default", _descriptorLayouts->GetDescriptorSetLayout(config));
	_graphicsPipline->CreateGraphicsPipeline("test_triangle_vulkan","default",_assetManager.getShaderByName("Rectangle_Vulkan"),_renderPass->GetRenderPass());
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,_descriptorLayouts->GetDescriptorSetLayout(config));
	_descriptorPools->CreatePreFrameDescriptorSets(layouts);
}

//Set up DescriptorLayoutManager
void vulkan::VulkanRenderer::SetUpDescriptorLayoutManager()
{
	_descriptorLayouts.reset(new DescriptorLayoutManager(*_devices));
}

//Set up DescriptorPoolsManger
void vulkan::VulkanRenderer::SetUpDescriptorPoolsManager()
{
	_descriptorPools.reset(new DescriptorPoolManager(*_devices));
}

void vulkan::VulkanRenderer::CreateFrameBuffer()
{
	_swapchain->CreateFrameBuffer(_renderPass->GetRenderPass());
	
}

void vulkan::VulkanRenderer::SetUpBufferManager()
{
	_bufferManager.reset(new BufferManager(*_devices, *_commandPools));
}

void vulkan::VulkanRenderer::SetUpVulkanResouceManager()
{
	_resouceManager.reset(new vulkan::VulkanResouceManager(*_bufferManager, _assetManager));
	_resouceManager->ConstructVulkanRenderObject("test", _graphicsPipline->GetGraphicsPipeline("test_triangle_vulkan"), _graphicsPipline->GetGraphicsPipelineLayout("default"), "test_data");
}

void vulkan::VulkanRenderer::SetUpCommandPools()
{
	if (!_commandPools)
	{
		_commandPools.reset(new vulkan::CommandPoolManager(*_devices));
	}
}

void vulkan::VulkanRenderer::CreateCommandBuffer(QueueFamily family)
{
	_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocaInfo{};
	allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocaInfo.commandPool = _commandPools->GetCommandPool(family);
	allocaInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocaInfo.commandBufferCount = (uint32_t)_commandBuffers.size();
	Check(vkAllocateCommandBuffers(_devices->Handle(), &allocaInfo, _commandBuffers.data()), "Allocate Command buffer!");
}

void vulkan::VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VulkanRenderObject object)
{
	
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	//Check(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Start record commandbuffer");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _renderPass->GetRenderPass();
	renderPassInfo.framebuffer =_swapchain->GetSwapChainBuffer()[imageIndex];
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = _swapchain->GetSwapchainExtent();
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f,1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline);
	VkBuffer vertexBuffers[] = { object.vertexBuffer }; 
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, object.indiceBuffer, 0, VK_INDEX_TYPE_UINT32);


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_swapchain->GetSwapchainExtent().width);
	viewport.height = static_cast<float>(_swapchain->GetSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _swapchain->GetSwapchainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		object.Pipelinelayout,
		0, 1,
		&_descriptorPools->GetHardCodedDescriptorSet()[_currentFrame],
		0, nullptr);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	vkCmdDrawIndexed(commandBuffer, object.indiceCounts[0], 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	vkEndCommandBuffer(commandBuffer);
	//Check(vkEndCommandBuffer(commandBuffer), "Record command buffer!");

}

//void vulkan::VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
//{
//	VkBufferCreateInfo bufferInfo{};
//	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	bufferInfo.size = size;
//	bufferInfo.usage = usage;
//	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
//	bufferInfo.queueFamilyIndexCount = 2;
//	uint32_t families[] = { _devices->GraphicsFamilyIndex(), _devices->TransferFamilyIndex() };
//	bufferInfo.pQueueFamilyIndices = families;
//	Check(vkCreateBuffer(_devices->Handle(), &bufferInfo, nullptr, &buffer), "Create buffer!");
//	VkMemoryRequirements memRequirements;
//	vkGetBufferMemoryRequirements(_devices->Handle(), buffer, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memRequirements.size;
//	allocInfo.memoryTypeIndex = vulkan::VulkanResource::findMemoryType(memRequirements.memoryTypeBits, properties,_devices->PhysicalDevice());
//
//	Check(vkAllocateMemory(_devices->Handle(), &allocInfo, nullptr, &bufferMemory), "AllocateMemory!");
//
//	vkBindBufferMemory(_devices->Handle(), buffer, bufferMemory, 0);
//}

//void vulkan::VulkanRenderer::CreateIndexBuffer()
//{
//	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//	_bufferManager->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//		bufferSize,
//		&stagingBuffer,
//		&stagingBufferMemory,
//		indices.data());
//	_bufferManager->createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//		bufferSize,
//		&_indexBuffer,
//		&_indexBufferMemory);
//	_bufferManager->copyBuffer(stagingBuffer, _indexBuffer, bufferSize, QueueFamily::TRANSFER);
//	vkDestroyBuffer(_devices->Handle(), stagingBuffer, nullptr);
//	vkFreeMemory(_devices->Handle(), stagingBufferMemory, nullptr);
//}

//void vulkan::VulkanRenderer::createVertexBuffer()
//{
//	VkDeviceSize bufferSize = sizeof(test_vertices[0]) * test_vertices.size();
//	//创建一个临时缓冲区，用于直接传输顶点数据至GPU
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//	_bufferManager->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//		bufferSize,
//		&stagingBuffer,
//		&stagingBufferMemory,
//		test_vertices.data());
//	_bufferManager->createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//		bufferSize,
//		&_vertexBuffer,
//		&_vertexBufferMemory);
//	_bufferManager->copyBuffer(stagingBuffer, _vertexBuffer, bufferSize, QueueFamily::TRANSFER);
//	vkDestroyBuffer(_devices->Handle(), stagingBuffer, nullptr);
//	vkFreeMemory(_devices->Handle(), stagingBufferMemory, nullptr);
//
//}

//void vulkan::VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, QueueFamily family)
//{
//	VkCommandBufferAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = _commandPools->GetCommandPool(family);
//	allocInfo.commandBufferCount = 1;
//
//	VkCommandBuffer commandBuffer;
//	vkAllocateCommandBuffers(_devices->Handle(), &allocInfo, &commandBuffer);
//	VkCommandBufferBeginInfo beginInfo{};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//	vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//	VkBufferCopy copyRegion{};
//	copyRegion.srcOffset = 0;
//	copyRegion.dstOffset = 0;
//	copyRegion.size = size;
//	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//	vkEndCommandBuffer(commandBuffer);
//
//	VkSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &commandBuffer;
//	vkQueueSubmit(_devices->TransferQueue(), 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(_devices->TransferQueue());
//	_commandPools->FreeCommandBuffer(family,1, commandBuffer);
//
//}

void vulkan::VulkanRenderer::CreateSyncObjects()
{
	_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		Check(vkCreateSemaphore(_devices->Handle(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]), "Create Image avaiable Semaphore");
		Check(vkCreateSemaphore(_devices->Handle(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]), "Create render finish Semaphore");
		Check(vkCreateFence(_devices->Handle(), &fenceInfo, nullptr, &_inFlightFences[i]), "Create fence");
	}

}

void vulkan::VulkanRenderer::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(_window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(_window, &width, &height);
		glfwWaitEvents();
	}

	_devices->WaitIdle();
	_swapchain->CleanUpSwapChain();
	_swapchain->CreateSwapChain(_presentMode);
	_swapchain->CreateFrameBuffer(_renderPass->GetRenderPass());
	
}


void vulkan::VulkanRenderer::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		_bufferManager->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize,
			&_uniformBuffers[i], &_uniformBuffersMemory[i]);
		//CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
		//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		//	_uniformBuffers[i], _uniformBuffersMemory[i]);

		vkMapMemory(_devices->Handle(), _uniformBuffersMemory[i], 0, bufferSize, 0, &_uniformBuffersMapped[i]);
	}
} 

void vulkan::VulkanRenderer::ConfigureDescriptorSet()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorPools->GetHardCodedDescriptorSet()[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;
		vkUpdateDescriptorSets(_devices->Handle(),1, &descriptorWrite, 0, nullptr);
	}
}

void vulkan::VulkanRenderer::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f),
		_swapchain->GetSwapchainExtent().width / (float)_swapchain->GetSwapchainExtent().height, 
		0.1f, 10.f);
	ubo.proj[1][1] *= -1;
	memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

bool vulkan::VulkanRenderer::isMinimized() const
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	return width==0 && height==0;
}

vulkan::VulkanRenderer::VulkanRenderer(GLFWwindow* window, VkPresentModeKHR presentmode, asset::AssetManager& assetManager) :_window(window), _presentMode(presentmode), _assetManager(assetManager)
{
	if (!Init())
	{
		std::cout << "Init vulkan Failed!" << std::endl;
	}
	
}
