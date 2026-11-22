#pragma once

#include <glad/gl.h>

#include "../core/debug.h"
#include "../../third_party/stb_image.h"

class TextureArray {
public:
	static TextureArray& get();

	// creates a texture object
	void create();

	// adds a texture to the texture object
	void loadTexture(const char* filepath, int layer);

	// generates mipmaps for all textures in the texture object
	void generateMipmaps();

	// delete copy and move constructors
	TextureArray(const TextureArray&) = delete;
	TextureArray(TextureArray&&) = delete;
private:
	// texture object handle
	GLuint textureArray;

	TextureArray() = default;
};