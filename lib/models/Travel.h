#pragma once

struct Travel {
    byte forward;
    byte lateral;
    byte yaw;

    bool operator==(const Travel& other) const {
        return forward == other.forward && lateral == other.lateral && yaw == other.yaw;
    };
};