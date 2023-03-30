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
		std::vector<std::size_t> pointIndices;
		std::vector<int> fovIndices;
		std::vector<float> fovVertices;
		static const Scene* scene;
		static const Settings* settings;
		void recalculate();
		void recalculatePart(std::size_t index);
		void setFovBuffers();
	public:
		BezierCurve(bf::ObjectArray& array);
		void onRemoveObject(std::size_t index) override;

		void onMoveObject(std::size_t index) override;

		bool addPoint(std::size_t index);
		bool removePoint(std::size_t index);
		void draw(const Shader &shader) const override;
		static void initData(const Scene& scene, const Settings& settings, GLFWwindow* window);
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override {return false;}
	};
}


#endif //MG1_ZAD2_BEZIERCURVE_H
