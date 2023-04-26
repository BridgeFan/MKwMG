//
// Created by kamil-hp on 23.03.23.
//

#ifndef MG1_ZAD2_MULTICURSOR_H
#define MG1_ZAD2_MULTICURSOR_H

#include "Solid.h"

namespace bf {
	class ShaderArray;
	struct Settings;
    class MultiCursor {
        bf::Solid lines[3];
        void initLines();
    public:
        Transform transform;
        explicit MultiCursor(const bf::Transform &t = bf::Transform::Default);
        void draw(const bf::ShaderArray &shader, const bf::Settings &settings);
        void ObjectGui();
    };
}

#endif //MG1_ZAD2_MULTICURSOR_H
