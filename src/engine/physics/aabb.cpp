#include "aabb.h"

#include "../core/debug.h"

AABB::AABB(const glm::vec3& position, const glm::vec3& size) :
	position(position), size(size) {
	max = position + size;

	if (size.x < 0.0f || size.y < 0.0f || size.z < 0.0f) {
		PROGRAM_WARNING(
			"AABB::AABB(): size has negative components:\n",
			"=> ",
			"(", size.x, ", ", size.y, ", ", size.z, ")"
		);
	}
}

bool AABB::isPointInside(const glm::vec3& point) const {
	return (
		point.x >= position.x &&
		point.x <= max.x &&
		point.y >= position.y &&
		point.y <= max.y &&
		point.z >= position.z &&
		point.z <= max.z
		);
}

void AABB::setPosition(const glm::vec3& position) {
	AABB::position = position;
	max = position + size;
}

void AABB::setSize(const glm::vec3& size) {
	AABB::size = size;
	max = position + size;
}

const glm::vec3& AABB::getMin() const {
	return position;
}

const glm::vec3& AABB::getMax() const {
	return max;
}