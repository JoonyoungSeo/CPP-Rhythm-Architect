#pragma once
#include <SFML/Graphics.hpp>
#include "Editor.h" // 에디터를 소유함

class GameEngine {
private:
    sf::RenderWindow window;
    Editor editor; // [포함 관계] 엔진은 에디터를 가진다.

public:
    GameEngine();
    void run(); // 메인 루프 실행
};