#include "scenemanager.h"
#include <vector>
#include <algorithm>

SceneManager::SceneManager()
    : m_currentScene(nullptr), m_currentSceneName("")
{
}

SceneManager::~SceneManager()
{
    clear();
}

SceneManager::SceneManager(SceneManager &&other) noexcept
    : m_scenes(std::move(other.m_scenes)),
      m_currentScene(std::move(other.m_currentScene)),
      m_currentSceneName(std::move(other.m_currentSceneName))
{
}

SceneManager &SceneManager::operator=(SceneManager &&other) noexcept
{
    if (this != &other)
    {
        m_scenes = std::move(other.m_scenes);
        m_currentScene = std::move(other.m_currentScene);
        m_currentSceneName = std::move(other.m_currentSceneName);
    }
    return *this;
}

void SceneManager::addScene(const std::string &name, std::shared_ptr<Scene> scene)
{
    if (!scene)
        return;

    m_scenes[name] = scene;

    // 如果没有当前场景，设置第一个添加的场景为当前场景
    if (!m_currentScene)
    {
        m_currentScene = scene;
        m_currentSceneName = name;
    }
}

void SceneManager::removeScene(const std::string &name)
{
    auto it = m_scenes.find(name);
    if (it != m_scenes.end())
    {
        // 如果要移除的是当前场景，需要重置当前场景
        if (m_currentScene == it->second)
        {
            m_currentScene = nullptr;
            m_currentSceneName = "";

            // 尝试设置另一个场景为当前场景
            if (!m_scenes.empty())
            {
                auto firstScene = m_scenes.begin();
                m_currentScene = firstScene->second;
                m_currentSceneName = firstScene->first;
            }
        }

        m_scenes.erase(it);
    }
}

std::shared_ptr<Scene> SceneManager::getScene(const std::string &name) const
{
    auto it = m_scenes.find(name);
    if (it != m_scenes.end())
    {
        return it->second;
    }
    return nullptr;
}

bool SceneManager::switchScene(const std::string &name)
{
    auto newScene = getScene(name);
    if (newScene)
    {
        m_currentScene = newScene;
        m_currentSceneName = name;
        return true;
    }
    return false;
}

std::vector<std::string> SceneManager::getSceneNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : m_scenes)
    {
        names.push_back(pair.first);
    }
    return names;
}

bool SceneManager::hasScene(const std::string &name) const
{
    return m_scenes.find(name) != m_scenes.end();
}

void SceneManager::clear()
{
    m_scenes.clear();
    m_currentScene = nullptr;
    m_currentSceneName = "";
}

void SceneManager::initializeCurrentScene()
{
    if (m_currentScene)
    {
        m_currentScene->initialize();
    }
}

void SceneManager::updateCurrentScene(float deltaTime)
{
    if (m_currentScene)
    {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::renderCurrentScene()
{
    if (m_currentScene)
    {
        m_currentScene->render();
    }
}
