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
	const std::unordered_map<std::string, Image>& GetImageDatas() { return _ImageFile; };
	const void freeImage(unsigned char* pixel);
private:
	void processNode(aiNode* node, const aiScene* scene);
	void loadAllImage();
	void loadAllModel();
	void loadImage(std::string filename, std::string path, bool isktx = false);
	void loadTestExample();
	void loadobj(std::string filePath);
	void loadgltf(std::string filename);
	void loadNode(aiNode* scene, std::vector<Vertex1>& vertexbuffer, std::vector<uint32_t>& indicebuffer);

private:
	std::unordered_map<std::string, ModelData>						_model;
	std::unordered_map<std::string, Image>							_ImageFile;

	std::unordered_map<std::string, FileInfo>						_modelFilesInfo;
	std::unordered_map<std::string, FileInfo>						_imageFilesInfo;
	/*std::unordered_map<FileInfo, */

};
}

