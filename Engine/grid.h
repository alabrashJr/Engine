#ifndef GRID_H
#define GRID_H

#include <GLEW/glew.h> // holds all OpenGL type declarations

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define WIDTH 50

using namespace std;

class Grid {
public:
	float gridNum;
	
	Grid() {
		setupMeshes();
	}

	void draw() {
		for (int i = -WIDTH; i < WIDTH; i++)
		{
			glBegin(GL_LINES);
			glVertex3f(i, 0, 50);
			glVertex3f(i, 0, -50);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(50, 0, i);
			glVertex3f(-50, 0, i);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glEnd();
		}
		
	}
	void setupMeshes() {
		
	}
};
#endif