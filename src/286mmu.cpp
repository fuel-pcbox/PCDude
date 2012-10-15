namespace CPU
{
u16 msw;
struct
{
	u32 base;
	u16 limit;
} gdtr, ldtr;
};

namespace RAM24
{
u8 RAM[0x1000000];
u8 rb(u16 seg, u16 offset)
{
	u32 tmp = (seg & 0xFFF8);
	if(seg & 0x0004)
	{
		u32 tmp1 = (ldtr.base + tmp);
		if(tmp1 > (ldtr.base + ldtr.limit)) return 0;
		u32 addr = (((RAM[tmp1 + 3]<<16) | ((RAM[tmp1 + 5]<<8)|RAM[tmp1 + 4])));
		if(RAM[tmp1 + 2] & 0x08)
		{
			if(RAM[tmp1 + 2] & 0x04)
			{
				addr+=offset;
			}
			else addr-=offset;
		}
		else
		{
			if(!(RAM[tmp1 + 2] & 0x02))
			{
				CPU_IRQ::irq = true;
				CPU_IRQ::irq_number = 0xD;
				return 0;
			}
			addr+=offset;
		}
		return RAM[addr];
	}
	else
	{
		u32 tmp1 = (gdtr.base + tmp);
		if(tmp1 > (gdtr.base + gdtr.limit)) return 0;
		u32 addr = (((RAM[tmp1 + 3]<<16) | ((RAM[tmp1 + 5]<<8)|RAM[tmp1 + 4])));
		if(RAM[tmp1 + 2] & 0x08)
		{
			if(RAM[tmp1 + 2] & 0x04)
			{
				addr+=offset;
			}
			else addr-=offset;
		}
		else
		{
			if(!(RAM[tmp1 + 2] & 0x02))
			{
				CPU_IRQ::irq = true;
				CPU_IRQ::irq_number = 0xD;
				return 0;
			}
			addr+=offset;
		}
		return RAM[addr];
	}
}

void wb(u16 seg, u16 offset, u8 data)
{
	u32 tmp = (seg & 0xFFF8);
	if(seg & 0x0004)
	{
		u32 tmp1 = (ldtr.base + tmp);
		if(tmp1 > (ldtr.base + ldtr.limit)) return 0;
		u32 addr = (((RAM[tmp1 + 3]<<16) | ((RAM[tmp1 + 5]<<8)|RAM[tmp1 + 4])));
		if(RAM[tmp1 + 2] & 0x08)
		{
			if(RAM[tmp1 + 2] & 0x04)
			{
				addr+=offset;
			}
			else addr-=offset;
			if(!(RAM[tmp1 + 2] & 0x02))
			{
				CPU_IRQ::irq = true;
				CPU_IRQ::irq_number = 0xD;
				return;
			}
		}
		else
		{
			addr += offset;
		}
		RAM[addr] = data;
	}
	else
	{
		u32 tmp1 = (gdtr.base + tmp);
		if(tmp1 > (gdtr.base + gdtr.limit)) return 0;
		u32 addr = (((RAM[tmp1 + 3]<<16) | ((RAM[tmp1 + 5]<<8)|RAM[tmp1 + 4])));
		if(RAM[tmp1 + 2] & 0x08)
		{
			if(RAM[tmp1 + 2] & 0x04)
			{
				addr+=offset;
			}
			else addr-=offset;
			if(!(RAM[tmp1 + 2] & 0x02))
			{
				CPU_IRQ::irq = true;
				CPU_IRQ::irq_number = 0xD;
				return;
			}
		}
		else
		{
			addr+=offset;
		}
		RAM[addr] = data;
	}
}
`
};