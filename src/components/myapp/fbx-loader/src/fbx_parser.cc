#include "fbx_data.h"
#include <fbxsdk.h>
#include <iostream>
#include <memory>
#include <cstring>

class FBXParser
{
private:
    FbxManager *fbxManager;
    FbxScene *fbxScene;

public:
    FBXParser() : fbxManager(nullptr), fbxScene(nullptr)
    {
        InitializeFBXSDK();
    }

    ~FBXParser()
    {
        CleanupFBXSDK();
    }

    bool LoadFBXFile(const std::string &filePath)
    {
        if (!fbxManager)
        {
            std::cerr << "FBX SDK not initialized" << std::endl;
            return false;
        }

        // 创建导入器
        FbxImporter *importer = FbxImporter::Create(fbxManager, "");
        if (!importer->Initialize(filePath.c_str(), -1, fbxManager->GetIOSettings()))
        {
            std::cerr << "Failed to initialize FBX importer: " << importer->GetStatus().GetErrorString() << std::endl;
            importer->Destroy();
            return false;
        }

        // 创建场景
        fbxScene = FbxScene::Create(fbxManager, "FBXScene");
        if (!importer->Import(fbxScene))
        {
            std::cerr << "Failed to import FBX scene: " << importer->GetStatus().GetErrorString() << std::endl;
            importer->Destroy();
            return false;
        }

        importer->Destroy();
        std::cout << "FBX file loaded successfully: " << filePath << std::endl;
        return true;
    }

    FBXScene ParseScene()
    {
        FBXScene scene;

        if (!fbxScene)
        {
            std::cerr << "No FBX scene loaded" << std::endl;
            return scene;
        }

        // 设置全局缩放
        scene.globalScale = 1.0f;

        // 处理根节点
        FbxNode *rootNode = fbxScene->GetRootNode();
        if (rootNode)
        {
            ProcessNode(rootNode, scene.nodes);
        }

        // 处理材质
        ProcessMaterials(scene.materials);

        std::cout << "Parsed scene with " << scene.nodes.size() << " nodes and "
                  << scene.materials.size() << " materials" << std::endl;

        return scene;
    }

private:
    void InitializeFBXSDK()
    {
        fbxManager = FbxManager::Create();
        if (!fbxManager)
        {
            std::cerr << "Failed to create FBX Manager" << std::endl;
            return;
        }

        FbxIOSettings *ios = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(ios);

        std::cout << "FBX SDK initialized successfully" << std::endl;
    }

    void CleanupFBXSDK()
    {
        if (fbxScene)
        {
            fbxScene->Destroy();
            fbxScene = nullptr;
        }
        if (fbxManager)
        {
            fbxManager->Destroy();
            fbxManager = nullptr;
        }
        std::cout << "FBX SDK cleaned up" << std::endl;
    }

    void ProcessNode(FbxNode *node, std::vector<FBXNode> &outputNodes)
    {
        FBXNode fbxNode;
        fbxNode.name = node->GetName();

        // 获取变换矩阵
        FbxAMatrix globalTransform = node->EvaluateGlobalTransform();
        ExtractTransformMatrix(globalTransform, fbxNode.transform);

        // 处理网格
        ProcessNodeMeshes(node, fbxNode.meshes);

        // 处理子节点
        for (int i = 0; i < node->GetChildCount(); i++)
        {
            ProcessNode(node->GetChild(i), fbxNode.children);
        }

        outputNodes.push_back(fbxNode);
    }

