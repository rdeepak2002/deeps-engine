//
// Created by Deepak Ramalingam on 5/1/22.
//

#include "LuaScriptComponentSystem.h"
#include "Application.h"
#include "Entity.h"
#include "Component.h"
#include "Input.h"
#include "KeyCodes.h"
#include "DeepsMath.h"
#include <iostream>

void LuaScriptComponentSystem::init() {
    ComponentSystem::init();
    Logger::Debug("initializing script system");

    // open libraries with lua
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::io);

    // define lua bindings
    lua.new_enum("Key",
                 "Space", DeepsEngine::Key::Space,
                 "A", DeepsEngine::Key::A,
                 "B", DeepsEngine::Key::B,
                 "C", DeepsEngine::Key::C,
                 "D", DeepsEngine::Key::D,
                 "E", DeepsEngine::Key::E,
                 "F", DeepsEngine::Key::F,
                 "G", DeepsEngine::Key::G,
                 "H", DeepsEngine::Key::H,
                 "I", DeepsEngine::Key::I,
                 "J", DeepsEngine::Key::J,
                 "K", DeepsEngine::Key::K,
                 "L", DeepsEngine::Key::L,
                 "M", DeepsEngine::Key::M,
                 "N", DeepsEngine::Key::N,
                 "O", DeepsEngine::Key::O,
                 "P", DeepsEngine::Key::P,
                 "Q", DeepsEngine::Key::Q,
                 "R", DeepsEngine::Key::R,
                 "S", DeepsEngine::Key::S,
                 "T", DeepsEngine::Key::T,
                 "U", DeepsEngine::Key::U,
                 "V", DeepsEngine::Key::V,
                 "W", DeepsEngine::Key::W,
                 "X", DeepsEngine::Key::X,
                 "Y", DeepsEngine::Key::Y,
                 "Z", DeepsEngine::Key::Z,
                 "Right", DeepsEngine::Key::Right,
                 "Left", DeepsEngine::Key::Left,
                 "Down", DeepsEngine::Key::Down,
                 "Up", DeepsEngine::Key::Up,
                 "LeftShift", DeepsEngine::Key::LeftShift
    );

    lua.new_usertype<glm::vec3>("vec3",
                                sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                                "x", &glm::vec3::x,
                                "y", &glm::vec3::y,
                                "z", &glm::vec3::z,
                                sol::meta_function::multiplication, sol::overload(
                    [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1*v2; },
                    [](const glm::vec3& v1, float f) -> glm::vec3 { return v1*f; },
                    [](float f, const glm::vec3& v1) -> glm::vec3 { return f*v1; }
            ),
                                sol::meta_function::addition, sol::overload(
                    [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1+v2; }
            ),
                                sol::meta_function::subtraction, sol::overload(
                    [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1-v2; }
            ),
                                sol::meta_function::equal_to, sol::overload(
                    [](const glm::vec3& v1, const glm::vec3& v2) -> bool { return v1 == v2; }
            ),
                                sol::meta_function::to_string, sol::overload(
                    [](const glm::vec3& v1) -> std::string { return "(" + std::to_string(v1.x) + ", "+ std::to_string(v1.y) + ", " + std::to_string(v1.z) + ")"; }
            )
    );

    lua.new_usertype<DeepsEngine::Entity>("Entity",
                                          "new", sol::no_constructor,
                                          "GetId", &DeepsEngine::Entity::GetId,
                                          "GetTransform", &DeepsEngine::Entity::GetComponent<DeepsEngine::Component::Transform>,
                                          "GetMeshFilter", &DeepsEngine::Entity::GetComponent<DeepsEngine::Component::MeshFilter>
    );

    lua.new_usertype<DeepsEngine::Component::Transform>("Transform",
                                                        "position", &DeepsEngine::Component::Transform::position,
                                                        "rotation", &DeepsEngine::Component::Transform::rotation,
                                                        "scale", &DeepsEngine::Component::Transform::scale,
                                                        "front", sol::as_function(&DeepsEngine::Component::Transform::front),
                                                        "right", sol::as_function(&DeepsEngine::Component::Transform::right));

    lua.new_usertype<DeepsEngine::Component::MeshFilter>("MeshFilter",
                                                        "mesh", &DeepsEngine::Component::MeshFilter::mesh,
                                                        "meshPath", &DeepsEngine::Component::MeshFilter::meshPath,
                                                        "setMeshPath", sol::as_function(&DeepsEngine::Component::MeshFilter::setMeshPath));

    lua.new_usertype<Logger>("Logger",
                             "Debug", sol::as_function(&Logger::Debug),
                             "Warn", sol::as_function(&Logger::Warn),
                             "Error", sol::as_function(&Logger::Error)
    );

    lua.new_usertype<Input>("Input",
                            "GetButtonDown", &Input::GetButtonDown,
                            "SetButtonDown", &Input::SetButtonDown
    );

    lua.new_usertype<DeepsMath>("DeepsMath",
                             "normalizeVec3", &DeepsMath::normalizeVec3,
                             "degreesToRadians", &DeepsMath::degreesToRadians,
                             "lengthVec3", &DeepsMath::lengthVec3
    );

    lua.end();
}

void LuaScriptComponentSystem::update(float deltaTime) {
    ComponentSystem::update(deltaTime);

    for(auto entity : Application::getInstance().scene.GetScriptableEntities()) {
//        if (!Application::getInstance().playing && !entity.HasComponent<DeepsEngine::Component::SceneCameraComponent>()) {
//            continue;
//        }

        auto &luaScriptComponent = entity.GetComponent<DeepsEngine::Component::LuaScript>();

        // ignore empty script paths
        if (luaScriptComponent.scriptPath.empty()) {
            continue;
        }

        std::string scriptPath = Application::getInstance().getProjectPath().append(luaScriptComponent.scriptPath);

        lua.script_file(scriptPath);

        if (entity.IsValid() && luaScriptComponent.shouldInit) {
            luaScriptComponent.self = lua.create_table_with("value", "key");
            lua["self"] = luaScriptComponent.self;

            auto f = lua["init"];

            if(f.valid()) {
                f(entity);
            } else {
                Logger::Warn("Invalid init function in script");
            }

            luaScriptComponent.shouldInit = false;
        } else if (entity.IsValid() && luaScriptComponent.shouldUpdate) {
            lua["self"] = luaScriptComponent.self;

            auto f = lua["update"];

            if (f.valid()) {
                if (Application::getInstance().playing || (!Application::getInstance().playing && entity.HasComponent<DeepsEngine::Component::SceneCameraComponent>())) {
                    f(entity, deltaTime);
                }
            } else {
                Logger::Warn("Invalid update function in script");
            }
        }
    }
}

void LuaScriptComponentSystem::destroy() {
    ComponentSystem::destroy();
}
