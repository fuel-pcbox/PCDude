
#include "video.h"
#include "api.h"

namespace mda
{

void tick()
{
	int ymax = vert_disp;
	int xmax = horz_disp + 1;
	for(int i = 0; i<(xmax*ymax); i++) // Foreach(character in memory)
	{
		u8 chr = RAM16::RAM[(0xb0000)+(i<<1)]; // Character number
		u8 attr = RAM16::RAM[(0xb0000)+(i<<1)+1]; // Attribute part
		sf::Color fg,bg; // Render color
		bool underline;
		bool high_intensity;
		if(attr == 0 || attr == 8 || attr == 0x80 || attr == 0x88)
		{
			fg=bg = sf::Color::Black; //Black on black
		}
		if(attr == 0x70 || attr == 0xF0)
		{
			pal = sf::Color::Black; //Black on green
		}
		if(attr == 0x78 || attr == 0xF8)
		{
			pal = 0x12; //Dark green on green.
		}
		else
		{
			if(attr & 0x01) underline = 1;
			else underline = 0;
			if(attr & 0x08) high_intensity = 1;
			else high_intensity = 0;
			if(high_intensity == 1) pal = 0x20;
			else pal = 0x10;
		}
		for(int j = 0; j<9; j++)
		{
			for(int k = 0; k<14; k++)
			{
				putpixel((xmax*9)+j,(ymax*14)+k,ROM[chr+0],pal);
			}
		}
	}
}

}

