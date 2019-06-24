#ifndef EDITOR_H
#define EDITOR_H

#include "common.h"


#define screenWIDTH 1366
#define screenHEIGTH 768

//class Server;

enum EditMode { noneMOD, moveMOD, sizeMOD, rotateMOD };
enum PrimitiveType { plane, cube, sphere };


class Editor
{
public:
	// Main
	Scene scene;
	Camera* camera;
	Grid grid;
	Shader defaultShader;
	
	GameObject* selected;

	int selectedID;

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWIDTH / (float)screenHEIGTH, 0.1f, 100.0f);
	glm::vec3 lightDir;

	// Control
	bool updatePhysics;
	bool anySelected;
	bool gameActive;
	int IDCOUNTER;

	// Engine mode
	int engineMode;

	Editor(int mode) {
		init();
		this->anySelected = false;
		this->gameActive = false;
		this->IDCOUNTER = 0;
		this->engineMode = mode;
	}
	
	Editor* getEditor() {
		return this;
	}

	void addGameObject(GameObject obj) {
		obj.setId(IDCOUNTER); IDCOUNTER++;
		std::vector<GameObject>::iterator it;
		it = scene.gameObjects.begin();
		scene.gameObjects.insert(it, obj);
		if (obj.hasPhysics())
			scene.physicsWorld->dynamicsWorld->addRigidBody(obj.getRigidBody());
	}
	int getObjectSize() {
		return scene.gameObjects.size();
	}
	void deletePhysics() {
		//delete scene.physicsWorld.dynamicsWorld;
		//delete scene.physicsWorld.solver;
		//delete scene.physicsWorld.collisionConfiguration;
		//delete scene.physicsWorld.dispatcher;
		//delete scene.physicsWorld.broadphase;
	}
	void update() {
		defaultShader.use();
		//renderGrid();
		renderObjects();
		//renderGUI();
	}
	// Editor rendering here
	void renderObjects() {
		for (int i = 0; i < scene.gameObjects.size(); i++)
		{
			// Getting Positions from Physics
			//scene.gameObjects[i].updateRenderPosition();
			if (scene.gameObjects[i].hasShader) {
				// don't forget to enable shader before setting uniforms
				scene.gameObjects[i].shader.use();
				scene.gameObjects[i].shader.setMat4("projection", projection);
				scene.gameObjects[i].shader.setMat4("view", camera->GetViewMatrix());
				scene.gameObjects[i].shader.setVec3("lightDir", lightDir);
				scene.gameObjects[i].shader.setMat4("model", scene.gameObjects[i].getModelMatrix());
				scene.gameObjects[i].shader.setFloat("scale", scene.gameObjects[i].scaleX);
				scene.gameObjects[i].shader.setVec4("color", scene.gameObjects[i].color);
				scene.gameObjects[i].draw();
			}
			else {
				defaultShader.use();
				defaultShader.setMat4("projection", projection);
				defaultShader.setMat4("view", camera->GetViewMatrix());
				defaultShader.setVec3("lightDir", lightDir);
				defaultShader.setMat4("model", scene.gameObjects[i].getModelMatrix());
				defaultShader.setFloat("scaleX", scene.gameObjects[i].scaleX);
				defaultShader.setFloat("scaleY", scene.gameObjects[i].scaleY);
				defaultShader.setFloat("scaleZ", scene.gameObjects[i].scaleZ);
				defaultShader.setVec4("color", scene.gameObjects[i].color);
				scene.gameObjects[i].draw(defaultShader);
			}

		}
	}
	void renderGUI() {
		// Imgui 
		//ImGui::ShowDemoWindow();
		
		//if (!gameActive) {
			/*ImGui::Begin("Inspector");
			ImGui::SetWindowSize(ImVec2(200.0, 400.0));
			ImGui::SetWindowPos(ImVec2(0.0, 0.0), false);
			
			ImGui::End();*/


			
		//}
	}
	void renderGrid() {
		grid.draw();
	}
	/*void generateObjForShow() {
		btVector3 pos = btVector3(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 20)),
			static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 20)),
			static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 20)));

		btCollisionShape* shape = new btBoxShape(pos);;
		MyMotionState* motionState;


		motionState =
			new MyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1), pos));

		Collider col;
		col.setCollider(3.0f, shape, motionState, pos);

		GameObject o(glm::vec3(pos.getX(), pos.getY(), pos.getZ()), 1);
		o.collider = col;
		this->addGameObject(o);

		this->scene.physicsWorld->dynamicsWorld->addRigidBody(o.collider.rigidBody);
	}*/
	void generateGamePhysicsWorld(int mode) {
		this->scene.physicsWorld = new PhysicsWorld(mode);

		for (int i = 0; i < scene.gameObjects.size(); i++) {

			if (!scene.gameObjects[i].isPhysics)
				break;

			/*
			// position
			btVector3 pos = scene.gameObjects[i].collider.rigidBody->getWorldTransform().getOrigin();

			btCollisionShape* shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));;
			//MyMotionState* motionState;
			
			// collider shape
			switch (scene.gameObjects[i].shapeType) {
				case 0: // cube
					shape = new btBoxShape(btVector3(scene.gameObjects[i].scaleX, scene.gameObjects[i].scaleY, scene.gameObjects[i].scaleZ));
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
			}
			
			//motionState = new MyMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1), pos));
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(pos);
			transform.setRotation(btQuaternion(scene.gameObjects[i].rotation.x, scene.gameObjects[i].rotation.y, scene.gameObjects[i].rotation.z));

			btScalar bodyMass = scene.gameObjects[i].collider.mass;
			btVector3 bodyInertia;
			shape->calculateLocalInertia(bodyMass, bodyInertia);

			btDefaultMotionState* motionState = new btDefaultMotionState(transform); // default motion state
			*/

			Collider col;
			col.setCollider(btVector3(scene.gameObjects[i].getPos().x, scene.gameObjects[i].getPos().y, scene.gameObjects[i].getPos().z),
				btVector3(scene.gameObjects[i].scaleX, scene.gameObjects[i].scaleY, scene.gameObjects[i].scaleZ),
				btQuaternion(scene.gameObjects[i].rotation.x, scene.gameObjects[i].rotation.y, scene.gameObjects[i].rotation.z, 1.0),
				scene.gameObjects[i].collider.mass);


			//btRigidBody* newRigidBody = new btRigidBody(bodyCI);

			// collider
			//Collider col;
			//col.setCollider(go.collider.mass, shape, motionState, pos);

			// generate new rigid body style objects..

			// set rotation of objects through rigidbody
			/*btTransform tr;
			tr.setIdentity();
			btQuaternion quat;
			quat.setEulerZYX(go.rotation.z, go.rotation.y, go.rotation.x); //or quat.setEulerZYX depending on the ordering you want
			tr.setRotation(quat);
			go.collider.rigidBody->setCenterOfMassTransform(tr);*/

			scene.gameObjects[i].collider = col;
			if(scene.gameObjects[i].isPhysics)
				this->scene.physicsWorld->dynamicsWorld->addRigidBody(scene.gameObjects[i].collider.rigidBody);
		}
	}
	void setSelectedObject(btCollisionObject *sel) {
		for (int i = 0; i < scene.gameObjects.size(); i++) {
			if (sel == scene.gameObjects[i].collider.rigidBody) {
				cout << "Selected object index is : "<< scene.gameObjects[i].id << "\n";
				this->selectedID = scene.gameObjects[i].id;
				this->selected = &scene.gameObjects[i];
				this->anySelected = true;
			}
		}
	}
	/*GameObject* getSelected() {
		for (int i = 0; i < scene.gameObjects.size(); i++) {
			if (selectedID == scene.gameObjects[i].id) {
				return &scene.gameObjects[i];
			}
		}
		return NULL;
	}*/
	bool hasSelected() {
		return anySelected;
	}
	void removeSelected() {
		this->anySelected = false;
	}
	int generatePrimitive(PrimitiveType type) {
		btCollisionShape* shape;
		Model* model;
		btScalar mass = 1.0;
		btVector3 inertia(0, 0, 0);
		// set rotation of object

		int mess = 0;

		switch (type) {
		case sphere:
			shape = new btSphereShape(1);
			model = new Model("assets/models/primitive/sphere.obj"); mess = 2;
			break;
		case cube:
			shape = new btBoxShape(btVector3(1, 1, 1));
			model = new Model("assets/models/primitive/cube.obj"); mess = 0;
			break;
		case plane:
			shape = new btStaticPlaneShape(btVector3(0, 1, 0), 1); mess = 1;
			model = new Model("assets/models/primitive/plane.obj");
			break;
		}
		if (&shape == nullptr) return 0;
		if (&model == nullptr) return 0;
		
		btVector3 pos(0, 0, 0);

		btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos)); // default motion state

		btScalar bodyMass = mass;
		btVector3 bodyInertia = pos;
		shape->calculateLocalInertia(bodyMass, bodyInertia);
		
		GameObject o(glm::vec3(0.0, 0.0, 0.0), mess);
		o.setModel(*model);
		

		Collider col;
		col.setCollider(btVector3(o.getPos().x, o.getPos().y, o.getPos().z),
			btVector3(o.scaleX, o.scaleY, o.scaleZ),
			btQuaternion(o.rotation.x, o.rotation.y, o.rotation.z, 1.0),
			1.0f);
		
		o.setPhysics(col);

		this->addGameObject(o);
		this->setSelectedObject(o.getRigidBody());
	}
	void init() {
		defaultShader.setShaders("shaders/basic.vert", "shaders/basic.frag");
		this->camera = new Camera(glm::vec3(0.0f, 3.0f, 10.0f));
		this->camera->setPitch(-30.0f);

		this->camera->setEngineMode(engineMode);
		this->scene.physicsWorld = new PhysicsWorld(0);
		lightDir = glm::vec3(3.0f, 15.0f, 5.0f);

		// Creating a framebuffer for drawing into
		// Setting this framebuffer to imgui image
	}
	void setCameraVector(glm::vec3 newcamera) {
		// make new camera vector
		cout << "yayyyyyyyyyyy";
	}
	
private:
	// Filds
	//EditorTool *currentMode;
	// Functions

	void displayTool() {
		
	}
	void changeEditMod(EditMode nem) {
		
	}
};
#endif