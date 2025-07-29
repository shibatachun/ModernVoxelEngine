#pragma once
#include "AssetCommonUtils.h"
#include "../../res/models/testVertexData.h"
#include "Shaders.h"
#include "ModelManager.h"
namespace asset {
	class AssetManager final {
	public:
		bool Init();
		AssetManager();
		~AssetManager();
		const std::unordered_map<std::string, asset::shader>& getShaderAssets() const { return _shadersManager->GetShaderAssets(); };
		const std::unordered_map<std::string, ModelData>& getModelDatas() const { return _modelManager->GetModeDatas(); };
		const asset::shader& getShaderByName (std::string) const;
		

	private:
		void InitShaderManager();
		void InitModelManager();
		
	private:
		std::unique_ptr<asset::ShadersManager>	_shadersManager;
		std::unique_ptr<asset::ModelManager>	_modelManager;
	
	};
}


