#include "renderpipeline.h"

RenderPipeline::RenderPipeline()
{
}

RenderPipeline::~RenderPipeline()
{
    clear();
}

RenderPipeline::RenderPipeline(RenderPipeline &&other) noexcept
    : m_renderPasses(std::move(other.m_renderPasses)),
      m_renderOrder(std::move(other.m_renderOrder))
{
}

RenderPipeline &RenderPipeline::operator=(RenderPipeline &&other) noexcept
{
    if (this != &other)
    {
        m_renderPasses = std::move(other.m_renderPasses);
        m_renderOrder = std::move(other.m_renderOrder);
    }
    return *this;
}

void RenderPipeline::addRenderPass(const std::string &name, std::shared_ptr<RenderPass> renderPass)
{
    if (m_renderPasses.find(name) == m_renderPasses.end())
    {
        m_renderOrder.push_back(name);
    }
    m_renderPasses[name] = renderPass;
}

void RenderPipeline::removeRenderPass(const std::string &name)
{
    auto it = m_renderPasses.find(name);
    if (it != m_renderPasses.end())
    {
        m_renderPasses.erase(it);

        // 从渲染顺序中移除
        auto orderIt = std::find(m_renderOrder.begin(), m_renderOrder.end(), name);
        if (orderIt != m_renderOrder.end())
        {
            m_renderOrder.erase(orderIt);
        }
    }
}

std::shared_ptr<RenderPass> RenderPipeline::getRenderPass(const std::string &name) const
{
    auto it = m_renderPasses.find(name);
    if (it != m_renderPasses.end())
    {
        return it->second;
    }
    return nullptr;
}

void RenderPipeline::setRenderPassEnabled(const std::string &name, bool enabled)
{
    auto renderPass = getRenderPass(name);
    if (renderPass)
    {
        renderPass->setEnabled(enabled);
    }
}

bool RenderPipeline::isRenderPassEnabled(const std::string &name) const
{
    auto renderPass = getRenderPass(name);
    if (renderPass)
    {
        return renderPass->isEnabled();
    }
    return false;
}

void RenderPipeline::render()
{
    // 按照渲染顺序执行所有启用的渲染过程
    for (const auto &name : m_renderOrder)
    {
        auto renderPass = getRenderPass(name);
        if (renderPass && renderPass->isEnabled())
        {
            renderPass->render();
        }
    }
}

void RenderPipeline::clear()
{
    m_renderPasses.clear();
    m_renderOrder.clear();
}

std::vector<std::string> RenderPipeline::getRenderPassNames() const
{
    return m_renderOrder;
}
