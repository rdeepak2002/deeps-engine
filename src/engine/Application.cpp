//
// Created by Deepak Ramalingam on 4/30/22.
//
#include "Application.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/scene/Entity.h"
#include "src/engine/component/Component.h"
#include <iostream>

using namespace DeepsEngine;
using std::filesystem::current_path;
using std::filesystem::exists;

void Application::update(bool clearScreen) {
    // calculate delta time
    float currentFrame = renderer->getCurrentTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // update renderer
    renderer->processInput();

    if (clearScreen) {
        renderer->clear();
    }

    renderer->update();

    // update lua component systems
    for (auto& componentSystem : componentSystems) {
        componentSystem->update(deltaTime);
    }
}

void Application::initialize() {
    // create window
    renderer->createWindow();

    // initialize renderer
    renderer->initialize();

    // initialize component systems
    for (auto& componentSystem : componentSystems) {
        componentSystem->init();
    }
}

void Application::close() {
    renderer->closeWindow();
}

bool Application::shouldClose() {
    return renderer->shouldCloseWindow();
}

void Application::createSampleEntities() {
    // add camera entity
    Entity camera = Application::getInstance().scene.CreateEntity("Main Camera");
    (&camera.GetComponent<Component::Transform>())->position.z = 5.0;
    camera.AddComponent<Component::Camera>(Component::Camera({45.0f, 0.1f, 100.0f}));

    // add a single cube entity
    Entity entity = Application::getInstance().scene.CreateEntity("Cube");
    (&entity.GetComponent<Component::Transform>())->position.y = -1.2;
    entity.AddComponent<Component::MeshFilter>(Component::MeshFilter{"cube"});
    std::string scriptPath = current_path().append("assets").append("res").append("example-project").append("scripts").append("script.lua");
    entity.AddComponent<Component::LuaScript>(Component::LuaScript({scriptPath}));
}

void Application::clearRenderer() {
    renderer->clear();
}

void Application::resizeWindow(unsigned int width, unsigned int height, bool update) {
    renderer->SCR_WIDTH = width;
    renderer->SCR_HEIGHT = height;

    if (update) {
        renderer->clear();
        renderer->update();
    }
}

