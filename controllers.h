#ifndef CONTROLLERS_H
#define CONTROLLERS_H

class PSteerable;
class AIStrategy;

/* This class bridges the gap between steering input (human input, AI) and
 * the PSteerables that the steering inputs control. */
class SteeringController
{
  protected:
    PSteerable *steerable;
  public:
    SteeringController(PSteerable *);
    virtual void steer() = 0;
};


class AIController : public SteeringController
{
    AIStrategy *strategy;
  public:
    AIController(PSteerable *);
    void setStrategy(AIStrategy *strategy);

    void steer();
};

#endif
