#pragma once
//
// Created by kamil-hp on 21.04.23.
//

#ifndef MG1_ZAD2_SHADERARRAY_H
#define MG1_ZAD2_SHADERARRAY_H

#include <vector>
#include <variant>
#include <map>
#include "Shader.h"

namespace bf {
	enum class StereoscopicState: int {
		None=0,
		LeftEye,
		RightEye
	};
    enum ShaderType: int {
        BasicShader=0,
        BezierShader=1,
        PointShader=2,
		BezierSurfaceShader0=3,
        BezierSurfaceShader2=4,
		GregoryShader=5,
        CursorShader=6,
        LinkShader=7,
        MultipleShaders
    };
    template<typename T>
    concept ShaderArrayVariantable = std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4> ||
            std::is_same_v<T, glm::mat2> || std::is_same_v<T, glm::mat3> || std::is_same_v<T, glm::mat4>;
    using ShaderArrayVariant = std::variant<glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4, int, bool, float>;
    struct ShaderArray {
        std::map<std::string,bf::ShaderArrayVariant> commonUniformMap;
        void setUniform(const std::string& name, const ShaderArrayVariant& var) const;
        int activeIndex=0;
        std::vector<bf::Shader> shaders;
		unsigned FBOs, colorBuffers, RBOs;
		StereoscopicState stereoscopicState=StereoscopicState::None;
        void addBasicShader(const std::string& vertFile, const std::string& fragFile={});
        void addTessellationShader(const std::string& path, bool isGeometric, bool isCommonUsed=true);
        float grayPercentage=.0f;
    public:
        void setGrayPercentage(float g);
        ShaderArray();
        [[nodiscard]] int getActiveIndex() const;
        const bf::Shader& getActiveShader() const;
        int getSize() const;
        template<ShaderArrayVariantable T>
        void addCommonUniform(const std::string& name, T v) {
            //object will be copied to map of uniforms
            commonUniformMap[name]=v;
            setUniform(name,v);
        }
        template<std::integral T>
        void addCommonUniform(const std::string& name, T v) {
            commonUniformMap[name]=static_cast<int>(v);
            setUniform(name,v);
        }
        template<std::floating_point T>
        void addCommonUniform(const std::string& name, T v) {
            commonUniformMap[name]=static_cast<float>(v);
            setUniform(name,v);
        }
        void addCommonUniform(const std::string& name, bool v) {
            commonUniformMap[name]=v;
            setUniform(name,v);
        }
        bool removeCommonUniform(const std::string& name) {
            return commonUniformMap.erase(name)>0;
        }
        bool changeShader(int n);
		void setColor(const glm::vec3& vec) const;
        void setColor(uint8_t r, uint8_t g, uint8_t b) const;
		void setStereoscopicState(StereoscopicState state);
    };
}


#endif //MG1_ZAD2_SHADERARRAY_H
