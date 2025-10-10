#include "mesh.h"

Mesh::Mesh()
    : m_vbo(BufferObject::Type::VertexBuffer),
      m_ebo(BufferObject::Type::ElementBuffer),
      m_vertexCount(0), m_indexCount(0)
{
}

Mesh::~Mesh()
{
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_vao(std::move(other.m_vao)),
      m_vbo(std::move(other.m_vbo)),
      m_ebo(std::move(other.m_ebo)),
      m_material(std::move(other.m_material)),
      m_vertexCount(other.m_vertexCount),
      m_indexCount(other.m_indexCount)
{
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other)
    {
        m_vao = std::move(other.m_vao);
        m_vbo = std::move(other.m_vbo);
        m_ebo = std::move(other.m_ebo);
        m_material = std::move(other.m_material);
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::setVertices(const float *vertices, GLsizei vertexCount, GLsizei vertexSize)
{
    m_vertexCount = vertexCount;

    // 配置VAO
    m_vao.bind();

    // 绑定VBO到VAO
    m_vao.bindVertexBuffer(m_vbo);

    // 设置顶点缓冲数据
    m_vbo.setData(vertices, vertexCount * vertexSize, BufferObject::Usage::StaticDraw);

    // 配置顶点属性指针
    m_vao.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    m_vao.enableVertexAttribArray(0);

    m_vao.unbind();
}

void Mesh::initialize()
{
    // 网格初始化逻辑
    // 这里可以添加网格的初始化代码，比如预计算边界框等
    // 目前不需要特殊初始化，因为setVertices和setIndices已经完成了主要工作
}

void Mesh::setIndices(const unsigned int *indices, GLsizei indexCount)
{
    m_indexCount = indexCount;

    // 设置元素缓冲数据
    m_ebo.setData(indices, indexCount * sizeof(unsigned int), BufferObject::Usage::StaticDraw);

    // 绑定EBO到VAO
    m_vao.bind();
    m_vao.bindElementBuffer(m_ebo);
    m_vao.unbind();
}

void Mesh::setMaterial(std::shared_ptr<Material> material)
{
    m_material = material;
}

void Mesh::render()
{
    if (!isValid() || !m_material)
        return;

    // 应用材质
    m_material->apply();

    // 绑定VAO并渲染
    m_vao.bind();
    // std::cout << "Rendering mesh with " << m_vertexCount << " vertices and " << m_indexCount << " indices." << std::endl;

    if (m_indexCount > 0)
    {
        // 使用索引绘制
        // std::cout << "Using indexed drawing." << std::endl;
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    }
    else
    {
        // 不使用索引绘制
        // std::cout << "Using non-indexed drawing." << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

    m_vao.unbind();
}
