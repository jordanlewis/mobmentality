#ifndef AI_H
#define AI_H

#include "kinematic.h"
#include "steering.h"
#include "physics.h"
#include <vector>

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

class WeightedSteering
{
  public:
    WeightedSteering(SteerInfo s, float weight);
    SteerInfo steering;
    float weight;
};

class BlendedStrategy : public AIStrategy
{
  protected:
    std::vector<WeightedSteering> strats;
  public:
    virtual SteerInfo getSteering();
};

class AvoidObstacles : public AIStrategy
{
    Seek seeker;
    CollContact obstacle;
  protected:
    bool wallTrapped;
    bool seeObstacle;
  public:
    void detectWalls();
    virtual SteerInfo getSteering();
};


class Flock : public BlendedStrategy
{
    Seek seeker;
    Wander wanderer;
  public:
    Flock();

    virtual SteerInfo getSteering();
};

class FollowLeader : public BlendedStrategy
{
  protected:
    Seek seeker;
    PSteerable *leader;
  public:
    FollowLeader(PSteerable *leader);
    void setLeader(PSteerable *leader) {this->leader = leader;};
    PSteerable *getLeader() {return leader;};
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

class DynamicFormation : public StaticFormation
{
  protected:
    int nSlots;
    virtual Kinematic getSpot() = 0;
  public:
    DynamicFormation(PSteerable *leader, int slot, int nSlots);
};

class DefensiveCircle : public DynamicFormation
{
  protected:
    virtual Kinematic getSpot();
  public:
    DefensiveCircle(PSteerable *leader, int slot, int nSlots);
};

class DynamicVees : public Flock
{
    Vee vee;
  public:
    virtual SteerInfo getSteering();
};

#endif
