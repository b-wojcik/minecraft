#define PROGRAM_DEBUG

#include "world/world.hpp"
#include "framework.h"

int main() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);

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

	ShaderProgram program = ShaderCache::get().load(
		"assets/shaders/voxel.vert",
		"assets/shaders/voxel.frag"
	);

	FPCamera camera;
	camera.create(90.0f, 0.8f, 64.0f);

	float deltaTime;
	float lastTime = Window::get().time();
	float currentTime;

	ShaderCache::get().use(program);
	ShaderCache::get().setMat4(program, "uProjection", camera.getProjection());
	ShaderCache::get().setInt(program, "uFaceTexture", 0);

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

	// create the world
	float t = Window::get().time();
	World world;
	world.create(camera);
	std::cout << "TOTAL TIME: " << Window::get().time() - t << '\n';

	while (!Window::get().shouldClose()) {
		currentTime = Window::get().time();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		Window::get().beginFrame();

		Window::get().setClearColor(0.39f, 0.70f, 0.96f);
		
		camera.update(deltaTime);

		ShaderCache::get().use(program);
		ShaderCache::get().setMat4(program, "uView", camera.getView());

		world.draw();

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

		Window::get().endFrame();
	}
}