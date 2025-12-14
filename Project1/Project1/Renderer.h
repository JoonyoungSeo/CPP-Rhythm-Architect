#pragma once
#include <SFML/Graphics.hpp>
#include "ChartManager.h"
#include "GameConfig.h"

// [리팩토링] 그리기 전담 클래스 (SRP 준수)
// Editor나 GameEngine이 이 클래스를 멤버로 가짐
class Renderer {
private:
    sf::RectangleShape laneLine;
    sf::RectangleShape judgmentLine;
    sf::RectangleShape beatLine;
    sf::Font mainFont;
    sf::Text infoText;

public:
    Renderer() {
        // 리소스 로드 (실제로는 ResourceManager 등을 쓰는게 좋음)
        if (!mainFont.loadFromFile("tuffy.ttf")) {
            // 폰트 로드 실패 처리
        }
        infoText.setFont(mainFont);
        infoText.setCharacterSize(16);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(10, 10);
    }

    void drawGameScreen(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed) {
        // 1. 레인 그리기
        drawLanes(window);

        // 2. 비트 그리드 그리기 (에디터 모드에서 중요)
        drawGrid(window, chart, musicTime, scrollSpeed);

        // 3. 판정선 그리기
        judgmentLine.setSize(sf::Vector2f(GameConfig::WINDOW_WIDTH, 4.0f));
        judgmentLine.setFillColor(GameConfig::JUDGMENT_LINE_COLOR);
        judgmentLine.setOrigin(0, 2.0f); // 중앙 정렬
        judgmentLine.setPosition(0, GameConfig::JUDGMENT_LINE_Y);
        window.draw(judgmentLine);

        // 4. 노트 그리기
        // ChartManager에게 노트 목록을 빌려옴 (const reference)
        const auto& notes = chart.getNotes();
        for (const auto& note : notes) {
            note->draw(window, musicTime, scrollSpeed);
        }
    }

    void drawUI(sf::RenderWindow& window, const std::string& debugInfo) {
        infoText.setString(debugInfo);
        window.draw(infoText);
    }

private:
    void drawLanes(sf::RenderWindow& window) {
        laneLine.setSize(sf::Vector2f(2.0f, GameConfig::WINDOW_HEIGHT));
        laneLine.setFillColor(GameConfig::LANE_BORDER_COLOR);

        // 4개의 레인 경계선
        for (int i = 0; i <= 4; i++) {
            float x = GameConfig::START_X + (i * GameConfig::LANE_WIDTH);
            laneLine.setPosition(x, 0);
            window.draw(laneLine);
        }
    }

    void drawGrid(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed) {
        // 현재 화면에 보여야 할 시간 범위 계산
        double screenDuration = GameConfig::WINDOW_HEIGHT / scrollSpeed; // 화면 높이(px) / 속도(px/ms) = ms
        double renderStartTime = musicTime - (GameConfig::WINDOW_HEIGHT - GameConfig::JUDGMENT_LINE_Y) / scrollSpeed;
        double renderEndTime = musicTime + screenDuration;

        // 현재 BPM 정보 가져오기
        TimingPoint tp = chart.getCurrentTimingPoint(musicTime);
        double beatDur = 60000.0 / tp.bpm;
        int snap = chart.getSnapDiv();
        double interval = beatDur * (4.0 / snap); // 그리드 간격 (ms)

        // 그리드 시작 시간 계산 (타이밍 포인트 기준 정렬)
        double t = tp.time + std::ceil((renderStartTime - tp.time) / interval) * interval;

        beatLine.setSize(sf::Vector2f(GameConfig::LANE_WIDTH * 4, 1.0f));

        while (t < renderEndTime) {
            float y = GameConfig::JUDGMENT_LINE_Y - (float)(t - musicTime) * scrollSpeed;

            // 정박(beatDur 배수)인지 확인하여 색상 다르게 표시
            bool isMainBeat = std::abs(fmod(t - tp.time, beatDur)) < 1.0; // 1ms 오차 허용

            beatLine.setFillColor(isMainBeat ? sf::Color(150, 150, 150) : sf::Color(60, 60, 60));
            beatLine.setPosition(GameConfig::START_X, y);

            window.draw(beatLine);
            t += interval;
        }
    }
};