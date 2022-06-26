//
// Created by Deepak Ramalingam on 6/20/22.
//

#pragma once

#ifndef NATIVE_SPINNINGENTITY_HPP
#define NATIVE_SPINNINGENTITY_HPP

#include <iostream>
#include "NativeScript.h"

#define CLASS_NAME SpinningEntity

extern "C" class CLASS_NAME : public NativeScript {
public:
    void init() override;
    void update(DeepsEngine::Entity& entity, double dt) override;
};

extern "C" NativeScript* CREATE_FUNC(CLASS_NAME)() {
    return new CLASS_NAME;
}

extern "C" void DESTROY_FUNC(CLASS_NAME)(NativeScript* p) {
    delete p;
}

#endif //NATIVE_SPINNINGENTITY_HPP