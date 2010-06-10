#ifndef WOBJECT_H
#define WOBJECT_H
#include "quat.h"
#include "vec3f.h"

class PObject;
class GObject;

class WorldObject
{
  public:
    WorldObject(PObject *pobject, GObject *gobject);

    PObject *pobject;
    GObject *gobject;

    bool alive;

    void getQuat(Quatf_t quat);
    Vec3f getPos();
};
#endif
