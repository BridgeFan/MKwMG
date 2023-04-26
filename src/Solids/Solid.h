//
// Created by kamil-hp on 16.03.2022.
//

#ifndef MG1_ZAD2_SOLID_H
#define MG1_ZAD2_SOLID_H
#include <vector>
#include <climits>
#include "src/Object/Object.h"
namespace bf {
    struct Vertex {
        float x;
        float y;
        float z;
        Vertex(float x, float y, float z);
        Vertex(const glm::vec3& p);
    };
	class Shader;
	struct Settings;
	class Solid : public bf::Object {
		static int sindex;
	protected:
		void addVertex(const glm::vec3& p);
	public:
		virtual ~Solid() override;
		Solid(const bf::Transform &t, const std::string &solidName) : bf::Object(t, solidName) {}
		explicit Solid(const bf::Transform &t = bf::Transform::Default) : bf::Solid(t, "Solid " + std::to_string(
				sindex)) { sindex++; }
		explicit Solid(const std::string &solidName) : Solid(Transform::Default, solidName) {}
		unsigned int VBO = UINT_MAX, VAO = UINT_MAX, IBO = UINT_MAX;
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
		void setBuffers();
	public:
		virtual void draw(const bf::ShaderArray &shader) const override;
		virtual void draw(const bf::ShaderArray &shader, const bf::Transform &relativeTo) const;
		void ObjectGui() override;

        ShaderType getShaderType() const override;

    };
}


#endif //MG1_ZAD2_SOLID_H
