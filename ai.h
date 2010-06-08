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

#endif
