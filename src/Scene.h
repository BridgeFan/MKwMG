#pragma once
//
// Created by kamil-hp on 28.03.23.
//

#ifndef MG1_ZAD2_SCENE_H
#define MG1_ZAD2_SCENE_H

#include "src/Gizmos/Cursor.h"
#include "src/Gizmos/MultiCursor.h"
#include "Camera.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"

namespace bf {
	class ConfigState;
	class Scene {
	private:
		glm::mat4 projection, inverseProjection, view, inverseView;

        void internalDraw(const ConfigState& configState);
        unsigned FBO=UINT_MAX, texture = UINT_MAX, RBO=UINT_MAX;
        unsigned VBO=UINT_MAX, VAO=UINT_MAX, EBO=UINT_MAX;
	public:
        virtual ~Scene();
        void resizeFramebuffers(int x, int y) const;
        bf::ObjectArray objectArray;
		bf::Cursor cursor;
		bf::MultiCursor multiCursor;
		bf::Camera camera;
        bf::DummySolid boxRectangle;

		const glm::mat4 &getProjection() const;
		const glm::mat4 &getInverseProjection() const;
		const glm::mat4 &getView() const;
		const glm::mat4 &getInverseView() const;
		explicit Scene(const ConfigState& configState);
		void draw(const ConfigState& configState);
        bool onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState& configState);
        bool onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit modKeyBit, const bf::ConfigState& configState);
        bool onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods, const bf::ConfigState& configState);
        bool onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods, const bf::ConfigState& configState);
		void onMouseMove(const glm::vec2& oldMousePos, const bf::ConfigState& configState);

        bf::ShaderArray shaderArray;
    };
}


#endif //MG1_ZAD2_SCENE_H
