#include "controllers.h"
#include "ai.h"
#include "pobject.h"

SteeringController::SteeringController(PSteerable *steerable)
    : steerable(steerable)
{}

AIController::AIController(PSteerable *steerable) : SteeringController(steerable)
{}

void AIController::setStrategy(AIStrategy *strategy)
{
    this->strategy = strategy;
    this->strategy->steerable = steerable;
}

void AIController::steer()
{
    steerable->setSteering(strategy->getSteering());
}
