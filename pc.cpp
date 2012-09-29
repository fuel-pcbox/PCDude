#include <cstdio>
#include <cstring>
#include <cstdint>
#include <unistd.h>

#ifdef VIDEO
#include <SDL/SDL.h>
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t s8;
typedef int16_t s16;

#ifdef VIDEO
SDL_Surface* screen = NULL;
SDL_Event e;
#endif

namespace CPU_IRQ
{ 
        bool irq;
        u8 irq_number;
}

namespace MDA
{
        u8 ROM[0x2000];
        
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
                                        if(reload_edge){counter = reload; reload_edge = false;}
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
                                        if(reload_edge){counter = reload; reload_edge = false;}
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
                                        if(reload_edge){counter = reload; reload_edge = false;}
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
};

namespace RAM16
{
	u8 RAM[0x100000];
};

namespace CPU
{
	const int i8086 = 0;
	const int i186 = 1;
	const int i286 = 2;
	const int intel386 = 3; //Not i386, that makes GCC mad for some stupid reason.
	int type;
	bool halted = false;
	union
	{
		struct
		{
			u8 lo,hi;
		} parts;
		u16 whole;
	} a,b,c,d;

#define ax a.whole
#define ah a.parts.hi
#define al a.parts.lo

#define bx b.whole
#define bh b.parts.hi
#define bl b.parts.lo

#define cx c.whole
#define ch c.parts.hi
#define cl c.parts.lo

#define dx d.whole
#define dh d.parts.hi
#define dl d.parts.lo

	u16 sp,bp,di,si;
	u16 cs,ds,es,ss;
	u16 ip;

	u16 flags;

	int InitCPU(int t)
	{
		type = t;
		if(type == 0) flags = 0xF002;
		if(type < 2) {cs = 0xF000; ip = 0xFFF0;}
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
	}
	void dumpivt()
	{
	        for(int i = 0;i<0x400;i++)
	        {
	                printf("0x%02x\n",RAM16::RAM[i]);
	        }
	}	
	
#define addr ((cs<<4)+ip)

	void inc_addr()
	{
		if(ip == 0xFFFF)
		{
			cs+=0x1000;
			ip = 0;
		}
		else ip++;
	}

