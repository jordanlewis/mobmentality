#ifndef AI_H
#define AI_H

#include "kinematic.h"
#include "steering.h"

class PSteerable;

class AIStrategy
{
  public:
    PSteerable *steerable;
    virtual SteerInfo getSteering() = 0;
};

class Seek : public AIStrategy
{
  public:
    Seek();
    Kinematic target;

    float slowRadius;
    float targetRadius;

    SteerInfo getSteering();
};


/* This class bridges the gap between steering input (human input, AI) and
 * the PSteerables that the steering inputs control. */
class SteeringController
{
  protected:
    PSteerable *steerable;
  public:
    SteeringController(PSteerable *);
    virtual void steer() = 0;
};


class AIController : public SteeringController
{
    AIStrategy *strategy;
  public:
    AIController(PSteerable *);
    void setStrategy(AIStrategy *strategy);

    void steer();
};

#endif
