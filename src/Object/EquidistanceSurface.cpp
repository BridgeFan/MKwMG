//
// Created by Kamil on 17.12.2024.
//

#include "EquidistanceSurface.h"
#include "MullingPathCreator.h"

namespace bf {
    double bf::EquidistanceSurface::scale = 0.0;
    EquidistanceSurface::EquidistanceSurface(Object &obj, double r): object(obj), scaledR(r/scale) {

    }

    void EquidistanceSurface::setMullingPathCreator(const MullingPathCreator &mpc) {
        scale = mpc.getMPCScale();
    }

    void EquidistanceSurface::draw(const ShaderArray &) const {}

    void EquidistanceSurface::onMergePoints(int, int) {}

    ShaderType EquidistanceSurface::getShaderType() const {
        return ShaderType::BasicShader;
    }

    bf::vec2d EquidistanceSurface::getParameterMin() const {
        return object.getParameterMin();
    }

    bf::vec2d EquidistanceSurface::getParameterMax() const {
        return object.getParameterMax();
    }

    bool EquidistanceSurface::parameterWrappingU() const {
        return object.parameterWrappingU();
    }

    bool EquidistanceSurface::parameterWrappingV() const {
        return object.parameterWrappingV();
    }

    bf::vec3d EquidistanceSurface::parameterFunction(double u, double v) const {
        auto n = glm::cross(object.parameterGradientU(u,v), object.parameterGradientV(u,v));
        return Object::parameterFunction(u, v)-glm::normalize(n)*scaledR;
    }

    bf::vec3d EquidistanceSurface::parameterGradientU(double u, double v) const {
        auto n = glm::cross(object.parameterHesseUU(u,v), object.parameterGradientV(u,v))+glm::cross(object.parameterGradientU(u,v), object.parameterHesseUV(u,v));
        return Object::parameterFunction(u, v)-glm::normalize(n)*scaledR;
    }

    bf::vec3d EquidistanceSurface::parameterGradientV(double u, double v) const {
        auto n = glm::cross(object.parameterHesseUV(u,v), object.parameterGradientV(u,v))+glm::cross(object.parameterGradientU(u,v), object.parameterHesseVV(u,v));
        return Object::parameterFunction(u, v)-glm::normalize(n)*scaledR;
    }

    bf::Object &EquidistanceSurface::getObject() {
        return object;
    }

    const bf::Object &EquidistanceSurface::getObject() const {
        return object;
    }
} // bf