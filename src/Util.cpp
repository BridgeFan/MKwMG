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

void bf::clearSelection(std::vector<bool>& vec, int index, bf::Settings& settings) {
	for(int i=0;i<(int)vec.size();i++)
		if(i!=index)
			vec[i]=false;
	if(settings.isMultiState)
		settings.activeIndex=index;
	else
		settings.activeIndex=-1;
}

glm::vec3 bf::getCentre(std::vector<bool>& selection, const std::vector<std::unique_ptr<bf::Object> >& objects) {
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

void bf::deleteObjects(std::vector<bool> &selection, std::vector<std::unique_ptr<bf::Object>> &objects) {
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

bool almostEqual(float a1, float a2) {
	constexpr auto epsilon = (float)1e-7;
	return std::abs(a1-a2)<epsilon*std::max(std::abs(a1),std::abs(a2));
}

bf::GlfwStruct::GlfwStruct(bf::Settings &settings1, bf::Camera &camera1, std::vector<bool> &selection1, std::vector<std::unique_ptr<bf::Object> >& objects1,
                       const float &deltaTime1, ImGuiIO &io1, bf::Cursor &cursor1, bf::MultiCursor &multiCursor1, bf::Transform &multiTransform1,
                       glm::vec3 &multiCentre1) : settings(settings1), camera(camera1), selection(selection1), objects(objects1),
                                                deltaTime(deltaTime1), io(io1), cursor(cursor1), multiCursor(multiCursor1),
                                                 multiTransform(multiTransform1), multiCentre(multiCentre1) {}
