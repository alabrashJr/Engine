#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include "gameobject.h"
#include "physicsworld.h"

class Scene {
public:
	// We implemented this physics world here for editor functions: Selecting object from scene
	// If bulletphysics works good then no worry to use physics for editor functions.
	// But same issue we must seperate editor and game physics world.
	// An editorphysics class could be implemented. When game engine works its works.
	// And editorphysics will help us for creating settings for gamephysics's physics world.
	// Gamephysics class will be a main compnents of our game. It will collobrate with game objects.
	std::vector<GameObject> gameObjects;
	PhysicsWorld* physicsWorld;

	Scene() {
		
	}
	Scene(int mod) {

	}
};
#endif