#include "AssetManager.h"

asset::AssetManager::AssetManager(){
}

asset::AssetManager::~AssetManager(){
}

const asset::shader& asset::AssetManager::getShaderByName(std::string name) const
{
	return utils::findInMap(_shadersManager->GetShaderAssets(), name);
}

const ModelData& asset::AssetManager::getModelDataByName(std::string name) const
{
	return _modelManager->GetModelDataByName(name);
}

const ModelData& asset::AssetManager::getRequireModelData(std::string)
{
	// TODO: insert return statement here
	return ModelData{};
}

const Image& asset::AssetManager::getImageDataByName(std::string name) const
{
	return _modelManager->GetImageDataByName(name);
}

const Image& asset::AssetManager::getRequrieImageData(std::string) const
{
	// TODO: insert return statement here
	return Image{};
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


