#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <stack>
#include <string>
#include "Note.h"
#include "Command.h"
#include "TimingPoint.h"

class Editor {
private:
    // 데이터
    std::vector<Note*> notes;
    std::vector<TimingPoint> timingPoints;
    std::stack<Command*> undoStack;
    std::stack<Command*> redoStack;

    // 상태 변수
    sf::Music bgm;
    bool isMusicLoaded;
    double musicTime;
    bool isPlaying;
    float scrollSpeed;
    int snapDiv;
    bool isGameMode;

    // 마우스 입력
    bool isMouseDown;
    sf::Vector2i startMousePos;
    int currentLane;

    // 상수
    const float JUDGMENT_LINE_Y = 600.0f;
    const float START_X = 400.0f;
    const float LANE_WIDTH = 120.0f;

    // 헬퍼
    double quantize(double rawTime);
    void clearStack(std::stack<Command*>& s);
    TimingPoint getCurrentTimingPoint(double time);

public:
    Editor();
    ~Editor();

    void handleInput(sf::RenderWindow& window, sf::Event& event);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);

    void saveProject(const std::string& filename);
    void loadProject(const std::string& filename);
};