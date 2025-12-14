#pragma once
#include <SFML/Graphics.hpp>

// [리팩토링] 모든 상수를 한 곳에서 관리하여 유지보수성 향상
namespace GameConfig {
    // 화면 설정
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr int FRAME_LIMIT = 60;
    const std::string WINDOW_TITLE = "Rhythm Architect - Refactored";

    // 리듬 게임 UI 설정
    constexpr float LANE_WIDTH = 100.0f; // 레인 너비
    constexpr float NOTE_WIDTH = 80.0f;  // 노트 너비 (레인보다 약간 작게)
    constexpr float NOTE_HEIGHT = 20.0f; // 노트 높이
    constexpr float JUDGMENT_LINE_Y = 600.0f; // 판정선 Y좌표

    // 4개 레인을 화면 중앙에 배치하기 위한 계산
    // (1280 - (100 * 4)) / 2 = 440
    constexpr float START_X = (WINDOW_WIDTH - (LANE_WIDTH * 4)) / 2.0f;

    // 색상 팔레트
    const sf::Color BG_COLOR = sf::Color(20, 20, 20); // 어두운 회색 배경
    const sf::Color LANE_BORDER_COLOR = sf::Color(100, 100, 100);
    const sf::Color JUDGMENT_LINE_COLOR = sf::Color(255, 50, 50); // 붉은색
    const sf::Color NOTE_COLOR_TAP = sf::Color(0, 255, 255); // Cyan
    const sf::Color NOTE_COLOR_LONG_HEAD = sf::Color(50, 50, 255); // Blue
    const sf::Color NOTE_COLOR_LONG_BODY = sf::Color(50, 50, 255, 128); // 반투명 Blue
}