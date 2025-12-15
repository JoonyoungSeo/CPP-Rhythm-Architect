#pragma once
#include <vector>
#include "Note.h"

// [인터페이스]
class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// ---------------------------------------------------------
// [명령 1] 노트 생성 (Place)
// ---------------------------------------------------------
class PlaceNoteCommand : public Command {
private:
    std::vector<Note*>& notes;
    Note* note;
    bool ownsNote;

public:
    // 생성자 선언
    PlaceNoteCommand(std::vector<Note*>& targetList, Note* newNote);

    // 소멸자 선언
    ~PlaceNoteCommand();

    // 함수 오버라이딩 선언
    void execute() override;
    void undo() override;
};

// ---------------------------------------------------------
// [명령 2] 노트 삭제 (Delete)
// ---------------------------------------------------------
class DeleteNoteCommand : public Command {
private:
    std::vector<Note*>& notes;
    Note* note;
    bool ownsNote;

public:
    DeleteNoteCommand(std::vector<Note*>& targetList, Note* targetNote);

    ~DeleteNoteCommand();

    void execute() override;
    void undo() override;
};