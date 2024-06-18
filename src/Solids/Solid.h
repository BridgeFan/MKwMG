#pragma once
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
        float tX;
        float tY;
		Vertex() noexcept;
        Vertex(float x, float y, float z, float tX=.0f, float tY=.0f) noexcept;
        Vertex(const glm::vec3& p, const glm::vec2& t=glm::vec2(.0f)) noexcept;
        void setPosition(const glm::vec3& p) noexcept;
        void setTexturePosition(const glm::vec2& t) noexcept;
		glm::vec3 getPosition() const {return {x,y,z};}
		glm::vec2 getTexturePosition() const {return {tX,tY};}
    };
	struct Shader;
	struct ConfigState;
	class Solid : public bf::Object {
		static int sindex;
	protected:
		void addVertex(const glm::vec3& p);
		bool isDynamic = false;
        static void swapSolids(bf::Solid& a, bf::Solid& b);
		[[nodiscard]] virtual std::pair<std::vector<bf::Vertex>,std::vector<unsigned> > createDebugInfo(int n) const;
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
		unsigned int debugVBO = UINT_MAX, debugVAO = UINT_MAX, debugIBO = UINT_MAX;
		unsigned int debugN=0;
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;
		void setBuffers();
		void drawDebug(const bf::ShaderArray &shader, bool isModelUsed=false) const;
		void updateDebug(int N=20);
		void setDebugBuffers(const std::vector<bf::Vertex>& vert, const std::vector<unsigned>& ind, bool areIndicesSet=false);
	public:
		void draw(const bf::ShaderArray &shader) const override;
        void anyDraw(const bf::ShaderArray &shader) const;
		//virtual void bezierDraw(const bf::ShaderArray &shader, const bf::Transform &relativeTo) const;
		void ObjectGui() override;
        void glUpdateVertices() const;
        ShaderType getShaderType() const override;

    };
    class DummySolid : public bf::Solid {
    public:
        DummySolid(const DummySolid&)=delete;
        DummySolid(bf::DummySolid&& solid) noexcept : bf::Solid(std::move(solid)) {}
        DummySolid& operator=(const DummySolid&)=delete;
        DummySolid& operator=(bf::DummySolid&& solid) noexcept;
        DummySolid(const std::string &solidName, bool dynamic=false);
		void onMergePoints(int, int) override {}
    };
}


#endif //MG1_ZAD2_SOLID_H
