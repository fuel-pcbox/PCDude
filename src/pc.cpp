
#include "includes.h"
#include "video.h"
#include "api.h"

namespace EMULATOR
{
Jzon::Object settings;
std::string emu_path;
std::string emu_file;
}

using namespace EMULATOR;

void fn_void_0() {}

int main(int argc,char** argv, char** envv)
{
	std::string emup;
	decltype(&fn_void_0) ureg;
	if(argc<=2)
	{
		PCLOG("Usage: %s [EMULATION DIRECTORY] [JSON FILE]",argv[0]);
		return 1;
	}
	else
	{
		emu_path = argv[1];
		emu_path.push_back('/');
		emu_file = argv[2];
		emup = emu_path+emu_file;
		PCLOG("Loading emulation description from: %s.",emup.c_str());
	}
	Jzon::FileReader::ReadFile(emup, settings);

	std::string biosfn = emu_path + settings.Get("bios").ToString();
	PCLOG("Loading BIOS image from %s..." , biosfn.c_str());
	FILE* bios = fopen( biosfn.c_str() ,"rb");
	u8* RAM_ptr = RAM16::RAM;
	fread(RAM_ptr+0xfe000,1,0x2000,bios);
	PCLOG1("Done!");

	std::string gfxcard=settings.Get("graphics").AsObject().Get("card").ToString();
	if(gfxcard=="mda")
	{
		mda::Register();
		ureg = &mda::Unregister;
	}

	fclose(bios);

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
			else
			{
				Video.gui.HandleEvent(Video.ev);
			}
		}
		Video.RefreshScreen();
		PIT::tick();
	}
}

