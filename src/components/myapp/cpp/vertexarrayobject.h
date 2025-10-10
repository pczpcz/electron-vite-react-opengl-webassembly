#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class BufferObject;

/**
 * @brief Vertex Array Object (VAO) 封装类
 *
 * 封装OpenGL VAO操作，提供更简洁的接口来管理顶点属性
 */
class VertexArrayObject
{
public:
    VertexArrayObject();
    ~VertexArrayObject();

    // 禁用拷贝构造和赋值
    VertexArrayObject(const VertexArrayObject &) = delete;
    VertexArrayObject &operator=(const VertexArrayObject &) = delete;

    // 移动构造和赋值
    VertexArrayObject(VertexArrayObject &&other) noexcept;
    VertexArrayObject &operator=(VertexArrayObject &&other) noexcept;

    /**
     * @brief 绑定VAO
     */
    void bind() const;

    /**
     * @brief 解绑VAO
     */
    void unbind() const;

    /**
     * @brief 获取VAO ID
     * @return VAO ID
     */
    GLuint getId() const { return m_id; }

    /**
     * @brief 检查VAO是否有效
     * @return 是否有效
     */
    bool isValid() const { return m_id != 0; }

    /**
     * @brief 设置顶点属性指针
     * @param index 属性索引
     * @param size 每个顶点的分量数 (1, 2, 3, 4)
     * @param type 数据类型 (GL_FLOAT, GL_INT等)
     * @param normalized 是否归一化
     * @param stride 步长
     * @param pointer 偏移量
     */
    void setVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                GLboolean normalized, GLsizei stride, const void *pointer);

    /**
     * @brief 启用顶点属性数组
     * @param index 属性索引
     */
    void enableVertexAttribArray(GLuint index);

    /**
     * @brief 禁用顶点属性数组
     * @param index 属性索引
     */
    void disableVertexAttribArray(GLuint index);

    /**
     * @brief 绑定元素缓冲对象(EBO)
     * @param ebo EBO对象
     */
    void bindElementBuffer(const BufferObject &ebo);

    /**
     * @brief 绑定顶点缓冲对象(VBO)
     * @param vbo VBO对象
     */
    void bindVertexBuffer(const BufferObject &vbo);

private:
    GLuint m_id;
};

#endif // VERTEX_ARRAY_OBJECT_H
