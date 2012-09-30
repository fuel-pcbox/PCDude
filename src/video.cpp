
#include "video.h"


Surface screen=nullptr;
Event e;


void Video_t::Init()
{
#ifdef VIDEO
	SDLAssert(SDL_Init(SDL_INIT_EVERYTHING),"Failed to initialize video subsystem");
#endif
}

void Video_t::CreateScreen(int w, int h)
{
#ifdef VIDEO
	screen = SDL_SetVideoMode(720,350,24,SDL_SWSURFACE);
	if(screen==nullptr)
	{
		throw new VideoException("Couldn't create the display surface");
	}
#endif
}

void Video_t::RefreshScreen()
{
#ifdef VIDEO
	SDLAssert(SDL_Flip(screen),"Couldn't flip the screen, weird.");
#endif
}

bool Video_t::PollEvent()
{
#ifdef VIDEO
	return SDL_PollEvent(&e);
#endif
}

void Video_t::Quit()
{
#ifdef VIDEO
	SDL_Quit();
#endif
}

Video_t Video;

