#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "camera.h"
#include "model.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include "editor.h"
#include <chrono>
#include <thread>

#define screenWIDTH 1366
#define screenHEIGTH 768

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_press_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float delta, btDiscreteDynamicsWorld* dynamicsWorld);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Editor* editor;
//Game* game;

float lastX = screenWIDTH / 2.0f;
float lastY = screenHEIGTH / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main() {
	// Creation handling
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// Version control
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//Creating window
	GLFWwindow* window = glfwCreateWindow(screenWIDTH, screenHEIGTH, "Test program with GLFW", NULL, NULL);

	// Creation handling
	if (window == nullptr) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int screenWidth, screeHeigth;
	glfwGetFramebufferSize(window, &screenWidth, &screeHeigth);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_press_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewExperimental = GL_TRUE;
	
	if (GLEW_OK != glewInit()) {

		return EXIT_FAILURE;
	}
	// Checking versions 
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
	printf("GLEW version supported by this platform (%s): \n", glewGetString(GLEW_VERSION));

	glViewport(0, 0, screenWidth, screeHeigth);
	editor = new Editor();
	
	// Starting with editor It could be changable in future.
	// Engine would be startable with a playable scene.

	// Creating dear Imgui
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//*******************************************************************************************************************
	// Shapes
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	MyMotionState* groundMotionState =
		new MyMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	//*******************************************************************************************************************
	// ligth shader
	Shader lightShader, lampShader, ourShader, gridShader;
	lightShader.setShaders("shaders/ligth.vert", "shaders/ligth.frag");
	lampShader.setShaders("shaders/lamp.vert", "shaders/lamp.frag");
	ourShader.setShaders("shaders/basic.vert", "shaders/basic.frag");
	gridShader.setShaders("shaders/grid.vert", "shaders/grid.frag");

	Model plane("assets/models/primitive/plane.obj");
	Model house("assets/models/House/WoodenCabinObj.obj");

	GameObject o1( glm::vec3(0.0f, 0.0f, 0.0f));
	o1.setModel(plane);
	o1.setShader(ourShader);

	Collider col;
	col.setCollider(0, groundShape, groundMotionState, btVector3(0.0, 0.0, 0.0));

	o1.setPhysics(col);
	editor->addGameObject(o1);

	// Setting our camera
	glEnable(GL_DEPTH_TEST);
	
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		
		glClearColor(0.8f, 0.8f, 0.8f, 0.4f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();

		// Calculating deltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Change the camera direction 
		processInput(window, deltaTime, editor->scene.physicsWorld->dynamicsWorld);

			editor->scene.physicsWorld->dynamicsWorld->stepSimulation(1 / 60.f, 10);
			editor->update();
		
		// Swap buffers and check for events and editor
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	// Delete bullet things
	editor->deletePhysics();

	return EXIT_SUCCESS;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::cout << "MOuse clicked";
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		std::cout << "MOuse clicked";
}
void processInput(GLFWwindow *window, float delta, btDiscreteDynamicsWorld* dynamicsWorld)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		editor->camera->ProcessKeyboard(FORWARD, delta);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		editor->camera->ProcessKeyboard(BACKWARD, delta);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		editor->camera->ProcessKeyboard(LEFT, delta);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		editor->camera->ProcessKeyboard(RIGHT, delta);
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		std::cout << "MOuse clicked";

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		float x = (2.0f * xpos) / screenWIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / screenHEIGTH;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		std::cout << "X: " << x << "Y: " << y << "Z: " << z << "\n";
		glm::vec4 ray_clip = glm::vec4(x, y, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(editor->projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0);
		glm::vec3 ray_wor = glm::vec3((glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).x, (glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).y, (glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).z);
		std::cout << "Normalised view world coordinates" << "X: " << ray_wor.x << "Y: " << ray_wor.y << "Z: " << ray_wor.z << "\n";
		
		float distance = 100.0;
		// Perform raycast
		btVector3 btFrom(editor->camera->Position.x, editor->camera->Position.y, editor->camera->Position.z);
		btVector3 btTo(ray_wor.x*distance, ray_wor.y*distance, ray_wor.z*distance);
		btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

		dynamicsWorld->rayTest(btFrom, btTo, res); // m_btWorld is btDiscreteDynamicsWorld

		if (res.hasHit()) {
			editor->setSelectedObject((btCollisionObject*)res.m_collisionObject);
			printf("Collision at: <%.2f, %.2f, %.2f>\n", res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ());
		}else
			editor->removeSelected();

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		editor->camera->ProcessMouseMovement(xoffset, yoffset);
	}
}
void mouse_press_callback(GLFWwindow* window, int button, int action, int mods)
{
	std::cout << "Clicked here!";
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::cout << "Clicked here!";
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	editor->camera->ProcessMouseScroll(yoffset);
}
