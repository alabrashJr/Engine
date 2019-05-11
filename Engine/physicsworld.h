#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "btBulletDynamicsCommon.h"

class PhysicsWorld {
public:
	
	btBroadphaseInterface * broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	PhysicsWorld() {

	}
	PhysicsWorld(int mode) {
		broadphase = new btDbvtBroadphase();
		// Set up the collision configuration and dispatcher
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		// The actual physics solver
		solver = new btSequentialImpulseConstraintSolver;
		// The world.
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		if (mode != 0)
			dynamicsWorld->setGravity(btVector3(0, -10, 0));
		else 
			dynamicsWorld->setGravity(btVector3(0, 0, 0));
		// 0 means editor mode and there no gravity
	}

	void setGravity(float newGravity) {
		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}
};
#endif