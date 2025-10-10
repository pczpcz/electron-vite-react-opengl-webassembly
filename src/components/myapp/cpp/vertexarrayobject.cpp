#include "vertexarrayobject.h"
#include "bufferobject.h"

#include <GLES3/gl3.h>

VertexArrayObject::VertexArrayObject()
    : m_id(0)
{
    glGenVertexArrays(1, &m_id);
}

VertexArrayObject::~VertexArrayObject()
{
    if (m_id != 0)
    {
        glDeleteVertexArrays(1, &m_id);
    }
}

VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

VertexArrayObject &VertexArrayObject::operator=(VertexArrayObject &&other) noexcept
{
    if (this != &other)
    {
        if (m_id != 0)
        {
            glDeleteVertexArrays(1, &m_id);
        }
        m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void VertexArrayObject::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArrayObject::unbind() const
{
    glBindVertexArray(0);
}

void VertexArrayObject::setVertexAttribPointer(GLuint index, GLint size, GLenum type,
                                               GLboolean normalized, GLsizei stride, const void *pointer)
{
    bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void VertexArrayObject::enableVertexAttribArray(GLuint index)
{
    bind();
    glEnableVertexAttribArray(index);
}

void VertexArrayObject::disableVertexAttribArray(GLuint index)
{
    // bind();
    // glDisableVertexAttribArray(index);
}

void VertexArrayObject::bindElementBuffer(const BufferObject &ebo)
{
    bind();
    ebo.bind();
}

void VertexArrayObject::bindVertexBuffer(const BufferObject &vbo)
{
    bind();
    vbo.bind();
}
