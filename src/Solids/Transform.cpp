//
// Created by kamil-hp on 20.03.2022.
//

#include "Transform.h"
#include <vector>
#include <glm/gtx/euler_angles.hpp>
#include <cstdio>

const Transform Transform::Default=Transform();

std::vector<float> debugMat(const glm::mat4& m) {
    std::vector<float> array;
    for(int i=0;i<4;i++) {
        for (int j = 0; j < 4; j++)
            array.push_back(m[i][j]);
        array.push_back(std::nanf(""));
    }
    return array;
}

glm::mat4 getTranslateMatrix(const glm::vec3& pos) {
    glm::mat4 ret = {{1,0,0,0},
                     {0,1,0,0},
                     {0,0,1,0},
                     {pos.x,pos.y,pos.z,1}};
    return ret;
}
glm::mat4 getScalingMatrix(const glm::vec3& scale) {
    glm::mat4 ret = {{scale.x,0,0,0},
                     {0,scale.y,0,0},
                     {0,0,scale.z,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 getRotateXMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{1,0,0,0},
                     {0,c,s,0},
                     {0,-s,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 getRotateYMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{c,0,-s,0},
                     {0,1,0,0},
                     {s,0,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 getRotateZMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{c,s,0,0},
                     {-s,c,0,0},
                     {0,0,1,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 getRotateMatrix(const glm::vec3& rot) {
    glm::mat4 ret = getRotateXMatrix(rot.x);
    ret *= getRotateYMatrix(rot.y);
    ret *= getRotateZMatrix(rot.z);
    return ret;
}

glm::mat4 getInverseRotateMatrix(const glm::vec3& rot) {
    glm::mat4 ret = getRotateZMatrix(-rot.z);
    ret *= getRotateYMatrix(-rot.y);
    ret *= getRotateXMatrix(-rot.x);
    return ret;
}

/*glm::vec3 solve3(const glm::mat4x3& matrix) {
    //column, row
    glm::mat4x3 mat = matrix;
    constexpr int n = 3;
    int results[n];
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            float f = mat[i][j] / mat[i][i];
            for (int k = 0; k < n + 1; k++) {
                mat[k][j] = mat[k][j] - f * mat[k][i];
            }
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        results[i] = mat[n][i];
        for (int j = i + 1; j < n; j++) {
            if (i != j) {
                results[i] = results[i] - mat[j][i] * results[j];
            }
        }
        results[i]=results[i]/mat[i][i];
    }
    return {results[0],results[1],results[2]};
}*/

Transform decomposeModelMatrix(const glm::mat4& matrix) {
    Transform t;
    //position
    t.position = {matrix[3][0],matrix[3][1],matrix[3][2]};
    //scale
    auto myMat3 = glm::mat3(matrix);
    t.scale.x = glm::length(myMat3[0]);
    t.scale.y = glm::length(myMat3[1]);
    t.scale.z = glm::length(myMat3[2]);
    myMat3[0]/=t.scale.x;
    myMat3[1]/=t.scale.y;
    myMat3[2]/=t.scale.z;
    //rotation
    glm::extractEulerAngleXYZ(glm::mat4(myMat3), t.rotation.x,t.rotation.y,t.rotation.z);
    t.rotation = glm::degrees(t.rotation);
    return t;
}


Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc): position(pos), rotation(rot), scale(sc) {}

glm::mat4 Transform::CalculateMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo.rotation==Default.rotation && relativeTo.position==Default.position && relativeTo.scale == Default.scale)
        ret = glm::mat4(1.f);
    else
        ret = CalculateMatrix(relativeTo);
    ret = ret * getTranslateMatrix(position) * getRotateMatrix(rotation) * getScalingMatrix(scale);
    return ret;
}
glm::mat4 Transform::CalculateInverseMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo.rotation==Default.rotation && relativeTo.position==Default.position && relativeTo.scale == Default.scale)
        ret = glm::mat4(1.f);
    else
        ret = CalculateInverseMatrix(relativeTo);
    ret = getScalingMatrix(1.f/scale) * getInverseRotateMatrix(rotation) * getTranslateMatrix(-position) * ret;
    return ret;
}

glm::vec3 Transform::CalculateRelativePosition(const glm::vec3 &pos) const
{
	float C=std::cos(glm::radians(rotation.z));
	float S=std::sin(glm::radians(rotation.z));
	glm::vec3 ret;
	ret.x=position.x+(pos.x*C+pos.z*S);
	ret.y=position.y+pos.y;
	ret.z=position.z+(-pos.x*S+pos.z*C);
	return ret;
}
glm::vec3 Transform::CalculateRelativeFront(const glm::vec3 &pos) const
{
	float C=std::cos(glm::radians(rotation.z));
	float S=std::sin(glm::radians(rotation.z));
	glm::vec3 ret;
	ret.x=pos.x*C+pos.z*S;
	ret.y=0.0f;
	ret.z=-pos.x*S+pos.z*C;
	return ret;
}

bool operator==(const Transform &t1, const Transform &t2) {
    return t1.position==t2.position && t1.rotation==t2.rotation && t1.scale==t2.scale;
}

glm::vec3 rotate(const glm::vec3 &pos, const glm::vec3 &rot) {
    auto rotMatrix = getRotateMatrix(rot);
    auto vec = rotMatrix * glm::vec4(pos.x,pos.y,pos.z,1);
    return {vec.x,vec.y,vec.z};
	//return glm::rotateZ(glm::rotateY(glm::rotateX(pos, glm::radians(rot.x)), glm::radians(rot.y)), glm::radians(rot.z));
}

glm::vec3 combineRotations(const glm::vec3& r1, const glm::vec3& r2) {
    auto mat = getRotateMatrix(r2)*getRotateMatrix(r1);
    glm::vec3 ret;
    glm::extractEulerAngleXYZ(mat,ret.x,ret.y,ret.z);
    return ret;
}

Transform rotateAboutPoint(const Transform& transform, const glm::vec3& centre, const glm::vec3& rot) {
    Transform ret=transform;
    ret.position -= centre;
    ret.position = rotate(ret.position,rot);
    ret.rotation = glm::degrees(combineRotations(transform.rotation,rot));
    ret.position += centre;
    return ret;
}

glm::mat4 getProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(glm::radians(fov*.5f));
    glm::mat4 ret = {{1.0f/(t*aspect),0,0,0},
                     {0,1/t,0,0},
                     {0,0,-(far+near)/(far-near),-1},
                     {0,0,-2*far*near/(far-near),0}};
    return ret;
}
glm::mat4 getInverseProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(glm::radians(fov*.5f));
    glm::mat4 ret = {{t*aspect,0,0,0},
                     {0,t,0,0},
                     {0,0,0,(near-far)/(2*far*near)},
                     {0,0,-1,(far+near)/(2*far*near)}};
    return ret;
}

glm::mat4 getRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c) {
    return getTranslateMatrix(-c)*getRotateMatrix(rot)*getTranslateMatrix(c);
}
glm::mat4 getInverseRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c) {
    return getTranslateMatrix(-c)*getInverseRotateMatrix(rot)*getTranslateMatrix(c);
}

