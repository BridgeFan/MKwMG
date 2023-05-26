#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "src/Object/Transform.h"
//source based on learnopengl.com

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
namespace bf {
    struct ConfigState;
    class Camera : public Transform {
    protected:
        glm::vec3 front={.0f,.0f,1.f};
		glm::vec3 up={.0f,.1f,0.f};
		glm::vec3 right={1.f,.0f,.0f};
    public:
        [[nodiscard]] const glm::vec3 &getFront() const;
        [[nodiscard]] const glm::vec3 &getUp() const;
        [[nodiscard]] const glm::vec3 &getRight() const;
    public:
        // constructor with vectors
        Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f));
        //functions
        glm::mat4 GetViewMatrix();
        glm::mat4 GetInverseViewMatrix(const glm::mat4& view);
        void ObjectGui(bf::ConfigState& configState);
    };
}
#endif

