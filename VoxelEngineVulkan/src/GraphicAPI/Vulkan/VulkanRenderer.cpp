#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
	SetPhysicalDevices();
	CreateGraphicPipeline();
	CreateFrameBuffer();
	CreateCommandPools();
	CreateCommandBuffer(QueueFamily::GRAPHIC);
	CreateSyncObjects();
	createVertexBuffer();
	CreateIndexBuffer();
	
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
	recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex,"Triangle_Vulkan");
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

	Check(vkQueueSubmit(_devices->GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]), "Submit to graphics queue family");

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
	VkPhysicalDeviceFeatures  deviceFeatures = {};

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
	_devices.reset(new vulkan::Device(_physicalDevice, *_surface, requeredExtensions, deviceFeatures, nullptr));
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
void vulkan::VulkanRenderer::CreateGraphicPipeline()
{
	_renderPass.reset(new vulkan::RenderPass(*_swapchain));
	_graphicsPipline.reset(new vulkan::GraphicPipeline(_assetManager.getShaderAssets(), _devices->Handle(),*_swapchain,*_renderPass));
}

void vulkan::VulkanRenderer::CreateFrameBuffer()
{
	_swapchain->CreateFrameBuffer(_renderPass->GetRenderPass());
	
}

void vulkan::VulkanRenderer::CreateCommandPools()
{
	if (!_commandPools)
	{
		_commandPools.reset(new vulkan::CommandPool(*_devices));
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


void vulkan::VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, std::string pipeline_name)
{
	if (!_graphicsPipline->GetGraphicsPipeline()[pipeline_name])
	{
		throw std::runtime_error("This pipeline is unaccessiable!");
	}
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
	Check(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Start record commandbuffer");

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
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipline->GetGraphicsPipeline()[pipeline_name]);
	VkBuffer vertexBuffers[] = { _vertexBuffer }; 
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

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

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	Check(vkEndCommandBuffer(commandBuffer), "Record command buffer!");

}

void vulkan::VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferInfo.queueFamilyIndexCount = 2;
	uint32_t families[] = { _devices->GraphicsFamilyIndex(), _devices->TransferFamilyIndex() };
	bufferInfo.pQueueFamilyIndices = families;
	Check(vkCreateBuffer(_devices->Handle(), &bufferInfo, nullptr, &buffer), "Create buffer!");
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_devices->Handle(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	Check(vkAllocateMemory(_devices->Handle(), &allocInfo, nullptr, &bufferMemory), "AllocateMemory!");

	vkBindBufferMemory(_devices->Handle(), buffer, bufferMemory, 0);
}

void vulkan::VulkanRenderer::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(_devices->Handle(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(_devices->Handle(), stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

	CopyBuffer(stagingBuffer, _indexBuffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(_devices->Handle(), stagingBuffer, nullptr);
	vkFreeMemory(_devices->Handle(), stagingBufferMemory, nullptr);
}

void vulkan::VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, QueueFamily family)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _commandPools->GetCommandPool(family);
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(_devices->Handle(), &allocInfo, &commandBuffer);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(_devices->TransferQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_devices->TransferQueue());
	_commandPools->FreeCommandBuffer(family,1, commandBuffer);

}

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

void vulkan::VulkanRenderer::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	//创建一个临时缓冲区，用于直接传输顶点数据至GPU
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(_devices->Handle(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(_devices->Handle(), stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		_vertexBuffer, _vertexBufferMemory);
	CopyBuffer(stagingBuffer, _vertexBuffer, bufferSize, QueueFamily::TRANSFER);
	vkDestroyBuffer(_devices->Handle(), stagingBuffer, nullptr);
	vkFreeMemory(_devices->Handle(), stagingBufferMemory, nullptr);

}

uint32_t vulkan::VulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_devices->PhysicalDevice(), &memProperties); 
	//待会瞅瞅看
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++){
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	
	throw std::runtime_error("failed to find suitable memory type!");
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
