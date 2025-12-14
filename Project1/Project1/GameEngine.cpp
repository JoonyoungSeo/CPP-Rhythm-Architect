#include "GameEngine.h"
#include "GameConfig.h"

GameEngine::GameEngine() {
    // 설정 파일의 상수값을 사용하여 윈도우 생성
    window.create(sf::VideoMode(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT),
        GameConfig::WINDOW_TITLE);
    window.setFramerateLimit(GameConfig::FRAME_LIMIT);
}

void GameEngine::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        // 1. 이벤트 처리 (입력)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // 입력 처리는 에디터에게 위임
            editor.handleInput(window, event);
        }

        // 2. 로직 업데이트
        editor.update(dt);

        // 3. 렌더링
        window.clear(GameConfig::BG_COLOR); // 배경색 상수 사용
        editor.draw(window);
        window.display();
    }
}