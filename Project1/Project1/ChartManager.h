#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include "Note.h"
#include "TimingPoint.h"

class ChartManager {
private:
    std::vector<std::unique_ptr<Note>> notes;
    std::vector<TimingPoint> timingPoints;
    int snapDiv;

public:
    ChartManager();
    ~ChartManager() = default;

    // 노트 조작
    void addNote(std::unique_ptr<Note> note);
    void removeNoteAt(double time, int lane);
    Note* findNoteAt(double time, int lane);

    // 시간 및 퀀타이즈 관련
    TimingPoint getCurrentTimingPoint(double time) const;
    double quantize(double rawTime);
    void setSnapDiv(int div) { snapDiv = div; }
    int getSnapDiv() const { return snapDiv; }

    // BPM 업데이트
    void updateCurrentBPM(double time, double newBPM);

    // 새 타이밍 포인트 추가
    void addTimingPoint(double time, double bpm, int upper = 4, int lower = 4);

    // 데이터 접근 (ReadOnly)
    const std::vector<std::unique_ptr<Note>>& getNotes() const { return notes; }
    const std::vector<TimingPoint>& getTimingPoints() const { return timingPoints; }

    // 파일 입출력
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void clear();
};