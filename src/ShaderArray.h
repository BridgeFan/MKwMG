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
    enum ShaderType: int {
        BasicShader=0,
        BezierShader=1,
        MultipleShaders
    };
    template<typename T>
    concept ShaderArrayVariantable = std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4> ||
            std::is_same_v<T, glm::mat2> || std::is_same_v<T, glm::mat3> || std::is_same_v<T, glm::mat4>;
    typedef std::variant<glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4, int, bool, float> ShaderArrayVariant;
    class ShaderArray {
        std::map<std::string,bf::ShaderArrayVariant> commonUniformMap;
        void setUniform(const std::string& name, const ShaderArrayVariant& var) const;
        int activeIndex=0;
        std::vector<bf::Shader> shaders;
		unsigned FBOs, colorBuffers, RBOs;
    public:
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
		void initGL(int width, int height);
        void addBasicShader(const std::string& path, bool isGeometric);
    };
}


#endif //MG1_ZAD2_SHADERARRAY_H
