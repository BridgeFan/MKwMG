//
// Created by kamil-hp on 27.03.23.
//

#include "ObjectArray.h"
#include "ConfigState.h"
#include "Object.h"
#include "ImGui/imgui_include.h"
#include "ImGui/ImGuiUtil.h"
#include <algorithm>
#include <iostream>
#include "Shader/ShaderArray.h"
#include "Curves/BezierCurve0.h"
#include "src/Gizmos/MultiCursor.h"
#include "Point.h"
#include "Util.h"
#include "glm/gtc/epsilon.hpp"
#include "Event.h"

auto isActiveLambda = [](const std::pair<std::unique_ptr<bf::Object>, bool>& o){return o.second;};
std::vector<bool> activeBefore;
bool isBoxSelectActive=false;

bf::Object &bf::ObjectArray::operator[](std::size_t index) {
	if(index >= objects.size())
		throw std::out_of_range("Index too large");
	if(!objects[index].first)
		throw std::domain_error("Null pointer exception");
	return *objects[index].first;
}

const bf::Object &bf::ObjectArray::operator[](std::size_t index) const {
	if(index >= objects.size())
		throw std::out_of_range("Index too large");
	if(!objects[index].first)
		throw std::domain_error("Null pointer exception");
	return *objects[index].first;
}

bool bf::ObjectArray::isCorrect(std::size_t index) const {
	return (index < objects.size() && objects[index].first);
}

void bf::ObjectArray::add(bf::Object* object) {
	objects.emplace_back(object, false);
    activeBefore.push_back(false);
}

bool bf::ObjectArray::remove(std::size_t index) {
    activeRedirector=-1;
	if(index>=objects.size() || (isCorrect(index) && objects[index].first->indestructibilityIndex>0))
		return false;
	for(auto&& [key, ptr]: listeners)
		if(ptr)
			ptr->onRemoveObject(index);
    isForcedActive=false;
	for(std::size_t i=index+1;i<objects.size();i++)
		std::swap(objects[i-1],objects[i]);
	if(objects.back().second) {
        countActive--;
        updateCentre();
    }
	objects.pop_back();
	if(countActive==0)
		activeIndex = -1;
	else if(countActive==1)
		activeIndex=static_cast<int>(std::find_if(objects.begin(),objects.end(),isActiveLambda)-objects.begin());
	return true;
}


bool bf::ObjectArray::isActive(std::size_t index) {
	if(index >= objects.size())
		throw std::out_of_range("Index too large");
	return objects[index].second;
}

void bf::ObjectArray::removeActive() {
    if(isForcedActive) {
        return;
    }
	for(std::size_t i=0u;i<size();i++)
		if(objects[i].second && remove(i)) {
			i--;
		}
    activeBefore.resize(objects.size());
    std::fill(activeBefore.begin(), activeBefore.end(), false);
}

void bf::ObjectArray::clearSelection(std::size_t index) {
    if(isForcedActive)
        return;
    if(isCorrect(activeRedirector)) {
        if(activeRedirector==static_cast<int>(index)) {
            activeRedirector = -1;
        }
        else {
            operator[](activeRedirector).addPoint(index);
            return;
        }
    }
	for(std::size_t i=0;i<objects.size();i++)
		if(i!=index)
			objects[i].second=false;
    std::fill(activeBefore.begin(), activeBefore.end(), false);
	if(isCorrect(index)) {
		objects[index].second=true;
        activeBefore[index]=true;
		activeIndex = static_cast<int>(index);
		countActive = 1;
	}
	else {
		activeIndex = -1;
		countActive = 0;
	}
    updateCentre();
}

glm::vec3 bf::ObjectArray::getCentre() {
    return centre;
}
void bf::ObjectArray::updateCentre() {
    float count=0.f;
    glm::vec3 sum = {.0f,.0f,.0f};
    for(std::size_t i=0;i<objects.size();i++) {
        if(!isCorrect(i))
            continue;
        if(objects[i].second) {
            sum+=objects[i].first->getPosition();
            count+=1.f;
        }
    }
    if(count>0)
        centre = sum * (1.f/count);
    else
        centre = {};
}

