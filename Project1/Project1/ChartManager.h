#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include "Note.h"
#include "TimingPoint.h"

// [리팩토링] 데이터 관리 책임 분리 (SRP 준수)
// Editor 클래스에서 노트 관리와 시간 계산 로직을 떼어냄
class ChartManager {
private:
    // Raw pointer 대신 unique_ptr 사용으로 메모리 안전성 확보
    std::vector<std::unique_ptr<Note>> notes;
    std::vector<TimingPoint> timingPoints;
    int snapDiv; // 4, 8, 12, 16 박자 분할

public:
    ChartManager();
    ~ChartManager() = default; // unique_ptr 덕분에 별도 해제 코드 불필요

    // 노트 조작 (소유권 이동을 고려한 인터페이스)
    void addNote(std::unique_ptr<Note> note);
    void removeNoteAt(double time, int lane);
    Note* findNoteAt(double time, int lane); // 조회용 Raw Pointer 반환 (소유권X)

    // 시간 및 퀀타이즈 관련
    TimingPoint getCurrentTimingPoint(double time) const;
    double quantize(double rawTime);
    void setSnapDiv(int div) { snapDiv = div; }
    int getSnapDiv() const { return snapDiv; }

    // 데이터 접근 (ReadOnly)
    const std::vector<std::unique_ptr<Note>>& getNotes() const { return notes; }
    const std::vector<TimingPoint>& getTimingPoints() const { return timingPoints; }

    // 파일 입출력
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void clear();
};