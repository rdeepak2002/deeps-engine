//
// Created by Deepak Ramalingam on 2/5/22.
//

#ifndef EXAMPLE_RENDERER_H
#define EXAMPLE_RENDERER_H

#if defined(STANDALONE)
#ifdef EMSCRIPTEN
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif
#else
#include <QOpenGLExtraFunctions>
#endif

#include "Shader.h"
#include "Scene.h"
#include "Renderer.h"
#include "AnimatedModel.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"

#if defined(STANDALONE)
class OpenGLRenderer: public Renderer {
#else
class OpenGLRenderer: public Renderer, QOpenGLExtraFunctions {
#endif
public:
    void initialize() override;
    void deinit() override;
    void clear() override;
    void update() override;
    unsigned int loadTexture(char const * path) override;
    void applyLighting(Shader* shader);
    unsigned int loadCubemap(vector<std::string> faces);
private:
    Shader* simpleMeshShader;
    Shader* animatedMeshShader;
    Shader* lightCubeShader;
    Shader* skyboxShader;
    Shader* physicsDebugShader;
    unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
};

#endif //EXAMPLE_RENDERER_H