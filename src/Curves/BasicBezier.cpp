//
// Created by kamil-hp on 18.04.23.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "BasicBezier.h"
#include "GlfwUtil.h"
#include "Util.h"
#include "Scene.h"
#include "ShaderArray.h"

glm::vec3 deCasteljau(float t, const std::vector<glm::vec3>& pos) {
    unsigned n = pos.size();
    auto beta = pos;
    for(unsigned i=1;i<=n;i++) {
        for(unsigned k=0;k<n-i;k++) {
            beta[k] = beta[k]*(1-t)+beta[k+1]*t;
        }
    }
    return beta[0];
}


bf::BasicBezier::BasicBezier() {

}
void bf::BasicBezier::recalculate(bool wasSizeChanged) {
	isDynamic=true;
	vertices.clear();
	for(int i=0;i<points.size();i++) {
		vertices.emplace_back(points[i]);
	}
    //update curve
    if(wasSizeChanged) {
        indices.clear();
		//basic topology
		for(unsigned i=0u;i<vertices.size();i++) {
			indices.push_back(i);
		}
		//tessellation topology
		for(unsigned i=0u;i<vertices.size();i++) {
			indices.push_back(i);
			if(i>0 && i%3==0 && i+1<vertices.size())
				indices.push_back(i);
		}
        setBuffers();
    }
    else {
        glNamedBufferSubData(VBO, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
}

void bf::BasicBezier::draw(const bf::ShaderArray &shaderArray, GLFWwindow *window, const bf::Scene &scene,
                           const bf::Settings&, bool isLineDrawn, bool isPointDrawn) const {
	auto& shader = shaderArray.getActiveShader();
	if(shaderArray.getActiveIndex()==BasicShader) {
		glBindVertexArray(VAO);
		shader.setVec3("color", {1.f,0.f,1.f});
		if(isLineDrawn) {
			glDrawElements(GL_LINE_STRIP, vertices.size(), GL_UNSIGNED_INT, 0);
		}
		if(isPointDrawn) {
			glDrawElements(GL_POINTS, vertices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	else if(shaderArray.getActiveIndex()==BezierShader) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glBindVertexArray(VAO);
		glPatchParameteri( GL_PATCH_VERTICES, 4);
        shader.setInt("MinSegments", 1);
        shader.setInt("MaxSegments", 4096);
        shader.setInt("ScreenWidth", width);
        shader.setInt("ScreenHeight", height);
        shader.setInt("BezierNum", 4);
        int vertSize = static_cast<int>(vertices.size());
        int fullSegments = (vertSize-1)/3;
		glDrawElements(GL_PATCHES, 4*fullSegments, GL_UNSIGNED_INT, reinterpret_cast<void*>(vertSize*sizeof(GLuint)));
        int restLength = (vertSize-1)%3+1;
        if(restLength>1) {
            shader.setInt("BezierNum", restLength);
            glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, reinterpret_cast<void*>((vertSize+4*fullSegments)*sizeof(GLuint)));
        }
	}
}

std::vector<glm::vec3> bf::bezier2ToBezier0(const std::vector<glm::vec3> &points) {
	std::vector<glm::vec3> bezier;
	if(points.size()>3) {
		bezier.resize(3*points.size()-8);
		for (int i = 1; i < static_cast<int>(points.size()) - 2; i++) {
			bezier[3*i-2]=lerp(points[i],points[i+1],1.f/3.f);
			bezier[3*i-1]=lerp(points[i],points[i+1],2.f/3.f);
			if(i==1u)
				bezier[0]=lerp(lerp(points[0],points[1],2.f/3.f),bezier[1],.5f);
			else
				bezier[3*i-3]=lerp(bezier[3*i-4],bezier[3*i-2],.5f);
		}
		bezier[3*points.size()-9]=
				lerp(bezier[3*points.size()-10],
					 lerp(points[points.size()-2],points[points.size()-1],1.f/3.f),
					 .5f);
	}
	return bezier;
}

std::vector<glm::vec3> bf::bezier0ToBezier2(const std::vector<glm::vec3> &points) {
	if(points.size()%3!=1 || points.size()<4)
		return {};
	std::vector<glm::vec3> retPoints;
	retPoints.resize(points.size()/3+3);
	retPoints[1]=lerp(points[1],points[2],-1.f);
	for(unsigned i=1u;i<points.size()-1;i++) {
		retPoints[i+1]=lerp(points[3*i-1],points[3*i-2],-1.f);
	}
	retPoints[0]=3.f*(points[0]-points[1])+retPoints[2];
	retPoints[retPoints.size()-1]=3.f*(points[points.size()-1]-points[points.size()-2])+retPoints[retPoints.size()-3];
	return retPoints;
}
