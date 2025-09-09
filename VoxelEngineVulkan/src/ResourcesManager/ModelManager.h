#pragma once
#include "AssetCommonUtils.h"


namespace asset {



class ModelManager
{

public:
	ModelManager();
	~ModelManager();
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

	//test function
	void loadgltf_test(std::string filename, std::string path);
	void loadImage_test(tinygltf::Model& gltfModel, ModelData& model);
	void loadMaterial_test(tinygltf::Model& gltfMode, ModelData& model);
	void loadNode_test(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& tingymodel, ModelData& model, float globalscale);
	bool loadImageDataFunc(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData);
	bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData);


private:
	std::unordered_map<std::string, ModelData>						_model;
	std::unordered_map<std::string, Image>							_ImageFile;

	std::unordered_map<std::string, FileInfo>						_modelFilesInfo;
	std::unordered_map<std::string, FileInfo>						_imageFilesInfo;
	/*std::unordered_map<FileInfo, */

};
}

