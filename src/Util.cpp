//
// Created by kamil-hp on 23.03.2022.
//
#include <memory>
#include "Util.h"
#include "Settings.h"
#include "Solids/Object.h"

std::string toString(const glm::vec3& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+")";
}

std::string toString(const glm::vec4& v) {
	return "("+std::to_string(v.x)+","+std::to_string(v.y)+", "+std::to_string(v.z)+","+std::to_string(v.w)+")";
}

void clearSelection(std::vector<bool>& vec, int index, Settings& settings) {
	for(int i=0;i<(int)vec.size();i++)
		if(i!=index)
			vec[i]=false;
	if(settings.isMultiState)
		settings.activeIndex=index;
	else
		settings.activeIndex=-1;
}

glm::vec3 getCentre(std::vector<bool>& selection, const std::vector<std::unique_ptr<Object> >& objects) {
	int count=0;
	glm::vec3 sum = {.0f,.0f,.0f};
	for(int i=0;i<(int)objects.size();i++) {
		if(!objects[i])
			continue;
		if(selection[i]) {
			sum+=objects[i]->getPosition();
			count++;
		}
	}
	if(count>0)
		sum /= count;
	return sum;
}

void deleteObjects(std::vector<bool> &selection, std::vector<std::unique_ptr<Object>> &objects) {
	int removed=0;
	for(int i=0;i<(int)selection.size()-removed;i++) {
		if(selection[i]) {
			removed++;
			for(int j=i;j<(int)selection.size()-removed;j++) {
				std::swap(objects[j],objects[j+1]);
				bool val = selection[j];
				selection[j] = selection[j + 1];
				selection[j + 1] = val;
				//std::swap(selection[j],selection[j+1]);
			}
			objects.pop_back();
			selection.pop_back();
		}
	}
}

std::string readWholeFile(const char* path) {
    FILE* f = fopen(path, "r");
    if(f==nullptr)
        return "";
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = new char[fsize + 1];
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    std::string ret(string);
    delete[] string;
    return ret;
}

bool isnan(const glm::vec3 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}
bool isnan(const glm::vec4 &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w);
}

GlfwStruct::GlfwStruct(Settings &settings, Camera &camera, std::vector<bool> &selection, std::vector<std::unique_ptr<Object> >& objects,
                       const float &deltaTime, ImGuiIO &io, Cursor &cursor, MultiCursor &multiCursor, Transform &multiTransform,
                       glm::vec3 &multiCentre) : settings(settings), camera(camera), selection(selection), objects(objects),
                                                deltaTime(deltaTime), io(io), cursor(cursor), multiCursor(multiCursor),
                                                 multiTransform(multiTransform), multiCentre(multiCentre) {}
