//
// Created by kamil-hp on 23.03.2022.
//
#include "Util.h"
#include "ConfigState.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

std::string toString(const glm::vec3& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string toString(const glm::vec4& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+","+std::to_string(v.w)+")";
}

std::string readWholeFile(const std::string& path) {
    std::ifstream t(path);
    if(t.bad()) {
        std::cerr << path+" not found!\n";
        return "";
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

template<typename T, int N>
std::ostream& operator<<(std::ostream& out, const glm::vec<N, T>& v) {
	out << v.x << " " << v.y;
	if (N>=3)
		out << " " << v.z;
	if (N>=4)
		out << " " << v.a;
	return out;

}

bool isnan(const glm::vec3 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}
bool isnan(const glm::vec4 &v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w);
}

std::optional<SegmentIntersectionResult> segmentIntersection(const bf::vec3d &p1, const bf::vec3d &p2, const bf::vec3d &q1, const bf::vec3d &q2) {
	//Au+Bv=C
	bf::vec3d A = p2-p1;
	bf::vec3d B = q1-q2;
	bf::vec3d C = q1-p1;
	double N = A.x*B.y - A.y*B.x;
	if(std::abs(N) < 1e-5) {
		//parallel - no reasonable intersecion
		return std::nullopt;
	}
	double u = (C.x*B.y - C.y*B.x)/N;
	double v = (A.x*C.y - A.y*C.x)/N;
	u=std::clamp(u,0.0,1.0);
	v=std::clamp(v,0.0,1.0);
	bf::vec3d C1 = lerp(p1, p2, u);
	bf::vec3d C2 = lerp(q1, q2, v);
	double error = glm::dot(C1-C2,C1-C2);
	if(error<0.0016)
		return SegmentIntersectionResult(u,v,error);
	else
		return std::nullopt;
}

bool almostEqual(float a1, float a2, float eps) {
	return std::abs(a1-a2)<=eps*std::max(std::max(std::abs(a1),std::abs(a2)),1e-5f);
}
bool almostEqual(const glm::vec3& v1, const glm::vec3& v2, float eps) {
	return bf::sqrDistance(v1,v2)<=eps;
}

glm::vec3 bf::toScreenPos(int screenWidth, int screenHeight, const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& projection) {
    auto v = projection*view*glm::vec4(worldPos,1.f);
    v/=v.w;
    v.x=(v.x+1.f)*static_cast<float>(screenWidth)*.5f;
    v.y=(1.f-v.y)*static_cast<float>(screenHeight)*.5f;
    return {v.x,v.y,v.z};
}

glm::vec3 bf::toGlobalPos(int screenWidth, int screenHeight, const glm::vec3& mousePos, const glm::mat4& inverseView, const glm::mat4& inverseProjection) {
    auto mp = mousePos;
    mp.x = 2.f*mp.x/static_cast<float>(screenWidth)-1.f;
    mp.y = 1.f-2.f*mp.y/static_cast<float>(screenHeight);
    auto v = inverseView*inverseProjection*glm::vec4(mp,1.f);
    v/=v.w;
    return {v.x,v.y,v.z};
}


bool bf::isInBounds(int screenWidth, int screenHeight, const glm::vec2 &screenPos) {
    return isInBounds(screenWidth,screenHeight,{screenPos.x,screenPos.y,0.f});
}

bool bf::isInBounds(int screenWidth, int screenHeight, const glm::vec3 &mousePos) {
    if(std::abs(mousePos.z)>1.f)
        return false;
    if(mousePos.x<0 || mousePos.x>screenWidth)
        return false;
    if(mousePos.y<0 || mousePos.y>screenHeight)
        return false;
    return true;
}

float bf::getDeltaTime() {
    using clock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;
    static clock::time_point start = clock::now();
    duration elapsed = clock::now() - start;
    start = clock::now();
    return elapsed.count();
}
float bf::sqrLength(const glm::vec3 &a) {
	return a.x*a.x+a.y*a.y+a.z*a.z;
}
float bf::length(const glm::vec3 &a) {
	return std::sqrt(sqrLength(a));
}
float bf::distance(const glm::vec3 &a, const glm::vec3 &b) {
	return length(a-b);
}
float bf::degrees(float a) {
	return 180.f/PI*a;
}
glm::vec3 bf::degrees(const glm::vec3& a) {
	return (180.f/PI)*a;
}
float bf::radians(float a) {
	return a*PI/180.f;
}
glm::vec3 bf::radians(const glm::vec3& a) {
	return a*(PI/180.f);
}
glm::vec3 bf::matrixToEulerXYZ(const glm::mat4 &M) {
	//calculate X
	float x = std::atan2(M[2][1], M[2][2]);
	//calculate Y
	float cy = std::sqrt(M[0][0]*M[0][0] + M[1][0]*M[1][0]);
	float y = std::atan2(-M[2][0], cy);
	//calculate Z
	float sx = std::sin(x);
	float cx = std::cos(x);
	float z = std::atan2(sx*M[0][2] - cx*M[0][1], cx*M[1][1] - sx*M[1][2]);
	return {-x,-y,-z};
}

glm::vec3 bf::matrixToEulerYXZ(const glm::mat4 &M) {
	//calculate Y
	float x = std::atan2(M[2][0], M[2][2]);
	//calculate X
	float cy = std::sqrt(M[0][1]*M[0][1] + M[1][1]*M[1][1]);
	float y = std::atan2(-M[2][1], cy);
	//calculate Z
	float sx = std::sin(x);
	float cx = std::cos(x);
	float z = std::atan2(sx*M[1][2] - cx*M[1][0], cx*M[0][0] - sx*M[0][2]);
	return {-x,-y,-z};
}

void floodFill(std::vector<uint8_t>& array, int TN, int i, int j, bool wrapX, bool wrapY, uint8_t color1, uint8_t color2) {
	if(array[i+TN*j]>color2 || i<0 || i>=TN || j<0 || j>=TN) //already filled or out of bounds
		return;
	std::queue<std::pair<int, int>> Q;
	Q.emplace(i,j);
	while(!Q.empty()) {
		auto [pi,pj] = Q.front();
		Q.pop();
		if(array[pi+TN*pj]<color2 && array[pi+TN*pj]!=color1) {//to fill
			array[pi+TN*pj]=color1;
			if(pi>0 || wrapX)
				Q.emplace((TN+pi-1)%TN,pj);
			if(pi<TN-1 || wrapX)
				Q.emplace((pi+1)%TN,pj);
			if(pj>0 || wrapY)
				Q.emplace(pi,(TN+pj-1)%TN);
			if(pj<TN-1 || wrapY)
				Q.emplace(pi,(pj+1)%TN);
		}
	}

}



void setPixel(std::vector<uint8_t>& array, int width, int i, int j, uint8_t color) {
	int xi=(i+width)%width;
	int xj=(j+width)%width;
	array[xi+width*xj]=color;
}

uint8_t getPixel(const std::vector<uint8_t>& array, int width, int i, int j) {
	int xi=(i+width)%width;
	int xj=(j+width)%width;
	return array[xi+width*xj];
}

void BresenhamLine(std::vector<uint8_t>& array, int TN, int x1, int y1, int x2, int y2, uint8_t color)
{	//kod wzięty z artykułu https://pl.wikipedia.org/wiki/Algorytm_Bresenhama
	// zmienne pomocnicze
	int d, dx, dy, ai, bi, xi, yi;
	const int BIG=static_cast<int>(TN*0.8);
	if(x2-x1>=BIG) {
		x1+=TN;
	}
	if(x2-x1<=-BIG) {
		x2+=TN;
	}
	if(y2-y1>=BIG) {
		y1+=TN;
	}
	if(y2-y1<=-BIG) {
		y2+=TN;
	}
	int x = x1, y = y1;
	// ustalenie kierunku rysowania
	if (x1 < x2)
	{
		xi = 1;
		dx = x2 - x1;
	}
	else
	{
		xi = -1;
		dx = x1 - x2;
	}
	// ustalenie kierunku rysowania
	if (y1 < y2)
	{
		yi = 1;
		dy = y2 - y1;
	}
	else
	{
		yi = -1;
		dy = y1 - y2;
	}
	// pierwszy piksel
	setPixel(array,TN,x%TN,y%TN,color);
	// oś wiodąca OX
	if (dx > dy)
	{
		ai = (dy - dx) * 2;
		bi = dy * 2;
		d = bi - dx;
		// pętla po kolejnych x
		while (x != x2)
		{
			// test współczynnika
			if (d >= 0)
			{
				x += xi;
				y += yi;
				d += ai;
			}
			else
			{
				d += bi;
				x += xi;
			}
			setPixel(array,TN,x%TN,y%TN,color);
		}
	}
	// oś wiodąca OY
	else
	{
		ai = ( dx - dy ) * 2;
		bi = dx * 2;
		d = bi - dy;
		// pętla po kolejnych y
		while (y != y2)
		{
			// test współczynnika
			if (d >= 0)
			{
				x += xi;
				y += yi;
				d += ai;
			}
			else
			{
				d += bi;
				y += yi;
			}
			setPixel(array,TN,x%TN,y%TN,color);
		}
	}
}

