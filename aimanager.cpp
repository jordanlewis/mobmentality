#include "aimanager.h"
#include "ai.h"
#include "controllers.h"
#include "pobject.h"
AIManager AIManager::_instance;

AIManager::AIManager() : behavior(FLOCK), nextBehavior(false)
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
    if (nextBehavior)
    {
        behavior = (Behavior_t) ((((int)behavior) + 1) % (int) nBehaviors);
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
                default: break;

            }
            if (strat)
                controllers[i]->setStrategy(strat);
        }
        controllers[i]->steer();
    }
    nextBehavior = false;
}
