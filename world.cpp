#include "world.h"
#include "wobject.h"
#include "pobject.h"
#include "gobject.h"
#include "aimanager.h"
#include "geominfo.h"
#include "steering.h"
#include "kinematic.h"

World World::_instance;
int World::size = 50;

World::World() : done(false)
{}
World::~World()
{}

void World::addObject(WorldObject *obj)
{
    wobjects.push_back(obj);
}

void World::addAgent()
{
    AIManager &aim = AIManager::getInstance();
    GObject *gobj;
    WorldObject *wobj;

    BoxInfo *box = new BoxInfo(.5,.5,.5);
    Kinematic k = Kinematic(randomVec3f(Vec3f(30, 0, 30)),
                            randomVec3f(Vec3f(10, 0, 10)));
    k.pos.y = 1;
    SteerInfo s = SteerInfo(Vec3f(1, 0, 0));
    PSteerable *psteer = new PSteerable(s, k, 100.0f, box);
    gobj = new GObject(box);
    wobj = new WorldObject (psteer, gobj);
    addObject(wobj);

    aim.control(psteer);
}

void World::cleanObjects()
{
    for (unsigned int i = 0; i < wobjects.size(); i++)
    {
        if (!wobjects[i]->alive)
        {
            delete wobjects[i];
            wobjects.erase(wobjects.begin() + i--);
        }
    }
}
