
#include "video.h"
#include "api.h"
#include "mda.hxbm"

namespace mda
{



// Tiles -> 9x14

sf::Texture* tileset;
void render(sf::RenderWindow & rw, int xoff, int yoff);
void Register();
void Unregister();
constexpr int ymax = 25;
constexpr int xmax = 80;
constexpr int bpc  = 9*14; //Bits per character
constexpr int ymul = mdabmp_width/8;
sf::Uint8 tex[720*350*4];
std::bitset<mdabmp_width*mdabmp_height> bts;

void Register()
{
	Video.gfxCardRender = &render;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};
	tileset = new sf::Texture();
	tileset->create(720,350);
	IO_XT::wb(0x3BA,bin<11110110>::value);
	for(int i=0;i<(mdabmp_width*mdabmp_height)/8;i++)
	{
		auto B = mdabmp_bits[i];
		for(int j=0;j<8;j++)
		{
			bts[(i<<3)+j] = B&(1<<j);
		}
	}
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
	delete tileset;
}

void render(sf::RenderWindow & rw, int xoff, int yoff)
{
	u8 rgstr = IO_XT::rb(0x3B8);
	if(((rgstr&(1<<3))==0))
	{
		return;
	}
	bool blk = ((rgstr&(1<<5))>0);
	static int frame = -1;
	frame++;
	frame &= 15; // frame %= 16;
	int RamOff = 0xB0000;
	sf::RectangleShape chrct(sf::Vector2f(720.0F,350.0F));
	chrct.setPosition(sf::Vector2f(xoff,yoff));
	for(int i = 0; i<(xmax*ymax); i++) // Foreach(character in memory)
	{
		u8 chr =  RAM16::RAM[RamOff+(i<<1)]; // Character number
		u8 attr = RAM16::RAM[RamOff+1+(i<<1)]; // Attribute part
		u8 fg(255),bg(0); // Render color
		bool underline=false,high_intensity=false,blink=false;
		if(attr == 0 || attr == 8 || attr == 0x80 || attr == 0x88) // Invisible
		{
			bg = fg = 0;
		}
		else if(attr == 0x70 || attr == 0xF0) // Black on green
		{
			bg = 255;
			fg = 0;
			if(attr == 0xF0) blink=true;
		}
		else if(attr == 0x78 || attr == 0xF8) // Dark green on green
		{
			bg = 255;
			fg = 127;
			if(attr == 0xF8) blink=true;
		}
		else
		{
			if(attr & 0x01) underline = true;
			if(attr & 0x08) high_intensity = true;
			if(attr & 0x80) blink = true;
			if(high_intensity == true) fg=255;
			else fg=127;
		}
		if(blk && blink && (frame<8))
		{
			fg=bg;
		}
		int oo = ((i%xmax) * 9 + xoff)+(((i/xmax) * 14 + yoff)*720);
		int chx = (chr & 63)*9;
		int chy = ((chr >>5)*14)*mdabmp_width;
		
		for(int ix=0;ix<9;ix++)
			for(int iy=0;iy<14;iy++)
			{
				int CRD = (iy*9+ix)+chx+chy;
				tex[((oo+ix+(iy*720))<<2)+0] = (bts[CRD]==1)?(bg):(fg);
				tex[((oo+ix+(iy*720))<<2)+1] = (bts[CRD]==1)?(bg):(fg);
				tex[((oo+ix+(iy*720))<<2)+2] = (bts[CRD]==1)?(bg):(fg);
				tex[((oo+ix+(iy*720))<<2)+3] = 255;
			}

		/*if(underline)
		{
			fillrct.setSize(sf::Vector2f(7.0F,1.0F));
			fillrct.setPosition(pos+sf::Vector2f(1.0F,13.0F));
			fillrct.setFillColor(fg);
			rw.draw(fillrct);
		}*/
		
	}
	tileset->update(tex);
	chrct.setTexture(tileset);
	rw.draw(chrct);
	
}

}


