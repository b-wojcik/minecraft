#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

typedef GLuint ShaderProgram;

class ShaderCache {
public:
	static ShaderCache& get();

	// links a shader program from vertex and fragment shaders
	// returns its handle on success
	[[nodiscard]]
	ShaderProgram load(const char* vertexFilePath, const char* fragmentFilePath);

	// tries to use the program
	void use(ShaderProgram program);

	// invalidates the cached program
	// should be used when glUseProgram() is called externally
	void invalidate();

	// creates a std140 global shader uniform buffer for a binding
	void createGlobalUniform(unsigned int binding, unsigned int bufferSize, bool dynamicDraw);

	// updates a global shader uniform buffer for a binding
	void updateGlobalUniform(unsigned int binding, unsigned int bufferSize, const void* data);

	// sets the value of a uniform variable
	void setBool(ShaderProgram program, const char* name, bool value);

	// sets the value of a uniform variable
	void setInt(ShaderProgram program, const char* name, int value);

	// sets the value of a uniform variable
	void setFloat(ShaderProgram program, const char* name, float value);

	// sets the value of a uniform variable
	void setVec2(ShaderProgram program, const char* name, const GLfloat* ptr);

	// sets the value of a uniform variable
	void setVec2(ShaderProgram program, const char* name, const glm::vec2& vector);

	// sets the value of a uniform variable
	void setVec3(ShaderProgram program, const char* name, const GLfloat* ptr);

	// sets the value of a uniform variable
	void setVec3(ShaderProgram program, const char* name, const glm::vec3& vector);

	// sets the value of a uniform variable
	void setVec4(ShaderProgram program, const char* name, const GLfloat* ptr);

	// sets the value of a uniform variable
	void setVec4(ShaderProgram program, const char* name, const glm::vec4& vector);

	// sets the value of a uniform variable
	void setMat3(ShaderProgram program, const char* name, const GLfloat* ptr);

	// sets the value of a uniform variable
	void setMat3(ShaderProgram program, const char* name, const glm::mat3& matrix);

	// sets the value of a uniform variable
	void setMat4(ShaderProgram program, const char* name, const GLfloat* ptr);

	// sets the value of a uniform variable
	void setMat4(ShaderProgram program, const char* name, const glm::mat4& matrix);

	// delete copy and move constructors
	ShaderCache(const ShaderCache&) = delete;
	ShaderCache(ShaderCache&&) = delete;
private:
	bool m_isProgramValid = false;
	ShaderProgram m_lastProgram = 0;

	// global Uniform Buffer Objects
	const GLuint UBO_INVALID = 0x3f3f3f3f;
	std::array<GLuint, 4> UBOs;

	ShaderCache();

	~ShaderCache() = default;
};