#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <btBulletDynamicsCommon.h>
#include "PhysicsDebugDrawer.h"

class PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    void initialize();
    void update(float deltaTime);
    void cleanup();

private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    PhysicsDebugDrawer* debugDrawer;
};

#endif // PHYSICSMANAGER_H
