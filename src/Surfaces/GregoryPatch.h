//
// Created by kamil-hp on 28.05.23.
//

#ifndef MKWMG_GREGORYPATCH_H
#define MKWMG_GREGORYPATCH_H
#include "Object/ObjectArrayListener.h"
#include "Solids/Solid.h"
#include <array>

namespace bf {
	class BezierSurfaceSegment;
	class BezierSurface0;
	class GregoryPatch: public bf::Solid, public bf::ObjectArrayListener {
		std::array<bf::BezierSurfaceSegment*,3> segments={nullptr,nullptr,nullptr};
		std::array<bf::BezierSurface0*, 3> surfaces={nullptr,nullptr,nullptr};

	public:
		[[nodiscard]] const std::array<bf::BezierSurface0 *, 3> &getSurfaces() const;

	private:
		std::array<uint8_t, 6> order; //indices in Béziers
		void recalculate();
		bool isDebug=true;
		int samples=4;
		glm::vec3 getPointPos(int seg, bool isFurther);
		glm::vec3 getPointPos(int seg, int x, int y);
	public:
		[[nodiscard]] const std::array<bf::BezierSurfaceSegment *, 3> &getSegments() const;
		static constexpr std::array tmpArray{0u,3u,12u,15u};
		GregoryPatch(bf::ObjectArray& objectArray);
		void onMergePoints(int p1, int p2) override;
		void onRemoveObject(unsigned int index) override;
		void onMoveObject(unsigned int index) override;
		bool postInit() override;
		void draw(const ShaderArray &shader) const override;
		~GregoryPatch() override;
		void ObjectGui() override;
		bool isMovable() const override;
		[[nodiscard]] const std::array<uint8_t, 6>& getOrder() const {return order;}
	};

}// namespace bf

#endif//MKWMG_GREGORYPATCH_H
