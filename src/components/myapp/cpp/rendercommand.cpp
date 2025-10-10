#include "rendercommand.h"
#include <GLFW/glfw3.h>

FunctionalRenderCommand::FunctionalRenderCommand(CommandFunction func)
    : m_function(std::move(func))
{
}

void FunctionalRenderCommand::execute()
{
    if (m_function)
    {
        m_function();
    }
}

bool FunctionalRenderCommand::isValid() const
{
    return static_cast<bool>(m_function);
}

ClearCommand::ClearCommand(float r, float g, float b, float a, unsigned int mask)
    : m_clearMask(mask)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void ClearCommand::execute()
{
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(m_clearMask);
}

RenderCommandQueue::RenderCommandQueue()
{
}

RenderCommandQueue::~RenderCommandQueue()
{
    clear();
}

RenderCommandQueue::RenderCommandQueue(RenderCommandQueue &&other) noexcept
    : m_commands(std::move(other.m_commands))
{
}

RenderCommandQueue &RenderCommandQueue::operator=(RenderCommandQueue &&other) noexcept
{
    if (this != &other)
    {
        m_commands = std::move(other.m_commands);
    }
    return *this;
}

void RenderCommandQueue::addCommand(std::unique_ptr<RenderCommand> command)
{
    if (command && command->isValid())
    {
        m_commands.push_back(std::move(command));
    }
}

void RenderCommandQueue::addCommand(std::function<void()> func)
{
    if (func)
    {
        m_commands.push_back(std::make_unique<FunctionalRenderCommand>(std::move(func)));
    }
}

void RenderCommandQueue::executeAll()
{
    for (auto &command : m_commands)
    {
        if (command && command->isValid())
        {
            command->execute();
        }
    }
}

void RenderCommandQueue::clear()
{
    m_commands.clear();
}
