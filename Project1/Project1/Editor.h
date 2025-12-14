#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stack>
#include <iostream>
#include "ChartManager.h"
#include "Renderer.h"
#include "Command.h"
#include "GameConfig.h"

class Editor {
private:
    // [핵심 모듈]
    ChartManager chartManager;
    Renderer renderer;

    // [Undo/Redo 스택]
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;

    // [상태 변수]
    sf::Music bgm;
    bool isPlaying;
    double musicTime;      // 현재 재생 시간 (ms)
    float scrollSpeed;     // 스크롤 속도
    bool isGameMode;       // 게임/에디터 모드 전환

    // [입력 상태]
    bool isMouseDown;
    sf::Vector2i startMousePos;
    int currentLane;

    void processCommand(Command* cmd);
    void clearStacks();

public:
    Editor();
    ~Editor(); // 스택 정리 필요없음 (unique_ptr)

    void handleInput(sf::RenderWindow& window, sf::Event& event);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
};