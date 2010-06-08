#ifndef GRAPHICS_H
#define GRAPHICS_H

class WorldObject;

class Graphics
{
    Graphics();
    ~Graphics();
    Graphics(const Graphics&);
    Graphics &operator=(const Graphics &);
    static Graphics _instance;

  public:
    int wres, hres;
    void render();
    void render(WorldObject *);

    static Graphics &getInstance() {return _instance;};
};
#endif
