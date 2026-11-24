#pragma once

#include "../engine/framework.h"
#include "world/world.h"

class GameManager {
public:
	GameManager();
	void run();
private:
	glm::mat4 cameraProjection;
	glm::mat4 cameraProjView;
	FPCamera camera;

	// shaders
	ShaderProgram voxelProgram;
	ShaderProgram highlightProgram;

	glm::vec3 highlightPos;
	bool drawHighlight;

	CubeMesh highlightMesh;

	World world;

	void update(float deltatime);
	void draw();

	void initialize();
	void loadTextures();
};