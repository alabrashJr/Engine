#include "common.h"
#include "editor.h"
#include "server.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

#define screenWIDTH 1260
#define screenHEIGTH 800

#define renderWIDTH 1080
#define renderHEIGTH 1080

#define VRscreenRes 667
#define VRscreenRes 667

int saveCurrentSceneAs();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_press_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float delta, btDiscreteDynamicsWorld* dynamicsWorld);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void createGameObject(string const &modelPath, Shader shader, glm::vec3 position);
void webServer();
string strFromFile();
void ShowExampleMenuFile();
void ShowHierarch();
void sample_other();
int engine();
int openSceneFromFile(string path);

void strToFile(string outstring, string path);

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

btCollisionShape* groundShape;
btDefaultMotionState* groundMotionState;

int VRoffsetX = (screenWIDTH/2 - VRscreenRes) / 2;
int VRoffsetY = (screenHEIGTH - VRscreenRes) / 2;

int screenWidth, screenHeigth;

int mainEngineMode = 0; // 0 for editor, 1 for game, 2 for vr

void webServer() {
	server->startServer();
}

string strFromFile(string path) {
	std::ifstream ifs(path);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	ifs.close();
	return content;
}

void strToFile(string outstring, string path) {
	std::ofstream file{ path };
	file << outstring;
	file.close();
}

int main() {

	engine();

	return EXIT_SUCCESS;
}
int openSceneFromFile(string path)
{
	FILE* fp = fopen(path.c_str(), "rb"); // non-Windows use "r"
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);
	fclose(fp);
	
	assert(d["engine_mode"].IsInt());
	editor = new Editor(d["engine_mode"].GetInt());

	rapidjson::Value::Array gameobjects = d["gameobjects"].GetArray();


	for (rapidjson::Value::ConstValueIterator itr = gameobjects.Begin(); itr != gameobjects.End(); ++itr) {
		const rapidjson::Value& go = *itr;
		assert(go.IsObject()); // each attribute is an object
		
		// create gameobject here
		editor->generatePrimitive(cube);

		// scale_factor
		assert(go["scale_factor"].IsArray());
		//assert(go["scale_factor"][0].IsFloat());

		float scaleX = go["scale_factor"][0].GetFloat();
		float scaleY = go["scale_factor"][1].GetFloat();
		float scaleZ = go["scale_factor"][2].GetFloat();
		// position
		float posX = go["position"][0].GetFloat();
		float posY = go["position"][1].GetFloat();
		float posZ = go["position"][2].GetFloat();
		// rotation
		float rotX = go["rotation"][0].GetFloat();
		float rotY = go["rotation"][1].GetFloat();
		float rotZ = go["rotation"][2].GetFloat();
		// color
		float r = go["color"][0].GetFloat();
		float g = go["color"][1].GetFloat();
		float b = go["color"][2].GetFloat();
		float a = go["color"][3].GetFloat();
		// mass
		float mass = go["mass"].GetFloat();

		editor->selected->updatePosition(posX, posY, posZ);
		editor->selected->updateRotation(rotX, rotY, rotZ);
		editor->selected->updateScale(scaleX, scaleY, scaleZ);
		editor->selected->updateColor(r, g, b, a);
		editor->selected->collider.mass = mass;
	}

	return 0;
}
int saveCurrentSceneAs(string filename) {
	// Create 
	// document is the root of a json message
	rapidjson::Document document;

	// define the document as an object rather than an array
	document.SetObject();

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Engine values
	document.AddMember("version", "0.0.1", allocator);
	document.AddMember("engine_mode", engineMode, allocator);

	// Gameobject arrays
	rapidjson::Value gameobjects(rapidjson::kArrayType);

	for (GameObject go : editor->scene.gameObjects) {
		rapidjson::Document d;
		d.SetObject();
		
		// create gameobject thing
		d.AddMember("type", "model", allocator);
		d.AddMember("name", "none", allocator);
		d.AddMember("has_collider", go.isPhysics, allocator);
		d.AddMember("collider_type", go.shapeType, allocator);
		d.AddMember("mass", go.collider.mass, allocator);
		// scale_factor array
		rapidjson::Value scale_factor(rapidjson::kArrayType);
		scale_factor.PushBack(go.scaleX, allocator);
		scale_factor.PushBack(go.scaleY, allocator);
		scale_factor.PushBack(go.scaleZ, allocator);
		d.AddMember("scale_factor", scale_factor, allocator);
		// position array
		rapidjson::Value position(rapidjson::kArrayType);
		position.PushBack(go.collider.rigidBody->getWorldTransform().getOrigin().getX(), allocator);
		position.PushBack(go.collider.rigidBody->getWorldTransform().getOrigin().getY(), allocator);
		position.PushBack(go.collider.rigidBody->getWorldTransform().getOrigin().getZ(), allocator);
		d.AddMember("position", position, allocator);
		// rotation array
		rapidjson::Value rotation(rapidjson::kArrayType);
		rotation.PushBack(go.rotation.x, allocator);
		rotation.PushBack(go.rotation.y, allocator);
		rotation.PushBack(go.rotation.z, allocator);
		d.AddMember("rotation", rotation, allocator);
		// other values
		Value model_path;
		model_path.SetString(go.model.fullpath.c_str(), allocator);
		d.AddMember("model_path", model_path, allocator);
		d.AddMember("shader_type", "textured", allocator);
		d.AddMember("custom_vertex_shader_path", "none", allocator);
		d.AddMember("custom_fragment_shader_path", "none", allocator);
		d.AddMember("light_type", "none", allocator);
		// color array
		rapidjson::Value color(rapidjson::kArrayType);
		color.PushBack(go.color.r, allocator);
		color.PushBack(go.color.g, allocator);
		color.PushBack(go.color.b, allocator);
		color.PushBack(go.color.a, allocator);
		d.AddMember("color", color, allocator);
		// add to gameobjects array
		gameobjects.PushBack(d, allocator);
	}
	// gameobjects array to final document

	document.AddMember("gameobjects", gameobjects, allocator);

	// save to file with string 
	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	document.Accept(writer);

	strToFile(strbuf.GetString(),"scenes\\" + filename + ".txt");
	return 0;
}
void sample_other() {
	// document is the root of a json message
	rapidjson::Document document;

	// define the document as an object rather than an array
	document.SetObject();

	// create a rapidjson array type with similar syntax to std::vector
	rapidjson::Value array(rapidjson::kArrayType);

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// chain methods as rapidjson provides a fluent interface when modifying its objects
	array.PushBack("hello", allocator).PushBack("world", allocator);//"array":["hello","world"]

	document.AddMember("Name", "XYZ", allocator);
	document.AddMember("Rollnumer", 2, allocator);
	document.AddMember("array", array, allocator);

	// create a rapidjson object type
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("Math", "50", allocator);
	object.AddMember("Science", "70", allocator);
	object.AddMember("English", "50", allocator);
	object.AddMember("Social Science", "70", allocator);
	document.AddMember("Marks", object, allocator);
	//	fromScratch["object"]["hello"] = "Yourname";

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	document.Accept(writer);

	std::cout << strbuf.GetString() << std::endl;
}


