#pragma once

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

#include "../core/debug.hpp"

typedef GLuint ShaderProgram;

class ShaderCache {
public:
	static ShaderCache& get() {
		static ShaderCache instance;
		return instance;
	}

	// links a shader program from vertex and fragment shaders
	// returns its handle on success
	[[nodiscard]]
	ShaderProgram load(const char* vertexFilePath, const char* fragmentFilePath) {
		bool openFailed = false;

		// try to open the vertex shader file
		std::ifstream vertexFile(vertexFilePath);
		if (!vertexFile.is_open()) {
			PROGRAM_WARNING(
				"ShaderCache::load(): failed to open vertex shader file:\n",
				"=> ",
				vertexFilePath
			);

			openFailed = true;
		}

		// try to open the fragment shader file
		std::ifstream fragmentFile(fragmentFilePath);
		if (!fragmentFile.is_open()) {
			PROGRAM_WARNING(
				"ShaderCache::load(): failed to open fragment shader file:\n",
				"=> ",
				fragmentFilePath
			);

			openFailed = true;
		}

		if (openFailed) {
			PROGRAM_ERROR(
				"ShaderCache::load(): failed to load a shader program\n"
			);

			return 0;
		}

		// used for error checking
		int success;
		char infoLog[512];
		bool compilationFailed = false;

		// load vertex shader source code
		std::stringstream vertexShaderBuffer;
		vertexShaderBuffer << vertexFile.rdbuf();
		vertexFile.close();

		std::string vertexSource = vertexShaderBuffer.str();
		const char* vertexSourcePtr = vertexSource.c_str();

		// compile the vertex shader
		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSourcePtr, NULL);
		glCompileShader(vertexShader);

		// check for errors
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

			PROGRAM_WARNING(
				"ShaderCache::load(): vertex shader failed to compile:\n",
				"=> ",
				vertexFilePath,
				'\n',
				infoLog
			);

