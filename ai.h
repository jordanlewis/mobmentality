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

    virtual SteerInfo getSteering();
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
    virtual SteerInfo getSteering();
};

class Flock : public AIStrategy
{
    Seek seeker;
    Wander wanderer;
  public:
    Flock();

    virtual SteerInfo getSteering();
};

class FollowLeader : public AIStrategy
{
  protected:
    Seek seeker;
    PSteerable *leader;
  public:
    FollowLeader(PSteerable *leader);
    virtual SteerInfo getSteering();
};

class StaticFormation : public FollowLeader
{
  protected:
    int slot;
    virtual Kinematic getSpot() = 0;
  public:
    StaticFormation(PSteerable *leader, int slot);
    virtual SteerInfo getSteering();
};

class Line : public StaticFormation
{
  protected:
    virtual Kinematic getSpot();
  public:
    Line(PSteerable *leader, int slot);
};

class Vee : public StaticFormation
{
  protected:
    virtual Kinematic getSpot();
  public:
    Vee(PSteerable *leader, int slot);
};

#endif
