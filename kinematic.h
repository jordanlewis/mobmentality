#ifndef KINEMATIC_H
#define KINEMATIC_H

#include "vec3f.h"

class Kinematic
{
  public:
    Kinematic(Vec3f pos = Vec3f(0,0,0), Vec3f vel = Vec3f(0,0,0),
              float orientation=0.0)
        : pos(pos), vel(vel), orientation(orientation) {}
    Vec3f pos;
    Vec3f vel;
    float orientation;
    Vec3f orientation_v;
};

#endif
