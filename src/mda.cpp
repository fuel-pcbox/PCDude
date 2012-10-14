
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
	frame &= 7; // frame %= 8;
	sf::RectangleShape fillrct(sf::Vector2f(9.0F,14.0F));
	sf::RectangleShape chrct(sf::Vector2f(9.0F,14.0F));
	chrct.setTexture(tileset);
	for(int i = 0; i<(xmax*ymax); i++) // Foreach(character in memory)
	{
		u8 chr =  RAM16::RAM[(0xb0000)+(i<<1)]; // Character number
		u8 attr = RAM16::RAM[(0xb0001)+(i<<1)]; // Attribute part
		sf::Color fg(0,210,0),bg(sf::Color::Black); // Render color
		bool underline=false,high_intensity=false;
		if(attr == 0 || attr == 8 || attr == 0x80 || attr == 0x88) // Invisible
		{
			bg = fg = sf::Color::Black;
		}
		else if(attr == 0x70 || attr == 0xF0) // Black on green
		{
			bg = sf::Color::Black;
			fg = sf::Color(0,250,0);
		}
		else if(attr == 0x78 || attr == 0xF8) // Dark green on green
		{
			bg = sf::Color(0,250,0);
			fg = sf::Color(0,120,0);
		}
		else
		{
			if(attr & 0x01) underline = true;
			if(attr & 0x08) high_intensity = true;
			if(high_intensity == true) fg=sf::Color(0,250,0);
		}
		sf::Vector2f pos((i%xmax) * 9 + xoff , (i/xmax) * 14 + yoff);
		// Draws background
		fillrct.setPosition(pos);
		fillrct.setFillColor(bg);
		rw.draw(fillrct);
		// Draws character
		int chx = (chr & 63)*9;
		int chy = (chr >>5)*14;
		chrct.setTextureRect(sf::IntRect(chx,chy,9,14));
		chrct.setFillColor(fg);
		chrct.setPosition(pos);
		rw.draw(chrct);
	}
}

}