    void ProcessNodeMeshes(FbxNode *node, std::vector<FBXMesh> &meshes)
    {
        for (int i = 0; i < node->GetNodeAttributeCount(); i++)
        {
            FbxNodeAttribute *attribute = node->GetNodeAttributeByIndex(i);
            if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh *fbxMesh = static_cast<FbxMesh *>(attribute);
                FBXMesh mesh = ProcessMesh(fbxMesh);
                meshes.push_back(mesh);
            }
        }
    }

    FBXMesh ProcessMesh(FbxMesh *fbxMesh)
    {
        FBXMesh mesh;
        mesh.name = fbxMesh->GetName();

        // 处理顶点数据
        ProcessVertices(fbxMesh, mesh.vertices);

        // 处理三角形数据
        ProcessTriangles(fbxMesh, mesh.triangles);

        // 计算包围盒
        CalculateBoundingBox(mesh.vertices, mesh.boundingBox);

        std::cout << "Processed mesh: " << mesh.name
                  << " (vertices: " << mesh.vertices.size()
                  << ", triangles: " << mesh.triangles.size() << ")" << std::endl;

        return mesh;
    }

    void ProcessVertices(FbxMesh *fbxMesh, std::vector<FBXVertex> &vertices)
    {
        int vertexCount = fbxMesh->GetControlPointsCount();
        FbxVector4 *controlPoints = fbxMesh->GetControlPoints();

        vertices.resize(vertexCount);

        for (int i = 0; i < vertexCount; i++)
        {
            FBXVertex &vertex = vertices[i];

            // 位置
            vertex.position[0] = static_cast<float>(controlPoints[i][0]);
            vertex.position[1] = static_cast<float>(controlPoints[i][1]);
            vertex.position[2] = static_cast<float>(controlPoints[i][2]);

            // 默认值
            vertex.normal[0] = 0.0f;
            vertex.normal[1] = 1.0f;
            vertex.normal[2] = 0.0f;

            vertex.texcoord[0] = 0.0f;
            vertex.texcoord[1] = 0.0f;

            vertex.color[0] = 1.0f;
            vertex.color[1] = 1.0f;
            vertex.color[2] = 1.0f;
            vertex.color[3] = 1.0f;
        }

        // 处理法线（简化版本）
        ProcessNormals(fbxMesh, vertices);

        // 处理纹理坐标（简化版本）
        ProcessTextureCoords(fbxMesh, vertices);
    }

    void ProcessNormals(FbxMesh *fbxMesh, std::vector<FBXVertex> &vertices)
    {
        FbxGeometryElementNormal *normalElement = fbxMesh->GetElementNormal();
        if (!normalElement)
            return;

        for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++)
        {
            FbxVector4 normal;
            if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
            {
                normal = normalElement->GetDirectArray().GetAt(vertexIndex);
            }

            vertices[vertexIndex].normal[0] = static_cast<float>(normal[0]);
            vertices[vertexIndex].normal[1] = static_cast<float>(normal[1]);
            vertices[vertexIndex].normal[2] = static_cast<float>(normal[2]);
        }
    }

    void ProcessTextureCoords(FbxMesh *fbxMesh, std::vector<FBXVertex> &vertices)
    {
        FbxGeometryElementUV *uvElement = fbxMesh->GetElementUV();
        if (!uvElement)
            return;

        for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++)
        {
            FbxVector2 uv;
            if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
            {
                uv = uvElement->GetDirectArray().GetAt(vertexIndex);
            }

            vertices[vertexIndex].texcoord[0] = static_cast<float>(uv[0]);
            vertices[vertexIndex].texcoord[1] = static_cast<float>(uv[1]);
        }
    }

    void ProcessTriangles(FbxMesh *fbxMesh, std::vector<FBXTriangle> &triangles)
    {
        int polygonCount = fbxMesh->GetPolygonCount();

        for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
        {
            int polygonSize = fbxMesh->GetPolygonSize(polygonIndex);

            // 只处理三角形
            if (polygonSize == 3)
            {
                FBXTriangle triangle;
                for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
                {
                    int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, vertexIndex);
                    triangle.indices[vertexIndex] = static_cast<uint32_t>(controlPointIndex);
                }
                triangles.push_back(triangle);
            }
        }
    }

    void ProcessMaterials(std::vector<FBXMaterial> &materials)
    {
        int materialCount = fbxScene->GetMaterialCount();

        for (int i = 0; i < materialCount; i++)
        {
            FbxSurfaceMaterial *fbxMaterial = fbxScene->GetMaterial(i);
            FBXMaterial material;
            material.name = fbxMaterial->GetName();

            // 处理漫反射颜色
            FbxProperty diffuseProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (diffuseProperty.IsValid())
            {
                FbxDouble3 diffuseColor = diffuseProperty.Get<FbxDouble3>();
                material.diffuseColor[0] = static_cast<float>(diffuseColor[0]);
                material.diffuseColor[1] = static_cast<float>(diffuseColor[1]);
                material.diffuseColor[2] = static_cast<float>(diffuseColor[2]);
            }
            else
            {
                material.diffuseColor[0] = 0.8f;
                material.diffuseColor[1] = 0.8f;
                material.diffuseColor[2] = 0.8f;
            }

            materials.push_back(material);
        }
    }

    void ExtractTransformMatrix(const FbxAMatrix &fbxMatrix, float *outputMatrix)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                outputMatrix[i * 4 + j] = static_cast<float>(fbxMatrix.Get(i, j));
            }
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
