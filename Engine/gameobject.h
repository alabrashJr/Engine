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
	glm::vec3 position;
	// There will be a list stored all components
	Model model; // This model class will inheriting from component class
	Collider collider; // All Physics info stored in this class
	Shader shader;

	// Control info
	bool isSelected;
	bool isPhysics;
	bool hasShader;

	GameObject() {
		
	}
	GameObject(glm::vec3 pos) {
		this->position = pos;
		this->isSelected = false;
		this->isPhysics = false;
		this->hasShader = false;
	}
	void setPhysics(Collider collid) {
		this->collider = collid;
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
	void setModel(Model mod) {
		this->model = mod;
	}
	void setShader(Shader s) {
		this->shader = s;
		hasShader = true;
	}
	void updateRenderPosition() {
		// Collider need own update function for update physics. shouldnt be there 
		btVector3 vec =  collider.rigidBody->getWorldTransform().getOrigin();
		position.x = (float)vec.x();
		position.y = (float)vec.y();
		position.z = (float)vec.z();
	}
	void updatePhysicsPosition(btScalar x, btScalar y, btScalar z) {
		// Also this function will move to collider class.
		btTransform trans = collider.rigidBody->getWorldTransform();
		trans.setOrigin(btVector3(x, y, z));
		collider.rigidBody->setWorldTransform(trans);
	}
	glm::vec3 getPos() const {
		//return position;
		glm::vec3 pos;
		btVector3 vec = collider.rigidBody->getWorldTransform().getOrigin();
		pos.x = (float)vec.x();
		pos.y = (float)vec.y();
		pos.z = (float)vec.z();
		return pos;
	}
	glm::mat4 getModelMatrix() {
		glm::mat4 model;
		return glm::translate(model, position);
	}
	void draw () {
		model.Draw(shader);
	}
	void draw(Shader sha) {
		model.Draw(sha);
	}
};
#endif