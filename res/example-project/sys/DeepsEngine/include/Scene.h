//
// Created by Deepak Ramalingam on 3/18/22.
//
#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

#include <entt/entt.hpp>
#include <iostream>

namespace DeepsEngine {
    class Entity;

    class Scene {
    public:
        entt::registry registry;
        DeepsEngine::Entity CreateEntity(std::string name, std::string guid);
        DeepsEngine::Entity CreateEntity(std::string name);
        DeepsEngine::Entity CreateEntity();
        std::vector<DeepsEngine::Entity> GetEntities();
        std::tuple<std::vector<DeepsEngine::Entity>, std::vector<DeepsEngine::Entity>> GetMeshEntities();
        std::vector<DeepsEngine::Entity> GetScriptableEntities();
        std::vector<DeepsEngine::Entity> GetCameraEntities();
        std::tuple<std::vector<DeepsEngine::Entity>, std::vector<DeepsEngine::Entity>, std::vector<DeepsEngine::Entity>> GetLightEntities();
        void DestroyEntity(DeepsEngine::Entity entity);
        void DestroyAllEntities();
        entt::entity findEntityByGuid(std::string guid);
        bool entityExists(std::string guid);
    };
}

#endif //EDITOR_SCENE_H
