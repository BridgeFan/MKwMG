//
// Created by kamil-hp on 22.04.23.
//
#include <iostream>
#include <memory>
#define VALIJSON
#include "Camera.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "FileLoading.h"
#include "JsonUtil.h"
#include "Object/Object.h"
#include "Object/ObjectArray.h"
#include "Shader/ShaderArray.h"
#include "Solids/Torus.h"
#include "Surfaces/BezierSurface0.h"
#include "Surfaces/BezierSurface2.h"
#include "Util.h"
#include "library_include.h"
#include "src/Gizmos/Cursor.h"
#include "src/Object/Point.h"
#include <format>
#include "Schema.h"

valijson::Schema modelSchema;
bool wasValidatorLoaded=false;

void loadValidator() {
// Load JSON document using JsonCpp with Valijson helper function
	Json::Value schemaValue;
	std::istringstream schemaStream(schemaStr);
	try {
		schemaStream >> schemaValue;
	}
	catch(std::exception &e) {
		std::cerr << std::format("Failed to load schema document\n{}\n", e.what());
	}
	// Parse JSON schema content using valijson
	valijson::SchemaParser parser;
	valijson::adapters::JsonCppAdapter modelSchemaAdapter(schemaValue);
	parser.populateSchema(modelSchemaAdapter, modelSchema);
	wasValidatorLoaded=true;
}

template<std::derived_from<bf::BezierCommon> T>
std::pair<unsigned, T*> loadBezier(Json::Value& bezierValue, bf::ObjectArray& oa, const std::string& ptName) {
	unsigned id = bezierValue["id"].asUInt();
	T* bezier;
	if(bezierValue.isMember("name"))
		bezier = new T(oa, bezierValue["name"].asString());
	else
		bezier = new T(oa);
	auto& cPts = bezierValue[ptName];
	for(unsigned i=0u;i<cPts.size();i++) {
		bezier->addPoint(cPts[i]["id"].asUInt());
	}
	return {id, bezier};
}

struct Segment {
	glm::vec<2, int> samples;
	std::string name;
	bf::pArray pointIndices;
};

std::vector<std::vector<Segment> > recognizeSegments(std::vector<Segment>&& segments,
		const glm::vec<2,int>& size, bool wrappedX, bool wrappedY) {
	std::map<unsigned, std::array<unsigned, 4> > pointCount;
	//count points
	for(unsigned i=0;i<segments.size();i++) {
		const auto& s = segments[i];
		constexpr std::array indices = {0,3,12,15};
		for(unsigned k=0;k<4;k++) {
			unsigned j = indices[k];
			if(!pointCount.contains(s.pointIndices[j]))
				pointCount[s.pointIndices[j]] = {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};
			pointCount[s.pointIndices[j]][k] = i;
		}
	}
	unsigned leftBottomIndex=0u;
	for(auto&& [key, values]: pointCount) {
		if(values[0]==UINT_MAX)
			continue;
		if(values[1]==values[2] && values[2]==values[3] && values[3]==UINT_MAX) {
			leftBottomIndex=values[0];
			break;
		}
		if(wrappedX || wrappedY) {
			int cnt = 0;
			cnt = values[1]==UINT_MAX ? cnt+1 : cnt;
			cnt = values[2]==UINT_MAX ? cnt+1 : cnt;
			cnt = values[3]==UINT_MAX ? cnt+1 : cnt;
			if(cnt==1) {
				leftBottomIndex=values[0];
				break;
			}
		}
	}
	std::vector<std::vector<Segment> > ret;
	for(int i=0;i<size.y;i++) {
		std::vector<Segment> retRow;
		if(i==0) {
			retRow.emplace_back(std::move(segments[leftBottomIndex]));
		}
		else {
			unsigned index = pointCount[ret.back()[0].pointIndices[12]][0];
			retRow.emplace_back(std::move(segments[index]));
		}
		for(int j=1;j<size.x;j++) {
			unsigned index = pointCount[retRow.back().pointIndices[3]][0];
			retRow.emplace_back(std::move(segments[index]));
		}
		ret.emplace_back(std::move(retRow));
	}
	return ret;
}

