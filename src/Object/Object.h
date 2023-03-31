//
// Created by kamil-hp on 20.03.2022.
//

#ifndef MG1_ZAD2_OBJECT_H
#define MG1_ZAD2_OBJECT_H

#include <string>
#include <vector>
#include "Transform.h"

namespace bf {
	class Shader;
	struct Settings;
	class Object {
	private:
		static int _objIndex;
	protected:
		bf::Transform transform;
	public:
		std::string name;
		Object(const bf::Transform &t, const std::string &objName) : transform(t), name(objName) {}
		explicit Object(const bf::Transform &t = bf::Transform::Default) : Object(t, "Object " + std::to_string(
				_objIndex)) { _objIndex++; }
		explicit Object(const std::string &objName) : bf::Object(Transform::Default, objName) {}
		virtual ~Object() = default;
		virtual void draw(const bf::Shader &shader) const = 0;
        virtual bool addPoint(unsigned index) { return false; }
		[[nodiscard]] const glm::vec3 &getPosition() const { return transform.position; }
		virtual void setPosition(const glm::vec3 &pos) { transform.position = pos; }
		[[nodiscard]] [[maybe_unused]] const glm::vec3 &getRotation() const { return transform.rotation; }
		void setRotation(const glm::vec3 &rot) { transform.rotation = rot; }
		[[nodiscard]] const glm::vec3 &getScale() const { return transform.scale; }
		void setScale(const glm::vec3 &scale) { transform.scale = scale; }
		[[nodiscard]] glm::mat4 getModelMatrix(const bf::Transform &relativeTo = bf::Transform::Default) const;
		virtual void setTransform(const bf::Transform &t) { transform = t; }
		virtual void setTransform(bf::Transform &&t) { transform = t; }
		[[nodiscard]] const bf::Transform &getTransform() const { return transform; }
		void setNewTransform(const glm::vec3 &centre, const bf::Transform &oldTransform, const bf::Transform &newTransform);
		void setRelativeScale(const glm::vec3 &pos, float multiplier);
		virtual void ObjectGui();
		friend glm::vec3 getMiddle(const std::vector<Object> &objects);
        [[nodiscard]] virtual std::vector<unsigned> usedVectors() const {return {};}
		//utility functions
		[[nodiscard]] virtual bool isMovable() const {return true;}
	};

	glm::vec3 getMiddle(const std::vector<bf::Object> &objects);
}


#endif //MG1_ZAD2_OBJECT_H
