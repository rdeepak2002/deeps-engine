//
// Created by Deepak Ramalingam on 11/14/21.
//

#ifndef ADDON_RENDERER_H
#define ADDON_RENDERER_H

#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "Shader.h"
#include "Component.h"
#include "ComponentSystem.h"
#include "Coordinator.h"
#include "ECS.h"
#include "PhysicsSystem.h"

static int scrWidth = 800;
static int scrHeight = 600;

class Renderer {
private:
    // TODO: pass in variables for dimensions (replace 800x600)
    const char *cmd = "ffmpeg -f rawvideo -pixel_format rgba -video_size 800x600 -re -framerate 60  -i - -f mpegts -pix_fmt yuv420p -crf 21 -vf vflip -preset fast udp://127.0.0.1:1234";
//    FILE* ffmpeg;
    std::string projectPath;
    bool showWindow;
    bool saveOutputRender;
    unsigned int VBO, VAO, EBO;
    unsigned int texture1, texture2;
    Shader *ourShader;
    Coordinator gCoordinator;

//    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
//    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

public:
    GLFWwindow *window;
    std::string cachedFrame;
    std::vector<Entity> entities;

    Renderer(bool showWindow, bool saveOutputRender, std::string projectPath) {
        this->showWindow = showWindow;
        this->saveOutputRender = saveOutputRender;
        this->projectPath = projectPath;

        gCoordinator.Init();

        gCoordinator.RegisterComponent<Gravity>();
        gCoordinator.RegisterComponent<RigidBody>();
        gCoordinator.RegisterComponent<Transform>();

        auto physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();

        Signature signature;
        signature.set(gCoordinator.GetComponentType<Gravity>());
        signature.set(gCoordinator.GetComponentType<RigidBody>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
    }

    ~Renderer() {
        delete ourShader;
        ourShader = nullptr;
        // TODO: free component system
    }

    void saveImage();

    int init(); // creates window/context
    void render(); //performs actual drawing
    bool shuttingDown();

    void shutDown();

    void processInput(GLFWwindow *window);

    void handleEditorResize(int width, int height);

    std::string addEntity(std::string name);

    void updateComponent(Entity entity, std::string json);

    void addComponent(Component *);

    void handleEditorDragging(float dx, float dy);
    void handleEditorDragRelease();
};


#endif //ADDON_RENDERER_H