			compilationFailed = true;
		}

		// load fragment shader source code
		std::stringstream fragmentShaderBuffer;
		fragmentShaderBuffer << fragmentFile.rdbuf();
		fragmentFile.close();

		std::string fragmentSource = fragmentShaderBuffer.str();
		const char* fragmentSourcePtr = fragmentSource.c_str();

		// compile the fragment shader
		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSourcePtr, NULL);
		glCompileShader(fragmentShader);

		// check for errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

			PROGRAM_WARNING(
				"ShaderCache::load(): fragment shader failed to compile:\n",
				"=> ",
				fragmentFilePath,
				'\n',
				infoLog
			);

			compilationFailed = true;
		}

		if (compilationFailed) {
			PROGRAM_ERROR(
				"ShaderCache::load(): failed to load a shader program\n"
			);

			return 0;
		}

		// link the shader program
		ShaderProgram program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		// cleanup
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// check for errors
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);

			PROGRAM_ERROR(
				"ShaderCache::load(): shader program failed to link:\n",
				"=> ",
				vertexFilePath,
				'\n',
				"=> ",
				fragmentFilePath,
				'\n',
				infoLog
			);

			return 0;
		}

		return program;
	}

	// tries to use the program
	void use(ShaderProgram program) {
		if (program == m_lastProgram && m_isProgramValid) {
			return;
		}

		glUseProgram(program);

		m_isProgramValid = true;
		m_lastProgram = program;
	}

	// invalidates the cached program
	// should be used when glUseProgram() is called externally
	void invalidate() {
		m_isProgramValid = false;
	}

	// creates a std140 global shader uniform buffer for a binding
	void createGlobalUniform(unsigned int binding, unsigned int bufferSize, bool dynamicDraw) {
		if (binding > 3) {
			PROGRAM_ERROR(
				"ShaderCache::createGlobalUniform(): binding out of bounds [0 , 3]:\n",
				"=> ",
				binding
			);
			return;
		}
			
		// create buffer
		GLuint UBO;
		glGenBuffers(1, &UBO);
			
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(
			GL_UNIFORM_BUFFER, 
			bufferSize,
			(void*)0,
			dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
		);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, UBO);

		// store the UBO
		UBOs[binding] = UBO;
	}

	// updates a global shader uniform buffer for a binding
	void updateGlobalUniform(unsigned int binding, unsigned int bufferSize, const void* data) {
		if (binding > 3) {
			PROGRAM_ERROR(
				"ShaderCache::updateGlobalUniform(): binding out of bounds [0 , 3]:\n",
				"=> ",
				binding
			);
			return;
		}

		// fail when buffer was not created
		if (UBOs[binding] == UBO_INVALID) {
			PROGRAM_ERROR(
				"ShaderCache::updateGlobalUniform(): buffer not created for binding:\n",
				"=> ",
				binding
			);
			return;
		}

		// update buffer data
		glBindBuffer(GL_UNIFORM_BUFFER, UBOs[binding]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, data);
	}

	// sets the value of a uniform variable
	void setBool(ShaderProgram program, const char* name, bool value) {
		glProgramUniform1i(
			program,
			glGetUniformLocation(program, name),
			int(value)
		);
	}

	// sets the value of a uniform variable
	void setInt(ShaderProgram program, const char* name, int value) {
		glProgramUniform1i(
			program,
			glGetUniformLocation(program, name),
			value
		);
	}

	// sets the value of a uniform variable
	void setFloat(ShaderProgram program, const char* name, float value) {
		glProgramUniform1f(
			program,
			glGetUniformLocation(program, name),
			value
		);
	}

	// sets the value of a uniform variable
	void setVec2(ShaderProgram program, const char* name, const GLfloat* ptr) {
		glProgramUniform2fv(
			program,
			glGetUniformLocation(program, name),
			1,
			ptr
		);
	}

	// sets the value of a uniform variable
	void setVec2(ShaderProgram program, const char* name, const glm::vec2& vector) {
		glProgramUniform2fv(
			program,
			glGetUniformLocation(program, name),
			1,
			glm::value_ptr(vector)
		);
	}

	// sets the value of a uniform variable
	void setVec3(ShaderProgram program, const char* name, const GLfloat* ptr) {
		glProgramUniform3fv(
			program,
			glGetUniformLocation(program, name),
			1,
			ptr
		);
	}

	// sets the value of a uniform variable
	void setVec3(ShaderProgram program, const char* name, const glm::vec3& vector) {
		glProgramUniform3fv(
			program,
			glGetUniformLocation(program, name),
			1,
			glm::value_ptr(vector)
		);
	}

	// sets the value of a uniform variable
	void setVec4(ShaderProgram program, const char* name, const GLfloat* ptr) {
		glProgramUniform4fv(
			program,
			glGetUniformLocation(program, name),
			1,
			ptr
		);
	}

	// sets the value of a uniform variable
	void setVec4(ShaderProgram program, const char* name, const glm::vec4& vector) {
		glProgramUniform4fv(
			program,
			glGetUniformLocation(program, name),
			1,
			glm::value_ptr(vector)
		);
	}

	// sets the value of a uniform variable
	void setMat3(ShaderProgram program, const char* name, const GLfloat* ptr) {
		glProgramUniformMatrix3fv(
			program,
			glGetUniformLocation(program, name),
			1,
			GL_FALSE,
			ptr
		);
	}

	// sets the value of a uniform variable
	void setMat3(ShaderProgram program, const char* name, const glm::mat3& matrix) {
		glProgramUniformMatrix3fv(
			program,
			glGetUniformLocation(program, name),
			1,
			GL_FALSE,
			glm::value_ptr(matrix)
		);
	}

	// sets the value of a uniform variable
	void setMat4(ShaderProgram program, const char* name, const GLfloat* ptr) {
		glProgramUniformMatrix4fv(
			program,
			glGetUniformLocation(program, name),
			1,
			GL_FALSE,
			ptr
		);
	}

	// sets the value of a uniform variable
	void setMat4(ShaderProgram program, const char* name, const glm::mat4& matrix) {
		glProgramUniformMatrix4fv(
			program,
			glGetUniformLocation(program, name),
			1,
			GL_FALSE,
			glm::value_ptr(matrix)
		);
	}

	// delete copy and move constructors
	ShaderCache(const ShaderCache&) = delete;
	ShaderCache(ShaderCache&&) = delete;
private:
	bool m_isProgramValid = false;
	ShaderProgram m_lastProgram = 0;

	// global Uniform Buffer Objects
	const GLuint UBO_INVALID = 0x3f3f3f3f;
	std::array<GLuint, 4> UBOs;

	ShaderCache() {
		UBOs.fill(UBO_INVALID);
	}

	~ShaderCache() = default;
};