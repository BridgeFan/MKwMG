//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE2_H
#define MG1_ZAD2_BEZIERCURVE2_H

#include "Solids/Solid.h"
#include "Object/ObjectArrayListener.h"
#include "BasicBezier.h"

class GLFWwindow;
namespace bf {
	class Scene;
	struct Settings;
	class BezierCurve2: public bf::Solid, public bf::ObjectArrayListener {
        bf::BasicBezier bezier;
		static GLFWwindow* window;
		static int _index;
		bool isPolygonVisible, isCurveVisible;
		std::size_t activeIndex;
		std::vector<unsigned> pointIndices;
		static const Scene* scene;
		static const Settings* settings;
		void recalculate();
		void recalculatePart(int index); //index of part
	public:
		explicit BezierCurve2(bf::ObjectArray& array);
		void onRemoveObject(unsigned index) override;
		void onMoveObject(unsigned index) override;

        [[nodiscard]] std::vector<unsigned int> usedVectors() const override;

        bool addPoint(unsigned index) override;
		bool removePoint(unsigned index);
		virtual void draw(const Shader &shader) const override;
		static void initData(const Scene& scene, const Settings& settings, GLFWwindow* window);
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override {return false;}

        bool onKeyPressed(int key, int mods) override;
    };
}


#endif //MG1_ZAD2_BEZIERCURVE2_H
