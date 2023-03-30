//
// Created by kamil-hp on 04.02.2021.
//

#include "Shader.h"
#include "Util.h"
#include <GL/glew.h>
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
    std::string code = readWholeFile(path.c_str());
    if(code.empty())
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: %s\n", path.c_str());
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

}
