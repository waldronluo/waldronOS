
#include "bootpack.h"

struct Queue8 keyinfo;

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char keybuf[32];
	char s[40], mcursor[17*17];	
//	int mx, my;
	int i;
	queue8_init(&keyinfo, 32, keybuf);
	init_gdtidt();
	init_pic();
	io_sti();
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );
//	putfonts8_asc(binfo->vram, binfo->scrnx, 0 , 0, find_palette(0x00ffffff) , s);
	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);
	for ( ;; )
	{
		io_cli();
		if ( queue8_status(&keyinfo) == 0 ) {
			io_stihlt();
		} else {
			if ( queue8_status(&keyinfo) != 0 )	{
				i = queue8_get ( &keyinfo );
				io_sti();
				sprintf(s , "%d" , i );
				boxfill8(binfo->vram , binfo->scrnx, find_palette(0x00008484) , 0,0,100,16);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0 , 0 , find_palette(0x00ffffff) , s );
			}
		}
	}
}
