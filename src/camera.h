#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <vector>
#include "src/Object/Transform.h"
//source based on learnopengl.com

// Default camera values
constexpr float SPEED       = 2.5f;
constexpr float ROT_SPEED   = 20.0f;
constexpr float ZOOM        = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
namespace bf {
    class Camera : public Transform {
    protected:
        glm::vec3 front={.0f,.0f,1.f};
		glm::vec3 up={.0f,.1f,0.f};
		glm::vec3 right={1.f,.0f,.0f};
    public:
        [[nodiscard]] const glm::vec3 &getFront() const;
        [[nodiscard]] const glm::vec3 &getUp() const;
        [[nodiscard]] const glm::vec3 &getRight() const;
		float zNear, zFar;
    public:
        //constants
        float MovementSpeed;
        float RotationSpeed;
        //editable directly
        float Zoom;
        // constructor with vectors
        Camera(float near, float far, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f))
                : Transform(pos, rot), zNear(near), zFar(far), MovementSpeed(SPEED), RotationSpeed(ROT_SPEED), Zoom(ZOOM) {}
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

