#include "VulkanRenderer.h"

bool vulkan::VulkanRenderer::Init()
{
	
	_instance.reset(new vulkan::Instance(_window));
	_surface.reset(new vulkan::Surface(*_instance));
	_debugMessenger.reset(new vulkan::DebugUtilsMessenger(*_instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT));
	SetPhysicalDevices();
	CreateGraphicPipeline();
	CreateFrameBuffer();
	
	return true;
	 
}

void vulkan::VulkanRenderer::DrawFrame()
{
	std::cout << "yo, wasup" << std::endl;
}

void vulkan::VulkanRenderer::Cleanup()
{
	_devices->WaitIdle();
	for (auto framebuffer : _swapChainFramebuffers) {
		vkDestroyFramebuffer(_devices->Handle(), framebuffer, nullptr);
	}
	_renderPass->Destroy();
	_swapchain.reset();
	_graphicsPipline.reset();
	vkDestroyCommandPool(_devices->Handle(), _commandPool, nullptr);
	_devices.reset();
	_surface.reset();
#ifdef _DEBUG
	_debugMessenger.reset();
#endif // _DEBUG
	
	_instance.reset();
}

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

void vulkan::VulkanRenderer::SetSwapChain()
{
	while (isMinimized())
	{
		glfwWaitEvents();
	}

	_swapchain.reset(new vulkan::SwapChain(*_devices, _presentMode));


}

void vulkan::VulkanRenderer::CreateGraphicPipeline()
{
	_renderPass.reset(new vulkan::RenderPass(*_swapchain));
	_graphicsPipline.reset(new vulkan::GraphicPipeline(_assetManager.getShaderAssets(), _devices->Handle(),*_swapchain,*_renderPass));
}

void vulkan::VulkanRenderer::CreateFrameBuffer()
{
	_swapChainFramebuffers.resize(_swapchain->GetImageViews().size());
	auto ImageViewSize = _swapchain->GetImageViews().size();
	for (size_t i = 0; i < ImageViewSize; i++)
	{
		VkImageView attachments[] = {
			_swapchain->GetImageViews()[i]
		};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass->GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _swapchain->GetSwapchainExtent().width;
		framebufferInfo.height = _swapchain->GetSwapchainExtent().height;
		framebufferInfo.layers = 1;
		Check(vkCreateFramebuffer(_devices->Handle(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]), "Create framebuffer!");

	}
	
}

void vulkan::VulkanRenderer::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = _devices->GraphicsFamilyIndex();
	Check(vkCreateCommandPool(_devices->Handle(), &poolInfo, nullptr, &_commandPool), "Create Command pool");
}

void vulkan::VulkanRenderer::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocaInfo{};
	allocaInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocaInfo.commandPool = _commandPool;
	allocaInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocaInfo.commandBufferCount = 1;
	Check(vkAllocateCommandBuffers(_devices->Handle(), &allocaInfo, &_commandBuffer), "Allocate Command buffer!");
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
	Check(vkBeginCommandBuffer(_commandBuffer, &beginInfo), "Start record commandbuffer");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _renderPass->GetRenderPass();
	renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = _swapchain->GetSwapchainExtent();
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f,1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipline->GetGraphicsPipeline()[pipeline_name]);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_swapchain->GetSwapchainExtent().width);
	viewport.height = static_cast<float>(_swapchain->GetSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _swapchain->GetSwapchainExtent();
	vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

	vkCmdDraw(_commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(_commandBuffer);

	Check(vkEndCommandBuffer(_commandBuffer), "Record command buffer!");

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
