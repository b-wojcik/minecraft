#include <iostream>
#include "game/game_manager.h"

int main() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);

	GameManager manager;
	manager.run();
}