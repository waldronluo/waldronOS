
#include "bootpack.h"

extern struct Queue8 keyinfo;
extern struct Queue8 mouseinfo;
extern struct MOUSE_DEC mdec;
void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char keybuf[32], mousebuf[128], s[40], mcursor[17*17];	
	unsigned char i;

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
	init_mouse(&mdec);	

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
			if ( mouse_decode (&mdec,i) == 1 ){
				sprintf ( s , "[lcr %4d %4d]", mdec.x , mdec.y );
				if ( (mdec.btn & 0x01 ) != 0 ) s[1] = 'L';
				if ( (mdec.btn & 0x02 ) != 0 ) s[3] = 'R';
				if ( (mdec.btn & 0x04 ) != 0 ) s[2] = 'C';
				boxfill8 ( binfo->vram , binfo->scrnx, find_palette( 0x00008484 ) , 32, 16 , 32 + 15*8 -1 , 31 );
				putfonts8_asc( binfo -> vram , binfo->scrnx, 32,16,find_palette(0x00ffffff) , s );
			}
		}
	}
}
