#include "GameEngine.h"

GameEngine::GameEngine() {
    window.create(sf::VideoMode(1280, 720), "Rhythm Note Architect - Final");
    window.setFramerateLimit(60);
}

void GameEngine::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            editor.handleInput(window, event);
        }
        editor.update(dt);
        window.clear(sf::Color::Black);
        editor.draw(window);
        window.display();
    }
}