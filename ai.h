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
    float maxSpeed;
    bool flee;

    SteerInfo getSteering();
};

class Wander : public AIStrategy
{
    float orientation;
  public:
    Seek seeker;
    float radius;
    float offset;
    float rate;

    Wander();
    SteerInfo getSteering();
};

class Flock : public AIStrategy
{
    Seek seeker;
    Wander wanderer;
  public:
    Flock();

    SteerInfo getSteering();
};

class FollowLeader : public AIStrategy
{
    Seek seeker;
    PSteerable *leader;
  public:
    FollowLeader(PSteerable *leader);
    SteerInfo getSteering();
};

#endif
