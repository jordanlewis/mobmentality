#include <iostream>
#include "ai.h"
#include "pobject.h"
#include "vec3f.h"

using namespace std;

Seek::Seek() : AIStrategy(), target(Vec3f(0,0,0)), slowRadius(0),targetRadius(0)
{}

SteerInfo Seek::getSteering()
{
    SteerInfo s;

    const Kinematic &k = steerable->getKinematic();
    Vec3f dir = target.pos - k.pos;
    Vec3f targetVelocity;
    float targetSpeed;
    float maxAccel = 10;
    float maxSpeed = maxAccel * 3;
    float distance = dir.length();
    dir.normalize();

    if (distance < targetRadius)
        targetSpeed = 0;
    else if (distance < slowRadius || slowRadius == 0)
        targetSpeed = maxSpeed;
    else
        targetSpeed = maxSpeed * distance / slowRadius;

    targetVelocity = dir * targetSpeed;

    s.acc = targetVelocity - k.vel;
    s.acc = s.acc / .1; // how fast to get to target?

    if (s.acc.length() > maxAccel)
        s.acc = s.acc.unit() * maxAccel;

    s.rot = 0;
    return s;
}