bf::ObjectArray::~ObjectArray() {
    listeners.clear();
    objects.clear();
}

bool bf::ObjectArray::isAnyActive() const {
	return countActive>0;
}
bool bf::ObjectArray::isMultipleActive() const {
	return countActive>1;
}

bool bf::ObjectArray::setActive(std::size_t index) {
	if(!isCorrect(index) || isForcedActive)
		return false;
    if(isCorrect(activeRedirector)) {
        if(operator[](activeRedirector).addPoint(index))
            return true;
    }
	if(!objects[index].second)
		countActive++;
	if(countActive==1)
		activeIndex=index;
	objects[index].second=true;
	objects[index].first->onSetActive();
    if(!isBoxSelectActive)
        activeBefore[index]=true;
    updateCentre();
	return true;
}
bool bf::ObjectArray::setUnactive(std::size_t index) {
	if(!isCorrect(index) || isForcedActive) {
        return false;
    }
    if(static_cast<int>(index)==activeRedirector) {
        activeRedirector = -1;
    }
	if(objects[index].second) {
        countActive--;
    }
	if(countActive==1) {
        activeIndex = std::find_if(objects.begin(), objects.end(), isActiveLambda) - objects.begin();
    }
	objects[index].second=false;
	objects[index].first->onSetInactive();
    if(!isBoxSelectActive)
        activeBefore[index]=false;
    updateCentre();
	return true;
}

bool bf::ObjectArray::toggleActive(std::size_t index) {
	if(!isCorrect(index))
		return false;
	objects[index].second ? setUnactive(index) : setActive(index);
	return true;
}

void bf::ObjectArray::addListener(bf::ObjectArrayListener &listener) {
	listeners[reinterpret_cast<intptr_t>(&listener)]=&listener;
}
void bf::ObjectArray::removeListener(bf::ObjectArrayListener &listener) {
    auto p = reinterpret_cast<intptr_t>(&listener);
    if(listeners.contains(p))
	    listeners.erase(p);
}

bool bf::ObjectArray::isMovable(std::size_t index) {
	return isCorrect(index) && objects[index].first->isMovable();
}

int bf::ObjectArray::getActiveIndex() const
{
	if(isCorrect(activeIndex))
		return activeIndex;
    return -1;
}

bool bf::ObjectArray::imGuiCheckChanged(std::size_t index, bf::MultiCursor& multiCursor, const bf::ConfigState& configState) {
	if(!isCorrect(index)) {
		ImGui::Text("_");
		return false;
	}
	bool val = isActive(index);
	if(configState.arePointsHidden && typeid(*objects[index].first)==typeid(bf::Point))
		return false;
	bool ret = bf::imgui::checkSelectableChanged(objects[index].first->name.c_str(), index,val);
	if(ret) {
        toggleActive(index);
        multiCursor.transform = bf::Transform();
    }
	return ret;
}

void bf::ObjectArray::draw(bf::ShaderArray& shaderArray, const bf::ConfigState& configState) {
    std::vector<unsigned> usedIndices;
    if(isCorrect(activeIndex)) {
        usedIndices = objects[activeIndex].first->usedVectors();
    }
    for(int k=0;k<shaderArray.getSize();k++) {
		if(k>0)
        	shaderArray.changeShader(k);
        if(k==PointShader)
            shaderArray.getActiveShader().setFloat("pointSize",2.f*configState.pointRadius);
        for (std::size_t i = 0; i < objects.size(); i++) {
            if (isCorrect(i) && std::find(usedIndices.begin(), usedIndices.end(), i) == usedIndices.end()) {
                if (isActive(i))
					shaderArray.setColor(255,128,0);
                else
					shaderArray.setColor(255,255,255);
                objects[i].first->draw(shaderArray);
            }
        }
    }
}

void bf::ObjectArray::onMove(std::size_t index) {
	for(auto&& [key, ptr]: listeners)
		if(ptr)
            ptr->onMoveObject(index);
}

int bf::ObjectArray::getAddToIndex() const {
    return addToIndex;
}

