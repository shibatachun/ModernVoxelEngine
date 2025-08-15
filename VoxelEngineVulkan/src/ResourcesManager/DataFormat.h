#include "../utils/GlmUtils.h"
#include <vector>
#include <map>
//ktx
#include <ktx.h>
#include <ktxvulkan.h>

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

struct Node;

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
//图片format
enum class TextureFormat {
    RGBA8_UNORM,
    RGBA8_SRGB,
    BGRA8_UNORM,
    BGRA8_SRGB,
    RG8_UNORM,
    R8_UNORM,
    RGBA16F,
    RGBA32F,

    BC1_RGB_UNORM,
    BC1_RGB_SRGB,
    BC3_RGBA_UNORM,
    BC3_RGBA_SRGB,
    BC4_R_UNORM,
    BC5_RG_UNORM,
    BC7_RGBA_UNORM,
    BC7_RGBA_SRGB,

    ETC2_RGBA8_UNORM,
    ETC2_RGBA8_SRGB,
    ASTC_4x4_UNORM,
    ASTC_4x4_SRGB,
    UNKNOWN
};
//图片数据，用于texture creation whatever
struct SubResource {
    uint32_t mip, layer, face;
    uint32_t width, height, depth;
    size_t offset;
    uint32_t rowPitch;
    uint32_t slicePitch;
};

struct Image {
    std::string name;
    unsigned char* pixel;
    bool isKtx = false;
    size_t size;
    TextureFormat format;
    int mipLevels;
    int texWidth;
    int texHeight;
    int texChannels;
    std::vector<SubResource> subresource;

};

struct Material {
    enum AlphaMode{ ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND};
    AlphaMode alphaMode = ALPHAMODE_OPAQUE;
    float alphaCutoff = 1.0f;
    float matallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec4 baseColorFactor{};
    std::string baseColorTexture ;
    std::string matallicRoughnessTexture;
    std::string normalTexture;
    std::string occlusionTexture;
    std::string emissiveTexture;
    std::string specularGlossinessTexture;
    std::string diffuseTexture;

};


inline TextureFormat FromVk(VkFormat f) {
    switch (f) {                          
    case VK_FORMAT_R8G8B8A8_UNORM:  return TextureFormat::RGBA8_UNORM;
    case VK_FORMAT_R8G8B8A8_SRGB:   return TextureFormat::RGBA8_SRGB;
    case VK_FORMAT_B8G8R8A8_UNORM:  return TextureFormat::BGRA8_UNORM;
    case VK_FORMAT_B8G8R8A8_SRGB:   return TextureFormat::BGRA8_SRGB;
    case VK_FORMAT_R8_UNORM:        return TextureFormat::R8_UNORM;
    case VK_FORMAT_R8G8_UNORM:      return TextureFormat::RG8_UNORM;
    case VK_FORMAT_R16G16B16A16_SFLOAT: return  TextureFormat::RGBA16F;
    case VK_FORMAT_R32G32B32A32_SFLOAT: return  TextureFormat::RGBA32F;

    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:   return TextureFormat::BC1_RGB_UNORM;
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:    return TextureFormat::BC1_RGB_SRGB;
    case VK_FORMAT_BC3_UNORM_BLOCK:       return TextureFormat::BC3_RGBA_UNORM;
    case VK_FORMAT_BC3_SRGB_BLOCK:        return TextureFormat::BC3_RGBA_SRGB;
    case VK_FORMAT_BC4_UNORM_BLOCK:       return TextureFormat::BC4_R_UNORM;
    case VK_FORMAT_BC5_UNORM_BLOCK:       return TextureFormat::BC5_RG_UNORM;
    case VK_FORMAT_BC7_UNORM_BLOCK:       return TextureFormat::BC7_RGBA_UNORM;
    case VK_FORMAT_BC7_SRGB_BLOCK:        return TextureFormat::BC7_RGBA_SRGB;
                                                      
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return  TextureFormat::ETC2_RGBA8_UNORM;
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:  return  TextureFormat::ETC2_RGBA8_SRGB;

    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:  return  TextureFormat::ASTC_4x4_UNORM;
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:   return  TextureFormat::ASTC_4x4_SRGB;
    }
    return TextureFormat::UNKNOWN;
}



//Skin 蒙皮？
struct Skin {
    std::string name;
    Node* skeletonRoot = nullptr;
    std::vector<glm::mat4> inverseBindMatrices;
    std::vector<Node*> joints;
};

//node 节点
struct Node {
    Node* parent;
    uint32_t index;
    std::vector<Node*> children;
    glm::mat4 matrix;
    std::string name;
    MeshData* mesh;
    Skin* skin;
    int32_t skinIndex = -1;
    glm::vec3 translation{};
    glm::vec3 scale{ 1.0f };
    glm::quat rotation{};
    glm::mat4 localMatrix();
    glm::mat4 getMatrix();
    void update();
    ~Node();
};

//模型数据
struct ModelData {
    std::string name;
    uint32_t meshCount;
    uint32_t materialCount;
    uint64_t timestamp;
    std::vector<MeshData> meshes;
    std::vector<std::string> images;
    std::vector<Material> materials;
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
};