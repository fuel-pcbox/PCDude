
#ifndef API_H_INC
#define API_H_INC

#include "includes.h"

namespace EMULATOR
{
extern Jzon::Object settings;
extern std::string emu_path;
extern std::string emu_file;
}

namespace CPU_IRQ
{
extern bool irq;
extern u8 irq_number;
}

namespace MDA
{
constexpr std::size_t ROM_SIZE = 0x2000U;
extern u8 ROM[ROM_SIZE];

extern u8 textcols;
extern u8 textmode;
extern u8 color;
extern u8 video_enable;
extern u8 graphicsmode;
extern u8 attr_blink_mode;

extern u8 crtc_index;
extern  u8 horz_total;
extern  u8 horz_disp;
extern u8 horz_sync_pos;
extern u8 horz_sync_width;
extern u8 vert_total;
extern u8 vert_total_adjust;
extern u8 vert_disp;
extern  u8 maximum_scanline;
};

namespace HGC
{
extern int hercules;
};

namespace CGA
{
extern u8 textcols;
extern u8 textmode;
extern u8 color;
extern u8 video_enable;
extern u8 graphicsmode;
extern u8 attr_blink_mode;

extern u8 crtc_index;
extern  u8 horz_total;
extern  u8 horz_disp;
extern u8 horz_sync_pos;
extern u8 horz_sync_width;
extern u8 vert_total;
extern u8 vert_total_adjust;
extern u8 vert_disp;
extern  u8 maximum_scanline;
};

namespace LPT0
{
extern u8 data;
};

namespace LPT1
{
extern u8 data;
};

namespace LPT2
{
extern u8 data;
};

namespace DMA_XT
{
extern u16 start_addr_chan0;
extern u16 start_addr_chan1;
extern u16 start_addr_chan2;
extern u16 start_addr_chan3;
extern u8 page_chan3;
extern bool enabled;
extern bool lohi_chan0;
extern bool lohi_chan1;
extern bool lohi_chan2;
extern bool lohi_chan3;
extern bool lohi;
extern u8 channel;
extern u8 trans_mode;
extern bool autotrans;
extern bool trans_dir;
extern u8 trans_method;
extern bool drq_chan0_masked;
extern bool drq_chan1_masked;
extern bool drq_chan2_masked;
extern bool drq_chan3_masked;
}

namespace PIT
{
extern u8 BCD;
extern u8 opmode;
extern u8 accmode;
extern u8 channel;
extern u16 counter;
extern u16 reload;
extern bool reload_set;
extern bool reload_edge;
extern bool lohi;
extern bool latch;
extern bool latch_lohi;

void dumpPITstate();

void tick();
};

namespace PIC
{
extern bool single;
extern bool icw4_req;
extern u8 vector_offset;
extern bool initing;
extern bool icw2;
extern u8 IRQmask;
}

namespace PPI
{
extern u8 amode;
extern u8 bmode;
extern u8 porta;
extern u8 portb;
extern u8 portc;
extern  u8 porta_in;
extern  u8 portb_in;
extern u8 loportc_in;
extern u8 hiportc_in;
extern u8 iomode;
};

namespace IO_XT
{
extern u8 nmi_enabled;
u8 rb(u16 addr);
void wb(u16 addr, u8 value);
};

namespace RAM16
{
constexpr std::size_t RAM_SIZE = 0x100000U;
extern u8 RAM[RAM_SIZE];
};

namespace CPU
{
enum e_processortype
{
	i8086=0,
	i186,
	i286,
	intel386
};
extern int type;
extern bool halted;
union anonunion_t_1
{
	struct
	{
		u8 lo,hi;
	} parts;
	u16 whole;
} ;
extern anonunion_t_1 a,b,c,d;

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

extern u16 sp,bp,di,si;
extern u16 cs,ds,es,ss;
extern u16 ip;

extern u16 flags;

int InitCPU(int t);

void dumpregs();
void dumpivt();

void inc_addr();

int tick();
};


#include "videocards.h"

#define addr ((cs<<4)+ip)
#define PCLOG1(FSTR) printf( FSTR "\n"  )
#define PCLOG(FSTR,...) printf( FSTR "\n" , __VA_ARGS__ )

#endif

