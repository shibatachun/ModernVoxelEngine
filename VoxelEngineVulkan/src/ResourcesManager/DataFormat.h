#include "../utils/GlmUtils.h"
#include <vector>
#include <map>


//顶点数据
struct Vertex1 {
    glm::vec3 pos{};
    glm::vec4 color{};
    glm::vec2 uv{};
    glm::vec3 normal{};
    glm::vec4 joint0{};
    glm::vec4 weight0{};
    glm::vec3 tangent{};
  
    
};

//用来存储网格数据的数据结构
struct MeshData {
    std::string name;
    std::vector<Vertex1> vertices;
    std::vector<uint32_t> indices;
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t parentIndex = -1;
    glm::mat4 localTransform = glm::mat4(1.0f);
    glm::mat4 inverseBindTransform = glm::mat4(1.0f);
};

//模型文件
struct ModelData {
    std::string name;
    uint32_t meshCount;
    uint64_t timestamp;
    std::vector<MeshData> meshes;
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
};


struct Image {
    std::string name;
    unsigned char* pixel;
    int texWidth;
    int texHeight;
    int texChannels;
};