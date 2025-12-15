#include "ChartManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ChartManager::ChartManager() : snapDiv(4) {
    // 기본 타이밍 포인트 추가 (120 BPM, 4/4박자)
    timingPoints.push_back(TimingPoint(0.0, 120.0, 4, 4));
}

void ChartManager::addNote(std::unique_ptr<Note> note) {
    if (findNoteAt(note->getTimestamp(), note->getLane()) == nullptr) {
        notes.push_back(std::move(note));
    }
}

void ChartManager::removeNoteAt(double time, int lane) {
    notes.erase(
        std::remove_if(notes.begin(), notes.end(),
            [time, lane](const std::unique_ptr<Note>& n) {
                return n->getLane() == lane && std::abs(n->getTimestamp() - time) < 1.0;
            }),
        notes.end());
}

Note* ChartManager::findNoteAt(double time, int lane) {
    for (const auto& n : notes) {
        if (n->getLane() == lane && std::abs(n->getTimestamp() - time) < 1.0) {
            return n.get();
        }
    }
    return nullptr;
}

TimingPoint ChartManager::getCurrentTimingPoint(double time) const {
    TimingPoint target = timingPoints[0];
    for (const auto& tp : timingPoints) {
        if (time >= tp.time) target = tp;
        else break;
    }
    return target;
}

// ⭐ [NEW] 현재 위치의 BPM 업데이트
void ChartManager::updateCurrentBPM(double time, double newBPM) {
    // 현재 시간에 해당하는 타이밍 포인트 찾기
    for (auto& tp : timingPoints) {
        if (time >= tp.time) {
            // 다음 타이밍 포인트가 있고, 현재 시간이 그 이전이면
            // 이 타이밍 포인트가 현재 시간에 해당
            auto it = std::find_if(timingPoints.begin(), timingPoints.end(),
                [&tp](const TimingPoint& other) { return other.time > tp.time; });

            if (it == timingPoints.end() || time < it->time) {
                tp.bpm = newBPM;
                return;
            }
        }
    }

    // 첫 번째 타이밍 포인트의 BPM 변경 (fallback)
    if (!timingPoints.empty()) {
        timingPoints[0].bpm = newBPM;
    }
}

// ⭐ [NEW] 새로운 타이밍 포인트 추가
void ChartManager::addTimingPoint(double time, double bpm, int upper, int lower) {
    // 같은 시간에 이미 타이밍 포인트가 있는지 확인
    for (auto& tp : timingPoints) {
        if (std::abs(tp.time - time) < 1.0) {
            // 기존 타이밍 포인트 업데이트
            tp.bpm = bpm;
            tp.signatureUpper = upper;
            tp.signatureLower = lower;
            return;
        }
    }

    // 새 타이밍 포인트 추가
    timingPoints.push_back(TimingPoint(time, bpm, upper, lower));

    // 시간순 정렬
    std::sort(timingPoints.begin(), timingPoints.end(),
        [](const TimingPoint& a, const TimingPoint& b) { return a.time < b.time; });
}

double ChartManager::quantize(double rawTime) {
    TimingPoint tp = getCurrentTimingPoint(rawTime);
    double beatDuration = 60000.0 / tp.bpm;
    double snapInterval = beatDuration * (4.0 / snapDiv);

    double relativeTime = rawTime - tp.time;
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

    clear();

    std::string line;
    int section = 0;

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

    std::sort(timingPoints.begin(), timingPoints.end(),
        [](const TimingPoint& a, const TimingPoint& b) { return a.time < b.time; });

    if (timingPoints.empty()) timingPoints.push_back(TimingPoint(0, 120, 4, 4));

    std::cout << "Chart Loaded: " << filename << std::endl;
}

void ChartManager::clear() {
    notes.clear();
    timingPoints.clear();
}