template<std::derived_from<bf::BezierSurfaceCommon> T>
std::pair<unsigned, T*> loadSurface(Json::Value& bezierValue, bf::ObjectArray& oa) {
	unsigned id = bezierValue["id"].asUInt();
	T* surface;
	static bf::Cursor cursor;
	if(bezierValue.isMember("name"))
		surface = new T(oa, bezierValue["name"].asString(), cursor);
	else
		surface = new T(oa, cursor);
	auto& cPts = bezierValue["patches"];
	surface->segs = bf::loadVec2i(bezierValue["size"]);
	surface->isWrappedX = bezierValue["parameterWrapped"]["u"].asBool();
	surface->isWrappedY = bezierValue["parameterWrapped"]["v"].asBool();
	std::vector<Segment> segments;
	if(static_cast<int>(cPts.size())!=surface->segs.x*surface->segs.y) {
		std::cerr << std::format("Bezier surface with index {} has incorrect size!\n", id);
		return {id, nullptr};
	}
	//PHASE 1: prepare points
	for(auto& patch : cPts) {
		Segment segment;
		segment.samples=bf::loadVec2i(patch["samples"]);
		if(bezierValue.isMember("name"))
			segment.name=patch["name"].asString();
		segment.samples=bf::loadVec2i(patch["samples"]);
		auto& sPts = patch["controlPoints"];
		for(unsigned j=0u;j<sPts.size();j++) {
			segment.pointIndices[j]=sPts[j]["id"].asUInt();
			oa[sPts[j]["id"].asUInt()].indestructibilityIndex=1u;
		}
		segments.emplace_back(std::move(segment));
	}
	//PHASE 2: recognize segments
	std::vector<std::vector<Segment> > newSegments;
	if(typeid(T)==typeid(bf::BezierSurface0))
		newSegments = recognizeSegments(std::move(segments), surface->segs,
		surface->isWrappedX, surface->isWrappedY);
	else {
		int i=0;
		for(auto&& s: segments) {
			if(i%surface->segs.x==0)
				newSegments.emplace_back();
			newSegments.back().emplace_back(std::move(s));
		}
	}
	std::vector<std::vector<std::string> > segmentNames(newSegments.size());
	std::vector<std::vector<glm::vec<2,int> > > segmentSamples(newSegments.size());
	std::vector<std::vector<bf::pArray> > pointIndices(newSegments.size());
	pointIndices.resize(newSegments.size());
	for(unsigned i=0;i<newSegments.size();i++) {
		segmentNames[i].resize(newSegments[i].size());
		segmentSamples[i].resize(newSegments[i].size());
		pointIndices[i].resize(newSegments[i].size());
		for(unsigned j=0;j<newSegments[i].size();j++) {
			pointIndices[i][j] = std::move(newSegments[i][j].pointIndices);
			segmentNames[i][j] = std::move(newSegments[i][j].name);
			segmentSamples[i][j] = std::move(newSegments[i][j].samples);
		}
	}
	surface->initSegments(std::move(segmentNames),std::move(segmentSamples), std::move(pointIndices));
	oa.isForcedActive=false;
	return {id, surface};
}

template<std::derived_from<bf::Object> T>
void emplaceToObjectArray(std::vector<std::pair<std::unique_ptr<bf::Object>,bool> >& objects, std::pair<unsigned, T*> o) {
	for(unsigned j=objects.size();j<=o.first;j++) {
		objects.emplace_back(nullptr, false);
	}
	objects[o.first].first.reset(o.second);
}

bool bf::loadFromFile(bf::ObjectArray &objectArray, bf::Camera& camera, const std::string &path) {
    std::cout << "\nChosen file " << path << "\n";
    std::ifstream file(path);
    if(!file.good()) {
        std::cerr << "File not found!\n";
        return false;
    }
    bool ret = loadFromStream(objectArray, camera,file);
    return ret;
}

