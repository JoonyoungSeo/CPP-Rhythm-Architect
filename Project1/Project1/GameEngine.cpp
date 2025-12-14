#include "GameEngine.h"

GameEngine::GameEngine() {
    // 윈도우 초기화
    window.create(sf::VideoMode(1280, 720), "Rhythm Note Architect - Refactored");
    window.setFramerateLimit(60);
}

void GameEngine::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        // 1. 이벤트 처리
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // 입력 처리는 에디터에게 위임 (Delegation)
            editor.handleInput(window, event);
        }

        // 2. 업데이트
        editor.update(dt);

        // 3. 렌더링
        window.clear(sf::Color::Black);
        editor.draw(window); // 에디터야, 화면에 그려라.
        window.display();
    }
}