#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "gameobject.h"

/**
 * @brief 场景类
 *
 * 管理场景中的所有游戏对象，提供场景级别的操作
 */
class Scene
{
public:
    Scene();
    explicit Scene(const std::string &name);
    ~Scene();

    // 禁用拷贝构造和赋值
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    // 移动构造和赋值
    Scene(Scene &&other) noexcept;
    Scene &operator=(Scene &&other) noexcept;

    /**
     * @brief 获取场景名称
     * @return 场景名称
     */
    const std::string &getName() const { return m_name; }

    /**
     * @brief 设置场景名称
     * @param name 场景名称
     */
    void setName(const std::string &name) { m_name = name; }

    /**
     * @brief 添加游戏对象
     * @param gameObject 游戏对象
     */
    void addGameObject(std::shared_ptr<GameObject> gameObject);

    /**
     * @brief 移除游戏对象
     * @param gameObject 游戏对象
     */
    void removeGameObject(std::shared_ptr<GameObject> gameObject);

    /**
     * @brief 通过名称获取游戏对象
     * @param name 游戏对象名称
     * @return 游戏对象
     */
    std::shared_ptr<GameObject> getGameObject(const std::string &name) const;

    /**
     * @brief 获取所有游戏对象
     * @return 游戏对象列表
     */
    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const { return m_gameObjects; }

    /**
     * @brief 获取所有游戏对象名称
     * @return 游戏对象名称列表
     */
    std::vector<std::string> getGameObjectNames() const;

    /**
     * @brief 清空场景
     */
    void clear();

    /**
     * @brief 检查场景是否为空
     * @return 是否为空
     */
    bool empty() const { return m_gameObjects.empty(); }

    /**
     * @brief 获取游戏对象数量
     * @return 游戏对象数量
     */
    size_t getGameObjectCount() const { return m_gameObjects.size(); }

    /**
     * @brief 场景初始化
     */
    virtual void initialize();

    /**
     * @brief 场景更新
     * @param deltaTime 时间增量
     */
    virtual void update(float deltaTime);

    /**
     * @brief 场景渲染
     */
    virtual void render();

private:
    std::string m_name;
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    std::unordered_map<std::string, std::shared_ptr<GameObject>> m_gameObjectMap;
};

#endif // SCENE_H
