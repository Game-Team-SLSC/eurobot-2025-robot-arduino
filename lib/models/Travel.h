#pragma once

struct Travel {
    int8_t forward;
    int8_t lateral;
    int8_t yaw;

    bool operator==(const Travel& other) const {
        return forward == other.forward && lateral == other.lateral && yaw == other.yaw;
    };
};