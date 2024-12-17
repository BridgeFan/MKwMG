//
// Created by kamil-hp on 20.03.2022.
//

#include "Object.h"
#include "Util.h"
#include "src/ImGui/ImGuiUtil.h"
#include <algorithm>

const bf::Scene* bf::Object::scene = nullptr;
const bf::ConfigState* bf::Object::configState = nullptr;
int bf::Object::_objIndex = 1;

void bf::Object::setNewTransform(const glm::vec3& centre, const bf::Transform& oldTransform, const bf::Transform& newTransform) {
    if(oldTransform==newTransform)
        return;
    //TODO - non-uniform scaling in different ways
	glm::mat4 matrix = newTransform.CalculateMatrix()*oldTransform.CalculateInverseMatrix();
	auto vec = glm::vec4(getPosition()-centre,1.f);
	setPosition(glm::vec3(matrix*vec)+centre);
    transform.scale *= (newTransform.scale / oldTransform.scale);
    //rotation
    glm::vec3 r = getRotation();
    auto rotMat = bf::getRotateMatrix(newTransform.rotation)*bf::getInverseRotateMatrix(oldTransform.rotation)*bf::getRotateMatrix(r);
    r = bf::matrixToEulerXYZ(rotMat);
    setRotation(bf::degrees(r));
}

void bf::Object::ObjectGui() {
	bf::imgui::checkChanged("Object name", name);
	glm::vec3 pos = getPosition();
	if (bf::imgui::checkChanged("Object position", pos)) {
		setPosition(pos);
	}
	glm::vec3 rot = getRotation();
	if (bf::imgui::checkChanged("Object rotation", rot)) {
		setRotation(rot);
	}
	glm::vec3 s = getScale();
	if (bf::imgui::checkChanged("Object scale", s, true)) {
		setScale(s);
	}
}

std::array<double,3> solve(std::array<double, 9>&& a, std::array<double, 3>&& free) {
	//2-1, 3-1
	for(int j=1;j<3;j++) {
		double val=a[3*j]/a[0];
		for(int i=0;i<3;i++) {
			a[3*j+i]-=val*a[i];
		}
		free[j]-=val*free[0];
	}
	//3-2
	double val=a[7]/a[4];
	for(int i=1;i<3;i++) {
		a[6+i]-=val*a[3+i];
	}
	free[2]-=val*free[1];
	//division
	std::array<double, 3> ret;
	ret[2]=free[2]/a[8];
	ret[1]=(free[1]-ret[2]*a[5])/a[4];
	ret[0]=(free[0]-ret[2]*a[2]-ret[1]*a[1])/a[0];
	return ret;
}

std::array<double, 8> bf::getApproximationParaboloid(const bf::Object &o, double u, double v,
													 const std::function<bf::vec3d(const bf::vec3d &)> &convert, const std::function<bf::vec3d(const bf::vec3d &)> &dConvert) {
	//P.x, P.y, P.z, zx, zy, zxx, zxy, zyy
	std::array<double, 8> ret;
	auto P = convert(o.parameterFunction(u, v));
	ret[0] = P.x;
	ret[1] = P.y;
	ret[2] = P.z;
	bf::vec3d Pu = dConvert(o.parameterGradientU(u, v));
	double xu = Pu.x, yu = Pu.y, zu = Pu.z;
	bf::vec3d Pv = dConvert(o.parameterGradientV(u, v));
	double xv = Pv.x, yv = Pv.y, zv = Pv.z;
	double bot = xu * yv - yu * xv;
	ret[3] = (yv * zu - yu * zv) / bot;//zx
	ret[4] = (xu * zu - xv * zv) / bot;//zy
	bf::vec3d Puu = dConvert(o.parameterHesseUU(u, v));
	double xuu = Puu.x, yuu = Puu.y, zuu = Puu.z;
	bf::vec3d Puv = dConvert(o.parameterHesseUV(u, v));
	double xuv = Puv.x, yuv = Puv.y, zuv = Puv.z;
	bf::vec3d Pvv = dConvert(o.parameterHesseVV(u, v));
	double xvv = Pvv.x, yvv = Pvv.y, zvv = Pvv.z;
	std::array<double, 9> A = {
			xu * zu, 2 * xu * yu, yu * yu,
			xu * xv, xu * yv + xv * yu, yu * yv,
			xv * xv, 2 * xv * yv, yv * yv};
	std::array<double, 3> free = {
			zuu - ret[3] * xuu - ret[4] * yuu,
			zuv - ret[3] * xuv - ret[4] * yuv,
			zvv - ret[3] * xvv - ret[4] * yvv};
	auto Ret = solve(std::move(A), std::move(free));
	for (int i = 0; i < 3; i++)
		ret[5 + i] = Ret[i];
	return ret;
}
double bf::calculateParaboloidMove(const std::array<double, 8> &paraboloid, float R) {
	//TODO - calculate minimal curvature
	//P.x, P.y, P.z, zx, zy, zxx, zxy, zyy
	double zxx = paraboloid[5];
	double zxy = paraboloid[6];
	double zyy = paraboloid[7];
	double Delta = std::sqrt(std::pow(zxx-zyy,2)+std::pow(2.*zxy,2));
	double minCurvature = -(-Delta + zxx + zyy);
	if(minCurvature<=0.0) //not problematic
		return 0.0;
	double r2 = R*R-1./(4.*minCurvature*minCurvature);
	return 0.5*minCurvature*r2-(R-std::sqrt(R*R-r2));
}

