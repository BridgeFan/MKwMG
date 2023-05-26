//
// Created by kamil-hp on 22.04.23.
//
#include <iostream>
#include <memory>
#define VALIJSON
#include "library_include.h"
#include <format>
#include "FileLoading.h"
#include "Object/ObjectArray.h"
#include "src/Object/Point.h"
#include "Solids/Torus.h"
#include "Curves/BezierCurve0.h"
#include "Curves/BezierCurve2.h"
#include "Curves/BezierCurveInter.h"
#include "Object/Object.h"
#include "Shader/ShaderArray.h"
#include "Surfaces/BezierSurface0.h"
#include "Surfaces/BezierSurface2.h"
#include "JsonUtil.h"
#include "Surfaces/BezierSurfaceSegment.h"
#include "src/Gizmos/Cursor.h"


const std::string schemaStr = R"_JSON_({
    "$schema": "https://json-schema.org/draft/2020-12/schema",

            "type": "object",
            "properties": {
        "points": {
            "type": "array",
                    "items": { "$ref": "#/definitions/geometry/point" }
        },
        "geometry": {
            "type": "array",
                    "items": {
                "oneOf": [
                { "$ref": "#/definitions/geometry/torus" },
                { "$ref": "#/definitions/geometry/bezierC0" },
                { "$ref": "#/definitions/geometry/bezierC2" },
                { "$ref": "#/definitions/geometry/interpolatedC2" },
                { "$ref": "#/definitions/geometry/bezierSurfaceC0" },
                { "$ref": "#/definitions/geometry/bezierSurfaceC2" }
                ]
            }
        }
    },
    "additionalProperties": false,

            "definitions": {

        "normalizedValue": {
            "type": "number",
                    "minimum": 0.0,
                    "maximum": 1.0
        },

        "positiveValue": {
            "type": "number",
                    "minimum": 0.0
        },

        "uint": {
            "type": "integer",
                    "minimum": 0
        },

        "float3": {
            "type": "object",
                    "properties" : {
                "x": { "type": "number" },
                "y": { "type": "number" },
                "z": { "type": "number" }
            },
            "required": ["x", "y", "z"],
            "additionalProperties": false
        },

        "uint2": {
            "type": "object",
                    "properties" : {
                "x": { "$ref": "#/definitions/uint" },
                "y": { "$ref": "#/definitions/uint" }
            },
            "required": ["x", "y"],
            "additionalProperties": false
        },

        "geometry": {

            "point": {
                "type": "object",
                        "properties": {
                    "id":       { "$ref": "#/definitions/uint" },
                    "name":     { "type": "string" },
                    "position": { "$ref": "#/definitions/float3" }
                },
                "required": ["id", "position"],
                "additionalProperties": false
            },

            "pointRef": {
                "type": "object",
                        "properties": {
                    "id": { "$ref": "#/definitions/uint" }
                },
                "required": ["id"],
                "additionalProperties": false
            },

            "controlPoints":    {
                "type": "array",
                        "items": { "$ref": "#/definitions/geometry/pointRef" }
            },

            "patchControlPoints": {
                "type": "array",
                        "items": { "$ref": "#/definitions/geometry/pointRef" },
                "minItems": 16,
                        "maxItems": 16
            },

            "torus": {
                "type": "object",
                        "properties": {
                    "objectType":   { "const": "torus" },
                    "id":           { "$ref": "#/definitions/uint" },
                    "name":         { "type": "string" },
                    "position":     { "$ref": "#/definitions/float3" },
                    "rotation":     { "$ref": "#/definitions/float3" },
                    "scale":        { "$ref": "#/definitions/float3" },
                    "samples":      { "$ref": "#/definitions/uint2" },
                    "smallRadius":  { "type": "number", "minimum": 0.0 },
                    "largeRadius":  { "type": "number", "minimum": 0.0 }
                },
                "required": ["objectType", "id", "position", "rotation", "scale", "samples", "smallRadius", "largeRadius"],
                "additionalProperties": false
            },

            "bezierC0": {
                "type": "object",
                        "properties": {
                    "objectType":       { "const": "bezierC0" },
                    "id":               { "$ref": "#/definitions/uint" },
                    "name":             { "type": "string" },
                    "controlPoints":    { "$ref": "#/definitions/geometry/controlPoints" }
                },
                "required": ["objectType", "id", "controlPoints"],
                "additionalProperties": false
            },

            "bezierC2": {
                "type": "object",
                        "properties": {
                    "objectType":       { "const": "bezierC2" },
                    "id":               { "$ref": "#/definitions/uint" },
                    "name":             { "type": "string" },
                    "deBoorPoints":     { "$ref": "#/definitions/geometry/controlPoints" }
                },
                "required": ["objectType", "id", "deBoorPoints"],
                "additionalProperties": false
            },

            "interpolatedC2": {
                "type": "object",
                        "properties": {
                    "objectType":       { "const": "interpolatedC2" },
                    "id":               { "$ref": "#/definitions/uint" },
                    "name":             { "type": "string" },
                    "controlPoints":    { "$ref": "#/definitions/geometry/controlPoints" }
                },
                "required": ["objectType", "id", "controlPoints"],
                "additionalProperties": false
            },

            "bezierPatchC0": {
                "type": "object",
                        "properties": {
                    "objectType":       { "const": "bezierPatchC0" },
                    "id":               { "$ref": "#/definitions/uint" },
                    "name":             { "type": "string" },
                    "controlPoints":    { "$ref": "#/definitions/geometry/patchControlPoints" },
                    "samples":          { "$ref": "#/definitions/uint2" }
                },
                "required": ["objectType", "id", "controlPoints", "samples"],
                "additionalProperties": false
            },

            "bezierSurfaceC0": {
                "type": "object",
                        "properties": {
                    "objectType": { "const": "bezierSurfaceC0" },
                    "id": { "$ref": "#/definitions/uint" },
                    "name": { "type": "string" },
                    "patches": {
                        "type": "array",
                                "items": { "$ref": "#/definitions/geometry/bezierPatchC0" }
                    },
                    "parameterWrapped": {
                        "type": "object",
                                "properties": {
                            "u": { "type": "boolean" },
                            "v": { "type": "boolean" }
                        },
                        "additionalProperties": false,
                                "required": ["u", "v"]
                    },
                    "size": { "$ref": "#/definitions/uint2" }
                },
                "required": ["objectType", "id", "patches", "parameterWrapped", "size"],
                "additionalProperties": false
            },

            "bezierPatchC2": {
                "type": "object",
                        "properties": {
                    "objectType":       { "const": "bezierPatchC2" },
                    "id":               { "$ref": "#/definitions/uint" },
                    "name":             { "type": "string" },
                    "controlPoints":    { "$ref": "#/definitions/geometry/patchControlPoints" },
                    "samples":          { "$ref": "#/definitions/uint2" }
                },
                "required": ["objectType", "id", "controlPoints", "samples"],
                "additionalProperties": false
            },

            "bezierSurfaceC2": {
                "type": "object",
                        "properties": {
                    "objectType":   { "const": "bezierSurfaceC2" },
                    "id":           { "$ref": "#/definitions/uint" },
                    "name":         { "type": "string" },
                    "patches":      {
                        "type": "array",
                                "items": { "$ref": "#/definitions/geometry/bezierPatchC2" }
                    },
                    "parameterWrapped": {
                        "type": "object",
                                "properties": {
                            "u": { "type": "boolean" },
                            "v": { "type": "boolean" }
                        },
                        "additionalProperties": false,
                                "required": ["u", "v"]
                    },
                    "size": {
                        "$ref": "#/definitions/uint2"
                    }
                },
                "required": ["objectType", "id", "patches", "parameterWrapped", "size"],
                "additionalProperties": false
            }
        }
    }
})_JSON_";

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

