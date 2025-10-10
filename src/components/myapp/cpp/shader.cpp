#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // 使用从内存编译的方法
    unsigned int vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    m_IsValid = linkProgram(vertexShader, fragmentShader);

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const char *vertexSource, const char *fragmentSource, bool fromMemory)
{
    if (!fromMemory)
    {
        m_IsValid = false;
        return;
    }

    // 直接从内存源码编译着色器
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    m_IsValid = linkProgram(vertexShader, fragmentShader);

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    if (m_IsValid)
    {
        glDeleteProgram(ID);
    }
}

void Shader::use()
{
    if (m_IsValid)
    {
        glUseProgram(ID);
    }
}

unsigned int Shader::compileShader(const char *source, GLenum shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cout << "ERROR::SHADER::" << shaderTypeStr << "::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}

bool Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }

    return true;
}

void Shader::setBool(const std::string &name, bool value) const
{
    if (m_IsValid)
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
}

void Shader::setInt(const std::string &name, int value) const
{
    if (m_IsValid)
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void Shader::setFloat(const std::string &name, float value) const
{
    if (m_IsValid)
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    if (m_IsValid)
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    if (m_IsValid)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
}
