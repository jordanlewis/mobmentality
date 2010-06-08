#include "wobject.h"
#include "pobject.h"
#include "gobject.h"

WorldObject::WorldObject(PObject *pobject, GObject *gobject)
            : pobject(pobject), gobject(gobject)
{}

void WorldObject::getQuat(Quatf_t quat)
{
    pobject->getQuat(quat);
}

Vec3f WorldObject::getPos()
{
    return pobject->getPos();
}
