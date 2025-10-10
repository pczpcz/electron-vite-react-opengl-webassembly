#ifndef MATERIAL_H
#define MATERIAL_H

#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "shader.h"
#include "texture.h"

/**
 * @brief 材质类
 *
 * 封装渲染材质属性，包括着色器、纹理、颜色等
 */
class Material
{
public:
    Material();
    explicit Material(std::shared_ptr<Shader> shader);
    ~Material();

    // 禁用拷贝构造和赋值
    Material(const Material &) = delete;
    Material &operator=(const Material &) = delete;

    // 移动构造和赋值
    Material(Material &&other) noexcept;
    Material &operator=(Material &&other) noexcept;

    /**
     * @brief 设置着色器
     * @param shader 着色器对象
     */
    void setShader(std::shared_ptr<Shader> shader);

    /**
     * @brief 获取着色器
     * @return 着色器对象
     */
    std::shared_ptr<Shader> getShader() const { return m_shader; }

    /**
     * @brief 设置颜色属性
     * @param name 属性名称
     * @param r 红色分量
     * @param g 绿色分量
     * @param b 蓝色分量
     * @param a 透明度分量
     */
    void setColor(const std::string &name, float r, float g, float b, float a = 1.0f);

    /**
     * @brief 设置浮点数属性
     * @param name 属性名称
     * @param value 属性值
     */
    void setFloat(const std::string &name, float value);

    /**
     * @brief 设置整数属性
     * @param name 属性名称
     * @param value 属性值
     */
    void setInt(const std::string &name, int value);

    /**
     * @brief 设置布尔属性
     * @param name 属性名称
     * @param value 属性值
     */
    void setBool(const std::string &name, bool value);

    /**
     * @brief 设置纹理
     * @param name 纹理属性名称
     * @param texture 纹理对象
     * @param unit 纹理单元
     */
    void setTexture(const std::string &name, std::shared_ptr<Texture> texture, GLuint unit = 0);

    /**
     * @brief 获取纹理
     * @param name 纹理属性名称
     * @return 纹理对象
     */
    std::shared_ptr<Texture> getTexture(const std::string &name) const;

    /**
     * @brief 应用材质属性到着色器
     */
    void apply();

    /**
     * @brief 检查材质是否有效
     * @return 是否有效
     */
    bool isValid() const { return m_shader != nullptr; }

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, float> m_floatProperties;
    std::unordered_map<std::string, int> m_intProperties;
    std::unordered_map<std::string, bool> m_boolProperties;
    std::unordered_map<std::string, std::vector<float>> m_colorProperties;
    std::unordered_map<std::string, std::pair<std::shared_ptr<Texture>, GLuint>> m_textureProperties;
};

#endif // MATERIAL_H
