#ifndef WOBJECT_H
#define WOBJECT_H
#include "quat.h"
#include "vec3f.h"

class PObject;
class GObject;
class Agent;

class WorldObject
{
  public:
    WorldObject(PObject *pobject, GObject *gobject, Agent *agent);

    PObject *pobject;
    GObject *gobject;
    Agent *agent;

    void getQuat(Quatf_t quat);
    Vec3f getPos();
};
#endif
