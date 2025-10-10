#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <string>
#include "mesh.h"

/**
 * @brief 游戏对象类
 *
 * 表示场景中的一个实体，包含变换信息和渲染组件
 */
class GameObject
{
public:
    GameObject();
    explicit GameObject(const std::string &name);
    ~GameObject();

    // 禁用拷贝构造和赋值
    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;

    // 移动构造和赋值
    GameObject(GameObject &&other) noexcept;
    GameObject &operator=(GameObject &&other) noexcept;

    /**
     * @brief 设置名称
     * @param name 对象名称
     */
    void setName(const std::string &name);

    /**
     * @brief 获取名称
     * @return 对象名称
     */
    const std::string &getName() const { return m_name; }

    /**
     * @brief 设置位置
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     */
    void setPosition(float x, float y, float z);

    /**
     * @brief 设置旋转
     * @param x X轴旋转角度
     * @param y Y轴旋转角度
     * @param z Z轴旋转角度
     */
    void setRotation(float x, float y, float z);

    /**
     * @brief 设置缩放
     * @param x X轴缩放
     * @param y Y轴缩放
     * @param z Z轴缩放
     */
    void setScale(float x, float y, float z);

    /**
     * @brief 获取位置
     * @return 位置数组 [x, y, z]
     */
    const float *getPosition() const { return m_position; }

    /**
     * @brief 获取旋转
     * @return 旋转数组 [x, y, z]
     */
    const float *getRotation() const { return m_rotation; }

    /**
     * @brief 获取缩放
     * @return 缩放数组 [x, y, z]
     */
    const float *getScale() const { return m_scale; }

    /**
     * @brief 添加网格
     * @param mesh 网格对象
     */
    void addMesh(std::shared_ptr<Mesh> mesh);

    /**
     * @brief 获取所有网格
     * @return 网格对象列表
     */
    const std::vector<std::shared_ptr<Mesh>> &getMeshes() const { return m_meshes; }

    /**
     * @brief 渲染游戏对象
     */
    void render();

    /**
     * @brief 检查对象是否可见
     * @return 是否可见
     */
    bool isVisible() const { return m_visible; }

    /**
     * @brief 设置可见性
     * @param visible 是否可见
     */
    void setVisible(bool visible) { m_visible = visible; }

    /**
     * @brief 初始化游戏对象
     */
    void initialize();

    /**
     * @brief 更新游戏对象
     * @param deltaTime 时间增量
     */
    void update(float deltaTime);

private:
    std::string m_name;
    float m_position[3];
    float m_rotation[3];
    float m_scale[3];
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    bool m_visible;
};

#endif // GAMEOBJECT_H
