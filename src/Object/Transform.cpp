//
// Created by kamil-hp on 20.03.2022.
//

#include "Transform.h"
#include "Util.h"
#include <vector>
#include <glm/gtx/euler_angles.hpp>

const bf::Transform bf::Transform::Default=bf::Transform();

std::vector<float> debugMat(const glm::mat4& m) {
    std::vector<float> array;
    for(int i=0;i<4;i++) {
        for (int j = 0; j < 4; j++)
            array.push_back(m[i][j]);
        array.push_back(std::nanf(""));
    }
    return array;
}

glm::mat4 bf::getTranslateMatrix(const glm::vec3& pos) {
    glm::mat4 ret = {{1,0,0,0},
                     {0,1,0,0},
                     {0,0,1,0},
                     {pos.x,pos.y,pos.z,1}};
    return ret;
}
glm::mat4 bf::getScalingMatrix(const glm::vec3& scale) {
    glm::mat4 ret = {{scale.x,0,0,0},
                     {0,scale.y,0,0},
                     {0,0,scale.z,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateXMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{1,0,0,0},
                     {0,c,s,0},
                     {0,-s,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateYMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{c,0,-s,0},
                     {0,1,0,0},
                     {s,0,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateZMatrix(float degrees) {
    float c = std::cos(glm::radians(degrees));
    float s = std::sin(glm::radians(degrees));
    glm::mat4 ret = {{c,s,0,0},
                     {-s,c,0,0},
                     {0,0,1,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateMatrix(const glm::vec3& rot) {
    glm::mat4 ret = bf::getRotateXMatrix(rot.x);
    ret *= bf::getRotateYMatrix(rot.y);
    ret *= bf::getRotateZMatrix(rot.z);
    return ret;
}

glm::mat4 bf::getInverseRotateMatrix(const glm::vec3& rot) {
    glm::mat4 ret = bf::getRotateZMatrix(-rot.z);
    ret *= bf::getRotateYMatrix(-rot.y);
    ret *= bf::getRotateXMatrix(-rot.x);
    return ret;
}

bf::Transform bf::decomposeModelMatrix(const glm::mat4& matrix) {
    bf::Transform t;
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


bf::Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc): position(pos), rotation(rot), scale(sc) {}

glm::mat4 bf::Transform::CalculateMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo.rotation==Default.rotation && relativeTo.position==Default.position && relativeTo.scale == Default.scale)
        ret = glm::mat4(1.f);
    else
        ret = CalculateMatrix(relativeTo);
    ret = ret * bf::getTranslateMatrix(position) * bf::getRotateMatrix(rotation) * bf::getScalingMatrix(scale);
    return ret;
}
glm::mat4 bf::Transform::CalculateInverseMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo.rotation==Default.rotation && relativeTo.position==Default.position && relativeTo.scale == Default.scale)
        ret = glm::mat4(1.f);
    else
        ret = CalculateInverseMatrix(relativeTo);
    ret = bf::getScalingMatrix(1.f/scale) * bf::getInverseRotateMatrix(rotation) * bf::getTranslateMatrix(-position) * ret;
    return ret;
}

glm::vec3 bf::Transform::CalculateRelativePosition(const glm::vec3 &pos) const
{
	float C=std::cos(glm::radians(rotation.z));
	float S=std::sin(glm::radians(rotation.z));
	glm::vec3 ret;
	ret.x=position.x+(pos.x*C+pos.z*S);
	ret.y=position.y+pos.y;
	ret.z=position.z+(-pos.x*S+pos.z*C);
	return ret;
}
glm::vec3 bf::Transform::CalculateRelativeFront(const glm::vec3 &pos) const
{
	float C=std::cos(glm::radians(rotation.z));
	float S=std::sin(glm::radians(rotation.z));
	glm::vec3 ret;
	ret.x=pos.x*C+pos.z*S;
	ret.y=0.0f;
	ret.z=-pos.x*S+pos.z*C;
	return ret;
}

bool bf::operator==(const bf::Transform &t1, const bf::Transform &t2) {
    return t1.position==t2.position && t1.rotation==t2.rotation && t1.scale==t2.scale;
}

glm::vec3 bf::rotate(const glm::vec3 &pos, const glm::vec3 &rot) {
    auto rotMatrix = bf::getRotateMatrix(rot);
    auto vec = rotMatrix * glm::vec4(pos.x,pos.y,pos.z,1);
    return {vec.x,vec.y,vec.z};
	//return glm::rotateZ(glm::rotateY(glm::rotateX(pos, glm::radians(rot.x)), glm::radians(rot.y)), glm::radians(rot.z));
}

glm::vec3 bf::combineRotations(const glm::vec3& r1, const glm::vec3& r2) {
    auto mat = bf::getRotateMatrix(r2)*bf::getRotateMatrix(r1);
    glm::vec3 ret;
    glm::extractEulerAngleXYZ(mat,ret.x,ret.y,ret.z);
    return ret;
}

bf::Transform bf::rotateAboutPoint(const bf::Transform& transform, const glm::vec3& centre, const glm::vec3& rot) {
    bf::Transform ret=transform;
    ret.position -= centre;
    ret.position = rotate(ret.position,rot);
    ret.rotation = glm::degrees(bf::combineRotations(transform.rotation,rot));
    ret.position += centre;
    return ret;
}

glm::mat4 bf::getProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(glm::radians(fov*.5f));
    glm::mat4 ret = {{1.0f/(t*aspect),0,0,0},
                     {0,1/t,0,0},
                     {0,0,-(far+near)/(far-near),-1},
                     {0,0,-2*far*near/(far-near),0}};
    return ret;
}
glm::mat4 bf::getInverseProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(glm::radians(fov*.5f));
    glm::mat4 ret = {{t*aspect,0,0,0},
                     {0,t,0,0},
                     {0,0,0,(near-far)/(2*far*near)},
                     {0,0,-1,(far+near)/(2*far*near)}};
    return ret;
}

glm::mat4 bf::getRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c) {
    return bf::getTranslateMatrix(-c)*bf::getRotateMatrix(rot)*bf::getTranslateMatrix(c);
}
glm::mat4 bf::getInverseRelativeRotateMatrix(const glm::vec3 &rot, const glm::vec3 &c) {
    return bf::getTranslateMatrix(-c)*bf::getInverseRotateMatrix(rot)*bf::getTranslateMatrix(c);
}
constexpr float sqrt2 = 1.35f;

glm::mat4 bf::getLeftProjectionMatrix(float fov, float aspect, float near, float far, float convergence, float IOD) {
    float top, bottom, left, right;

    top     = near * tan(fov*.5f);
    bottom  = -top;

    float a = aspect * tan(fov*.5f) * convergence;

    float b = a - IOD*.5f;
    float c = a + IOD*.5f;

    left    = -b * near/convergence;
    right   =  c * near/convergence;
    glm::mat4 ret = {{2.f*sqrt2*near/(right-left),0,0,0},
                     {0,2.f*sqrt2*near/(top-bottom),0,0},
                     {sqrt2*(right+left)/(right-left),sqrt2*(top+bottom)/(top-bottom),-(far+near)/(far-near),-1.f},
                     {0,0,(-2.f*far*near)/(far-near),0}};
    return ret;
}

glm::mat4 bf::getRightProjectionMatrix(float fov, float aspect, float near, float far, float convergence, float IOD) {
    float top, bottom, left, right;

    top     = near * tan(fov*.5f);
    bottom  = -top;

    float a = aspect * tan(fov*.5f) * convergence;

    float b = a - IOD*.5f;
    float c = a + IOD*.5f;

    left    = -c * near/convergence;
    right   =  b * near/convergence;

    glm::mat4 ret = {{2.f*sqrt2*near/(right-left),0,0,0},
                     {0,2.f*sqrt2*near/(top-bottom),0,0},
                     {sqrt2*(right+left)/(right-left),sqrt2*(top+bottom)/(top-bottom),-(far+near)/(far-near),-1.f},
                     {0,0,(-2.f*far*near)/(far-near),0}};
    return ret;
}
