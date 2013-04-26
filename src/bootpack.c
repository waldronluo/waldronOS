
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
	unsigned int count = 0;

	/*sheet.c*/
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse, *sht_win;
	unsigned char *buf_back, buf_mouse[256], *buf_win;
	/* end of sheet.c*/

	init_gdtidt();
	init_pic();
	io_sti();
	queue8_init(&keyinfo, 32, keybuf);
	queue8_init(&mouseinfo, 128, mousebuf);
	

	io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xef);

	init_keyboard();
	/* Init mouse  */	
	enable_mouse();
	init_mouse(&mdec);	
	mx = ( binfo->scrnx - 16 ) / 2;
	my = ( binfo->scrny - 28 - 16 ) / 2;

	/* memory info */
	memtotal = memtest ( 0x00400000 , 0xbfffffff );
	memman_init(memman);
	memman_free(memman , 0x00001000 , 0x0009e000 );
	memman_free(memman , 0x00400000 , memtotal - 0x00400000 );	

	/* sheet.c*/
	init_palette();
	shtctl = shtctl_init ( memman, binfo->vram, binfo->scrnx, binfo->scrny );
	sht_back = sheet_alloc ( shtctl );
	sht_mouse= sheet_alloc ( shtctl );
	sht_win  = sheet_alloc ( shtctl );
	buf_back = (unsigned char*) memman_alloc_4k ( memman, binfo->scrnx * binfo->scrny );
	buf_win  = (unsigned char*) memman_alloc_4k ( memman, 160 * 52 );
	sheet_setbuf ( sht_back, buf_back, binfo->scrnx, binfo->scrny, -1 );
	sheet_setbuf ( sht_mouse,buf_mouse,16, 16, 99 );
	sheet_setbuf ( sht_win, buf_win, 160, 52 , -1 );
	init_screen ( buf_back , binfo->scrnx, binfo->scrny );
	init_mouse_curosr8( buf_mouse, 99 );
	make_window8 ( buf_win, 160, 68, "count" );
	sheet_slide (  sht_mouse, mx, my );
	sheet_slide ( sht_win, 80, 72 );
	sheet_updown (  sht_back, 0 );
	sheet_updown (  sht_win, 1 );
	sheet_updown (  sht_mouse, 2 );
	sheet_refresh (  sht_back, 0, 0, binfo->scrnx, 48);

	/* The OS */
	for ( ;; )
	{	
		count ++;
		sprintf(s, "%010d", count );
		boxfill8(buf_win, 160, find_palette(0x00c6c6c6), 40, 28, 119, 43 );
		putfonts8_asc(buf_win, 160, 40, 28, find_palette(0), s );
		sheet_refresh(sht_win, 40,28,120,44);
		io_cli();
		if ( queue8_status(&keyinfo)+queue8_status(&mouseinfo) == 0 ) {
			io_stihlt();
		} 
		else if ( queue8_status(&keyinfo) != 0 ) {
			i = queue8_get ( &keyinfo );
			io_sti();
			sprintf(s , "%02x %d %d" , i, memman->free[0].addr, memman->free[1].addr );
			boxfill8(buf_back , binfo->scrnx, find_palette(0x00008484) , 0,16,15,31);
			putfonts8_asc(buf_back, binfo->scrnx, 0 , 16 , find_palette(0x00ffffff) , s );
			sheet_refresh( sht_back, 0, 16, 16, 32);
		}
		else if ( queue8_status(&mouseinfo) != 0 ) {
			i = queue8_get(&mouseinfo);
			io_sti();
			if ( mouse_decode (&mdec,i) == 1 ){
				sprintf ( s , "[lcr %4d %4d]", mdec.x , mdec.y );
				if ( (mdec.btn & 0x01 ) != 0 ) s[1] = 'L';
				if ( (mdec.btn & 0x02 ) != 0 ) s[3] = 'R';
				if ( (mdec.btn & 0x04 ) != 0 ) s[2] = 'C';
				boxfill8 ( buf_back , binfo->scrnx, find_palette( 0x00008484 ) , 32, 16 , 32 + 15*8 -1 , 31 );
				putfonts8_asc( buf_back , binfo->scrnx, 32,16,find_palette(0x00ffffff) , s );
				sheet_refresh ( sht_back, 32, 16, 32 + 15 * 8, 32 );	
				
				mx += mdec.x;
				my += mdec.y;
				if ( mx < 0 ) mx = 0;
				if ( my < 0 ) my = 0;
				if ( mx > binfo->scrnx - 1 ) mx = binfo->scrnx - 1;
				if ( my > binfo->scrny - 1 ) my = binfo->scrny - 1;
				sprintf ( s, "(%3d, %3d)", mx,my );
				boxfill8 ( buf_back, binfo->scrnx, find_palette(0x00008484), 0, 0, 79, 15 );
				putfonts8_asc( buf_back, binfo->scrnx, 0 , 0 , find_palette(0x00ffffff), s );
				sheet_refresh( sht_back, 0, 0, 80, 16 );
				sheet_slide (  sht_mouse, mx, my );
			
			}
		}
	}
}
