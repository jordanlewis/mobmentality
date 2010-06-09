#include "controllers.h"
#include "ai.h"
#include "pobject.h"

SteeringController::SteeringController(PSteerable *steerable)
    : steerable(steerable)
{}

AIController::AIController(PSteerable *steerable)
    : SteeringController(steerable), strategy(NULL)
{}

void AIController::setStrategy(AIStrategy *strategy)
{
    if (this->strategy != NULL)
        delete this->strategy;

    this->strategy = strategy;
    this->strategy->steerable = steerable;
}

void AIController::steer()
{
    steerable->setSteering(strategy->getSteering());
}
