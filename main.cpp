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

    for (int i = 0; i < 30; i++)
    {
        BoxInfo box = BoxInfo(.5,.5,.5);
        Kinematic k = Kinematic(randomVec3f(Vec3f(30, 0, 30)),
                                randomVec3f(Vec3f(10, 0, 10)));
        k.pos.y = 1;
        SteerInfo s = SteerInfo(Vec3f(1, 0, 0));
        PSteerable *psteer = new PSteerable(s, k, 100.0f, &box);
        gobj = new GObject(&box);
        wobj = new WorldObject (psteer, gobj);
        world.addObject(wobj);

        aim.control(psteer);
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
        usleep(10000);
    }
}

