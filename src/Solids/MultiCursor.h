//
// Created by kamil-hp on 23.03.23.
//

#ifndef MG1_ZAD2_MULTICURSOR_H
#define MG1_ZAD2_MULTICURSOR_H

#include "Solid.h"
#include "Transform.h"


class MultiCursor {
    Solid lines[3];
    void initLines();
public:
    Transform transform;
    explicit MultiCursor(const Transform& transform=Transform::Default);
    void draw(const Shader& shader, const Settings& settings);
    void ObjectGui();
};


#endif //MG1_ZAD2_MULTICURSOR_H
