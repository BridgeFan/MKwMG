#pragma once
//
// Created by kamil-hp on 20.03.2022.
//

#ifndef MG1_ZAD2_TRANSFORM_H
#define MG1_ZAD2_TRANSFORM_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace bf {
    struct Transform {
        glm::vec3 position, rotation, scale;
        explicit Transform(const glm::vec3 &pos = glm::vec3(0.0f), const glm::vec3 &rot = glm::vec3(0.0f),
                           const glm::vec3 &sc = glm::vec3(1.0f));
        static const Transform Default;
        [[nodiscard]] glm::vec3 CalculateRelativePosition(const glm::vec3 &pos) const;
        [[nodiscard]] glm::vec3 CalculateRelativeFront(const glm::vec3 &pos) const;
        [[nodiscard]] glm::mat4 CalculateMatrix(const Transform &relativeTo = Default) const;
        [[nodiscard]] glm::mat4 CalculateInverseMatrix(const Transform &relativeTo = Default) const;
        friend bool operator==(const Transform &t1, const Transform &t2);
    };
	bool operator==(const Transform &t1, const Transform &t2);
    glm::vec3 rotate(const glm::vec3 &pos, const glm::vec3 &rot);
    glm::vec3 combineRotations(const glm::vec3 &r1, const glm::vec3 &r2);
    glm::vec3 combineRotations(const glm::mat4 &m1, const glm::vec3 &r2);
    glm::vec3 combineRotations(const glm::vec3 &r1, const glm::mat4 &m2);
    glm::vec3 combineRotations(const glm::mat4 &m1, const glm::mat4 &m2);
	glm::mat4 rotationAxisMatrix(const glm::vec3 &axis, float rotation);
    Transform rotateAboutPoint(const Transform &transform, const glm::vec3 &centre, const glm::vec3 &rot);
    glm::mat4 getTranslateMatrix(const glm::vec3 &pos);
    glm::mat4 getScalingMatrix(const glm::vec3 &scale);
    glm::mat4 getRotateXMatrix(float degrees);
    glm::mat4 getRotateYMatrix(float degrees);
    glm::mat4 getRotateZMatrix(float degrees);
    glm::mat4 getRotateMatrix(const glm::vec3 &rot);
    glm::mat4 getRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c);
    glm::mat4 getInverseRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c);
    glm::mat4 getProjectionMatrix(float fov, float aspect, float near = .1f, float far = 100.f);
    glm::mat4 getInverseProjectionMatrix(float fov, float aspect, float near = .1f, float far = 100.f);
    glm::mat4 getLeftProjectionMatrix(float fov, float aspect, float near, float far, float convergence, float IOD);
    glm::mat4 getRightProjectionMatrix(float fov, float aspect, float near, float far, float convergence, float IOD);
    Transform decomposeModelMatrix(const glm::mat4 &matrix);
    glm::mat4 getInverseRotateMatrix(const glm::vec3 &rot);
}



#endif //MG1_ZAD2_TRANSFORM_H
