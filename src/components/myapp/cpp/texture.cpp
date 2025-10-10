#include "texture.h"
#include <iostream>

Texture::Texture()
    : m_textureId(0), m_width(0), m_height(0), m_format(GL_RGBA)
{
    glGenTextures(1, &m_textureId);
}

Texture::~Texture()
{
    if (m_textureId != 0)
    {
        glDeleteTextures(1, &m_textureId);
    }
}

Texture::Texture(Texture &&other) noexcept
    : m_textureId(other.m_textureId),
      m_width(other.m_width),
      m_height(other.m_height),
      m_format(other.m_format)
{
    other.m_textureId = 0;
    other.m_width = 0;
    other.m_height = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept
{
    if (this != &other)
    {
        if (m_textureId != 0)
        {
            glDeleteTextures(1, &m_textureId);
        }

        m_textureId = other.m_textureId;
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;

        other.m_textureId = 0;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

bool Texture::loadFromFile(const std::string &filename)
{
    // 在WebAssembly环境中，文件加载需要通过Emscripten的文件系统API
    // 这里简化处理，实际项目中需要实现文件加载逻辑
    std::cout << "Texture loading from file: " << filename << std::endl;
    return false;
}

bool Texture::createFromData(const unsigned char *data, int width, int height, GLenum format)
{
    if (!data || width <= 0 || height <= 0)
        return false;

    m_width = width;
    m_height = height;
    m_format = format;

    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // 生成mipmap
    // glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture::bind(GLuint unit) const
{
    if (m_textureId != 0)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
    }
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
