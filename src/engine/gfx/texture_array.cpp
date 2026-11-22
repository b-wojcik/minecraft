#include "texture_array.h"

TextureArray& TextureArray::get() {
	static TextureArray instance;
	return instance;
}

void TextureArray::create() {
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	// 192 layers for 16x16 textures
	glTexStorage3D(
		GL_TEXTURE_2D_ARRAY,
		5,
		GL_SRGB8_ALPHA8,
		16,
		16,
		192
	);

	// setup wrapping and filtering
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void TextureArray::loadTexture(const char* filepath, int layer) {
	stbi_set_flip_vertically_on_load(true);

	// load texture data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 4);

	if (!data) {
		PROGRAM_WARNING(
			"TextureArray::load(): failed to load image data from:\n",
			"=> ",
			filepath
		);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	// upload texture data to the array
	glTexSubImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, layer,
		16, 16, 1,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);

	stbi_image_free(data);
}

void TextureArray::generateMipmaps() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}