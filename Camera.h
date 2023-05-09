#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>



class Camera
{
	void DoJumpLogic(float deltaTime);

public:
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 LookingDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 UpVector = glm::vec3(0.0f, 1.0f, 0.0f);

	float ScrnWidth;
	float ScrnHeight;

	float Sensitivity = 0.1f;
	float MovementSpeed = 2.0f;
	float JumpPower = 3.0f;
	float MaxJumpHeight = 0.75f;

	// Field Of View
	float FOV = 45.0f;

	float Yaw = -90.0f;
	float Pitch = 0.0f;

	bool CanJump = true;
	bool ReachedMaxHeight = false;
	bool IsJumping = false;

	Camera(float screenWidth, float screenHeight, glm::vec3 startPosition);

	void InputHandler(GLFWwindow* window, float deltaTime);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	glm::mat4 getCamMatrix();


	glm::mat4 getProjection();

	glm::mat4 getView();

};


#endif // !CAMERA_CLASS_H