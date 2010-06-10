#include "wobject.h"
#include "pobject.h"
#include "gobject.h"

WorldObject::WorldObject(PObject *pobject, GObject *gobject)
            : pobject(pobject), gobject(gobject), alive(true)
{
    if (pobject)
        pobject->wobject = this;
}

void WorldObject::getQuat(Quatf_t quat)
{
    pobject->getQuat(quat);
}

Vec3f WorldObject::getPos()
{
    return pobject->getPos();
}
