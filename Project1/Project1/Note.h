#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// [부모 클래스]
class Note {
protected:
    double timestamp;
    int lane;

public:
    Note(double time, int laneIndex) : timestamp(time), lane(laneIndex) {}
    virtual ~Note() {}

    // 순수 가상 함수
    virtual void draw(sf::RenderWindow& window, double currentMusicTime, float speed) = 0;
    virtual std::string serialize() const = 0;

    double getTimestamp() const { return timestamp; }
    int getLane() const { return lane; }
};

// [자식 클래스: 단타]
class TapNote : public Note {
public:
    TapNote(double time, int laneIndex) : Note(time, laneIndex) {}
    void draw(sf::RenderWindow& window, double currentMusicTime, float speed) override;
    std::string serialize() const override;
};

// [자식 클래스: 롱노트]
class LongNote : public Note {
private:
    double duration;
public:
    LongNote(double time, int laneIndex, double len)
        : Note(time, laneIndex), duration(len) {
    }
    void draw(sf::RenderWindow& window, double currentMusicTime, float speed) override;
    std::string serialize() const override;
};