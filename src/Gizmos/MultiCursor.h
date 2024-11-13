#pragma once
//
// Created by kamil-hp on 23.03.23.
//

#ifndef MG1_ZAD2_MULTICURSOR_H
#define MG1_ZAD2_MULTICURSOR_H

#include "Solids/Solid.h"

namespace bf {
	struct ShaderArray;
	struct ConfigState;
    class MultiCursor {
        bf::DummySolid lines[3];
        void initLines();
    public:
        Transform transform;
        explicit MultiCursor(const bf::Transform &t = bf::Transform::Default);
        void draw(const bf::ShaderArray &shader, const bf::ConfigState &configState, const glm::vec3& cameraPosition);
        void ObjectGui();
    };
}

#endif //MG1_ZAD2_MULTICURSOR_H
