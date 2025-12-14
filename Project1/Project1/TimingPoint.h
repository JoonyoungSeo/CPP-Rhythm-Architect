#pragma once
#include <string>

struct TimingPoint {
    double time;        // 시작 시간 (ms)
    double bpm;         // 템포
    int signatureUpper; // 박자 분자 (4/4의 4)
    int signatureLower; // 박자 분모

    TimingPoint(double t, double b, int up, int low)
        : time(t), bpm(b), signatureUpper(up), signatureLower(low) {
    }

    // 파일 저장을 위한 문자열 변환
    std::string serialize() const {
        return std::to_string(time) + " " + std::to_string(bpm) + " " +
            std::to_string(signatureUpper) + " " + std::to_string(signatureLower);
    }
};