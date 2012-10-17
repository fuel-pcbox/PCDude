#include "video.h"
#include "api.h"

namespace hercules
{

// Tiles -> 9x14

sf::Texture* tileset;
void render(sf::RenderWindow & rw, int xoff, int yoff);
void Register();
void Unregister();
constexpr int ymax = 25;
constexpr int xmax = 80;
sf::Uint8 pixs[720*384*4];
sf::Texture* pixt;

void Register()
{
	Video.gfxCardRender = &render;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};
	tileset = new sf::Texture();
	tileset->loadFromFile("gfx/mda.png");
	pixt=new sf::Texture();
	pixt->create(720,384);
	IO_XT::wb(0x3BA,bin<10000110>::value);
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
	delete tileset;
}

void render(sf::RenderWindow & rw, int xoff, int yoff)
{
	u8 rg1 = IO_XT::rb(0x3B8);
	u8 rg2 = IO_XT::rb(0x3BF);
	if(((rg1&(1<<3))==0))
	{
		return;   // Disabled gfx
	}
	bool ful1 = ((rg2&2)>0);
	bool ful2 = ((rg1&(1<<7))>0);
	bool gfl = ((rg2&1)>0);
	bool blk = ((rg1&(1<<5))>0);
	int RamOff = (ful1)? ( (ful2) ? ( 0xB8000 ) : ( 0xB000 ) ) : ( 0xB8000 );
	bool mode = ful2&&gfl;
	if(!mode)
	{
		static int frame = -1;
		frame++;
		frame &= 15; // frame %= 16;
		sf::RectangleShape fillrct(sf::Vector2f(9.0F,14.0F));
		sf::RectangleShape chrct(sf::Vector2f(9.0F,14.0F));
		chrct.setTexture(tileset);
		for(int i = 0; i<(xmax*ymax); i++) // Foreach(character in memory)
		{
			u8 chr =  RAM16::RAM[(RamOff)+(i<<1)]; // Character number
			u8 attr = RAM16::RAM[(RamOff+1)+(i<<1)]; // Attribute part
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
			if(blk && blink && (frame<8))
			{
				fg=bg;
			}
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
	else // Graphics mode 720x384
	{
		for(int i=0; i<34560; i++)
		{
			u8 px = RAM16::RAM[0xB0000+i];
			for(int j=0; j<8; j++)
			{
				u8 p = (px>>j)&1;
				int n = ((i*8)+j)*4;
				if(p==1)
				{
					pixs[n]=pixs[n+1]=pixs[n+2]=pixs[n+3]=255;
				}
				else
				{
					pixs[n]=pixs[n+1]=pixs[n+2]=pixs[n+3]=0;
				}
			}
		}
		pixt->update(&pixs[0]);
		static sf::RectangleShape drw(sf::Vector2f(720.0F,384.0F));
		drw.setTexture(pixt);
		drw.setPosition(sf::Vector2f(xoff,yoff));
		rw.draw(drw);
	}
}

}

