#include <napi.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <cstring>
#include "fbx_data.h"

// 模拟共享内存管理器
class SharedMemoryManager
{
private:
    std::unordered_map<std::string, std::vector<uint8_t>> sharedMemory;
    std::random_device rd;
    std::mt19937_64 gen;
    std::uniform_int_distribution<uint64_t> dis;

public:
    SharedMemoryManager() : gen(rd()), dis(1000000000, 9999999999) {}

    std::string CreateSharedMemory(size_t size)
    {
        std::string memoryId = std::to_string(dis(gen));
        sharedMemory[memoryId] = std::vector<uint8_t>(size, 0);
        return memoryId;
    }

    bool WriteToSharedMemory(const std::string &memoryId, const void *data, size_t size)
    {
        auto it = sharedMemory.find(memoryId);
        if (it == sharedMemory.end() || it->second.size() < size)
        {
            return false;
        }
        std::memcpy(it->second.data(), data, size);
        return true;
    }

    const uint8_t *ReadFromSharedMemory(const std::string &memoryId)
    {
        auto it = sharedMemory.find(memoryId);
        if (it == sharedMemory.end())
        {
            return nullptr;
        }
        return it->second.data();
    }

    size_t GetSharedMemorySize(const std::string &memoryId)
    {
        auto it = sharedMemory.find(memoryId);
        if (it == sharedMemory.end())
        {
            return 0;
        }
        return it->second.size();
    }

    bool DeleteSharedMemory(const std::string &memoryId)
    {
        return sharedMemory.erase(memoryId) > 0;
    }
};

static SharedMemoryManager g_sharedMemoryManager;

// 模拟FBX数据生成器
class MockFBXGenerator
{
public:
    FBXScene GenerateMockScene()
    {
        FBXScene scene;
        scene.globalScale = 1.0f;

        // 创建一个简单的立方体网格
        FBXNode node;
        node.name = "MockCube";

        // 单位矩阵
        std::fill(std::begin(node.transform), std::end(node.transform), 0.0f);
        node.transform[0] = node.transform[5] = node.transform[10] = node.transform[15] = 1.0f;

        FBXMesh mesh;
        mesh.name = "CubeMesh";

        // 创建立方体的8个顶点
        GenerateCubeVertices(mesh.vertices);
        GenerateCubeTriangles(mesh.triangles);
        CalculateBoundingBox(mesh.vertices, mesh.boundingBox);

        node.meshes.push_back(mesh);
        scene.nodes.push_back(node);

        // 添加一个默认材质
        FBXMaterial material;
        material.name = "DefaultMaterial";
        material.diffuseColor[0] = 0.8f;
        material.diffuseColor[1] = 0.8f;
        material.diffuseColor[2] = 0.8f;
        material.specularColor[0] = 0.5f;
        material.specularColor[1] = 0.5f;
        material.specularColor[2] = 0.5f;
        material.emissiveColor[0] = 0.0f;
        material.emissiveColor[1] = 0.0f;
        material.emissiveColor[2] = 0.0f;
        material.opacity = 1.0f;
        material.shininess = 32.0f;

        scene.materials.push_back(material);

        return scene;
    }

private:
    void GenerateCubeVertices(std::vector<FBXVertex> &vertices)
    {
        // 立方体的8个顶点
        float cubeVertices[8][3] = {
            {-1.0f, -1.0f, -1.0f}, // 0
            {1.0f, -1.0f, -1.0f},  // 1
            {1.0f, 1.0f, -1.0f},   // 2
            {-1.0f, 1.0f, -1.0f},  // 3
            {-1.0f, -1.0f, 1.0f},  // 4
            {1.0f, -1.0f, 1.0f},   // 5
            {1.0f, 1.0f, 1.0f},    // 6
            {-1.0f, 1.0f, 1.0f}    // 7
        };

        vertices.resize(8);
        for (int i = 0; i < 8; i++)
        {
            vertices[i].position[0] = cubeVertices[i][0];
            vertices[i].position[1] = cubeVertices[i][1];
            vertices[i].position[2] = cubeVertices[i][2];

            // 简单法线（指向外侧）
            vertices[i].normal[0] = cubeVertices[i][0];
            vertices[i].normal[1] = cubeVertices[i][1];
            vertices[i].normal[2] = cubeVertices[i][2];

            // 标准化法线
            float length = std::sqrt(
                vertices[i].normal[0] * vertices[i].normal[0] +
                vertices[i].normal[1] * vertices[i].normal[1] +
                vertices[i].normal[2] * vertices[i].normal[2]);
            if (length > 0.0f)
            {
                vertices[i].normal[0] /= length;
                vertices[i].normal[1] /= length;
                vertices[i].normal[2] /= length;
            }

            // 简单纹理坐标
            vertices[i].texcoord[0] = (cubeVertices[i][0] + 1.0f) * 0.5f;
            vertices[i].texcoord[1] = (cubeVertices[i][1] + 1.0f) * 0.5f;

            // 默认颜色（白色）
            vertices[i].color[0] = 1.0f;
            vertices[i].color[1] = 1.0f;
            vertices[i].color[2] = 1.0f;
            vertices[i].color[3] = 1.0f;
        }
    }

