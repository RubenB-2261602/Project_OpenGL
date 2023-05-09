#include "Camera.h"


Camera::Camera(float screenWidth, float screenHeight, glm::vec3 startPosition)
{
    ScrnWidth = screenWidth;
    ScrnHeight = screenHeight;
    Position = startPosition;
}

glm::mat4 Camera::getCamMatrix()
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(Position, Position + LookingDirection, UpVector);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    projection = glm::perspective(glm::radians(FOV), ScrnWidth/ScrnHeight, nearPlane, farPlane);

    return projection * view;
}

glm::mat4 Camera::getProjection()
{
    glm::mat4 projection = glm::perspective(glm::radians(FOV), ScrnWidth / ScrnHeight, 0.1f, 100.0f);


    return projection;
}

glm::mat4 Camera::getView()
{
    return glm::lookAt(Position, Position + LookingDirection, UpVector);

}



void Camera::InputHandler(GLFWwindow* window, float deltaTime)
{
    // * deltatime => to make movement smooth on every machine
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        MovementSpeed = 4.0f * deltaTime;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        MovementSpeed = 2.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Position += MovementSpeed * LookingDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // cross on lookingdirection and up gives us the horizontal vector,
        // changing sign gives oposite direction
        Position += MovementSpeed * -glm::normalize(glm::cross(LookingDirection, UpVector));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Position += MovementSpeed * -LookingDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Position += MovementSpeed * glm::normalize(glm::cross(LookingDirection, UpVector));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        IsJumping = true;
        CanJump = false;
    }

    DoJumpLogic(deltaTime);

}

void Camera::DoJumpLogic(float deltaTime)
{
    if (IsJumping)
    {
        if (Position.y >= MaxJumpHeight)
        {
            ReachedMaxHeight = true;
        }

        if (ReachedMaxHeight && Position.y > 0.0f)
        {
            Position.y += JumpPower * -1.0f * deltaTime;

            if (Position.y <= 0.0f)
            {
                IsJumping = false;
                CanJump = true;
                ReachedMaxHeight = false;
            }
        }
        else
        {
            Position.y += JumpPower * 1.0f * deltaTime;
        }
    }
    else
    {
        Position.y = 0.0f;
    }
}
