#include "ModelManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

asset::ModelManager::ModelManager()
{
	loadAllModel();
	loadAllImage();
	loadModel("res/models/generator_LP.fbx");
	loadImage("test","res/textures/text_texture.jpg");
	loadImage("viking", "res/textures/viking_room.png");
	//loadTestExample();
	loadobj("res/models/viking_room.obj");
	loadgltf("res/models/sponza/sponza.gltf");
	//CollectTexturePaths("res/models/sponza/sponza.gltf");
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
	std::vector<FileInfo> allTextureFiles;
	IterateAllDirectories("res/textures", { "png","jpg","ktx" }, allTextureFiles);
	for (auto& x : allTextureFiles) {
		_imageFilesInfo.emplace(x.GetFileNameWithExtension(), x);
	}
	
}

void asset::ModelManager::loadAllModel()
{
	for (auto& x : IterateDirectory("res/models", { "obj","fbx","gltf"})) {
		_modelFilesInfo.emplace(x.name, x);
	}
}

void asset::ModelManager::loadImage(std::string filename, std::string path, bool isktx)
{

	Image image;

	if (isktx) {
		ktxTexture* kt;
		ktxResult result = KTX_SUCCESS;
		result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kt);
		assert(result == KTX_SUCCESS);
		image.texWidth = kt->baseWidth;
		image.texHeight = kt->baseHeight;
		image.mipLevels = kt->numLevels;
		image.subresource.resize(image.mipLevels);
		image.pixel = ktxTexture_GetData(kt);
		image.size = ktxTexture_GetDataSize(kt);
		image.isKtx = true;
		image.name = filename;
		image.format = FromVk(ktxTexture_GetVkFormat(kt));

		for (uint32_t i = 0; i < image.mipLevels; i++) {
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(kt, i, 0, 0, &offset);
			assert(result == KTX_SUCCESS);
			SubResource& subResouces = image.subresource[i];
			subResouces.offset = offset;
			subResouces.depth = 1;
			subResouces.width = std::max(1u, kt->baseWidth >> i);
			subResouces.height = std::max(1u, kt->baseHeight >> i);
			subResouces.layer = 0;
		}
		_ImageFile.emplace(filename, image);
		
	}
	else{

		image.pixel = stbi_load(path.c_str(), &image.texWidth, &image.texHeight, &image.texChannels, STBI_rgb_alpha);
		_ImageFile.emplace(filename, image);
	}
	
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

	{.pos = {-0.5f, -0.5f, -0.5f},	.color = {1.0f,0.0f,0.0f,1.0f}, .uv = {1.0f, 0.0f}},
	{.pos = {0.5f, -0.5f, -0.5f},	.color = {0.0f,1.0f,0.0f,0.0f}, .uv = {0.0f,0.0f}},
	{.pos = {0.5f, 0.5f, -0.5f},		.color = {0.0f,0.0f,1.0f,1.0f}, .uv = {0.0f,1.0f}},
	{.pos = {-0.5f, 0.5f, -0.5f},	.color = {1.0f,1.0f,1.0f,1.0f},	.uv = {1.0f, 1.0f}},
	};
	std::vector<uint16_t> indices = {
		0,1,2,2,3,0,4,5,6,6,7,4
	};
	//push back用于完整的对象，如果容器内的构造的
	meshData.vertexCount = static_cast<uint32_t>(test_vertices.size());
	meshData.indexCount = static_cast<uint32_t>(indices.size());
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

void asset::ModelManager::loadobj(std::string filePath)
{
	ModelData data;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, 
		aiProcess_Triangulate |            // 和 tinyobj 的自动三角化等价
		aiProcess_JoinIdenticalVertices |  // 合并完全相同的顶点
		aiProcess_GenSmoothNormals |       // 如无法线则生成
		aiProcess_CalcTangentSpace |       // 如需要切线
		aiProcess_ImproveCacheLocality |
		aiProcess_FlipUVs |
		aiProcess_SortByPType);
	if (!scene) {
		std::cout << "LoadAndExportCustomFormat() failed to loaded model" << filePath << "\n";
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}
	data.meshCount = scene->mNumMeshes;
	data.meshes.resize(data.meshCount);
	
	for (int i = 0; i < scene->mNumMeshes; i++) {
		MeshData& meshData = data.meshes[i];
		meshData.vertexCount = scene->mMeshes[i]->mNumVertices;
		meshData.indexCount = scene->mMeshes[i]->mNumFaces * 3;
		meshData.name = scene->mMeshes[i]->mName.C_Str();
		meshData.vertices.resize(meshData.vertexCount);
		meshData.indices.resize(meshData.indexCount);
		meshData.name = meshData.name.substr(0, meshData.name.find('.'));

	}
	for (int i = 0; i < scene->mNumMeshes; i++) {
		MeshData& meshdata = data.meshes[i];
		const aiMesh* mesh = scene->mMeshes[i];
		for (int j = 0; j < mesh->mNumVertices; j++) {
			meshdata.vertices[j] = Vertex1{
			.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y,mesh->mVertices[j].z),
			.color = glm::vec4(1.0f, 1.0f, 1.0f,1.0f),
			.uv = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y) : glm::vec2(0.0f,0.0f),
			};
		}
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			unsigned int baseIndex = j * 3;
			meshdata.indices[baseIndex] = face.mIndices[0];
			meshdata.indices[baseIndex + 1] = face.mIndices[1];
			meshdata.indices[baseIndex + 2] = face.mIndices[2];

		}
	}
	_model.emplace("viking_room", data);


}

