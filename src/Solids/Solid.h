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
	struct ConfigState;
	class Solid : public bf::Object {
		static int sindex;
	protected:
		void addVertex(const glm::vec3& p);
		bool isDynamic = false;
        static void swapSolids(bf::Solid& a, bf::Solid& b);
	public:
		virtual ~Solid()=0;
        Solid(const Solid&)=delete;
        Solid(Solid&&) noexcept;
		Solid(const bf::Transform &t, const std::string &solidName, bool dynamic=false) : bf::Object(t, solidName),
			isDynamic(dynamic) {}

		explicit Solid(const bf::Transform &t = bf::Transform::Default, bool dynamic=false) : bf::Solid(t, "Solid " + std::to_string(
				sindex),dynamic) { sindex++; }
		explicit Solid(const std::string &solidName, bool dynamic=false) : Solid(Transform::Default, solidName,dynamic) {}
		unsigned int VBO = UINT_MAX, VAO = UINT_MAX, IBO = UINT_MAX;
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
		void setBuffers();
	public:
		virtual void draw(const bf::ShaderArray &shader) const override;
		//virtual void draw(const bf::ShaderArray &shader, const bf::Transform &relativeTo) const;
		void ObjectGui() override;
        ShaderType getShaderType() const override;

    };
    class DummySolid : public bf::Solid {
    public:
        DummySolid(const DummySolid&)=delete;
        DummySolid(bf::DummySolid&& solid) noexcept : bf::Solid(std::move(solid)) {}
        DummySolid& operator=(const DummySolid&)=delete;
        DummySolid& operator=(bf::DummySolid&& solid) noexcept;
        DummySolid(const std::string &solidName, bool dynamic=false);
    };
}


#endif //MG1_ZAD2_SOLID_H
