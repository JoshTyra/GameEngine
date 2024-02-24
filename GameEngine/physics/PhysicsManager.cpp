#include "PhysicsManager.h"
#include "FileSystemUtils.h"
#include "Renderer.h"
#include "CameraController.h"

Renderer* g_renderer = nullptr; // Actual definition
CameraController* g_cameraController = nullptr; // Actual definition

PhysicsManager::PhysicsManager()
    : dynamicsWorld(nullptr), collisionConfiguration(nullptr),
    dispatcher(nullptr), overlappingPairCache(nullptr),
    solver(nullptr), debugDrawer(nullptr) {
}

PhysicsManager::~PhysicsManager() {
    cleanup();
}

void PhysicsManager::initialize() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    debugDrawer = new PhysicsDebugDrawer(
        FileSystemUtils::getAssetFilePath("shaders/debug_vertex.glsl"),
        FileSystemUtils::getAssetFilePath("shaders/debug_fragment.glsl"),
        g_renderer,
        g_cameraController
    );
    dynamicsWorld->setDebugDrawer(debugDrawer);
}

void PhysicsManager::update(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime);
}

void PhysicsManager::cleanup() {
    delete debugDrawer;
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}
