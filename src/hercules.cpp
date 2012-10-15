#include "videocards.h"
#include "video.h"
#include "api.h"

namespace HGC
{

void render(sf::RenderWindow& rw, int xoff, int yoff);

sf::Texture* tileset = NULL;

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

void render(sf::RenderWindow& rw, int xoff, int yoff)
{
	int ymax = MDA::vert_disp*(MDA::maximum_scanline+1);
	int xmax = (MDA::horz_disp + 1)*9;
	if(hercules==1)
	{
		for(int i = 0; i<ymax; i++) // Foreach(character in memory)
		{
			for(int j = 0; j<xmax; j++)
			{
				u8 col = RAM16::RAM[0xb0000 + ((j+(i*xmax))>>3)];
				col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 1));
				sf::RectangleShape pixrct(sf::Vector2f(1,1));
				pixrct.setFillColor((col == 1) ? sf::Color::White : sf::Color::Black);
				pixrct.setPosition(sf::Vector2f(j,i));
				rw.draw(pixrct);
			}
		}
	}
	else mda::render(rw,xoff,yoff);
}
}


