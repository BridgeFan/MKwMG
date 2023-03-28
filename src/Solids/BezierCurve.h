//
// Created by kamil-hp on 27.03.23.
//

#ifndef MG1_ZAD2_BEZIERCURVE_H
#define MG1_ZAD2_BEZIERCURVE_H

#include "Solid.h"
#include "Object/ObjectArrayListener.h"

namespace bf {
	class Camera;
	struct Settings;
	class BezierCurve: public bf::Object, public bf::ObjectArrayListener {
		static int _index;
		bool isPolygonVisible, isCurveVisible;
		std::size_t activeIndex;
		std::vector<std::size_t> indices;
		const Camera& camera;
		const Settings& settings;
		void recalculate();
		void recalculatePart(std::size_t index);
	public:
		BezierCurve(bf::ObjectArray& array, const bf::Camera& camera, const bf::Settings& settings);
		void onRemoveObject(std::size_t index) override;

		void onMoveObject(std::size_t index) override;

		bool addPoint(std::size_t index);
		bool removePoint(std::size_t index);
		void draw(const Shader &shader) const override;
		void ObjectGui() override;
		[[nodiscard]] bool isMovable() const override {return false;}
	};
}


#endif //MG1_ZAD2_BEZIERCURVE_H
