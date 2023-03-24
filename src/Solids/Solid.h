//
// Created by kamil-hp on 16.03.2022.
//

#ifndef MG1_ZAD2_SOLID_H
#define MG1_ZAD2_SOLID_H
#include <vector>
#include <climits>
#include "Object.h"

class Shader;
struct Settings;

class Solid: public Object {
	static int index;
public:
	~Solid() override;
	Solid(const Transform& transform, const std::string& name): Object(transform, name) {}
	explicit Solid(const Transform& transform=Transform::Default): Solid(transform, "Solid "+std::to_string(index)) {index++;}
	explicit Solid(const std::string& name): Solid(Transform::Default, name) {}
	unsigned int VBO=UINT_MAX, VAO=UINT_MAX, IBO=UINT_MAX;
	std::vector<float> vertices;
	std::vector<unsigned> indices;
	void setBuffers();
public:
	virtual void draw(const Shader& shader) const override;
    virtual void draw(const Shader& shader, const Transform& relativeTo) const;
	void ObjectGui() override;

};


#endif //MG1_ZAD2_SOLID_H
