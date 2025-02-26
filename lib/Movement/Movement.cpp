#include <Movement.h>

Movement::Movement(MovementCallback callback) :
dependencies(nullptr),
dependenciesCount(0),
status(createState<MovementStatus>(MovementStatus::IDLE)),
target(createState<bool>(true)),
callback(callback)
{};

bool Movement::getTarget() const {
    return target->get();
};

MovementStatus Movement::getStatus() const {
    return status->get();
};

void Movement::exec(bool active) {
    target->set(active);

    if (!target->hasChanged() && status->get() != MovementStatus::IDLE) return;

    status->set(MovementStatus::UPDATING_DEPS);

    update();
};

void Movement::checkAndExecuteDeps() {
    if (status->get() != MovementStatus::UPDATING_DEPS) return;

    for (byte i = 0; i < dependenciesCount; i++) {
        MovementDependency& dep = dependencies[i];

        bool currentTarget = target->get();

        if (dep.bindedTarget != currentTarget) continue;

        dep.moveCall.movement->exec(dep.moveCall.desiredState);
        
        if (dep.moveCall.movement->getStatus() == MovementStatus::SET) continue;

        dep.moveCall.movement->update();

        return;
    }

    status->set(MovementStatus::MOVING);

    return;
}

void Movement::addDep(MovementDependency* dep) {
    if (dependenciesCount >= MAX_DEP) {
        error("Dependencies limit reached");
        return;
    }
    dependencies[dependenciesCount - 1] = *dep;
    dependenciesCount++;
}