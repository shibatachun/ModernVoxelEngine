#pragma once
#include "AssetCommonUtils.h"

namespace asset {


class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	void loadModel(const char* filename);
private:
	void processNode(aiNode* node, const aiScene* scene);
	
	
private:
	std::unordered_map<std::string, ModelData>						_model;
	

};
}