std::vector<TexturePath> asset::ModelManager::CollectTexturePaths(std::string gltfPath) {
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(
		gltfPath,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene) {
		std::cerr << "Assimp error: " << imp.GetErrorString() << "\n";
		return {};
	}

	// glTF2 常用贴图类型（注意：LIGHTMAP 在 assimp 里对应 glTF 的 occlusion）
	static const aiTextureType kTypes[] = {
		aiTextureType_BASE_COLOR,
		aiTextureType_NORMALS,
		aiTextureType_METALNESS,
		aiTextureType_DIFFUSE_ROUGHNESS,
		aiTextureType_LIGHTMAP,   // occlusion
		aiTextureType_EMISSIVE,
		// 兜底（有些资源/旧版会落到这些槽位）
		aiTextureType_DIFFUSE,
		aiTextureType_SPECULAR
	};

	std::vector<TexturePath> out;
	std::unordered_set<std::string> dedup; // 去重

	const auto baseDir = std::filesystem::absolute(gltfPath).parent_path();

	for (unsigned mi = 0; mi < scene->mNumMaterials; ++mi) {
		aiMaterial* m = scene->mMaterials[mi];
		for (aiTextureType tt : kTypes) {
			const unsigned cnt = m->GetTextureCount(tt);
			for (unsigned ti = 0; ti < cnt; ++ti) {
				aiString p;
				m->GetTexture(tt, ti, &p);
				const char* s = p.C_Str();
				if (!s || !*s) continue;
				if (s[0] == '*') continue; // 内嵌纹理，既然你只要路径，这里直接跳过

				std::filesystem::path abs = baseDir / s;
				abs = abs.lexically_normal();

				if (dedup.insert(abs.string()).second) {
					out.push_back({ abs.string(), tt, mi, ti });
				}
			}
		}
	}
	return out;

}

void asset::ModelManager::loadgltf(std::string filename)
{                     
	ModelData data;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate |            // 和 tinyobj 的自动三角化等价
		aiProcess_JoinIdenticalVertices |  // 合并完全相同的顶点
		aiProcess_GenSmoothNormals |       // 如无法线则生成
		aiProcess_CalcTangentSpace |       // 如需要切线
		aiProcess_ImproveCacheLocality |
		aiProcess_FlipUVs |
		aiProcess_SortByPType);

	if (!scene) {
		std::cout << "LoadAndExportCustomFormat() failed to loaded model" << filename << "\n";
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}

	// glTF2 常用贴图类型（注意：LIGHTMAP 在 assimp 里对应 glTF 的 occlusion）
	static const aiTextureType kTypes[] = {
		aiTextureType_BASE_COLOR,
		aiTextureType_NORMALS,
		aiTextureType_METALNESS,
		aiTextureType_DIFFUSE_ROUGHNESS,
		aiTextureType_LIGHTMAP,   // occlusion
		aiTextureType_EMISSIVE,
		aiTextureType_AMBIENT_OCCLUSION,
		aiTextureType_GLTF_METALLIC_ROUGHNESS,
		
	};

	std::unordered_set<std::string> dedup_textures;
	data.meshCount = scene->mNumMeshes;
	data.materialCount = scene->mNumMaterials;
	data.materials.resize(data.materialCount);
	for (int i = 0; i < data.materialCount; i++) {
		aiMaterial* material = scene->mMaterials[i];
		Material& mat = data.materials[i];
		float f;
		//获得Base color factor
		if (material->Get(AI_MATKEY_BASE_COLOR, f) == AI_SUCCESS) mat.baseColorFactor = f;
		//获得Roughness factor
		if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, f) == AI_SUCCESS) mat.roughnessFactor = f;
		//获得Metallic factor
		if (material->Get(AI_MATKEY_METALLIC_FACTOR, f) == AI_SUCCESS) mat.matallicFactor = f;

		{
			aiString s;
			if (material->Get(AI_MATKEY_GLTF_ALPHAMODE, s) == AI_SUCCESS) {
				if (s.C_Str() == "BLEND") {
					mat.alphaMode = Material::ALPHAMODE_BLEND;
				}
				if (s.C_Str() == "MASK") {
					mat.alphaMode = Material::ALPHAMODE_MASK;
				}
			}
		}
		material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, mat.alphaCutoff);

		for (aiTextureType texture_type: kTypes) {
	
			aiString p;
			//TODO，这里可以获取更多的信息
			if (material->GetTexture(texture_type, 0, &p) != AI_SUCCESS) {
				continue;
			}
			
			const char* s = p.C_Str();
			
			if (!s || !*s) continue;
			if (s[0] == '*') continue; // 内嵌纹理，既然你只要路径，这里直接跳过

			switch (texture_type)
			{
			case	aiTextureType_BASE_COLOR:
				mat.baseColorTexture = std::string(s);
				break;
			case	aiTextureType_NORMALS:
				mat.normalTexture = std::string(s);
				break;
			case	aiTextureType_GLTF_METALLIC_ROUGHNESS:
				mat.matallicRoughnessTexture = std::string(s);
				break;
			case	aiTextureType_DIFFUSE:
				mat.diffuseTexture = std::string(s);
				break;	
			case	aiTextureType_EMISSIVE:
				mat.emissiveTexture = std::string(s);
				break;
			case	aiTextureType_SPECULAR:
				mat.specularGlossinessTexture = std::string(s);
				break;
			case	aiTextureType_AMBIENT_OCCLUSION:
				mat.occlusionTexture = std::string(s);
			default:
				break;
			}
			dedup_textures.insert(s);
				
			
		}
	}

	for (const auto& texture_name : dedup_textures) {
		//在目录下所有的texture file中储存对应的信息
		bool isktx = false;
		auto& tx = utils::findInMap(_imageFilesInfo,texture_name);
		if (tx.ext == "ktx") {
			isktx = true;

		}
		//加载image
		loadImage(tx.name,tx.path,isktx);
	}
	





}


