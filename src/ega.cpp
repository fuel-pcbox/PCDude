namespace EGA
{
void Register()
{
	Video.gfxCardRender = &tick;
	Video.gfxCardGetDisplaySize = []() -> std::tuple<int,int> {return std::make_tuple(720,350);};	
	tileset = new sf::Texture();
	tileset->loadFromFile("gfx/mda.png");
}

void Unregister()
{
	Video.gfxCardRender = [](sf::RenderWindow & rw, int xoff, int yoff)->void {};
	delete tileset;
}

void putpixel(sf::RenderWindow& win,int x,int y,u8 col)

void tick(sf::RenderWindow& win)
{
	int xmax = crtc.horz_disp_end - 1;
	int ymax = crtc.vert_disp_end;
	if(seq.clock & 1) xmax << 3;
	else xmax *= 9;
	for(int i = 0;i<ymax;i++)
	{
		for(int k = 0;k<((crtc.max_scan & 0xF)+1);k++)
		{
			for(int j = 0;j<xmax;j++)
			{
				u8 col = RAM16::RAM[0xA0000 + ((j+(i*xmax))>>1)];
				col = col & ((((j+(i*xmax)))&1) ? 0x0F : 0xF0);
				int xshift = attr.horz_pel & 7;
				putpixel(win,j-xshift,i,col);
			}
		}
	}
}

};