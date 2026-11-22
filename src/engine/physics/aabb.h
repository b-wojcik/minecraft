#pragma once

#include <glm/glm.hpp>

class AABB {
public:
	// position of (0 , 0 , 0) is (left , bottom , back) by convention
	AABB(const glm::vec3& position, const glm::vec3& size);

	// returns true for points inside and on the edges
	bool isPointInside(const glm::vec3& point) const;

	void setPosition(const glm::vec3& position);
	void setSize(const glm::vec3& size);

	const glm::vec3& getMin() const;
	const glm::vec3& getMax() const;

	AABB() = default;
private:
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 max;
};