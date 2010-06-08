#include "world.h"
#include "wobject.h"

World World::_instance;

World::World() : done(false)
{}
World::~World()
{}

void World::addObject(WorldObject *obj)
{
    wobjects.push_back(obj);
}
