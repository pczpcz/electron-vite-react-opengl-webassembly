#ifndef RENDERCOMMAND_H
#define RENDERCOMMAND_H

#include <functional>
#include <memory>
#include <vector>

/**
 * @brief 渲染命令基类
 *
 * 使用命令模式解耦逻辑和渲染，所有渲染操作都通过命令执行
 */
class RenderCommand
{
public:
    virtual ~RenderCommand() = default;

    /**
     * @brief 执行渲染命令
     */
    virtual void execute() = 0;

    /**
     * @brief 检查命令是否有效
     * @return 是否有效
     */
    virtual bool isValid() const = 0;
};

/**
 * @brief 函数式渲染命令
 *
 * 使用函数对象包装渲染操作
 */
class FunctionalRenderCommand : public RenderCommand
{
public:
    using CommandFunction = std::function<void()>;

    explicit FunctionalRenderCommand(CommandFunction func);
    ~FunctionalRenderCommand() override = default;

    void execute() override;
    bool isValid() const override;

private:
    CommandFunction m_function;
};

/**
 * @brief 清除缓冲区命令
 */
class ClearCommand : public RenderCommand
{
public:
    ClearCommand(float r, float g, float b, float a, unsigned int mask);
    ~ClearCommand() override = default;

    void execute() override;
    bool isValid() const override { return true; }

private:
    float m_clearColor[4];
    unsigned int m_clearMask;
};

/**
 * @brief 渲染命令队列
 *
 * 管理渲染命令的执行顺序
 */
class RenderCommandQueue
{
public:
    RenderCommandQueue();
    ~RenderCommandQueue();

    // 禁用拷贝构造和赋值
    RenderCommandQueue(const RenderCommandQueue &) = delete;
    RenderCommandQueue &operator=(const RenderCommandQueue &) = delete;

    // 移动构造和赋值
    RenderCommandQueue(RenderCommandQueue &&other) noexcept;
    RenderCommandQueue &operator=(RenderCommandQueue &&other) noexcept;

    /**
     * @brief 添加渲染命令
     * @param command 渲染命令
     */
    void addCommand(std::unique_ptr<RenderCommand> command);

    /**
     * @brief 添加函数式渲染命令
     * @param func 渲染函数
     */
    void addCommand(std::function<void()> func);

    /**
     * @brief 执行所有渲染命令
     */
    void executeAll();

    /**
     * @brief 清空命令队列
     */
    void clear();

    /**
     * @brief 获取命令数量
     * @return 命令数量
     */
    size_t size() const { return m_commands.size(); }

    /**
     * @brief 检查队列是否为空
     * @return 是否为空
     */
    bool empty() const { return m_commands.empty(); }

private:
    std::vector<std::unique_ptr<RenderCommand>> m_commands;
};

#endif // RENDERCOMMAND_H
