#pragma once

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "input.hpp"
#include "debug.hpp"

// window settings
struct WindowState {
	// postion
	int xpos = 0;
	int ypos = 0;

	// size
	int width = 512;
	int height = 512;
};

enum class FullscreenMode {
	Native = 0,
	Borderless = 1
};

class Window {
public:
	static Window& get() {
		static Window instance;
		return instance;
	}

	// fullscreen mode is set to native and vsync is enabled by default
	// does nothing if the window has already been created
	void create(int width, int height, const char* title, bool resizable = false) {
		if (m_window) {
			PROGRAM_WARNING(
				"Window::create(): window has been created already"
			);
			return;
		}

		// initialize GLFW
		if (!glfwInit()) {
			PROGRAM_ERROR(
				"Window::create(): failed to initialize GLFW"
			);
		}
		m_glfwInitialized = true;

		// set OpenGL version to 4.4
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DEPTH_BITS, 32);
		glfwWindowHint(GLFW_SAMPLES, 4);

		// create GLFW window
		m_window = glfwCreateWindow(
			width,
			height,
			title,
			NULL,
			NULL
		);

		if (!m_window) {
			PROGRAM_ERROR(
				"Window::create(): failed to create an OpenGL 4.4 GLFW window"
			);
		}
		glfwMakeContextCurrent(m_window);

		// setup window settings
		glfwGetWindowPos(m_window, &m_currentState.xpos, &m_currentState.ypos);
		glfwGetWindowSize(m_window, &m_currentState.width, &m_currentState.height);

		m_preFullscreenState.xpos = m_currentState.xpos;
		m_preFullscreenState.ypos = m_currentState.ypos;
		m_preFullscreenState.width = m_currentState.width;
		m_preFullscreenState.height = m_currentState.height;

		glfwSwapInterval(m_vsync ? 1 : 0);
		setResizable(resizable);

		// load OpenGL 4.4
		if (!gladLoadGL(glfwGetProcAddress)) {
			PROGRAM_ERROR(
				"Window::create(): failed to initialize GLAD"
			);
		}

		// enable depth test
		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
			
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glClearDepth(0.0f);

		// create an appropriate viewport
		int framebufferWidth;
		int framebufferHeight;

		glfwGetFramebufferSize(
			m_window,
			&framebufferWidth,
			&framebufferHeight
		);

		glViewport(0, 0, framebufferWidth, framebufferHeight);

		// setup event handling
		Input::get().setWindowDimensions(m_currentState.width, m_currentState.height);
		createCallbacks();

