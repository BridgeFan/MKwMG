//
// Created by kamil-hp on 21.04.23.
//

#include <iostream>
#include "ShaderArray.h"

bool bf::ShaderArray::changeShader(int n) {
    if(n<0 || n>=static_cast<int>(shaders.size())) {
		return false;
	}
	activeIndex=n;
    shaders[n].use();
	for (auto &&[name, v]: commonUniformMap) {
		setUniform(name, v);
	}
    return true;
}

void bf::ShaderArray::setUniform(const std::string& name, const ShaderArrayVariant& var) const {
    std::visit([this,&name](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, glm::vec2>)
            shaders[activeIndex].setVec2(name,arg);
        if constexpr (std::is_same_v<T, glm::vec3>)
            shaders[activeIndex].setVec3(name,arg);
        if constexpr (std::is_same_v<T, glm::vec4>)
            shaders[activeIndex].setVec4(name,arg);
        if constexpr (std::is_same_v<T, glm::mat2>)
            shaders[activeIndex].setMat2(name,arg);
        if constexpr (std::is_same_v<T, glm::mat3>)
            shaders[activeIndex].setMat3(name,arg);
        if constexpr (std::is_same_v<T, glm::mat4>)
            shaders[activeIndex].setMat4(name,arg);
        if constexpr (std::is_same_v<T, int>)
            shaders[activeIndex].setInt(name,arg);
        if constexpr (std::is_same_v<T, float>)
            shaders[activeIndex].setFloat(name,arg);
        if constexpr (std::is_same_v<T, bool>)
            shaders[activeIndex].setBool(name,arg);
    }, var);
}

int bf::ShaderArray::getActiveIndex() const {
    return activeIndex;
}

const bf::Shader &bf::ShaderArray::getActiveShader() const {return shaders[activeIndex];}

int bf::ShaderArray::getSize() const {return shaders.size();}

void bf::ShaderArray::addBasicShader(const std::string &path, bool isGeometric) {
    if(isGeometric)
        shaders.emplace_back(path + ".vert", path + ".frag",path+".geom");
    else
        shaders.emplace_back(path + ".vert", path + ".frag");
}
void bf::ShaderArray::addTessellationShader(const std::string &path, bool isGeometric) {
	if(isGeometric)
		shaders.emplace_back(path + ".vert", path + ".frag", path+".tesc", path+".tese", path+".geom");
	else
		shaders.emplace_back(path + ".vert", path + ".frag", path+".tesc", path+".tese");
}

