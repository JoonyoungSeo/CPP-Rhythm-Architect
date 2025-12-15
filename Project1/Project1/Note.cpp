#include "Note.h"
#include <string> // to_string 사용

// 디자인 상수
const float NOTE_WIDTH = 100.0f;
const float NOTE_HEIGHT = 20.0f;
const float LANE_WIDTH = 120.0f;
const float START_X = 400.0f;
const float JUDGMENT_LINE_Y = 600.0f;

// --- TapNote ---
void TapNote::draw(sf::RenderWindow& window, double currentMusicTime, float speed) {
    float timeDiff = (float)(timestamp - currentMusicTime);
    float yPos = JUDGMENT_LINE_Y - (timeDiff * speed);

    sf::RectangleShape shape(sf::Vector2f(NOTE_WIDTH, NOTE_HEIGHT));
    shape.setFillColor(sf::Color::Cyan);
    shape.setPosition(START_X + (lane * LANE_WIDTH), yPos);
    window.draw(shape);
}

std::string TapNote::serialize() const {
    return "0 " + std::to_string(timestamp) + " " + std::to_string(lane) + " 0";
}

// --- LongNote ---
void LongNote::draw(sf::RenderWindow& window, double currentMusicTime, float speed) {
    float timeDiff = (float)(timestamp - currentMusicTime);
    float headY = JUDGMENT_LINE_Y - (timeDiff * speed);
    float tailHeight = (float)duration * speed;

    // 몸통
    sf::RectangleShape body(sf::Vector2f(NOTE_WIDTH, tailHeight));
    body.setFillColor(sf::Color(0, 255, 255, 100));
    body.setPosition(START_X + (lane * LANE_WIDTH), headY - tailHeight);

    // 머리
    sf::RectangleShape head(sf::Vector2f(NOTE_WIDTH, NOTE_HEIGHT));
    head.setFillColor(sf::Color::Blue);
    head.setPosition(START_X + (lane * LANE_WIDTH), headY);

    window.draw(body);
    window.draw(head);
}

std::string LongNote::serialize() const {
    return "1 " + std::to_string(timestamp) + " " + std::to_string(lane) + " " + std::to_string(duration);
}