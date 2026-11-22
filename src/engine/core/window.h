#pragma once

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "debug.h"

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
	static Window& get();

	// fullscreen mode is set to native and vsync is enabled by default
	// does nothing if the window has already been created
	void create(int width, int height, const char* title, bool resizable = false);

	// polls window events, clears color and depth buffers
	// does nothing if the window has not been created
	void beginFrame();

	// swaps the window buffers, displaying a new frame
	// does nothing if the window has not been created
	void endFrame();

	// sets that the window should close
	// does nothing if the window has not been created
	void close();

	// returns true if the window should close or has not been created
	[[nodiscard]]
	bool shouldClose() const;

	// overwrites the window color buffer
	// does nothing if the window has not been created
	void setClearColor(float r, float g, float b, float a = 1.0f) const;

	// sets fullscreen according to the current fullscreen mode
	// does nothing if the window has not been created
	void setFullscreen(bool value);

	bool isFullscreen() const;

	void setFullscreenMode(FullscreenMode mode);

	FullscreenMode getFullscreenMode() const;

	// even non-resizable window can go fullscreen
	// does nothing if the window has not been created
	void setResizable(bool value);

	bool isResizable() const;

	// does nothing if the window has not been created
	void setVsync(bool value);

	bool isVsync() const;

	// does nothing if the window is fullscreen or has not been created
	void setSize(int width, int height);

	// does nothing if the window is fullscreen or has not been created
	void setPos(int xpos, int ypos);

	// does nothing if the window has not been created
	void setTitle(const std::string& title);

	// returns an empty string if the window has not been created
	const char* getTitle() const;

	// does nothing if the window has not been created
	void hideCursor();

	// does nothing if the window has not been created
	void showCursor();

	// returns elapsed time since window was created
	float time() const;

	const WindowState& getState() const;

	// delete copy and move constructors
	Window(const Window&) = delete;
	Window(Window&&) = delete;
private:
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

	Window() = default;

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

	// update window settings
	void updateSize(int width, int height);

	void updateFramebufferSize(int width, int height);

	void updatePos(int xpos, int ypos);

	void setExclusiveFullscreen(bool value);

	void setBorderlessFullscreen(bool value);

	void createCallbacks();
};