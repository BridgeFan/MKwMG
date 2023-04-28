//
// Created by kamil-hp on 28.03.23.
//

#ifndef MG1_ZAD2_SCENE_H
#define MG1_ZAD2_SCENE_H

#include "Solids/Cursor.h"
#include "Solids/MultiCursor.h"
#include "camera.h"
#include "Object/ObjectArray.h"

namespace bf {
	class ShaderArray;
	class ConfigState;

	class Scene {
	private:
		const static glm::vec4 clearColor;
		glm::mat4 projection, inverseProjection, view, inverseView;
	public:
		bf::ObjectArray objectArray;
		bf::Cursor cursor;
		bf::MultiCursor multiCursor;
		bf::Camera camera;

		const glm::mat4 &getProjection() const;
		const glm::mat4 &getInverseProjection() const;
		const glm::mat4 &getView() const;
		const glm::mat4 &getInverseView() const;
		Scene(float aspect, glm::vec3&& cameraPos, glm::vec3&& cameraRot, float cameraZoom, float cameraNear=.1f, float cameraFar=100.f);
		void draw(bf::ShaderArray& shaderArray, const ConfigState& configState);
	};
}


#endif //MG1_ZAD2_SCENE_H
