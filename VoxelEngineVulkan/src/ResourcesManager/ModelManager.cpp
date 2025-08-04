#include "ModelManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

asset::ModelManager::ModelManager()
{
	loadAllModel();
	loadAllImage();
	loadModel("res/models/generator_LP.fbx");
	loadImage("test","text_texture.jpg");
	loadTestExample();
}

asset::ModelManager::~ModelManager()
{
}

void asset::ModelManager::loadModel(const char* filename)
{
	ModelData modelData;
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | 
												aiProcess_JoinIdenticalVertices |
												aiProcess_ImproveCacheLocality |
												aiProcess_RemoveRedundantMaterials|
												aiProcess_FlipUVs);
	if (!scene) {
		std::cout << "LoadAndExportCustomFormat() failed to loaded model" << filename << "\n";
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}
	modelData.name = GetFileName(filename);
	modelData.meshCount = scene->mNumMeshes;
	modelData.meshes.resize(modelData.meshCount);
	modelData.timestamp = GetLastModifiedTime(filename);

	for (int i = 0; i < modelData.meshes.size(); i++) {
		MeshData& meshData = modelData.meshes[i];
		meshData.vertexCount = scene->mMeshes[i]->mNumVertices;
		meshData.indexCount = scene->mMeshes[i]->mNumFaces * 3;
		meshData.name = scene->mMeshes[i]->mName.C_Str();
		meshData.vertices.resize(meshData.vertexCount);
		meshData.indices.resize(meshData.indexCount);

		meshData.name = meshData.name.substr(0, meshData.name.find('.'));
	}
	for (int i = 0; i < modelData.meshes.size(); i++) {
		MeshData& meshData = modelData.meshes[i];
		const aiMesh* assimpMesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < meshData.vertexCount; j++) {
			meshData.vertices[j] = (Vertex1{
				//位置
				.pos = glm::vec3(assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z),
		
				//UV坐标
				.uv = assimpMesh->HasTextureCoords(0) ? glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f,0.0f),
				//N向量
				.normal = glm::vec3(assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z),
				//Tangent
				.tangent = assimpMesh->HasTangentsAndBitangents() ? glm::vec3(assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z) : glm::vec3(0.0f) }
				);
			meshData.aabbMin = utils::math::VecMin(meshData.vertices[j].pos, meshData.aabbMin);
			meshData.aabbMax = utils::math::VecMax(meshData.vertices[j].pos, meshData.aabbMax);
		}
		for (unsigned int j = 0; j < assimpMesh->mNumFaces; j++) {
			const aiFace& face = assimpMesh->mFaces[j];
			unsigned int baseIndex = j * 3;
			meshData.indices[baseIndex] = face.mIndices[0];
			meshData.indices[baseIndex + 1] = face.mIndices[1];
			meshData.indices[baseIndex + 2] = face.mIndices[2];

		}
		for (Vertex1& vertex : meshData.vertices)
		{
			vertex.normal = glm::normalize(vertex.normal);
		}
		// Generate Tangents
		for (int i = 0; i < meshData.indices.size(); i += 3) {
			Vertex1* vert0 = &meshData.vertices[meshData.indices[i]];
			Vertex1* vert1 = &meshData.vertices[meshData.indices[i + 1]];
			Vertex1* vert2 = &meshData.vertices[meshData.indices[i + 2]];
			glm::vec3 deltaPos1 = vert1->pos - vert0->pos;
			glm::vec3 deltaPos2 = vert2->pos - vert0->pos;
			glm::vec2 deltaUV1 = vert1->uv - vert0->uv;
			glm::vec2 deltaUV2 = vert2->uv - vert0->uv;
			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
			vert0->tangent = tangent;
			vert1->tangent = tangent;
			vert2->tangent = tangent;
		}
		modelData.aabbMin = utils::math::VecMin(modelData.aabbMin, meshData.aabbMin);
		modelData.aabbMax = utils::math::VecMax(modelData.aabbMax, meshData.aabbMax);
	}
	importer.FreeScene();

	_model.emplace(modelData.name, modelData);

}

void asset::ModelManager::processNode(aiNode* node, const aiScene* scene)
{
}

void asset::ModelManager::loadAllImage()
{
	for (auto& x : IterateDirectory("res/textures", { "jpg" })) {
		_imageFilesInfo.emplace(x.GetFileNameWithExtension(), x);
	}
	
}

void asset::ModelManager::loadAllModel()
{
	for (auto& x : IterateDirectory("res/models", { "obj","fbx","gltf" })) {
		_modelFilesInfo.emplace(x.name, x);
	}
}

void asset::ModelManager::loadImage(std::string filename, std::string path)
{

	auto& file = utils::findInMap(_imageFilesInfo, path);
	Image image;
	int texWidth, texHeight, texChannels;
	image.pixel = stbi_load(file.path.c_str(), &image.texWidth, &image.texHeight, &image.texChannels, STBI_rgb_alpha);
	_ImageFile.emplace(filename, image);
	
}

const void asset::ModelManager::freeImage(unsigned char* pixel)
{
	stbi_image_free(pixel);
}

void asset::ModelManager::loadTestExample()
{
	ModelData data;
	data.meshCount = 1;
	data.meshes.resize(data.meshCount);
	MeshData& meshData = data.meshes[0];
	std::vector<Vertex1> test_vertices = {
	{.pos = {-0.5f, -0.5f, 0.0f},	.color = {1.0f,0.0f,0.0f,1.0f}, .uv = {1.0f, 0.0f}},
	{.pos = {0.5f, -0.5f, 0.0f},	.color = {0.0f,1.0f,0.0f,0.0f}, .uv = {0.0f,0.0f}},
	{.pos = {0.5f, 0.5f, 0.0f},		.color = {0.0f,0.0f,1.0f,1.0f}, .uv = {0.0f,1.0f}},
	{.pos = {-0.5f, 0.5f, 0.0f},	.color = {1.0f,1.0f,1.0f,1.0f},	.uv = {1.0f, 1.0f}},
	};
	std::vector<uint16_t> indices = {
		0,1,2,2,3,0
	};
	//push back用于完整的对象，如果容器内的构造的
	meshData.vertexCount = test_vertices.size();
	meshData.indexCount = indices.size();
	//什么时候用reserve，在后面要用push_back加入元素可以减去多次预分配内存使用时间，用reserve事先获得需要的内存，但是reserve也不混直接[i]复制，因为元素的数量没变，需要用resize
	meshData.vertices.reserve(meshData.vertexCount);
	meshData.indices.reserve(meshData.indexCount);
	//为什么用auto &, 因为直接拿引用避免一次拷贝，不拿的话会再auto x阶段又进行一次构造拷贝，push back又进行一次，会增加开销
	for (const auto& x : test_vertices) {
		meshData.vertices.push_back(x);
	}
	for (auto x : indices) {
		meshData.indices.push_back(x);
	}

		
	_model.emplace("test_data", data);

}


