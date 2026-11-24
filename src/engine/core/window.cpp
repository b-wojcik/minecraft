#include "window.h"

Window& Window::get() {
	static Window instance;
	return instance;
}

void Window::create(int width, int height, const char* title, bool resizable) {
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

	// depth buffer and MSAA sample count
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

void Window::beginFrame() {
	if (!m_window) return;
	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::endFrame() {
	if (!m_window) return;
	glfwSwapBuffers(m_window);
}

void Window::close() {
	if (!m_window) return;
	glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

[[nodiscard]]
bool Window::shouldClose() const {
	return !m_window || glfwWindowShouldClose(m_window);
}

void Window::setClearColor(float r, float g, float b, float a) const {
	if (!m_window) return;
	glClearColor(r, g, b, a);
}

void Window::setFullscreen(bool value) {
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

bool Window::isFullscreen() const {
	return m_fullscreen;
}

void Window::setFullscreenMode(FullscreenMode mode) {
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

FullscreenMode Window::getFullscreenMode() const {
	return m_fullscreenMode;
}

void Window::setResizable(bool value) {
	if (!m_window) return;
	glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, value);
	m_resizable = value;
}

bool Window::isResizable() const {
	return m_resizable;
}

void Window::setVsync(bool value) {
	if (!m_window || value == m_vsync) return;

	glfwSwapInterval(value ? 1 : 0);
	m_vsync = value;
}

bool Window::isVsync() const {
	return m_vsync;
}

void Window::setSize(int width, int height) {
	if (!m_window || m_fullscreen) return;
	glfwSetWindowSize(m_window, width, height);
}

void Window::setPos(int xpos, int ypos) {
	if (!m_window || m_fullscreen) return;
	glfwSetWindowPos(m_window, xpos, ypos);
}

void Window::setTitle(const std::string& title) {
	if (!m_window) return;
	glfwSetWindowTitle(m_window, title.c_str());
}

const char* Window::getTitle() const {
	if (!m_window) return "";
	return glfwGetWindowTitle(m_window);
}

void Window::hideCursor() {
	if (!m_window) return;
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::showCursor() {
	if (!m_window) return;
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

float Window::time() const {
	if (!m_glfwInitialized) return 0.0f;
	return float(glfwGetTime());
}

const WindowState& Window::getState() const {
	return m_currentState;
}

// update window settings
void Window::updateSize(int width, int height) {
	m_currentState.width = width;
	m_currentState.height = height;

	if (!m_fullscreen && !m_changingFullscreen) {
		m_preFullscreenState.width = width;
		m_preFullscreenState.height = height;
	}

	// notify Input
	Input::get().setWindowDimensions(width, height);
}

void Window::updateFramebufferSize(int width, int height) {
	glViewport(0, 0, width, height);
}

void Window::updatePos(int xpos, int ypos) {
	m_currentState.xpos = xpos;
	m_currentState.ypos = ypos;

	if (!m_fullscreen && !m_changingFullscreen) {
		m_preFullscreenState.xpos = xpos;
		m_preFullscreenState.ypos = ypos;
	}
}

void Window::setExclusiveFullscreen(bool value) {
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

void Window::setBorderlessFullscreen(bool value) {
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

void Window::createCallbacks() {
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