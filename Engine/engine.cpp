#include "common.h"
#include "editor.h"
#include "server.h"

#define screenWIDTH 1920
#define screenHEIGTH 1080

#define VRscreenRes 900
#define VRscreenRes 900

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_press_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float delta, btDiscreteDynamicsWorld* dynamicsWorld);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void webServer();

Editor* editor;
//Game* game;
Server* server;

float lastX = screenWIDTH / 2.0f;
float lastY = screenHEIGTH / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const char* glsl_version = "#version 130";

ImGuiWindowFlags window_flags;

int VRoffsetX = (screenWIDTH/2 - VRscreenRes) / 2;
int VRoffsetY = (screenHEIGTH - VRscreenRes) / 2;

void webServer() {
	server->startServer();
}

int main() {
	// Creation handling
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// Version control
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//Creating window
	GLFWwindow* window = glfwCreateWindow(screenWIDTH, screenHEIGTH, "Test program with GLFW", glfwGetPrimaryMonitor(), NULL);

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
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	//*******************************************************************************************************************
	// Web Server
	server = new Server();
	editor->camera->setServer(server);
	std::thread first(webServer);

	//*******************************************************************************************************************
	// Shapes
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 0.1f, 0), 1);
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

	
	// Actual framebuffer operation 111111111111111111
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// generate texture
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWIDTH, screenHEIGTH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// Actual framebuffer operation 222222222222222222
	unsigned int framebuffer2;
	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);

	// generate texture
	unsigned int texColorBuffer2;
	glGenTextures(1, &texColorBuffer2);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWIDTH, screenHEIGTH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer2, 0);

	/*// Render buffer object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWIDTH, screenHEIGTH);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);*/

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//window_flags |= ImGuiWindowFlags_NoDocking;

	while (!glfwWindowShouldClose(window)) {
		

		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Calculating deltaTime
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Change the camera direction 
		processInput(window, deltaTime, editor->scene.physicsWorld->dynamicsWorld);
		editor->scene.physicsWorld->dynamicsWorld->stepSimulation(1 / 60.f, 10);
		
		//*****************************************************FRAME111111111111111111
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glClearColor(0.8f, 0.8f, 0.8f, 0.4f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		editor->renderObjects();
		editor->renderGrid();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
		//*****************************************************FRAME111111111111111111


		//*****************************************************FRAME222222222222222222
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);

		glClearColor(0.8f, 0.8f, 0.8f, 0.4f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		editor->renderObjects();
		editor->renderGrid();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
		//*****************************************************FRAME22222222222222222222


		// Imgui for editor scene
		/*ImGui::Begin("Main ImGui", 0, window_flags);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(screenWIDTH, screenHEIGTH));
		ImGui::End();

		editor->renderGUI();

		ImGui::Begin("Scene Window");
		ImGui::GetWindowDrawList()->AddImage(
			(void *)texColorBuffer2,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + 860,
				ImGui::GetCursorScreenPos().y + 860),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetWindowSize(ImVec2(860, 860));
		ImGui::End();*/

		// Imgui for RemoteVR operation
		ImGui::Begin("Left Eye");
		ImGui::GetWindowDrawList()->AddImage(
			(void *)texColorBuffer,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + VRscreenRes,
				ImGui::GetCursorScreenPos().y + VRscreenRes),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetWindowSize(ImVec2(VRscreenRes, VRscreenRes));
		ImGui::SetWindowPos(ImVec2(VRoffsetX, VRoffsetY));
		ImGui::End();

		ImGui::Begin("Right Eye");
		ImGui::GetWindowDrawList()->AddImage(
			(void *)texColorBuffer2,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + VRscreenRes,
				ImGui::GetCursorScreenPos().y + VRscreenRes),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetWindowSize(ImVec2(VRscreenRes, VRscreenRes));
		ImGui::SetWindowPos(ImVec2(screenWIDTH / 2 + VRoffsetX, VRoffsetY));
		ImGui::End();

		// Swap buffers and check for events and editor
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
	glfwDestroyWindow(window);
	glfwTerminate();


	// Delete bullet things
	editor->deletePhysics();
	
	// join webserver thread
	first.join();
	terminate();

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
