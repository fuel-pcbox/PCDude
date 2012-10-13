
#include "api.h"

namespace CPU_IRQ
{
bool irq;
u8 irq_number;
}

namespace MDA
{
u8 ROM[ROM_SIZE];

u8 textcols;
u8 textmode;
u8 color;
u8 video_enable;
u8 graphicsmode;
u8 attr_blink_mode;

u8 crtc_index;
u8 horz_total;
u8 horz_disp;
u8 horz_sync_pos;
u8 horz_sync_width;
u8 vert_total;
u8 vert_total_adjust;
u8 vert_disp;
u8 maximum_scanline;
};

namespace LPT0
{
u8 data;
};

namespace LPT1
{
u8 data;
};

namespace LPT2
{
u8 data;
};

namespace DMA_XT
{
u16 start_addr_chan0;
u16 start_addr_chan1;
u16 start_addr_chan2;
u16 start_addr_chan3;
u8 page_chan3;
bool enabled = false;
bool lohi_chan0 = false;
bool lohi_chan1 = false;
bool lohi_chan2 = false;
bool lohi_chan3 = false;
bool lohi = false;
u8 channel;
u8 trans_mode;
bool autotrans;
bool trans_dir;
u8 trans_method;
bool drq_chan0_masked = false;
bool drq_chan1_masked = false;
bool drq_chan2_masked = false;
bool drq_chan3_masked = false;
}

namespace PIT
{
u8 BCD;
u8 opmode;
u8 accmode;
u8 channel;
u16 counter;
u16 reload;
bool reload_set;
bool reload_edge;
bool lohi = false;
bool latch = false;
bool latch_lohi = false;
};

namespace PIC
{
bool single;
bool icw4_req;
u8 vector_offset;
bool initing;
bool icw2;
u8 IRQmask;
}

namespace PPI
{
u8 amode;
u8 bmode;
u8 porta;
u8 portb;
u8 portc;
u8 porta_in;
u8 portb_in;
u8 loportc_in;
u8 hiportc_in;
u8 iomode;
};

namespace IO_XT
{
u8 nmi_enabled;
};

namespace RAM16
{
u8 RAM[RAM_SIZE];
};

namespace CPU
{
int type;
bool halted = false;
anonunion_t_1 a,b,c,d;

u16 sp,bp,di,si;
u16 cs,ds,es,ss;
u16 ip;

u16 flags;
};

namespace PIT
{
void dumpPITstate()
{
	printf("counter: %04x\n",counter);
}

void tick()
{
	switch(opmode)
	{
	case 0:
	{
		if(reload_set && (!latch))
		{
			usleep(1);
			if(reload_edge)
			{
				counter = reload;
				reload_edge = false;
			}
			else
			{
				counter--;
			}
			if(counter==0)
			{
				//CPU_IRQ::irq = true;
				//CPU_IRQ::irq_number = 0;
			}
		}
		break;
	}
	case 2:
	{
		if(reload_set)
		{
			usleep(1);
			if(reload_edge)
			{
				counter = reload;
				reload_edge = false;
			}
			else
			{
				counter--;
				if(counter == 1)
				{
					usleep(1);
					counter = reload;
					//CPU_IRQ::irq = true;
					//CPU_IRQ::irq_number = 0;
				}
			}
		}
		break;
	}
	case 3:
	{
		if(reload_set)
		{
			usleep(1);
			if(reload_edge)
			{
				counter = reload;
				reload_edge = false;
			}
			else
			{
				counter-=2;
				if(counter==1)
				{
					usleep(1);
					counter = reload;
					//CPU_IRQ::irq = true;
					//CPU_IRQ::irq_number = 0;
				}
			}
		}
		break;
	}
	default:
	{
		printf("PIT: Unimplemented operation mode %02x!\n",opmode);
		break;
	}
	}
	dumpPITstate();
}
}

