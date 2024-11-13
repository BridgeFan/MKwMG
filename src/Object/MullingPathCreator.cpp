//
// Created by kamil-tp on 12.11.24.
//

#include "MullingPathCreator.h"
#include "ObjectArray.h"
#include "Util.h"

#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"

#include <fstream>
#include <iostream>

namespace bf {
	constexpr double Radius = 8.0;
	constexpr double step = 0.05;
	constexpr int RESX = 14, RESY = 50; //10.71mm/3mm of precision
	constexpr int difY= std::ceil(Radius*RESY/150.0);

	inline bf::vec3d MullingPathCreator::c(const bf::vec3d &p) const {
		return p*scale+move;
	}
	inline bf::vec3d MullingPathCreator::dc(const bf::vec3d &p) const {
		return (p-move)/scale;
	}

	MullingPathCreator::MullingPathCreator(bf::ObjectArray &a) : objectArray(a) {
		name = "MullingPathCreator";
		bf::vec3d minPoint={0.0, 0.0, 0.0};
		bf::vec3d maxPoint={0.0, 0.0, 0.0};
		//find extreme points and set scale
		for (unsigned i = 0; i < objectArray.size(); i++) {
			if (!objectArray.isCorrect(i))
				continue;
			if (typeid(objectArray[i]) != typeid(bf::MullingPathCreator)) {
				objectArray[i].indestructibilityIndex++;
				auto&& [lmin, lmax] = objectArray[i].getObjectRange();
				if(lmin==lmax)
					continue;
				for(int j=0;j<3;j++) {
					minPoint[j]=std::min<double>(minPoint[j], lmin[j]);
					maxPoint[j]=std::max<double>(maxPoint[j], lmax[j]);
				}
			}
			else {
				shallBeDestroyed=true;
			}
		}
		scale=140.0/std::max({maxPoint.x-minPoint.x, maxPoint.y-minPoint.y});
		move = -lerp(minPoint,maxPoint,0.5)*scale; //movement IN MULLING SPACE
		move.z = 15.0;
		//create debug dummy solids
		static const std::vector vertices = {bf::Vertex(-0.5,-0.5,0.0),bf::Vertex(-0.5,0.5,0.0),bf::Vertex(0.5,0.5,0.0),bf::Vertex(0.5,-0.5,0.0)};
		static const std::vector indices = {0u,1u,1u,2u,2u,3u,3u,0u,0u,2u,1u,3u};
		bf::vec3d scaleVector = {150.0/RESX/scale,150.0/RESY/scale,1.0};
		for(int i=0;i<RESX;i++) {
			for(int j=0;j<RESY;j++) {
				bf::DummySolid tmpSolid("");
				tmpSolid.vertices = vertices;
				tmpSolid.indices = indices;
				bf::vec3d pos = {(150.0*(i+0.5-RESX*0.5)/RESX),(150.0*(j+0.5-RESY*0.5)/RESY),0.0};
				tmpSolid.setPosition(dc(pos));
				tmpSolid.setScale(scaleVector);
				tmpSolid.setBuffers();
				dummySolids.emplace_back(std::move(tmpSolid));
			}
		}
	}


	MullingPathCreator::~MullingPathCreator() {
		for (unsigned i = 0; i < objectArray.size(); i++) {
			if (objectArray.isCorrect(i) && typeid(objectArray[i]) != typeid(bf::MullingPathCreator))
				objectArray[i].indestructibilityIndex--;
		}
	}
	void MullingPathCreator::draw(const bf::ShaderArray & shaderArray) const {
		shaderArray.setColor(255u,0u,0u);
		for(auto& s: dummySolids)
			s.draw(shaderArray);
	}


	void MullingPathCreator::ObjectGui() {
		//TODO - functions
		ImGui::Text("Scale: %f", scale);
		ImGui::Text("Move: %f, %f, %f", move.x, move.y, move.z);
		bf::imgui::checkChanged("Object name", name);
		if (ImGui::Button("Approximate path")) {
			createApproximateMullingPath();
		}
		bool wereFound = areIntersectionsFound;
		if (areIntersectionsFound)
			ImGui::BeginDisabled();
		if (ImGui::Button("Find intersections path")) {
			areIntersectionsFound = true;
		}
		if (wereFound)
			ImGui::EndDisabled();
		if (!areIntersectionsFound)
			ImGui::BeginDisabled();
		if (ImGui::Button("Flat-end mulling path")) {
		}
		if (ImGui::Button("Exact mulling path")) {
		}
		if (ImGui::Button("Author signature")) {
		}
		if (!areIntersectionsFound)
			ImGui::EndDisabled();
	}


