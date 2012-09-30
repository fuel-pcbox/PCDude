
#include "includes.h"
#include "video.h"
#include "api.h"

int main()
{
	FILE* bios = fopen("pcxt.rom","rb");
	u8* RAM_ptr = RAM16::RAM;
	fread(RAM_ptr+0xfe000,1,0x2000,bios);

	FILE* mda_rom = fopen("5788005.u33","rb");
	u8* MDA_ROM_ptr = MDA::ROM;
	fread(MDA_ROM_ptr,1,0x2000,mda_rom);

	fclose(bios);
	fclose(mda_rom);

	CPU::InitCPU(0);
	Video.Init();
	Video.CreateScreen(720,350);

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
#ifdef VIDEO
		while(Video.PollEvent()) if(Video.e.type == SDL_QUIT)
			{
				return 2;
			}
#endif
		Video.RefreshScreen();
		PIT::tick();
	}
}
