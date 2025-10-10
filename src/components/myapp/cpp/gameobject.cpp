#include "gameobject.h"

GameObject::GameObject()
    : m_name("GameObject"), m_visible(true)
{
    m_position[0] = 0.0f;
    m_position[1] = 0.0f;
    m_position[2] = 0.0f;

    m_rotation[0] = 0.0f;
    m_rotation[1] = 0.0f;
    m_rotation[2] = 0.0f;

    m_scale[0] = 1.0f;
    m_scale[1] = 1.0f;
    m_scale[2] = 1.0f;
}

GameObject::GameObject(const std::string &name)
    : m_name(name), m_visible(true)
{
    m_position[0] = 0.0f;
    m_position[1] = 0.0f;
    m_position[2] = 0.0f;

    m_rotation[0] = 0.0f;
    m_rotation[1] = 0.0f;
    m_rotation[2] = 0.0f;

    m_scale[0] = 1.0f;
    m_scale[1] = 1.0f;
    m_scale[2] = 1.0f;
}

GameObject::~GameObject()
{
}

GameObject::GameObject(GameObject &&other) noexcept
    : m_name(std::move(other.m_name)),
      m_meshes(std::move(other.m_meshes)),
      m_visible(other.m_visible)
{
    m_position[0] = other.m_position[0];
    m_position[1] = other.m_position[1];
    m_position[2] = other.m_position[2];

    m_rotation[0] = other.m_rotation[0];
    m_rotation[1] = other.m_rotation[1];
    m_rotation[2] = other.m_rotation[2];

    m_scale[0] = other.m_scale[0];
    m_scale[1] = other.m_scale[1];
    m_scale[2] = other.m_scale[2];
}

GameObject &GameObject::operator=(GameObject &&other) noexcept
{
    if (this != &other)
    {
        m_name = std::move(other.m_name);
        m_meshes = std::move(other.m_meshes);
        m_visible = other.m_visible;

        m_position[0] = other.m_position[0];
        m_position[1] = other.m_position[1];
        m_position[2] = other.m_position[2];

        m_rotation[0] = other.m_rotation[0];
        m_rotation[1] = other.m_rotation[1];
        m_rotation[2] = other.m_rotation[2];

        m_scale[0] = other.m_scale[0];
        m_scale[1] = other.m_scale[1];
        m_scale[2] = other.m_scale[2];
    }
    return *this;
}

void GameObject::setName(const std::string &name)
{
    m_name = name;
}

void GameObject::setPosition(float x, float y, float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void GameObject::setRotation(float x, float y, float z)
{
    m_rotation[0] = x;
    m_rotation[1] = y;
    m_rotation[2] = z;
}

void GameObject::setScale(float x, float y, float z)
{
    m_scale[0] = x;
    m_scale[1] = y;
    m_scale[2] = z;
}

void GameObject::addMesh(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);
}

void GameObject::render()
{
    if (!m_visible)
        return;

    // 渲染所有网格
    for (auto &mesh : m_meshes)
    {
        if (mesh)
        {
            mesh->render();
        }
    }
}

void GameObject::initialize()
{
    // 初始化所有网格
    for (auto &mesh : m_meshes)
    {
        if (mesh)
        {
            mesh->initialize();
        }
    }
}

void GameObject::update(float deltaTime)
{
    // 更新游戏对象逻辑
    // 这里可以添加动画、物理等更新逻辑
    // 例如：m_position[0] += 0.1f * deltaTime; // 简单的移动示例
}
