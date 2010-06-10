#include <iostream>
#include "ai.h"
#include "pobject.h"
#include "vec3f.h"
#include "world.h"
#include "steering.h"

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

    if (abs(target.pos.x - k.pos.x) > World::size)
    {
        target.pos.x += 2 * World::size * (k.pos.x > 0 ? 1 : -1);
    }
    if (abs(target.pos.z - k.pos.z) > World::size)
    {
        target.pos.z += 2 * World::size * (k.pos.z > 0 ? 1 : -1);
    }



    if (!flee)
        dir = target.pos - k.pos;
    else
        dir = k.pos - target.pos;

    Vec3f targetVelocity;
    float targetSpeed;
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

WeightedSteering::WeightedSteering(SteerInfo s, float weight)
            : steering(s), weight(weight)
{}

SteerInfo BlendedStrategy::getSteering()
{
    SteerInfo out;
    float totalWeight = 0;
    for (unsigned int i = 0; i < strats.size(); i++)
    {
        out.acc += strats[i].steering.acc * strats[i].weight;
        out.rot += strats[i].steering.rot * strats[i].weight;

        totalWeight += strats[i].weight;
    }

    out.acc /= totalWeight;
    out.rot /= totalWeight;

    strats.clear();

    return out;
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
        seeker.target.pos = cohesionVel;
        seeker.flee = false;
        strats.push_back(WeightedSteering(seeker.getSteering(), 1));
    }

    if (separationCnt)
    {
        seeker.target.pos = separationVel;
        seeker.flee = true;
        strats.push_back(WeightedSteering(seeker.getSteering(), 1));
    }

    if (alignmentCnt)
    {
        a.acc = alignmentVel - k.vel;
        a.acc /= .1;
        if (a.acc.length() > MAXACCEL)
            a.acc = a.acc.unit() * MAXACCEL;
        strats.push_back(WeightedSteering(a, 2));
    }

    wanderer.steerable = steerable;
    wanderer.rate = 1;
    wanderer.radius = 3;
    wanderer.offset = 5;
    wanderer.seeker.maxSpeed = 100;
    strats.push_back(WeightedSteering(wanderer.getSteering(), 1));

    return BlendedStrategy::getSteering();
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
        seeker.target.pos = separationVec;
        seeker.flee = true;
        seeker.maxSpeed = 30;
        strats.push_back(WeightedSteering(seeker.getSteering(), 1));
    }

    Vec3f leaderpos = leader->getKinematic().pos;
    seeker.flee = false;
    seeker.slowRadius = 5;
    seeker.target.pos = leaderpos;
    strats.push_back(WeightedSteering(seeker.getSteering(), 1));

    return BlendedStrategy::getSteering();
}

StaticFormation::StaticFormation(PSteerable *leader, int slot)
    : FollowLeader(leader), slot(slot)
{}

SteerInfo StaticFormation::getSteering()
{
    seeker.steerable = steerable;
    seeker.target = getSpot();
    seeker.maxSpeed = 30;
    seeker.slowRadius = 2;
    seeker.targetRadius = 0;
    return seeker.getSteering();
}

Line::Line(PSteerable *leader, int slot) : StaticFormation(leader, slot)
{}

Kinematic Line::getSpot()
{
    const Kinematic &k = leader->getKinematic();
    Kinematic ret;
    ret.pos = k.pos + k.vel.perp(Vec3f(0,1,0)).unit() * slot * (slot % 2 ? -1 : 1);
    return ret;
}

Vee::Vee(PSteerable *leader, int slot) : StaticFormation(leader, slot)
{
}

Kinematic Vee::getSpot()
{
    const Kinematic &k = leader->getKinematic();
    Kinematic ret;
    ret.pos = k.pos + slot *
        (k.vel.perp(Vec3f(0,1,0)).unit() * (slot % 2 ? -1 : 1) - k.vel.unit());
    return ret;
}

DynamicFormation::DynamicFormation(PSteerable *leader, int slot, int nSlots)
    : StaticFormation(leader, slot), nSlots(nSlots)
{}

DefensiveCircle::DefensiveCircle(PSteerable *leader, int slot, int nSlots):
    DynamicFormation(leader, slot, nSlots)
{}

Kinematic DefensiveCircle::getSpot()
{
    const Kinematic &k = leader->getKinematic();
    Kinematic ret;
    float theta = slot * 2 * M_PI / nSlots;
    float radius = nSlots / M_PI / 2;
    ret.pos.x = k.pos.x + radius * cos(theta);
    ret.pos.z = k.pos.z + radius * sin(theta);

    return ret;
}
