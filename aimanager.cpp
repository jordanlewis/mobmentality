#include "aimanager.h"
#include "ai.h"
#include "world.h"
#include "controllers.h"
#include "pobject.h"
AIManager AIManager::_instance;

AIManager::AIManager() : behavior(FLOCK), nextBehavior(false), spawn(false),
                         cull(false)
{}
AIManager::~AIManager()
{}

AIController *AIManager::control(PSteerable *steerable)
{
    AIController *aic = new AIController(steerable);
    controllers.push_back(aic);
    return aic;
}

void AIManager::run()
{
    World &world = World::getInstance();
    if (nextBehavior)
    {
        behavior = (Behavior_t) ((((int)behavior) + 1) % (int) nBehaviors);
    }
    if (spawn)
    {
        world.addAgent();
        spawn = false;
        // just to make it reset everyone's strategy
        nextBehavior = true;
    }


    for (unsigned int i = 0; i < controllers.size(); i++)
    {
        AIStrategy *strat = NULL;
        PSteerable *leader;
        if (nextBehavior)
        {
            switch (behavior)
            {
                case FLOCK: strat = new Flock(); break;
                case LEADER: if (i == 0)
                             {
                                 strat = new Wander();
                                 reinterpret_cast<Wander *>(strat)->rate = 2;
                                 reinterpret_cast<Wander *>(strat)->offset = 3;
                                 reinterpret_cast<Wander *>(strat)->radius = 1;
                                 reinterpret_cast<Wander *>(strat)->seeker.maxSpeed = 20;
                                 leader = controllers[i]->getSteerable();
                             }
                             else
                             {
                                 strat = new FollowLeader(leader);
                             }
                             break;
                case LINE: if (i == 0)
                               leader = controllers[i]->getSteerable();
                           else
                               strat = new Line(leader, i);
                           break;
                case VEE: if (i == 0)
                              leader = controllers[i]->getSteerable();
                          else
                              strat = new Vee(leader, i);
                          break;
                case CIRCLE: if (i == 0)
                                 leader = controllers[i]->getSteerable();
                             else
                                 strat = new DefensiveCircle(leader, i,
                                           controllers.size());
                             break;
                default: break;

            }
            if (strat)
                controllers[i]->setStrategy(strat);
        }
        controllers[i]->steer();
    }
    nextBehavior = false;

    int toCull = 0;
    if (cull)
    {
        toCull = drand48() * controllers.size();
        if (toCull == 0)
            toCull = 1;
        controllers[toCull]->getSteerable()->wobject->alive = false;
        controllers.erase(controllers.begin() + toCull);
        cull = false;
    }
}
