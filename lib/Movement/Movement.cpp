#include <Movement.h>

Movement::Movement(MovementCallback callback) :
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
    warn("here");
    delay(1000);
    
    target->set(active);
    
    warn("here");
    delay(1000);

    if (!target->hasChanged() && status->get() != MovementStatus::IDLE) return;
    
    warn("here");
    delay(1000);

    status->set(MovementStatus::UPDATING_DEPS);
    
    warn("here");
    delay(1000);
};

void Movement::checkAndExecuteDeps() {
    if (status->get() != MovementStatus::UPDATING_DEPS) return;

    for (byte i = 0; i < dependenciesCount; i++) {
        MovementDependency& dep = dependencies[i];

        bool currentTarget = target->get();

        warn("%s", dep.bindedTarget ? "true" : "false");
        warn("%s", currentTarget ? "true" : "false");

        if (dep.bindedTarget != currentTarget) continue;

        dep.moveCall.movement->exec(dep.moveCall.desiredState);
        
        if (dep.moveCall.movement->getStatus() == MovementStatus::SET) continue;

        warn("herree agains");

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