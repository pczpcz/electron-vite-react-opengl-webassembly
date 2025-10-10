#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include <GLFW/glfw3.h>
#include <vector>
#include <cstddef>

/**
 * @brief Buffer Object (VBO/EBO) 封装类
 *
 * 封装OpenGL缓冲对象操作，支持顶点缓冲(VBO)和元素缓冲(EBO)
 */
class BufferObject
{
public:
    enum class Type
    {
        VertexBuffer = GL_ARRAY_BUFFER,
        ElementBuffer = GL_ELEMENT_ARRAY_BUFFER,
        UniformBuffer = GL_UNIFORM_BUFFER
    };

    enum class Usage
    {
        StaticDraw = GL_STATIC_DRAW,
        DynamicDraw = GL_DYNAMIC_DRAW,
        StreamDraw = GL_STREAM_DRAW
    };

    BufferObject(Type type = Type::VertexBuffer);
    ~BufferObject();

    // 禁用拷贝构造和赋值
    BufferObject(const BufferObject &) = delete;
    BufferObject &operator=(const BufferObject &) = delete;

    // 移动构造和赋值
    BufferObject(BufferObject &&other) noexcept;
    BufferObject &operator=(BufferObject &&other) noexcept;

    /**
     * @brief 绑定缓冲对象
     */
    void bind() const;

    /**
     * @brief 解绑缓冲对象
     */
    void unbind() const;

    /**
     * @brief 获取缓冲对象ID
     * @return 缓冲对象ID
     */
    GLuint getId() const { return m_id; }

    /**
     * @brief 检查缓冲对象是否有效
     * @return 是否有效
     */
    bool isValid() const { return m_id != 0; }

    /**
     * @brief 获取缓冲类型
     * @return 缓冲类型
     */
    Type getType() const { return m_type; }

    /**
     * @brief 设置缓冲数据
     * @param data 数据指针
     * @param size 数据大小（字节）
     * @param usage 使用方式
     */
    void setData(const void *data, GLsizeiptr size, Usage usage = Usage::StaticDraw);

    /**
     * @brief 设置缓冲数据（模板版本）
     * @param data 数据容器
     * @param usage 使用方式
     */
    template <typename T>
    void setData(const std::vector<T> &data, Usage usage = Usage::StaticDraw)
    {
        setData(data.data(), static_cast<GLsizeiptr>(data.size() * sizeof(T)), usage);
    }

    /**
     * @brief 更新缓冲数据的一部分
     * @param offset 偏移量
     * @param data 数据指针
     * @param size 数据大小（字节）
     */
    void updateData(GLintptr offset, const void *data, GLsizeiptr size);

    /**
     * @brief 更新缓冲数据的一部分（模板版本）
     * @param offset 偏移量
     * @param data 数据容器
     */
    template <typename T>
    void updateData(GLintptr offset, const std::vector<T> &data)
    {
        updateData(offset, data.data(), static_cast<GLsizeiptr>(data.size() * sizeof(T)));
    }

    /**
     * @brief 获取缓冲大小
     * @return 缓冲大小（字节）
     */
    GLsizeiptr getSize() const { return m_size; }

private:
    GLuint m_id;
    Type m_type;
    GLsizeiptr m_size;
};

#endif // BUFFER_OBJECT_H
