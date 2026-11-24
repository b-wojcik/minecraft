#pragma once

#include <glad/gl.h>

class CubeMesh {
public:
	CubeMesh() = default;

	void create();
	void draw();

	GLuint getVAO() const;
private:
	GLuint VAO;
};