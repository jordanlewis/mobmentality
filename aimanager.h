#ifndef AIMANAGER_H
#define AIMANAGER_H
#include <vector>

class AIController;
class PSteerable;

typedef enum
{
    FLOCK = 0,
    LEADER,
    LINE,
    VEE,
    nBehaviors
} Behavior_t;

class AIManager
{
    static AIManager _instance;
    AIManager();
    ~AIManager();
    AIManager(const AIManager &);
    AIManager &operator=(const AIManager &);

  public:
    Behavior_t behavior;
    bool nextBehavior;
    std::vector<AIController *> controllers;
    static AIManager &getInstance() {return _instance;}
    AIController *control(PSteerable *);

    void run();
};
#endif
