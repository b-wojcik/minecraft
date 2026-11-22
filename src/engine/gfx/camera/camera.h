#pragma once

#include <glm/glm.hpp>

struct Camera {
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 position;

	glm::mat4 view;
	glm::mat4 projection;

	// creates a perspective camera looking in the -Z direction
	// meant for a reverse-z depth buffer
	void createPerspective(
		float fov,
		float near,
		float aspectRatio
	);

	// creates an orthographic camera looking in the -Z direction
	void createOrthographic(
		float left,
		float right,
		float bottom,
		float top,
		float near,
		float far
	);

	// should be called after transformations
	void updateView();
};