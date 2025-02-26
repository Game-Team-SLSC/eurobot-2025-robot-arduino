#pragma once

class Movement;

struct MovementCall {
    Movement* movement;
    bool desiredState;

    bool operator==(const MovementCall& other) const {
        return desiredState == other.desiredState && movement == other.movement;
    }
};