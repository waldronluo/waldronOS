
#include "bootpack.h"

extern struct Queue8 keyinfo;
extern struct Queue8 mouseinfo;
extern struct MOUSE_DEC mdec;
struct MEMMAN* memman = (struct MEMMAN *) MEMMAN_ADDR;
void HariMain(void)
{
	struct BOOTINFO* binfo = (struct BOOTINFO *)0x0ff0;
	char keybuf[32], mousebuf[128], s[40], mcursor[17*17];	
	unsigned char i;
	int mx,my;
	unsigned int memtotal;


	init_gdtidt();
	init_pic();
	io_sti();
	queue8_init(&keyinfo, 32, keybuf);
	queue8_init(&mouseinfo, 128, mousebuf);
	
	init_palette();
	init_screen( binfo->vram , binfo->scrnx, binfo->scrny );

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);

//	memtotal = memtest ( 0x00400000 , 0xbfffffff ) / (1024);	
//	sprintf(s , "%dkB" , (memtotal) );
//	boxfill8(binfo->vram , binfo->scrnx, find_palette(0x00008484) , 48,48,148,64);
//	putfonts8_asc(binfo->vram, binfo->scrnx, 48 , 48 , find_palette(0x00ffffff) , s );
	init_keyboard();
	/* Init mouse  */	
	enable_mouse();
	init_mouse(&mdec);	
	mx = ( binfo->scrnx - 16 ) / 2;
	my = ( binfo->scrny - 28 - 16 ) / 2;
	init_mouse_curosr8 ( mcursor , find_palette ( 0x008484 ) );
	putblock8_8( binfo ->vram , binfo->scrnx , 16 , 16 , mx , my , mcursor , 16 );
	/* memory info */
	memtotal = memtest ( 0x00400000 , 0xbfffffff );
	memman_init(memman);
	memman_free(memman , 0x00001000 , 0x0009e000 );
	memman_free(memman , 0x00400000 , memtotal - 0x00400000 );	
	sprintf(s , "memory:%dMB   free:%dkB" , (memtotal/1024/1024) , memman_total(memman) / 1024 );
	boxfill8(binfo->vram , binfo->scrnx, find_palette(0x00008484) ,0,48,320,64);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0 , 48 , find_palette(0x00ffffff) , s );
	/* The OS */
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
			
			boxfill8 ( binfo -> vram , binfo->scrnx , find_palette(0x00008484) , mx , my , mx+15 , my+15 );
			mx += mdec.x;
			my += mdec.y;
			if ( mx < 0 ) mx = 0;
			if ( my < 0 ) my = 0;
			if ( mx > binfo->scrnx - 16 ) mx = binfo->scrnx - 16;
			if ( my > binfo->scrny - 16 ) my = binfo->scrny - 16;
			putblock8_8(binfo->vram , binfo->scrnx, 16 , 16 , mx , my , mcursor , 16 );	
			}
		}
	}
}
