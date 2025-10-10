#include "scene.h"

Scene::Scene()
    : m_name("DefaultScene")
{
}

Scene::Scene(const std::string &name)
    : m_name(name)
{
}

Scene::~Scene()
{
    clear();
}

Scene::Scene(Scene &&other) noexcept
    : m_name(std::move(other.m_name)),
      m_gameObjects(std::move(other.m_gameObjects)),
      m_gameObjectMap(std::move(other.m_gameObjectMap))
{
}

Scene &Scene::operator=(Scene &&other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_gameObjects = std::move(other.m_gameObjects);
        m_gameObjectMap = std::move(other.m_gameObjectMap);
    }
    return *this;
}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject)
{
    if (!gameObject)
        return;

    // 检查是否已存在同名游戏对象
    auto it = m_gameObjectMap.find(gameObject->getName());
    if (it != m_gameObjectMap.end())
    {
        // 如果已存在，先移除旧的
        removeGameObject(it->second);
    }

    m_gameObjects.push_back(gameObject);
    m_gameObjectMap[gameObject->getName()] = gameObject;
}

void Scene::removeGameObject(std::shared_ptr<GameObject> gameObject)
{
    if (!gameObject)
        return;

    // 从向量中移除
    auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
    if (it != m_gameObjects.end())
    {
        m_gameObjects.erase(it);
    }

    // 从映射中移除
    auto mapIt = m_gameObjectMap.find(gameObject->getName());
    if (mapIt != m_gameObjectMap.end() && mapIt->second == gameObject)
    {
        m_gameObjectMap.erase(mapIt);
    }
}

std::shared_ptr<GameObject> Scene::getGameObject(const std::string &name) const
{
    auto it = m_gameObjectMap.find(name);
    if (it != m_gameObjectMap.end())
    {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> Scene::getGameObjectNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : m_gameObjectMap)
    {
        names.push_back(pair.first);
    }
    return names;
}

void Scene::clear()
{
    m_gameObjects.clear();
    m_gameObjectMap.clear();
}

void Scene::initialize()
{
    // 初始化所有游戏对象
    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            gameObject->initialize();
        }
    }
}

void Scene::update(float deltaTime)
{
    // 更新所有游戏对象
    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            gameObject->update(deltaTime);
        }
    }
}

void Scene::render()
{
    // 渲染所有游戏对象
    for (auto &gameObject : m_gameObjects)
    {
        if (gameObject)
        {
            gameObject->render();
        }
    }
}
