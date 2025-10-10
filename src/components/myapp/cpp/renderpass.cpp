#include "renderpass.h"

RenderPass::RenderPass()
    : m_clearMask(GL_COLOR_BUFFER_BIT), m_enabled(true)
{
    m_clearColor[0] = 0.2f;
    m_clearColor[1] = 0.3f;
    m_clearColor[2] = 0.3f;
    m_clearColor[3] = 1.0f;
}

RenderPass::~RenderPass()
{
}

RenderPass::RenderPass(RenderPass &&other) noexcept
    : m_gameObjects(std::move(other.m_gameObjects)),
      m_preRenderCallback(std::move(other.m_preRenderCallback)),
      m_postRenderCallback(std::move(other.m_postRenderCallback)),
      m_clearMask(other.m_clearMask),
      m_enabled(other.m_enabled)
{
    m_clearColor[0] = other.m_clearColor[0];
    m_clearColor[1] = other.m_clearColor[1];
    m_clearColor[2] = other.m_clearColor[2];
    m_clearColor[3] = other.m_clearColor[3];
}

RenderPass &RenderPass::operator=(RenderPass &&other) noexcept
{
    if (this != &other)
    {
        m_gameObjects = std::move(other.m_gameObjects);
        m_preRenderCallback = std::move(other.m_preRenderCallback);
        m_postRenderCallback = std::move(other.m_postRenderCallback);
        m_clearMask = other.m_clearMask;
        m_enabled = other.m_enabled;

        m_clearColor[0] = other.m_clearColor[0];
        m_clearColor[1] = other.m_clearColor[1];
        m_clearColor[2] = other.m_clearColor[2];
        m_clearColor[3] = other.m_clearColor[3];
    }
    return *this;
}

void RenderPass::setClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void RenderPass::setClearMask(GLbitfield mask)
{
    m_clearMask = mask;
}

void RenderPass::addGameObject(std::shared_ptr<GameObject> gameObject)
{
    m_gameObjects.push_back(gameObject);
}

void RenderPass::removeGameObject(std::shared_ptr<GameObject> gameObject)
{
    auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
    if (it != m_gameObjects.end())
    {
        m_gameObjects.erase(it);
    }
}

void RenderPass::setPreRenderCallback(RenderCallback callback)
{
    m_preRenderCallback = callback;
}

void RenderPass::setPostRenderCallback(RenderCallback callback)
{
    m_postRenderCallback = callback;
}

std::vector<std::shared_ptr<Mesh>> RenderPass::getAllMeshes() const
{
    std::vector<std::shared_ptr<Mesh>> meshes;

    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            auto gameObjectMeshes = gameObject->getMeshes();
            meshes.insert(meshes.end(), gameObjectMeshes.begin(), gameObjectMeshes.end());
        }
    }

    return meshes;
}

std::vector<std::shared_ptr<Material>> RenderPass::getAllMaterials() const
{
    std::vector<std::shared_ptr<Material>> materials;

    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            auto gameObjectMeshes = gameObject->getMeshes();
            for (auto &mesh : gameObjectMeshes)
            {
                if (mesh && mesh->getMaterial())
                {
                    materials.push_back(mesh->getMaterial());
                }
            }
        }
    }

    return materials;
}

void RenderPass::render()
{
    if (!m_enabled)
        return;

    // 创建渲染命令队列
    RenderCommandQueue commandQueue;

    // 添加清除命令
    commandQueue.addCommand(std::make_unique<ClearCommand>(
        m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3], m_clearMask));

    // 添加渲染前回调命令
    if (m_preRenderCallback)
    {
        commandQueue.addCommand(m_preRenderCallback);
    }

    // 添加游戏对象渲染命令
    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            commandQueue.addCommand([gameObject]()
                                    { gameObject->render(); });
        }
    }

    // 添加渲染后回调命令
    if (m_postRenderCallback)
    {
        commandQueue.addCommand(m_postRenderCallback);
    }

    // 执行所有渲染命令
    commandQueue.executeAll();
}
