//
// Created by kamil-hp on 18.04.23.
//

#include "BasicBezier.h"
#include "GlfwUtil.h"
#include "Util.h"
#include "Scene.h"

int getIndex(unsigned lod) {
    //begin
    return fastPow(2,lod+1u)-2;
}
inline unsigned countParts(unsigned i) {return (i+1)/3;}

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

void bf::BasicBezier::draw(const bf::Shader &shader, GLFWwindow* window, const bf::Scene& scene, const bf::Settings& settings) const {
    int LOD=0;
    glBindVertexArray(VAO);
    for(unsigned i=0u;i<countParts(points.size());i++) {
        if (!window)
            LOD = 3;
        else {
            //bool pr = false;
            auto gPos1 = glm::vec2(bf::glfw::toScreenPos(window, points[i * 3], scene.getView(), scene.getProjection()));
            auto gPos2 = glm::vec2(bf::glfw::toScreenPos(window, points[i * 3 + 1], scene.getView(), scene.getProjection()));
            float distance = glm::distance(gPos1, gPos2);
            if (i + 2 < points.size()) {
                gPos1 = glm::vec2(bf::glfw::toScreenPos(window, points[i * 3 + 2], scene.getView(), scene.getProjection()));
                distance += glm::distance(gPos1, gPos2);
            }
            if (i + 3 < points.size()) {
                gPos2 = glm::vec2(bf::glfw::toScreenPos(window, points[i * 3 + 3], scene.getView(), scene.getProjection()));
                distance += glm::distance(gPos1, gPos2);
            }
            if (i + 2 >= points.size())
                LOD = 0;
            else {
                float tmp = 1.f;
                for (LOD = 0; i < MAX_FOV_LOG_PARTS; LOD++) {
                    if (distance <= tmp * 3.f)
                        break;
                    tmp *= 2.f;
                }
            }
        }
        LOD = std::min(LOD, MAX_FOV_LOG_PARTS);
        printf("%d ", LOD);
        glDrawElements(GL_LINES, fastPow(2, LOD + 1), GL_UNSIGNED_INT,   // type
                       reinterpret_cast<void *>((getIndex(LOD) + i * getIndex(MAX_FOV_LOG_PARTS + 1)) *
                                                sizeof(GLuint))           // element array buffer offset
        );
    }
    printf("\n");
}

bf::BasicBezier::BasicBezier() {

}
void bf::BasicBezier::recalculate(bool wasSizeChanged) {
    vertices.clear();
    //update curve
    //vertices
    for(int i=0;i<static_cast<int>(points.size())-1;i+=3) {
        //set basic points for de Casteljau
        std::vector<glm::vec3> pos = {points[i],points[i+1]};
        if (points.size() - i >= 3)
            pos.push_back(points[i+2]);
        if (points.size() - i >= 4)
            pos.push_back(points[i+3]);
        for (int k = 0; k <= MAX_FOV_PARTS; k++) {
            auto dcPos = deCasteljau(static_cast<float>(k) / MAX_FOV_PARTS, pos);
            vertices.emplace_back(dcPos);
        }
    }
    if(wasSizeChanged) {
        indices.clear();
        //indices
        for (unsigned k = 0u; k < countParts(points.size()); k++) {
            //set indices for adaptivity
            int s = k * MAX_FOV_PARTS;
            int offset = MAX_FOV_PARTS;
            for (unsigned i = 0u; i <= MAX_FOV_LOG_PARTS; i++) {
                indices.push_back(s + 0);
                for (int j = offset; j < MAX_FOV_PARTS; j += offset) {
                    indices.push_back(s + j);
                    indices.push_back(s + j);
                }
                indices.push_back(s + MAX_FOV_PARTS);
                offset /= 2;
            }
        }
        setBuffers();
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
    }
}
