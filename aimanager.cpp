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
        AIStrategy *strat;
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
                                 reinterpret_cast<Wander *>(strat)->radius = 5;
                                 leader = controllers[i]->getSteerable();
                             }
                             else
                             {
                                 strat = new FollowLeader(leader);
                             }
                             break;
                default: break;

            }
            controllers[i]->setStrategy(strat);
        }
        controllers[i]->steer();
    }
    nextBehavior = false;
}
