#include "AssetManager.h"

asset::AssetManager::AssetManager(){
}

asset::AssetManager::~AssetManager(){
}

bool asset::AssetManager::Init(){
	std::filesystem::path cwd = std::filesystem::current_path();
	std::cout << "AssetManager当前工作目录" << cwd.string() << std::endl;
	InitShaderManager();
	
	return true;
}

void asset::AssetManager::InitShaderManager()
{
	_shadersManager.reset(new asset::ShadersManager("res/shaders"));
}


