
#include "video.h"


Surface screen=nullptr;
Event e;


void Video_t::Init()
{
	win.create( sf::VideoMode(900,600), "PCDude - 8086 emulator", sf::Style::Default^sf::Style::Resize );
}

void Video_t::RefreshScreen()
{
	win.display();
}

bool Video_t::PollEvent()
{
	return win.pollEvent(ev);
}

Video_t Video;

