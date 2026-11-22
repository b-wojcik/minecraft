#pragma once

#include "../engine/framework.h"
#include "world/world.h"

class GameManager {
public:
	GameManager();
	void run();
private:
	FPCamera camera;
	ShaderProgram shaderProgram;

	World world;

	void update(float deltatime);
	void draw();

	void initialize();
	void loadTextures();
};