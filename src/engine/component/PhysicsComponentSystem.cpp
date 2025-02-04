//
// Created by Deepak Ramalingam on 7/16/22.
//

#include "PhysicsComponentSystem.h"
#include "Logger.h"
#include "Application.h"
#include "Entity.h"
#include "Component.h"

void PhysicsComponentSystem::init() {
    ComponentSystem::init();
    // Build the broadphase
    broadphase = new btDbvtBroadphase();

    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;

    // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -18.0f, 0));
    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
}

void PhysicsComponentSystem::destroy() {
    ComponentSystem::destroy();
}

void PhysicsComponentSystem::update(float deltaTime) {
    ComponentSystem::update(deltaTime);

    auto entityHandles = Application::getInstance().scene.registry.view<DeepsEngine::Component::PhysicsComponent>();
    for (auto entityHandle : entityHandles) {
        DeepsEngine::Entity entity = {entityHandle};
        auto &physicsComponent = entity.GetComponent<DeepsEngine::Component::PhysicsComponent>();
        auto &transformComponent = entity.GetComponent<DeepsEngine::Component::Transform>();

        if (physicsComponent.rigidBody) {
            btTransform trans;
            if (physicsComponent.rigidBody && physicsComponent.rigidBody->getMotionState()) {
                physicsComponent.rigidBody->getMotionState()->getWorldTransform(trans);
                transformComponent.position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
                transformComponent.rotation = glm::eulerAngles(glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()));
            }
        } else {
            auto quaternion = glm::quat(transformComponent.rotation);

            auto* motionState = new btDefaultMotionState(btTransform(
                    btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w).normalized(),
                    btVector3(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z)
            ));

            btVector3 localInertia(0, 0,0);

            bool isDynamic = (physicsComponent.mass != 0.f);

            if (isDynamic) {
                physicsComponent.compoundShape->calculateLocalInertia(physicsComponent.mass, localInertia);
            }

            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
                    physicsComponent.mass,
                    motionState,
                    physicsComponent.compoundShape,
                    localInertia
            );

            auto *rigidBody = new btRigidBody(rigidBodyCI);

            rigidBody->setFriction(physicsComponent.friction);
            rigidBody->setRollingFriction(physicsComponent.rollingFriction);
//            rigidBody->setSpinningFriction(physicsComponent.spinningFriction);
            rigidBody->setAnisotropicFriction(physicsComponent.compoundShape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
            rigidBody->setDamping(physicsComponent.linearDamping, physicsComponent.angularDamping);
            rigidBody->setRestitution(physicsComponent.restitution);
//            rigidBody->setFlags(rigidBody->getFlags() | btCollisionObject::CollisionFlags::CF_DYNAMIC_OBJECT);

            rigidBody->setAngularFactor(0.0f);

            rigidBody->activate();
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
            dynamicsWorld->addRigidBody(rigidBody);
            physicsComponent.rigidBody = rigidBody;
        }
    }

    float timeStep = 0.0f;
    if (Application::getInstance().playing) {
        timeStep = deltaTime;
    }
    dynamicsWorld->stepSimulation(timeStep, 10);
}