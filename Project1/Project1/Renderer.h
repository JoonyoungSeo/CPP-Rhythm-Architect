#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ChartManager는 포인터나 참조로만 쓰이므로 헤더를 다 가져올 필요 없이 전방 선언만 해도 됨
// 컴파일 속도 향상에 도움
class ChartManager;

class Renderer {
private:
    sf::RectangleShape laneLine;     // 레인 구분선
    sf::RectangleShape judgmentLine; // 판정선
    sf::RectangleShape beatLine;     // 박자 그리드 선
    sf::Font mainFont;               // 폰트
    sf::Text infoText;               // 디버그 정보 텍스트

public:
    Renderer(); // 생성자

    // 메인 그리기 함수
    void drawGameScreen(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed);

    // UI 그리기 함수
    void drawUI(sf::RenderWindow& window, const std::string& debugInfo);

private:
    // 내부적으로만 쓰는 도우미 함수들
    void drawLanes(sf::RenderWindow& window);
    void drawGrid(sf::RenderWindow& window, const ChartManager& chart, double musicTime, float scrollSpeed);
};