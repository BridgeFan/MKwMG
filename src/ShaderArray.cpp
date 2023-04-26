//
// Created by kamil-hp on 21.04.23.
//

#include <GL/glew.h>
#include <iostream>
#include <format>
#include "ShaderArray.h"

bool bf::ShaderArray::changeShader(int n) {
    if(n<0 || n>=static_cast<int>(shaders.size())) {
		return false;
	}
	/*if(n<static_cast<int>(shaders.size())-1) {
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs + n);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}*/
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

void bf::ShaderArray::initGL(int width, int height) {
	//TODO - window size change support
	int FBOnum = static_cast<int>(shaders.size())-1;
	glGenRenderbuffers(FBOnum, &RBOs);
	glGenTextures(FBOnum, &colorBuffers);
	glGenFramebuffers(FBOnum, &FBOs);
	for(int i=0;i<FBOnum;i++) {
		//set texture
		glBindTexture(GL_TEXTURE_2D, colorBuffers+i);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//set depth buffer
		glBindRenderbuffer(GL_RENDERBUFFER, RBOs+i);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		//set buffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs+i);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers+i, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBOs+i);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
