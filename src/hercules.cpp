#include "video.h"
#include "api.h"

namespace HGC
{

void Register()
{
	Video.gfxCardRender = &tick;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};
	tileset = new sf::Texture();
	tileset->loadFromFile("gfx/mda.png");
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
	delete tileset;
}

void putpixel(sf::RenderWindow& win,int x,int y,u8 chr,sf::Color col)
{
	sf::Drawable fg0( {x,y}, {1,1},0,sf::Color::White);
	sf::Drawable bg0( {x,y}, {1,1},0,sf::Color::Black);
	if(col == 1) win.Draw(fg0);
	else win.Draw(bg0);
}

void tick(sf::RenderWindow& win)
{
	int ymax = vert_disp;
	int xmax = horz_disp + 1;
	if(hercules==1) for(int i = 0; i<ymax; i++) // Foreach(character in memory)
		{
			for(int j = 0; j<xmax; m++)
			{
				u8 col = RAM16::RAM[0xb0000 + ((j+(i*xmax))>>3)];
				col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 1);
				       putpixel(win,j,i,col);
			}
		}
	       else MDA::tick();
	framecounter++;
	if(framecounter == 16) framecounter = 0;
}


