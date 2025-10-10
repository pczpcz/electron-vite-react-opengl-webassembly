#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include "scene.h"

/**
 * @brief 场景管理器类
 *
 * 管理多个场景，提供场景切换、加载、卸载等功能
 */
class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    // 禁用拷贝构造和赋值
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;

    // 移动构造和赋值
    SceneManager(SceneManager &&other) noexcept;
    SceneManager &operator=(SceneManager &&other) noexcept;

    /**
     * @brief 添加场景
     * @param name 场景名称
     * @param scene 场景对象
     */
    void addScene(const std::string &name, std::shared_ptr<Scene> scene);

    /**
     * @brief 移除场景
     * @param name 场景名称
     */
    void removeScene(const std::string &name);

    /**
     * @brief 获取场景
     * @param name 场景名称
     * @return 场景对象
     */
    std::shared_ptr<Scene> getScene(const std::string &name) const;

    /**
     * @brief 切换当前场景
     * @param name 场景名称
     * @return 是否切换成功
     */
    bool switchScene(const std::string &name);

    /**
     * @brief 获取当前场景
     * @return 当前场景对象
     */
    std::shared_ptr<Scene> getCurrentScene() const { return m_currentScene; }

    /**
     * @brief 获取当前场景名称
     * @return 当前场景名称
     */
    const std::string &getCurrentSceneName() const { return m_currentSceneName; }

    /**
     * @brief 获取所有场景名称
     * @return 场景名称列表
     */
    std::vector<std::string> getSceneNames() const;

    /**
     * @brief 检查场景是否存在
     * @param name 场景名称
     * @return 是否存在
     */
    bool hasScene(const std::string &name) const;

    /**
     * @brief 获取场景数量
     * @return 场景数量
     */
    size_t getSceneCount() const { return m_scenes.size(); }

    /**
     * @brief 清空所有场景
     */
    void clear();

    /**
     * @brief 初始化当前场景
     */
    void initializeCurrentScene();

    /**
     * @brief 更新当前场景
     * @param deltaTime 时间增量
     */
    void updateCurrentScene(float deltaTime);

    /**
     * @brief 渲染当前场景
     */
    void renderCurrentScene();

private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
    std::shared_ptr<Scene> m_currentScene;
    std::string m_currentSceneName;
};

#endif // SCENEMANAGER_H
