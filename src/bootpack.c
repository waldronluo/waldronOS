
#include "bootpack.h"

extern struct Queue8 keyinfo;
extern struct Queue8 mouseinfo;

void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char keybuf[32], mousebuf[128], s[40], mcursor[17*17];	
	unsigned char i;
	unsigned char mouse_dbuf[3], mouse_phase;	

	init_gdtidt();
	init_pic();
	io_sti();
	queue8_init(&keyinfo, 32, keybuf);
	queue8_init(&mouseinfo, 128, mousebuf);
	
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);

	init_keyboard();
	
	enable_mouse();
	mouse_phase = 0;

	for ( ;; )
	{
		io_cli();
		if ( queue8_status(&keyinfo)+queue8_status(&mouseinfo) == 0 ) {
			io_stihlt();
		} 
		else if ( queue8_status(&keyinfo) != 0 ) {
				i = queue8_get ( &keyinfo );
				io_sti();
				sprintf(s , "%02x" , i );
				boxfill8(binfo->vram , binfo->scrnx, find_palette(0x00008484) , 0,0,100,16);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0 , 0 , find_palette(0x00ffffff) , s );
		}
		else if ( queue8_status(&mouseinfo) != 0 ) {
			i = queue8_get(&mouseinfo);
			io_sti();
			switch ( mouse_phase )
			{
				case 0: if ( i == 0xfa ) mouse_phase = 1; break;
				case 1: mouse_dbuf[0] = i ; mouse_phase = 2;break;
				case 2: mouse_dbuf[1] = i ; mouse_phase = 3;break;
				case 3: mouse_dbuf[2] = i ; mouse_phase = 1;
					sprintf( s, "%02X %02X %02X" , mouse_dbuf[0] , mouse_dbuf[1] , mouse_dbuf[2] );
					boxfill8( binfo->vram, binfo->scrnx, find_palette(0x00008484) , 32, 16 , 32+8*8-1 , 31 );
					putfonts8_asc(binfo->vram, binfo->scrnx, 32,16,find_palette(0x00ffffff) , s );
				break;
				default:;
			}
		}
	}
}
