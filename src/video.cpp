
#include "video.h"
#include "api.h"

Video_t::Video_t()
{
#ifdef USE_NCURSES
	initscr();
#endif
}

Video_t::~Video_t()
{
#ifdef USE_NCURSES
	endwin();
#endif
}

void Video_t::Init()
{
	win.create( sf::VideoMode(720+WW,350+HH), "PCDude - 8086 emulator", sf::Style::Default^sf::Style::Resize );

	fntMono.loadFromFile("gfx/libermono.ttf");
	fntMonoBold.loadFromFile("gfx/libermono-bold.ttf");
	fntSans.loadFromFile("gfx/libersans.ttf");

	std::tuple<int,int> sz = gfxCardGetDisplaySize();
	int Tx, Ty;
	std::tie(Tx,Ty) = sz;
	sf::Uint32 nx = Tx+WW;
	sf::Uint32 ny = Ty+HH;
	win.setSize(sf::Vector2u(nx,ny));

#ifdef USE_NCURSES
	w.reset(newwin(0,0,0,0),&delwin);
	wrefresh(w.get());
#endif
}

void Video_t::RefreshScreen()
{
	static sf::RectangleShape rs(sf::Vector2f(722.0f,352.0f));
	rs.setPosition(sf::Vector2f(23,71));
	rs.setOutlineColor(sf::Color::White);
	rs.setOutlineThickness(1.0f);
	rs.setFillColor(sf::Color::Transparent);
	win.clear(sf::Color(237,230,197));
	gfxCardRender(win,24,72);
	win.display();
}

void Video_t::RefreshRegisters()
{
	std::stringstream ss;
	ss.flags(std::ios_base::hex);
	using namespace CPU;
#define addout(x) ss << #x ": " << x << "\n"
	addout(ax);
	addout(bx);
	addout(cx);
	addout(dx);
	addout(cs);
	addout(ip);
	addout(ss);
	addout(sp);
	addout(ds);
	addout(es);
	addout(di);
	addout(si);
	addout(bp);
	addout(flags);
#undef addout
	ss << std::flush;
}

bool Video_t::PollEvent()
{
	return win.pollEvent(ev);
}

Video_t Video;

/// Null video card definitions
#ifndef USE_MDA
namespace mda
{
void Register() {}
void Unregister() {}
}
#endif
#ifndef USE_HERCULES
namespace HGC
{
void Register() {}
void Unregister() {}
}
#endif
#ifndef USE_CGA
namespace CGA
{
void Register() {}
void Unregister() {}
}
#endif

