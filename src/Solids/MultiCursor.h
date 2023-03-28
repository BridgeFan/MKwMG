//
// Created by kamil-hp on 23.03.23.
//

#ifndef MG1_ZAD2_MULTICURSOR_H
#define MG1_ZAD2_MULTICURSOR_H

#include "Solid.h"
#include "src/Object/Transform.h"

namespace bf {
	class Shader;
	struct Settings;
    class MultiCursor {
        bf::Solid lines[3];
        void initLines();
    public:
        Transform transform;
        explicit MultiCursor(const bf::Transform &t = bf::Transform::Default);
        void draw(const bf::Shader &shader, const bf::Settings &settings);
        void ObjectGui();
    };
}

#endif //MG1_ZAD2_MULTICURSOR_H
