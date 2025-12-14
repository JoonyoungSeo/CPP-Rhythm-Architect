#include "Editor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

Editor::Editor()
    : snapDiv(4), scrollSpeed(0.5f), musicTime(0.0),
    isPlaying(false), isMouseDown(false), currentLane(-1),
    isGameMode(false), isMusicLoaded(false)
{
    // 기본 타이밍 포인트
    timingPoints.push_back(TimingPoint(0.0, 120.0, 4, 4));

    if (bgm.openFromFile("music.ogg")) {
        isMusicLoaded = true;
        std::cout << "Music Loaded!" << std::endl;
    }
    else {
        std::cout << "Music not found (music.ogg). Using internal clock." << std::endl;
    }
}

Editor::~Editor() {
    clearStack(undoStack);
    clearStack(redoStack);
    for (Note* note : notes) delete note;
}

TimingPoint Editor::getCurrentTimingPoint(double time) {
    TimingPoint target = timingPoints[0];
    for (const auto& tp : timingPoints) {
        if (time >= tp.time) target = tp;
        else break;
    }
    return target;
}

double Editor::quantize(double rawTime) {
    TimingPoint tp = getCurrentTimingPoint(rawTime);
    double beatDuration = 60000.0 / tp.bpm;
    double snapInterval = beatDuration * (4.0 / snapDiv);
    double relativeTime = rawTime - tp.time;
    return tp.time + std::round(relativeTime / snapInterval) * snapInterval;
}

void Editor::clearStack(std::stack<Command*>& s) {
    while (!s.empty()) { delete s.top(); s.pop(); }
}

// 업데이트
void Editor::update(sf::Time dt) {
    if (isPlaying) {
        if (isMusicLoaded && bgm.getStatus() == sf::Music::Playing) {
            musicTime = bgm.getPlayingOffset().asMilliseconds();
        }
        else {
            musicTime += dt.asSeconds() * 1000.0;
        }
    }
}

// 입력 처리
void Editor::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // 1. 공통 키
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            isPlaying = !isPlaying;
            if (isMusicLoaded) {
                if (isPlaying) {
                    bgm.setPlayingOffset(sf::Time(sf::milliseconds(musicTime)));
                    bgm.play();
                }
                else bgm.pause();
            }
        }
        if (event.key.code == sf::Keyboard::Enter) {
            isGameMode = !isGameMode;
            isPlaying = false;
            if (isMusicLoaded) bgm.pause();
            std::cout << "Mode: " << (isGameMode ? "GAME" : "EDITOR") << std::endl;
        }
    }

    // 2. 게임 모드
    if (isGameMode) {
        if (event.type == sf::Event::KeyPressed) {
            int lane = -1;
            if (event.key.code == sf::Keyboard::D) lane = 0;
            if (event.key.code == sf::Keyboard::F) lane = 1;
            if (event.key.code == sf::Keyboard::J) lane = 2;
            if (event.key.code == sf::Keyboard::K) lane = 3;

            if (lane != -1) {
                for (auto n : notes) {
                    if (n->getLane() == lane && std::abs(n->getTimestamp() - musicTime) < 100.0) {
                        std::cout << "HIT! Lane " << lane << std::endl;
                        break;
                    }
                }
            }
        }
        return;
    }

    // 3. 에디터 모드
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Num1) snapDiv = 4;
        if (event.key.code == sf::Keyboard::Num2) snapDiv = 8;
        if (event.key.code == sf::Keyboard::Num3) snapDiv = 12;
        if (event.key.code == sf::Keyboard::Num4) snapDiv = 16;

        // [T] 변속 추가 (콘솔 입력)
        if (event.key.code == sf::Keyboard::T && !isPlaying) {
            double b; int u, l;
            std::cout << "Add Timing Point at " << musicTime << "ms" << std::endl;
            std::cout << "BPM? "; std::cin >> b;
            std::cout << "Upper? "; std::cin >> u;
            std::cout << "Lower? "; std::cin >> l;
            timingPoints.push_back(TimingPoint(musicTime, b, u, l));
            std::sort(timingPoints.begin(), timingPoints.end(),
                [](const TimingPoint& a, const TimingPoint& b) { return a.time < b.time; });
        }

        if (event.key.control) {
            if (event.key.code == sf::Keyboard::S) saveProject("song_data.rna");
            if (event.key.code == sf::Keyboard::O) loadProject("song_data.rna");
            if (event.key.code == sf::Keyboard::Z) {
                if (!event.key.shift && !undoStack.empty()) {
                    Command* c = undoStack.top(); undoStack.pop(); c->undo(); redoStack.push(c);
                }
                else if (event.key.shift && !redoStack.empty()) {
                    Command* c = redoStack.top(); redoStack.pop(); c->execute(); undoStack.push(c);
                }
            }
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled && !isPlaying) {
        musicTime += event.mouseWheelScroll.delta * 200.0;
        if (musicTime < 0) musicTime = 0;
        if (isMusicLoaded) bgm.setPlayingOffset(sf::Time(sf::milliseconds(musicTime)));
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mPos = sf::Mouse::getPosition(window);
        if (mPos.y < JUDGMENT_LINE_Y) {
            isMouseDown = true;
            startMousePos = mPos;
            currentLane = (int)((mPos.x - START_X) / LANE_WIDTH);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && isMouseDown) {
        isMouseDown = false;
        sf::Vector2i endPos = sf::Mouse::getPosition(window);
        if (currentLane >= 0 && currentLane <= 3) {
            double startOffset = (JUDGMENT_LINE_Y - startMousePos.y) / scrollSpeed;
            double startTime = quantize(musicTime + startOffset);

            float dist = (float)(startMousePos.y - endPos.y);
            Command* cmd = nullptr;

            if (dist > 10.0f) {
                double endOffset = (JUDGMENT_LINE_Y - endPos.y) / scrollSpeed;
                double endTime = quantize(musicTime + endOffset);
                if (endTime - startTime > 0) cmd = new PlaceNoteCommand(notes, new LongNote(startTime, currentLane, endTime - startTime));
            }
            else {
                Note* target = nullptr;
                for (auto n : notes) {
                    if (n->getLane() == currentLane && std::abs(n->getTimestamp() - startTime) < 1.0) {
                        target = n; break;
                    }
                }
                if (target) cmd = new DeleteNoteCommand(notes, target);
                else cmd = new PlaceNoteCommand(notes, new TapNote(startTime, currentLane));
            }

            if (cmd) { cmd->execute(); undoStack.push(cmd); clearStack(redoStack); }
        }
    }
}

