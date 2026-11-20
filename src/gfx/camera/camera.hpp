#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	) {
		position = glm::vec3(0.0f);

		direction = glm::vec3(0.0f, 0.0f, -1.0f);

		right = glm::normalize(
			glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -direction)
		);

		up = glm::normalize(
			glm::cross(-direction, right)
		);

		updateView();

		// https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
		// math magic

		float f = 1.0f / tan(glm::radians(fov) * 0.5f);
		projection = glm::mat4(0.0f);
			
		projection[0][0] = f / aspectRatio;
		projection[1][1] = f;
		projection[2][3] = -1.0f;
		projection[3][2] = near;
	}

	// creates an orthographic camera looking in the -Z direction
	void createOrthographic(
		float left, 
		float right, 
		float bottom, 
		float top,
		float near = 0.1f,
		float far = 10.0f
	) {
		position = glm::vec3(0.0f);
		
		// initialize 3d navigation vectors 
		direction = glm::vec3(0.0f, 0.0f, -1.0f);

		Camera::right = glm::vec3(1.0f, 0.0f, 0.0f);
		Camera::up = glm::vec3(0.0f, 1.0f, 0.0f);

		// initialize matrices
		updateView();
		projection = glm::ortho(left, right, bottom, top, near, far);
	}

	// should be called after transformations
	void updateView() {
		view = glm::lookAt(
			position, 
			position + direction, 
			up
		);
	}
};