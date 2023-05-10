//
// Created by kamil-hp on 21.04.23.
//

#include <GL/glew.h>
#include <algorithm>
#include "ShaderArray.h"
#include "Util.h"

bool bf::ShaderArray::changeShader(int n) {
    if(n<0 || n>=static_cast<int>(shaders.size())) {
		return false;
	}
    auto type = static_cast<bf::ShaderType>(activeIndex);
    //special settings - old
    switch(type) {
        case BasicShader:
        case BezierShader:
        case PointShader:   break;
        case BezierSurfaceShader:
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case MultipleShaders:   break;
    }
	activeIndex=n;
    type = static_cast<bf::ShaderType>(activeIndex);
    //special settings - new
    switch(type) {
        case BasicShader:   break;
        case BezierShader:
            glPatchParameteri( GL_PATCH_VERTICES, 4);
            break;
        case PointShader:   break;
        case BezierSurfaceShader:
            glPatchParameteri( GL_PATCH_VERTICES, 16);
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            break;
        case MultipleShaders:   break;
    }
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

bf::ShaderArray::ShaderArray() {
    const std::string SHADER_PATH = "../shaders/";
    //add shaders here
    addBasicShader(SHADER_PATH+"shader", false);
    addTessellationShader(SHADER_PATH+"bezierShader", false);
    addBasicShader(SHADER_PATH+"pointShader", false);
    addTessellationShader(SHADER_PATH+"surfaceShader", false);
}

glm::vec3 getGray(const glm::vec3& vec) {
    float f = .2126f*vec.r+.7152f*vec.g+.0722f*vec.b;
    return {f,f,f};
}

void bf::ShaderArray::setColor(const glm::vec3 &vec) const {
    glm::vec3 v = lerp(vec,getGray(vec),grayPercentage);
	switch(stereoscopicState) {
		case bf::StereoscopicState::None:
			shaders[activeIndex].setVec3("color", v);
			break;
		case bf::StereoscopicState::LeftEye:
			shaders[activeIndex].setVec3("color", {v.r,.0f,.0f});
			break;
		case bf::StereoscopicState::RightEye:
			shaders[activeIndex].setVec3("color", {.0f,v.g,v.b});
			break;
	}
}

void bf::ShaderArray::setStereoscopicState(bf::StereoscopicState state) {
	stereoscopicState = state;
}

void bf::ShaderArray::setColor(uint8_t r, uint8_t g, uint8_t b) const {
    setColor({static_cast<float>(r)/255.f,static_cast<float>(g)/255.f,static_cast<float>(b)/255.f});
}

void bf::ShaderArray::setGrayPercentage(float g) {
    grayPercentage=std::clamp(g,.0f,1.f);
}

