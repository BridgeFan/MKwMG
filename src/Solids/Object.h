//
// Created by kamil-hp on 20.03.2022.
//

#ifndef MG1_ZAD2_OBJECT_H
#define MG1_ZAD2_OBJECT_H

#include <string>
#include <vector>
#include "Transform.h"

class Shader;
struct Settings;

class Object {
private:
	static int index;
    Transform transform;
public:
	std::string name;
	Object(const Transform& transform, const std::string& name): transform(transform), name(name) {}
	explicit Object(const Transform& transform=Transform::Default): Object(transform, "Object "+std::to_string(index)) {index++;}
	explicit Object(const std::string& name): Object(Transform::Default, name) {}
	virtual ~Object()=default;
	virtual void draw(const Shader& shader) const = 0;
	[[nodiscard]] const glm::vec3& getPosition() const {return transform.position;}
	void setPosition(const glm::vec3& pos) {transform.position=pos;}
	[[nodiscard]] [[maybe_unused]] const glm::vec3& getRotation() const {return transform.rotation;}
	void setRotation(const glm::vec3& rot) {transform.rotation=rot;}
	[[nodiscard]] const glm::vec3& getScale() const {return transform.scale;}
	void setScale(const glm::vec3& scale) {transform.scale=scale;}
	[[nodiscard]] glm::mat4 getModelMatrix(const Transform &relativeTo=Transform::Default) const {return transform.CalculateMatrix(relativeTo);}
	void setTransform(const Transform& t) {transform=t;}
    void setTransform(Transform&& t) {transform=t;}
	[[nodiscard]] const Transform& getTransform() const {return transform;}
	void setNewTransform(const glm::vec3& centre, const Transform& oldTransform, const Transform& newTransform);
	void setRelativeScale(const glm::vec3& pos, float multiplier);
	virtual void ObjectGui();
	friend glm::vec3 getMiddle(const std::vector<Object>& objects);
};


#endif //MG1_ZAD2_OBJECT_H
