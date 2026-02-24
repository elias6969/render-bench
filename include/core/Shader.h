#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
    unsigned int ID;

    Shader() = default;

    // --------------------------------------------------------
    // Shader loading and compilation
    // --------------------------------------------------------
    void LoadShaders(const char* vertexPath, const char* fragmentPath);
    void LoadAdvShaders(const char* vertexPath, const char* fragmentPath,
                        const char* geopath = nullptr);

    void use() const { glUseProgram(ID); }

    // --------------------------------------------------------
    // Generic template uniform setter (specialized below)
    // --------------------------------------------------------
    template <typename T>
    void setUniform(const std::string& name, const T& value) const;

private:
    void CheckShaderCompilation(GLuint shader, const std::string& type);
    void CheckProgramLinking(GLuint program);
    void checkCompileErrors(GLuint shader, std::string type);
};

// ------------------------------------------------------------
// Inline method definitions
// ------------------------------------------------------------

// Utility: check shader compilation
inline void Shader::CheckShaderCompilation(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                  << infoLog
                  << "\n -- --------------------------------------------------- -- "
                  << std::endl;
    }
}

// Utility: check program linking
inline void Shader::CheckProgramLinking(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n"
                  << infoLog
                  << "\n -- --------------------------------------------------- -- "
                  << std::endl;
    }
}

// Utility: check compile errors for both shader and program
inline void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}

// Load simple vertex + fragment shaders
inline void Shader::LoadShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::cout << "SUCCESS::SHADER::PROGRAM::SHADERS_SUCCESSFULLY_LOADED \n"
              << "SUCCESS::PATH::VERTEX::" << vertexPath
              << "\nSUCCESS::FRAGMENT::" << fragmentPath << std::endl;
}

// Load vertex + fragment + geometry shader
inline void Shader::LoadAdvShaders(const char* vertexPath, const char* fragmentPath,
                                   const char* geopath) {
    std::string vertexCode, fragmentCode, geoCode;
    std::ifstream vShaderFile, fShaderFile, gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        if (geopath) gShaderFile.open(geopath);

        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (geopath) gShaderStream << gShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        if (geopath) gShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geoCode      = gShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment, geo;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    if (geopath) {
        const char* gShaderCode = geoCode.c_str();
        geo = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geo, 1, &gShaderCode, NULL);
        glCompileShader(geo);
        checkCompileErrors(geo, "GEOMETRY");
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geopath) glAttachShader(ID, geo);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geopath) glDeleteShader(geo);

    std::cout << "SUCCESS::SHADER::PROGRAM::SHADERS_SUCCESSFULLY_LOADED \n"
              << "SUCCESS::PATH::VERTEX::" << vertexPath
              << "\nSUCCESS::FRAGMENT::" << fragmentPath << std::endl;
}

// ------------------------------------------------------------
// Template specializations
// ------------------------------------------------------------

// int specialization
template <>
inline void Shader::setUniform<int>(const std::string& name, const int& value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// float specialization
template <>
inline void Shader::setUniform<float>(const std::string& name, const float& value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// vec2 specialization
template <>
inline void Shader::setUniform<glm::vec2>(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

// vec3 specialization
template <>
inline void Shader::setUniform<glm::vec3>(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

// vec4 specialization
template <>
inline void Shader::setUniform<glm::vec4>(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

// mat2 specialization
template <>
inline void Shader::setUniform<glm::mat2>(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// mat3 specialization
template <>
inline void Shader::setUniform<glm::mat3>(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// mat4 specialization
template <>
inline void Shader::setUniform<glm::mat4>(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

#endif
