#pragma once
#include "AssetCommonUtils.h"

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
	
	
private:
	std::unordered_map<std::string, ModelData>						_model;
	

};
}

