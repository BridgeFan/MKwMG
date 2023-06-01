//
// Created by kamil-hp on 20.03.2022.
//

#include "Transform.h"
#include "Util.h"
#include <vector>

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
    float c = std::cos(bf::radians(degrees));
    float s = std::sin(bf::radians(degrees));
    glm::mat4 ret = {{1,0,0,0},
                     {0,c,s,0},
                     {0,-s,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateYMatrix(float degrees) {
    float c = std::cos(bf::radians(degrees));
    float s = std::sin(bf::radians(degrees));
    glm::mat4 ret = {{c,0,-s,0},
                     {0,1,0,0},
                     {s,0,c,0},
                     {0,0,0,1}};
    return ret;
}
glm::mat4 bf::getRotateZMatrix(float degrees) {
    float c = std::cos(bf::radians(degrees));
    float s = std::sin(bf::radians(degrees));
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
    t.scale.x = bf::length(myMat3[0]);
    t.scale.y = bf::length(myMat3[1]);
    t.scale.z = bf::length(myMat3[2]);
    myMat3[0]/=t.scale.x;
    myMat3[1]/=t.scale.y;
    myMat3[2]/=t.scale.z;
    //rotation
	t.rotation = bf::matrixToEulerXYZ(glm::mat4(myMat3));
    t.rotation = bf::degrees(t.rotation);
    return t;
}

glm::vec3 bf::decomposeRotationMatrix(const glm::mat4& matrix) {
	auto myMat3 = glm::mat3(matrix);
	glm::vec3 v;
	v = bf::matrixToEulerXYZ(glm::mat4(myMat3));
	v = bf::degrees(v);
	return v;
}


bf::Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sc): position(pos), rotation(rot), scale(sc) {}


glm::mat4 bf::Transform::CalculateMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo==Default)
        ret = glm::mat4(1.f);
    else
        ret = CalculateMatrix(relativeTo);
    ret = ret * bf::getTranslateMatrix(position) * bf::getRotateMatrix(rotation) * bf::getScalingMatrix(scale);
    return ret;
}
glm::mat4 bf::Transform::CalculateInverseMatrix(const Transform &relativeTo) const {
    glm::mat4 ret;
    if(relativeTo==Default)
        ret = glm::mat4(1.f);
    else
        ret = CalculateInverseMatrix(relativeTo);
    ret = bf::getScalingMatrix(1.f/scale) * bf::getInverseRotateMatrix(rotation) * bf::getTranslateMatrix(-position) * ret;
    return ret;
}

glm::vec3 bf::Transform::CalculateRelativePosition(const glm::vec3 &pos) const
{
	float C=std::cos(bf::radians(rotation.z));
	float S=std::sin(bf::radians(rotation.z));
	glm::vec3 ret;
	ret.x=position.x+(pos.x*C+pos.z*S);
	ret.y=position.y+pos.y;
	ret.z=position.z+(-pos.x*S+pos.z*C);
	return ret;
}
glm::vec3 bf::Transform::CalculateRelativeFront(const glm::vec3 &pos) const
{
	float C=std::cos(bf::radians(rotation.z));
	float S=std::sin(bf::radians(rotation.z));
	glm::vec3 ret;
	ret.x=pos.x*C+pos.z*S;
	ret.y=0.0f;
	ret.z=-pos.x*S+pos.z*C;
	return ret;
}

bool bf::operator==(const bf::Transform &t1, const bf::Transform &t2) {
    return almostEqual(t1.position,t2.position,1e-6f) &&
        almostEqual(t1.rotation,t2.rotation,1e-6f) &&
        almostEqual(t1.scale,t2.scale,1e-6f);
}

glm::vec3 bf::rotate(const glm::vec3 &pos, const glm::vec3 &rot) {
    auto rotMatrix = bf::getRotateMatrix(rot);
    auto vec = rotMatrix * glm::vec4(pos.x,pos.y,pos.z,1);
    return {vec.x,vec.y,vec.z};
}

glm::vec3 bf::combineRotations(const glm::vec3& r1, const glm::vec3& r2) {return bf::combineRotations(bf::getRotateMatrix(r1),bf::getRotateMatrix(r2));}
glm::vec3 bf::combineRotations(const glm::mat4& m1, const glm::vec3& r2) {return bf::combineRotations(m1,bf::getRotateMatrix(r2));}
glm::vec3 bf::combineRotations(const glm::vec3& r1, const glm::mat4& m2) {return bf::combineRotations(bf::getRotateMatrix(r1),m2);}

glm::vec3 bf::combineRotations(const glm::mat4& m1, const glm::mat4& m2) {
	return bf::degrees(bf::matrixToEulerXYZ(m2*m1));
}

glm::mat4 bf::rotationAxisMatrix(const glm::vec3 &axis, float rotation) {
	float c = std::cos(bf::radians(rotation));
	float mc = 1.f-c;
	float s = std::sin(bf::radians(rotation));
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	glm::mat3 ret(
		{c+x*x*mc, y*x*mc+z*s, z*x*mc-y*s},
		{x*y*mc-z*s, c+y*y*mc, x*y*mc+x*s},
		{x*z*mc+y*s, y*z*mc-x*s, c+z*z*mc}
	);
	glm::mat4 m(ret);
    m[3][3]=1.f;
	return m;
}

bf::Transform bf::rotateAboutPoint(const bf::Transform& transform, const glm::vec3& centre, const glm::vec3& rot) {
    bf::Transform ret=transform;
    ret.position -= centre;
    ret.position = rotate(ret.position,rot);
    ret.rotation = bf::combineRotations(transform.rotation,rot);
    ret.position += centre;
    return ret;
}

glm::mat4 bf::getProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(bf::radians(fov*.5f));
    glm::mat4 ret = {{1.0f/(t*aspect),0,0,0},
                     {0,1/t,0,0},
                     {0,0,-(far+near)/(far-near),-1},
                     {0,0,-2*far*near/(far-near),0}};
    return ret;
}
glm::mat4 bf::getInverseProjectionMatrix(float fov, float aspect, float near, float far) {
    float t = std::tan(bf::radians(fov*.5f));
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
