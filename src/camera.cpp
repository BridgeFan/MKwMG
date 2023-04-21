//
// Created by kamil-hp on 15.03.2022.
//

#include "camera.h"
#include "ImGuiUtil.h"
#include "Util.h"
#include <glm/gtc/matrix_inverse.hpp>

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
    //update vectors
    right = rotate({1.0f, 0.0f, 0.0f}, rotation);
    up = rotate({0.0f, 1.0f, 0.0f}, rotation);
    front = rotate({0.0f, 0.0f, -1.0f}, rotation);
    //return matrix
    auto m3 = glm::mat3(1.f,0.f,0.f,
                        0.f,1.f,0.f,
                        0.f,0.f,-1.f);
    glm::mat3 matrix = m3*glm::mat3(bf::getInverseRotateMatrix(rotation));
    auto ret = glm::mat4{{matrix[0], 0},
                         {matrix[1], 0},
                         {matrix[2], 0},
                         {-matrix * position, 1},
    };
    return ret;
}

glm::mat4 bf::Camera::GetInverseViewMatrix(const glm::mat4& view) {
    auto tmp = glm::inverse(view);
    //TODO - improve position
    auto m3 = glm::mat3(1.f,0.f,0.f,
                        0.f,1.f,0.f,
                        0.f,0.f,-1.f);
    const auto matrix = glm::mat3(view);
    const auto matrixT = glm::transpose(matrix);
    const glm::mat3 invMatrix = glm::mat3(bf::getRotateMatrix(rotation))*m3;
    glm::mat4 ret = glm::mat4(invMatrix);
    ret[3]=tmp[3]; //TODO
    //ret[3] = {invMatrix * tmpVec, 1.f};
    return ret;
}
