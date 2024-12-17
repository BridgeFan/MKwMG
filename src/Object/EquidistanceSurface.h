//
// Created by Kamil on 17.12.2024.
//

#ifndef MKWMG_EQUIDISTANCESURFACE_H
#define MKWMG_EQUIDISTANCESURFACE_H
#include "Object.h"

namespace bf {
    class MullingPathCreator;
    struct ShaderArray;
class EquidistanceSurface: public bf::Object {
    bf::Object &object;
    const double scaledR; //radius of mulling in space coordiantes
public:
    static double scale;
    static void setMullingPathCreator(const bf::MullingPathCreator& mpc);
    EquidistanceSurface(bf::Object& object, double R);
    //formal functions (required by parent class
    void draw(const ShaderArray &shader) const override;
    void onMergePoints(int p1, int p2) override;
    ShaderType getShaderType() const override;
    //important functions
    vec2d getParameterMin() const override;
    vec2d getParameterMax() const override;
    bool parameterWrappingU() const override;
    bool parameterWrappingV() const override;
    vec3d parameterFunction(double u, double v) const override;
    vec3d parameterGradientU(double u, double v) const override;
    vec3d parameterGradientV(double u, double v) const override;
    bf::Object& getObject();
    const bf::Object& getObject() const;
};

} // bf

#endif //MKWMG_EQUIDISTANCESURFACE_H
