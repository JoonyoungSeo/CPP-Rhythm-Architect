#include "ChartManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ChartManager::ChartManager() : snapDiv(4) {
    // 기본 타이밍 포인트 추가 (120 BPM, 4/4박자)
    timingPoints.push_back(TimingPoint(0.0, 120.0, 4, 4));
}

void ChartManager::addNote(std::unique_ptr<Note> note) {
    // 이미 같은 위치에 노트가 있는지 확인 후 추가 (중복 방지)
    if (findNoteAt(note->getTimestamp(), note->getLane()) == nullptr) {
        notes.push_back(std::move(note)); // 소유권 이동
    }
}

void ChartManager::removeNoteAt(double time, int lane) {
    // remove_if와 람다를 사용한 모던 C++ 스타일 삭제
    notes.erase(
        std::remove_if(notes.begin(), notes.end(),
            [time, lane](const std::unique_ptr<Note>& n) {
                // 오차 범위 1ms 이내면 같은 노트로 간주
                return n->getLane() == lane && std::abs(n->getTimestamp() - time) < 1.0;
            }),
        notes.end());
}

Note* ChartManager::findNoteAt(double time, int lane) {
    for (const auto& n : notes) {
        if (n->getLane() == lane && std::abs(n->getTimestamp() - time) < 1.0) {
            return n.get(); // Raw Pointer 반환 (사용만 하고 소유는 하지 않음)
        }
    }
    return nullptr;
}

TimingPoint ChartManager::getCurrentTimingPoint(double time) const {
    // 현재 시간보다 이전에 있는 타이밍 포인트 중 가장 늦은 것 찾기
    TimingPoint target = timingPoints[0];
    for (const auto& tp : timingPoints) {
        if (time >= tp.time) target = tp;
        else break; // 시간순 정렬되어 있다고 가정
    }
    return target;
}

double ChartManager::quantize(double rawTime) {
    TimingPoint tp = getCurrentTimingPoint(rawTime);
    double beatDuration = 60000.0 / tp.bpm; // 1박자 길이 (ms)
    double snapInterval = beatDuration * (4.0 / snapDiv); // 스냅 간격

    double relativeTime = rawTime - tp.time;
    // 반올림을 통한 자석 효과
    double snappedTime = std::round(relativeTime / snapInterval) * snapInterval;

    return tp.time + snappedTime;
}

void ChartManager::saveToFile(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "[TIMING]" << std::endl;
    for (const auto& tp : timingPoints) {
        out << tp.serialize() << std::endl;
    }

    out << "[NOTES]" << std::endl;
    for (const auto& n : notes) {
        out << n->serialize() << std::endl;
    }
    std::cout << "Chart Saved: " << filename << std::endl;
}

void ChartManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;

    clear(); // 기존 데이터 초기화

    std::string line;
    int section = 0; // 0: None, 1: Timing, 2: Notes

    while (std::getline(in, line)) {
        if (line == "[TIMING]") { section = 1; continue; }
        if (line == "[NOTES]") { section = 2; continue; }
        if (line.empty()) continue;

        std::stringstream ss(line);
        if (section == 1) {
            double t, b; int u, l;
            ss >> t >> b >> u >> l;
            timingPoints.push_back(TimingPoint(t, b, u, l));
        }
        else if (section == 2) {
            int type, lane;
            double time, duration;
            ss >> type >> time >> lane >> duration;

            if (type == 0) {
                notes.push_back(std::make_unique<TapNote>(time, lane));
            }
            else if (type == 1) {
                notes.push_back(std::make_unique<LongNote>(time, lane, duration));
            }
        }
    }

    // 타이밍 포인트 시간순 정렬
    std::sort(timingPoints.begin(), timingPoints.end(),
        [](const TimingPoint& a, const TimingPoint& b) { return a.time < b.time; });

    if (timingPoints.empty()) timingPoints.push_back(TimingPoint(0, 120, 4, 4));

    std::cout << "Chart Loaded: " << filename << std::endl;
}

void ChartManager::clear() {
    notes.clear(); // 스마트 포인터들이 자동으로 메모리 해제
    timingPoints.clear();
}