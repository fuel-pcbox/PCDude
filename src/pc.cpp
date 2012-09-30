
#include "includes.h"
#include "video.h"
#include "api.h"

int main(int argc,char** argv, char** envv)
{
	std::string biosfn="pcxt.rom",mdafn="5788005.u33";
	if(argc==1)
	{
		printf("Defaulting BIOS to pcxt.rom and MDA ROM to 5788005.u33\nIf you want to change, then relaunch with: %s [BIOS] [MDA ROM]",argv[0]);
	}
	else if(argc==2)
	{
		biosfn = argv[1];
		printf("Loading BIOS from %s, defaulting MDA ROM to 5788005.u33\nIf you want to change, then relaunch with: %s %s [MDA ROM]",argv[1],argv[0],argv[1]);
	}
	else
	{
		biosfn = argv[1];
		mdafn  = argv[2];
		printf("Loading BIOS from %s, MDA ROM from %s.",argv[1],argv[2]);
	}
	FILE* bios = fopen(biosfn.c_str(),"rb");
	u8* RAM_ptr = RAM16::RAM;
	fread(RAM_ptr+0xfe000,1,0x2000,bios);

	FILE* mda_rom = fopen(mdafn.c_str(),"rb");
	u8* MDA_ROM_ptr = MDA::ROM;
	fread(MDA_ROM_ptr,1,0x2000,mda_rom);

	fclose(bios);
	fclose(mda_rom);

	CPU::InitCPU(0);
	Video.Init();

	for(;;)
	{
		for(int i = 0; i<4; i++)
		{
			CPU::dumpregs();
			if(CPU::tick() == 1)
			{
				return 1;
			}
		}
		while(Video.PollEvent())
		{
			if(Video.ev.type==sf::Event::Closed)
			{
				return 2;
			}
		}
		Video.RefreshScreen();
		PIT::tick();
	}
}
