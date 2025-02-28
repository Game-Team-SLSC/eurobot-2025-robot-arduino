#pragma once

#include <DynamicStateController.h>
#include <MovementStatus.h>
#include <MovementCall.h>
#include <MovementStatus.h>
#include <MovementDependency.h>

#define MAX_DEP 2

typedef void(*MovementCallback)(bool active);
class Movement {
    public:
    
    // Const/destructor
    Movement(MovementCallback callback);
    virtual ~Movement() = default;

    /*
    @brief execute the movement
    @param active true if the movement is active, false otherwise
    */
    virtual void exec(bool active);

    /*
    @brief called at each iteration of the main loop
    Must update status
    */
    virtual void update() = 0;

    void addDep(MovementDependency* dep);

    /*
    @returns status of the movement
    */
    MovementStatus getStatus() const;

    /*
    @returns target of the movement
    */
    bool getTarget() const;

    
    protected :
    // vars 
    MovementDependency dependencies[MAX_DEP];
    byte dependenciesCount;
    DynamicState<MovementStatus>* status;
    DynamicState<bool>* target;
    MovementCallback callback;
    
    // functions
    /*
    @brief Check and executes dependencies
    @returns false if movement is still updating dependencies, true otherwise
    */
   void checkAndExecuteDeps();
};