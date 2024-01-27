#include "FrameTimer.h"

FrameTimer::FrameTimer(size_t frameSampleSize)
    : frameTimes(frameSampleSize, 0.0f),
    currentFrame(0),
    accumulatedTime(0.0f),
    frameSampleSize(frameSampleSize) {}

void FrameTimer::update(float deltaTime) {
    accumulatedTime -= frameTimes[currentFrame];
    frameTimes[currentFrame] = deltaTime;
    accumulatedTime += deltaTime;
    currentFrame = (currentFrame + 1) % frameSampleSize;
}

float FrameTimer::getSmoothedDeltaTime() const {
    return frameSampleSize > 0 ? accumulatedTime / frameSampleSize : 0.0f;
}