		glfwPollEvents();
	}

	// polls window events, clears color and depth buffers
	// does nothing if the window has not been created
	void beginFrame() {
		if (!m_window) return;
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// swaps the window buffers, displaying a new frame
	// does nothing if the window has not been created
	void endFrame() {
		if (!m_window) return;
		glfwSwapBuffers(m_window);
	}

	// sets that the window should close
	// does nothing if the window has not been created
	void close() {
		if (!m_window) return;
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	// returns true if the window should close or has not been created
	[[nodiscard]]
	bool shouldClose() const {
		return !m_window || glfwWindowShouldClose(m_window);
	}

	// overwrites the window color buffer
	// does nothing if the window has not been created
	void setClearColor(float r, float g, float b, float a = 1.0f) const {
		if (!m_window) return;
		glClearColor(r, g, b, a);
	}

	// sets fullscreen according to the current fullscreen mode
	// does nothing if the window has not been created
	void setFullscreen(bool value) {
		if (!m_window || value == m_fullscreen) return;

		m_changingFullscreen = true;

		if (m_fullscreenMode == FullscreenMode::Native) {
			setExclusiveFullscreen(value);
		}
		else {
			setBorderlessFullscreen(value);
		}

		m_changingFullscreen = false;
		m_fullscreen = value;
	}

	bool isFullscreen() const {
		return m_fullscreen;
	}

	void setFullscreenMode(FullscreenMode mode) {
		if (mode == m_fullscreenMode) return;

		// if the window is in fullscreen, then that state will have to be re-enabled
		if (m_fullscreen) {
			setFullscreen(false);
			m_fullscreenMode = mode;
			setFullscreen(true);
				
			return;
		}

		m_fullscreenMode = mode;
	}

	FullscreenMode getFullscreenMode() const {
		return m_fullscreenMode;
	}

	// even non-resizable window can go fullscreen
	// does nothing if the window has not been created
	void setResizable(bool value) {
		if (!m_window) return;
		glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, value);
		m_resizable = value;
	}

	bool isResizable() const {
		return m_resizable;
	}

	// does nothing if the window has not been created
	void setVsync(bool value) {
		if (!m_window || value == m_vsync) return;

		glfwSwapInterval(value ? 1 : 0);
		m_vsync = value;
	}

	bool isVsync() const {
		return m_vsync;
	}

	// does nothing if the window is fullscreen or has not been created
	void setSize(int width, int height) {
		if (!m_window || m_fullscreen) return;
		glfwSetWindowSize(m_window, width, height);
	}

	// does nothing if the window is fullscreen or has not been created
	void setPos(int xpos, int ypos) {
		if (!m_window || m_fullscreen) return;
		glfwSetWindowPos(m_window, xpos, ypos);
	}

	// does nothing if the window has not been created
	void setTitle(const std::string& title) {
		if (!m_window) return;
		glfwSetWindowTitle(m_window, title.c_str());
	}

	// returns an empty string if the window has not been created
	const char* getTitle() const {
		if (!m_window) return "";
		return glfwGetWindowTitle(m_window);
	}

	// does nothing if the window has not been created
	void hideCursor() {
		if (!m_window) return;
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// does nothing if the window has not been created
	void showCursor() {
		if (!m_window) return;
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// returns elapsed time since glfw was initialized
	float time() const {
		if (!m_glfwInitialized) return 0.0f;
		return float(glfwGetTime());
	}

	const WindowState& getState() const {
		return m_currentState;
	}

	// delete copy and move constructors
	Window(const Window&) = delete;
	Window(Window&&) = delete;
private:
	~Window() {
		if (m_window) {
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}

		if (m_glfwInitialized) {
			glfwTerminate();
			m_glfwInitialized = false;
		}
	}
	Window() = default;
	
	WindowState m_currentState;
	WindowState m_preFullscreenState;

	FullscreenMode m_fullscreenMode = FullscreenMode::Native;
	bool m_changingFullscreen = false;
		
	bool m_fullscreen = false;
	bool m_resizable = false;
	bool m_vsync = true;

	// GLFW window handle
	GLFWwindow* m_window = nullptr;
	bool m_glfwInitialized = false;

	// update window settings
	void updateSize(int width, int height) {
		m_currentState.width = width;
		m_currentState.height = height;

		if (!m_fullscreen && !m_changingFullscreen) {
			m_preFullscreenState.width = width;
			m_preFullscreenState.height = height;
		}

		// notify Input
		Input::get().setWindowDimensions(width, height);
	}

	void updateFramebufferSize(int width, int height) {
		glViewport(0, 0, width, height);
	}

	void updatePos(int xpos, int ypos) {
		m_currentState.xpos = xpos;
		m_currentState.ypos = ypos;

		if (!m_fullscreen && !m_changingFullscreen) {
			m_preFullscreenState.xpos = xpos;
			m_preFullscreenState.ypos = ypos;
		}
	}

	void setExclusiveFullscreen(bool value) {
		if (value) {
			// save windowed state before entering fullscreen
			glfwGetWindowPos(
				m_window,
				&m_preFullscreenState.xpos,
				&m_preFullscreenState.ypos
			);

			glfwGetWindowSize(
				m_window,
				&m_preFullscreenState.width,
				&m_preFullscreenState.height
			);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(
				m_window,
				monitor,
				0,
				0,
				mode->width,
				mode->height,
				mode->refreshRate
			);
		}
		else {
			glfwSetWindowMonitor(
				m_window,
				nullptr,
				m_preFullscreenState.xpos,
				m_preFullscreenState.ypos,
				m_preFullscreenState.width,
				m_preFullscreenState.height,
				0
			);
		}
	}

	void setBorderlessFullscreen(bool value) {
		if (value) {
			// save windowed state before entering fullscreen
			glfwGetWindowPos(
				m_window,
				&m_preFullscreenState.xpos,
				&m_preFullscreenState.ypos
			);

			glfwGetWindowSize(
				m_window,
				&m_preFullscreenState.width,
				&m_preFullscreenState.height
			);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			int monitorX;
			int monitorY;
			glfwGetMonitorPos(monitor, &monitorX, &monitorY);
				
			glfwSetWindowAttrib(
				m_window, 
				GLFW_DECORATED, 
				GLFW_FALSE
			);
				
			glfwSetWindowPos(
				m_window, 
				monitorX, 
				monitorY
			);

			glfwSetWindowSize(
				m_window, 
				mode->width, 
				mode->height
			);
		}
		else {
			glfwSetWindowAttrib(
				m_window, 
				GLFW_DECORATED, 
				GLFW_TRUE
			);

			glfwSetWindowPos(
				m_window, 
				m_preFullscreenState.xpos, 
				m_preFullscreenState.ypos
			);

			glfwSetWindowSize(
				m_window, 
				m_preFullscreenState.width, 
				m_preFullscreenState.height
			);
		}
	}

	void createCallbacks() {
		// framebuffer size
		glfwSetFramebufferSizeCallback(
			m_window,
			[](GLFWwindow* window, int width, int height) {
				Window::get().updateFramebufferSize(width, height);
			}
		);
		// window size
		glfwSetWindowSizeCallback(
			m_window,
			[](GLFWwindow* window, int width, int height) {
				Window::get().updateSize(width, height);
			}
		);

		// window position
		glfwSetWindowPosCallback(
			m_window,
			[](GLFWwindow* window, int xpos, int ypos) {
				Window::get().updatePos(xpos, ypos);
			}
		);

		// key press
		glfwSetKeyCallback(
			m_window,
			[](GLFWwindow* windowObject, int key, int scancode, int action, int mods) {
				if (action == GLFW_RELEASE) {
					Input::get().setKeyState(key, KeyState::Release);
				}
				else if (action == GLFW_PRESS) {
					Input::get().setKeyState(key, KeyState::Click);
				}
			}
		);

		// mouse button press
		glfwSetMouseButtonCallback(
			m_window,
			[](GLFWwindow* window, int button, int action, int mods) {
				if (action == GLFW_RELEASE) {
					Input::get().setMouseButtonState(button, KeyState::Release);
				}
				else if (action == GLFW_PRESS) {
					Input::get().setMouseButtonState(button, KeyState::Click);
				}
			}
		);

		// cursor position
		glfwSetCursorPosCallback(
			m_window,
			[](GLFWwindow* window, double xpos, double ypos) {
				Input::get().setCursorPos(float(xpos), float(ypos));
			}
		);
	}
};