glm::vec3 bf::getMiddle(const std::vector<bf::Object>& objects) {
	glm::vec3 sumPos;
	for(const auto& o: objects) {
		sumPos+=o.getPosition();
	}
	return sumPos/=objects.size();
}

void bf::Object::setRelativeScale(const glm::vec3 &pos, float multiplier) {
	setPosition(transform.position + (pos-transform.position) * (multiplier-1));
}

glm::mat4 bf::Object::getModelMatrix(const bf::Transform &relativeTo) const {
	return transform.CalculateMatrix(relativeTo);
}

bool bf::Object::addPoint(unsigned int) { return false; }
bool bf::Object::onKeyPressed(bf::event::Key, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onKeyReleased(bf::event::Key, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onMouseButtonPressed(bf::event::MouseButton, bf::event::ModifierKeyBit) {return false;}
bool bf::Object::onMouseButtonReleased(bf::event::MouseButton, bf::event::ModifierKeyBit) {return false;}
void bf::Object::onMouseMove(const glm::vec2&, const glm::vec2&) {}

void bf::Object::initData(const bf::ConfigState &cs, const bf::Scene &s) {
	configState = &cs;
	scene = &s;
}
bf::vec4d bf::Object::clampParam(double u, double v, double modulo) const {
	bf::vec4d ret;
	if (parameterWrappingU()) {
		ret.x = std::fmod(u, getParameterMax().x);
		if (ret.x < 0) ret.x += getParameterMax().x;
	} else
		ret.x = std::clamp(u, getParameterMin().x, getParameterMax().x);
	if (parameterWrappingV()) {
		ret.y = std::fmod(v, getParameterMax().y);
		if (ret.y < 0) ret.y += getParameterMax().y;
	} else
		ret.y = std::clamp(v, getParameterMin().y, getParameterMax().y);
	if (modulo > .0) {
		ret.z = std::floor(ret.x / modulo);
		ret.w = std::floor(ret.y / modulo);
		ret.x = std::fmod(ret.x, modulo);
		ret.y = std::fmod(ret.y, modulo);
	}
	return ret;
}
bf::vec3d bf::Object::getNormal(double u, double v) const {
	return glm::cross(parameterGradientU(u, v), parameterGradientV(u, v));
}
bf::vec3d bf::Object::parameterFunction(double, double) const {return {0.,0.,0.};}
bf::vec3d bf::Object::parameterGradientU(double, double) const {return {0.,0.,0.};}
bf::vec3d bf::Object::parameterGradientV(double, double) const {return {0., 0., 0.};}
bf::vec3d bf::Object::parameterHesseUU(double, double) const {return {0., 0., 0.};}
bf::vec3d bf::Object::parameterHesseUV(double, double) const {return {0., 0., 0.};}
bf::vec3d bf::Object::parameterHesseVV(double, double) const {return {0., 0., 0.};}
std::array<bool, 2> bf::Object::parameterWrapping() const {
	return {parameterWrappingU(), parameterWrappingV()};
}
