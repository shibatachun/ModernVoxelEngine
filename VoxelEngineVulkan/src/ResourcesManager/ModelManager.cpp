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
				glm::vec3(assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z),
				//N向量
				glm::vec3(assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z),
				//UV坐标
				assimpMesh->HasTextureCoords(0) ? glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f,0.0f),
				//Tangent
				assimpMesh->HasTangentsAndBitangents() ? glm::vec3(assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z) : glm::vec3(0.0f) }
				);
			meshData.aabbMin = utils::math::VecMin(meshData.vertices[j].position, meshData.aabbMin);
			meshData.aabbMax = utils::math::VecMax(meshData.vertices[j].position, meshData.aabbMax);
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
			glm::vec3 deltaPos1 = vert1->position - vert0->position;
			glm::vec3 deltaPos2 = vert2->position - vert0->position;
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

	_model.emplace(filename, modelData);

}

void asset::ModelManager::processNode(aiNode* node, const aiScene* scene)
{
}