// 그리기
void Editor::draw(sf::RenderWindow& window) {
    // 1. 레인
    for (int i = 0; i <= 4; i++) {
        sf::RectangleShape l(sf::Vector2f(2.0f, 720.0f));
        l.setFillColor(sf::Color(50, 50, 50));
        l.setPosition(START_X + (i * LANE_WIDTH), 0);
        window.draw(l);
    }

    // 2. 비트 그리드
    double screenDuration = 720.0 / scrollSpeed;
    TimingPoint tp = getCurrentTimingPoint(musicTime);
    double beatDur = 60000.0 / tp.bpm;
    double interval = beatDur * (4.0 / snapDiv);
    double t = tp.time + std::ceil((musicTime - tp.time) / interval) * interval;

    while (t < musicTime + screenDuration) {
        float y = JUDGMENT_LINE_Y - (float)(t - musicTime) * scrollSpeed;
        sf::RectangleShape l(sf::Vector2f(4 * LANE_WIDTH, 1.0f));
        bool isMain = std::abs(fmod(t - tp.time, beatDur)) < 0.01;
        l.setFillColor(isMain ? sf::Color(150, 150, 150) : sf::Color(60, 60, 60));
        l.setPosition(START_X, y);
        window.draw(l);
        t += interval;
    }

    // 3. 타이밍 포인트 선
    for (const auto& p : timingPoints) {
        if (p.time >= musicTime && p.time <= musicTime + screenDuration) {
            float y = JUDGMENT_LINE_Y - (float)(p.time - musicTime) * scrollSpeed;
            sf::RectangleShape l(sf::Vector2f(1280.0f, 2.0f));
            l.setFillColor(sf::Color::Green);
            l.setPosition(0, y);
            window.draw(l);
        }
    }

    // 4. 판정선 및 노트
    sf::RectangleShape jl(sf::Vector2f(1280.0f, 5.0f));
    jl.setFillColor(sf::Color::Red);
    jl.setPosition(0, JUDGMENT_LINE_Y);
    window.draw(jl);

    for (Note* n : notes) n->draw(window, musicTime, scrollSpeed);

    // 5. 드래그 미리보기
    if (!isGameMode && isMouseDown && currentLane >= 0 && currentLane <= 3) {
        float h = std::max((float)(startMousePos.y - sf::Mouse::getPosition(window).y), 20.0f);
        sf::RectangleShape p(sf::Vector2f(100.0f, h));
        p.setFillColor(sf::Color(0, 255, 0, 128));
        p.setPosition(START_X + (currentLane * LANE_WIDTH), (float)startMousePos.y - h);
        window.draw(p);
    }

    // 6. UI
    sf::RectangleShape b(sf::Vector2f(1280, 720));
    b.setFillColor(sf::Color::Transparent);
    b.setOutlineThickness(-8.0f);
    if (isGameMode) { b.setOutlineColor(sf::Color::Cyan); window.draw(b); }
    else if (!isPlaying) { b.setOutlineColor(sf::Color::Yellow); window.draw(b); }
}

void Editor::saveProject(const std::string& filename) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << "[TIMING]" << std::endl;
        for (const auto& tp : timingPoints) out << tp.serialize() << std::endl;
        out << "[NOTES]" << std::endl;
        for (const auto& n : notes) out << n->serialize() << std::endl;
        std::cout << "Saved." << std::endl;
    }
}

void Editor::loadProject(const std::string& filename) {
    std::ifstream in(filename);
    if (in.is_open()) {
        clearStack(undoStack); clearStack(redoStack);
        for (auto n : notes) delete n; notes.clear(); timingPoints.clear();
        std::string line; int sec = 0;
        while (std::getline(in, line)) {
            if (line == "[TIMING]") { sec = 1; continue; }
            if (line == "[NOTES]") { sec = 2; continue; }
            if (line.empty()) continue;
            std::stringstream ss(line);
            if (sec == 1) {
                double t, b; int u, l; ss >> t >> b >> u >> l;
                timingPoints.push_back(TimingPoint(t, b, u, l));
            }
            else if (sec == 2) {
                int ty, la; double t, d; ss >> ty >> t >> la >> d;
                if (ty == 0) notes.push_back(new TapNote(t, la));
                else notes.push_back(new LongNote(t, la, d));
            }
        }
        if (timingPoints.empty()) timingPoints.push_back(TimingPoint(0, 120, 4, 4));
        std::sort(timingPoints.begin(), timingPoints.end(), [](const TimingPoint& a, const TimingPoint& b) {return a.time < b.time; });
        musicTime = 0; isPlaying = false;
        std::cout << "Loaded." << std::endl;
    }
}