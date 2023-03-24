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
class Camera: public Transform
{
protected:
    glm::vec3 front, up, right;
public:
    const glm::vec3 &getFront() const;
    const glm::vec3 &getUp() const;
    const glm::vec3 &getRight() const;

public:
	//constants
	const float MovementSpeed;
	const float RotationSpeed;
	//editable directly
	float Zoom;
	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f)) : Transform(position, rotation), MovementSpeed(SPEED), RotationSpeed(ROT_SPEED), Zoom(ZOOM) {}
	//functions
	glm::mat4 GetViewMatrix();
    glm::mat4 GetInverseViewMatrix();
	void ProcessMouseScroll(float yoffset) {
		Zoom = std::max(std::min(Zoom-yoffset,120.f),5.f);
	}
	void ObjectGui();
};
#endif

