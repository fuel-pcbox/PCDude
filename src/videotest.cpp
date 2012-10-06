#include <cstdlib>
#include "includes.h"
#include "video.h"
#include "api.h"

int main(int ac, char** av)
{
	bool mda,cga,ega,hgc;
	FILE* rom = fopen(av[1],"rb");
	if(ac < 3)
	{
		std::string arg = av[2];
		if(arg == "--mda")
		{
			mda = true;
			u8* MDA_ROM_ptr = MDA::ROM;
			fread(MDA_ROM_ptr,1,0x2000,rom);
			MDA::Register();
			MDA::textcols = 1;
			MDA::horz_disp = 79;
			MDA::vert_disp = 25;
			MDA::video_enable = 1;
		}
		/*		if(arg == "--hgc")
				{
					hgc = true;
					u8* MDA_ROM_ptr = MDA::ROM;
					fread(MDA_ROM_ptr,1,0x2000,rom);
					HGC::Register();
					MDA::textcols = 1;
					MDA::horz_disp = 720;
					MDA::vert_disp = 347;
					HGC::hercules = 1;
					MDA::video_enable = 1;
				}
				if(arg == "--cga")
				{
					cga = true;
					u8* CGA_ROM_ptr = CGA::ROM;
					fread(CGA_ROM_ptr,1,0x2000,rom);
					CGA::Register();
					CGA::textcols = 0;
					CGA::textmode = 1;
					CGA::color = 0;
					CGA::horz_disp = 320;
					CGA::vert_disp = 199;
					CGA::video_enable = 1;
				}
				if(arg == "--ega")
				{
					ega = true;
					EGA::Register();
					EGA::crtc.horz_disp_end = 321;
					EGA::crtc.vert_disp_end = 200;
				}*/
	}
	fclose(rom);
	Video.Init();

	srand(0xFD89E930);

	for(;;)
	{
		if(mda == true) for(int i = 0; i<0x8000; i++)
			{
				RAM16::RAM[i + 0xB0000] = rand() & 0xFF;
			}
		/*
		if(cga == true) for(int i = 0;i<16000;i++)
		{
			RAM16::RAM[i + 0xB8000] = rand() & 0xFF;
		}
		if(ega == true) for(int i = 0;i<16000;i++)
		{
			RAM16::RAM[i + 0xA0000] = rand() & 0xFF;
		}
		*/
		Video.RefreshScreen();
	}
}