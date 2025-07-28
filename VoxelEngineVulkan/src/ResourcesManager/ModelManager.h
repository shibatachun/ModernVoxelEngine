#pragma once
#include "AssetCommonUtils.h"
#define STB_IMAGE_IMPLEMENTATION
namespace asset {


class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	void loadModel(const char* filename);
	const std::unordered_map<std::string, ModelData>& GetModeDatas() { return _model; };
private:
	void processNode(aiNode* node, const aiScene* scene);
	void loadImage(std::string filename);
	
	
private:
	std::unordered_map<std::string, ModelData>						_model;
	/*std::unordered_map<FileInfo, */

};
}

