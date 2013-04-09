
#include "bootpack.h"

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
//	char s[40], mcursor[17*17];	
	int mx, my;
	
	init_gdtidt();
	init_pic();
	io_sti();
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );
//	putfonts8_asc(binfo->vram, binfo->scrnx, 0 , 0, find_palette(0x00ffffff) , s);
	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);
	for ( ;; )
		io_hlt();
}
