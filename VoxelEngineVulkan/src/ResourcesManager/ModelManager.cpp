#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "ModelManager.h"


asset::ModelManager::ModelManager()
{
	loadAllModel();
	loadAllImage();
	loadModel("res/models/generator_LP.fbx");
	loadImage("test","res/textures/text_texture.jpg");
	loadImage("viking", "res/textures/viking_room.png");
	//loadTestExample();
	loadobj("res/models/viking_room.obj");
	//loadgltf("res/models/Zzz/zzz_alice/alice.pmx");
	//loadgltf("res/models/sponza/sponza.gltf");
	loadgltf_test("res/models/sponza/sponza.gltf");
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

		for (int i = 0; i < image.mipLevels; i++) {
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
	
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		MeshData& meshData = data.meshes[i];
		meshData.vertexCount = scene->mMeshes[i]->mNumVertices;
		meshData.indexCount = scene->mMeshes[i]->mNumFaces * 3;
		meshData.name = scene->mMeshes[i]->mName.C_Str();
		meshData.vertices.resize(meshData.vertexCount);
		meshData.indices.resize(meshData.indexCount);
		meshData.name = meshData.name.substr(0, meshData.name.find('.'));

	}
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		MeshData& meshdata = data.meshes[i];
		const aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
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
	data.meshes.resize(data.meshCount);
	for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* meshdata = scene->mMeshes[i];
		MeshData& mesh = data.meshes[i];
		mesh.aabbMin = glm::vec3(std::numeric_limits<float>::max());
		mesh.aabbMax = glm::vec3(-std::numeric_limits<float>::max());
		mesh.vertexCount = meshdata->mNumVertices;
		mesh.indexCount = meshdata->mNumFaces * 3;
		mesh.vertices.reserve(mesh.vertexCount);
		mesh.indices.reserve(mesh.indexCount);
		
	}
	
	data.materialCount = scene->mNumMaterials;
	data.materials.resize(data.materialCount);
	for (uint32_t i = 0; i < data.materialCount; i++) {
		aiMaterial* material = scene->mMaterials[i];
		Material& mat = data.materials[i];
		aiColor4D base = { 1,1,1,1 };
		float roughness = 1.0f;
		float metallic = 1.0f;
		//获得Base color factor
		if (material->Get(AI_MATKEY_BASE_COLOR, base) == AI_SUCCESS) mat.baseColorFactor = {base.r, base.g, base.b, base.a};
		//获得Roughness factor
		if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) mat.roughnessFactor = roughness;
		//获得Metallic factor
		if (material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) mat.matallicFactor = metallic = 1.0f;
		;

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

void asset::ModelManager::loadNode(aiNode* scene, std::vector<Vertex1>& vertexbuffer, std::vector<uint32_t>& indicebuffer)
{
	
}

 
//Tutorial test functions
void asset::ModelManager::loadgltf_test(std::string filename)
{
	ModelData model;
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	gltfContext.SetImageLoader(tinygltf::LoadImageData, nullptr);
	std::string error, warning;
	bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);
	if (fileLoaded) {
		
		loadImage_test(gltfModel, model);
		loadMaterial_test(gltfModel, model);
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			loadNode_test(nullptr, node, scene.nodes[i], gltfModel, model, 1.0f);
			
		}
	}
	_model.emplace("sponza", model);
}

void asset::ModelManager::loadImage_test(tinygltf::Model& gltfModel, ModelData& model)
{
	for (tinygltf::Image& image : gltfModel.images) {
		Image texture_image; 
		std::string path = utils::findInMap(_imageFilesInfo, image.uri).path;
		model.images.push_back(image.uri);
		ktxTexture* ktxTexture;
		ktxResult result = KTX_SUCCESS;
		if (!FileExists(path)) {
			throw std::runtime_error("Could not load texture from " + path + "\n\nMake sure the assets submodule has been checked out and is up-to-date.");
		}
		result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		assert(result == KTX_SUCCESS);
		texture_image.texWidth = ktxTexture->baseWidth;
		texture_image.texHeight = ktxTexture->baseHeight;
		texture_image.mipLevels = ktxTexture->numLevels;

		texture_image.pixel = ktxTexture_GetData(ktxTexture);
		texture_image.size = ktxTexture_GetDataSize(ktxTexture);
		texture_image.format = FromVk(ktxTexture_GetVkFormat(ktxTexture));
		texture_image.subresource.resize(texture_image.mipLevels);
		for (uint32_t i = 0; i < texture_image.mipLevels; i++) {
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			assert(result == KTX_SUCCESS);
			SubResource& sub = texture_image.subresource[i];
			sub.mip = i;
			sub.width = std::max(1u, ktxTexture->baseWidth >> i);
			sub.height = std::max(1u, ktxTexture->baseHeight >> i);
			sub.depth = 1;
			sub.offset = offset;
			sub.layer = 0;

		}
		_ImageFile.emplace(image.uri, texture_image);

	}

}

