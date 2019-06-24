#ifndef COLLIDER_H
#define COLLIDER_H

#include "btBulletDynamicsCommon.h"
#include "mymotionstate.h"

class Collider {
public:
	// Phsics info
	btRigidBody * rigidBody; // Everyrhing starts this guy
	btScalar mass;
	btCollisionShape* shape;
	MyMotionState* motionState;
	btDefaultMotionState* ms;
	btVector3 inertia;

	Collider() {

	}
	Collider(btScalar mass, btCollisionShape* shape, MyMotionState* motionState, btVector3 inertia) {
		// This will implement inside collider class
		this->mass = mass; this->shape = shape; this->motionState = motionState; this->inertia = inertia;
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->motionState, this->shape, this->inertia);
		rigidBody = new btRigidBody(rigidBodyCI);
	}
	void setCollider(btVector3 pos, btVector3 scale, btQuaternion rotation, btScalar mass) {
		// Generate all collsion objects here...
		btCollisionShape* shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));;
		//MyMotionState* motionState;

		// position
		//btVector3 pos = go.collider.rigidBody->getWorldTransform().getOrigin();

		// collider shape
		/*switch (go.shapeType) {
		case 0: // cube
			shape = new btBoxShape(btVector3(go.scaleX, go.scaleY, go.scaleZ));
			break;
		case 1: // plane
			shape = new btBoxShape(btVector3(50.0f, 50.0f, 50.0f));
			pos = btVector3(0.0, -50.0, 0.0);
			break;
		case 2:
			shape = new btSphereShape(btScalar(1.));
			break;
		default:
			break;
		}*/
		
		shape = new btBoxShape(scale);

		//motionState = new MyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1), pos));
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(pos);
		transform.setRotation(rotation);

		btScalar bodyMass = mass;
		btVector3 bodyInertia;
		shape->calculateLocalInertia(bodyMass, bodyInertia);

		ms = new btDefaultMotionState(transform); // default motion state

		this->mass = mass; this->shape = shape; this->inertia = inertia;
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->ms, this->shape, this->inertia);
		//rigidBodyCI.m_restitution = 1.0f;
		//rigidBodyCI.m_friction = 0.5f;
		rigidBody = new btRigidBody(rigidBodyCI);
	}

	void getNewRigidBody() {
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(this->mass, this->ms, this->shape, this->inertia);
		rigidBody = new btRigidBody(rigidBodyCI);
	}

private:

};
#endif // !COLLIDER_H