//
// Created by kamil-hp on 15.03.2022.
//

#include "camera.h"
#include "ImGuiUtil.h"

void bf::Camera::ObjectGui() {
	bf::imgui::checkChanged("Position", position);
	bf::imgui::checkChanged("Rotation", rotation);
	bf::imgui::checkChanged("fov", Zoom, 5.f, 120.f);
}

const glm::vec3 &bf::Camera::getFront() const {
    return front;
}

const glm::vec3 &bf::Camera::getUp() const {
    return up;
}

const glm::vec3 &bf::Camera::getRight() const {
    return right;
}

glm::mat4 bf::Camera::GetViewMatrix() {
    right = rotate({1.0f, 0.0f, 0.0f}, rotation);
    up = rotate({0.0f, 1.0f, 0.0f}, rotation);
    front = rotate({0.0f, 0.0f, 1.0f}, rotation);
    glm::mat3 matrix = glm::transpose(glm::mat3(right, up, -front)); //rotation part
    auto ret = glm::mat4{{matrix[0], 0},
                         {matrix[1], 0},
                         {matrix[2], 0},
                         {-matrix * position, 1},
    };
    return ret;
}

glm::mat4 bf::Camera::GetInverseViewMatrix() {
    glm::mat4 ret =
                    glm::mat4({1.f,0.f,0.f,0.f},
                              {0.f,1.f,0.f,0.f},
                              {0.f,0.f,-1.f,0.f},
                              {-position.x,-position.y,position.z,1.f})*bf::getInverseRotateMatrix(rotation);
    return ret;
}
