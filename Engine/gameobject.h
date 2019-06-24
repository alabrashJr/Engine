#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


#include "shader_s.h"
#include "model.h"
#include "collider.h"
#include <string>

class GameObject
{
public:
	// Basic info
	int id;
	btTransform mInitialPosition;
	//glm::vec3 position;
	// There will be a list stored all components
	Model model; // This model class will inheriting from component class
	Collider collider; // All Physics info stored in this class
	//btRigidBody* rigidBody;
	Shader shader;

	// Control info
	bool isSelected;
	bool isPhysics;
	bool hasShader;

	int shapeType;

	glm::vec3 rotation;
	glm::vec4 color;
	float scaleX;
	float scaleY;
	float scaleZ;

	GameObject() {
		
	}
	GameObject(glm::vec3 pos, int shapeType) {
		//this->position = pos;
		this->isSelected = false;
		this->isPhysics = false;
		this->hasShader = false;
		this->shapeType = shapeType;

		scaleX = 1.0f;
		scaleY = 1.0f;
		scaleZ = 1.0f;

		rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec4(0.5f, 0.4f, 0.45f, 1.0f);
	}
	/*void createRigidBody(btRigidBody::btRigidBodyConstructionInfo bodyCI) {
		collider.rigidBody = new btRigidBody(bodyCI);
	}*/
	void setPhysics(Collider collid) {
		this->collider = collid;
		isPhysics = true;
	}
	void setPhysics() {
		isPhysics = true;
	}
	void setId(int i) {
		this->id = i;
	}
	bool hasPhysics() {
		return isPhysics;
	}
	btRigidBody* getRigidBody() {
		return collider.rigidBody;
	}
	void setRigidBody(btRigidBody* rb) {
		collider.rigidBody = rb;
	}
	void setModel(Model mod) {
		this->model = mod;
	}
	void setShader(Shader s) {
		this->shader = s;
		hasShader = true;
	}
	/*void updateRenderPosition() {
		// Collider need own update function for update physics. shouldnt be there 
		btVector3 vec =  collider.rigidBody->getWorldTransform().getOrigin();
		position.x = (float)vec.x();
		position.y = (float)vec.y();
		position.z = (float)vec.z();
	}*/
	void updatePosition(btScalar x, btScalar y, btScalar z)
	{
		// Also this function will move to collider class.
		btTransform trans = collider.rigidBody->getWorldTransform();
		trans.setOrigin(btVector3(x, y, z));
		collider.rigidBody->setWorldTransform(trans);
	}
	void updateRotation(btScalar x, btScalar y, btScalar z)
	{
		/*btTransform trans = collider.rigidBody->getWorldTransform();
		btQuaternion quat;
		quat.setEuler(x, y, z);
		trans.setRotation(quat);
		collider.rigidBody->setCenterOfMassTransform(trans);*/
		rotation = glm::vec3(x, y, z);
	}
	void updateScale(int sX, int sY, int sZ)
	{
		scaleX = (float)sX;
		scaleY = (float)sY;
		scaleZ = (float)sZ;
		
		btCollisionShape* shape = new btBoxShape(btVector3(scaleX, scaleY, scaleZ));

		switch (shapeType) {
		case 0:
			shape = new btSphereShape(1);
			break;
		case 1:
			shape = new btBoxShape(btVector3(scaleX, scaleY, scaleZ));
			break;
		}
		collider.rigidBody->setCollisionShape(shape);
	}
	void updateColor(btScalar r, btScalar g, btScalar b, btScalar a)
	{
		color = glm::vec4(r, g, b, a);
	}
	glm::vec3 getPos() {
		//return position;
		if (collider.ms == NULL) {
			return glm::vec3(0, 0, 0);
		}
		glm::vec3 pos;
		btVector3 vec = collider.rigidBody->getWorldTransform().getOrigin();
		pos.x = (float)vec.x();
		pos.y = (float)vec.y();
		pos.z = (float)vec.z();
		return pos;
	}
	glm::mat4 getModelMatrix() {
		glm::mat4 model;
		glm::vec3 orj = glm::vec3(collider.rigidBody->getWorldTransform().getOrigin().getX(),
			collider.rigidBody->getWorldTransform().getOrigin().getY(),
			collider.rigidBody->getWorldTransform().getOrigin().getZ());
		
		model = glm::translate(model, orj);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis
		
		return model;
	}
	void draw () {
		model.Draw(shader);
	}
	void draw(Shader sha) {
		model.Draw(sha);
	}
};
#endif