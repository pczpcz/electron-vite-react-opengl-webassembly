#ifndef MESH_H
#define MESH_H

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "vertexarrayobject.h"
#include "bufferobject.h"
#include "material.h"

/**
 * @brief 网格类
 *
 * 封装几何数据和渲染状态，包含顶点数据、索引数据和材质
 */
class Mesh
{
public:
    Mesh();
    ~Mesh();

    // 禁用拷贝构造和赋值
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    // 移动构造和赋值
    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;

    /**
     * @brief 设置顶点数据
     * @param vertices 顶点数据
     * @param vertexCount 顶点数量
     * @param vertexSize 每个顶点的大小（字节）
     */
    void setVertices(const float *vertices, GLsizei vertexCount, GLsizei vertexSize);

    /**
     * @brief 设置索引数据
     * @param indices 索引数据
     * @param indexCount 索引数量
     */
    void setIndices(const unsigned int *indices, GLsizei indexCount);

    /**
     * @brief 设置材质
     * @param material 材质对象
     */
    void setMaterial(std::shared_ptr<Material> material);

    /**
     * @brief 获取材质
     * @return 材质对象
     */
    std::shared_ptr<Material> getMaterial() const { return m_material; }

    /**
     * @brief 渲染网格
     */
    void render();

    /**
     * @brief 检查网格是否有效
     * @return 是否有效
     */
    bool isValid() const { return m_vao.isValid() && m_vbo.isValid(); }

    /**
     * @brief 获取顶点数量
     * @return 顶点数量
     */
    GLsizei getVertexCount() const { return m_vertexCount; }

    /**
     * @brief 获取索引数量
     * @return 索引数量
     */
    GLsizei getIndexCount() const { return m_indexCount; }

    /**
     * @brief 初始化网格
     */
    void initialize();

private:
    VertexArrayObject m_vao;
    BufferObject m_vbo;
    BufferObject m_ebo;
    std::shared_ptr<Material> m_material;
    GLsizei m_vertexCount;
    GLsizei m_indexCount;
};

#endif // MESH_H