    void GenerateCubeTriangles(std::vector<FBXTriangle> &triangles)
    {
        // 立方体的12个三角形（6个面，每个面2个三角形）
        uint32_t cubeIndices[12][3] = {
            // 前面
            {0, 1, 2},
            {0, 2, 3},
            // 后面
            {4, 6, 5},
            {4, 7, 6},
            // 左面
            {0, 3, 7},
            {0, 7, 4},
            // 右面
            {1, 5, 6},
            {1, 6, 2},
            // 上面
            {3, 2, 6},
            {3, 6, 7},
            // 下面
            {0, 4, 5},
            {0, 5, 1}};

        triangles.resize(12);
        for (int i = 0; i < 12; i++)
        {
            triangles[i].indices[0] = cubeIndices[i][0];
            triangles[i].indices[1] = cubeIndices[i][1];
            triangles[i].indices[2] = cubeIndices[i][2];
        }
    }

    void CalculateBoundingBox(const std::vector<FBXVertex> &vertices, float *boundingBox)
    {
        if (vertices.empty())
        {
            std::fill(boundingBox, boundingBox + 6, 0.0f);
            return;
        }

        boundingBox[0] = boundingBox[1] = boundingBox[2] = std::numeric_limits<float>::max();
        boundingBox[3] = boundingBox[4] = boundingBox[5] = std::numeric_limits<float>::lowest();

        for (const auto &vertex : vertices)
        {
            boundingBox[0] = std::min(boundingBox[0], vertex.position[0]);
            boundingBox[1] = std::min(boundingBox[1], vertex.position[1]);
            boundingBox[2] = std::min(boundingBox[2], vertex.position[2]);
            boundingBox[3] = std::max(boundingBox[3], vertex.position[0]);
            boundingBox[4] = std::max(boundingBox[4], vertex.position[1]);
            boundingBox[5] = std::max(boundingBox[5], vertex.position[2]);
        }
    }
};

// 序列化FBX场景到共享内存
size_t SerializeFBXSceneToMemory(const FBXScene &scene, uint8_t *buffer, size_t bufferSize)
{
    if (!buffer || bufferSize < sizeof(SharedMemoryHeader))
    {
        return 0;
    }

    SharedMemoryHeader *header = reinterpret_cast<SharedMemoryHeader *>(buffer);
    header->magicNumber = FBX_MAGIC_NUMBER;
    header->version = FBX_DATA_VERSION;
    header->vertexCount = 0;
    header->triangleCount = 0;
    header->nodeCount = static_cast<uint32_t>(scene.nodes.size());
    header->materialCount = static_cast<uint32_t>(scene.materials.size());
    header->checksum = 0;

    // 计算顶点和三角形总数
    for (const auto &node : scene.nodes)
    {
        for (const auto &mesh : node.meshes)
        {
            header->vertexCount += static_cast<uint32_t>(mesh.vertices.size());
            header->triangleCount += static_cast<uint32_t>(mesh.triangles.size());
        }
    }

    // 计算数据总大小（简化版本，只包含基本信息）
    header->dataSize = sizeof(SharedMemoryHeader);

    return header->dataSize;
}

