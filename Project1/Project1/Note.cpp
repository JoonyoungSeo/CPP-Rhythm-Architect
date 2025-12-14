#include "Note.h"
#include "GameConfig.h" // 상수 모음
#include <string>

// --- TapNote 구현 ---
void TapNote::draw(sf::RenderWindow& window, double currentMusicTime, float speed) {
    // Y위치 계산: 판정선 - (남은 시간 * 속도)
    float timeDiff = (float)(timestamp - currentMusicTime);
    float yPos = GameConfig::JUDGMENT_LINE_Y - (timeDiff * speed);

    // 화면 밖으로 너무 멀리 나간 노트는 그리기 최적화 (Culling)
    if (yPos < -50 || yPos > GameConfig::WINDOW_HEIGHT + 50) return;

    sf::RectangleShape shape(sf::Vector2f(GameConfig::NOTE_WIDTH, GameConfig::NOTE_HEIGHT));
    shape.setFillColor(GameConfig::NOTE_COLOR_TAP);
    // 중심축을 가운데로 설정하여 레인 중앙에 오도록 함
    shape.setOrigin(GameConfig::NOTE_WIDTH / 2.0f, GameConfig::NOTE_HEIGHT / 2.0f);

    float xPos = GameConfig::START_X + (lane * GameConfig::LANE_WIDTH) + (GameConfig::LANE_WIDTH / 2.0f);
    shape.setPosition(xPos, yPos);

    window.draw(shape);
}

std::string TapNote::serialize() const {
    // Type 0
    return "0 " + std::to_string(timestamp) + " " + std::to_string(lane) + " 0";
}

// --- LongNote 구현 ---
void LongNote::draw(sf::RenderWindow& window, double currentMusicTime, float speed) {
    float timeDiff = (float)(timestamp - currentMusicTime);
    float headY = GameConfig::JUDGMENT_LINE_Y - (timeDiff * speed);
    float tailLenPixels = (float)duration * speed;
    float tailY = headY - tailLenPixels;

    // 화면 밖 검사 (Culling)
    if (headY < -50 && tailY < -50) return; // 이미 지나감
    if (tailY > GameConfig::WINDOW_HEIGHT + 50) return; // 아직 안옴

    float xPos = GameConfig::START_X + (lane * GameConfig::LANE_WIDTH) + (GameConfig::LANE_WIDTH / 2.0f);

    // 1. 몸통 (Body)
    sf::RectangleShape body(sf::Vector2f(GameConfig::NOTE_WIDTH, tailLenPixels));
    body.setFillColor(GameConfig::NOTE_COLOR_LONG_BODY);
    body.setOrigin(GameConfig::NOTE_WIDTH / 2.0f, 0); // 위쪽 기준
    body.setPosition(xPos, tailY);

    // 2. 머리 (Head) - 시작점
    sf::RectangleShape head(sf::Vector2f(GameConfig::NOTE_WIDTH, GameConfig::NOTE_HEIGHT));
    head.setFillColor(GameConfig::NOTE_COLOR_LONG_HEAD);
    head.setOrigin(GameConfig::NOTE_WIDTH / 2.0f, GameConfig::NOTE_HEIGHT / 2.0f);
    head.setPosition(xPos, headY);

    // 3. 꼬리 끝 (Tail) - 끝점 표시 (선택사항)
    sf::RectangleShape tailEnd(sf::Vector2f(GameConfig::NOTE_WIDTH, 5.0f));
    tailEnd.setFillColor(GameConfig::NOTE_COLOR_LONG_HEAD);
    tailEnd.setOrigin(GameConfig::NOTE_WIDTH / 2.0f, 0);
    tailEnd.setPosition(xPos, tailY);

    window.draw(body);
    window.draw(tailEnd);
    window.draw(head);
}

std::string LongNote::serialize() const {
    // Type 1
    return "1 " + std::to_string(timestamp) + " " + std::to_string(lane) + " " + std::to_string(duration);
}