//
// Created by kamil-hp on 04.02.2021.
//

#include "Shader.h"
#include "Util.h"
#include <GL/glew.h>
#include <iostream>
#ifdef USE_STD_FORMAT
#include <format>
#endif
//file based on https://learnopengl.com tutorials for OpenGL

std::string getShaderTypeName(int shaderType) {
    switch(shaderType) {
        case GL_VERTEX_SHADER:
            return "SHADER";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT";
        case GL_GEOMETRY_SHADER:
            return "GEOMETRY";
        case GL_TESS_CONTROL_SHADER:
            return "GL_TESS_CONTROL_SHADER";
        case GL_TESS_EVALUATION_SHADER:
            return "GL_TESS_EVALUATION_SHADER";
        default:
            return "";
    }
}

unsigned bf::Shader::compileShaderFromFile(const std::string& path, int shaderType) const {
    std::string code = readWholeFile(path);
    if(code.empty())
#ifdef USE_STD_FORMAT
        std::cerr << std::format("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {}\n", path);
#else
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << path << "\n";
#endif
    const char* codePtr = code.c_str();
    unsigned index = glCreateShader(shaderType);
    glShaderSource(index, 1, &codePtr, nullptr);
    glCompileShader(index);
    checkCompileErrors(index, getShaderTypeName(shaderType));
    return index;
}

bf::Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
    bool isGeometryShaderUsed = !geometryPath.empty();
    unsigned int vertex = compileShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = compileShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
	unsigned int geometry;
	if(isGeometryShaderUsed) {
        geometry = compileShaderFromFile(geometryPath, GL_GEOMETRY_SHADER);
	}
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if(!geometryPath.empty())
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if(isGeometryShaderUsed)
		glDeleteShader(geometry);
    std::cout << std::format("Shader {} {} loaded successfully\n",vertexPath,fragmentPath);
}
void bf::Shader::use() const
{
	glUseProgram(ID);
}
void bf::Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}
void bf::Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void bf::Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void bf::Shader::checkCompileErrors(unsigned int shader, const std::string& type) const
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
#ifdef USE_STD_FORMAT
            std::cerr << std::format("ERROR::SHADER_COMPILATION_ERROR of type: {0}\n{1}",type,infoLog);
#else
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog;
#endif
            std::cerr <<  "\n -- --------------------------------------------------- --\n";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
#ifdef USE_STD_FORMAT
			std::cerr << std::format("ERROR::PROGRAM_LINKING_ERROR of type: {0}\n{1}",type,infoLog);
#else
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog;
#endif
            std::cerr << "\n -- --------------------------------------------------- --\n";
		}
	}
}

bf::Shader::~Shader()
{
	if(ID!=UINT_MAX)
		glDeleteProgram(ID);
}

bf::Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &tessControlPath,
                   const std::string &tessEvalPath, const std::string &geometryPath) {
    bool isGeometryShaderUsed = !geometryPath.empty();
    unsigned int vertex = compileShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = compileShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    unsigned int tessCtrl = compileShaderFromFile(tessControlPath, GL_TESS_CONTROL_SHADER);
    unsigned int tessEval = compileShaderFromFile(tessEvalPath, GL_TESS_EVALUATION_SHADER);
    unsigned int geometry;
    if(isGeometryShaderUsed) {
        geometry = compileShaderFromFile(geometryPath, GL_GEOMETRY_SHADER);
    }
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glAttachShader(ID, tessCtrl);
    glAttachShader(ID, tessEval);
    if(!geometryPath.empty())
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(tessCtrl);
    glDeleteShader(tessEval);
    if(isGeometryShaderUsed)
        glDeleteShader(geometry);
    std::cout << std::format("Tessellation Shader {} {} loaded successfully\n",vertexPath,fragmentPath);

}

void bf::Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void bf::Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void bf::Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void bf::Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void bf::Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void bf::Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void bf::Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void bf::Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void bf::Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

unsigned int bf::Shader::getID() const {
	return ID;
}

bf::Shader& bf::Shader::operator=(bf::Shader &&shader) noexcept {
	this->ID = shader.ID;
	shader.ID = UINT_MAX;
	return *this;
}

bf::Shader::Shader(bf::Shader &&shader) noexcept {
	ID = shader.ID;
	shader.ID = UINT_MAX;
}