int engine() {
	// Creation handling
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// Version control
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//Creating window
	GLFWwindow* window = glfwCreateWindow(screenWIDTH, screenHEIGTH, "Test program with GLFW", NULL, NULL);
	//window = glfwCreateWindow(screenWIDTH, screenHEIGTH, "Test program with GLFW", glfwGetPrimaryMonitor(), NULL);

	// Creation handling
	if (window == nullptr) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	
	glfwGetFramebufferSize(window, &screenWidth, &screenHeigth);
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

	glViewport(0, 0, screenWidth, screenHeigth);
	editor = new Editor(mainEngineMode);
	
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
	groundShape = new btStaticPlaneShape(btVector3(0, 0.1f, 0), 1);
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	//*******************************************************************************************************************
	// ligth shader
	Shader lightShader, lampShader, ourShader, gridShader, textureShader;
	lightShader.setShaders("shaders/ligth.vert", "shaders/ligth.frag");
	lampShader.setShaders("shaders/lamp.vert", "shaders/lamp.frag");
	ourShader.setShaders("shaders/basic.vert", "shaders/basic.frag");
	gridShader.setShaders("shaders/grid.vert", "shaders/grid.frag");
	textureShader.setShaders("shaders/texture.vert", "shaders/texture.frag");

	Model Plane("assets/models/primitive/plane.obj");
	//Model House("assets/models/House/WoodenCabinObj.obj");

	GameObject o(glm::vec3(0.0f, 0.0f, 0.0f), 1); // plane shapeType = 1
	//o1.setModel(plane);
	o.setShader(ourShader);

	Collider col;
	//col.setCollider(0, groundShape, groundMotionState, btVector3(0.0, 0.0, 0.0));
	col.setCollider(btVector3(o.getPos().x, o.getPos().y, o.getPos().z),
		btVector3(o.scaleX, o.scaleY, o.scaleZ),
		btQuaternion(o.rotation.x, o.rotation.y, o.rotation.z, 1.0),
		o.collider.mass);

	o.setPhysics(col);
	o.isPhysics = false;

	//editor->addGameObject(o);
	
	//editor->generatePrimitive(plane);

	// Adding game objects for presentationScene
	// createGameObject("assets/models/primitive/WoodenCabinObj.obj", textureShader, glm::vec3(50.0f, 0.0f, 0.0f));


	switch (mainEngineMode) {
	case 0: // editor
		//createGameObject("assets/models/handgun/Handgun_obj.obj", textureShader, glm::vec3(00.0f, 40.0f, 0.0f));
		//createGameObject("assets/models/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj", textureShader, glm::vec3(00.0f, 40.0f, 0.0f)); 
		//createGameObject("assets/models/UH60/uh60.obj", textureShader, glm::vec3(00.0f, 20.0f, 0.0f));
		break;
	case 1: // game
		break;
	case 2: // RemoteVR
		break;
	default:
		break;
	}

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
		//editor->renderGrid();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
		//*****************************************************FRAME111111111111111111

		bool a = true;

		switch (mainEngineMode) {
			case 0: // Editor mode
				
				editor->renderGUI();
				
				ShowExampleMenuFile(); // Main menu window
				ShowHierarch();		   // Edit and info window
				
				ImGui::Begin("Scene Window");
				ImGui::GetWindowDrawList()->AddImage(
					(void *)texColorBuffer,
					ImVec2(ImGui::GetCursorScreenPos()),
					ImVec2(ImGui::GetCursorScreenPos().x + renderWIDTH,
						ImGui::GetCursorScreenPos().y + renderHEIGTH),
					ImVec2(0, 1), ImVec2(1, 0));
				ImGui::SetWindowSize(ImVec2(screenWidth * 0.6f, screenHeigth));
				ImGui::SetWindowPos(ImVec2(screenWidth * 0.2f, 0));
				ImGui::End();
				break;
			case 1: // Game mode

				break;
			case 2: // RemoteVR mode

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
				break;
			default:
				break;
		}


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

	return 0;
}
void ShowHierarch() {
	ImGui::Begin("Hierarchy");
	ImGui::SetWindowSize(ImVec2(screenWidth * 0.2f, screenHeigth));
	ImGui::SetWindowPos(ImVec2(screenWidth * 0.8f, 0.0), false);
	ImGui::Text("Hello from hierarchy");
	if (ImGui::TreeNode("Selection State: Single Selection"))
	{
		static int selected = -1;
		for (int n = 0; n < editor->getObjectSize(); n++)
		{
			char buf[32];
			sprintf(buf, "Game Object %d", editor->scene.gameObjects[n].id);
			if (ImGui::Selectable(buf, selected == n)) {
				selected = n; // Selected code down there
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (editor->hasSelected()) {
		//int grid;
		//ImGui::InputInt("Grid", &grid);
		ImGui::Separator();
		int scaleX = (int) editor->selected->scaleX;
		int scaleY = (int) editor->selected->scaleY;
		int scaleZ = (int) editor->selected->scaleZ;

		if (ImGui::DragInt(": Scale X",&scaleX, 0.2)) {
			editor->selected->updateScale(scaleX, scaleY, scaleZ);
		}
		if (ImGui::DragInt(": Scale Y", &scaleY, 0.2)) {
			editor->selected->updateScale(scaleX, scaleY, scaleZ);
		}
		if (ImGui::DragInt(": Scale Z", &scaleZ, 0.2)) {
			editor->selected->updateScale(scaleX, scaleY, scaleZ);
		}
		ImGui::Separator();
		ImGui::Text("Translations");
		GameObject* sel = editor->selected;
		glm::vec3 trans = sel->getPos();
		int x =(int) trans.x;
		int y = (int)trans.y;
		int z = (int)trans.z;
		float mas = (float)editor->selected->collider.mass;
		if (ImGui::DragInt(": X", &x, 0.2f)) {
			editor->selected->updatePosition(x, y, z);
		}
		if (ImGui::DragInt(": Y", &y, 0.2)) {
			editor->selected->updatePosition(x, y, z);
		}
		if (ImGui::DragInt(": Z", &z, 0.2)) {
			editor->selected->updatePosition(x, y, z);
		}
		if (ImGui::DragFloat(": Mass", &mas, 0.2, 0.0f, 10.0f)) {
			editor->selected->collider.mass = mas;
			//editor->selected->collider.getNewRigidBody();
		}
		ImGui::Separator();
		ImGui::Text("Rotations");
		glm::vec3 rotation = editor->selected->rotation;
		int xx = (int)rotation.x;
		int yy = (int)rotation.y;
		int zz = (int)rotation.z;
		if (ImGui::DragInt(": x", &xx, 0.2)) {
			editor->selected->updateRotation(xx, yy, zz);
		}
		if (ImGui::DragInt(": y", &yy, 0.2)) {
			editor->selected->updateRotation(xx, yy, zz);
		}
		if (ImGui::DragInt(": z", &zz, 0.2)) {
			editor->selected->updateRotation(xx, yy, zz);
		}
		ImGui::Separator();
		ImGui::Text("Color");
		glm::vec4 color = editor->selected->color;
		float red = color.x;
		float green = color.y;
		float blue = color.z;
		float alpha = color.a;
		if (ImGui::DragFloat(": red", &red, 0.01f, 0.0f, 1.0f)) {
			editor->selected->updateColor(red, green, blue, alpha);
		}
		if (ImGui::DragFloat(": green", &green, 0.01f, 0.0f, 1.0f)) {
			editor->selected->updateColor(red, green, blue, alpha);
		}
		if (ImGui::DragFloat(": blue", &blue, 0.01f, 0.0f, 1.0f)) {
			editor->selected->updateColor(red, green, blue, alpha);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Wireframe mode", NULL))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (ImGui::MenuItem("Normal mode", NULL))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	ImGui::End();
}
void ShowExampleMenuFile()
{
	ImGui::MenuItem("(dummy menu)", NULL, false, false);
	ImGui::SetWindowPos(ImVec2(0, 0), false);
	ImGui::SetWindowSize(ImVec2(screenWidth * 0.2f, screenHeigth));
	if (ImGui::MenuItem("New")) {}
	if (ImGui::BeginMenu("Open", "Ctrl+O")) 
	{
		static char str0[128] = "scenes\\";
		ImGui::InputText(": scene name", str0, IM_ARRAYSIZE(str0));
		if (ImGui::MenuItem("Open File"))
		{
			openSceneFromFile(string(str0));
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S")) 
	{ 
		

	}
	if (ImGui::BeginMenu("Save As.."))
	{
		static char str0[128] = "scene_name";
		ImGui::InputText(": scene name", str0, IM_ARRAYSIZE(str0));
		if (ImGui::MenuItem("Save As.."))
		{
			saveCurrentSceneAs(str0);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Add Primitive")) 
	{
		if (ImGui::MenuItem("Add Cube")) {
			editor->generatePrimitive((PrimitiveType)cube);
		}
		if (ImGui::MenuItem("Add Sphere")) {
			editor->generatePrimitive((PrimitiveType)sphere);
		}
		ImGui::EndMenu();
	}
	ImGui::Separator();
	if (ImGui::BeginMenu("Gameplay")) 
	{
		if (ImGui::MenuItem("Editor mode")) {
			editor->generateGamePhysicsWorld(0); editor->gameActive = false;
			//scene.physicsWorld->setGravity(0); gameActive = false;
			cout << "call out my name";
		}
		if (ImGui::MenuItem("Gameplay mode")) {
			editor->generateGamePhysicsWorld(1); editor->gameActive = true;
			//scene.physicsWorld->setGravity(-10); gameActive = true;
		}
		if (ImGui::MenuItem("RemoteVR mode")) {
			
		}
		ImGui::EndMenu();
	}
	
	ImGui::Separator();
	
	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

void createGameObject(string const &modelPath, Shader shader, glm::vec3 position) {
	Model plane(modelPath);
	
	GameObject o1(position, 0); // shapeType 0
	o1.setModel(plane);
	o1.setShader(shader);

	Collider col;
	//col.setCollider(0, groundShape, groundMotionState, btVector3(position.x, position.y, position.z));

	o1.setPhysics(col);
	editor->addGameObject(o1);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*std::cout << "Mouse clicked: any";
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		std::cout << "Mouse clicked";*/
}
void processInput(GLFWwindow *window, float delta, btDiscreteDynamicsWorld* dynamicsWorld)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		editor->camera->ProcessKeyboard(FORWARD, delta);
	//if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		
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

	/*if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		std::cout << "MOuse clicked";
	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		std::cout << "MOuse clicked";*/

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		float x = (2.0f * xpos) / screenWIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / screenHEIGTH;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		//std::cout << "X: " << x << "Y: " << y << "Z: " << z << "\n";
		glm::vec4 ray_clip = glm::vec4(x, y, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(editor->projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0);
		glm::vec3 ray_wor = glm::vec3((glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).x, (glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).y, (glm::inverse(editor->camera->GetViewMatrix()) * ray_eye).z);
		//std::cout << "Normalised view world coordinates" << "X: " << ray_wor.x << "Y: " << ray_wor.y << "Z: " << ray_wor.z << "\n";
		
		float distance = 1000.0;
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
	//glViewport(0, 0, width, height);
	screenHeigth = height;
	screenWidth = width;
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
