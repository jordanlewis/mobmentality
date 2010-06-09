#include <SDL/SDL.h>
#include "graphics.h"
#include "gobject.h"
#include "pobject.h"
#include "aimanager.h"
#include "controllers.h"
#include "ai.h"
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
    AIManager &aim = AIManager::getInstance();

    glViewport(0,0, wres, hres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) 65, (GLdouble) wres / (GLdouble) hres, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 30, -50,
              0, 0, 0,
              0, 1, 0);



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

    glColor3f(1,1,1);
    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        render(world.wobjects[i]);
    }

    for (unsigned int i = 0; i < aim.controllers.size(); i++)
    {
        render(aim.controllers[i]);
    }

    glBegin(GL_QUADS);
    glColor3f(.2,.2,.2);
    glVertex3f(31,-1,-31);
    glVertex3f(-31,-1,-31);
    glVertex3f(-31,-1,31);
    glVertex3f(31,-1,31);
    glEnd();

    SDL_GL_SwapBuffers();

}

void Graphics::render(WorldObject *wobject)
{
    if (wobject->gobject == NULL)
        return;
    Quatf_t quat;
    wobject->getQuat(quat);
    wobject->gobject->draw(wobject->getPos(), quat);

    if (PSteerable *p = dynamic_cast<PSteerable *>(wobject->pobject))
    {
        //DrawArrow(wobject->getPos(), p->getSteering().acc);
    }
}

void Graphics::render(AIController *aic)
{
    AIStrategy *ais = aic->getStrategy();
    Wander *wander;
    if (ais && (wander = dynamic_cast<Wander *>(ais)))
    {
        Vec3f pos = ais->steerable->getPos();
        DrawArrow(pos, wander->seeker.target.pos - pos);
        glPushMatrix();
        pos += ais->steerable->getKinematic().vel.unit() * wander->offset;
        glTranslatef(pos[0], 0, pos[2]);
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(.1, wander->radius, 20, 30);
        glPopMatrix();
    }
}

void Graphics::DrawArrow(Vec3f pos, Vec3f dir)
{
    glDisable(GL_LIGHTING);
    float l = dir.length();
    /* the 6 verts we need for the arrow */
    Vec3f_t verts[6];
    pos.toArray(        verts[0]);
    (pos + dir).toArray(verts[1]);

    /* make perpendicular vectors */
    Vec3f p1 = dir.perp();
    Vec3f p2 = dir.perp(p1);

    (pos + (dir * 0.7f) + (p1 * l * 0.3f)).toArray(verts[2]);
    (pos + (dir * 0.7f) - (p1 * l * 0.3f)).toArray(verts[3]);
    (pos + (dir * 0.7f) + (p2 * l * 0.3f)).toArray(verts[4]);
    (pos + (dir * 0.7f) - (p2 * l * 0.3f)).toArray(verts[5]);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verts);
    uint16_t lineIndices[2 * 5] = {0, 1, 1, 2, 1, 3, 1, 4, 1, 5}; /* 2 indices per line, 5 lines */
    glDrawElements(GL_LINES, 2 * 5, GL_UNSIGNED_SHORT, lineIndices);

    glDisableClientState(GL_VERTEX_ARRAY);

    glEnable(GL_LIGHTING);
}