bool bf::loadFromFile(bf::ObjectArray &objectArray, const std::string &path) {
    std::cout << std::format("Began loading file {}\n",path);
    if(!wasValidatorLoaded) {
        loadValidator();
        if(!wasValidatorLoaded) {
            std::cout << "Validator not loaded correctly\n";
            return false;
        }
    }
    Json::Value value;
    if (!valijson::utils::loadDocument(path, value)) {
        std::cerr << std::format("File {} not found!\n",path);
        return false;
    }
    std::cout << std::format("{} loaded successfully\n",path);
    valijson::Validator validator;
    valijson::adapters::JsonCppAdapter myTargetAdapter(value);
    valijson::ValidationResults errors;
    if (!validator.validate(modelSchema, myTargetAdapter, &errors)) {
        std::cerr << std::format("File {} is not valid model file! Found {} errors.\n",path, errors.numErrors());
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
    std::cout << std::format("{} validated successfully\n",path);
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
    std::cout << std::format("{} loading finished\n",path);
    return true;
}

bool bf::saveToFile(const bf::ObjectArray &objectArray, const std::string &path) {
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
    Json::Value root;
    root["points"]=pValue;
    root["geometry"]=gValue;

    std::ofstream file(path);
    if(!file.good())
        return false;
    file << root;
	return true;
}

