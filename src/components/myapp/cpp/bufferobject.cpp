#include "bufferobject.h"
#include <GLES3/gl3.h>

BufferObject::BufferObject(Type type)
    : m_id(0), m_type(type), m_size(0)
{
    glGenBuffers(1, &m_id);
}

BufferObject::~BufferObject()
{
    if (m_id != 0)
    {
        glDeleteBuffers(1, &m_id);
    }
}

BufferObject::BufferObject(BufferObject &&other) noexcept
    : m_id(other.m_id), m_type(other.m_type), m_size(other.m_size)
{
    other.m_id = 0;
    other.m_size = 0;
}

BufferObject &BufferObject::operator=(BufferObject &&other) noexcept
{
    if (this != &other)
    {
        if (m_id != 0)
        {
            glDeleteBuffers(1, &m_id);
        }
        m_id = other.m_id;
        m_type = other.m_type;
        m_size = other.m_size;
        other.m_id = 0;
        other.m_size = 0;
    }
    return *this;
}

void BufferObject::bind() const
{
    glBindBuffer(static_cast<GLenum>(m_type), m_id);
}

void BufferObject::unbind() const
{
    glBindBuffer(static_cast<GLenum>(m_type), 0);
}

void BufferObject::setData(const void *data, GLsizeiptr size, Usage usage)
{
    bind();
    glBufferData(static_cast<GLenum>(m_type), size, data, static_cast<GLenum>(usage));
    m_size = size;
}

void BufferObject::updateData(GLintptr offset, const void *data, GLsizeiptr size)
{
    bind();
    glBufferSubData(static_cast<GLenum>(m_type), offset, size, data);
}
