#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "renderpass.h"

/**
 * @brief 渲染管线类
 *
 * 管理多个渲染过程，支持多pass渲染架构
 */
class RenderPipeline
{
public:
    RenderPipeline();
    ~RenderPipeline();

    // 禁用拷贝构造和赋值
    RenderPipeline(const RenderPipeline &) = delete;
    RenderPipeline &operator=(const RenderPipeline &) = delete;

    // 移动构造和赋值
    RenderPipeline(RenderPipeline &&other) noexcept;
    RenderPipeline &operator=(RenderPipeline &&other) noexcept;

    /**
     * @brief 添加渲染过程
     * @param name 渲染过程名称
     * @param renderPass 渲染过程对象
     */
    void addRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass);

    /**
     * @brief 移除渲染过程
     * @param name 渲染过程名称
     */
    void removeRenderPass(const std::string &name);

    /**
     * @brief 获取渲染过程
     * @param name 渲染过程名称
     * @return 渲染过程对象
     */
    std::shared_ptr<RenderPass> getRenderPass(const std::string &name) const;

    /**
     * @brief 启用/禁用渲染过程
     * @param name 渲染过程名称
     * @param enabled 是否启用
     */
    void setRenderPassEnabled(const std::string &name, bool enabled);

    /**
     * @brief 检查渲染过程是否启用
     * @param name 渲染过程名称
     * @return 是否启用
     */
    bool isRenderPassEnabled(const std::string &name) const;

    /**
     * @brief 执行整个渲染管线
     */
    void render();

    /**
     * @brief 获取渲染过程数量
     * @return 渲染过程数量
     */
    size_t getRenderPassCount() const { return m_renderPasses.size(); }

    /**
     * @brief 清空渲染管线
     */
    void clear();

    /**
     * @brief 检查管线是否为空
     * @return 是否为空
     */
    bool empty() const { return m_renderPasses.empty(); }

    /**
     * @brief 获取所有渲染过程名称
     * @return 渲染过程名称列表
     */
    std::vector<std::string> getRenderPassNames() const;

private:
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_renderPasses;
    std::vector<std::string> m_renderOrder;
};

#endif // RENDERPIPELINE_H
