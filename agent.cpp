#include <iostream>
#include "agent.h"

short Agent::maxId = 0;

Kinematic::Kinematic(Vec3f pos, Vec3f vel, float orientation)
                    : pos(pos), vel(vel), orientation(orientation)
{
}

SteerInfo::SteerInfo(Vec3f acc, float rot)
                    : acc(acc), rot(rot)
{}

Agent::Agent(Vec3f pos, float orientation) :
    kinematic(pos, Vec3f(0,0,0), orientation)
{
    id = maxId++;
}


void Agent::setKinematic (Kinematic & kinematic)
{
    this->kinematic = kinematic;
}
Kinematic &Agent::getKinematic()
{
    return kinematic;
}

void Agent::setSteering(SteerInfo & steerInfo)
{
    this->steerInfo = steerInfo;
}

SteerInfo &Agent::getSteering()
{
    return steerInfo;
}
