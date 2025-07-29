#include "AssetManager.h"

asset::AssetManager::AssetManager(){
}

asset::AssetManager::~AssetManager(){
}

const asset::shader& asset::AssetManager::getShaderByName(std::string name) const
{
	auto it = _shadersManager->GetShaderAssets().find(name);
	if (it == _shadersManager->GetShaderAssets().end()) {
		throw std::runtime_error("unable find the shader :" + name);
	}
	return it->second;
}

bool asset::AssetManager::Init(){
	std::filesystem::path cwd = std::filesystem::current_path();
	std::cout << "AssetManager当前工作目录" << cwd.string() << std::endl;
	
	InitModelManager();
	InitShaderManager();
	
	return true;
}

void asset::AssetManager::InitShaderManager()
{
	_shadersManager.reset(new asset::ShadersManager("res/shaders"));
}

void asset::AssetManager::InitModelManager()
{
	_modelManager.reset(new asset::ModelManager());
}


