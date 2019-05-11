#ifndef SERVER_H
#define SERVER_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "crow_all.h"

class Editor;

class Server
{
public :
	Editor* editor;
	
	int x = 0;
	int y = 0;
	int z = 0;

	glm::vec3 getSensorData() {
		return glm::vec3((float)x, (float)y, (float)z);
	}

	void startServer()
	{
		crow::SimpleApp app;

		CROW_ROUTE(app, "/")([]() {
			return "engineremotevrserver";
		});

		CROW_ROUTE(app, "/sensor")
			.methods("POST"_method)
			([&](const crow::request& req) {
			auto response = crow::json::load(req.body);
			
			if (!response)
				return crow::response(400);
			
			crow::json::rvalue sensor = response["sensor"];

			x = sensor["x"].i();
			y = sensor["y"].i();
			z = sensor["z"].i();

			std::ostringstream os;
			os << "x:";
			os << x;
			os << "y:";
			os << y;
			os << "z:";
			os << z;
			return crow::response{ os.str() };
		});

		app.loglevel(crow::LogLevel::Warning);

		app.port(18080).multithreaded().run();
	}

};
#endif
