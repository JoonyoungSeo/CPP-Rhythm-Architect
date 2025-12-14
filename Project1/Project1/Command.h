#pragma once
#include <vector>
#include <algorithm>
#include "Note.h"

// 인터페이스
class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// 생성 명령
class PlaceNoteCommand : public Command {
private:
    std::vector<Note*>& notes;
    Note* note;
    bool ownsNote;

public:
    PlaceNoteCommand(std::vector<Note*>& targetList, Note* newNote)
        : notes(targetList), note(newNote), ownsNote(false) {
    }

    ~PlaceNoteCommand() {
        if (ownsNote && note) delete note;
    }

    void execute() override {
        notes.push_back(note);
        ownsNote = false;
    }

    void undo() override {
        auto it = std::find(notes.begin(), notes.end(), note);
        if (it != notes.end()) {
            notes.erase(it);
            ownsNote = true;
        }
    }
};

// 삭제 명령
class DeleteNoteCommand : public Command {
private:
    std::vector<Note*>& notes;
    Note* note;
    bool ownsNote;

public:
    DeleteNoteCommand(std::vector<Note*>& targetList, Note* targetNote)
        : notes(targetList), note(targetNote), ownsNote(false) {
    }

    ~DeleteNoteCommand() {
        if (ownsNote && note) delete note;
    }

    void execute() override {
        auto it = std::find(notes.begin(), notes.end(), note);
        if (it != notes.end()) {
            notes.erase(it);
            ownsNote = true;
        }
    }

    void undo() override {
        notes.push_back(note);
        ownsNote = false;
    }
};