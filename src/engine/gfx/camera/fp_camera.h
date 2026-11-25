#pragma once

#include "../../core/window.h"
#include "camera.h"

class FPCamera {
public:
	// does nothing if the first person camera has already been created
	void create(float fov, float sensitivity, float speed);

	// calculates look direction and handles movement
	// does nothing if the first person camera has not been created
	void update(float deltaTime);

	glm::vec3& getPosition();
	
	const glm::vec3& getDirection() const;

	const glm::mat4& getView() const;
	const glm::mat4& getProjection() const;

	FPCamera() = default;

	// delete copy and move constructors
	FPCamera(const FPCamera&) = delete;
	FPCamera(FPCamera&&) = delete;
private:
	Camera camera;

	// safety guards
	bool created = false;
	bool warningShown = false;

	// movement
	float sensitivity = 0.0f;
	float speed = 0.0f;

	// euler angles
	float yaw = -90.0f;
	float pitch = 0.0f;

	// cursor
	float lastX = 0.0f;
	float lastY = 0.0f;
	bool firstMouse = true;
};