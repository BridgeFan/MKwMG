//
// Created by kamil-hp on 27.03.23.
//

#include "ObjectArray.h"
#include "Settings.h"
#include "Object.h"
#include "imgui-master/imgui.h"
#include "ImGuiUtil.h"
#include <algorithm>
#include "ShaderArray.h"
#include "Curves/BezierCurve.h"
#include "Solids/MultiCursor.h"

auto isActiveLambda = [](const std::pair<std::unique_ptr<bf::Object>, bool>& o){return o.second;};

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
}

bool bf::ObjectArray::remove(std::size_t index) {
    activeRedirector=-1;
	if(index>=objects.size())
		return false;
	for(auto a: listeners)
		if(a)
			a->onRemoveObject(index);
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
		activeIndex=std::find_if(objects.begin(),objects.end(),isActiveLambda)-objects.begin();
	return true;
}


bool bf::ObjectArray::isActive(std::size_t index) {
	if(index >= objects.size())
		throw std::out_of_range("Index too large");
	return objects[index].second;
}

void bf::ObjectArray::removeActive() {
	for(std::size_t i=0u;i<size();i++)
		if(objects[i].second) {
			remove(i);
			i--;
		}
}

void bf::ObjectArray::clearSelection(std::size_t index) {
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
	if(isCorrect(index)) {
		objects[index].second=true;
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

bool bf::ObjectArray::isAnyActive() {
	return countActive>0;
}
bool bf::ObjectArray::isMultipleActive() {
	return countActive>1;
}

bool bf::ObjectArray::setActive(std::size_t index) {
	if(!isCorrect(index))
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
    updateCentre();
	return true;
}
bool bf::ObjectArray::setUnactive(std::size_t index) {
	if(!isCorrect(index)) {
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
	listeners.insert(&listener);
}
void bf::ObjectArray::removeListener(bf::ObjectArrayListener &listener) {
	listeners.erase(&listener);
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

bool bf::ObjectArray::imGuiCheckChanged(std::size_t index, bf::MultiCursor& multiCursor) {
	if(!isCorrect(index)) {
		ImGui::Text("_");
		return false;
	}
	bool val = isActive(index);
	bool ret = bf::imgui::checkSelectableChanged(objects[index].first->name.c_str(),val);
	if(ret) {
        toggleActive(index);
        multiCursor.transform = bf::Transform();
    }
	return ret;
}

void bf::ObjectArray::draw(bf::ShaderArray& shaderArray) {
    std::vector<unsigned> usedIndices;
    if(isCorrect(activeIndex)) {
        usedIndices = objects[activeIndex].first->usedVectors();
    }
    for(int k=0;k<shaderArray.getSize();k++) {
		if(k>0)
        	shaderArray.changeShader(k);
        for (std::size_t i = 0; i < objects.size(); i++) {
            if (isCorrect(i) && std::find(usedIndices.begin(), usedIndices.end(), i) == usedIndices.end()) {
                if (isActive(i))
                    shaderArray.getActiveShader().setVec3("color", 1.f, .5f, .0f);
                else
                    shaderArray.getActiveShader().setVec3("color", 1.f, 1.f, 1.f);
                objects[i].first->draw(shaderArray);
            }
        }
    }
}

void bf::ObjectArray::onMove(std::size_t index) {
	for(auto a: listeners)
		if(a)
			a->onMoveObject(index);
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

bool bf::ObjectArray::onKeyPressed(int key, int mods) {
    for(auto&& [o, b]: objects) {
        if(b && o->onKeyPressed(key, mods))
            return true;
    }
    return false;
}
bool bf::ObjectArray::onKeyReleased(int key, int mods) {
    for(auto&& [o, b]: objects) {
        if(b && o->onKeyReleased(key, mods))
            return true;
    }
    return false;
}

bool bf::ObjectArray::onMouseButtonPressed(int button, int mods) {
	for(auto&& [o, b]: objects) {
		if(b && o->onMouseButtonPressed(button, mods))
			return true;
	}
	return false;
}
bool bf::ObjectArray::onMouseButtonReleased(int button, int mods) {
	for(auto&& [o, b]: objects) {
		if(b && o->onMouseButtonReleased(button, mods))
			return true;
	}
	return false;
}

void bf::ObjectArray::onMouseMove(const glm::vec2 &oldPos, const glm::vec2 &newPos) {
	for(auto&& [o, b]: objects) {
		if(b)
			o->onMouseMove(oldPos, newPos);
	}
}

void bf::ObjectArray::removeAll() {
	objects.clear();
}