void bf::ObjectArray::setAddToIndex(int adt) {
    ObjectArray::addToIndex = adt;
}

void bf::ObjectArray::setActiveRedirector(const bf::Object *redirector) {
    activeRedirector=-1;
    for(unsigned i=0u;i<objects.size();i++) {
        if(objects[i].first.get()==redirector) {
            activeRedirector = i;
            return;
        }
    }
}

bool bf::ObjectArray::onKeyPressed(bf::event::Key key, bf::event::ModifierKeyBit mods) {
	using namespace bf::event;
	if(key==Key::M && isMultipleActive()) {
		int m1=INT_MAX;
		int m2=INT_MAX;
		for(int i=0;i<static_cast<int>(objects.size());i++) {
			if(objects[i].second && typeid(*objects[i].first)==typeid(bf::Point)) {
				if(m1==INT_MAX)
					m1=i;
				else {
					m2=i;
					break;
				}
			}
		}
		if(isCorrect(m2)) {
			auto& o1 = operator[](m1);
			auto& o2 = operator[](m2);
			o1.setPosition((o1.getPosition()+o2.getPosition())*.5f);
			for(auto&& [o, b]: objects) {
				if(o)
					o->onMergePoints(m1,m2);
			}
		}
	}
    for(auto&& [o, b]: objects) {
        if(b && o->onKeyPressed(key, mods))
            return true;
    }
    return false;
}
bool bf::ObjectArray::onKeyReleased(bf::event::Key key, bf::event::ModifierKeyBit mods) {
    for(auto&& [o, b]: objects) {
        if(b && o->onKeyReleased(key, mods))
            return true;
    }
    return false;
}

bool bf::ObjectArray::onMouseButtonPressed(bf::event::MouseButton button, bf::event::ModifierKeyBit mods) {
	for(auto&& [o, b]: objects) {
		if(b && o->onMouseButtonPressed(button, mods))
			return true;
	}
	return false;
}
bool bf::ObjectArray::onMouseButtonReleased(bf::event::MouseButton button, bf::event::ModifierKeyBit mods) {
	for(auto&& [o, b]: objects) {
		if(b && o->onMouseButtonReleased(button, mods))
			return true;
	}
	return false;
}

bool isBetween(const glm::vec2& v, const glm::vec2& pos1, const glm::vec2& pos2) {
    glm::vec2 m = {std::min(pos1.x,pos2.x),std::min(pos1.y,pos2.y)};
    glm::vec2 M = {std::max(pos1.x,pos2.x),std::max(pos1.y,pos2.y)};
    return v.x>=m.x && v.x<=M.x && v.y>=m.y && v.y<=M.y;
}

void bf::ObjectArray::onMouseMove(const glm::vec2 &oldPos, const glm::vec2 &newPos, const bf::ConfigState& configState,
const glm::mat4& view, const glm::mat4& projection) {
    isBoxSelectActive=configState.isBoxSelect;
    if(configState.isBoxSelect) {
        for(unsigned i=0;i<objects.size();i++) {
            auto& o = objects[i].first;
            if(o && typeid(*o)==typeid(bf::Point)) {
                auto screenPos = bf::toScreenPos(configState.screenWidth,configState.screenHeight,
                                                 o->getPosition(), view, projection);
                if(almostEqual(screenPos,bf::outOfWindow, 1e-6f))
                    continue;
                if(isBetween(screenPos, newPos, {configState.boxMouseX, configState.boxMouseY})) {
                    setActive(i);
                }
                else if(i>=activeBefore.size() || !activeBefore[i]){
                    setUnactive(i);
                }
            }
            else {
                setUnactive(i);
            }
        }
    }
    else {
        activeBefore.resize(objects.size());
        for(unsigned i=0;i<objects.size();i++) {
            activeBefore[i]=objects[i].second;
            if(!objects[i].first) continue;
            if(objects[i].second)
                objects[i].first->onMouseMove(oldPos, newPos);
        }
    }
}

void bf::ObjectArray::removeAll() {
	objects.clear();
}

