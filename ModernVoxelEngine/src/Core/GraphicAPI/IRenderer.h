#pragma once

enum class API
{
	VULKAN,
	OPENGL,
	DIRECTX,
	RVULKAN
};

class IRenderer {
public:
	
	virtual ~IRenderer() = default;
	virtual bool Init() = 0;
	virtual void DrawFrame() = 0;
	virtual void Cleanup() = 0;
	virtual API getAPI() const = 0;
};