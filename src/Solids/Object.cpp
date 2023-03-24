//
// Created by kamil-hp on 20.03.2022.
//

#include <glm/gtx/euler_angles.hpp>
#include "Object.h"
#include "../Settings.h"
#include "../ImGuiUtil.h"

int Object::index = 1;

void Object::setNewTransform(const glm::vec3& centre, const Transform& oldTransform, const Transform& newTransform) {
    if(oldTransform==newTransform)
        return;
    //position
    auto v = glm::vec4(transform.position-(centre+oldTransform.position),1.f);
    glm::mat4 invMatrix = getScalingMatrix(1.f/oldTransform.scale)*getInverseRotateMatrix(oldTransform.rotation);
    glm::mat4 matrix = getRotateMatrix(newTransform.rotation)*getScalingMatrix(newTransform.scale);
    transform.position = glm::vec3(matrix * invMatrix * v) + centre + newTransform.position;
    //scale (assumed that scaling is INDEPENDENT of rotation)
    transform.scale *= (newTransform.scale / oldTransform.scale);
    //rotation
    auto rotMat = getInverseRotateMatrix(oldTransform.rotation)*getRotateMatrix(transform.rotation)*getRotateMatrix(newTransform.rotation);
    glm::extractEulerAngleXYZ(rotMat, transform.rotation.x, transform.rotation.y, transform.rotation.z);
    transform.rotation = glm::degrees(transform.rotation);
}

void Object::ObjectGui() {
	checkChanged("Object name", name);
	checkChanged("Object position", transform.position);
	checkChanged("Object rotation", transform.rotation);
	checkChanged("Object scale", transform.scale, true);
}

glm::vec3 getMiddle(const std::vector<Object>& objects) {
	glm::vec3 sumPos;
	for(const auto& o: objects) {
		sumPos+=o.getPosition();
	}
	return sumPos/=objects.size();
}

void Object::setRelativeScale(const glm::vec3 &pos, float multiplier) {
	transform.position += (pos-transform.position) * (multiplier-1);
}
