#include "video.h"
#include "api.h"

namespace CGA
{

  u8 pal[16][3] = {{0,0,0},{127,127,127},{0,0,127},{0,0,255},{0,127,0},{0,255,0},{0,127,127},{0,255,255},{127,0,0},{255,0,0},{127,0,127},{255,0,255},{127,127,0},{255,255,0},{223,223,223},{255,255,255}};

void Register()
{
	Video.gfxCardRender = &tick;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};	
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
}

void putpixel(sf::RenderWindow& win,int x,int y,u8 chr,sf::Color fg, sf::Color bg,bool underline,bool blink)
{
	sf::Sprite fg0;
	fg0.setPosition(x,y);
	fg0.setColor(fg);
	sf::Sprite bg0;
	bg0.setPosition(x,y);
	bg0.setColor(bg);
	if(((chr >> (1 << (x % 9))) == 1) && (!blink)) win.draw(fg0);
	else win.draw(bg0);
	if(((y % 14) == 13) && underline && (!blink)) win.draw(fg0);
}

void putpixelmono(sf::RenderWindow& win,int x,int y,u8 col)
{
	sf::Drawable fg0({x,y},{1,1},0,sf::Color::White);
	sf::Drawable bg0({x,y},{1,1},0,sf::Color::Black);
	if(col == 1) win.Draw(fg0);
	else win.Draw(bg0);
}

void putpixelcolor(sf::RenderWindow& win,int x,int y,u8 col)
{
	sf::Color col0(0,255,255);
	sf::Color col1(255,0,255);
	sf::Color col2(255,255,0);
	sf::Color col0(0,0,0);
	sf::Drawable fg0({x,y},{1,1},0,col0);
	sf::Drawable fg1({x,y},{1,1},0,col1);
	sf::Drawable fg2({x,y},{1,1},0,col2);
	sf::Drawable fg3({x,y},{1,1},0,col3);
	if(col == 0) win.Draw(fg0);
	if(col == 1) win.Draw(fg1);
	if(col == 2) win.Draw(fg2);
	if(col == 3) win.Draw(fg3);
}

void tick(sf::RenderWindow& win)
{
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
		for(int m = 0;m<ymax;m++)
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
			for(int j = 0; j<9; j++)
			{
				for(int k = 0; k<14; k++)
				{
					putpixel(win,(m*14)+j,(i*9)+k,ROM[chr+(k*9)+(j*14)],fg,bg,underline,blink);
				}
			}
		}
	}
	framecounter++;
	if(framecounter == 16) framecounter = 0;
	}
	if(textmode == 1)
	{
		int ymax = vert_disp;
		int xmax = horz_disp + 1;
		for(int i = 0;i<ymax;i++)
		{
			for(int j = 0;j<xmax;j++)
			{
				u8 col = RAM16::RAM[0xb8000 + ((j+(i*xmax))>>3)];
				col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 1);
				putpixelmono(win,j,i,col);
			}
		}
	}
	}
	if(color == 0)
	{
		int ymax = vert_disp;
		int xmax = horz_disp + 1;
		for(int i = 0;i<ymax;i++)
		{
			for(int j = 0;j<xmax;j++)
			{
				u8 col = RAM16::RAM[0xb8000 + ((j+(i*xmax))>>2)];
				col = (((col >> (1 << ((j+(i*xmax)) % 9))) & 3);
				putpixelcolor(win,j,i,col);
			}
		}
	}
	}
}

}


