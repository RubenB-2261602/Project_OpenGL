#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "FileReader.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Model.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool CheckCollision(glm::vec3 object);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera playerCam = Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f));

struct MazeLocation
{
    glm::vec3 position;
    bool isWall;

    MazeLocation(glm::vec3 position, bool isWall) {
        this->position = position;
        this->isWall = isWall;
    }
};

// Cube information
float vertices[] = {
        // Postion            // Normals           // TextCoord
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 0,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  2, 0,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  2, 2,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 2,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0, 0,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  2, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  2, 2,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0, 2,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 0,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  2, 0,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  2, 2,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 2,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0, 0,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  2, 0,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  2, 2,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0, 2,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0, 0,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  2, 0,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  2, 2,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0, 2,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0, 0,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  2, 0,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  2, 2,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0, 2,
        
};

unsigned int indices[] = {
    // Front
    0, 1, 2,
    0, 2, 3, 

    // Back
    4, 5, 6,
    4, 6, 7,

    // Left
    8, 9, 10,
    8, 10, 11,

    // Right
    12, 13, 14,
    12, 14, 15,

    // Bottom
    16, 17, 18,
    16, 18, 19,

    //// Top
    20, 21, 22,
    20, 22, 23

};

int main()
{
    // glfw initialize
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Maze project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad load all OpenGL funnction pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Read maze TXT file
    std::vector<MazeLocation> mazeWalls;
    std::string maze = FileReader("resources/maze.txt").getFileContent();
    int i = 0;
    float x = 0.0f;
    float z = 0.0f;
    while (maze[i] != '\0')
    {
        switch (maze[i])
        {
        case ' ':
            mazeWalls.push_back(MazeLocation(glm::vec3(x, 0.0f, z), false));
            x--;
            break;
        case '#':
            mazeWalls.push_back(MazeLocation(glm::vec3(x, 0.0f, z), true));
            x--;
            break;
        case '\n':
            x = 0.0f;
            z -= 1.0f;
            break;
        }
        i++;
    }

    // Creating offset list for instancing
    std::vector<glm::vec3> floorTranslations;
    for (size_t i = 0; i < mazeWalls.size(); i++)
    {
        glm::vec3 translation(mazeWalls[i].position.x, -1.0f, mazeWalls[i].position.z);
        floorTranslations.push_back(translation);
    }

    std::vector<glm::vec3> wallTranslations;
    for (size_t i = 0; i < mazeWalls.size(); i++)
    {
        if (mazeWalls[i].isWall)
        {
            glm::vec3 translation(mazeWalls[i].position.x, mazeWalls[i].position.y, mazeWalls[i].position.z);
            wallTranslations.push_back(translation);
        }
    }

    // Create shaders
    Shader shader(FileReader("resources/shaders/cubeShader.vs").getFileContent(),
        FileReader("resources/shaders/cubeShader.fs").getFileContent());

    Shader lightShader(FileReader("resources/shaders/cubeShader.vs").getFileContent(),
                       FileReader("resources/shaders/lightShader.fs").getFileContent());

    Shader modelShader("resources/shaders/modelShader.vs", "resources/shaders/modelShader.fs");


    // Create VAO, VBO & EBO's
    VAO floorVAO = VAO();
    VBO cubeVBO = VBO(vertices, sizeof(vertices));
    EBO cubeEBO = EBO(indices, sizeof(indices));

    floorVAO.AddAttrib(cubeVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    floorVAO.AddAttrib(cubeVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(6*sizeof(float)));
    floorVAO.AddAttrib(cubeVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));

    unsigned int floorInstanceVBO;
    glGenBuffers(1, &floorInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * floorTranslations.size(), &floorTranslations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, floorInstanceVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    floorVAO.Unbind();
    cubeVBO.UnBind();
    cubeEBO.UnBind();

    VAO wallVAO = VAO();
    cubeVBO.Bind();
    cubeEBO.Bind();

    wallVAO.AddAttrib(cubeVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    wallVAO.AddAttrib(cubeVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    wallVAO.AddAttrib(cubeVBO, 3, 2, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int wallInstanceVBO;
    glGenBuffers(1, &wallInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, wallInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * wallTranslations.size(), &wallTranslations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, wallInstanceVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    wallVAO.Unbind();
    cubeVBO.UnBind();
    cubeEBO.UnBind();

    VAO lightVAO = VAO();
    cubeVBO.Bind();
    cubeEBO.Bind();

    lightVAO.AddAttrib(cubeVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);

    lightVAO.Unbind();
    cubeVBO.UnBind();
    cubeEBO.UnBind();

    // Create Textures
    Texture leaves("resources/textures/leaves.png", GL_TEXTURE_2D, GL_TEXTURE0);
    Texture gravel("resources/textures/gravel.png", GL_TEXTURE_2D, GL_TEXTURE0);

    // Enable GL functions
    glEnable(GL_DEPTH_TEST);
  
  
    //Loads diamond for testing, this can be changed later
    Model diamondModel("resources/models/diamond/source/Diamond.blend");


    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // Draw loop
    while (!glfwWindowShouldClose(window))
    {
        // timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        
        processInput(window);

        glm::vec3 lastPos = playerCam.Position;
        playerCam.InputHandler(window, deltaTime);
        bool isCollision = CheckCollision(wallTranslations[0]);


        for (size_t i = 0; i < wallTranslations.size(); i++)
        {
            isCollision = CheckCollision(wallTranslations[i]);
            if (isCollision)
            {
                break;
            }
        }

        if (isCollision)
        {
            playerCam.Position.x = lastPos.x;
            playerCam.Position.z = lastPos.z;
        }


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);

        lightShader.Enable();
        lightVAO.Bind();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(playerCam.getCamMatrix()));
        glUniform3f(glGetUniformLocation(lightShader.ID, "viewPos"), playerCam.Position.x, playerCam.Position.y, playerCam.Position.z);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Draw 3d model
        modelShader.Enable();

        glm::mat4 view = playerCam.getView();
        glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(playerCam.getProjection()));
        glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 assimpModel;
        assimpModel = glm::translate(assimpModel, glm::vec3(0.0f, -1.75f, 0.0f));
        assimpModel = glm::scale(assimpModel, glm::vec3(0.2f, 0.2f, 0.2f));
        
        glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(assimpModel));
        diamondModel.Draw(modelShader)
          
        // Transform local coordinats to view coordiantes.Enable();
        model = glm::mat4(1.0f);
        glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(playerCam.getCamMatrix()));

        // render
        wallVAO.Bind();
        leaves.Bind();
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, wallTranslations.size());

        floorVAO.Bind();
        gravel.Bind();
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, floorTranslations.size());



        // swap buffers & check events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    floorVAO.CleanUp();
    cubeVBO.CleanUp();
    wallVAO.CleanUp();
    leaves.CleanUp();
    gravel.CleanUp();
    shader.CleanUp();

	return 0;
}

void processInput(GLFWwindow* window) 
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    playerCam.Yaw += xoffset;
    playerCam.Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (playerCam.Pitch > 89.0f)
        playerCam.Pitch = 89.0f;
    if (playerCam.Pitch < -89.0f)
        playerCam.Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(playerCam.Yaw)) * cos(glm::radians(playerCam.Pitch));
    front.y = sin(glm::radians(playerCam.Pitch));
    front.z = sin(glm::radians(playerCam.Yaw)) * cos(glm::radians(playerCam.Pitch));
    playerCam.LookingDirection = glm::normalize(front);
}

bool CheckCollision(glm::vec3 object)
{
    glm::vec3 posBeforeColl = playerCam.Position;
    if (playerCam.Position.x <= object.x + 0.65f &&
        playerCam.Position.y <= object.y + 1.65f &&
        playerCam.Position.z <= object.z + 0.65f &&
        playerCam.Position.x >= object.x - 0.65f &&
        playerCam.Position.y >= object.y - 1.65f &&
        playerCam.Position.z >= object.z - 0.65f )
    {
        std::cout << "Collision!" << std::endl;
        return true;
    }

    return false;
}
