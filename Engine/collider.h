#include "btBulletDynamicsCommon.h"
#include "mymotionstate.h"

class Collider {
public:
	// Phsics info
	btRigidBody * rigidBody; // Everyrhing starts this guy
	btScalar mass;
	btCollisionShape* shape;
	MyMotionState* motionState;
	btVector3 inertia;

	Collider() {

	}
	Collider(btScalar mass, btCollisionShape* shape, MyMotionState* motionState, btVector3 inertia) {
		// This will implement inside collider class
		this->mass = mass; this->shape = shape; this->motionState = motionState; this->inertia = inertia;
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->motionState, this->shape, this->inertia);
		rigidBody = new btRigidBody(rigidBodyCI);
	}
	void setCollider(btScalar mass, btCollisionShape* shape, MyMotionState* motionState, btVector3 inertia) {
		// This will implement inside collider class
		this->mass = mass; this->shape = shape; this->motionState = motionState; this->inertia = inertia;
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->motionState, this->shape, this->inertia);
		rigidBody = new btRigidBody(rigidBodyCI);
	}
	void getNewRigidBody() {
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->motionState, this->shape, this->inertia);
		rigidBody = new btRigidBody(rigidBodyCI);
	}

private:

};