//
// Created by kamil-hp on 22.04.23.
//
#include <iostream>
#include <jsoncpp/json/json.h>
#include <memory>
#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/utils/jsoncpp_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
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
#include "JsonUtil.h"
#include "Surfaces/BezierSurfaceSegment0.h"


valijson::Schema modelSchema;
bool wasValidatorLoaded=false;

void loadValidator() {
// Load JSON document using JsonCpp with Valijson helper function
    Json::Value schemaValue;
    if (!valijson::utils::loadDocument("../schema.json", schemaValue)) {
        std::cerr << "Failed to load schema document\n";
        return;
    }
    // Parse JSON schema content using valijson
    valijson::SchemaParser parser;
    valijson::adapters::JsonCppAdapter modelSchemaAdapter(schemaValue);
    parser.populateSchema(modelSchemaAdapter, modelSchema);
    wasValidatorLoaded=true;
}

template<bf::child<bf::BezierCommon> T>
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

template<bf::child<bf::Object> T>
void emplaceToObjectArray(std::vector<std::pair<std::unique_ptr<bf::Object>,bool> >& objects, std::pair<unsigned, T*> o) {
    for(unsigned j=objects.size();j<=o.first;j++) {
        objects.emplace_back(nullptr, false);
    }
    objects[o.first].first.reset(o.second);
}

bool bf::loadFromFile(bf::ObjectArray &objectArray, const std::string &path) {
    //TODO - surfacesC0, surfacesC2
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
    valijson::Validator validator;
    valijson::adapters::JsonCppAdapter myTargetAdapter(value);
    valijson::ValidationResults errors;
    if (!validator.validate(modelSchema, myTargetAdapter, &errors)) {
        std::cerr << std::format("File {} is not valid model file! Found {} errors.\n",path, errors.numErrors());
        while(errors.numErrors()>0) {
            valijson::ValidationResults::Error error;
            std::cout << "Error:\n";
            errors.popError(error);
            for(const auto& a: error.context) {
                std::cout << std::format("{}/",a);
            }
            std::cout << std::format("\nDESC: {}\n",error.description);
        }
        return false;
    }
    objectArray.removeAll();
    Json::Value& pointValue = value["points"];
    for(auto & pValue : pointValue) {
        emplaceToObjectArray(objectArray.objects,loadPoint(pValue));
    }
    Json::Value& geometry = value["geometry"];
    for(auto& gValue: geometry) {
        //TODO - other types
        if(gValue["objectType"]=="torus") {
            emplaceToObjectArray(objectArray.objects,loadTorus(gValue));
        }
        if(gValue["objectType"]=="bezierC0") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurve0>(gValue, objectArray, "controlPoints"));
        }
        if(gValue["objectType"]=="bezierC2") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurve2>(gValue, objectArray, "deBoorPoints"));
        }
        if(gValue["objectType"]=="interpolatedC2") {
            emplaceToObjectArray(objectArray.objects,loadBezier<bf::BezierCurveInter>(gValue, objectArray, "controlPoints"));
        }
    }
    for(const auto& o: objectArray.objects) {
        if(o.first)
            o.first->postInit();
    }
    return true;
}

bool bf::saveToFile(const bf::ObjectArray &objectArray, const std::string &path) {
    //TODO - save surfacesC0, surfacesC2
    Json::Value pValue(Json::arrayValue);
    Json::Value gValue(Json::arrayValue);
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

