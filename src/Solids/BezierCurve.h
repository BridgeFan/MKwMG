//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE_H
#define MG1_ZAD2_BEZIERCURVE_H

#include "Solid.h"
#include "Object/ObjectArrayListener.h"

constexpr int MAX_FOV_PARTS = 256;
constexpr int MAX_FOV_LOG_PARTS = 8; //should be equal to log2(MAX_FOV_PARTS)

class GLFWwindow;
namespace bf {
	class Scene;
	struct Settings;
	class BezierCurve: public bf::Solid, public bf::ObjectArrayListener {
		unsigned int FVBO = UINT_MAX, FVAO = UINT_MAX, FIBO = UINT_MAX;
		static GLFWwindow* window;
		static int _index;
		bool isPolygonVisible, isCurveVisible;
		std::size_t activeIndex;
		std::vector<unsigned> pointIndices;
		std::vector<int> fovIndices;
		std::vector<Vertex> fovVertices;
		static const Scene* scene;
		static const Settings* settings;
		void recalculate();
		void recalculatePart(int index); //index of part
		void setFovBuffers();
	public:
		explicit BezierCurve(bf::ObjectArray& array);
		void onRemoveObject(unsigned index) override;
		void onMoveObject(unsigned index) override;

        [[nodiscard]] std::vector<unsigned int> usedVectors() const override;

        bool addPoint(unsigned index) override;
		bool removePoint(unsigned index);
		void draw(const Shader &shader) const override;
		static void initData(const Scene& scene, const Settings& settings, GLFWwindow* window);
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override {return false;}

        bool onKeyPressed(int key, int mods) override;
    };
}


#endif //MG1_ZAD2_BEZIERCURVE_H
