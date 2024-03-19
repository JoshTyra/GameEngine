#pragma once

#include <string>
#include <vector>
#include "geometry/AnimationKeyframe.h"

struct AnimationChannel {
    std::string boneName;
    std::vector<Keyframe> keyframes;

    AnimationChannel(const std::string& boneName) : boneName(boneName) {}
};
