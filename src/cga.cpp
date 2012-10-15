#include "video.h"
#include "api.h"

namespace CGA
{

u8 pal[16][3] = {{0,0,0},{127,127,127},{0,0,127},{0,0,255},{0,127,0},{0,255,0},{0,127,127},{0,255,255},{127,0,0},{255,0,0},{127,0,127},{255,0,255},{127,127,0},{255,255,0},{223,223,223},{255,255,255}};

void Register()
{
	Video.gfxCardRender = &render;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(640,200);};
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
}

void render(sf::RenderWindow& rw, int xoff, int yoff)
{
	static int framecounter = -1;
	framecounter++;
	framecounter &= 15;
	if(video_enable == 1)
	{
		if(color == 1)
		{
			if(textmode == 0)
			{
				int ymax = vert_disp;
				int xmax = horz_disp + 1;
				for(int i = 0; i<xmax; i++) // Foreach(character in memory)
				{
					for(int m = 0; m<ymax; m++)
					{
						u8 chr = RAM16::RAM[(0xb8000)+((i+(m*xmax))<<1)]; // Character number
						u8 attr = RAM16::RAM[(0xb8000)+((i+(m*xmax))<<1)+1]; // Attribute part
						sf::Color fg,bg; // Render color
						bool high_intensity;
						bool underline;
						bool blink = false;
						if(attr == 0 || attr == 8 || attr == 0x80 || attr == 0x88) // Invisible
						{
							fg=bg = sf::Color::Black;
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
						else if(attr == 0x70) // Black on green
						{
							bg.r = 255;
							bg.g = 255;
							bg.b = 255;

							fg = sf::Color::Black;
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
						if(attr == 0xF0)
						{
							bg.r = 255;
							bg.g = 255;
							bg.b = 255;

							fg = sf::Color::Black;
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
						else if(attr == 0x78) // Dark green on green
						{
							fg.r = 127;
							fg.g = 127;
							fg.b = 127;

							bg.r = 255;
							bg.g = 255;
							bg.b = 255;
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
						if(attr == 0xF0)
						{
							fg.r = 127;
							fg.g = 127;
							fg.b = 127;

							bg.r = 255;
							bg.g = 255;
							bg.b = 255;
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
						else
						{
							if(attr & 0x01) underline = 1;
							else underline = 0;
							if(attr & 0x08) high_intensity = 1;
							else high_intensity = 0;
							if(high_intensity == 1)
							{
								fg.r = 255;
								fg.g = 255;
								fg.b = 255;

								bg.r = 0;
								bg.g = 0;
								bg.b = 0;
							}
							else
							{
								fg.r = 127;
								fg.g = 127;
								fg.b = 127;

								bg.r = 0;
								bg.g = 0;
								bg.b = 0;
							}
							if((framecounter < 8) && (attr & 0x80)) blink = true;
						}
					}
				}
			}
			if(textmode == 1)
			{
				int ymax = vert_disp * (maximum_scanline + 1);
				int xmax = (horz_disp + 1) * 8;
				for(int i = 0; i<ymax; i++)
				{
					for(int j = 0; j<xmax; j++)
					{
						u8 col = RAM16::RAM[0xb8000 + ((j+(i*xmax))>>3)];
						col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 1));
						sf::RectangleShape pixrct(sf::Vector2f(1,1));
						pixrct.setFillColor((col == 1) ? sf::Color::White : sf::Color::Black);
						pixrct.setPosition(sf::Vector2f(j,i));
						rw.draw(pixrct);
					}
				}
			}
		}
		if(color == 0)
		{
			int ymax = vert_disp;
			int xmax = horz_disp + 1;
			for(int i = 0; i<ymax; i++)
			{
				for(int j = 0; j<xmax; j++)
				{
					u8 col = RAM16::RAM[0xb8000 + ((j+(i*xmax))>>2)];
					col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 3));
				}
			}
		}
	}
}

}


