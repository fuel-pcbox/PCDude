
#include "video.h"
#include "api.h"

void Video_t::Init()
{
	win.create( sf::VideoMode(900,600), "PCDude - 8086 emulator", sf::Style::Default^sf::Style::Resize );

	fntMono.loadFromFile("gfx/libermono.ttf");
	fntMonoBold.loadFromFile("gfx/libermono-bold.ttf");
	fntSans.loadFromFile("gfx/libersans.ttf");
	guis = sfg::Fixed::Create();
	regs = sfg::Label::Create();
	float tmx, tmy;
	tmx = win.getSize().x - 130;
	guis->Put(regs,sf::Vector2f(tmx,tmy));
}

void Video_t::RefreshScreen()
{
	static sf::Clock c;
	static sf::RectangleShape rs(sf::Vector2f(722.0f,352.0f));
	rs.setPosition(sf::Vector2f(23,71));
	rs.setOutlineColor(sf::Color::White);
	rs.setOutlineThickness(1.0f);
	rs.setFillColor(sf::Color::Transparent);
	gui.Update(c.restart().asSeconds());
	win.clear(sf::Color(237,230,197));
	gfxCardRender(win,24,72);
	RefreshRegisters();
	gs.Display(win);
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
	regs->SetText(ss.str());
}

bool Video_t::PollEvent()
{
	return win.pollEvent(ev);
}

Video_t Video;

