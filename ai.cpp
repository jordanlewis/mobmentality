#include <iostream>
#include "ai.h"
#include "pobject.h"
#include "vec3f.h"
#include "world.h"

using namespace std;

#define MAXACCEL 100

Seek::Seek() : target(Vec3f(0,0,0)), slowRadius(0), targetRadius(0),
               maxSpeed(50), flee(false)
{}

SteerInfo Seek::getSteering()
{
    SteerInfo s;

    const Kinematic &k = steerable->getKinematic();
    Vec3f dir;
    if (!flee)
        dir = target.pos - k.pos;
    else
        dir = k.pos - target.pos;

    Vec3f targetVelocity;
    float targetSpeed;
    float maxSpeed = 50;
    float distance = dir.length();
    dir.normalize();

    if (distance < targetRadius)
        targetSpeed = 0;
    else if (slowRadius != 0 && distance < slowRadius)
        targetSpeed = maxSpeed * distance / slowRadius;
    else
        targetSpeed = maxSpeed;

    targetVelocity = dir * targetSpeed;

    s.acc = targetVelocity - k.vel;
    s.acc = s.acc / .1; // how fast to get to target?

    if (s.acc.length() > MAXACCEL)
        s.acc = s.acc.unit() * MAXACCEL;

    s.rot = 0;
    return s;
}

Wander::Wander() : orientation(0), radius(1), offset(0)
{}

SteerInfo Wander::getSteering()
{
    seeker.steerable = steerable;

    const Kinematic &k = steerable->getKinematic();

    orientation += (ranf() - ranf()) * rate;
    seeker.target.pos = k.pos + k.vel.unit() * offset;
    seeker.target.pos += Vec3f(sin(orientation), 0, cos(orientation)) * radius;

    return seeker.getSteering();
}

Flock::Flock()
{}

SteerInfo Flock::getSteering()
{
    SteerInfo s, f, a;

    const Kinematic &k = steerable->getKinematic();
    seeker.steerable = steerable;

    World &world = World::getInstance();

    Vec3f cohesionVel, separationVel, alignmentVel;
    int   cohesionCnt = 0, separationCnt = 0, alignmentCnt = 0;
    Vec3f fleeVec = Vec3f(0,0,0);

    Vec3f diff;
    Vec3f pos;
    float dist;
    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        PObject *p = world.wobjects[i]->pobject;
        PSteerable *pobj;
        if (p == steerable || !(pobj = dynamic_cast<PSteerable *>(p)))
            continue;

        pos = world.wobjects[i]->pobject->getPos();
        diff = pos - k.pos;
        dist = diff.length();

        if (dist < 15)
        {
            alignmentCnt++;
            alignmentVel += pobj->getKinematic().vel;
        }

        if (dist < 10)
        {
            cohesionCnt++;
            cohesionVel += pos;
        }

        if (dist < 3)
        {
            separationCnt++;
            separationVel += pos;
        }
    }
    alignmentVel /= alignmentCnt;
    cohesionVel /= cohesionCnt;
    separationVel /= separationCnt;

    /* Seek toward average position, avoid local collisions, align w avg
     * velocity and alignment*/
    if (cohesionCnt)
    {
        seeker.target = cohesionVel;
        seeker.flee = false;
        s = seeker.getSteering();
    }

    if (separationCnt)
    {
        seeker.target = separationVel;
        seeker.flee = true;
        f = seeker.getSteering();
    }

    if (alignmentCnt)
    {
        a.acc = alignmentVel - k.vel;
        a.acc /= .1;
        if (a.acc.length() > MAXACCEL)
            a.acc = a.acc.unit() * MAXACCEL;
    }

    wanderer.steerable = steerable;
    wanderer.rate = 1;
    wanderer.radius = 3;
    wanderer.offset = 5;
    wanderer.seeker.maxSpeed = 30;
    SteerInfo w = wanderer.getSteering();

    s.acc *= 2;
    s.acc += 2 * f.acc;
    s.acc += 3 * a.acc;
    s.acc += 1 * w.acc;
    s.acc /= 8;


    return s;
}

FollowLeader::FollowLeader(PSteerable *leader) : leader(leader)
{}

SteerInfo FollowLeader::getSteering()
{
    World &world = World::getInstance();
    const Kinematic k = steerable->getKinematic();

    Vec3f separationVec;
    int separationCnt = 0;
    seeker.steerable = steerable;
    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        PObject *p = world.wobjects[i]->pobject;
        PSteerable *pobj;
        if (p == steerable || p == leader ||
            !(pobj = dynamic_cast<PSteerable *>(p)))
            continue;

        Vec3f pos = world.wobjects[i]->pobject->getPos();

        if ((pos - k.pos).length() < 3)
        {
            separationCnt++;
            separationVec += pos;
        }
    }

    separationVec /= separationCnt;

    SteerInfo s, l;
    if (separationCnt)
    {
        seeker.target = separationVec;
        seeker.flee = true;
        seeker.maxSpeed = 100;
        l = seeker.getSteering();
    }

    Vec3f leaderpos = leader->getKinematic().pos;
    seeker.flee = false;
    seeker.slowRadius = 5;
    seeker.target = leaderpos;
    s = seeker.getSteering();

    s.acc += l.acc;
    s.acc /= 2;

    return s;
}
