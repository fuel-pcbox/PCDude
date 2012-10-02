
#include "video.h"


void Video_t::Init()
{
	win.create( sf::VideoMode(900,600), "PCDude - 8086 emulator", sf::Style::Default^sf::Style::Resize );
}

void Video_t::RefreshScreen()
{
	win.clear();
	gfxCardRender(win,24,72);
	win.display();
}

bool Video_t::PollEvent()
{
	return win.pollEvent(ev);
}

Video_t Video;

