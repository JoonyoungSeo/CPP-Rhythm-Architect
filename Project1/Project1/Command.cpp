#include "Command.h"
#include <algorithm> // std::find 사용을 위해 필수

// =========================================================
// PlaceNoteCommand 구현
// =========================================================

PlaceNoteCommand::PlaceNoteCommand(std::vector<Note*>& targetList, Note* newNote)
    : notes(targetList), note(newNote), ownsNote(false) {
}

PlaceNoteCommand::~PlaceNoteCommand() {
    // 내가 노트를 소유하고 있다면(Undo된 상태라면) 메모리 해제
    if (ownsNote && note) {
        delete note;
    }
}

void PlaceNoteCommand::execute() {
    notes.push_back(note);
    ownsNote = false; // 리스트에 들어갔으니 소유권 포기
}

void PlaceNoteCommand::undo() {
    auto it = std::find(notes.begin(), notes.end(), note);
    if (it != notes.end()) {
        notes.erase(it);
        ownsNote = true; // 리스트에서 빠졌으니 내가 관리
    }
}

// =========================================================
// DeleteNoteCommand 구현
// =========================================================

DeleteNoteCommand::DeleteNoteCommand(std::vector<Note*>& targetList, Note* targetNote)
    : notes(targetList), note(targetNote), ownsNote(false) {
}

DeleteNoteCommand::~DeleteNoteCommand() {
    if (ownsNote && note) {
        delete note;
    }
}

void DeleteNoteCommand::execute() {
    auto it = std::find(notes.begin(), notes.end(), note);
    if (it != notes.end()) {
        notes.erase(it);
        ownsNote = true; // 삭제되었으니 복구용으로 내가 보관
    }
}

void DeleteNoteCommand::undo() {
    notes.push_back(note);
    ownsNote = false; // 리스트로 복귀
}