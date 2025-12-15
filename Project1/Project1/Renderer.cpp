#include "Renderer.h"
#include "ChartManager.h"
#include "GameConfig.h"
#include "Note.h" // Note 클래스의 draw 함수를 호출하기 위해 필요
#include <iostream>
#include <cmath> // std::abs, std::ceil, fmod 사용

Renderer::Renderer() {
    // 폰트 로드 실패 시 에러 메시지 출력
    if (!mainFont.loadFromFile("tuffy.ttf")) {
        std::cerr << "[Renderer] Failed to load tuffy.ttf" << std::endl;
    }

    // 디버그 텍스트 초기화
    infoText.setFont(mainFont);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10, 10);

    // 판정선 초기화 (위치나 크기는 고정적이므로 생성자에서 일부 설정 가능)
    judgmentLine.setSize(sf::Vector2f(GameConfig::WINDOW_WIDTH, 4.0f));
    judgmentLine.setFillColor(GameConfig::JUDGMENT_LINE_COLOR);
    judgmentLine.setOrigin(0, 2.0f); // 중심점을 선의 가운데로 설정
    judgmentLine.setPosition(0, GameConfig::JUDGMENT_LINE_Y);

    // 레인 구분선 색상 초기화
    laneLine.setFillColor(GameConfig::LANE_BORDER_COLOR);
}

void Renderer::drawGameScreen(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed) {
    // 1. 레인 그리기
    drawLanes(window);

    // 2. 비트 그리드 그리기 (노트 뒤에 깔려야 함)
    drawGrid(window, chart, musicTime, scrollSpeed);

    // 3. 판정선 그리기
    window.draw(judgmentLine);

    // 4. 노트 그리기
    // ChartManager에서 노트 목록을 가져와 순회하며 그리기
    const auto& notes = chart.getNotes();
    for (const auto& note : notes) {
        if (note) { // nullptr 체크
            note->draw(window, musicTime, scrollSpeed);
        }
    }
}

void Renderer::drawUI(sf::RenderWindow& window, const std::string& debugInfo) {
    infoText.setString(debugInfo);
    window.draw(infoText);
}

void Renderer::drawLanes(sf::RenderWindow& window) {
    laneLine.setSize(sf::Vector2f(2.0f, GameConfig::WINDOW_HEIGHT));

    // 0번부터 4번까지 (총 5개의 선) 그리기
    for (int i = 0; i <= 4; i++) {
        float x = GameConfig::START_X + (i * GameConfig::LANE_WIDTH);
        laneLine.setPosition(x, 0);
        window.draw(laneLine);
    }
}

void Renderer::drawGrid(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed) {
    // 현재 화면에 보여질 시간 범위 계산 (화면 높이 / 속도)
    double screenDuration = GameConfig::WINDOW_HEIGHT / scrollSpeed;

    // 렌더링해야 할 시작 시간과 끝 시간 계산
    // (판정선이 화면 하단에 있으므로, 현재 시간보다 약간 이전부터 계산해야 위쪽에서 내려오는 그리드를 그릴 수 있음)
    double renderStartTime = musicTime - (GameConfig::WINDOW_HEIGHT - GameConfig::JUDGMENT_LINE_Y) / scrollSpeed;
    double renderEndTime = musicTime + screenDuration;

    // 현재 시점의 BPM과 박자 정보 가져오기
    TimingPoint tp = chart.getCurrentTimingPoint(musicTime);

    // 박자 간격 계산 (BPM 기반)
    double beatDur = 60000.0 / tp.bpm; // 한 박자의 길이 (ms)
    int snap = chart.getSnapDiv();     // 현재 스냅 설정 (예: 4비트, 8비트)

    // 실제 그리드 사이의 시간 간격
    double interval = beatDur * (4.0 / snap);

    // 그리드를 그리기 시작할 최초 시간 계산 (타이밍 포인트 시간 기준으로 정렬)
    // 예: 음악 시작 후 100ms, 간격이 50ms라면 100, 150, 200... 순서로 딱 떨어지게 계산
    double t = tp.time + std::ceil((renderStartTime - tp.time) / interval) * interval;

    // 가로선 설정 (4개 레인 너비만큼)
    beatLine.setSize(sf::Vector2f(GameConfig::LANE_WIDTH * 4, 1.0f));

    // 화면 끝까지 반복하며 선 그리기
    while (t < renderEndTime) {
        // [핵심 수학] 시간을 Y좌표로 변환
        // (노트 시간 - 현재 노래 시간) * 속도
        // 판정선(JUDGMENT_LINE_Y)에서 위로 올라가야 하므로 값을 뺌 (-)
        float y = GameConfig::JUDGMENT_LINE_Y - (float)(t - musicTime) * scrollSpeed;

        // 정박(한 박자)인지 엇박인지 확인하여 색상 다르게 표시
        // 부동 소수점 오차를 고려해 1ms 미만의 차이는 같다고 판단
        bool isMainBeat = std::abs(fmod(t - tp.time, beatDur)) < 1.0;

        if (isMainBeat) {
            beatLine.setFillColor(sf::Color(150, 150, 150)); // 밝은 회색 (정박)
        }
        else {
            beatLine.setFillColor(sf::Color(60, 60, 60));    // 어두운 회색 (엇박)
        }

        beatLine.setPosition(GameConfig::START_X, y);
        window.draw(beatLine);

        t += interval; // 다음 그리드 시간으로 이동
    }
}