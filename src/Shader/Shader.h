#pragma once
//
// Created by kamil-hp on 04.02.2021.
//

#ifndef GK_4_SHADER_H
#define GK_4_SHADER_H
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
//file based on https://learnopengl.com tutorials for OpenGL
namespace bf {
    struct Shader {
        // the program ID
        unsigned int ID;
		void operator=(const Shader&)=delete;
		Shader(const Shader&)=delete;
		bf::Shader& operator=(Shader&& shader) noexcept;
		Shader(Shader&& shader) noexcept;
        // constructor reads and builds the shader
        Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = {});
        Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &tessControlPath, const std::string &tessEvalPath, const std::string& geometryPath = {});
        ~Shader();
		[[nodiscard]] unsigned int getID() const;
        // use/activate the shader
        void use() const;
        // utility uniform functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        // ------------------------------------------------------------------------
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        // ------------------------------------------------------------------------
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w) const;
        // ------------------------------------------------------------------------
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        // ------------------------------------------------------------------------
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        // ------------------------------------------------------------------------
        void setMat4(const std::string &name, const glm::mat4 &mat) const;

    private:
        void checkCompileErrors(unsigned int shader, const std::string &type) const;
        unsigned compileShaderFromFile(const std::string& path, int shaderType) const;
    };
}

#endif //GK_4_SHADER_H
