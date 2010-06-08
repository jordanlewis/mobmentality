#include <SDL/SDL.h>
#include "graphics.h"
#include "gobject.h"
#include "world.h"
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

Graphics Graphics::_instance;

Graphics::Graphics() : wres(800), hres(600)
{
    int colorDepth = 32;
    SDL_Surface *screen;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    screen = SDL_SetVideoMode(wres, hres, colorDepth, SDL_OPENGL);

    if (!screen)
    {
        fprintf(stderr, "Failed to set res to %i by %i: %s\n",
                        wres, hres, SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    int argc = 0;
    glutInit(&argc, NULL);
}
Graphics::~Graphics()
{
    SDL_Quit();
}

void Graphics::render()
{
    World &world = World::getInstance();

    glViewport(0,0, wres, hres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) 65, (GLdouble) wres / (GLdouble) hres, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 25, 0,
              0, 0, 0,
              1, 0, 0);



    GLfloat mat_specular[] = {.2, .2, .2, 1.0};
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    GLfloat light_position[] = {10,10,-10,1};
    GLfloat light_color[] = {1,1,1,1};
    GLfloat ambient_color[] = {.2,.2,.2,1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);

    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        render(world.wobjects[i]);
    }

    SDL_GL_SwapBuffers();

}

void Graphics::render(WorldObject *wobject)
{
    if (wobject->gobject == NULL)
        return;
    Quatf_t quat;
    wobject->getQuat(quat);
    wobject->gobject->draw(wobject->getPos(), quat);
}
