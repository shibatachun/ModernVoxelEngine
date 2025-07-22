#include "ModelManager.h"

asset::ModelManager::ModelManager()
{
	loadModel("res/models/generator_LP.fbx");
}

asset::ModelManager::~ModelManager()
{
}

void asset::ModelManager::loadModel(const char* filename)
{
	const aiScene* scene = aiImportFile(filename, aiProcess_Triangulate);
}


