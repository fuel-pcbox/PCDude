
#include "video.h"


void Video_t::Init()
{
	win.create( sf::VideoMode(900,600), "PCDude - 8086 emulator", sf::Style::Default^sf::Style::Resize );

	fntMono.loadFromFile("gfx/libermono.ttf");
	fntMonoBold.loadFromFile("gfx/libermono-bold.ttf");
	fntSans.loadFromFile("gfx/libersans.ttf");


}

void Video_t::RefreshScreen()
{
	static sf::Clock c;
	gui.Update(c.restart().asSeconds());
	win.clear();
	gfxCardRender(win,24,72);
	gs.Display(win);
	win.display();
}

bool Video_t::PollEvent()
{
	return win.pollEvent(ev);
}

Video_t Video;

