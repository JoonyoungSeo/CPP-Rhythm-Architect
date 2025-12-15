#pragma once
#include <SFML/Graphics.hpp>
#include "Editor.h"

class GameEngine {
private:
    sf::RenderWindow window;
    Editor editor;

public:
    GameEngine();
    void run();
};