	int tick()
	{
		switch(type)
		{
			case 0:
			{
				if(!halted){u8 op = RAM16::RAM[addr];
				switch(op)
				{
				        case 0x02:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0x07:
				                        {
				                                printf("ADD AL,BYTE PTR DS:[BX]\n");
				                                al += RAM16::RAM[(ds<<4)+bx];
				                                break;
				                        }
				                }
				                ip+=2;
				                break;  
				        }
				        case 0x03:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC3:
				                        {
				                                printf("ADD AX,BX\n");
				                                ax += bx;
				                                if(ax==0) flags |= 0x0040;
        				                        else flags &= 0xFFBF;
        				                        if(ax>=0x8000) flags |= 0x0001;
        				                        else flags &= 0xFFFE;
        				                        u16 tmp = 0;
		        		                        for(int i = 0;i<16;i++)
			        	                        {
				                                        tmp += (ax&(1<<i))>>i;
				                                }
        		        		                if(!(tmp&1)) flags |= 0x0004;
	                			                else flags &= 0xFFFB;
				                                break;
				                        }
				                        case 0xF3:
				                        {
				                                printf("ADD SI,BX\n");
				                                si += bx;
				                                if(si==0) flags |= 0x0040;
        				                        else flags &= 0xFFBF;
        				                        u16 tmp = 0;
		        		                        for(int i = 0;i<16;i++)
			        	                        {
				                                        tmp += (si&(1<<i))>>i;
				                                }
        		        		                if(!(tmp&1)) flags |= 0x0004;
	                			                else flags &= 0xFFFB;
				                                break;
				                        }
				                }
				                ip+=2;
				                break;
				        }
				        case 0x04:
				        {
				                printf("ADD AL,%02x\n",RAM16::RAM[addr+1]);
				                al += RAM16::RAM[addr+1];
				                if(al==0) flags |= 0x0040;
				                else flags &= 0xFFBF;
				                u8 tmp = 0;
				                for(int i = 0;i<8;i++)
				                {
				                        tmp += (al&(1<<i))>>i;
				                }
				                if(!(tmp&1)) flags |= 0x0004;
				                else flags &= 0xFFFB;
				                ip+=2;
				                break;
				        }
				        case 0x05:
				        {
				                printf("ADD AX,%04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
				                ax += (RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1];
				                if(ax==0) flags |= 0x0040;
				                else flags &= 0xFFBF;
				                u16 tmp = 0;
				                for(int i = 0;i<16;i++)
				                {
				                        tmp += (ax&(1<<i))>>i;
				                }
				                if(!(tmp&1)) flags |= 0x0004;
				                else flags &= 0xFFFB;
				                ip+=3;
				                break;
				        }
				        case 0x06:
					{
					        printf("PUSH ES\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = es & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = es >> 8;
					        ip+=1;
					        break;
					}
					case 0x07:
					{
					        printf("POP ES\n");
					        es  = (RAM16::RAM[(ss<<4)+sp]<<8)|RAM16::RAM[(ss<<4)+sp+1];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x08:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0x06:
				                        {
				                                printf("OR BYTE PTR DS:%04x,AL\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
				                                RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] |= al;
				                                ip+=2;
				                                break;
				                        }
				                        case 0xC0:
				                        {
				                                printf("OR AL,AL\n");
				                                break;
				                        }
				                        case 0xC4:
				                        {
				                                printf("OR AH,AL\n");
				                                ah |= al;
				                                break;
				                        }
				                        case 0xD8:
				                        {
				                                printf("OR AL,BL\n");
				                                al |= bl;
				                                break;
				                        }
				                }
                                                ip+=2;
                                                break;
				        }
				        case 0x0A:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC0:
				                        {
				                                printf("OR AL,AL\n");
				                                break;
				                        }
				                        case 0xC4:
				                        {
				                                printf("OR AL,AH\n");
				                                al |= ah;
				                                break;
				                        }
				                        case 0xD8:
				                        {
				                                printf("OR BL,AL\n");
				                                bl |= al;
				                                break;
				                        }
				                }
                                                ip+=2;
                                                break;
				        }
				        case 0x0B:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC7:
				                        {
				                                printf("OR AX,DI\n");
				                                ax |= di;
				                                if(ax==0) flags |= 0x0040;
				                                else flags &= 0xFFBF;
				                                break;
				                        }
				                }
                                                ip+=2;
                                                break;
				        }
				        case 0x0C:
				        {
				                printf("OR AL,%02x\n",RAM16::RAM[addr+1]);
				                al |= RAM16::RAM[addr+1];
				                ip+=2;
				                break;
				        }
				        case 0x0E:
					{
					        printf("PUSH CS\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = cs & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = cs >> 8;
					        ip+=1;
					        break;
					}
					case 0x0F:
					{
					        printf("POP CS\n");
					        cs  = (RAM16::RAM[(ss<<4)+sp]<<8)|RAM16::RAM[(ss<<4)+sp+1];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x16:
					{
					        printf("PUSH SS\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = ss & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = ss >> 8;
					        ip+=1;
					        break;
					}
					case 0x17:
					{
					        printf("POP SS\n");
					        ss  = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x1A:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0xC0:
					                {
					                        printf("SBB AL,AL");
                                                                al = 0 - (flags & 0x0001);
					                        break;
					                }
					        } 
					        ip+=2;
					        break;
					}
				        case 0x1E:
					{
					        printf("PUSH DS\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = ds & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = ds >> 8;
					        ip+=1;
					        break;
					}
					case 0x1F:
					{
					        printf("POP DS\n");
					        ds  = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x22:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0xD8:
					                {
					                        printf("AND BL,AL\n");
					                        bl &= al;
					                        if(bl==0) flags |= 0x0040;
					                        else flags &= 0xFFBF;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x24:
				        {
				                printf("AND AL,%02x\n",RAM16::RAM[addr+1]);
				                al &= RAM16::RAM[addr+1];
				                if(al==0) flags |= 0x0040;
				                else flags &= 0xFFBF;
				                ip+=2;
				                break;
				        }
				        case 0x26:
				        {
				                u8 op2 = RAM16::RAM[addr+1];
				                switch(op2)
				                {
				                        case 0x3A:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x05:
				                                        {
				                                                printf("CMP AL, BYTE PTR ES:[DI]\n");
				                                                u8 tmp = al - RAM16::RAM[(es<<4)+di];
				                                                if(tmp==0) flags |= 0x0040;
				                                                else flags &= 0xFFBF;
				                                                break;
				                                        }
				                                }
				                                ip+=2;
				                                break;
				                        }
				                        case 0x3B:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x15:
				                                        {
				                                                printf("CMP DX, WORD PTR ES:[DI]\n");
				                                                u16 tmp = dx - ((RAM16::RAM[((es<<4)+di)]<<8) | RAM16::RAM[((es<<4)+di+1)]);
				                                                if(tmp==0) flags |= 0x0040;
				                                                else flags &= 0xFFBF;
				                                                break;
				                                        }
				                                }
				                                ip+=2;
				                                break;
				                        }
				                        case 0x88:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x05:
				                                        {
				                                                printf("MOV BYTE PTR ES:[DI],AL\n");
				                                                RAM16::RAM[(es<<4)+di] = al;
				                                                break;
				                                        }
				                                }
				                                ip+=2;
				                                break;
				                        }
				                        case 0x89:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x15:
				                 
				                                        {
				                                                printf("MOV WORD PTR ES:[DI],DX\n");
				                                                RAM16::RAM[(es<<4)+di+1] = dx & 0xFF;
				                                                RAM16::RAM[(es<<4)+di] = dx >> 8;
				                                                break;
				                                        }
				                                }
				                                ip+=2;
				                                break;
				                        }
				                        case 0x8A:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x04:
				                                        {
				                                                printf("MOV AL,BYTE PTR ES:[SI]\n");
				                                                RAM16::RAM[(es<<4)+si] = al;
				                                                break;
				                                        }
				                                }
				                                ip+=2;
				                                break;
				                        }
				                        case 0xC7:
				                        {
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0x06:
				                                        {
				                                                printf("MOV WORD PTR ES:%04x,%04x\n",(RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3],(RAM16::RAM[addr+6]<<8)|RAM16::RAM[addr+5]);
				                                                RAM16::RAM[(es<<4)+((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])] = RAM16::RAM[addr+6];
				                                                RAM16::RAM[(es<<4)+((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])+1] = RAM16::RAM[addr+5];
				                                                ip+=2;
				                                                break;
				                                        }
				                                }
				                                ip+=4;
				                                break;
				                        }
				                }
				                ip+=1;
				                break;
				        }
				        case 0x27:
				        {
				                printf("DAA\n");
				                if(((al & 0x0F)>9) || (flags & 0x10))
				                {
				                        al+=6;
				                }
				                if((al > 0x9F) || (flags & 1))
				                {
				                        al-=0x60;
				                }
				                ip+=1;
				                break;
				        }
				        case 0x2A:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC0:
				                        {
				                                printf("SUB AL,AL\n");
				                                al = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                }
				                ip+=2;
				                break;
				        }
				        case 0x2B:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC0:
				                        {
				                                printf("SUB AX,AX\n");
				                                ax = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                        case 0xC9:
				                        {
				                                printf("SUB CX,CX\n");
				                                cx = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                        case 0xFF:
				                        {
				                                printf("SUB DI,DI\n");
				                                di = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                }
				                ip+=2;
				                break;
				        }
				        case 0x2D:
				        {
				                printf("SUB AX,%04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
				                ax -= (RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1];
				                if(ax >= 0x8000) flags |= 0x0080;
				                else flags &= 0xFF7F;
				                ip+=3;
				                break;
				        }
				        case 0x2E:
				        {
				                u8 op2 = RAM16::RAM[addr+1];
				                switch(op2)
				                {
				                        case 0x02:
                				        {
		                		                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
                				                {
		                		                        case 0x07:
				                                        {
				                                                printf("ADD AL,BYTE PTR CS:[BX]\n");
                				                                al += RAM16::RAM[(cs<<4)+bx];
		                		                                break;
				                                        }
                                                                }
                                                                ip+=2;
                                                                break;
                                                        }
                                                        case 0x8A:
                                                        {
                                                                u8 modrm = RAM16::RAM[addr+2];
                                                                switch(modrm)
                                                                {
                                                                        case 0x87:
                                                                        {
                                                                                printf("MOV AL,BYTE PTR CS:[BX+%04x]\n",(RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3]);
                                                                                al = RAM16::RAM[bx + (cs<<4) + (s8)((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])];
                                                                                ip+=2;
                                                                                break;
                                                                        }
                                                                        case 0x9F:
                                                                        {
                                                                                printf("MOV BL,BYTE PTR CS:[BX+%04x]\n",(RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3]);
                                                                                bl = RAM16::RAM[bx + (cs<<4) + (s8)((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])];
                                                                                ip+=2;
                                                                                break;
                                                                        }
                                                                }
                                                                ip+=2;
                                                                break;
                                                        }
                                                        case 0x8B:
                                                        {
                                                                u8 modrm = RAM16::RAM[addr+2];
                                                                switch(modrm)
                                                                {
                                                                        case 0x14:
                                                                        {
                                                                                printf("MOV DX,WORD PTR CS:[SI]\n");
                                                                                dx = (RAM16::RAM[(cs<<4)+si+1]<<8)|RAM16::RAM[(cs<<4)+si];
                                                                                break;
                                                                        }
                                                                        case 0x87:
                                                                        {
                                                                                printf("MOV AX,WORD PTR CS:[BX+%04x]\n",(RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3]);
                                                                                ax = (RAM16::RAM[bx + (cs<<4) + (s8)((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])])|RAM16::RAM[1 + bx + (cs<<4) + (s8)((RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3])]     ;
                                                                                ip+=2;
                                                                                break;
                                                                        }
                                                                }
                                                                ip+=2;
                                                                break;
                                                        }
				                        case 0xFF:
				                        {	
				                                u8 modrm = RAM16::RAM[addr+2];
				                                switch(modrm)
				                                {
				                                        case 0xA7:
				                                        {
				                                                printf("JMP WORD PTR CS:[BX+%04x]\n",(RAM16::RAM[addr+4]<<8)|RAM16::RAM[addr+3]);
				                                                ip = (RAM16::RAM[bx + (cs<<4) + ((RAM16::RAM[addr+4]<<8) | RAM16::RAM[addr+3])+1]<<8) | RAM16::RAM[bx + (cs<<4) + ((RAM16::RAM[addr+4]<<8) | RAM16::RAM[addr+3])];
				                                                ip-=1;
				                                                break;
				                                        }
				                                }
				                                break;
				                        }
				                }
				                ip+=1;
				                break;
				        }
				        case 0x30:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC0:
				                        {
				                                printf("XOR AL,AL\n");
				                                al = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                        case 0xC4:
				                        {
				                                printf("XOR AH,AL\n");
				                                ah ^= al;
				                                if(al==0) flags |= 0x0040;
				                                else flags &= 0xFFBF;
				                                break;
				                        }
				                        case 0xE4:
				                        {
				                                printf("XOR AH,AH\n");
				                                ah = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                }
				                ip+=2;
				                break;
				        }
				        case 0x32:
				        {
				                u8 modrm = RAM16::RAM[addr+1];
				                switch(modrm)
				                {
				                        case 0xC0:
				                        {
				                                printf("XOR AL,AL\n");
				                                al = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                        case 0xC4:
				                        {
				                                printf("XOR AL,AH\n");
				                                al ^= ah;
				                                if(al==0) flags |= 0x0040;
				                                else flags &= 0xFFBF;
				                                break;
				                        }
				                        case 0xE4:
				                        {
				                                printf("XOR AH,AH\n");
				                                ah = 0;
				                                flags &= 0xFFBA;
				                                flags |= 0x0044;
				                                break;
				                        }
				                }
				                ip+=2;
				                break;
				        }
					case 0x33:
					{
						u8 modrm = RAM16::RAM[addr+1];
						switch(modrm)
						{
							case 0xC0:
							{
								printf("XOR AX,AX\n");
								ax = 0;
								flags &= 0xFFBA;
								flags |= 0x0044;
								break;
							}
							case 0xC7:
							{
							        printf("XOR AX,DI\n");
							        ax ^= di;
							        if(ax==0) flags |= 0x0040;
							        else flags &= 0xFFBF;
							        break;
							}
							case 0xC9:
							{
							        printf("XOR CX,CX\n");
							        cx = 0;
							        flags &= 0xFFBA;
							        flags |= 0x0044;
							        break;
							}
							case 0xDB:
							{
								printf("XOR BX,BX\n");
								bx = 0;
								flags &= 0xFFBA;
								flags |= 0x0044;
								break;
							}
							case 0xFF:
							{
							        printf("XOR DI,DI\n");
							        di = 0;
							        flags &= 0xFFBA;
							        flags |= 0x0044;
							        break;
							}
						}
						inc_addr();
						inc_addr();
						break;
					}
					case 0x34:
					{
					        printf("XOR AL,%04x\n",RAM16::RAM[addr+1]);
					        al ^= RAM16::RAM[addr+1];
					        if(al==0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        ip+=2;
					        break; 
					}
					case 0x35:
					{
					        printf("XOR AX,%04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        ax ^= (RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1];
					        if(ax==0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        ip+=3;
					        break; 
					}
					case 0x3A:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0xD6:
					                {
					                        printf("CMP DL,DH\n");
					                        u8 tmp = dl - dh;
					                        if(tmp==0) flags |= 0x0040;
					                        else flags &= 0xFFBF;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x3B:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0xD8:
					                {
					                        printf("CMP BX,AX\n");
					                        u16 tmp = bx - ax;
					                        if(tmp==0) flags |= 0x0040;
					                        else flags &= 0xFFBF;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x3C:
					{
					        printf("CMP AL,%02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = al - RAM16::RAM[addr+1];
					        if(tmp==0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        ip+=2;
					        break;
					}
					case 0x3D:
					{
					        printf("CMP AX,%04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        u16 tmp = ax - ((RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        if(tmp==0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        ip+=3;
					        break;
					}
					case 0x40:
					{
					        printf("INC AX\n");
					        ax++;
					        if(ax >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x42:
					{
					        printf("INC DX\n");
					        dx++;
					        if(dx >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x43:
					{
					        printf("INC BX\n");
					        bx++;
					        if(bx >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x46:
					{
					        printf("INC SI\n");
					        si++;
					        if(si >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x47:
					{
					        printf("INC DI\n");
					        di++;
					        if(di >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x4D:
					{
					        printf("DEC BP\n"); 
					        bp--;
					        if(bp == 0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        if(bp >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x4F:
					{
					        printf("DEC DI\n");
					        di--;
					        if(di >= 0x8000) flags |= 0x0800;
					        else flags &= 0xF7FF;
					        ip+=1;
					        break;
					}
					case 0x50:
					{
					        printf("PUSH AX\n");
                                                sp-=2;                        
					        RAM16::RAM[(ss<<4)+sp] = al;
					        RAM16::RAM[(ss<<4)+sp+1] = ah;
					        ip+=1;
					        break;
					}
					case 0x51:
					{
					        printf("PUSH CX\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = cl;
					        RAM16::RAM[(ss<<4)+sp+1] = ch;
					        ip+=1;
					        break;
					}
					case 0x52:
					{
					        printf("PUSH DX\n");
					        sp-=2;
					        RAM16::RAM[(ss<<4)+sp] = dl;
					        RAM16::RAM[(ss<<4)+sp+1] = dh;
					        ip+=1;
					        break;
					}
					case 0x53:
					{
					        printf("PUSH BX\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = bl;
					        RAM16::RAM[(ss<<4)+sp+1] = bh;
					        ip+=1;
					        break;
					}
					case 0x55:
					{
					        printf("PUSH BP\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = bp & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = bp >> 8;
					        ip+=1;
					        break;
					}
					case 0x56:
					{
					        printf("PUSH SI\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = si & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = si >> 8;
					        ip+=1;
					        break;
					}
					case 0x57:
					{
					        printf("PUSH DI\n");
					        sp -= 2;
					        RAM16::RAM[(ss<<4)+sp] = di & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = di >> 8;
					        ip+=1;
					        break;
					}
					case 0x58:
					{
					        printf("POP AX\n");
					        ax  = (RAM16::RAM[(ss<<4)+sp]<<8)|RAM16::RAM[(ss<<4)+sp+1];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x59:
					{
					        printf("POP CX\n");
					        cx = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x5A:
					{
					        printf("POP DX\n");
					        dx  = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x5B:
					{
					        printf("POP BX\n");
					        bx  = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x5D:
					{
					        printf("POP BP\n");
					        bp = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x5E:
					{
					        printf("POP SI\n");
					        si = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x5F:
					{
					        printf("POP DI\n");
					        di = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        ip+=1;
					        sp+=2;
					        break;
					}
					case 0x70:
					{
						printf("JO %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(flags&0x0800) ip += (s8)tmp;
						ip+=2;
						break;
					}
					case 0x71:
					{
						printf("JNO %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(!(flags&0x0800)) ip += (s8)tmp;
						ip+=2;
						break;
					}
					case 0x72:
					{
						printf("JC %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(flags&0x0001) ip += (s8)tmp;
						ip+=2;
						break;
					}
					case 0x73:
					{
						printf("JNC %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(!(flags&0x0001)) ip += (s8)tmp;
						ip+=2;
						break;
					}
					case 0x74:
					{
					        printf("JZ %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        if(flags&0x0040) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x75:
					{
					        printf("JNZ %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        if(!(flags&0x0040)) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x76:
					{
					        printf("JBE %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        if((flags&0x0040)||(flags&0x0001)) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x78:
					{
						printf("JS %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(flags&0x0080) ip += (s8)tmp;
						ip+=2;
						break;   
					}
					case 0x79:
					{
						printf("JNS %02x\n",RAM16::RAM[addr+1]);
						u8 tmp = RAM16::RAM[addr+1];
						if(!(flags&0x0080)) ip += (s8)tmp;
						ip+=2;
						break;   
					}
					case 0x7A:
					{
					        printf("JP %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        if(flags&0x0004) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x7B:
					{
					        printf("JNP %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        if(!(flags&0x0004)) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x7C:
					{
					        printf("JL %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = RAM16::RAM[addr+1];
					        u8 sign = (flags&0x0080)>>7;
					        u8 over = (flags&0x0800)>>11;
					        if(sign != over) ip += (s8)tmp;
					        ip+=2;
					        break;
					}
					case 0x80:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 0:
					                {
					                        switch(modrm&7)
					                        {
					                                case 2:
					                                {
					                                        printf("ADD DL,%02x\n",RAM16::RAM[addr+2]);
					                                        dl += RAM16::RAM[addr+2];
					                                        break;
					                                }
					                                case 7:
					                                {
					                                        printf("ADD BH,%02x\n",RAM16::RAM[addr+2]);
					                                        bh += RAM16::RAM[addr+2];
					                                        break;
                                                                        }
                                                                }
                                                                break;
					                }
					                case 1:
					                {
					                        switch(modrm&7)
					                        {
					                                case 6:
					                                {
					                                        switch((modrm>>6)&3)
					                                        {
					                                                case 0:
					                                                {
					                                                        printf("OR BYTE PTR DS:%04x,%02x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2],RAM16::RAM[addr+4]);
					                                                        RAM16::RAM[(ds<<4)|((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] |= RAM16::RAM[addr+4];
					                                                        ip+=2;
					                                                        break;
					                                                }
					                                        }
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 4:
					                {
					                        switch(modrm&7)
					                        {
					                                case 3:
					                                {
					                                        printf("AND BL,%02x\n",RAM16::RAM[addr+2]);
					                                        bl &= RAM16::RAM[addr+2];
					                                        if(bl==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF; 
					                                        break;
					                                }
					                                case 4:
					                                {
					                                        printf("AND AH,%02x\n",RAM16::RAM[addr+2]);
					                                        ah &= RAM16::RAM[addr+2];
					                                        if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF; 
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 7:
					                {
					                        switch(modrm&7)
					                        {
					                                case 3:
					                                {
					                                        printf("CMP BL,%02x\n",RAM16::RAM[addr+2]);
					                                        u8 tmp = bl - RAM16::RAM[addr+2];
					                                        if(tmp==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 4:
					                                {
					                                        printf("CMP AH,%02x\n",RAM16::RAM[addr+2]);
					                                        u8 tmp = ah - RAM16::RAM[addr+2];
					                                        if(tmp==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        if(tmp>ah) flags |= 0x0001;
					                                        else flags &= 0xFFFE;
					                                        break;
					                                }
					                                case 6:
					                                {
					                                        printf("CMP DH,%02x\n",RAM16::RAM[addr+2]);
					                                        u8 tmp = dh - RAM16::RAM[addr+2];
					                                        if(tmp==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        if(tmp>dh) flags |= 0x0001;
					                                        else flags &= 0xFFFE;
					                                        break;
					                                }
					                                case 7:
					                                {
					                                        printf("CMP BH,%02x\n",RAM16::RAM[addr+2]);
					                                        u8 tmp = bh - RAM16::RAM[addr+2];
					                                        if(tmp==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=3;
					        break;
					}
					case 0x81:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 0:
					                {
					                        switch(modrm&7)
					                        {
					                                case 2:
					                                {
					                                        printf("ADD DX,%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                                        bx += (RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2];
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 7:
					                {
					                        switch(modrm&7)
					                        {
					                                case 6:
					                                {
                                                                                switch((modrm>>6)&3)
                                                                                {
                                                                                        case 0:
                                                                                        {
                                                                                                printf("CMP WORD PTR DS:%04x,%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2],(RAM16::RAM[addr+5]<<8)|RAM16::RAM[addr+4]);
                                                                                                u16 tmp = ((RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])]<<8)|RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1]) - ((RAM16::RAM[addr+5]<<8)|RAM16::RAM[addr+4]);
                                                                                                if(tmp==0) flags |= 0x0040;
                                                                                                else flags &= 0xFFBF;
			        		                                                ip+=2;
					                                                }
					                                        }
					                                        break;
					                                }
					                                case 2:
					                                {
                                                                                printf("CMP DX,%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                                        u16 tmp = dx - (RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2];
					                                        if(tmp > dx) flags |= 0x0800;
				                                                else flags &= 0xF7FF;
			                                                        if(tmp >= 0x8000) flags |= 0x0080;
			                                                        else flags &= 0xFF7F;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=4;
					        break;
					}
					case 0x83:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 0:
					                {
					                        switch(modrm&7)
					                        {
					                                case 2:
					                                {
					                                        printf("ADD DX,%02x\n",RAM16::RAM[addr+2]);
					                                        dx += RAM16::RAM[addr+2];
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=3;
					        break;
					}
					case 0x86:
					{
                                                u8 modrm = RAM16::RAM[addr+1];
                                                switch(modrm)
                                                {
                                                        case 0xC4:
                                                        {
                                                                printf("XCHG AH,AL\n");
                                                                u8 tmp = al;
                                                                al = ah;
                                                                ah = tmp;
                                                                break;
                                                        }
                                                }
                                                ip+=2;
                                                break;
					}
					case 0x88:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0x1E:
					                {
					                        printf("MOV BYTE PTR DS:%04x,BL\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] = bl;
					                        ip+=2;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x89:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0x05:
					                {
					                        printf("MOV WORD PTR DS:[DI],AX\n");
					                        RAM16::RAM[(ds<<4)+di] = al;
					                        RAM16::RAM[(ds<<4)+di+1] = ah;
					                        break;
					                }
					                case 0x0E:
					                {
					                        printf("MOV WORD PTR DS:%04x,CX\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] = cl;
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1] = ch;
					                        ip+=2;
					                        break;
					                }
					                case 0x16:
					                {
					                        printf("MOV WORD PTR DS:%04x,DX\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] = dl;
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1] = dh;
					                        ip+=2;
					                        break;
					                }
					                case 0x1E:
					                {
					                        printf("MOV WORD PTR DS:%04x,BX\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] = bl;
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1] = bh;
					                        ip+=2;
					                        break;
					                }
					                case 0x36:
					                {
					                        printf("MOV WORD PTR DS:%04x,SI\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])] = si >> 8;
					                        RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1] = si & 0xFF;
					                        ip+=2;
					                        break;
					                }
					                case 0x55:
					                {
					                        printf("MOV WORD PTR DS:[DI+%02x],DX\n",RAM16::RAM[addr+2]);
					                        dx = (RAM16::RAM[di + (ds<<4) + (s8)(RAM16::RAM[addr+2])]<<8)|RAM16::RAM[di+(ds<<4)+(s8)(RAM16::RAM[addr+2])+1];
					                        ip+=1;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x8A:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0x1E:
					                {
					                        printf("MOV BL,BYTE PTR DS:%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        bl = RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])];
					                        ip+=2;
					                        break;
					                }
					                case 0x26:
					                {
					                        printf("MOV AH,BYTE PTR DS:%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        ah = RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])];
					                        ip+=2;
					                        break;
					                }
					                case 0x5E:
						        {
						                printf("MOV BL,BYTE PTR SS:[BP+%02x]\n",RAM16::RAM[addr+2]);
						                bl = RAM16::RAM[(ds<<4)+bp+(s8)(RAM16::RAM[addr+2])];
						                ip+=1;
						                break;
						        }
					                case 0xC1:
					                {
					                        printf("MOV AL,CL\n");
					                        al = cl;
					                        break;
					                }
					                case 0xC3:
					                {
					                        printf("MOV AL,BL\n");
					                        al = bl;
					                        break;
					                }
					                case 0xC5:
					                {
					                        printf("MOV AL,CH\n");
					                        al = ch;
					                        break;
					                }
					                case 0xD8:
					                {
					                        printf("MOV BL,AL\n");
					                        bl = al;
					                        break;
					                }
					                case 0xD9:
					                {
					                        printf("MOV BL,CL\n");
					                        bl = cl;
					                        break;
					                }
					                case 0xDC:
					                {
					                        printf("MOV BL,AH\n");
					                        bl = ah;
					                        break;
					                }
					                case 0xE0:
					                {
					                        printf("MOV AH,AL\n");
					                        ah = al;
					                        break;
					                }
					                case 0xF8:
					                {
                                                                printf("MOV BH,AL\n");
                                                                bh = al;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x8B:
					{
						u8 modrm = RAM16::RAM[addr+1];
						switch(modrm)
						{
						        case 0x07:
						        {
						                printf("MOV AX, WORD PTR DS:[BX]\n");
						                ax = (RAM16::RAM[(ds<<4)+bx]<<8)|RAM16::RAM[(ds<<4)+bx+1];
						                break;
						        }
						        case 0x16:
						        {
						                printf("MOV DX, WORD PTR DS:%04x\n",((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]));
						                dx = (RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1]<<8)|RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])];
						                ip+=2;
						                break;
						        }
						        case 0x17:
						        {
						                printf("MOV DX, WORD PTR DS:[BX]\n");
						                dx = (RAM16::RAM[(ds<<4)+bx]<<8)|RAM16::RAM[(ds<<4)+bx+1];
						                break;
						        }
						        case 0x1E:
						        {
						                printf("MOV BX, WORD PTR DS:%04x\n",((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]));
						                bx = (RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])]<<8)|RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1];
						                ip+=2;
						                break;
						        }
						        case 0x36:
						        {
						                printf("MOV SI, WORD PTR DS:%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
						                si = RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])];
						                ip+=2;
						                break;  
						        }
						        case 0x4E:
						        {
						                printf("MOV CX,WORD PTR SS:[BP+%02x]\n",RAM16::RAM[addr+2]);
						                cx = (RAM16::RAM[(ss<<4)+bp+(s8)(RAM16::RAM[addr+2])+1]<<8)|RAM16::RAM[(ds<<4)+bp+(s8)(RAM16::RAM[addr+2])];
						                ip+=1;
						                break;
						        }
						        case 0x5E:
						        {
						                printf("MOV BX,WORD PTR SS:[BP+%02x]\n",RAM16::RAM[addr+2]);
						                bx = (RAM16::RAM[(ss<<4)+bp+(s8)(RAM16::RAM[addr+2])+1]<<8)|RAM16::RAM[(ds<<4)+bp+(s8)(RAM16::RAM[addr+2])];
						                ip+=1;
						                break;
						        }
						        case 0xC7:
						        {
						                printf("MOV AX,DI\n");
						                ax = di;
						                break;
						        }
						        case 0xC8:
						        {
						                printf("MOV CX,AX\n");
						                cx = ax;
						                break;
						        }
						        case 0xCB:
						        {
						                printf("MOV CX,BX\n");
						                cx = bx;
						                break;
						        }
						        case 0xCD:
						        {
						                printf("MOV CX,BP\n");
						                cx = bp;
						                break;
						        }
						        case 0xD4:
						        {
						                printf("MOV DX,SP\n");
						                dx = sp;
						                break;
						        }
						        case 0xD8:
						        {
						                printf("MOV BX,AX\n");
						                bx = ax;
						                break;
						        }
						        case 0xD9:
						        {
						                printf("MOV BX,CX\n");
						                bx = cx;
						                break;
						        }
						        case 0xE1:
						        {
						                printf("MOV SP,CX\n");
						                sp = cx;
						                break;
						        }
						        case 0xE2:
						        {
						                printf("MOV SP,DX\n");
						                sp = dx;
						                break;
						        }
						        case 0xE5:
						        {
						                printf("MOV SP,BP\n");
						                sp = bp;
						                break;
						        }
						        case 0xE8:
						        {
						                printf("MOV BP,AX\n");
						                bp = ax;
						                break;
						        }
							case 0xEB:
							{
								printf("MOV BP,BX\n");
								bp = bx;
								break;
							}
							case 0xEC:
							{
							        printf("MOV BP,SP\n");
							        bp = sp;
							        break;
							}
							case 0xF5:
							{
							        printf("MOV SI,BP\n");
							        si = bp;
							        break;
							}
							case 0xF7:
							{
							        printf("MOV SI,DI\n");
							        si = di;
							        break;
							}
							case 0xF8:
							{
							        printf("MOV DI,AX\n");
							        di = ax;
							        break;
							}
							case 0xFE:
							{
							        printf("MOV DI,SI\n");
							        di = si;
							        break;
							}
						}
						ip+=2;
						break;
					}
					case 0x8C:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0x0F:
					                {
					                        printf("MOV WORD PTR DS:[BX],CS\n");
					                        RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]))] = cs & 0xFF;
					                        RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]))+1] = cs >> 8;
					                        break;
					                }
					                case 0xC0:
					                {
					                        printf("MOV AX,ES\n");
					                        ax = es;
					                        break;
					                }
					                case 0xC1:
					                {
					                        printf("MOV CX,ES\n");
					                        cx = es;
					                        break;
					                }
					                case 0xC7:
					                {
					                        printf("MOV DI,ES\n");
					                        di = es;
					                        break;
					                }
					                case 0xC8:
					                {
					                        printf("MOV AX,CS\n");
					                        ax = cs;
					                        break;
					                }
					                case 0xD2:
					                {
					                        printf("MOV DX,SS\n");
					                        dx = ss;
					                        break;
					                } 
					                case 0xD6:
					                {
					                        printf("MOV SI,SS\n");
					                        si = ss;
					                        break;
					                }
					                case 0xD8:
					                {
					                        printf("MOV AX,DS\n");
					                        ax = ss;
					                        break;
					                }
					                case 0xDB:
					                {
					                        printf("MOV BX,DS\n");
					                        bx = ds;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0x8E:
					{
						u8 modrm = RAM16::RAM[addr+1];
						switch(modrm)
						{
						        case 0xC0:
						        {
						                printf("MOV ES,AX\n");
						                es = ax;
						                break;
						        }
						        case 0xC2:
						        {
						                printf("MOV ES,DX\n");
						                es = dx;
						                break;
						        }
						        case 0xC3:
						        {
						                printf("MOV ES,BX\n");
						                es = bx;
						                break;
						        }
						        case 0xC6:
						        {
						                printf("MOV ES,SI\n");
						                es = si;
						                break;
						        }
						        case 0xD0:
						        {
						                printf("MOV SS,AX\n");
						                ss = ax;
						                break;
						        }
						        case 0xD1:
						        {
						                printf("MOV SS,CX\n");
						                ss = cx;
						                break;
						        }
						        case 0xD2:
						        {
						                printf("MOV SS,DX\n");
						                ss = dx;
						                break;
						        }
							case 0xD8:
							{
								printf("MOV DS,AX\n");
								ds = ax;
								break;
							}
							case 0xDA:
							{
							        printf("MOV DS,DX\n");
							        ds = dx;
							        break;
							}
							case 0xDB:
							{
                                                                printf("MOV DS,BX\n");
                                                                ds = bx;
                                                                break;
							}
							case 0xDE:
							{
							        printf("MOV DS,SI\n");
							        ds = si;
							        break;
							}
							case 0xDF:
							{
							        printf("MOV DS,DI\n");
							        ds = di;
							        break;
							}
						}
						inc_addr();
						inc_addr();
						break;
					}
					case 0x90:
					{
					        printf("NOP\n");
					        ip+=1;
					        break;
					}
					case 0x9E:
					{
					        printf("SAHF\n");
					        flags &= 0xFF2A;
					        flags |= ah & 0xD5;
					        ip+=1;
					        break;
					}
					case 0x9F:
					{
					        printf("LAHF\n");
					        ah &= 0x2A;
					        ah |= flags & 0x00D5;
					        ip+=1;
					        break;
					}
					case 0xA0:
					{
					        printf("MOV AL,BYTE PTR DS:%04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        al = RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1])];
					        ip+=3;
					        break;
					}
					case 0xA2:
					{
					        printf("MOV BYTE PTR DS:%04x,AL\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1])] = al;
					        ip+=3;
					        break;
					}
					case 0xA3:
					{
						printf("MOV WORD PTR DS:%04x, AX\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						RAM16::RAM[(ds<<4)+((RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1])] = al;
						RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]))+1] = ah;
						ip+=3;
						break;
					}
					case 0xA5:
					{
					        printf("MOVSW\n");
					        RAM16::RAM[(es<<4)+di] = RAM16::RAM[(ds<<4)+si];
					        RAM16::RAM[(es<<4)+di+1] = RAM16::RAM[(ds<<4)+si+1];
					        if(!(flags & 0x0400)){di+=2;si+=2;}
					        else{di-=2; si-=2;}
					        ip+=1;
					        break;
					}
					case 0xA8:
					{
					        printf("TEST AL,%02x\n",RAM16::RAM[addr+1]);
					        u8 tmp = al & RAM16::RAM[addr+1];
					        if(tmp==0) flags |= 0x0040;
					        else flags &= 0xFFBF;
					        ip+=2;
					        break;
					}
					case 0xAA:
					{
					        printf("STOSB\n");
					        RAM16::RAM[(es<<4)+di] = al;
					        if(!(flags & 0x0400)) di+=1;
					        else di-=1;
					        ip+=1;
					        break;
					}
					case 0xAB:
					{
					        printf("STOSW\n");
					        RAM16::RAM[(es<<4)+di] = ax >> 8;
					        RAM16::RAM[(es<<4)+di+1] = ax & 0xFF;
					        if(!(flags & 0x0400)) di+=2;
					        else di-=2;
					        ip+=1;
					        break;
					}
					case 0xAC:
					{
					        printf("LODSB\n");
					        al = RAM16::RAM[(ds<<4)+si];
					        if(!(flags & 0x0400)) di+=1;
					        else di-=1;
					        ip+=1;
					        break;
					}
					case 0xB0:
					{
					        printf("MOV AL,%02x\n",RAM16::RAM[addr+1]);
					        al = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB1:
					{
					        printf("MOV CL,%02x\n",RAM16::RAM[addr+1]);
					        cl = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB2:
					{
					        printf("MOV DL,%02x\n",RAM16::RAM[addr+1]);
					        dl = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB3:
					{
					        printf("MOV BL,%02x\n",RAM16::RAM[addr+1]);
					        bl = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB4:
					{
					        printf("MOV AH,%02x\n",RAM16::RAM[addr+1]);
					        ah = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB5:
					{
					        printf("MOV CH,%02x\n",RAM16::RAM[addr+1]);
					        ch = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB7:
					{
					        printf("MOV BH,%02x\n",RAM16::RAM[addr+1]);
					        bh = RAM16::RAM[addr+1];
					        ip+=2;
					        break;
					}
					case 0xB8:
					{
						printf("MOV AX, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						ax = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						inc_addr();
						inc_addr();
						inc_addr();
						break;
					}
					case 0xB9:
					{
						printf("MOV CX, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						cx = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						inc_addr();
						inc_addr();
						inc_addr();
						break;
					}
					case 0xBA:
					{
						printf("MOV DX, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						dx = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xBB:
					{
						printf("MOV BX, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						bx = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xBC:
					{
						printf("MOV SP, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						sp = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xBD:
					{
						printf("MOV BP, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						bp = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xBE:
					{
						printf("MOV SI, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						si = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xBF:
					{
						printf("MOV DI, %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						di = (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xC3:
					{
					        printf("RET\n");
					        ip = (RAM16::RAM[(ss<<4)+sp]) | (RAM16::RAM[(ss<<4)+sp+1]<<8);
					        sp+=2;
					        break;
					}
					case 0xC4:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch(modrm)
					        {
					                case 0x36:
					                {
					                        printf("LES SI,DS:%04x\n",(RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2]);
					                        si = (RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+1]<<8)|RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])];
					                        es = (RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+2]<<8)|RAM16::RAM[(ds<<4) + ((RAM16::RAM[addr+3]<<8)|RAM16::RAM[addr+2])+3];
					                        ip+=2;
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xC7:
					{
						u8 modrm = RAM16::RAM[addr+1];
						switch(modrm)
						{
							case 0x06:
							{
								printf("MOV WORD PTR DS:%04x,%04x\n",(RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2], (RAM16::RAM[addr+5]<<8) | RAM16::RAM[addr+4]);
								RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]))] = (RAM16::RAM[addr+5]<<8) | RAM16::RAM[addr+4];
								inc_addr();
								inc_addr();
 								break;
							}
							case 0x07:
							{
								printf("MOV WORD PTR DS:[BX],%04x\n",(RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]);
								RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]))] = bl;
								RAM16::RAM[((ds<<4)+((RAM16::RAM[addr+3]<<8) | RAM16::RAM[addr+2]))+1] = bh;
 								break;
							}
						}
						inc_addr();
						inc_addr();
						inc_addr();
						inc_addr();
						break;
					}
					case 0xCB:
					{
					        printf("RET\n");
					        ip = (RAM16::RAM[(ss<<4)+sp+1]<<8) | RAM16::RAM[(ss<<4)+sp];
					        sp+=2;
					        cs = (RAM16::RAM[(ss<<4)+sp+1]<<8) | RAM16::RAM[(ss<<4)+sp];
					        sp+=2;
					        break;
					}
					case 0xCD:
					{
					        printf("INT %02x\n",RAM16::RAM[addr+1]);
					        u8 tmp1 = RAM16::RAM[addr+1];
					        //dumpivt();
					        sp-=2;
					        RAM16::RAM[(ss<<4)+sp] = flags & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = flags >> 8;
					        flags &= 0xFCFF;
					        sp-=2;
					        RAM16::RAM[(ss<<4)+sp] = cs & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = cs >> 8;
					        sp-=2;
					        RAM16::RAM[(ss<<4)+sp] = (ip+2) & 0xFF;
					        RAM16::RAM[(ss<<4)+sp+1] = (ip+2) >> 8;
					        int tmp = addr; 
					        cs = (RAM16::RAM[(tmp1<<2)+2]<<8)|RAM16::RAM[(tmp1<<2)+3];
					        ip = (RAM16::RAM[(tmp1<<2)+1]<<8)|RAM16::RAM[(tmp1<<2)];
					        break;
					}
					case 0xCF:
					{
					        printf("IRET\n");
					        ip = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        sp+=2;
					        cs = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        sp+=2;
					        flags = (RAM16::RAM[(ss<<4)+sp+1]<<8)|RAM16::RAM[(ss<<4)+sp];
					        sp+=2;
					        break;
					}
					case 0xD0:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 4:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u8 tmp = al;
        			        		                        printf("SHL AL,1\n");
	        	        			                        al <<= 1;
		                			                        if(tmp >= 0x80) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
			        		                                if(al >= 0x80) flags |= 0x0800;
			        		                                else flags &= 0xF7FF;
				        	                                if(al==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 3:
					                                {
					                                        u8 tmp = bl;
        			        		                        printf("SHL BL,1\n");
	        	        			                        bl <<= 1;
		                			                        if(tmp >= 0x80) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(bl==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 4:
					                                {
					                                        u8 tmp = ah;
        			        		                        printf("SHL AH,1\n");
	        	        			                        ah <<= 1;
		                			                        if(tmp >= 0x80) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
			        		                                if(ah >= 0x80) flags |= 0x0800;
			        		                                else flags &= 0xF7FF;
  				        	                                if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xD1:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 4:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u16 tmp = ax;
        			        		                        printf("SHL AX,1\n");
	        	        			                        ax <<= 1;
		                			                        if(tmp >= 0x8000) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(ax==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 3:
					                                {
					                                        u16 tmp = bx;
        			        		                        printf("SHL BX,1\n");
	        	        			                        bx <<= 1;
		                			                        if(tmp >= 0x8000) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(bx==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 5:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u16 tmp = ax;
        			        		                        printf("SHR AX,1\n");
	        	        			                        ax >>= 1;
		                			                        if(tmp >= 0x8000) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(ax==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 3:
					                                {
					                                        u16 tmp = bx;
        			        		                        printf("SHR BX,1\n");
	        	        			                        bx >>= 1;
		                			                        if(tmp >= 0x8000) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(bx==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xD2:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 1:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        printf("ROR AL,CL\n");
					                                        al = (al >> cl) | (al << (8 - cl));
					                                        break;
  					                                }
					                        }
					                        break;
					                }
					                case 4:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u8 tmp = al;
					                                        printf("SHL AL,CL\n");
					                                        al <<= cl;
					                                        if(tmp&(0x80>>cl)) flags |= 0x0001;
					                                        else flags &= 0xFFFE;
					                                        if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 5:
					                {
					                        switch(modrm&7)
					                        {
					                                case 4:
					                                {
					                                        u8 tmp = ah;
        			        		                        printf("SHR AH,CL\n");
	        	        			                        ah >>= cl;
			        		                                if(tmp&1) flags |= 0x0001;
			        		                                else flags &= 0xFFFE;
				        	                                if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xD3:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 4:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u16 tmp = ax;
        			        		                        printf("SHL AX,CL\n");
	        	        			                        ax <<= cl;
			        		                                flags &= 0xFFFE;
				        	                                if(ax==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					                case 5:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        u16 tmp = ax;
        			        		                        printf("SHR AX,CL\n");
	        	        			                        ax >>= cl;
			        		                                flags &= 0xFFFE;
				        	                                if(ax==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xE2:
					{
					        printf("LOOP %02x\n",RAM16::RAM[addr+1]);
                                                cx--;
					        if(cx>0)
					        {
					                ip += (s8)RAM16::RAM[addr+1];
                                                }
                                                ip+=2;
					        break;
					}
					case 0xE4:
					{
					        printf("IN AL,%02x\n",RAM16::RAM[addr+1]);
					        al = IO_XT::rb(RAM16::RAM[addr+1]);
					        ip+=2;
					        break;
					}
					case 0xE6:
					{
					        printf("OUT %02x,AL\n",RAM16::RAM[addr+1]);
					        IO_XT::wb(RAM16::RAM[addr+1],al);
					        ip+=2;
					        break;
					}
					case 0xE8:
					{
						printf("CALL %04x\n",(RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						sp-=2;
						RAM16::RAM[(ss<<4)+sp+1] = (ip+3) >> 8;
						RAM16::RAM[(ss<<4)+sp] = (ip+3) & 0xFF;
						ip += (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1];
						ip+=3;
						break;
					}
					case 0xE9:
					{
					        printf("JMP %04x\n",(RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        ip += (s16)((RAM16::RAM[addr+2]<<8)|RAM16::RAM[addr+1]);
					        ip += 3;
					        break;
					}
					case 0xEA:
					{
					        int tmp = addr;
						printf("JMP %04x:%04x\n",(RAM16::RAM[addr+4]<<8) | RAM16::RAM[addr+3], (RAM16::RAM[addr+2]<<8) | RAM16::RAM[addr+1]);
						cs = (RAM16::RAM[addr+4]<<8) | RAM16::RAM[addr+3];
						ip = (RAM16::RAM[tmp+2]<<8) | RAM16::RAM[tmp+1];
						break;
					}
					case 0xEB:
					{
					        printf("JMP %02x\n",RAM16::RAM[addr+1]);
					        ip += (s8)RAM16::RAM[addr+1];
					        ip += 2;
					        break;
					}
					case 0xEC:
					{
					        printf("IN AL,DX\n");
					        al = IO_XT::rb(dx);
					        ip+=1;
					        break;
					}
					case 0xEE:
					{
					        printf("OUT DX,AL\n");
					        IO_XT::wb(dx,al);
					        ip+=1;
					        break;
					}
					case 0xF3:
					{
					        u8 op2 = RAM16::RAM[addr+1];
					        switch(op2)
					        {
					                case 0xAA:
					                {
					                        printf("REP STOSB\n");
					                        for(int tmp = cx;tmp>0;tmp--)
					                        {
		                        			        RAM16::RAM[(es<<4)+di] = al;
				                        	        if(!(flags & 0x0400)) di+=1;
	                		        		        else di-=1;
	                		        		        cx--;
                                                                }
					                        break;
				                	}
					                case 0xAB:
					                {
					                        printf("REP STOSW\n");
					                        for(;cx>0;cx--)
					                        {
					                                RAM16::RAM[(es<<4)+di] = ax >> 8;
					                                RAM16::RAM[(es<<4)+di+1] = ax & 0xFF;
					                                if(!(flags & 0x0400)) di+=2;
					                                else di-=2;
                                                                }
                                                                break;
                                                        }  
					        }
					        ip+=2;
					        break;
					}
					case 0xF4:
					{
					        printf("HLT\n");
					        halted = true;
					        break;
					}
					case 0xF5:
					{
					        printf("CMC\n");
					        flags ^= 0x0001;
					        ip+=1;
					        break;
					}
					case 0xF6:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 0:
					                {
					                        switch(modrm&7)
					                        {
					                                case 4:
					                                {
					                                        printf("TEST AH,%02x\n",RAM16::RAM[addr+2]);
					                                        ah &= RAM16::RAM[addr+2];
					                                        flags &= 0xF7FE;
					                                        if(ah>=0x80) flags |= 0x0080;
					                                        else flags &= 0xFF7F;
					                                        if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
                                                                        }
					                        }
					                        ip+=1;
					                        break;
					                }
					                case 2:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        printf("NOT AL\n");
					                                        al = ~al;
					                                        if(al==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xF7:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7)
					        {
					                case 2:
					                {
					                        switch(modrm&3)
					                        {
					                                case 0:
					                                {
					                                        printf("NOT AX\n");
					                                        ax = ~ax;
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;
					        break;
					}
					case 0xF8:
					{
						printf("CLC\n");
						flags &= 0xFFFE;
						ip+=1;
						break;
					}
					case 0xF9:
					{
						printf("STC\n");
						flags |= 0x0001;
						ip+=1;
						break;
					}
					case 0xFA:
					{
						printf("CLI\n");
						flags &= 0xFDFF;
						inc_addr();
						break;
					}
					case 0xFB:
					{
						printf("STI\n");
						flags |= 0x0200;
						ip+=1;
						break;
					}
					case 0xFC:
					{
						printf("CLD\n");
						flags &= 0xFBFF;
						inc_addr();
						break;
					}
					case 0xFD:
					{
						printf("STD\n");
						flags |= 0x0400;
						ip+=1;
						break;
					}
					case 0xFE:
					{
					        u8 modrm = RAM16::RAM[addr+1];
					        switch((modrm>>3)&7) 
					        {
					                case 0:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        printf("INC AL\n");
					                                        al++;
					                                        if(al==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF;
					                                        break;
					                                }
					                                case 2:
					                                {
					                                        printf("INC DL\n");
					                                        dl++;
					                                        break;
					                                }
                                                                        case 4:
                                                                        {
                                                                                printf("INC AH\n");
                                                                                ah++;
                                                                                break;
                                                                        }
					                        }
					                        break;
					                }
					                case 1:
					                {
					                        switch(modrm&7)
					                        {
					                                case 0:
					                                {
					                                        printf("DEC AL\n");
					                                        al--;
					                                        if(al==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF; 
					                                        break;
					                                }
					                                case 4:
					                                {
					                                        printf("DEC AH\n");
					                                        ah--;
					                                        if(ah==0) flags |= 0x0040;
					                                        else flags &= 0xFFBF; 
					                                        break;
					                                }
					                        }
					                        break;
					                }
					        }
					        ip+=2;  
					        break; 
					}
					default:
					{
						printf("Unknown opcode %02x!\n",op);
						return 1;
						break;
					}
				}}
                                if(CPU_IRQ::irq && (flags & 0x0200)) 
                                {
                                        u8 tmp1 = CPU_IRQ::irq_number + PIC::vector_offset;
                                        printf("IRQ %02x\n",CPU_IRQ::irq_number);
                                        sp-=2;
                                        RAM16::RAM[(ss<<4)+sp] = flags & 0xFF;
                                        RAM16::RAM[(ss<<4)+sp+1] = flags >> 8;
				        flags &= 0xFCFF;
				        sp-=2;
				        RAM16::RAM[(ss<<4)+sp] = cs & 0xFF;
				        RAM16::RAM[(ss<<4)+sp+1] = cs >> 8;
                                        sp-=2;
				        RAM16::RAM[(ss<<4)+sp] = ip & 0xFF;
				        RAM16::RAM[(ss<<4)+sp+1] = ip >> 8;
				        int tmp = addr; 
				        cs = (RAM16::RAM[(tmp1<<2)+2]<<8)|RAM16::RAM[(tmp1<<2)+3];
				        ip = (RAM16::RAM[(tmp1<<2)+1]<<8)|RAM16::RAM[(tmp1<<2)];
                                }
				break;
                        }
		}
	}
};

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
	#ifdef VIDEO
	SDL_Init(SDL_INIT_VIDEO);
	
	screen = SDL_SetVideoMode(720,350,24,SDL_SWSURFACE);
	#endif
	
	for(;;)
	{
		for(int i = 0;i<4;i++){CPU::dumpregs(); if(CPU::tick() == 1){
		#ifdef VIDEO
		SDL_Quit(); 
		#endif
		return 1;}}
		#ifdef VIDEO
		if(SDL_PollEvent(&e)) if(e.type == SDL_QUIT){SDL_Quit(); return 2;}
		SDL_Flip(screen);
		#endif
		PIT::tick();
	}
}