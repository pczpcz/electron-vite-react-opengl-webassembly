#ifndef SHADER_H
#define SHADER_H

#include <GLES3/gl3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // 程序ID
    unsigned int ID;
    bool m_IsValid = false;

    // 构造器读取并构建着色器
    Shader(const char *vertexPath, const char *fragmentPath);

    // 从内存源码构造着色器
    Shader(const char *vertexSource, const char *fragmentSource, bool fromMemory);

    ~Shader();

    // 使用/激活程序
    void use();

    // 检查着色器是否有效
    bool isValid() const { return m_IsValid; }

    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;

private:
    // 编译着色器
    unsigned int compileShader(const char *source, GLenum shaderType);

    // 链接程序
    bool linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
};

#endif
