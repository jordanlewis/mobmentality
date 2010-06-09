#include <SDL/SDL.h>
#include "input.h"
#include "aimanager.h"
#include "world.h"

Input Input::_instance;

Input::Input()
{}
Input::~Input()
{}

void Input::processInput()
{
    SDL_Event SDLevt;
    //Uint8 *keystate = SDL_GetKeyState(NULL);
    World &world = World::getInstance();
    AIManager &aim = AIManager::getInstance();

    while (SDL_PollEvent(&SDLevt))
    {
        switch(SDLevt.type)
        {
            case SDL_KEYDOWN:
                switch(SDLevt.key.keysym.sym)
                {
                    case SDLK_q: world.done = true; break;
                    case SDLK_n: aim.nextBehavior = true; break;
                    default: break;
                }
                break;
            default: break;
        }
    }

}
