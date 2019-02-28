#ifndef EDITOR_H
#define EDITOR_H
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "editor_tool.h"
#include "grid.h"
#include "primitive.h"
#include "scene.h"
#include "camera.h"
#define screenWIDTH 1366
#define screenHEIGTH 768

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

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWIDTH / (float)screenHEIGTH, 0.1f, 100.0f);
	glm::vec3 lightDir;

	// Control
	bool updatePhysics;
	bool anySelected;
	bool gameActive;
	int IDCOUNTER;

	Editor() {
		init();
		this->anySelected = false;
		this->gameActive = false;
		this->IDCOUNTER = 0;
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
		renderObjects();
		//renderGUI();
		renderGrid();
	}
	// Editor rendering here
	void renderObjects() {
		for (int i = 0; i < scene.gameObjects.size(); i++)
		{
			// Getting Positions from Physics
			scene.gameObjects[i].updateRenderPosition();
			if (scene.gameObjects[i].hasShader) {
				// don't forget to enable shader before setting uniforms
				scene.gameObjects[i].shader.use();
				scene.gameObjects[i].shader.setMat4("projection", projection);
				scene.gameObjects[i].shader.setMat4("view", camera->GetViewMatrix());
				scene.gameObjects[i].shader.setVec3("lightDir", lightDir);
				scene.gameObjects[i].shader.setMat4("model", scene.gameObjects[i].getModelMatrix());
				scene.gameObjects[i].draw();
			}
			else {
				defaultShader.use();
				defaultShader.setMat4("projection", projection);
				defaultShader.setMat4("view", camera->GetViewMatrix());
				defaultShader.setVec3("lightDir", lightDir);
				defaultShader.setMat4("model", scene.gameObjects[i].getModelMatrix());
				scene.gameObjects[i].draw(defaultShader);
			}

		}
	}
	void renderToTexture() {

	}
	void renderGUI() {
		// Imgui 
		//ImGui::ShowDemoWindow();
		
		ImGui::Begin("Switcher");
		ImGui::SetWindowSize(ImVec2(200.0, 100.0));
		ImGui::SetWindowPos(ImVec2(screenWIDTH/2, 0.0), false);
		if (ImGui::Button("Toggle GamePlay")) {
			if (!gameActive) {
				generateGamePhysicsWorld(1); gameActive = true;
				//scene.physicsWorld->setGravity(-10); gameActive = true;
			}
			else {
				generateGamePhysicsWorld(0); gameActive = false; 
				//scene.physicsWorld->setGravity(0); gameActive = false;
			}
		}
		ImGui::End();
		//if (!gameActive) {
			ImGui::Begin("Inspector");
			ImGui::SetWindowSize(ImVec2(200.0, 600.0));
			ImGui::SetWindowPos(ImVec2(0.0, 0.0), false);
			if (ImGui::Button("Add Cube")) {
				generatePrimitive((PrimitiveType)cube);
			}
			if (ImGui::Button("Add Sphere")) {
				generatePrimitive((PrimitiveType)sphere);
			}
			if (hasSelected()) {
				ImGui::Text("Translations");
				glm::vec3 trans = selected->getPos();
				float x = trans.x;
				float y = trans.y;
				float z = trans.z;
				float mas = (float) selected->collider.mass;
				if (ImGui::DragFloat("X : ", &x, 0.2)) {
					selected->updatePhysicsPosition(x, y, z);
				}
				if (ImGui::DragFloat("Y : ", &y, 0.2)) {
					selected->updatePhysicsPosition(x, y, z);
				}
				if (ImGui::DragFloat("Z : ", &z, 0.2)) {
					selected->updatePhysicsPosition(x, y, z);
				}
				if (ImGui::DragFloat("Mass : ", &mas, 0.2)) {
					selected->collider.mass = mas;
				}
			}
			ImGui::End();
			ImGui::Begin("Hierarchy");
			ImGui::SetWindowSize(ImVec2(200, 600.0));
			ImGui::SetWindowPos(ImVec2(1166.0, 0.0), false);
			ImGui::Text("Hello from hierarchy");
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				static int selected = -1;
				for (int n = 0; n < getObjectSize(); n++)
				{
					char buf[32];
					sprintf(buf, "Game Object %d", scene.gameObjects[n].id);
					if (ImGui::Selectable(buf, selected == n)) {
						selected = n; // Selected code downd there
					}
				}
				ImGui::TreePop();
			}
			ImGui::End();
		//}
	}
	void renderGrid() {
		grid.draw();
	}
	void generateGamePhysicsWorld(int mode) {
		this->scene.physicsWorld = new PhysicsWorld(mode);
		for (int i = 0; i < this->scene.gameObjects.size(); i++) {
			this->scene.gameObjects[i].collider.getNewRigidBody();
			this->scene.physicsWorld->dynamicsWorld->addRigidBody(this->scene.gameObjects[i].collider.rigidBody);
		}
	}
	void setSelectedObject(btCollisionObject *sel) {
		for (int i = 0; i < scene.gameObjects.size()-1; i++) {
			if (sel == scene.gameObjects[i].collider.rigidBody) {
				cout << "Selected object index is : "<< scene.gameObjects[i].id << "\n";
				this->selected = &scene.gameObjects[i];
				this->anySelected = true;
			}
		}
	}
	bool hasSelected() {
		return anySelected;
	}
	void removeSelected() {
		this->anySelected = false;
	}
	int generatePrimitive(PrimitiveType type) {
		btCollisionShape* shape;
		Model* model;
		btScalar mass = 0;
		btVector3 inertia(0, 0, 0);
		switch (type) {
		case sphere:
			shape = new btSphereShape(1);
			model = new Model("assets/models/primitive/sphere.obj");
			break;
		case cube:
			shape = new btBoxShape(btVector3(1, 1, 1));
			model = new Model("assets/models/primitive/cube.obj");
			break;
		case plane:
			shape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
			model = new Model("assets/models/primitive/plane.obj");
			break;
		}
		if (&shape == nullptr) return 0;
		if (&model == nullptr) return 0;

		shape->calculateLocalInertia(mass, inertia);
		MyMotionState* st =
			new MyMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

		GameObject o(glm::vec3(0.0, 0.0, 0.0));
		o.setModel(*model);
		Collider coll;
		coll.setCollider(mass, shape, st, inertia);
		o.setPhysics(coll);
		this->addGameObject(o);
		this->setSelectedObject(o.getRigidBody());
	}
	void init() {
		defaultShader.setShaders("shaders/basic.vert", "shaders/basic.frag");
		this->camera = new Camera(glm::vec3(0.0f, 5.0f, 20.0f));
		this->camera->setPitch(-60.0f);
		this->scene.physicsWorld = new PhysicsWorld(0);
		lightDir = glm::vec3(3.0f, 15.0f, 5.0f);

		// Creating a framebuffer for drawing into
		// Setting this framebuffer to imgui image
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