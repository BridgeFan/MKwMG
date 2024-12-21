//
// Created by kamil-tp on 12.11.24.
//

#include "MullingPathCreator.h"

#include "ConfigState.h"
#include "IntersectionObject.h"
#include "ObjectArray.h"
#include "Util.h"

#include "ImGui/ImGuiUtil.h"
#include "ImGui/imgui_include.h"
#include "EquidistanceSurface.h"
#include "Surfaces/BezierSurface0.h"

#include "Solids/Torus.h"
#include "Json/JsonUtil.h"
#include <GL/glew.h>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <ranges>

namespace bf {
	constexpr double Radius = 8.0;
    constexpr double flatRadius = 5.0;
	constexpr double ExactRadius = 4.0;
	constexpr double step = 0.05;
	constexpr int RESX = 14, RESY = 50; //10.71mm/3mm of precision
	constexpr int difY= std::ceil(Radius*RESY/150.0);

	inline bf::vec3d MullingPathCreator::c(const bf::vec3d &p) const {
		return p*scale+move;
	}
	inline bf::vec3d MullingPathCreator::dc(const bf::vec3d &p) const {
		return (p-move)/scale;
	}
	constexpr int TN = 640;

	std::vector<bf::vec3d> MullingPathCreator::createPathForSurface(const bf::Object& o, const std::vector<uint8_t>& colorMap, uint8_t color, double paramDist) const {
		//assumes that U and V resolution are the same, max size: 65K
		//move is made on U axis
		auto size = static_cast<unsigned>(std::sqrt(colorMap.size()));
		auto dist = std::max(static_cast<unsigned>(paramDist*size/o.getParameterMax().y), 1u);
		std::vector<bf::vec3d> ret;
		for(unsigned i=0;i<size;i+=dist) {
			bool isColorInLineUsed=false;
			const double v = o.getParameterMax().y/i;
			for(unsigned j = i%2? size-1: 0; j%2 ? j<UINT16_MAX : j<size; i%2 ? j-- : j++) {
				const double u = o.getParameterMax().x/j;
				auto pos = i+size*j;
				auto fragPos = c(o.parameterFunction(u,v));
				if(colorMap[pos]==color) {
					isColorInLineUsed=true;
					auto paraboloid = getApproximationParaboloid(o,u,v,cFunc,dcFunc);
					double tMove = calculateParaboloidMove(paraboloid, ExactRadius);
					ret.push_back(fragPos+bf::vec3d(0,0,tMove));
				}
				else if(isColorInLineUsed) {
					ret.emplace_back(bf::vec2d(fragPos), 50.0);
				}
			}
		}
		return ret;
	}

	std::vector<bf::vec3d> MullingPathCreator::createPathForIntersection(const bf::IntersectionObject& io, double dist, unsigned begin, unsigned end) const {
		//distance from intersection is approximated by normal
		//TODO: move only on intersection is done
		std::vector<bf::vec3d> ret;
		auto size = io.intersectionPoints.size();
		if(begin>=end) {
			if(!io.isLooped)
				return {};
			end+=size;
		}
		ret.reserve(end-begin+1);
		for(unsigned ti=begin;ti<=end;ti++) {
			unsigned i = ti%io.intersectionPoints.size();
			auto params = io.intersectionPoints[i];
			auto pos = c(io.obj1->parameterFunction(params.x,params.y));
			auto paraboloid = getApproximationParaboloid(*io.obj1,params.x,params.y,cFunc,dcFunc);
			double tMove = calculateParaboloidMove(paraboloid, ExactRadius);
			paraboloid = getApproximationParaboloid(*io.obj1,params.z,params.w,cFunc,dcFunc);
			tMove = std::max(tMove, calculateParaboloidMove(paraboloid, ExactRadius));
			ret.push_back(pos+bf::vec3d(0,0,tMove));
		}
		return ret;
	}

