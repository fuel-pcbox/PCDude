
#include "video.h"
#include "api.h"

namespace mda
{



// Tiles -> 9x14

sf::Texture* tileset;
void render(sf::RenderWindow & rw, int xoff, int yoff);
void Register();
void Unregister();
constexpr int ymax = 25;
constexpr int xmax = 80;

void Register()
{
	Video.gfxCardRender = &render;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};
	tileset = new sf::Texture();
	tileset->loadFromFile("gfx/mda.png");
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
	delete tileset;
}

void render(sf::RenderWindow & rw, int xoff, int yoff)
{
	static int frame = -1;
	frame++;
	frame &= 15; // frame %= 16;
	sf::RectangleShape fillrct(sf::Vector2f(9.0F,14.0F));
	sf::RectangleShape chrct(sf::Vector2f(9.0F,14.0F));
	chrct.setTexture(tileset);
	for(int i = 0; i<(xmax*ymax); i++) // Foreach(character in memory)
	{
		u8 chr =  RAM16::RAM[(0xb0000)+(i<<1)]; // Character number
		u8 attr = RAM16::RAM[(0xb0001)+(i<<1)]; // Attribute part
		sf::Color fg(255,255,255),bg(sf::Color::Black); // Render color
		bool underline=false,high_intensity=false,blink=false;
		if(attr == 0 || attr == 8 || attr == 0x80 || attr == 0x88) // Invisible
		{
			bg = fg = sf::Color::Black;
		}
		else if(attr == 0x70 || attr == 0xF0) // Black on green
		{
			bg = sf::Color(255,255,255);
			fg = sf::Color::Black;
			if(attr == 0xF0) blink=true;
		}
		else if(attr == 0x78 || attr == 0xF8) // Dark green on green
		{
			bg = sf::Color(255,255,255);
			fg = sf::Color(127,127,127);
			if(attr == 0xF8) blink=true;
		}
		else
		{
			if(attr & 0x01) underline = true;
			if(attr & 0x08) high_intensity = true;
			if(attr & 0x80) blink = true;
			if(high_intensity == true) fg=sf::Color(255,255,255);
			else fg=sf::Color(127,127,127);
		}
		if(blink && (frame<8)){fg=bg;}
		sf::Vector2f pos((i%xmax) * 9 + xoff , (i/xmax) * 14 + yoff);
		// Draws background
		fillrct.setSize(sf::Vector2f(9.0F,14.0F));
		fillrct.setPosition(pos);
		fillrct.setFillColor(bg);
		rw.draw(fillrct);
		// Draws character
		int chx = (chr & 63)*9;
		int chy = (chr >>5)*14;
		chrct.setTextureRect(sf::IntRect(chx,chy,9,14));
		chrct.setFillColor(fg);
		chrct.setPosition(pos);

		if(underline)
		{
			fillrct.setSize(sf::Vector2f(7.0F,1.0F));
			fillrct.setPosition(pos+sf::Vector2f(1.0F,13.0F));
			fillrct.setFillColor(fg);
			rw.draw(fillrct);
		}

		rw.draw(chrct);
	}
}

}


