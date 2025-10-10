#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <functional>
#include "gameobject.h"
#include "rendercommand.h"

/**
 * @brief 渲染过程类
 *
 * 封装一个完整的渲染过程，包括渲染目标、渲染状态和渲染对象
 */
class RenderPass
{
public:
    /**
     * @brief 渲染回调函数类型
     */
    using RenderCallback = std::function<void()>;

    RenderPass();
    ~RenderPass();

    // 禁用拷贝构造和赋值
    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;

    // 移动构造和赋值
    RenderPass(RenderPass &&other) noexcept;
    RenderPass &operator=(RenderPass &&other) noexcept;

    /**
     * @brief 设置清除颜色
     * @param r 红色分量
     * @param g 绿色分量
     * @param b 蓝色分量
     * @param a 透明度分量
     */
    void setClearColor(float r, float g, float b, float a = 1.0f);

    /**
     * @brief 设置清除掩码
     * @param mask 清除掩码 (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT 等)
     */
    void setClearMask(GLbitfield mask);

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
     * @brief 获取所有游戏对象
     * @return 游戏对象列表
     */
    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const { return m_gameObjects; }

    /**
     * @brief 获取所有网格引用
     * @return 网格引用列表
     */
    std::vector<std::shared_ptr<Mesh>> getAllMeshes() const;

    /**
     * @brief 获取所有材质引用
     * @return 材质引用列表
     */
    std::vector<std::shared_ptr<Material>> getAllMaterials() const;

    /**
     * @brief 设置渲染前回调
     * @param callback 回调函数
     */
    void setPreRenderCallback(RenderCallback callback);

    /**
     * @brief 设置渲染后回调
     * @param callback 回调函数
     */
    void setPostRenderCallback(RenderCallback callback);

    /**
     * @brief 执行渲染过程
     */
    void render();

    /**
     * @brief 启用/禁用渲染过程
     * @param enabled 是否启用
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }

    /**
     * @brief 检查渲染过程是否启用
     * @return 是否启用
     */
    bool isEnabled() const { return m_enabled; }

private:
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    RenderCallback m_preRenderCallback;
    RenderCallback m_postRenderCallback;
    float m_clearColor[4];
    GLbitfield m_clearMask;
    bool m_enabled;
};

#endif // RENDERPASS_H
