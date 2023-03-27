//
// Created by kamil-hp on 04.02.2021.
//

#include "Shader.h"
#include "Util.h"
#include <GL/glew.h>
//file based on https://learnopengl.com tutorials for OpenGL

bf::Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
    bool isGeometryShaderUsed = !geometryPath.empty();
    // 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode = readWholeFile(vertexPath.c_str());
	std::string fragmentCode = readWholeFile(fragmentPath.c_str());
	std::string geometryCode = readWholeFile(geometryPath.c_str());
    if(vertexCode.empty())
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", vertexPath.c_str());
    if(fragmentCode.empty())
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", fragmentPath.c_str());
    if(!geometryPath.empty() && geometryCode.empty()) {
        fprintf(stderr, "ERROR::bf::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", geometryPath.c_str());
    }
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if(isGeometryShaderUsed)
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
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
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s",type.c_str(),infoLog);
            fprintf(stderr, "\n -- --------------------------------------------------- --\n");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s",type.c_str(),infoLog);
            fprintf(stderr, "\n -- --------------------------------------------------- --\n");
		}
	}
}

bf::Shader::~Shader()
{
	glDeleteProgram(ID);
}