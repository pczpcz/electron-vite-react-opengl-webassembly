#include "material.h"

Material::Material()
    : m_shader(nullptr)
{
}

Material::Material(std::shared_ptr<Shader> shader)
    : m_shader(shader)
{
}

Material::~Material()
{
}

Material::Material(Material &&other) noexcept
    : m_shader(std::move(other.m_shader)),
      m_floatProperties(std::move(other.m_floatProperties)),
      m_intProperties(std::move(other.m_intProperties)),
      m_boolProperties(std::move(other.m_boolProperties)),
      m_colorProperties(std::move(other.m_colorProperties)),
      m_textureProperties(std::move(other.m_textureProperties))
{
}

Material &Material::operator=(Material &&other) noexcept
{
    if (this != &other)
    {
        m_shader = std::move(other.m_shader);
        m_floatProperties = std::move(other.m_floatProperties);
        m_intProperties = std::move(other.m_intProperties);
        m_boolProperties = std::move(other.m_boolProperties);
        m_colorProperties = std::move(other.m_colorProperties);
        m_textureProperties = std::move(other.m_textureProperties);
    }
    return *this;
}

void Material::setShader(std::shared_ptr<Shader> shader)
{
    m_shader = shader;
}

void Material::setColor(const std::string &name, float r, float g, float b, float a)
{
    m_colorProperties[name] = {r, g, b, a};
}

void Material::setFloat(const std::string &name, float value)
{
    m_floatProperties[name] = value;
}

void Material::setInt(const std::string &name, int value)
{
    m_intProperties[name] = value;
}

void Material::setBool(const std::string &name, bool value)
{
    m_boolProperties[name] = value;
}

void Material::setTexture(const std::string &name, std::shared_ptr<Texture> texture, GLuint unit)
{
    m_textureProperties[name] = std::make_pair(texture, unit);
}

std::shared_ptr<Texture> Material::getTexture(const std::string &name) const
{
    auto it = m_textureProperties.find(name);
    if (it != m_textureProperties.end())
    {
        return it->second.first;
    }
    return nullptr;
}

void Material::apply()
{
    if (!m_shader || !m_shader->isValid())
        return;

    m_shader->use();

    // 应用浮点数属性
    for (const auto &[name, value] : m_floatProperties)
    {
        m_shader->setFloat(name, value);
    }

    // 应用整数属性
    for (const auto &[name, value] : m_intProperties)
    {
        m_shader->setInt(name, value);
    }

    // 应用布尔属性
    for (const auto &[name, value] : m_boolProperties)
    {
        m_shader->setBool(name, value);
    }

    // 应用颜色属性
    for (const auto &[name, color] : m_colorProperties)
    {
        if (color.size() >= 3)
        {
            if (color.size() >= 4)
            {
                m_shader->setVec4(name, color[0], color[1], color[2], color[3]);
            }
            else
            {
                m_shader->setVec3(name, color[0], color[1], color[2]);
            }
        }
    }

    // 应用纹理属性
    for (const auto &[name, textureInfo] : m_textureProperties)
    {
        auto texture = textureInfo.first;
        GLuint unit = textureInfo.second;

        if (texture && texture->isValid())
        {
            texture->bind(unit);
            m_shader->setInt(name, unit);
        }
    }
}