namespace IO_XT
{
#undef addr
void wb(u16 addr, u8 value)
{
	if(addr == 0x0000)
	{
		if(DMA_XT::lohi_chan0 == false)
		{
			DMA_XT::start_addr_chan0 = (DMA_XT::start_addr_chan0 & 0xFF00) | value;
			DMA_XT::lohi_chan0 = true;
		}
		else
		{
			DMA_XT::start_addr_chan0 = (DMA_XT::start_addr_chan0 & 0x00FF) | (value<<8);
			DMA_XT::lohi_chan0 = false;
		}
	}
	if(addr == 0x0001)
	{
		if(DMA_XT::lohi_chan1 == false)
		{
			DMA_XT::start_addr_chan1 = (DMA_XT::start_addr_chan1 & 0xFF00) | value;
			DMA_XT::lohi_chan1 = true;
		}
		else
		{
			DMA_XT::start_addr_chan1 = (DMA_XT::start_addr_chan1 & 0x00FF) | (value<<8);
			DMA_XT::lohi_chan1 = false;
		}
	}
	if(addr == 0x0002)
	{
		if(DMA_XT::lohi_chan2 == false)
		{
			DMA_XT::start_addr_chan2 = (DMA_XT::start_addr_chan2 & 0xFF00) | value;
			DMA_XT::lohi_chan2 = true;
		}
		else
		{
			DMA_XT::start_addr_chan2 = (DMA_XT::start_addr_chan2 & 0x00FF) | (value<<8);
			DMA_XT::lohi_chan2 = false;
		}
	}
	if(addr == 0x0003)
	{
		if(DMA_XT::lohi_chan3 == false)
		{
			DMA_XT::start_addr_chan3 = (DMA_XT::start_addr_chan3 & 0xFF00) | value;
			DMA_XT::lohi_chan3 = true;
		}
		else
		{
			DMA_XT::start_addr_chan3 = (DMA_XT::start_addr_chan3 & 0x00FF) | (value<<8);
			DMA_XT::lohi_chan3 = false;
		}
	}
	if(addr == 0x0004)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::start_addr_chan0 = (DMA_XT::start_addr_chan0 & 0xFF00) | value;
			DMA_XT::lohi = true;
		}
		else
		{
			DMA_XT::start_addr_chan0 = (DMA_XT::start_addr_chan0 & 0x00FF) | (value<<8);
			DMA_XT::lohi = false;
		}
	}
	if(addr == 0x0005)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::start_addr_chan1 = (DMA_XT::start_addr_chan1 & 0xFF00) | value;
			DMA_XT::lohi = true;
		}
		else
		{
			DMA_XT::start_addr_chan1 = (DMA_XT::start_addr_chan1 & 0x00FF) | (value<<8);
			DMA_XT::lohi = false;
		}
	}
	if(addr == 0x0006)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::start_addr_chan2 = (DMA_XT::start_addr_chan2 & 0xFF00) | value;
			DMA_XT::lohi = true;
		}
		else
		{
			DMA_XT::start_addr_chan2 = (DMA_XT::start_addr_chan2 & 0x00FF) | (value<<8);
			DMA_XT::lohi = false;
		}
	}
	if(addr == 0x0007)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::start_addr_chan3 = (DMA_XT::start_addr_chan3 & 0xFF00) | value;
			DMA_XT::lohi = true;
		}
		else
		{
			DMA_XT::start_addr_chan3 = (DMA_XT::start_addr_chan3 & 0x00FF) | (value<<8);
			DMA_XT::lohi = false;
		}
	}
	if(addr == 0x0008)
	{
		DMA_XT::enabled = (value & 4) ? false : true;
	}
	if(addr == 0x000A)
	{
		switch(value & 3)
		{
		case 0:
		{
			DMA_XT::drq_chan0_masked = (value & 4) >> 2;
			break;
		}
		}
	}
	if(addr == 0x000B)
	{
		DMA_XT::channel = value & 3;
		DMA_XT::trans_mode = (value & 0x0C) >> 2;
		DMA_XT::autotrans = (value & 0x10) >> 4;
		DMA_XT::trans_dir = (value & 0x20) >> 5;
		DMA_XT::trans_method = (value & 0xC0) >> 6;
	}
	if(addr == 0x0020)
	{
		if(value & 0x10)
		{
			PIC::single = ((value & 2) >> 1) ? false : true;
			PIC::icw4_req = (value & 1) ? true : false;
			PIC::initing = true;
			PIC::icw2 = true;
		}
	}
	if(addr == 0x0021)
	{
		if(PIC::initing && PIC::icw2)
		{
			PIC::vector_offset = value;
			PIC::icw2 = false;
			if(!PIC::icw4_req) PIC::initing = false;
		}
		else
		{
			PIC::IRQmask = ~value;
		}
	}
	if(addr == 0x0040)
	{
		if(PIT::channel == 0)
		{
			if(PIT::accmode == 3)
			{
				if(PIT::lohi == false)
				{
					PIT::reload = (PIT::reload&0xFF00) | value;
					PIT::lohi = true;
				}
				else
				{
					PIT::reload = (PIT::reload&0x00FF) | (value<<8);
					PIT::reload_set = true;
					PIT::reload_edge = true;
					PIT::lohi = false;
				}
			}
		}
	}
	if(addr == 0x0041)
	{
		if(PIT::channel == 1)
		{
			if(PIT::accmode == 1)
			{
				PIT::reload = (PIT::reload&0xFF00) | value;
				PIT::reload_set = true;
				PIT::reload_edge = true;
			}
		}
	}
	if(addr == 0x0043)
	{
		PIT::BCD = value & 1;
		PIT::opmode = (value & 0x0E) >> 1;
		PIT::accmode = (value & 0x30) >> 4;
		PIT::channel = (value & 0xC0) >> 6;
		if(PIT::accmode == 0) PIT::latch = true;
	}
	if(addr == 0x0061)
	{
		if(PPI::iomode==0)
		{
			PPI::portb = value;
		}
	}
	if(addr == 0x0063)
	{
		PPI::iomode = (value & 0x80) >> 7;
		if(PPI::iomode)
		{
			PPI::amode = (value & 0x60) >> 5;
			PPI::bmode = (value & 4) >> 2;
			PPI::loportc_in = (value & 1);
			PPI::portb_in = (value & 2) >> 1;
			PPI::hiportc_in = (value & 8) >> 3;
			PPI::porta_in = (value & 0x10) >> 4;
		}
	}
	if(addr == 0x0083)
	{
		DMA_XT::page_chan3 = value & 0x0F;
	}
	if(addr == 0x00A0)
	{
		nmi_enabled = value & 0x80;
		printf("NMI write %02x!\n",value);
	}
	if(addr == 0x0278)
	{
		LPT2::data = value;
	}
	if(addr == 0x0378)
	{
		LPT1::data = value;
	}
	if(addr == 0x03B4)
	{
		MDA::crtc_index = value;
	}
	if(addr == 0x03B5)
	{
		if(MDA::crtc_index == 0) MDA::horz_total = value;
		if(MDA::crtc_index == 1) MDA::horz_disp = value;
		if(MDA::crtc_index == 2) MDA::horz_sync_pos = value;
		if(MDA::crtc_index == 3) MDA::horz_sync_width = value;
		if(MDA::crtc_index == 4) MDA::vert_total = value;
		if(MDA::crtc_index == 5) MDA::vert_total_adjust = value;
		if(MDA::crtc_index == 6) MDA::vert_disp = value;
		if(MDA::crtc_index == 9) MDA::maximum_scanline = value;
	}
	if(addr == 0x03B8)
	{
		MDA::textcols = value & 1;
		MDA::textmode = (value & 2) >> 1;
		MDA::color = (value & 4) >> 2;
		MDA::video_enable = (value & 8) >> 3;
		MDA::graphicsmode = (value & 0x10) >> 4;
		MDA::attr_blink_mode = (value & 0x20) >> 5;
		printf("MDA mode control write %02x!\n",value);
	}
	if(addr == 0x03BC)
	{
		LPT0::data = value;
	}
}

