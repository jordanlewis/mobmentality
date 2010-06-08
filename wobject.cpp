#include "wobject.h"
#include "pobject.h"
#include "gobject.h"
#include "agent.h"

WorldObject::WorldObject(PObject *pobject, GObject *gobject, Agent *agent)
            : pobject(pobject), gobject(gobject), agent(agent)
{}

void WorldObject::getQuat(Quatf_t quat)
{
    pobject->getQuat(quat);
}

Vec3f WorldObject::getPos()
{
    return pobject->getPos();
}