// NAPI函数：加载FBX文件
Napi::Value LoadFBXFile(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected for file path").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filePath = info[0].As<Napi::String>();
    std::cout << "Loading FBX file: " << filePath << std::endl;

    try
    {
        // 使用模拟数据生成器
        MockFBXGenerator generator;
        FBXScene scene = generator.GenerateMockScene();

        // 计算所需内存大小
        size_t estimatedSize = sizeof(SharedMemoryHeader) +
                               scene.nodes.size() * sizeof(FBXNode) +
                               scene.materials.size() * sizeof(FBXMaterial) +
                               10000; // 额外空间用于顶点和三角形数据

        // 创建共享内存
        std::string sharedMemoryId = g_sharedMemoryManager.CreateSharedMemory(estimatedSize);

        // 序列化数据到共享内存
        uint8_t *buffer = const_cast<uint8_t *>(g_sharedMemoryManager.ReadFromSharedMemory(sharedMemoryId));
        size_t dataSize = SerializeFBXSceneToMemory(scene, buffer, estimatedSize);

        if (dataSize == 0)
        {
            Napi::Error::New(env, "Failed to serialize FBX data").ThrowAsJavaScriptException();
            return env.Null();
        }

        // 创建返回对象
        Napi::Object result = Napi::Object::New(env);
        result.Set("sharedMemoryId", Napi::String::New(env, sharedMemoryId));

        Napi::Object metadata = Napi::Object::New(env);
        metadata.Set("vertexCount", Napi::Number::New(env, 8));    // 立方体有8个顶点
        metadata.Set("triangleCount", Napi::Number::New(env, 12)); // 立方体有12个三角形
        metadata.Set("nodeCount", Napi::Number::New(env, scene.nodes.size()));
        metadata.Set("materialCount", Napi::Number::New(env, scene.materials.size()));
        metadata.Set("filePath", Napi::String::New(env, filePath));

        result.Set("metadata", metadata);

        std::cout << "FBX file loaded successfully. Shared memory ID: " << sharedMemoryId << std::endl;
        return result;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, std::string("Failed to load FBX file: ") + e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// NAPI函数：创建共享内存
Napi::Value CreateSharedMemory(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for size").ThrowAsJavaScriptException();
        return env.Null();
    }

    size_t size = info[0].As<Napi::Number>().Uint32Value();
    std::string memoryId = g_sharedMemoryManager.CreateSharedMemory(size);

    return Napi::String::New(env, memoryId);
}

// NAPI函数：从共享内存获取数据
Napi::Value GetSharedMemoryData(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected for shared memory ID").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string memoryId = info[0].As<Napi::String>();
    const uint8_t *data = g_sharedMemoryManager.ReadFromSharedMemory(memoryId);
    size_t dataSize = g_sharedMemoryManager.GetSharedMemorySize(memoryId);

    if (!data || dataSize == 0)
    {
        Napi::Error::New(env, "Invalid shared memory ID or empty data").ThrowAsJavaScriptException();
        return env.Null();
    }

    // 返回模拟的FBX数据
    Napi::Object result = Napi::Object::New(env);
    result.Set("vertexCount", Napi::Number::New(env, 8));
    result.Set("triangleCount", Napi::Number::New(env, 12));
    result.Set("nodeCount", Napi::Number::New(env, 1));
    result.Set("materialCount", Napi::Number::New(env, 1));
    result.Set("sharedMemoryId", Napi::String::New(env, memoryId));
    result.Set("dataSize", Napi::Number::New(env, static_cast<double>(dataSize)));

    return result;
}

// 初始化NAPI模块
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("loadFBXFile", Napi::Function::New(env, LoadFBXFile));
    exports.Set("createSharedMemory", Napi::Function::New(env, CreateSharedMemory));
    exports.Set("getSharedMemoryData", Napi::Function::New(env, GetSharedMemoryData));

    std::cout << "FBX Loader native module initialized" << std::endl;
    return exports;
}

NODE_API_MODULE(fbx_loader, Init)
