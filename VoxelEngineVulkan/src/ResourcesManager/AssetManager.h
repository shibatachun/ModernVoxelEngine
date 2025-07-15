#pragma once
#include "AssetCommonUtils.h"
#include "../../res/models/testVertexData.h"
#include "Shaders.h"
namespace asset {
	class AssetManager final {
	public:
		AssetManager();
		~AssetManager();
		const std::unordered_map<std::string, asset::shader>& getShaderAssets() { return _shadersManager->GetShaderAssets(); };
		bool Init();

	private:
		void InitShaderManager();
		
	private:
		std::unique_ptr<asset::ShadersManager> _shadersManager;
	
	};
}


