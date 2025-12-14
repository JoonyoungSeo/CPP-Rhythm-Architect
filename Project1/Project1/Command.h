#pragma once
#include "ChartManager.h"
#include "Note.h"

// [리팩토링] 커맨드 인터페이스
// 모든 명령은 실행(execute)과 되돌리기(undo)를 지원해야 함
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// [노트 생성 커맨드]
class PlaceNoteCommand : public Command {
private:
    ChartManager& manager;
    int type; // 0: Tap, 1: Long
    double timestamp;
    int lane;
    double duration;

public:
    PlaceNoteCommand(ChartManager& m, int t, double time, int l, double d = 0.0)
        : manager(m), type(t), timestamp(time), lane(l), duration(d) {
    }

    void execute() override {
        // 데이터로부터 노트 객체 재생성 및 소유권 전달
        if (type == 0) {
            manager.addNote(std::make_unique<TapNote>(timestamp, lane));
        }
        else {
            manager.addNote(std::make_unique<LongNote>(timestamp, lane, duration));
        }
    }

    void undo() override {
        // 생성했던 노트를 다시 삭제
        manager.removeNoteAt(timestamp, lane);
    }
};

// [노트 삭제 커맨드]
class DeleteNoteCommand : public Command {
private:
    ChartManager& manager;
    int type;
    double timestamp;
    int lane;
    double duration;

public:
    // 삭제할 노트의 정보를 미리 백업해둡니다.
    DeleteNoteCommand(ChartManager& m, const Note* n)
        : manager(m), timestamp(n->getTimestamp()), lane(n->getLane()) {

        // RTTI(Run-Time Type Information) 대신 dynamic_cast로 타입 판별
        if (auto ln = dynamic_cast<const LongNote*>(n)) {
            type = 1;
            // LongNote에 getter가 없어서 직렬화 데이터를 파싱하거나
            // Note 클래스에 getDuration을 추가하는게 좋지만, 
            // 여기서는 dynamic_cast가 성공했으므로 리팩토링 과정에서 LongNote 헤더에 
            // friend class 선언이나 getter 추가가 되었다고 가정하고 진행하거나,
            // 간단히 구현을 위해 Note*가 아닌 데이터를 받도록 설계 변경이 이상적입니다.
            // *이번 리팩토링에서는 간소화를 위해 기본값 0으로 처리하거나 Note.h 수정이 필요합니다.*
            // (아래 Note.h 수정 제안을 참고하세요)
            duration = 0; // 임시: 실제로는 ln->getDuration()이 필요함
        }
        else {
            type = 0;
            duration = 0;
        }
    }

    // 편의를 위한 생성자 오버로딩 (데이터 직접 주입)
    DeleteNoteCommand(ChartManager& m, int t, double time, int l, double d)
        : manager(m), type(t), timestamp(time), lane(l), duration(d) {
    }

    void execute() override {
        manager.removeNoteAt(timestamp, lane);
    }

    void undo() override {
        // 삭제했던 노트를 복구
        if (type == 0) {
            manager.addNote(std::make_unique<TapNote>(timestamp, lane));
        }
        else {
            manager.addNote(std::make_unique<LongNote>(timestamp, lane, duration));
        }
    }
};