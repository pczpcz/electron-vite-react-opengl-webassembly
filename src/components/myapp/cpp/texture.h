#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLFW/glfw3.h>
#include <string>

/**
 * @brief 纹理类
 *
 * 封装OpenGL纹理对象
 */
class Texture
{
public:
    Texture();
    ~Texture();

    // 禁用拷贝构造和赋值
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    // 移动构造和赋值
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    /**
     * @brief 从文件加载纹理
     * @param filename 纹理文件路径
     * @return 是否加载成功
     */
    bool loadFromFile(const std::string &filename);

    /**
     * @brief 从内存数据创建纹理
     * @param data 纹理数据
     * @param width 纹理宽度
     * @param height 纹理高度
     * @param format 纹理格式
     * @return 是否创建成功
     */
    bool createFromData(const unsigned char *data, int width, int height, GLenum format = GL_RGBA);

    /**
     * @brief 绑定纹理
     * @param unit 纹理单元
     */
    void bind(GLuint unit = 0) const;

    /**
     * @brief 解绑纹理
     */
    void unbind() const;

    /**
     * @brief 获取纹理ID
     * @return OpenGL纹理ID
     */
    GLuint getId() const { return m_textureId; }

    /**
     * @brief 检查纹理是否有效
     * @return 是否有效
     */
    bool isValid() const { return m_textureId != 0; }

private:
    GLuint m_textureId;
    int m_width;
    int m_height;
    GLenum m_format;
};

#endif // TEXTURE_H
