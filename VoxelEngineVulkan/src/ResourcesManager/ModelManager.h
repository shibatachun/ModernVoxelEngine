#pragma once
#include "AssetCommonUtils.h"

namespace asset {

	struct Model
	{

	};
class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	void loadModel(const char* filename);
private:
	std::unordered_map<std::string, Model>						_model;

};
}

