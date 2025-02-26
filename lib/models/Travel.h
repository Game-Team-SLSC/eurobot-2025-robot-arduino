#pragma once

struct Travel {
    uint8_t forward;
    uint8_t lateral;
    uint8_t yaw;

    bool operator==(const Travel& other) const {
        return forward == other.forward && lateral == other.lateral && yaw == other.yaw;
    };
};