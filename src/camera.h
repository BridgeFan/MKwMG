#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <vector>
#include "Solids/Transform.h"
//source based on learnopengl.com

// Default camera values
constexpr float SPEED       = 2.5f;
constexpr float ROT_SPEED   = 20.0f;
constexpr float ZOOM        = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
namespace bf {
    class Camera : public Transform {
    protected:
        glm::vec3 front, up, right;
    public:
        [[nodiscard]] const glm::vec3 &getFront() const;
        [[nodiscard]] const glm::vec3 &getUp() const;
        [[nodiscard]] const glm::vec3 &getRight() const;
    public:
        //constants
        const float MovementSpeed;
        const float RotationSpeed;
		const float zNear, zFar;
        //editable directly
        float Zoom;
        // constructor with vectors
        Camera(float near, float far, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f))
                : Transform(pos, rot), MovementSpeed(SPEED), RotationSpeed(ROT_SPEED), zNear(near), zFar(far), Zoom(ZOOM) {}
        //functions
        glm::mat4 GetViewMatrix();
        glm::mat4 GetInverseViewMatrix();
        void ProcessMouseScroll(float yoffset) {
            Zoom = std::max(std::min(Zoom - yoffset, 120.f), 5.f);
        }
        void ObjectGui();
    };
}
#endif

