#ifndef AGENT_H
#define AGENT_H

#include "vec3f.h"

class Kinematic
{
  public:
    Kinematic(Vec3f pos = Vec3f(0,0,0), Vec3f vel = Vec3f(0,0,0),
              float orientation=0.0);

    Vec3f pos;
    Vec3f vel;
    float orientation;
    Vec3f orientation_v;

};

class SteerInfo
{
  public:
    SteerInfo(Vec3f acc=Vec3f(0,0,0), float rot=0.0);
    Vec3f acc;
    float rot;
};

class Agent
{
    static short maxId;
    Kinematic kinematic;
    SteerInfo steerInfo;
  public:
    Agent(Vec3f, float);

    short id;

    void setKinematic (Kinematic &);
    Kinematic &getKinematic();

    void setSteering(SteerInfo &);
    SteerInfo &getSteering();
};


#endif
