#ifndef FBX_DATA_H
#define FBX_DATA_H

#include <cstdint>
#include <string>
#include <vector>

// FBX数据结构定义
struct FBXVertex
{
    float position[3]; // 顶点位置 (x, y, z)
    float normal[3];   // 法线向量 (nx, ny, nz)
    float texcoord[2]; // 纹理坐标 (u, v)
    float color[4];    // 顶点颜色 (r, g, b, a)
};

struct FBXTriangle
{
    uint32_t indices[3]; // 三角形顶点索引
};

struct FBXMesh
{
    std::string name;                   // 网格名称
    std::vector<FBXVertex> vertices;    // 顶点数据
    std::vector<FBXTriangle> triangles; // 三角形数据
    float boundingBox[6];               // 包围盒 [minX, minY, minZ, maxX, maxY, maxZ]
};

struct FBXMaterial
{
    std::string name;           // 材质名称
    float diffuseColor[3];      // 漫反射颜色
    float specularColor[3];     // 高光颜色
    float emissiveColor[3];     // 自发光颜色
    float opacity;              // 不透明度
    float shininess;            // 光泽度
    std::string diffuseTexture; // 漫反射贴图路径
    std::string normalTexture;  // 法线贴图路径
};

struct FBXNode
{
    std::string name;              // 节点名称
    float transform[16];           // 变换矩阵 (4x4)
    std::vector<FBXMesh> meshes;   // 节点包含的网格
    std::vector<FBXNode> children; // 子节点
};

struct FBXScene
{
    std::vector<FBXNode> nodes;         // 场景中的所有节点
    std::vector<FBXMaterial> materials; // 场景中的所有材质
    float globalScale;                  // 全局缩放因子
};

// 共享内存数据结构
struct SharedMemoryHeader
{
    uint64_t magicNumber;   // 魔数，用于验证数据完整性
    uint32_t version;       // 数据版本
    uint32_t dataSize;      // 数据总大小
    uint32_t vertexCount;   // 顶点总数
    uint32_t triangleCount; // 三角形总数
    uint32_t nodeCount;     // 节点总数
    uint32_t materialCount; // 材质总数
    uint64_t checksum;      // 数据校验和
};

// 常量定义
constexpr uint64_t FBX_MAGIC_NUMBER = 0x4642584441544100; // "FBXDATA\0"
constexpr uint32_t FBX_DATA_VERSION = 1;

#endif // FBX_DATA_H
