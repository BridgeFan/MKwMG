//
// Created by kamil-hp on 27.03.23.
//

#include "ObjectArray.h"
#include "Settings.h"
#include "Object.h"

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
	if(index>=objects.size())
		return false;
	for(auto a: listeners)
		if(a)
			a->onRemoveObject(index);
	for(std::size_t i=index+1;i<objects.size();i++)
		std::swap(objects[i],objects[i+1]);
	objects.pop_back();
	return true;
}

bool bf::ObjectArray::toggleActive(std::size_t index) {
	if(!isCorrect(index))
		return false;
	objects[index].second=!objects[index].second;
	return true;
}

bool bf::ObjectArray::isActive(std::size_t index) {
	if(index >= objects.size())
		throw std::out_of_range("Index too large");
	return objects[index].second;
}

void bf::ObjectArray::removeActive() {
	for(std::size_t i=0u;i<size();i++)
		if(objects[i].second)
			remove(i);
}

void bf::ObjectArray::clearSelection(std::size_t index, Settings& settings) {
	for(std::size_t i=0;i<objects.size();i++)
		if(i!=index)
			objects[i].second=false;
	if(isCorrect(index))
		objects[index].second=true;
	//if(settings.isMultiState)
	settings.activeIndex=static_cast<int>(index);
	/*else
		settings.activeIndex=-1;*/
}

glm::vec3 bf::ObjectArray::getCentre() {
	int count=0;
	glm::vec3 sum = {.0f,.0f,.0f};
	for(std::size_t i=0;i<objects.size();i++) {
		if(!isCorrect(i))
			continue;
		if(objects[i].second) {
			sum+=objects[i].first->getPosition();
			count++;
		}
	}
	if(count>0)
		sum /= count;
	return sum;
}

bool bf::ObjectArray::isAnyActive() {
	for(std::size_t i=0;i<size();i++)
		if(objects[i].second)
			return true;
	return false;
}

void bf::ObjectArray::clearSelection(bf::Settings &settings) {
	clearSelection(-1, settings);
}

bool bf::ObjectArray::setActive(std::size_t index) {
	if(!isCorrect(index))
		return false;
	objects[index].second=true;
	return true;
}
bool bf::ObjectArray::setUnactive(std::size_t index) {
	if(!isCorrect(index))
		return false;
	objects[index].second=false;
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
