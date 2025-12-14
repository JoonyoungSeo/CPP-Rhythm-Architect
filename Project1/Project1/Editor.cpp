#include "Editor.h"

Editor::Editor()
    : isPlaying(false), musicTime(0.0), scrollSpeed(0.8f), isGameMode(false),
    isMouseDown(false), currentLane(-1)
{
    if (bgm.openFromFile("music.ogg")) {
        std::cout << "[Editor] Music loaded successfully." << std::endl;
    }
    else {
        std::cerr << "[Editor] Failed to load music.ogg" << std::endl;
    }
}

Editor::~Editor() {
    // std::unique_ptr을 사용하므로 별도의 메모리 해제 로직 불필요
}

void Editor::processCommand(Command* cmd) {
    if (!cmd) return;
    cmd->execute();
    undoStack.push(std::unique_ptr<Command>(cmd));

    // 새로운 명령이 실행되면 Redo 스택은 비워야 함
    while (!redoStack.empty()) redoStack.pop();
}

void Editor::update(sf::Time dt) {
    if (isPlaying) {
        if (bgm.getStatus() == sf::Music::Playing) {
            // [Audio Driven Sync] 오디오 엔진의 시간을 기준으로 동기화
            musicTime = bgm.getPlayingOffset().asMilliseconds();
        }
        else {
            // 음악이 없을 경우를 대비한 폴백
            musicTime += dt.asMilliseconds();
        }
    }
}

void Editor::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // 1. 모드 전환 및 재생 제어
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            isGameMode = !isGameMode;
            isPlaying = false;
            if (bgm.getStatus() == sf::Music::Playing) bgm.pause();
            std::cout << "Mode switched: " << (isGameMode ? "GAME" : "EDITOR") << std::endl;
        }
        if (event.key.code == sf::Keyboard::Space) {
            isPlaying = !isPlaying;
            if (isPlaying) {
                bgm.setPlayingOffset(sf::Time(sf::milliseconds(musicTime)));
                bgm.play();
            }
            else {
                bgm.pause();
            }
        }
    }

    // 게임 모드일 때는 편집 입력 무시
    if (isGameMode) return;

    // 2. 에디터 전용 입력 (Undo/Redo, Save/Load)
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.control) {
            // Undo (Ctrl + Z)
            if (event.key.code == sf::Keyboard::Z) {
                if (!event.key.shift && !undoStack.empty()) {
                    auto cmd = std::move(undoStack.top());
                    undoStack.pop();
                    cmd->undo();
                    redoStack.push(std::move(cmd));
                    std::cout << "Undo." << std::endl;
                }
                // Redo (Ctrl + Shift + Z)
                else if (event.key.shift && !redoStack.empty()) {
                    auto cmd = std::move(redoStack.top());
                    redoStack.pop();
                    cmd->execute();
                    undoStack.push(std::move(cmd));
                    std::cout << "Redo." << std::endl;
                }
            }
            // Save (Ctrl + S)
            if (event.key.code == sf::Keyboard::S) {
                chartManager.saveToFile("chart.rna");
            }
            // Load (Ctrl + O)
            if (event.key.code == sf::Keyboard::O) {
                chartManager.loadFromFile("chart.rna");
                while (!undoStack.empty()) undoStack.pop();
                while (!redoStack.empty()) redoStack.pop();
            }
        }

        // 마우스 휠로 타임라인 이동
        if (!isPlaying) {
            // SFML 이벤트 루프 밖에서 처리하거나 여기서 처리
            // 휠 이벤트는 별도 분기 필요
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled && !isPlaying) {
        musicTime += event.mouseWheelScroll.delta * 200.0;
        if (musicTime < 0) musicTime = 0;
        if (bgm.getStatus() != sf::Music::Playing)
            bgm.setPlayingOffset(sf::Time(sf::milliseconds(musicTime)));
    }

    // 3. 노트 배치 (마우스 입력)
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mPos = sf::Mouse::getPosition(window);
        // 판정선 위쪽 영역인지 확인
        if (mPos.y < GameConfig::JUDGMENT_LINE_Y) {
            isMouseDown = true;
            startMousePos = mPos;
            currentLane = (int)((mPos.x - GameConfig::START_X) / GameConfig::LANE_WIDTH);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && isMouseDown) {
        isMouseDown = false;
        sf::Vector2i endPos = sf::Mouse::getPosition(window);

        // 유효한 레인인지 확인
        if (currentLane >= 0 && currentLane <= 3) {
            // Y좌표를 시간으로 역산
            double startY = startMousePos.y;
            double timeOffset = (GameConfig::JUDGMENT_LINE_Y - startY) / scrollSpeed;
            double rawTime = musicTime + timeOffset;
            double snappedTime = chartManager.quantize(rawTime);

            float dragDist = (float)(startMousePos.y - endPos.y);

            // 드래그 거리가 길면 롱노트, 짧으면 단타/삭제
            if (dragDist > 20.0f) {
                // 롱노트 생성
                double endY = endPos.y;
                double endTimeOffset = (GameConfig::JUDGMENT_LINE_Y - endY) / scrollSpeed;
                double snappedEndTime = chartManager.quantize(musicTime + endTimeOffset);
                double duration = snappedEndTime - snappedTime;

                if (duration > 0) {
                    processCommand(new PlaceNoteCommand(chartManager, 1, snappedTime, currentLane, duration));
                }
            }
            else {
                // 해당 위치에 노트가 있으면 삭제, 없으면 단타 생성
                Note* existing = chartManager.findNoteAt(snappedTime, currentLane);
                if (existing) {
                    // 삭제 커맨드 (LongNote 길이를 알기 위해선 Note에 가상함수 getDuration 필요. 여기선 0으로 가정)
                    processCommand(new DeleteNoteCommand(chartManager, 0, existing->getTimestamp(), existing->getLane(), 0));
                }
                else {
                    // 단타 생성
                    processCommand(new PlaceNoteCommand(chartManager, 0, snappedTime, currentLane));
                }
            }
        }
    }
}

void Editor::draw(sf::RenderWindow& window) {
    // Renderer에게 그리기 위임
    renderer.drawGameScreen(window, chartManager, musicTime, scrollSpeed);

    // UI 정보 표시
    std::string info = "Time: " + std::to_string((int)musicTime) + "ms\n";
    info += "Mode: " + std::string(isGameMode ? "GAME" : "EDITOR") + "\n";
    info += "Snap: 1/" + std::to_string(chartManager.getSnapDiv());
    renderer.drawUI(window, info);
}