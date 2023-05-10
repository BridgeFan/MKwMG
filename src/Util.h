//
// Created by kamil-hp on 23.03.2022.
//

#ifndef MG1_ZAD2_UTIL_H
#define MG1_ZAD2_UTIL_H
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>
#include <glm/mat4x4.hpp>

#if defined(__clang__) && __clang_major__>=14
#define USE_STD_FORMAT
#endif
#if defined(__GNUC__) && __GNUC_PREREQ(13,0)
#define USE_STD_FORMAT
#endif
#if defined(_MSC_VER) && _MSC_VER>=1929
#define USE_STD_FORMAT
#endif
constexpr float PI = std::numbers::pi_v<float>;

class ImGuiIO;
namespace bf {
	template<typename T>
	concept arithmetic = std::is_arithmetic<T>::value;
	template<typename T>
	concept linear = requires(const T& t1, const T& t2, float u) {
		{t1+t2} -> std::convertible_to<T>;
		{t1-t2} -> std::convertible_to<T>;
		{t1*u} -> std::convertible_to<T>;
		{u*t1} -> std::convertible_to<T>;
	};
}

std::string readWholeFile(const std::string& path);

std::string toString(const glm::vec3& v);
std::string toString(const glm::vec4& v);
bool isnan(const glm::vec3& v);
bool isnan(const glm::vec4& v);
template<bf::arithmetic T, std::integral U>
T fastPow(T base, U power) {
	if(power<0) {
		power = -power;
		base = static_cast<T>(1)/base;
	}
	T result = static_cast<T>(1);
	while(power > 1) {
		if(power & 1) { // Can also use (power & 1) to make code even faster
			result *= base;
		}
		base *= base;
		power >>= 1; // Can also use power >>= 1; to make code even faster
	}
	return base*result;
}
template<bf::linear T, std::floating_point U>
T lerp(const T& p1, const T& p2, U t) {
	return p1*(1-t)+p2*t;
}

template<bf::linear T>
std::vector<T> tridiagonalMatrixAlgorithm(const std::vector<float>& a, const std::vector<float>& b, const std::vector<float>& c, const std::vector<T>& d) {
	//a - lower diagonal (size N-1)
	//b - main diagonal (size N)
	//c - upper diagonal (size N-1)
	//d - right side of equation (size N)
	int n = d.size();
	if(n==0 || static_cast<int>(a.size())!=n-1 || static_cast<int>(b.size())!=n || static_cast<int>(c.size())!=n-1)
		return {};
    if(n==1) {
        //special case
        return {d[0]*(1/b[0])};
    }
	std::vector<float> ck;
	std::vector<T> dk, x;
	x.resize(n);
	ck.resize(n-1);
	dk.resize(n);
	ck[0]=c[0]/b[0];
	dk[0]=d[0]*(1.f/b[0]);
	for(int i=1;i<n;i++) {
		if(i<n-1)
			ck[i]=c[i]/(b[i]-a[i-1]*ck[i-1]);
		dk[i]=(d[i]-a[i-1]*dk[i-1])*(1.f/(b[i]-a[i-1]*ck[i-1]));
	}
	x[n-1]=dk[n-1];
	for(int i=n-2;i>=0;i--)
		x[i]=dk[i]-ck[i]*x[i+1];
	return x;
}


bool almostEqual(float a1, float a2, float eps=1e-7);
namespace  bf {
    glm::vec3 toScreenPos(int screenWidth, int screenHeight, const glm::vec3 &worldPos, const glm::mat4 &view,
                          const glm::mat4 &projection);

    glm::vec3 toGlobalPos(int screenWidth, int screenHeight, const glm::vec3 &mousePos, const glm::mat4 &inverseView,
                          const glm::mat4 &inverseProjection);

    constexpr glm::vec3 outOfWindow={-1.f,-1.f,-1.f};
    bool isInBounds(int screenWidth, int screenHeight, const glm::vec2& screenPos);
    bool isInBounds(int screenWidth, int screenHeight, const glm::vec3& mousePos);
    float getDeltaTime();
}
#endif //MG1_ZAD2_UTIL_H
