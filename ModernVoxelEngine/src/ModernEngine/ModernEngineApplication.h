#pragma once
#include "ModernEngineCommon.h"
class ModernEngineApplication : public Application, public Singleton<ModernEngineApplication>
{
	friend class Singleton<ModernEngineApplication>;
private:
	ModernEngineApplication(GLFWwindow* window, API api) : Application(window,api) {};
public:
	void Init() override;
	void InitRendererBackend(API api) override;
	void InitAssetManager() override;
	void Draw() override;
	void Resize() override;
	void Finish() override;
	

};