	bool MullingPathCreator::saveToFile(const std::string &path, const std::vector<bf::vec3d> &points) {
#ifdef WIN32
		std::string endl = "\n";
#else
		std::string endl = "\r\n";
#endif
		std::ofstream file(path);
		if (!file.good())
			return false;
		file << std::fixed << std::setprecision(3);
		int line = 3;
		setlocale(LC_ALL, "en_US.UTF-8");
		for (const auto &p: points) {
			file << "N" << line << "G01X" << p.x << "Y" << p.y << "Z" << p.z << endl;
			line++;
		}
		setlocale(LC_ALL, "");
		return true;
	}


	double aH(double h, double x, double y) {
		if(x*x+y*y>Radius*Radius)
			return -999.9;
		return h-Radius+std::sqrt(Radius*Radius-x*x-y*y);
	}

	void moveApproximate(double minHeight, std::vector<bf::vec3d>& points, bf::vec3d& actualPosition, const std::array<std::array<double, RESY>, RESX> heightMap) {
		constexpr double error=1.0;
		actualPosition={150.0*(0.5-RESX*0.5)/RESX,76.0+Radius,minHeight};
		points.push_back(actualPosition);
		for(int i=0;i<RESX;i++) {
			int sign = i%2 ? 1 : -1;
			actualPosition={150.0*(i+0.5-RESX*0.5)/RESX,-(76.0+Radius)*sign,minHeight};
			points.push_back(actualPosition);
			for(int kj=0; kj<RESY; kj++) {
				int j = sign ? kj : RESY-kj;
				double height = 0.0;
				//check min height in part
				for(int k=std::max(-difY,-j);k<=std::min(difY,RESY-j-1); k++) {
					double y=std::max(k-0.5,0.0)*150.0/RESY;
					double x=75.0/RESX;
					height=std::max(aH(heightMap[i][j+k],0.0,y), height);
					if(y*y+x*x<=Radius*Radius) {
						if(i>0)
							height=std::max(aH(heightMap[i-1][j+k],x,y), height);
						if(i<RESX-1)
							height=std::max(aH(heightMap[i+1][j+k],x,y), height);
					}
				}
				//adding heightmap error and comparing with minimal possible height
				height=std::max(minHeight,height+error);
				//move end
				auto tmpHeight = std::max(actualPosition.z,height);
				if(kj==0 && i==0)
					actualPosition.y = -75.0 * sign;
				else
					actualPosition.y+=sign*75.0/RESY;
				actualPosition.z=tmpHeight;
				points.push_back(actualPosition);
				actualPosition.y+=sign*75.0/RESY;
				actualPosition.z=height;
				points.push_back(actualPosition);
			}
			actualPosition.y=sign*(76.0+Radius);
			points.push_back(actualPosition);
			actualPosition.x+=150.0/RESX;
			points.push_back(actualPosition);
		}
	}


	void MullingPathCreator::createApproximateMullingPath() {
		//find trial points (scaled to mm)
		std::vector<bf::vec3d> tPoints;
		for(unsigned i=0;i<objectArray.size();i++) {
			if (!objectArray.isCorrect(i))
				continue;
			const auto& o=objectArray[i];
			if (!o.isIntersectable())
				continue;
			for(double u=0.0; u<o.getParameterMax().x-0.005; u+=step) {
				for(double v=0.0; v<o.getParameterMax().y-0.005; v+=step) {
					tPoints.emplace_back(c(o.parameterFunction(u,v)));
				}
			}
		}
		std::cout << tPoints.size() << "\n";
		//create heightmap
		std::array<std::array<double, RESY>, RESX> heightMap = {0.0};
		for(const auto& p: tPoints) {
			const int pX = static_cast<int>((p.x + 75.0) * RESX / 150.0);
			const int pY = static_cast<int>((p.y+75.0) * RESY / 150.0);
			heightMap[pX][pY]=std::max(heightMap[pX][pY], p.z);
		}
		for(int i=0;i<RESX;i++) {
			for(int j=0;j<RESY;j++) {
				auto& o=dummySolids[i*RESY+j];
				glm::vec3 pos = o.getPosition();
				pos.z = static_cast<float>((std::max(heightMap[i][j],15.0)-move.z)/scale);
				o.setPosition(pos);
			}
		}
		std::cout << "Heightmap created\n";
		std::vector<bf::vec3d> points;
		points.emplace_back(0.0,0.0,60.0);
		bf::vec3d actualPosition={150.0*(0.5-RESX*0.5)/RESX,76.0+Radius,60.0};
		points.push_back(actualPosition);
		moveApproximate(34.0,points,actualPosition,heightMap);
		moveApproximate(16.0,points,actualPosition,heightMap);
		//set path for
		actualPosition.z=60.0;
		points.push_back(actualPosition);
		actualPosition={0.0,0.0,60.0};
		points.push_back(actualPosition);
		if(saveToFile("1.k"+std::to_string(2*static_cast<int>(Radius)), points))
			std::cout << "Approximate paths created\n";
		else
			std::cerr << "Error when trying to save approximate paths\n";
	}
} // bf