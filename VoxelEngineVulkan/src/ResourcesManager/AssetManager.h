#pragma once
#include "AssetCommonUtils.h"
#include "../../res/models/testVertexData.h"
#include "Shaders.h"
#include "ModelManager.h"
namespace asset {
	class AssetManager final {
	public:
		AssetManager();
		~AssetManager();
		const std::unordered_map<std::string, asset::shader>& getShaderAssets() { return _shadersManager->GetShaderAssets(); };
		bool Init();

	private:
		void InitShaderManager();
		void InitModelManager();
		
	private:
		std::unique_ptr<asset::ShadersManager>	_shadersManager;
		std::unique_ptr<asset::ModelManager>	_modelManager;
	
	};
}