	bool MullingPathCreator::createPixelMap(unsigned index) {
		//255u - special value for lines
		if (pixelMaps.contains(index) || index>=surfaces.size() || (intersections.empty() && flatIntersections.empty()))
			return false;
		std::vector<uint8_t> ret(TN*TN, 0u);
		auto indexObj = surfaces[index].get();
		if (!indexObj) return false;
		bf::vec2d pixelSize = 1.0/static_cast<double>(TN-1)*indexObj->getParameterMax();
		//surface - additional line to divide hole into two sub-holes
		if (&surfaces[index]->getObject()==this)
			BresenhamLine(ret, TN, 302, 139, 302, 209, 254u);
		//draw lines
		for(int i=0;i<intersections.size()+flatIntersections.size();i++) {
			bf::IntersectionObject* io;
			if (i<intersections.size())
				io=intersections[i];
			else
				io=flatIntersections[i-intersections.size()];
			if (!io) continue;
			int i1=0, i2=0;
			if (indexObj == io->obj1) {
				i1=0; i2=1;
			}
			else if (indexObj == io->obj2) {
				i1=2; i2=3;
			}
			if (i2 != 0) {
				std::pair<int, int> prevPoint = {INT_MAX, INT_MAX};
				std::pair<int, int> point0 = {INT_MAX, INT_MAX};
				for (const auto& v: io->intersectionPoints) {
					int i = std::round(v[i1]/pixelSize.x);
					int j = std::round(v[i2]/pixelSize.y);
					if (prevPoint.first != INT_MAX) {
						BresenhamLine(ret, TN, prevPoint.first, prevPoint.second, i, j, 255u);
					}
					else {
						point0 = {i, j};
					}
					prevPoint = {i, j};
				}
				BresenhamLine(ret, TN, prevPoint.first, prevPoint.second, point0.first, point0.second, 255u);
			}
		}
		//flood fill
		for (int i=0;i<TN;i++) {
			double u = i*pixelSize.x;
			for (int j=0;j<TN;j++) {
				double v = j*pixelSize.y;
				auto pos = c(indexObj->parameterFunction(u,v));
				if ((pos.z>=15.05 || &surfaces[index]->getObject()==this) && getPixel(ret,TN,i,j)==0u) {
					usedColours.emplace_back(index);
					floodFill(ret,TN,i,j,indexObj->parameterWrappingU(),indexObj->parameterWrappingV(), usedColours.size()-1u);
				}
			}
		}
		//removing temporary line
		if (&surfaces[index]->getObject()==this) {
			for (int i=139;i<=209;i++) {
				auto pixel = getPixel(ret,TN,301,i);
				setPixel(ret,TN,302,i,pixel);
			}
		}
		pixelMaps[index]=std::move(ret);
		return true;
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
				if(!objectArray[i].isIntersectable())
					continue;
				//TODO - change line
				surfaces.emplace_back(new bf::EquidistanceSurface(objectArray[i], ExactRadius));
				std::cout << objectArray[i].name << "\n";
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
        surfaces.emplace_back(new bf::EquidistanceSurface(*this, 0.0));
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
				bf::vec3d pos = {(150.0*(i+0.5-RESX*0.5)/RESX),(150.0*(j+0.5-RESY*0.5)/RESY),15.0};
				tmpSolid.setPosition(dc(pos));
				tmpSolid.setScale(scaleVector);
				tmpSolid.setBuffers();
				debugDummySolids.emplace_back(std::move(tmpSolid));
			}
		}
		if (!shallBeDestroyed) {
			bf::EquidistanceSurface::setMullingPathCreator(*this);
			for (auto& o: surfaces) {
				o->updateScale(ExactRadius);
			}
		}
	}


	MullingPathCreator::~MullingPathCreator() {
		for(auto& s: surfaces) {
			if (&s->getObject()!=this)
				s->getObject().indestructibilityIndex--;
		}
	}

	void MullingPathCreator::draw(const bf::ShaderArray & shaderArray) const {
		shaderArray.setColor(255u,0u,0u);
		for(auto& s: debugDummySolids)
			s.draw(shaderArray);
		if (point) {
			shaderArray.setColor(0u,255u,0u);
			shaderArray.getActiveShader().setFloat("pointSize", 8.f*configState->pointRadius);
			point->draw(shaderArray);
			shaderArray.getActiveShader().setFloat("pointSize", configState->pointRadius);
		}
		if (debugSolid && shaderArray.activeIndex == ShaderType::PointShader) {
			shaderArray.setColor(0u,255u,255u);
			if(debugSolid->indices.empty() || debugSolid->vertices.empty())
				return;
			//function assumes set projection and view matrices
			glBindVertexArray(debugSolid->VAO);
			shaderArray.getActiveShader().setMat4("model", getModelMatrix());
			shaderArray.getActiveShader().setFloat("pointSize", 4.f*configState->pointRadius);
			glDrawElements(GL_POINTS, debugSolid->indices.size(), GL_UNSIGNED_INT,   // type
						   reinterpret_cast<void*>(0)           // element array buffer offset
			);
			shaderArray.getActiveShader().setFloat("pointSize", configState->pointRadius);
		}
		for (const auto& o: surfaces) {
			if (o && &o->getObject()==this) continue;
			shaderArray.setColor(0u,127u,255u);
			o->draw(shaderArray);
		}
		shaderArray.setColor(0u,0u,0u);
	}


	void MullingPathCreator::ObjectGui() {
		//TODO - functions
		ImGui::Text("Scale: %f", scale);
		ImGui::Text("Move: %f, %f, %f", move.x, move.y, move.z);
		bf::imgui::checkChanged("Object name", name);
		if (ImGui::Button("1. Approximate path")) {
			createApproximateMullingPath();
		}
		bool wereFound = areIntersectionsFound;
		if (areIntersectionsFound)
			ImGui::BeginDisabled();
		if (ImGui::Button("Find intersections")) {
			findIntersections();
			//create pixel maps for every
			/*if (pixelMaps.empty()) {
				for(unsigned i=0;i<surfaces.size();i++) {
					createPixelMap(i);
				}
				std::cout << "Created pixel maps\n";
			}
			setDebugTextureIndex(0);*/
			//TODO - uncomment
			areIntersectionsFound = true;
		}
		if (wereFound)
			ImGui::EndDisabled();
		if (!areIntersectionsFound)
			ImGui::BeginDisabled();
		if (ImGui::Button("2. Flat-end mulling path")) {
			createFlatMullingPath();
		}
		if (ImGui::Button("3. Exact mulling path")) {
			createExactMullingPath();
		}
		if (ImGui::Button("4. Author signature")) {
		}
		if (!areIntersectionsFound)
			ImGui::EndDisabled();
		if(textureIndex<UINT_MAX) {
			static unsigned index=0;
			const auto& activeMap = pixelMaps.at(index);
			const auto& activeSurface = *surfaces[index];
			if (ImGui::Button("Change object")) {
				index=(index+1u)%(surfaces.size());
				setDebugTextureIndex(index);
			}
			ImGui::Text("Object %d", index+1);
			{
				ImGuiIO& io = ImGui::GetIO();
				ImVec2 mousePos = ImGui::GetCursorScreenPos();
				if (!point) {
					point.reset(new bf::Point());
				}
				ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(textureIndex)), ImVec2(TN, TN));
				if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
					int posX = std::clamp<int>(std::round(io.MousePos.x - mousePos.x),0,TN-1);
					int posY = std::clamp<int>(std::round(io.MousePos.y - mousePos.y),0,TN-1);

					auto color = getPixel(activeMap,TN,posX,posY);
					ImGui::Text("Colour %d, pos: (%d, %d)", color, posX, posY);
					auto pixSize = 1.0/(TN-1)*activeSurface.getParameterMax();
					bf::vec3d position = activeSurface.parameterFunction(posX*pixSize.x,posY*pixSize.y);
					point->setPosition(position);
					bf::vec3d cPosition = c(position);
					ImGui::Text("Pixel position: (%.2f, %.2f, %.2f)", cPosition.x, cPosition.y, cPosition.z);
					ImGui::EndTooltip();
				}
			}
		}
	}

	void bf::MullingPathCreator::setDebugTextureIndex(int surface) {
		std::vector<uint32_t> val(TN*TN, 0);
		if (textureIndex==UINT_MAX) {
			unsigned int texture;
			glGenTextures(1, &textureIndex);
			glBindTexture(GL_TEXTURE_2D, textureIndex);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		std::array<uint32_t, 64> colours;
		for (int i=0;i<colours.size();i++) {
			uint8_t r = 85*(i%4);
			uint8_t g = 85*((i/4)%4);
			uint8_t b = 85*(i/16);
			colours[i]=(r<<24)+(g<<16)+(b<<8)+0xff;
		}
		glBindTexture(GL_TEXTURE_2D, textureIndex);
		for (int i=0;i<TN*TN;i++) {
			auto a = pixelMaps[surface][i];
			if (a>=colours.size())
				val[i]=0xffffffff;
			else
				val[i]=colours[pixelMaps[surface][i]];
		}
		// load and generate the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TN, TN, 0, GL_RGBA, GL_UNSIGNED_BYTE, val.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void MullingPathCreator::createExactMullingPath() {
		//TODO: temporary creating paths for every colour
		for(unsigned i=0;i<usedColours.size();i++) {
			std::vector<bf::vec3d> pts;
			if (usedColours[i]!=surfaces.size()-1)
				pts = generateExactPath(usedColours[i],i,8,false, 0,0.0);
			else
				pts = createFlatBase(usedColours[i]);
			saveToFile(std::string("3_")+(i<10 ? "0" : "")+std::to_string(i)+".k08",pts);
			if (usedColours[i]!=surfaces.size()-1)
				pts = generateExactPath(usedColours[i],i,8,true, 320, 0.0);
			else
				pts = createFlatBase(usedColours[i]);
			saveToFile(std::string("Y-3_")+(i<10 ? "0" : "")+std::to_string(i)+".k08",pts);
		}
		//TODO: find flat base colours
		//auto pts = createFlatBase(colour);
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

	struct IntersectionData {
		int index1;
		int index2;
		float cx, cy, cz;
	};

	double getIntersectiondistance(const bf::IntersectionObject& object, const bf::vec4d& vec) {
		auto p1 = object.obj1->parameterFunction(vec.x,vec.y);
		auto p2 = object.obj2->parameterFunction(vec.z,vec.a);
		return glm::dot(p1-p2,p1-p2);
	}

	void fillIntersections(bf::IntersectionObject* intObj, const bf::vec4d& nextBegin, bool isFifth = false) {
		auto& pts = intObj->intersectionPoints;
		double distance = glm::length(nextBegin-pts.back());
		while (distance > 0.05) {
			std::cout << distance << " ";
			auto direction = nextBegin-pts.back();
			double t = 0.05/distance/(1+4*isFifth);
			std::cout << t << "\n";
			auto perpendicular = bf::vec4d(direction.y,-direction.x,direction.a,-direction.z)*t;
			auto centre = pts.back() + t*direction;
			bf::vec4d minVec(centre);
			double minDist = getIntersectiondistance(*intObj, minVec);
			for (int i=-40;i<=40;i++) {
				auto dx = i*0.025;
				for (int j=-40;j<=40;j++) {
					auto dy = j*0.025;
					auto point = centre + bf::vec4d(dx,dx,dy,dy)*perpendicular;
					auto candidateDist = getIntersectiondistance(*intObj, point);
					if (candidateDist<minDist) {
						minDist = candidateDist;
						minVec = point;
					}
				}
			}
			if (minDist > 0.0009) {
				std::cout << "WRONG\n";
			}
			std::cout << std::sqrt(minDist) << "\n";
			pts.emplace_back(minVec);
			distance = glm::length(nextBegin-pts.back());
		}
	}

	bf::vec4d findU(bf::IntersectionObject &inter, const bf::MullingPathCreator* tis, double centralU, double v, double refZ, bool areReversed=false) {
		double r = centralU+0.1;
		double l = centralU-0.1;
		double u = centralU;
		int sign;
		if (areReversed)
			sign = inter.obj1->parameterFunction(v,r).z > inter.obj1->parameterFunction(v,l).z ? 1.0 : -1.0;
		else
			sign = inter.obj1->parameterFunction(r,v).z > inter.obj1->parameterFunction(l,v).z ? 1.0 : -1.0;
		bf::vec3d pM;
		//bisection method
		while (std::abs(r-l)>1e-6) {
			u=(l+r)*0.5;
			pM = areReversed ? inter.obj1->parameterFunction(v,u) : inter.obj1->parameterFunction(u,v);
			if (std::abs(pM.z-refZ) < 1e-4) {
				break;
			}
			if (sign*(pM.z-refZ) > 0.0) {
				r=u;
			}
			else {
				l=u;
			}
		}
		auto pos = areReversed ? inter.obj1->parameterFunction(v, u) : inter.obj1->parameterFunction(u, v);
		auto p=tis->toSurfaceSpace({pos.x, pos.y});
		if (areReversed)
			return {v,u,p.x,p.y};
		else
			return {u,v,p.x,p.y};
	}

	void bf::MullingPathCreator::finishToEdge(bf::IntersectionObject &inter, bool is4) {
		const double refZ = (15.0 - move.z) / scale;
		if (is4) {
			auto vec = inter.intersectionPoints.back();
			//std::cout << vec <<  "\t" << inter.obj1->parameterFunction(vec.x, vec.y) << "\n";
			auto pt = findU(inter, this, vec.x - 0.99, vec.y, refZ);
			inter.intersectionPoints.emplace_back(pt);
			//std::cout << pt << "\t" << inter.obj1->parameterFunction(pt.x, pt.y) << "\n";
			for (double i=pt.y; i<1.0; i+=0.01) {
				auto newPoint = findU(inter, this, pt.x, i,  refZ);
				//std::cout << newPoint << "\t" << inter.obj1->parameterFunction(newPoint.x, newPoint.y) << "\n";
				inter.intersectionPoints.emplace_back(newPoint);
			}
			return;
		}
		auto &pts = inter.intersectionPoints;
		if (inter.obj1->parameterWrappingU()) {
			double maxY = inter.obj1->getParameterMax().y;
			//my case
			bool isRising = pts.back().y > pts[0].y;
			{
				std::vector<bf::vec4d> points;
				auto pEnd = pts[0];
				double M = !isRising ? inter.obj1->getParameterMax().y : 0.0;
				double Vdist = std::abs(M - pEnd.y) / 0.01;
				for (int i = 0; i <= std::ceil(Vdist); i++) {
					double v = lerp(pEnd.y, M, std::min(i / Vdist, 1.0 - 1e-6));
					auto centralU = points.empty() ? pts[0].x : points.back().x;
					auto vec = findU(inter, this, centralU, v, refZ);
					std::cout << v << " " << vec << "\n";
					points.emplace_back(vec);
				}
				std::reverse(points.begin(), points.end());
				points.insert(points.end(), inter.intersectionPoints.begin(), inter.intersectionPoints.end());
				pEnd = points.back();
				M = isRising ? inter.obj1->getParameterMax().y : 0.0;
				Vdist = std::abs(M - pEnd.y) / 0.01;
				for (int i = 0; i <= std::ceil(Vdist); i++) {
					double v = lerp(pEnd.y, M, std::min(i / Vdist, 1.0 - 1e-6));
					auto centralU = points.back().x;
					auto vec = findU(inter, this, centralU, v, refZ);
					points.emplace_back(vec);
				}
				inter.intersectionPoints = std::move(points);
			}
		} else {
			//no examples in my model
		}
	}

	void addPointToC0Equidistant(bf::IntersectionObject *intObj, const bf::vec4d& myPoint) {
		auto& pts = intObj->intersectionPoints;
		double distance = glm::length(myPoint-pts.back());
		while (distance > 0.02) {
			std::cout << distance << " ";
			auto direction = myPoint-pts.back();
			double t = 0.01/distance;
			std::cout << t << "\n";
			auto perpendicular = bf::vec4d(direction.y,-direction.x,direction.a,-direction.z)*t;
			auto centre = pts.back() + t*direction;
			bf::vec4d minVec(centre);
			double minDist = getIntersectiondistance(*intObj, minVec);
			for (int i=-40;i<=40;i++) {
				auto dx = i*0.025;
				for (int j=-40;j<=40;j++) {
					auto dy = j*0.025;
					auto point = centre + bf::vec4d(dx,dx,dy,dy)*perpendicular;
					auto candidateDist = getIntersectiondistance(*intObj, point);
					if (candidateDist<minDist) {
						minDist = candidateDist;
						minVec = point;
					}
				}
			}
			if (minDist > 0.0009) {
				std::cout << "WRONG\n";
			}
			std::cout << std::sqrt(minDist) << "\n";
			pts.emplace_back(minVec);
			distance = glm::length(myPoint-pts.back());
		}
	}

	void MullingPathCreator::findIntersections() {
		constexpr std::array intersectionData = {
			IntersectionData(0, 1,-0.326, 9.022, -1.167),
			IntersectionData(0, 1, 0.254,11.692, -1.511),
			IntersectionData(0, 1,-0.292,-2.483, -0.323),
			IntersectionData(1, 2, 1.195, 2.431,  0.140),
			IntersectionData(1, 2, 3.630, 2.579,  0.507),
			IntersectionData(0, 2,-1.970, 2.097, -0.334),
			IntersectionData(0, 3, 0.000, 0.000, 0.000), //satisfactionary even though not looped
			IntersectionData(1, 3, 1.543, 1.531,  0.190),
			IntersectionData(3, 4,-0.882, 2.087,  0.740)
			};
		constexpr std::array specialintersectionData1 = { //WRONG - TODO: TO IMPROVE
			IntersectionData(0, 1, 1.433, -4.001, -0.500),
			IntersectionData(0, 1, 1.260,-3.958,  -1.000),
			IntersectionData(0, 1, 0.355,-3.876,  -1.000),
			IntersectionData(0, 1, 0.500,-4.300,  0.000),
			IntersectionData(0, 1, 1.020, -4.210, 1.000)
			};
		constexpr std::array flatIntersectionData = {
			IntersectionData(0, 5, -4.2, 0.0, 0),
			IntersectionData(0, 5,0.0, 0.0, 0),
			IntersectionData(1, 5, 4.0, 0.0, 0),
			IntersectionData(1, 5, 0.0, 0.0, 0),
			IntersectionData(2, 5,0.0, 2.0, 0),
			IntersectionData(2, 5, 0.0, 4.0, 0),
			IntersectionData(3, 5, 4.0, 0.0, 0),
			IntersectionData(3, 5, 0.0, 0.0, 0),
			IntersectionData(4, 5, 0.5, 1.0, 0),
			IntersectionData(4, 5,-0.5, 1.0, 0) //TODO: fill the break - WRONG
			};
		int k=1;
		constexpr int N = intersectionData.size()+flatIntersectionData.size()+specialintersectionData1.size();
		{
			bf::IntersectionObject* interObj = nullptr;
			int kk=0;
			for(auto&& [i1, i2, cx, cy, cz]: specialintersectionData1) {
				std::cout << "INDEX " << kk << "\n";
				auto* intObj = new bf::IntersectionObject(objectArray, *surfaces[i1], *surfaces[i2], {cx, cy, cz},0.01, true);
				kk++;
				if (kk>5) {
					intersections.push_back(intObj);
					objectArray.add(intObj);
					continue;
				}
				auto& newPts = intObj->intersectionPoints;
				if (interObj==nullptr)
					interObj = intObj;
				else {
					fillIntersections(interObj, newPts[0]);
					interObj->intersectionPoints.insert(interObj->intersectionPoints.end(), newPts.begin(), newPts.end());
					delete intObj;
				}
				std::cout << interObj->intersectionPoints.size() << "\n";
			}
			//TODO - temporary
			fillIntersections(interObj, interObj->intersectionPoints[0]);
			interObj->recalculate();
			interObj->setBuffers();
			intersections.push_back(interObj);
			objectArray.add(interObj);
		}

		//TODO - temporary
		bool isSecond=false;
		for(auto&& [i1, i2, cx, cy, cz]: flatIntersectionData) {
			auto* intObj = new bf::IntersectionObject(objectArray, *surfaces[i1], *this, {cx, cy, cz});
			if (i1 != 3) {
				finishToEdge(*intObj, i1==4);
				intObj->recalculate();
			}
			//intObj->isShown = false;
			if(isSecond && i1!=3 && (!surfaces[i1]->parameterWrappingU() || !surfaces[i1]->parameterWrappingV())) {
				std::cout << "Removing " << k << "\n";
				flatIntersections.back()->mergeFlatIntersections(*intObj);
				delete intObj;
			}
			else {
				flatIntersections.push_back(intObj);
				objectArray.add(intObj);
			}
			isSecond=!isSecond;
			std::cout << k++ << "/" << N << "\n\n";
		}

		for(auto&& [i1, i2, cx, cy, cz]: intersectionData) {
			//set true for needed examples
			auto* intObj = new bf::IntersectionObject(objectArray, *surfaces[i1], *surfaces[i2], {cx, cy, cz},0.01, true);
			auto& pts = intObj->intersectionPoints;
			if (intObj && !intObj->isLooped && intObj->intersectionPoints.size()>=3) { //finishing the loop
				//TODO - to remove
				if (i2==4) {
					auto& pts = intObj->intersectionPoints;
					const auto torusCentre = surfaces[3]->getObject().getPosition();
					const auto radius = (surfaces[3]->torus->bigRadius-surfaces[3]->torus->smallRadius-ExactRadius/scale);
					bf::vec4d myPoint1={INFINITY,INFINITY,INFINITY,INFINITY};
					const auto& pointList = flatIntersections.back()->intersectionPoints;
					double dist=1e10;
					for (int i=0; i<pointList.size()/2; i++) {
						bf::vec2d position = flatIntersections.back()->vertices[i].getPosition()-torusCentre;
						auto norm = std::abs(glm::dot(position, position)-radius*radius);
						if (myPoint1.x==INFINITY || norm<dist) {
							dist = norm;
							myPoint1.x = PI;
							myPoint1.y = std::atan2(position.y,position.x);
							myPoint1.z = pointList[i].x;
							myPoint1.a = pointList[i].y;
						}
					}
					dist=1e10;
					bf::vec4d myPoint2={INFINITY,INFINITY,INFINITY,INFINITY};
					for (int i=pointList.size()/2; i<pointList.size(); i++) {
						bf::vec2d position = flatIntersections.back()->vertices[i].getPosition()-torusCentre;
						auto norm = std::abs(glm::dot(position, position)-radius*radius);
						if (myPoint2.x==INFINITY || norm<dist) {
							dist = norm;
							myPoint2.x = PI;
							myPoint2.y = std::atan2(position.y,position.x);
							myPoint2.z = pointList[i].x;
							myPoint2.a = pointList[i].y;
						}
					}
					addPointToC0Equidistant(intObj, myPoint1);
					std::reverse(pts.begin(),pts.end());
					addPointToC0Equidistant(intObj, myPoint2);
				}
				else if (i1 != 0 || i2 != 3) { //except [0,3]
					//TODO - repair
					fillIntersections(intObj, pts[0]);
					intObj->isLooped=true;
				}
				std::cout << "Not looped\n";
				intObj->recalculate();
				intObj->setBuffers();
			}
			//intObj->isShown = false;
			intersections.push_back(intObj);
			objectArray.add(intObj);
			std::cout << k++ << "/" << N << "\n";
		}
	}

	std::vector<bf::vec3d> bf::MullingPathCreator::generateExactPath(unsigned objIndex, uint8_t color, int diff, bool isXMove, int move, double normPerc) const {
		std::vector<bf::vec3d> points;
		bool isDown = false;
		const auto& surface = *surfaces[objIndex];
		const auto& pixelMap = pixelMaps.at(objIndex);
		bf::vec3d prevPos;
		bf::vec3d pos;
		for (int ti=0;ti<TN;ti+=diff) {
			bool isIncluded=false;
			bool isNow=false;
			for (int tj=0;tj<TN;tj++) {
				int i=ti;
				int j=(tj+move)%TN;
				j = ((ti/diff)%2==0) ? j : TN-j;
				if (isXMove) {
					std::swap(i,j);
				}
				double id = i*surfaces[objIndex]->getParameterMax().x/(TN-1);
				double jd = j*surfaces[objIndex]->getParameterMax().y/(TN-1);
				if (getPixel(pixelMap,TN, i,j)==color) {
					pos = c(surface.parameterFunction(id,jd));
					if (normPerc>0.001) {
						auto n = glm::normalize(surface.getNormal(id,jd));
						pos -= (normPerc*ExactRadius)*n;
					}
					if (isIncluded && !isNow) {
						points.emplace_back(prevPos.x,  prevPos.y, 50.0);
						points.emplace_back(pos.x, pos.y, 50.0);
					}
					points.emplace_back(pos);
					isIncluded=true; isNow=true;
				}
				else if (isIncluded && isNow) {
					isNow=false;
					prevPos = pos;
				}
			}
			isDown = !isDown;
		}
		return points;
	}

	std::vector<bf::vec3d> MullingPathCreator::createFlatBase(uint8_t color) const {
		int index = surfaces.size()-1;
		auto ret1 = generateExactPath(index, color, 1, false, 0.0);
		//TODO - move between two phases
		auto ret2 = generateExactPath(index, color, 1, true, 0.0);
		ret1.insert(ret1.end(),ret2.begin(), ret2.end());
		return ret1;
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
		for (const auto& o: surfaces) {
			if (&o->getObject()==this) continue;
			for (double u = 0.0; u < o->getParameterMax().x - 0.005; u += step) {
				for (double v = 0.0; v < o->getParameterMax().y - 0.005; v += step) {
					tPoints.emplace_back(c(o->parameterFunction(u, v)));
				}
			}
		}
		std::cout << tPoints.size() << "\n";
		//create heightmap
		std::array<std::array<double, RESY>, RESX> heightMap = {0.0};
		for (const auto &p: tPoints) {
			const int pX = static_cast<int>((p.x + 75.0) * RESX / 150.0);
			const int pY = static_cast<int>((p.y + 75.0) * RESY / 150.0);
			heightMap[pX][pY] = std::max(heightMap[pX][pY], p.z);
		}
		for (int i = 0; i < RESX; i++) {
			for (int j = 0; j < RESY; j++) {
				auto &o = debugDummySolids[i * RESY + j];
				glm::vec3 pos = o.getPosition();
				pos.z = static_cast<float>((std::max(heightMap[i][j], 15.0) - move.z) / scale);
				o.setPosition(pos);
			}
		}
		std::cout << "Heightmap created\n";
		std::vector<bf::vec3d> points;
		points.emplace_back(0.0, 0.0, 60.0);
		bf::vec3d actualPosition = {150.0 * (0.5 - RESX * 0.5) / RESX, 76.0 + Radius, 60.0};
		points.push_back(actualPosition);
		moveApproximate(34.0, points, actualPosition, heightMap);
		moveApproximate(16.0, points, actualPosition, heightMap);
		//set path for
		actualPosition.z = 60.0;
		points.push_back(actualPosition);
		actualPosition = {0.0, 0.0, 60.0};
		points.push_back(actualPosition);
		if (saveToFile("1.k" + std::to_string(2 * static_cast<int>(Radius)), points))
			std::cout << "Approximate paths created\n";
		else
			std::cerr << "Error when trying to save approximate paths\n";
	}

	bf::vec3d MullingPathCreator::getPositionMovedByNormal(bf::IntersectionObject* obj, const glm::vec4& part) const {
		double u = part.x;
		double v = part.y;
		const auto* o = obj->obj1;
		auto n = glm::normalize(bf::vec2d(o->getNormal(u,v)));
		bf::vec3d normal = bf::vec3d(n, 0.0)*(flatRadius-ExactRadius);
		return c(o->parameterFunction(u,v))-normal;
	}
	struct IntersectionPathIntersection {
		static const std::vector<std::vector<bf::vec3d> >* paths;
		unsigned obj1, obj2;
		unsigned i1, i2;
		double t1, t2;
		[[nodiscard]] bf::vec3d getPosition() const {
			if(!paths) return {};
			const auto& o1 = (*paths)[obj1];
			return lerp(o1[i1],o1[(i1+1)%o1.size()], t1);
		}
	};
	const std::vector<std::vector<bf::vec3d> >* IntersectionPathIntersection::paths = nullptr;

	std::vector<IntersectionPathIntersection> getPathIntersections(const std::vector<std::vector<bf::vec3d> >& paths) {
		IntersectionPathIntersection::paths = &paths;
		std::vector<IntersectionPathIntersection> ret;
		for(unsigned i1=1;i1<paths.size();i1++) {
			for(unsigned i2=0;i2<i1;i2++) {
				const auto& p1 = paths[i1];
				const auto& p2 = paths[i2];
				std::vector<std::pair<IntersectionPathIntersection, double> > tmpIntersections;
				for(unsigned i=0;i<p1.size();i++) {
					for(unsigned j=0;j<p2.size();j++) {
            			if(glm::dot(p1[i]-p2[j],p1[i]-p2[j])>0.01) continue; //fast removing not probable
						auto inter = segmentIntersection(
							p1[i],p1[(i+1)%p1.size()],
							p2[j],p2[(j+1)%p2.size()]);
						if(inter) {
							auto intersection = IntersectionPathIntersection(i1, i2, i, j, inter->u, inter->v);
							bool wasFoundSimilar = false;
							for (auto& inte: tmpIntersections) {
								const auto& v = inte.first;
								if (fastPow(i-v.i1, 2)+fastPow(j-v.i2, 2) < 100) {
									wasFoundSimilar = true;
									if (inter->error < inte.second) {
										inte.first = intersection;
										inte.second = inter->error;
									}
									break;
								}
							}
							if (!wasFoundSimilar) {
								tmpIntersections.emplace_back(intersection, inter->error);
							}
						}
					}
				}
				for (auto &ina: tmpIntersections | std::views::keys) {
					ret.emplace_back(std::move(ina));
				}
			}
		}
		return ret;
	}

	void MullingPathCreator::createFlatMullingPath() {
		if (pixelMaps.empty()) {
			for(unsigned i=0;i<surfaces.size();i++) {
				createPixelMap(i);
			}
			std::cout << "Created pixel maps\n";
		}
		//getting central path points
		std::vector<std::vector<bf::vec3d> > paths;
		for(auto io: flatIntersections) {
			std::vector<bf::vec3d> pts;
			for(auto& p: io->intersectionPoints)
				pts.emplace_back(getPositionMovedByNormal(io, p));
			paths.emplace_back(std::move(pts));
		}
		//creating central path
		auto pathIntersections = getPathIntersections(paths);
		debugSolid = std::make_unique<bf::DummySolid>("");
		std::cout << "Intersections of flat intersections found\n";
		std::map<unsigned, std::vector<IntersectionPathIntersection> > intersectionMap;
		for(auto& p: pathIntersections) {

			if(intersectionMap.contains(p.obj1))
				intersectionMap[p.obj1].push_back(p);
			else
				intersectionMap[p.obj1] = {p};
			if(intersectionMap.contains(p.obj2))
				intersectionMap[p.obj2].push_back(p);
			else
				intersectionMap[p.obj2] = {p};
			debugSolid->vertices.emplace_back(dc(p.getPosition()));
			debugSolid->indices.emplace_back(debugSolid->indices.size()-1);
		}
		debugSolid->setBuffers();
		std::vector<bf::vec3d> centralPoints;
		unsigned objIndex=0, i=intersectionMap[0].size()/2;
		bool isGrowing = true;
		do {
			std::vector<IntersectionPathIntersection> *inter=nullptr;
			if(intersectionMap.contains(objIndex)) {
				inter = &intersectionMap[objIndex];
				for(auto& j: *inter) {
					if(j.obj1 == objIndex && j.i1 == i) {
						centralPoints.emplace_back(j.getPosition());
						centralPoints.back().z = 15.0;
						objIndex = j.obj2;
						i = (j.i2 + 1)%paths[objIndex].size();
					}
					if(j.obj2 == objIndex && j.i2 == i) {
						centralPoints.emplace_back(j.getPosition());
						centralPoints.back().z = 15.0;
						objIndex = j.obj1;
						i = (j.i1 + 1)%paths[objIndex].size();
					}
				}
			}
			//TODO: check if go clockwise or counterclockwise
			centralPoints.emplace_back(paths[objIndex][i]);
			centralPoints.back().z = 15.0;
			if(isGrowing)
				i = (i + 1)%paths[objIndex].size();
			else
				i = i>0 ? i-1 : paths[objIndex].size()-1;
		} while(i!=0 || objIndex !=0);
		//create flat extensions
		std::vector<bf::vec3d> points;
		points.emplace_back(0.0,0.0,60.0);
		points.emplace_back(81.0,81.0,60.0);
		constexpr int DIVISIONS = 16; //16 parts: 9.375mm distance
		constexpr int divMove = std::ceil(1.0/150.0*flatRadius*TN);
		const auto& thisMap = pixelMaps.at(surfaces.size()-1);
		uint8_t colour = getPixel(thisMap, TN, 0, 0);
		//TODO - make it working
		points.emplace_back(81.0,81.0,15.0);
		for(int ii=0;ii<4;ii++) {
			bool isX = ii%2;
			if (!isX) continue;
			bool isPositive = ii<2;
			bool isPositiveOther = ii!=0 && ii!=3;
			double axis1 = isPositiveOther ? -81.0 : 81.0;
			bf::vec3d pos = {isX ? axis1 : -81.0, isX ? -81.0 : axis1, 15.0};
			for(i=0;i<=DIVISIONS;i++) {
				const unsigned index = (!isPositiveOther ? i : DIVISIONS-i);
				const double actualAxisPos = -75.0+(index*150.0/DIVISIONS);
				const int actualIntPos = (actualAxisPos/150.0+0.5)*TN;
				bf::vec3d min={-81.0,-81.0,15.0}, max={81.0,81.0,15.0};
				int minJ, maxJ=TN-1;
				for (minJ=0;minJ<TN/2+divMove;minJ++) {
					bool shouldBeBroken = false;
					for (int k=std::max(0,actualIntPos-divMove);k<=std::min(TN-1,actualIntPos+divMove);k++) {
						int x = isX ? minJ : k;
						int y = isX ? k : minJ;
						if (getPixel(thisMap,TN,x,y)!=colour) {
							shouldBeBroken = true;
							break;
						}
					}
					if (shouldBeBroken)
						break;
				}
				for (maxJ=TN-1;maxJ>=TN/2-divMove;maxJ--) {
					bool shouldBeBroken = false;
					for (int k=std::max(0,actualIntPos-divMove);k<=std::min(TN-1,actualIntPos+divMove);k++) {
						int x = isX ? maxJ : k;
						int y = isX ? k : maxJ;
						if (getPixel(thisMap,TN,x,y)!=colour) {
							shouldBeBroken = true;
							break;
						}
					}
					if (shouldBeBroken)
						break;
				}
				min.x = -75.0 + (minJ-divMove)*150.0/(TN-1);
				min.y = min.x;
				max.x = -75.0 + (maxJ+divMove)*150.0/(TN-1);
				max.y = max.x;
				//TODO - finding max,min values
				double res2;
				if(isX)
					res2 = isPositive ? std::max(max.x,0.0) : std::min(min.x,0.0);
				else
					res2 = isPositive ? std::max(max.y,0.0) : std::min(min.y,0.0);
				int sgn = isPositive ? -1 : 1;
				if(i%2==0) {
					points.emplace_back(-81.0*sgn, actualAxisPos, 15.0);
					if(!isX) std::swap(points.back().x,points.back().y);
				}
				points.emplace_back(res2, actualAxisPos, 15.0);
				if (ii==3 && i==9) {
					auto myPos = points.back();
					points.back().x-=10.0;
					points.emplace_back(myPos);
				}
				if(!isX) std::swap(points.back().x,points.back().y);
				if(i%2==1) {
					points.emplace_back(-81.0*sgn, actualAxisPos, 15.0);
					if(!isX) std::swap(points.back().x,points.back().y);
				}
				if (ii==3 && i==6) {
					points.back().x-=10.0;
				}
			}
		}
		//TODO - check path to move to phase 2
		points.reserve(points.size()+centralPoints.size()+3);
		for(auto& p: centralPoints)
			points.emplace_back(p);
		points.emplace_back(points.back().x,points.back().y,60.0);
		points.emplace_back(0.0,0.0,60.0);
		//save to file
		if (saveToFile("2.f" + std::to_string(2 * static_cast<int>(flatRadius)), points))
			std::cout << "Flat paths created\n";
		else
			std::cerr << "Error when trying to save flat paths\n";
		IntersectionPathIntersection::paths = nullptr;
	}


	bf::vec3d bf::MullingPathCreator::parameterFunction(double u, double v) const {
		return dc({150.0*(u-0.5),150.0*(v-0.5),15.0});
	}
	bf::vec3d bf::MullingPathCreator::parameterGradientU(double, double) const {
		return {150.0/scale, 0.0, 0.0};
	}
	bf::vec3d bf::MullingPathCreator::parameterGradientV(double, double) const {
		return {0.0, 150.0/scale,  0.0};
	}
	bf::vec3d bf::MullingPathCreator::parameterHesseUU(double, double) const {
		return {0.0, 0.0,  0.0};
	}
	bf::vec3d bf::MullingPathCreator::parameterHesseUV(double, double) const {
		return {0.0, 0.0,  0.0};
	}
	bf::vec3d bf::MullingPathCreator::parameterHesseVV(double, double) const {
		return {0.0, 0.0,  0.0};
	}
} // bf
