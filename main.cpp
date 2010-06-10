#include <unistd.h>
#include "ai.h"
#include "aimanager.h"
#include "controllers.h"
#include "world.h"
#include "wobject.h"
#include "pobject.h"
#include "physics.h"
#include "geominfo.h"
#include "graphics.h"
#include "gobject.h"
#include "defs.h"
#include "input.h"

int main(int argc, char **argv)
{
    World &world = World::getInstance();
    Input &input = Input::getInstance();
    Graphics &graphics = Graphics::getInstance();
    AIManager &aim = AIManager::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, 0);
    PObject *pobj = new PObject(&info);
    GObject *gobj = new GObject(&info);
    WorldObject *wobj = new WorldObject(pobj, gobj);
    world.addObject(wobj);

    for (int i = 0 ; i < 30; i++)
    {
        world.addAgent();
    }
    aim.behavior = (Behavior_t) ((int) nBehaviors - 1);
    aim.nextBehavior = true;


    double now;
    double last = GetTime();
    Physics &physics = Physics::getInstance();
    while (!world.done)
    {
        now = GetTime();
        input.processInput();
        aim.run();
        physics.simulate(now - last);
        graphics.render();
        last = now;
        world.cleanObjects();
        usleep(10000);
    }
}

