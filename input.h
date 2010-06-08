#ifndef INPUT_H
#define INPUT_H
class Input
{
    static Input _instance;
    Input();
    ~Input();
    Input(const Input&);
    Input &operator=(const Input&);
  public:
    static Input &getInstance() { return _instance; };

    void processInput();
};

#endif