u8 rb(u16 addr)
{
	if(addr == 0x0000)
	{
		if(DMA_XT::lohi_chan0 == false)
		{
			DMA_XT::lohi_chan0 = true;
			return DMA_XT::start_addr_chan0 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi_chan0 = false;
			return (DMA_XT::start_addr_chan0 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0001)
	{
		if(DMA_XT::lohi_chan1 == false)
		{
			DMA_XT::lohi_chan1 = true;
			return DMA_XT::start_addr_chan1 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi_chan1 = false;
			return (DMA_XT::start_addr_chan1 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0002)
	{
		if(DMA_XT::lohi_chan2 == false)
		{
			DMA_XT::lohi_chan2 = true;
			return DMA_XT::start_addr_chan2 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi_chan2 = false;
			return (DMA_XT::start_addr_chan2 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0003)
	{
		if(DMA_XT::lohi_chan3 == false)
		{
			DMA_XT::lohi_chan3 = true;
			return DMA_XT::start_addr_chan3 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi_chan3 = false;
			return (DMA_XT::start_addr_chan3 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0004)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::lohi = true;
			return DMA_XT::start_addr_chan0 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi = false;
			return (DMA_XT::start_addr_chan0 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0005)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::lohi = true;
			return DMA_XT::start_addr_chan1 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi = false;
			return (DMA_XT::start_addr_chan1 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0006)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::lohi = true;
			return DMA_XT::start_addr_chan2 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi = false;
			return (DMA_XT::start_addr_chan2 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0007)
	{
		if(DMA_XT::lohi == false)
		{
			DMA_XT::lohi = true;
			return DMA_XT::start_addr_chan3 & 0x00FF;
		}
		else
		{
			DMA_XT::lohi = false;
			return (DMA_XT::start_addr_chan3 & 0xFF00)>>8;
		}
	}
	if(addr == 0x0041)
	{
		if(PIT::latch)
		{
			if(PIT::latch_lohi == false)
			{
				PIT::latch_lohi = true;
				return PIT::counter & 0xFF;
			}
			else
			{
				PIT::latch_lohi = false;
				PIT::latch = false;
				return PIT::counter >> 8;
			};
		}
	}
	if(addr == 0x0060)
	{
		if(PPI::porta_in)
		{
			return PPI::porta;
		}
	}
	if(addr == 0x0061)
	{
		if(PPI::portb_in)
		{
			return PPI::portb;
		}
	}
	if(addr == 0x0062)
	{
		return 0xFF; //TODO: Hardcoded for 4 floppy drives, MDA, 4 RAM banks, an 8087 and floppy installed.
	}
	if(addr == 0x0278)
	{
		return LPT2::data;
	}
	if(addr == 0x0378)
	{
		return LPT1::data;
	}
	if(addr == 0x03BC)
	{
		return LPT0::data;
	}
	return 0;
}

}

namespace CPU
{

int InitCPU(int t)
{
	type = t;
	if(type == 0) flags = 0xF002;
	if(type < 2)
	{
		cs = 0xF000;
		ip = 0xFFF0;
	}
}

void dumpregs()
{
	printf("ax: %04x\n",ax);
	printf("bx: %04x\n",bx);
	printf("cx: %04x\n",cx);
	printf("dx: %04x\n",dx);
	printf("cs: %04x\n",cs);
	printf("ip: %04x\n",ip);
	printf("ss: %04x\n",ss);
	printf("sp: %04x\n",sp);
	printf("ds: %04x\n",ds);
	printf("es: %04x\n",es);
	printf("di: %04x\n",di);
	printf("si: %04x\n",si);
	printf("bp: %04x\n",bp);
	printf("flags: %04x\n",flags);
	printf("addr: %08x\n",((cs<<4)+ip));
}


void inc_addr()
{
	if(ip == 0xFFFF)
	{
		cs+=0x1000;
		ip = 0;
	}
	else ip++;
}

void dumpivt()
{
	for(int i = 0; i<0x400; i++)
	{
		printf("0x%02x\n",RAM16::RAM[i]);
	}
}

}



