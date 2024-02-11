#pragma once

#include <vector>

class FrameTimer {
public:
    FrameTimer(size_t frameSampleSize);

    void update(float deltaTime);
    float getSmoothedDeltaTime() const;

private:
    std::vector<float> frameTimes;
    size_t currentFrame;
    float accumulatedTime;
    size_t frameSampleSize;
};
