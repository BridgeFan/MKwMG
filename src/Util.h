#pragma once
//
// Created by kamil-hp on 23.03.2022.
//

#ifndef MG1_ZAD2_UTIL_H
#define MG1_ZAD2_UTIL_H
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <numbers>
#include <optional>
#include <string>
#include <vector>
constexpr float PI = std::numbers::pi_v<float>;

struct ImGuiIO;
namespace bf {
	using vec2d = glm::vec<2, double, glm::defaultp>;
	using vec3d = glm::vec<3, double, glm::defaultp>;
	using vec4d = glm::vec<4, double, glm::defaultp>;
	template<typename T>
	concept arithmetic = std::is_arithmetic<T>::value;
	template<typename T>
	concept linearF = requires(const T& t1, const T& t2, float u) {
		{t1+t2} -> std::convertible_to<T>;
		{t1-t2} -> std::convertible_to<T>;
		{t1*u} -> std::convertible_to<T>;
		{u*t1} -> std::convertible_to<T>;
	};
	template<typename T>
	concept linearD = requires(const T& t1, const T& t2, double u) {
		{t1+t2} -> std::convertible_to<T>;
		{t1-t2} -> std::convertible_to<T>;
		{t1*u} -> std::convertible_to<T>;
		{u*t1} -> std::convertible_to<T>;
	};
	template<typename T>
	concept linearLD = requires(const T& t1, const T& t2, long double u) {
		{t1+t2} -> std::convertible_to<T>;
		{t1-t2} -> std::convertible_to<T>;
		{t1*u} -> std::convertible_to<T>;
		{u*t1} -> std::convertible_to<T>;
	};
	template<typename T>
	concept linear = linearF<T> || linearD<T> || linearLD<T>;
	template<std::size_t N>
	glm::vec<N,float,glm::defaultp> toFloatVec(const glm::vec<N,double,glm::defaultp>& v) {
		glm::vec<N,float,glm::defaultp> ret;
		for(std::size_t i=0u;i<N;i++)
			ret[i]=static_cast<float>(v[i]);
		return ret;
	}
}

template<std::floating_point T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& out, const glm::vec<2, T, Q>& v) {
	out << v.x << " " << v.y;
	return out;
}
template<std::floating_point T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& out, const glm::vec<3, T, Q>& v) {
	out << v.x << " " << v.y << " " << v.z;
	return out;
}
template<std::floating_point T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& out, const glm::vec<4, T, Q>& v) {
	out << v.x << " " << v.y << " " << v.z << " " << v.a;
	return out;
}

template<typename T, int N>
glm::mat<N,N,T> tensorProduct(const glm::vec<N,T>& a, const glm::vec<N,T>& b) {
	glm::mat<N,N,T> ret(T(0));
	for (int i=0;i<N;i++) {
		for (int j=0;j<N;j++) {
			ret[i][j]=a[i]*b[j];
		}
	}
	return ret;
}
template<typename T>
glm::mat<3,3,T> crossMatrix(const glm::vec<3,T>& a) {
	glm::mat<3,3,T> ret(T(0));
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			if (i==j) continue;
			int k=3-i-j;
			int sign = (((k>i)==(k>j))!=(i>j)) ? 1 : -1;
			ret[i][j]=a[k]*sign;
		}
	}
	return ret;
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

struct SegmentIntersectionResult {
	double u, v;
	double error;
};

template<bf::arithmetic T>
T sign(T a) {
	if (a==T()) return 0.0;
	return a>=T() ? T(1) : T(-1);
}

template<bf::arithmetic T>
glm::vec<3, T> nlerp(const glm::vec<3,T>& a, const glm::vec<3,T>& b, T t) {
	T na = glm::length(a);
	T nb = glm::length(b);
	return glm::normalize(lerp(a,b,t))*lerp(na,nb,t);
}


std::optional<SegmentIntersectionResult> segmentIntersection(const bf::vec3d& p1, const bf::vec3d& p2, const bf::vec3d& q1, const bf::vec3d& q2);
void floodFill(std::vector<uint8_t>& array, int TN, int i, int j, bool wrapX, bool wrapY, uint8_t color1, uint8_t color2=63u);
void BresenhamLine(std::vector<uint8_t>& array, int width, int x1, int y1, int x2, int y2, uint8_t color);
void setPixel(std::vector<uint8_t>& array, int width, int i, int j, uint8_t color);
uint8_t getPixel(const std::vector<uint8_t>& array, int width, int i, int j);


bool almostEqual(float a1, float a2, float eps=1e-7);
bool almostEqual(const glm::vec3& v1, const glm::vec3& v2, float eps=1e-5);
namespace  bf {
    glm::vec3 toScreenPos(int screenWidth, int screenHeight, const glm::vec3 &worldPos, const glm::mat4 &view,
                          const glm::mat4 &projection);

    glm::vec3 toGlobalPos(int screenWidth, int screenHeight, const glm::vec3 &mousePos, const glm::mat4 &inverseView,
                          const glm::mat4 &inverseProjection);

    constexpr glm::vec3 outOfWindow={-1.f,-1.f,-1.f};
    bool isInBounds(int screenWidth, int screenHeight, const glm::vec2& screenPos);
    bool isInBounds(int screenWidth, int screenHeight, const glm::vec3& mousePos);
    float getDeltaTime();
	float distance(const glm::vec3& a, const glm::vec3& b);
	float length(const glm::vec3& a);
	float sqrLength(const glm::vec3& a);
	template<int L, typename T>
	float sqrDistance(const glm::vec<L, T>& a, const glm::vec<L, T>& b) {
		return glm::dot(a - b, a-b);
	}
	float degrees(float a);
	glm::vec3 degrees(const glm::vec3& v);
	float radians(float a);
	glm::vec3 radians(const glm::vec3& v);
	glm::vec3 matrixToEulerXYZ(glm::mat4 const& M);
	glm::vec3 matrixToEulerYXZ(glm::mat4 const& M);
}

#endif //MG1_ZAD2_UTIL_H