void asset::ModelManager::loadMaterial_test(tinygltf::Model& gltfMode, ModelData& model)
{

	for (tinygltf::Material& mat : gltfMode.materials) {
		Material material;
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTexture = gltfMode.images.at(gltfMode.textures[mat.values["baseColorTexture"].TextureIndex()].source).uri;
			
		}
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.matallicRoughnessTexture = gltfMode.images.at(gltfMode.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source).uri;
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.matallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTexture = gltfMode.images.at(gltfMode.textures[mat.additionalValues["normalTexture"].TextureIndex()].source).uri;
		}
		else {
			//material.normalTexture = &emptyTexture;
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTexture = gltfMode.images.at(gltfMode.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source).uri;
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTexture = gltfMode.images.at(gltfMode.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source).uri;
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}

		model.materials.push_back(material);
		model.materialCount++;
	}


}

void asset::ModelManager::loadNode_test(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& tingymodel, ModelData& model, float globalscale)
{
	Node* newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = glm::mat4(1.0f);

	glm::vec3 translation = glm::vec3(0.f);
	if (node.translation.size() == 3) {
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	glm::mat4 rotation = glm::mat4(1.0f);
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	glm::vec3 scale = glm::vec3(1.0f);
	if (node.scale.size() == 3) {
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
		if (globalscale != 1.0f) {
			newNode->matrix = glm::scale(newNode->matrix, glm::vec3(globalscale));
		}
	}
	if (node.children.size() > 0) {
		for (auto i = 0; i < node.children.size(); i++) {
			loadNode_test(newNode, tingymodel.nodes[node.children[i]], node.children[i], tingymodel, model, globalscale);
		}
	}

	if (node.mesh > -1) {
		const tinygltf::Mesh mesh = tingymodel.meshes[node.mesh];
		MeshData meshdata{};
		meshdata.name = mesh.name;
		for (size_t j = 0; j < mesh.primitives.size(); j++) {
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			if (primitive.indices < 0) {
				continue;
			}
			uint32_t indexStart = static_cast<uint32_t>(model.indiceSize);
			uint32_t vertexStart = static_cast<uint32_t>(model.vertexSize);
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};
			bool hasSkin = false;
			//Vertex
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoords = nullptr;
				const float* bufferColors = nullptr;
				const float* bufferTangents = nullptr;
				uint32_t numColorComponents;
				const uint16_t* bufferJoints = nullptr;
				const float* bufferWeights = nullptr;

				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = tingymodel.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = tingymodel.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(tingymodel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				meshdata.aabbMax = utils::math::VecMax(posMax, meshdata.aabbMax);
				meshdata.aabbMin = utils::math::VecMin(posMin, meshdata.aabbMin);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor &normAccessor = tingymodel.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &normView = tingymodel.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float *>(&(tingymodel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = tingymodel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uvView = tingymodel.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float *>(&(tingymodel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = tingymodel.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = tingymodel.bufferViews[colorAccessor.bufferView];
					// Color buffer are either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float*>(&(tingymodel.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor &tangentAccessor = tingymodel.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &tangentView = tingymodel.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float *>(&(tingymodel.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &jointAccessor = tingymodel.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView &jointView = tingymodel.bufferViews[jointAccessor.bufferView];
					bufferJoints = reinterpret_cast<const uint16_t *>(&(tingymodel.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = tingymodel.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView &uvView = tingymodel.bufferViews[uvAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float *>(&(tingymodel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				hasSkin = (bufferJoints && bufferWeights);

				vertexCount = static_cast<uint32_t>(posAccessor.count);

				for (size_t v = 0; v < posAccessor.count; v++) {
					Vertex1 vert{};
					vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
					vert.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
					if (bufferColors) {
						switch (numColorComponents) {
						case 3:
							vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
						case 4:
							vert.color = glm::make_vec4(&bufferColors[v * 4]);
						}
					}
					else
					{
						vert.color = glm::vec4(1.0f);
					}
					vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
					vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
					meshdata.vertices.push_back(vert);
					model.vertexSize++;
				}
			}
			//Indice
			{
				const tinygltf::Accessor& accessor = tingymodel.accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = tingymodel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = tingymodel.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);

				switch (accessor.componentType)
				{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t* buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						meshdata.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t* buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						meshdata.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t* buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						meshdata.indices.push_back(buf[index] + vertexStart);
					}
					delete[] buf;
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
				model.indiceSize += indexCount;
				
			}
			Mesh meshoffset{};
			meshoffset.indexOffset = indexStart;
			meshoffset.indiceCount = indexCount;
			meshoffset.vertexOffset = vertexStart;
			meshoffset.vertexCount = vertexCount;
			meshoffset.materialID = primitive.material > -1 ? primitive.material : model.materials.size()-1;
			meshdata.meshes.push_back(meshoffset);

			
		}
		model.meshes.push_back(meshdata);
		model.aabbMax = utils::math::VecMax(meshdata.aabbMax, model.aabbMax);
		model.aabbMin = utils::math::VecMin(meshdata.aabbMin, model.aabbMin);
		model.meshCount++;
	}
}

bool asset::ModelManager::loadImageDataFunc(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	// KTX files will be handled by our own code
	if (image->uri.find_last_of(".") != std::string::npos) {
		if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx") {
			return true;
		}
	}
	return tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
	
}

bool asset::ModelManager::loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	return true;
}


