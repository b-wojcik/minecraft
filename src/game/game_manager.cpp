#include "game_manager.h"

GameManager::GameManager() {
	initialize();
}

void GameManager::run() {
	float deltaTime;
	float lastTime = Window::get().time();
	float currentTime;

	while (!Window::get().shouldClose()) {
		currentTime = Window::get().time();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		Window::get().beginFrame();

		update(deltaTime);
		draw();

		Window::get().endFrame();
	}
}

void GameManager::draw() {
	Window::get().setClearColor(0.39f, 0.70f, 0.96f);

	ShaderCache::get().use(shaderProgram);
	ShaderCache::get().setMat4(shaderProgram, "uView", camera.getView());

	world.draw();
}

void GameManager::update(float deltaTime) {
	if (Input::get().didKeyClick(KeyCode::Digit1)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (Input::get().didKeyClick(KeyCode::Digit2)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (Input::get().didKeyClick(KeyCode::Escape)) {
		Window::get().close();
	}
	if (Input::get().didKeyClick(KeyCode::F11)) {
		Window::get().setFullscreen(
			!Window::get().isFullscreen()
		);
	}

	camera.update(deltaTime);
}

void GameManager::initialize() {
	Window::get().create(1920, 1080, "Voxel");
	Window::get().setVsync(false);
	Window::get().setFullscreenMode(FullscreenMode::Native);

	// gamma correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	// face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// MSAA
	glEnable(GL_MULTISAMPLE);

	camera.create(90.0f, 0.8f, 64.0f);

	shaderProgram = ShaderCache::get().load(
		"assets/shaders/voxel.vert",
		"assets/shaders/voxel.frag"
	);

	ShaderCache::get().use(shaderProgram);
	ShaderCache::get().setMat4(shaderProgram, "uProjection", camera.getProjection());
	ShaderCache::get().setInt(shaderProgram, "uFaceTexture", 0);

	loadTextures();

	// create the world
	float t = Window::get().time();
	world.create(camera);
	std::cout << "TOTAL TIME: " << Window::get().time() - t << '\n';
}

void GameManager::loadTextures() {
	TextureArray::get().create();

	// grass
	TextureArray::get().loadTexture("assets/textures/grass_side.png", 3 * BLOCK_GRASS);
	TextureArray::get().loadTexture("assets/textures/dirt.png", 3 * BLOCK_GRASS + 1);
	TextureArray::get().loadTexture("assets/textures/grass_top.png", 3 * BLOCK_GRASS + 2);

	// dirt
	TextureArray::get().loadTexture("assets/textures/dirt.png", 3 * BLOCK_DIRT);
	TextureArray::get().loadTexture("assets/textures/dirt.png", 3 * BLOCK_DIRT + 1);
	TextureArray::get().loadTexture("assets/textures/dirt.png", 3 * BLOCK_DIRT + 2);

	// stone
	TextureArray::get().loadTexture("assets/textures/stone.png", 3 * BLOCK_STONE);
	TextureArray::get().loadTexture("assets/textures/stone.png", 3 * BLOCK_STONE + 1);
	TextureArray::get().loadTexture("assets/textures/stone.png", 3 * BLOCK_STONE + 2);

	// snow
	TextureArray::get().loadTexture("assets/textures/snow.png", 3 * BLOCK_SNOW);
	TextureArray::get().loadTexture("assets/textures/snow.png", 3 * BLOCK_SNOW + 1);
	TextureArray::get().loadTexture("assets/textures/snow.png", 3 * BLOCK_SNOW + 2);

	TextureArray::get().generateMipmaps();
}