bool bf::loadFromStream(bf::ObjectArray &objectArray, bf::Camera& camera, std::istream& in) {
    std::cout << "Began loading file\n";
    if(!wasValidatorLoaded) {
        loadValidator();
        if(!wasValidatorLoaded) {
            std::cout << "Validator not loaded correctly\n";
            return false;
        }
    }
    Json::Value value;
    try {
        in >> value;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::cerr << "File not found!\n";
        return false;
    }
    std::cout << "File loaded successfully\n";
	objectArray.clearSelection();
    valijson::Validator validator;
    valijson::adapters::JsonCppAdapter myTargetAdapter(value);
    valijson::ValidationResults errors;
    if (!validator.validate(modelSchema, myTargetAdapter, &errors)) {
        std::cerr << std::format("File is not valid model file! Found {} errors.\n", errors.numErrors());
        while(errors.numErrors()>0) {
            valijson::ValidationResults::Error error;
            errors.popError(error);
            for(const auto& a: error.context) {
                std::cout << std::format("{}/",a);
            }
            std::cout << std::format("\nDESC: {}\n",error.description);
        }
        return false;
    }
    std::cout << "File validated successfully\n";
    objectArray.removeAll();
    Json::Value& pointValue = value["points"];
    for(auto & pValue : pointValue) {
        emplaceToObjectArray(objectArray.objects,loadPoint(pValue));
    }
    Json::Value& geometry = value["geometry"];
    for(auto& gValue: geometry) {
        if(gValue["objectType"]=="torus") {
            emplaceToObjectArray(objectArray.objects,loadTorus(gValue));
        }
        else if(gValue["objectType"]=="bezierC0") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurve0>(gValue, objectArray, "controlPoints"));
        }
        else if(gValue["objectType"]=="bezierC2") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurve2>(gValue, objectArray, "deBoorPoints"));
        }
        else if(gValue["objectType"]=="interpolatedC2") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurveInter>(gValue, objectArray, "controlPoints"));
        }
        else if(gValue["objectType"]=="bezierSurfaceC0") {
            emplaceToObjectArray(objectArray.objects,loadSurface<BezierSurface0>(gValue, objectArray));
        }
        else if(gValue["objectType"]=="bezierSurfaceC2") {
            emplaceToObjectArray(objectArray.objects,loadSurface<BezierSurface2>(gValue, objectArray));
        }
        else {
            std::cout << std::format("Unsupported type {}\n", gValue["objectType"].asString());
        }
    }
    glm::vec3 oldPos=camera.position, oldRot=camera.rotation;
    bool isWrong=false;
    {
        Json::Value &cameraValue = value["camera"];
        if(cameraValue.isNull())
            isWrong=true;
        camera.position = loadVec3(cameraValue["focusPoint"]);
        float dist = cameraValue["distance"].asFloat();
        float rotX = cameraValue["rotation"]["x"].asFloat();
        float rotY = cameraValue["rotation"]["y"].asFloat();
        glm::mat4 matrix = bf::getRotateXMatrix(rotX)*bf::getRotateYMatrix(rotY);
        camera.rotation = decomposeRotationMatrix(matrix);
        camera.GetViewMatrix();
        camera.position += camera.getFront() * dist;
    }
    if(isWrong) {
        camera.position=oldPos;
        camera.rotation=oldRot;
        camera.GetViewMatrix();
    }
    //post init
    for(const auto& o: objectArray.objects) {
        if(o.first)
            o.first->postInit();
    }
    //clear empty pointers
    for(int i=0;i<static_cast<int>(objectArray.size());i++) {
        if(objectArray.getPtr(i)==nullptr) {
            objectArray.remove(i);
            i--;
        }
    }
    std::cout << "File loading finished\n";
    return true;
}

bool bf::saveToFile(const bf::ObjectArray &objectArray, const bf::Camera& camera, const std::string &path) {
    std::cout << "\nChosen file " << path << "\n";
    std::ofstream file(path);
    if(!file.good()) {
        std::cerr << "File not found!\n";
        return false;
    }
    bool ret = saveToStream(objectArray, camera, file);
    return ret;
}

bool bf::saveToStream(const bf::ObjectArray &objectArray, const bf::Camera& camera, std::ostream &out) {
	Json::Value pValue(Json::arrayValue);
	Json::Value gValue(Json::arrayValue);
	unsigned idTmp = objectArray.size();
	for(unsigned i=0;i<objectArray.size();i++) {
		const auto& o = objectArray[i];
		if(typeid(*&o)==typeid(bf::Point)) {
			const auto p = dynamic_cast<const bf::Point*>(&o);
			pValue.append(bf::saveValue(*p, i));
		}
		else if(typeid(*&o)==typeid(bf::Torus)) {
			const auto t = dynamic_cast<const bf::Torus*>(&o);
			gValue.append(bf::saveValue(*t, i));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurve0)) {
			const auto t = dynamic_cast<const bf::BezierCommon*>(&o);
			gValue.append(bf::saveValue(*t, i, "bezierC0", "controlPoints"));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurve2)) {
			const auto t = dynamic_cast<const bf::BezierCommon*>(&o);
			gValue.append(bf::saveValue(*t, i, "bezierC2", "deBoorPoints"));
		}
		else if(typeid(*&o)==typeid(bf::BezierCurveInter)) {
			const auto t = dynamic_cast<const bf::BezierCommon*>(&o);
			gValue.append(bf::saveValue(*t, i, "interpolatedC2", "controlPoints"));
		}
		else if(typeid(*&o)==typeid(bf::BezierSurface0)) {
			const auto s = dynamic_cast<const bf::BezierSurface0*>(&o);
			gValue.append(bf::saveValue(*s, i, "bezierSurfaceC0", "bezierPatchC0", idTmp));
		}
		else if(typeid(*&o)==typeid(bf::BezierSurface2)) {
			const auto s = dynamic_cast<const bf::BezierSurface2*>(&o);
			gValue.append(bf::saveValue(*s, i, "bezierSurfaceC2", "bezierPatchC2", idTmp));
		}
		else {
			std::cout << "Not implemented type\n";
		}
	}
	//save camera
	Json::Value cValue(Json::objectValue);
	cValue["focusPoint"]=saveValue(camera.position);
	cValue["distance"]=0.f;
	auto matrix = bf::getRotateMatrix(camera.rotation);
	glm::vec3 r = bf::matrixToEulerYXZ(matrix);
	r=bf::degrees(r);
	cValue["rotation"]=Json::objectValue;
	cValue["rotation"]["x"]=r.y;
	cValue["rotation"]["y"]=r.x;
	Json::Value root;
	root["points"]=pValue;
	root["geometry"]=gValue;
    root["camera"]=cValue;
	out << root;
	return true;
}

