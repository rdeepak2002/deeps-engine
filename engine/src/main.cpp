//
// Created by Deepak Ramalingam on 11/21/21.
//

#include "Renderer.h"

int main() {
    bool showWindow = true;
    bool saveOutputRender = false;

    Renderer *renderer = new Renderer(showWindow, saveOutputRender);

    renderer->init();

    while (!renderer->shuttingDown()) {
        renderer->render();
    }

    renderer->shutDown();

    return 0;
}