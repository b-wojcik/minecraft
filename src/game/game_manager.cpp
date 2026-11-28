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

	cameraProjView = cameraProjection * camera.getView();

	// draw the world
	ShaderCache::get().use(voxelProgram);

	ShaderCache::get().setMat4(
		voxelProgram, 
		"uProjView", 
		cameraProjView
	);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	world.draw();

	if (!drawHighlight) return;

	// draw the block highlight
	ShaderCache::get().use(highlightProgram);

	ShaderCache::get().setMat4(
		highlightProgram,
		"uProjView",
		cameraProjView
	);

	ShaderCache::get().setVec3(
		highlightProgram,
		"uPosition",
		highlightPos
	);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	highlightMesh.draw();
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

	glm::vec3 normal;

	drawHighlight = world.raycastForBlock(
		camera.getPosition(),
		camera.getDirection(),
		4.5f,
		highlightPos,
		normal
	);

	if (!drawHighlight) return;

	if (Input::get().didKeyClick(KeyCode::Mouse1)) {
		world.replaceBlock(highlightPos, BLOCK_AIR);
	}
	if (Input::get().didKeyClick(KeyCode::Mouse2)) {
		world.replaceBlock(highlightPos + normal, BLOCK_SNOW);
	}

	highlightPos += glm::vec3(0.5f);
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

	// enable depth test
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepth(0.0f);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.create(90.0f, 0.8f, 24.0f);
	cameraProjection = camera.getProjection();

	voxelProgram = ShaderCache::get().load(
		"assets/shaders/voxel.vert",
		"assets/shaders/voxel.frag"
	);

	highlightProgram = ShaderCache::get().load(
		"assets/shaders/highlight.vert",
		"assets/shaders/highlight.frag"
	);

	ShaderCache::get().use(voxelProgram);
	ShaderCache::get().setInt(voxelProgram, "uTexture", 0);

	loadTextures();

	// create the world
	float t = Window::get().time();
	world.create(camera);
	std::cout << "TOTAL TIME: " << Window::get().time() - t << '\n';

	highlightMesh.create();
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