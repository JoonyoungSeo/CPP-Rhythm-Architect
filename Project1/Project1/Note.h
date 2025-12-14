#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory> // 스마트 포인터 사용을 위해 필수

// [리팩토링] Note 클래스 인터페이스 정의
// OCP 준수: 새로운 노트 타입을 추가해도 기존 코드를 최소한으로 수정
class Note {
protected:
    double timestamp; // 판정 시간 (ms)
    int lane;         // 트랙 번호 (0~3)

public:
    Note(double time, int laneIndex) : timestamp(time), lane(laneIndex) {}
    virtual ~Note() = default; // 가상 소멸자 필수

    // 순수 가상 함수
    virtual void draw(sf::RenderWindow& window, double currentMusicTime, float speed) = 0;
    virtual std::string serialize() const = 0;

    // Getter
    double getTimestamp() const { return timestamp; }
    int getLane() const { return lane; }

    // 팩토리 메서드 패턴을 위한 정적 함수 (나중에 구현)
    // static std::unique_ptr<Note> createFromData(...);
};

// [단타 노트]
class TapNote : public Note {
public:
    TapNote(double time, int laneIndex) : Note(time, laneIndex) {}
    void draw(sf::RenderWindow& window, double currentMusicTime, float speed) override;
    std::string serialize() const override;
};

// [롱 노트]
class LongNote : public Note {
private:
    double duration; // 지속 시간 (ms)
public:
    LongNote(double time, int laneIndex, double len)
        : Note(time, laneIndex), duration(len) {
    }
    void draw(sf::RenderWindow& window, double currentMusicTime, float speed) override;
    std::string serialize() const override;
};