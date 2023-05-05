//
// Created by kamil-hp on 28.03.23.
//

#ifndef MG1_ZAD2_SCENE_H
#define MG1_ZAD2_SCENE_H

#include "src/Object/Cursor.h"
#include "src/Object/MultiCursor.h"
#include "Camera.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"

namespace bf {
	class ConfigState;
	class Scene {
	private:
		glm::mat4 projection, inverseProjection, view, inverseView;
		bf::ShaderArray shaderArray;
	public:
		bf::ObjectArray objectArray;
		bf::Cursor cursor;
		bf::MultiCursor multiCursor;
		bf::Camera camera;

		const glm::mat4 &getProjection() const;
		const glm::mat4 &getInverseProjection() const;
		const glm::mat4 &getView() const;
		const glm::mat4 &getInverseView() const;
		Scene(const ConfigState& configState);
		void draw(const ConfigState& configState);
        bool onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState& configState);
        bool onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState& configState);
        bool onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods, const bf::ConfigState& configState);
        bool onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods, const bf::ConfigState& configState);
		void onMouseMove(const glm::vec2& oldMousePos, const bf::ConfigState& configState);
	};
}


#endif //MG1_ZAD2_SCENE_H
